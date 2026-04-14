#include <Arduino.h>


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


void MidiPresets::readPresetsButtons(){
  slot1 = !digitalRead(slotsPin[1]);
  slot2 = !digitalRead(slotsPin[2]);
  slot3 = !digitalRead(slotsPin[3]);
  slot4 = !digitalRead(slotsPin[4]);
  slot5 = !digitalRead(slotsPin[5]);
  loadPresetButton = !digitalRead(loadButton);
  savePresetButton = !digitalRead(saveButton);
}

void MidiPresets::slotsLoad(){
  s = getActiveSequence();

}
