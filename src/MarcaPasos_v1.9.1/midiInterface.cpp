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
  for(int i = 0; i < 5; i++){
    seqButton[i] = !digitalRead(seqPinsButton[i]);
  }

  bool playButton = !digitalRead(playPinButton);
  bool stopButton = !digitalRead(stopPinButton);
  
  if(playButton && timeDebounce(ultimoTiempoBotonDevicePlay, 500)){
    if(deviceStop){
      deviceStop = false;
      devicePlay = true;
      drawUI.nAnteriorScreen = 200;
    }
    else{
      devicePlay = !devicePlay;
    }
    ultimoTiempoBotonDevicePlay = tiempoActualMillis;
    drawUI.updateLCD = true;
  }

  if(stopButton && timeDebounce(ultimoTiempoBotonDevicePlay, 500)){
    deviceStop = true;
    ultimoTiempoBotonDevicePlay = tiempoActualMillis;
    drawUI.updateLCD = true;
  }
  if(!midiUI.devicePlay && !midiUI.deviceStop){
    if(timeDebounce(tiempoLedPausa, 500)){
      ledState = !ledState;
      tiempoLedPausa = tiempoActualMillis;
    }
  }

  for(int i = 0; i < 5; i++){
    if(midiUI.deviceStop || !p->nSequence[i].armed){
      digitalWrite(seqPinLed[i], LOW);
    }
    else{
      digitalWrite(seqPinLed[i], midiUI.devicePlay ? HIGH : ledState);
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
      
      if (timeDebounce(TiempoPulsadoBotonMute[i], 100) && timeDebounce(ultimoTiempoBotonMute[i], 500)) {
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
        if(menusUI.putNameFlag){charNumber[i] = 32;}
        if(s->seqMode == 0 && !menusUI.putNameFlag){s->steps[i].mutes = !s->steps[i].mutes;}
        else if(s->seqMode == 1 && !menusUI.putNameFlag){s->steps[i].ccMutes = !s->steps[i].ccMutes;}
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
          s->steps[i].velocity = constrain(map(potSaveValue[i], MAX_POTE_VALUE, MIN_POTE_VALUE, 0, 127), 0, 127);
          aplicarCambiosPotes(s->steps[i].velocity, i);
          poteMovidoMutes = true;
        }
        else if(s->seqMode == 1){
          int potCentrado = constrain(map(potSaveValue[i], 900, 80, 0, 13), 0, 13);
          s->steps[i].ccSmoothCurve = potCentrado;
          aplicarCambiosPotes(s->steps[i].ccSmoothCurve, i);
          poteMovidoMutes = true;
        }
      }
      else{
        if(menusUI.putNameFlag){
          byte index = constrain(map(potSaveValue[i], MAX_POTE_VALUE, MIN_POTE_VALUE, 0, 35), 0, 35);
          if(index <= 9){
            charNumber[i] = index + 48;
          } 
          else{
            charNumber[i] = index + 55;
          }
        }
        else if(s->seqMode == 0 && !menusUI.putNameFlag){
          int notaEscala = constrain(map(potSaveValue[i], MAX_POTE_VALUE, MIN_POTE_VALUE, 0, SEQ.nNotasEscalas[s->escalaSeleccionada] - 1), 0, SEQ.nNotasEscalas[s->escalaSeleccionada] - 1);
          s->steps[i].note = SEQ.escalasNotas[s->escalaSeleccionada][notaEscala] + s->noteTone;
          aplicarCambiosPotes(s->steps[i].note, i);
          poteMovidoMutes = true;
        }
        else if(s->seqMode == 1 && !menusUI.putNameFlag){
          int valorCC = constrain(map(potSaveValue[i], MAX_POTE_VALUE, MIN_POTE_VALUE, 0, 127), 0, 127);
          s->steps[i].ccValue = valorCC;
          aplicarCambiosPotes(s->steps[i].ccValue, i);
        }
      }
    }
  }
}

void MidiInterface::aplicarCambiosPotes(int sendValue, int nPote){
  movedPoteValue = sendValue;
  movedPoteNumber = nPote;
  timeShowPotValue = tiempoActualMillis;
  if(s->seqMode == 0){menusUI.menuActual = 3;}
  drawUI.updateLCD = true;
}