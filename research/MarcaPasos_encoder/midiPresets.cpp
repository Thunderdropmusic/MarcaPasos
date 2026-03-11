#include <Arduino.h>

#include "midiPresets.h"
#include "midiProgramming.h"
#include "menus_buttons.h"
#include "midiInterface.h"
#include "globalVars.h"
#include "draw_menus.h"

MidiPresets presetsUI;
Pattern marcaPasos;



MidiPresets::MidiPresets() {
  indexSequence = 0;
}


byte& NOTA(byte stepIndex) {return marcaPasos.nSequence[presetsUI.indexSequence].steps[stepIndex].note;}
byte& CCVALUE(byte stepIndex) {return marcaPasos.nSequence[presetsUI.indexSequence].steps[stepIndex].ccValue;}
byte& VELOCITY(byte stepIndex) {return marcaPasos.nSequence[presetsUI.indexSequence].steps[stepIndex].velocity;}
byte& OCTAVA(byte stepIndex) {return marcaPasos.nSequence[presetsUI.indexSequence].steps[stepIndex].octava;}
bool& MUTES(byte stepIndex) {return marcaPasos.nSequence[presetsUI.indexSequence].steps[stepIndex].mute;}

Sequence& SEQ_ACTUAL() {return marcaPasos.nSequence[presetsUI.indexSequence];}
byte& CCNUMBER() {return marcaPasos.nSequence[presetsUI.indexSequence].ccNumber;}
byte& NTOTALSTEPS() {return marcaPasos.nSequence[presetsUI.indexSequence].nTotalSteps;}
byte& SUBDIV_MODE() {return marcaPasos.nSequence[presetsUI.indexSequence].subdivMode;}
byte& CANAL() {return marcaPasos.nSequence[presetsUI.indexSequence].canal;}
byte& ESCALA() {return marcaPasos.nSequence[presetsUI.indexSequence].escalaSeleccionada;}
byte& INDEX_SUBDIVISIONES() {return marcaPasos.nSequence[presetsUI.indexSequence].indexSubdivisiones;}
byte& INDEX_COMPLEX_Y() {return marcaPasos.nSequence[presetsUI.indexSequence].indComplexSubdivY;}
byte& INDEX_COMPLEX_X() {return marcaPasos.nSequence[presetsUI.indexSequence].indComplexSubdivX;}
byte& MODE_SEQUENCE() {return marcaPasos.nSequence[presetsUI.indexSequence].modeSequence;}
bool& ARMED() {return marcaPasos.nSequence[presetsUI.indexSequence].armed;}

