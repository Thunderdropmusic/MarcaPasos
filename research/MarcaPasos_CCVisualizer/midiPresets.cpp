#include <Arduino.h>
#include <SdFat.h>
#include <SPI.h> 

#include "midiPresets.h"
#include "midiProgramming.h"
#include "menus_buttons.h"
#include "midiInterface.h"
#include "draw_menus.h"
#include "config.h"

MidiPresets presetsUI;

Pattern marcaPasos;
Pattern* p = &marcaPasos;


MidiPresets::MidiPresets() {
  indexSequence = 0;
}

bool MidiPresets::timeDebounce(){
  return (tiempoActualMillis - ultimoTiempoBotonPresets >= 250);
}

void MidiPresets::aplicarCambiosBotones(){
  drawUI.updateLCD = true;
  ultimoTiempoBotonPresets = tiempoActualMillis;
  repeatedButton = true;
}

void MidiPresets::sdInit(){
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {drawUI.sdErrorMsg();}
  else{drawUI.sdCheckMsg();}
}


void MidiPresets::readPresetsButtons(){
  slot1 = !digitalRead(slotsPin[0]);
  //slot2 = !digitalRead(slotsPin[1]);
  //slot3 = !digitalRead(slotsPin[2]);
  //slot4 = !digitalRead(slotsPin[3]);
  savePresetButton = !digitalRead(saveButton);
  //loadPresetButton = !digitalRead(loadButton);
  Serial.print(slot1);
  Serial.print(slot2);  
  Serial.print(slot3);
  Serial.print(slot4);
  Serial.print(loadPresetButton);
  Serial.print(savePresetButton);
  Serial.println();

  if(slot1 && !savePresetButton){slotLoad(1);}
  else if(slot1 && savePresetButton){slotSave(1);}

  if(!slot1 && !slot2 && !slot3 && !slot4 && !loadPresetButton && !savePresetButton){repeatedButton = false;}

}

void MidiPresets::slotLoad(byte number){
  if (!timeDebounce()) return;
  if (repeatedButton) return;
  p->nSequence[indexSequence] = p->seq_slot1[indexSequence];
  menusUI.menuActual = 7;
  nSlot = number;
  aplicarCambiosBotones();
  Serial.print("guardado");
}

void MidiPresets::slotSave(byte number){
  if (!timeDebounce()) return;
  if (repeatedButton) return;
  p->seq_slot1[indexSequence] = p->nSequence[indexSequence];
  menusUI.menuActual = 6;
  nSlot = number;
  aplicarCambiosBotones();
  Serial.print("cargado");
}


