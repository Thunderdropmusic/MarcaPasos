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

