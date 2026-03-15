#include <Arduino.h>
#include <MIDI.h>
#include "MidiInterface.h"
#include "menus_buttons.h"
#include "MidiProgramming.h"
#include "midiPresets.h"
#include "globalVars.h"
#include "draw_menus.h"

#define SEQ midiProg[presetsUI.indexNotesSequence]

MidiInterface midiUI;

MidiPresets presetsUI;
Pattern marcaPasos;

MidiInterface::MidiInterface(){

};
 //Valores guardados al haber una variacion con los detectados

//declaracion de funciones
void MidiInterface::syncWithActiveSequence() {
  s = presetsUI.getActiveSequence();
}

void MidiInterface::checkMutes() {
  syncWithActiveSequence();

  bool octPlus = digitalRead(pinOctPlus);
  bool octRest = digitalRead(pinOctRest);

  if (octPlus && !octPlusPulsed) {
    octPlusPulsed = true;
  }
  if (octRest && !octRestPulsed) {
    octRestPulsed = true;
  }

  for (int i = 0; i < NUM_MUTES; i++) {
    bool mute = digitalRead(mutesPin[i]);
    if (mute) {
      if (mutePulsado[i] == false) {
        mutePulsado[i] = true;
        muteActivado = true;
        TiempoPulsadoBotonMute[i] = tiempoActualMillis;
        indexMovedMute = i;
      }
      
      if(mutePulsado[0] && mutePulsado[1] && !comandoEnviado && tiempoActualMillis - ultimoTiempoBotonMute[0] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[1] > 250){
        if(MidiProgramming::modeMidiClock == 0){MidiProgramming::flagArmed[presetsUI.indexNotesSequence] = true;}
        else if(MidiProgramming::modeMidiClock == 1){MidiProgramming::tipoMsgMidi = 0xFA;}
        ultimoTiempoBotonMute[0] = tiempoActualMillis;
        ultimoTiempoBotonMute[1] = tiempoActualMillis;
        comandoEnviado = true;
      }
      else if(mutePulsado[2] && mutePulsado [3] && !comandoEnviado && tiempoActualMillis - ultimoTiempoBotonMute[2] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[3] > 250){
        if(MidiProgramming::modeMidiClock == 0){globalArmed[presetsUI.indexNotesSequence] = false;}
        else if(MidiProgramming::modeMidiClock == 1){MidiProgramming::tipoMsgMidi = 0xFC;}
        ultimoTiempoBotonMute[2] = tiempoActualMillis;
        ultimoTiempoBotonMute[3] = tiempoActualMillis;
        comandoEnviado = true;
      }
      
      if (tiempoActualMillis - TiempoPulsadoBotonMute[i] > 100 && tiempoActualMillis - ultimoTiempoBotonMute[i] > 250) {
        ultimoTiempoBotonMute[i] = tiempoActualMillis;
      }
      if (octPlusPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500 && globalOctave[presetsUI.indexNotesSequence][i] < 9) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        s->steps[i].octava++;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
        pulsedIndividualOctave = true;
      } else if (octRestPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500 && globalOctave[presetsUI.indexNotesSequence][i] > 0) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        globalOctave[presetsUI.indexNotesSequence][i]--;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
        pulsedIndividualOctave = true;
      }
    } 
    else {
      if (!mutePulsado[0] && !mutePulsado[1] && !mutePulsado[2] && !mutePulsado[3]) {
        comandoEnviado = false; 
      }
      if (mutePulsado[i] == true && !pulsedIndividualOctave && !octPlusPulsed && !octRestPulsed && !poteMovidoMutes) {
        globalMutes[presetsUI.indexNotesSequence][i] = !globalMutes[presetsUI.indexNotesSequence][i];
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
  if (!octPlus && octPlusPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 250) { octPlusPulsed = false; }
  if (!octRest && octRestPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 250) { octRestPulsed = false; }
  if (!muteActivado && octPlusPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500) {
    ultimoTiempoBotonOct = tiempoActualMillis;
    timeShowOctValue = tiempoActualMillis;
    for (int i = 0; i < NUM_MUTES; i++) {
      if(globalOctave[presetsUI.indexNotesSequence][i] < 9){globalOctave[presetsUI.indexNotesSequence][i]++;}
    }
    menusUI.menuActual = 5;
    drawUI.updateLCD = true;
  } 
  else if (!muteActivado && octRestPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500) {
    ultimoTiempoBotonOct = tiempoActualMillis;
    timeShowOctValue = tiempoActualMillis;
    for (int i = 0; i < NUM_MUTES; i++) {
      if(globalOctave[presetsUI.indexNotesSequence][i] > 0){globalOctave[presetsUI.indexNotesSequence][i]--;}
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
        globalVelocity[presetsUI.indexNotesSequence][i] = constrain(map(potSaveValue[i], 1018, 30, 0, 127), 0, 127);
        movedPoteValue = globalVelocity[presetsUI.indexNotesSequence][i];
        movedPoteNumber = i;
        timeShowPotValue = tiempoActualMillis;
        menusUI.menuActual = 3;
        drawUI.updateLCD = true;
        poteMovidoMutes = true;

      }
      else{
        if(globalCCSeqNumber[presetsUI.indexNotesSequence] == 0){
          int notaEscala = constrain(map(potSaveValue[i], 1018, 30, 0, midiProg[0].nNotasEscalas[globalEscalaSeleccionada[presetsUI.indexNotesSequence]] - 1), 0, midiProg[0].nNotasEscalas[globalEscalaSeleccionada[presetsUI.indexNotesSequence]] - 1);
          globalNote[presetsUI.indexNotesSequence][i] = midiProg[0].escalasNotas[globalEscalaSeleccionada[presetsUI.indexNotesSequence]][notaEscala];
          movedPoteValue = globalNote[presetsUI.indexNotesSequence][i];
          movedPoteNumber = i;
          timeShowPotValue = tiempoActualMillis;
          menusUI.menuActual = 3;
          drawUI.updateLCD = true;
        }
        else if(globalCCSeqNumber[presetsUI.indexNotesSequence] == 1){
          int valorCC = constrain(map(potSaveValue[i], 1018, 30, 0, 127), 0, 127);
          globalCCValue[presetsUI.indexNotesSequence][i] = valorCC;
          movedPoteValue = globalCCValue[presetsUI.indexNotesSequence][i];
          movedPoteNumber = i;
          timeShowPotValue = tiempoActualMillis;
          menusUI.menuActual = 3;
          drawUI.updateLCD = true;
        }
      }
    }
  }
}