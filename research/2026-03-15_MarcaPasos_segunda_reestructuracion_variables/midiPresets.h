#ifndef MIDI_Presets_H
#define MIDI_Presets_H

#include <Arduino.h>



struct Step {
  byte note;
  int ccValue;
  byte velocity;
  float ccSmooth;
  bool mutes;
  byte octave;
};

struct Sequence {
  Step steps[8];
  byte canal;
  byte ccNumber;
  byte escalaSeleccionada;
  byte nTotalSteps;
  byte subdivMode;
  byte indexSubdivisiones;
  byte indComplexSubdivY;
  byte indComplexSubdivX;
  bool armed;
};

struct Pattern {
  Sequence nSequence[5];
  char nombreSequencia[10];
};

struct sequenceState {

};
extern Pattern marcaPasos;
extern Pattern* p;

class MidiPresets {
  public:
    MidiPresets();
    byte indexSequence;
    
    Sequence* getActiveSequence() {
      return &marcaPasos.nSequence[indexSequence];
    }
};

extern MidiPresets presetsUI;


#endif