#include <Arduino.h>
#include <MIDI.h>
#include "MidiInterface.h"
#include "menus_buttons.h"
#include "MidiProgramming.h"
#include "midiPresets.h"
#include "globalVars.h"
#include "draw_menus.h"

#define SEQ midiProg[presetsUI.indexSequence]

MidiInterface midiUI;

MidiInterface::MidiInterface(){

};
 //Valores guardados al haber una variacion con los detectados

//declaracion de funciones
void MidiInterface::checkMutes() {
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
        if(MidiProgramming::modeMidiClock == 0){MidiProgramming::flagArmed[presetsUI.indexSequence] = true;}
        else if(MidiProgramming::modeMidiClock == 1){MidiProgramming::tipoMsgMidi = 0xFA;}
        ultimoTiempoBotonMute[0] = tiempoActualMillis;
        ultimoTiempoBotonMute[1] = tiempoActualMillis;
        comandoEnviado = true;
      }
      else if(mutePulsado[2] && mutePulsado [3] && !comandoEnviado && tiempoActualMillis - ultimoTiempoBotonMute[2] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[3] > 250){
        if(MidiProgramming::modeMidiClock == 0){globalArmed[presetsUI.indexSequence] = false;}
        else if(MidiProgramming::modeMidiClock == 1){MidiProgramming::tipoMsgMidi = 0xFC;}
        ultimoTiempoBotonMute[2] = tiempoActualMillis;
        ultimoTiempoBotonMute[3] = tiempoActualMillis;
        comandoEnviado = true;
      }
      
      if (tiempoActualMillis - TiempoPulsadoBotonMute[i] > 100 && tiempoActualMillis - ultimoTiempoBotonMute[i] > 250) {
        ultimoTiempoBotonMute[i] = tiempoActualMillis;
      }
      if (octPlusPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500 && globalOctave[presetsUI.indexSequence][i] < 9) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        globalOctave[presetsUI.indexSequence][i]++;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
        pulsedIndividualOctave = true;
      } else if (octRestPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500 && globalOctave[presetsUI.indexSequence][i] > 0) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        globalOctave[presetsUI.indexSequence][i]--;
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
        globalMutes[presetsUI.indexSequence][i] = !globalMutes[presetsUI.indexSequence][i];
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
      if(globalOctave[presetsUI.indexSequence][i] < 9){globalOctave[presetsUI.indexSequence][i]++;}
    }
    menusUI.menuActual = 5;
    drawUI.updateLCD = true;
  } 
  else if (!muteActivado && octRestPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500) {
    ultimoTiempoBotonOct = tiempoActualMillis;
    timeShowOctValue = tiempoActualMillis;
    for (int i = 0; i < NUM_MUTES; i++) {
      if(globalOctave[presetsUI.indexSequence][i] > 0){globalOctave[presetsUI.indexSequence][i]--;}
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
        globalVelocity[presetsUI.indexSequence][i] = constrain(map(potSaveValue[i], 1018, 30, 0, 127), 0, 127);
        movedPoteValue = globalVelocity[presetsUI.indexSequence][i];
        movedPoteNumber = i;
        timeShowPotValue = tiempoActualMillis;
        menusUI.menuActual = 3;
        drawUI.updateLCD = true;
        poteMovidoMutes = true;

      }
      else{
        int notaEscala = constrain(map(potSaveValue[i], 1018, 30, 0, midiProg[0].nNotasEscalas[globalEscalaSeleccionada[presetsUI.indexSequence]] - 1), 0, midiProg[0].nNotasEscalas[globalEscalaSeleccionada[presetsUI.indexSequence]] - 1);
        globalNote[presetsUI.indexSequence][i] = midiProg[0].escalasNotas[globalEscalaSeleccionada[presetsUI.indexSequence]][notaEscala];
        movedPoteValue = globalNote[presetsUI.indexSequence][i];
        movedPoteNumber = i;
        timeShowPotValue = tiempoActualMillis;
        menusUI.menuActual = 3;
        drawUI.updateLCD = true;
      }
    }
  }
}