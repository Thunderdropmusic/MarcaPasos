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

};
 //Valores guardados al haber una variacion con los detectados

//declaracion de funciones
void MidiInterface::syncWithActiveSequence() {
  s = presetsUI.getActiveSequence();
}

bool MidiInterface::timeDebounce(unsigned long boton, int time){
  return (tiempoActualMillis - boton >= time);
}

void MidiInterface::checkMutes() {
  syncWithActiveSequence();

  bool octPlus = !digitalRead(pinOctPlus);
  bool octRest = !digitalRead(pinOctRest);

  seqButton[0] = !digitalRead(seq1PinButton);
  seqButton[1] = !digitalRead(seq2PinButton);
  seqButton[2] = !digitalRead(seq3PinButton);
  seqButton[3] = !digitalRead(seq4PinButton);
  seqButton[4] = !digitalRead(seq5PinButton);

  
  for(int i = 0; i < 5; i++){
    if(p->nSequence[i].armed){
      digitalWrite(seqPinLed[i], HIGH);
    }
    else if(!p->nSequence[i].armed){
      digitalWrite(seqPinLed[i], LOW);
    }
  }

  if (octPlus && !octPlusPulsed) {
    octPlusPulsed = true;
  }
  if (octRest && !octRestPulsed) {
    octRestPulsed = true;
  }
  for(int i = 0; i < 5; i++){
    if(seqButton[i]){
      seqArmPulsed[i] = true;
      if(seqArmPulsed[i] && timeDebounce(ultimoTiempoBotonArm, 500)){
        if(!p->nSequence[i].armed){
          if(MidiProgramming::modeMidiClock == 0){
            if(midiProg[i].play){MidiProgramming::flagArmed[i+seqLayer] = true;}
            else{p->nSequence[i].armed = true;}
          }
          else if(MidiProgramming::modeMidiClock == 1){MidiProgramming::tipoMsgMidi = 0xFA;}
          ultimoTiempoBotonArm = tiempoActualMillis;
        }
        else if(p->nSequence[i].armed){
          if(MidiProgramming::modeMidiClock == 0){p->nSequence[i+seqLayer].armed = false;}
          else if(MidiProgramming::modeMidiClock == 1){MidiProgramming::tipoMsgMidi = 0xFC;}
          digitalWrite(seqPinLed[i], LOW);
          ultimoTiempoBotonArm = tiempoActualMillis;
        }
      }
      break;
    }
    else{
      seqArmPulsed[i] = false;
    }
  }

  for (int i = 0; i < NUM_MUTES; i++) {
    bool mute = !digitalRead(mutesPin[i]);
    if (mute) {
      if (mutePulsado[i] == false) {
        mutePulsado[i] = true;
        muteActivado = true;
        TiempoPulsadoBotonMute[i] = tiempoActualMillis;
        indexMovedMute = i;
      }
      
      if (tiempoActualMillis - TiempoPulsadoBotonMute[i] > 100 && timeDebounce(ultimoTiempoBotonMute[i], 500)) {
        ultimoTiempoBotonMute[i] = tiempoActualMillis;
      }
      if (octPlusPulsed && timeDebounce(ultimoTiempoBotonOct, 500) && s->steps[i].octave < 9) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        s->steps[i].octave++;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
        pulsedIndividualOctave = true;
      } else if (octRestPulsed && timeDebounce(ultimoTiempoBotonOct, 500) && s->steps[i].octave > 0) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        s->steps[i].octave--;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
        pulsedIndividualOctave = true;
      }
    } 
    else {
      if (mutePulsado[i] == true && !pulsedIndividualOctave && !octPlusPulsed && !octRestPulsed && !poteMovidoMutes) {
        if(s->seqMode == 0){s->steps[i].mutes = !s->steps[i].mutes;}
        else if(s->seqMode == 1){s->steps[i].ccMutes = !s->steps[i].ccMutes;}
        drawUI.updateLCD = true;
        mutePulsado[i] = false;
        muteActivado = false;
      } 
      else if (mutePulsado[i] == true && pulsedIndividualOctave) {
        mutePulsado[i] = false;
        muteActivado = false;
        pulsedIndividualOctave = false;
      }
      else if(mutePulsado[i] == true && poteMovidoMutes){
        mutePulsado[i] = false;
        muteActivado = false;
        poteMovidoMutes = false;

      }
    }
  }
  if (!octPlus && octPlusPulsed && timeDebounce(ultimoTiempoBotonOct, 250)) { octPlusPulsed = false; }
  if (!octRest && octRestPulsed && timeDebounce(ultimoTiempoBotonOct, 250)) { octRestPulsed = false; }
  if (!muteActivado && octPlusPulsed && timeDebounce(ultimoTiempoBotonOct, 500)) {
    ultimoTiempoBotonOct = tiempoActualMillis;
    timeShowOctValue = tiempoActualMillis;
    for (int i = 0; i < N_MAX_STEPS; i++) {
      if(s->steps[i].octave < 9){s->steps[i].octave++;}
    }
    menusUI.menuActual = 5;
    drawUI.updateLCD = true;
  } 
  else if (!muteActivado && octRestPulsed && timeDebounce(ultimoTiempoBotonOct, 500)) {
    ultimoTiempoBotonOct = tiempoActualMillis;
    timeShowOctValue = tiempoActualMillis;
    for (int i = 0; i < N_MAX_STEPS; i++) {
      if(s->steps[i].octave > 0){s->steps[i].octave--;}
    }
    menusUI.menuActual = 5;
    drawUI.updateLCD = true;
  }
}



