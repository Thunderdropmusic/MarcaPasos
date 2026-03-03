#include <Arduino.h>
#include <MIDI.h>
#include "MidiInterface.h"
#include "menus_buttons.h"
#include "MidiProgramming.h"

MidiInterface midiUI;

MidiInterface::MidiInterface(){
  for(int i = 0; i < NUM_POTES; i++){
    octavaValue[i] = 4;
    mutePulsado[i] = false;
    mutesValue[i] = false;
  }
}
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
    mute = digitalRead(mutesPin[i]);
    if (mute) {
      if (mutePulsado[i] == false) {
        mutePulsado[i] = true;
        muteActivado = true;
        TiempoPulsadoBotonMute[i] = tiempoActualMillis;
        indexMovedMute = i;
      }
      /*
      if(mutePulsado[i][0] && mutePulsado[i][1] && octava < 9 && tiempoActualMillis - ultimoTiempoBotonMute[0] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[1] > 250){
        octava ++;
        ultimoTiempoBotonMute[0] = tiempoActualMillis;
        ultimoTiempoBotonMute[1] = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
      }
      else if(mutePulsado[i][2] && mutePulsado[i][3] && octava > 0 && tiempoActualMillis - ultimoTiempoBotonMute[2] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[3] > 250){
        octava --;
        ultimoTiempoBotonMute[2] = tiempoActualMillis;
        ultimoTiempoBotonMute[3] = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
      }
      */
      if (tiempoActualMillis - TiempoPulsadoBotonMute[i] > 100 && tiempoActualMillis - ultimoTiempoBotonMute[i] > 250) {
        ultimoTiempoBotonMute[i] = tiempoActualMillis;
      }
      if (octPlusPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500 && octavaValue[i] < 9) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        octavaValue[i]++;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
        pulsedIndividualOctave = true;
      } else if (octRestPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500 && octavaValue[i] > 0) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        octavaValue[i]--;
        menusUI.menuActual = 4;
        drawUI.updateLCD = true;
        pulsedIndividualOctave = true;
      }
    } 
    else {
      if (mutePulsado[i] == true && !pulsedIndividualOctave && !octPlusPulsed && !octRestPulsed && !poteMovidoMutes) {
        mutesValue[i] = !mutesValue[i];
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
      if(octavaValue[i] < 9){octavaValue[i]++;}
    }
    menusUI.menuActual = 5;
    drawUI.updateLCD = true;
  } 
  else if (!muteActivado && octRestPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500) {
    ultimoTiempoBotonOct = tiempoActualMillis;
    timeShowOctValue = tiempoActualMillis;
    for (int i = 0; i < NUM_MUTES; i++) {
      if(octavaValue[i] > 0){octavaValue[i]--;}
    }
    menusUI.menuActual = 5;
    drawUI.updateLCD = true;
  }
}



void MidiInterface::checkPotes() {
  for (int i = 0; i < NUM_POTES; i++) {
    int val = analogRead(potesPin[i]);
    if(abs(val - potSaveValue[i]) > 20){
      midiUI.potSaveValue[i] = val;
      if(muteActivado){
        velocityValue[i] = constrain(map(potSaveValue[i], 1018, 30, 0, 127), 0, 127);
        movedPoteValue = velocityValue[i];
        movedPoteNumber = i;
        timeShowPotValue = tiempoActualMillis;
        menusUI.menuActual = 3;
        drawUI.updateLCD = true;
        poteMovidoMutes = true;

      }
      else{
        int notaEscala = constrain(map(potSaveValue[i], 1018, 30, 0, midiProg.nNotasEscalas[midiProg.escalaSeleccionada] - 1), 0, midiProg.nNotasEscalas[midiProg.escalaSeleccionada] - 1);
        notesValue[i] = midiProg.escalasNotas[midiProg.escalaSeleccionada][notaEscala];
        movedPoteValue = notesValue[i];
        movedPoteNumber = i;
        timeShowPotValue = tiempoActualMillis;
        menusUI.menuActual = 3;
        if(notesValue[i] != lastNoteValue){
          drawUI.updateLCD = true;
          lastNoteValue = notesValue[i];
        }
      }
    }
  }
}