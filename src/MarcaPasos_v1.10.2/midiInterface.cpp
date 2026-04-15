#include <Arduino.h>
#include <MIDI.h>
#include "MidiInterface.h"
#include "menus_buttons.h"
#include "MidiProgramming.h"
#include "midiPresets.h"
#include "draw_menus.h"
#include "config.h"

#define SEQ midiProg[presetsUI.indexSequence]

// Números Mágicos
MidiInterface midiUI;



MidiInterface::MidiInterface(){
  // Inicialización si fuera necesaria en el futuro
};

// ==============================================================================
//                                FUNCIONES UTILES 
// ==============================================================================


void MidiInterface::syncWithActiveSequence() {
  s = presetsUI.getActiveSequence();
  stp = menusUI.editExtension ? s->ext_steps : s->steps;
}

bool MidiInterface::timeDebounce(unsigned long boton, int time){
  return (tiempoActualMillis - boton >= time);
}

// ==============================================================================
//                      LECTURA Y DETECCIÓN DE BOTONES 
// ==============================================================================


void MidiInterface::checkMidiButtons() {
  syncWithActiveSequence();

  // --- LECTURA FISICA ---
  bool octPlus = !digitalRead(pinOctPlus);
  bool octRest = !digitalRead(pinOctRest);

  bool playButton = !digitalRead(playPinButton);
  bool stopButton = !digitalRead(stopPinButton);

  for(int i = 0; i < N_MAX_SEQS; i++){
    seqButton[i] = !digitalRead(seqPinsButton[i]);
  }


  // CONTROL: --- PLAY/PAUSE/STOP ---
  if(playButton && timeDebounce(ultimoTiempoBotonDevicePlay, 500)){
    if(deviceStop){
      deviceStop = false;
      deviceFlagPlay = true;
      drawUI.nAnteriorScreen = 200;
    }
    else{ devicePlay = !devicePlay; } // pausa < -- > play

    ultimoTiempoBotonDevicePlay = tiempoActualMillis;
    drawUI.updateLCD = true;
  }

  if(stopButton && timeDebounce(ultimoTiempoBotonDevicePlay, 500)){
    deviceStop = true; // Stop
    devicePlay = false;
    ultimoTiempoBotonDevicePlay = tiempoActualMillis;
    drawUI.updateLCD = true;
  }

  // --- CONTROL: PARPADEO DE LOS LEDS ---
  if(!midiUI.devicePlay && !midiUI.deviceStop){
    if(timeDebounce(tiempoLedPausa, 500)){
      ledState = !ledState;
      tiempoLedPausa = tiempoActualMillis;
    }
  }

  // Actualización del parpadeo
  for(int i = 0; i < N_MAX_SEQS; i++){
    if(midiUI.deviceStop || !p->nSequence[i].armed){
      digitalWrite(seqPinLed[i], LOW);
    }
    else{
      if(menusUI.editExtension){
        if(!p->nSequence[i].extensionArmed){digitalWrite(seqPinLed[i], LOW);}
        else{digitalWrite(seqPinLed[i], p->nSequence[i].extensionArmed ? HIGH : ledState);}  
      }
      else{digitalWrite(seqPinLed[i], midiUI.devicePlay ? HIGH : ledState);}
    }
  }

  // --- CONTROL: ARMADO DE LAS SECUENCIAS ---
  for(int i = 0; i < N_MAX_SEQS; i++){
    if(seqButton[i]){
      seqArmPulsed[i] = true;
      if(seqArmPulsed[i] && timeDebounce(ultimoTiempoBotonArm, 500)){
        if(menusUI.editExtension){
          p->nSequence[i].extensionArmed = !p->nSequence[i].extensionArmed;
        }
        else{
          if(!p->nSequence[i].armed){
            if(MidiProgramming::modeMidiClock == 0){
              if(midiProg[i].play){MidiProgramming::flagArmed[i+seqLayer] = true;}
              else{p->nSequence[i].armed = true;}
            }
            else if(MidiProgramming::modeMidiClock == 1){MidiProgramming::tipoMsgMidi = 0xFA;}
          }
          else if(p->nSequence[i].armed){
            if(MidiProgramming::modeMidiClock == 0){p->nSequence[i+seqLayer].armed = false;}
            else if(MidiProgramming::modeMidiClock == 1){MidiProgramming::tipoMsgMidi = 0xFC;}
            digitalWrite(seqPinLed[i], LOW);
          }
        }
        ultimoTiempoBotonArm = tiempoActualMillis;
      }
      break;
    }
    else{
      seqArmPulsed[i] = false;
    }
  }
  // Flag de detección de botones de octava
  if (octPlus && !octPlusPulsed) { octPlusPulsed = true; }
  if (octRest && !octRestPulsed) {  octRestPulsed = true; }

// ==============================================================================
//                          CONTROL DE LOS MUTES 
// ==============================================================================

  for (int i = 0; i < NUM_MUTES; i++) {
    bool mute = !digitalRead(mutesPin[i]);
    // ----------------------------- MUTE ACTIVADO -----------------------------
    if (mute) {
      // Deteccion del mute y guardado de su estado
      if (mutePulsado[i] == false) {
        mutePulsado[i] = true;
        muteActivado = true;
        TiempoPulsadoBotonMute[i] = tiempoActualMillis;
        indexMovedMute = i;
        for(int p = 0; p < NUM_POTES; p++){
          poteBlocked[p] = true;
        }
      }
      if (timeDebounce(TiempoPulsadoBotonMute[i], 100) && timeDebounce(ultimoTiempoBotonMute[i], 500)) { // Guardado del tiempo de pulsado
        ultimoTiempoBotonMute[i] = tiempoActualMillis;
      }

      // --- SUBIR OCTAVA + MUTE ---
      if (octPlusPulsed && timeDebounce(ultimoTiempoBotonOct, 500) && stp[i].octave < 9) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        stp[i].octave++;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
        pulsedIndividualOctave = true; // octava de la nota que hemos seleccionado con el mute
      }

      // --- BAJAR OCTAVA + MUTE ---
      else if (octRestPulsed && timeDebounce(ultimoTiempoBotonOct, 500) && stp[i].octave > 0) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        stp[i].octave--;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
        pulsedIndividualOctave = true;
      }
    }

    // ----------------------------- MUTE LIBERADO -----------------------------
    else {
      // --- SE ACABA DE LEVANTAR EL BOTÓN DEL MUTE ---
      if (mutePulsado[i] == true && !pulsedIndividualOctave && !octPlusPulsed && !octRestPulsed && !poteMovidoMutes) {
        if(menusUI.putNameFlag){charNumber[i] = 32;} // Si estamos en modo edicion de nombre, escribe un espacio, Sino cambia el valor del mute.
        if(s->seqMode == 0 && !menusUI.putNameFlag){stp[i].mutes = !stp[i].mutes;} 
        else if(s->seqMode == 1 && !menusUI.putNameFlag){stp[i].ccMutes = !stp[i].ccMutes;}

        drawUI.updateLCD = true;
        mutePulsado[i] = false;
        muteActivado = false;
      }
      // reseteo de los flags de los botones activados de octavas individuales
      else if (mutePulsado[i] == true && pulsedIndividualOctave) {
        mutePulsado[i] = false;
        muteActivado = false;
        pulsedIndividualOctave = false;
      }
      // reseteo de los flags de los mutes y de los potenciometros
      else if(mutePulsado[i] == true && poteMovidoMutes){
        mutePulsado[i] = false;
        poteBlocked[i] = true;
        muteActivado = false;
        poteMovidoMutes = false;
      }
    }
  }

  // // ----------------------------- OCTAVAS GLOBALES -----------------------------
  if (!octPlus && octPlusPulsed && timeDebounce(ultimoTiempoBotonOct, 250)) { octPlusPulsed = false; } // Reseteo de los flags de las octavas
  if (!octRest && octRestPulsed && timeDebounce(ultimoTiempoBotonOct, 250)) { octRestPulsed = false; }

  // Subimos la octava a TODA la secuencia
  if (!muteActivado && octPlusPulsed && timeDebounce(ultimoTiempoBotonOct, 500)) {
    ultimoTiempoBotonOct = tiempoActualMillis;
    for (int i = 0; i < N_MAX_STEPS; i++) {
      if(p->nSequence[presetsUI.indexSequence].steps[i].octave < 9){p->nSequence[presetsUI.indexSequence].steps[i].octave++;}
      if(s->ext_steps[i].octave < 9){s->ext_steps[i].octave++;}
    }
    menusUI.menuActual = 5;
    drawUI.updateLCD = true;
  } 
  
  // Bajamos la octava a TODA la secuencia
  else if (!muteActivado && octRestPulsed && timeDebounce(ultimoTiempoBotonOct, 500)) {
    ultimoTiempoBotonOct = tiempoActualMillis;
    for (int i = 0; i < N_MAX_STEPS; i++) {
      if(p->nSequence[presetsUI.indexSequence].steps[i].octave > 0){p->nSequence[presetsUI.indexSequence].steps[i].octave--;}
      if(s->ext_steps[i].octave > 0){s->ext_steps[i].octave--;}
    }
    menusUI.menuActual = 5;
    drawUI.updateLCD = true;
  }
}

