#include <Arduino.h>

#include "midiPresets.h"
#include "midiProgramming.h"
#include "menus_buttons.h"
#include "midiInterface.h"
MidiPresets presetsUI;
Pattern marcaPasos;



MidiPresets::MidiPresets() {
  indexSequence = 0;
}


byte& NOTA(int stepIndex) {return marcaPasos.nSequence[presetsUI.indexSequence].steps[stepIndex].note;}
byte& VELOCITY(int stepIndex) {return marcaPasos.nSequence[presetsUI.indexSequence].steps[stepIndex].velocity;}
byte& OCTAVA(int stepIndex) {return marcaPasos.nSequence[presetsUI.indexSequence].steps[stepIndex].octava;}
bool& MUTES(int stepIndex) {return marcaPasos.nSequence[presetsUI.indexSequence].steps[stepIndex].mute;}

Sequence& SEQ_ACTUAL() {return marcaPasos.nSequence[presetsUI.indexSequence];}
byte& NTOTALSTEPS() {return marcaPasos.nSequence[presetsUI.indexSequence].nTotalSteps;}
byte& SUBDIV_MODE() {return marcaPasos.nSequence[presetsUI.indexSequence].subdivMode;}
byte& CANAL() {return marcaPasos.nSequence[presetsUI.indexSequence].canal;}
byte& ESCALA() {return marcaPasos.nSequence[presetsUI.indexSequence].escalaSeleccionada;}
byte& INDEX_SUBDIVISIONES() {return marcaPasos.nSequence[presetsUI.indexSequence].indexSubdivisiones;}
byte& INDEX_COMPLEX_Y() {return marcaPasos.nSequence[presetsUI.indexSequence].indComplexSubdivY;}
byte& INDEX_COMPLEX_X() {return marcaPasos.nSequence[presetsUI.indexSequence].indComplexSubdivX;}
byte& MODE_SEQUENCE() {return marcaPasos.nSequence[presetsUI.indexSequence].modeSequence;}