void MidiInterface::checkPotes() {
  for (int i = 0; i < NUM_POTES; i++) {
    int val = analogRead(potesPin[i]);
    if(abs(val - potSaveValue[i]) > 20){
      potSaveValue[i] = val;
      if(muteActivado){
        if(s->seqMode == 0){
          s->steps[i].velocity = constrain(map(potSaveValue[i], 1018, 30, 0, 127), 0, 127);
          movedPoteValue = s->steps[i].velocity;
          movedPoteNumber = i;
          timeShowPotValue = tiempoActualMillis;
          menusUI.menuActual = 3;
          drawUI.updateLCD = true;
          poteMovidoMutes = true;
        }
        else if(s->seqMode == 1){
          int potCentrado = constrain(map(potSaveValue[i], 900, 80, 0, 13), 0, 13);
          s->steps[i].ccSmoothCurve = potCentrado;
          movedPoteValue = s->steps[i].ccSmoothCurve;
          movedPoteNumber = i;
          timeShowPotValue = tiempoActualMillis;
          //menusUI.menuActual = 3;
          drawUI.updateLCD = true;
          poteMovidoMutes = true;
        }
      }
      else{
        if(s->seqMode == 0){
          int notaEscala = constrain(map(potSaveValue[i], 1018, 30, 0, SEQ.nNotasEscalas[s->escalaSeleccionada] - 1), 0, SEQ.nNotasEscalas[s->escalaSeleccionada] - 1);
          s->steps[i].note = SEQ.escalasNotas[s->escalaSeleccionada][notaEscala];
          movedPoteValue = s->steps[i].note;
          movedPoteNumber = i;
          timeShowPotValue = tiempoActualMillis;
          menusUI.menuActual = 3;
          drawUI.updateLCD = true;
          poteMovidoMutes = true;
        }
        else if(s->seqMode == 1){
          int valorCC = constrain(map(potSaveValue[i], 1018, 30, 0, 127), 0, 127);
          s->steps[i].ccValue = valorCC;
          movedPoteValue = s->steps[i].ccValue;
          movedPoteNumber = i;
          timeShowPotValue = tiempoActualMillis;
          //menusUI.menuActual = 3;
          drawUI.updateLCD = true;
        }
      }
    }
  }
}