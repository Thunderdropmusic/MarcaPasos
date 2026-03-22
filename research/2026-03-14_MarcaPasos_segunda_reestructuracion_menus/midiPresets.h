#ifndef MIDI_Presets_H
#define MIDI_Presets_H

#include <Arduino.h>



struct Step {
  byte note;
  byte ccValue;
  byte velocity;
  bool mute;
  byte octava;
};

struct Sequence {
  Step steps[8];
  byte canal;
  byte ccNumber;
  byte modeSequence;
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


class MidiPresets {
  public:
    MidiPresets();
    byte indexNotesSequence;
    
    Sequence* getActiveSequence() {
      return &marcaPasos.nSequence[indexNotesSequence];
    }
};

extern MidiPresets presetsUI;


#endif