// ==============================================================================
//                          DETECCIÓN DE LOS POTENCIÓMETROS 
// ==============================================================================

void MidiInterface::checkPotes() {
  // --- LECTURA DE LOS POTES ---
  for (int i = 0; i < NUM_POTES; i++) {
    int val = analogRead(potesPin[i]);

    if(abs(val - potSaveValue[i]) <= 20) continue; // Hasta que no lleguemos al valor guardado, no cambiará nada para evitar saltos
    // ---------------- MUTE PRESIONADO (Edición Secundaria) ----------------
    if(muteActivado){
      // --- VELOCITY ---
      if(s->seqMode == 0){
        int velMapping = constrain(map(val, MAX_POTE_VALUE, MIN_POTE_VALUE, 0, 127), 0, 127);
        if(poteBlocked[i]){ // si el valor del pote y el guardado coinciden, el pote puede empezar a guardar datos
          if (abs(velMapping - stp[i].velocity) <= 5) poteBlocked[i] = false;
          else continue;
        }
        stp[i].velocity = velMapping;
        aplicarCambiosPotes(stp[i].velocity, i);
        poteMovidoMutes = true;
        potSaveValue[i] = val;
      }
      // --- CANTIDAD DE SMOOTH ---
      else if(s->seqMode == 1){
        int potCentrado = constrain(map(val, 900, 80, 0, 12), 0, 12);
        if(poteBlocked[i]){
          if (abs(potCentrado - stp[i].ccSmoothCurve) == 0) poteBlocked[i] = false;
          else continue;
        }
        stp[i].ccSmoothCurve = potCentrado;
        aplicarCambiosPotes(stp[i].ccSmoothCurve, i);
        poteMovidoMutes = true;
        potSaveValue[i] = val;
      }
    }

    // ---------------- MUTE NO PRESIONADO (Edición Principal) ----------------
    else{
      // --- ESCRITURA DEL NOMBRE ---
      if(menusUI.putNameFlag){
        byte index = constrain(map(val, MAX_POTE_VALUE, MIN_POTE_VALUE, 0, 35), 0, 35);
        if(index <= 9){ charNumber[i] = index + 48; } 
        else{ charNumber[i] = index + 55; }
        potSaveValue[i] = val;
      }

      // --- NOTAS ---
      else if(s->seqMode == 0 && !menusUI.putNameFlag){
        // Mapeamos según la cantidad de notas que tenga nuestra escala.
        int notaEscala = constrain(map(val, MAX_POTE_VALUE, MIN_POTE_VALUE, 0, SEQ.nNotasEscalas[s->escalaSeleccionada] - 1), 0, SEQ.nNotasEscalas[s->escalaSeleccionada] - 1);
        // Y = escala seleccionada;  X = Numero de la nota de la escala
        int noteMapping = SEQ.escalasNotas[s->escalaSeleccionada][notaEscala] + s->noteTone;

        if(poteBlocked[i]){ 
          if (abs(noteMapping - stp[i].note) == 0) poteBlocked[i] = false; 
          else continue;
        }
        stp[i].note = noteMapping;
        aplicarCambiosPotes(stp[i].note, i);
        poteMovidoMutes = true;
        potSaveValue[i] = val;
      }

      // --- VALOR CC ---
      else if(s->seqMode == 1 && !menusUI.putNameFlag){
        int valorCC = constrain(map(val, MAX_POTE_VALUE, MIN_POTE_VALUE, 0, 127), 0, 127);
        if(poteBlocked[i]){
          if (abs(valorCC - stp[i].ccValue) < 5) poteBlocked[i] = false;
          else continue;
        }
        stp[i].ccValue = valorCC;
        aplicarCambiosPotes(stp[i].ccValue, i);
        poteMovidoMutes = true;
        potSaveValue[i] = val;
      }
    }
  }
}

// Enviamos los valores a la funcion draw_menus para su dibujado
void MidiInterface::aplicarCambiosPotes(int sendValue, int nPote){
  movedPoteValue = sendValue;
  movedPoteNumber = nPote;
  timeShowPotValue = tiempoActualMillis;
  if(s->seqMode == 0){menusUI.menuActual = 3;}
  drawUI.updateLCD = true;
}