#ifndef MIDI_Presets_H
#define MIDI_Presets_H

#include <Arduino.h>
#include <SdFat.h>
#include "config.h"

struct Step {
  byte note;
  int ccValue;
  byte velocity;
  int ccSmoothCurve;
  bool mutes;
  bool ccMutes;
  byte octave;
};

struct Sequence {
  Step steps[N_MAX_STEPS];
  byte canal;
  byte seqMode;
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
  Sequence nSequence[N_MAX_SEQS];
  char nombreSequencia[10];
  Sequence seq_slot1[N_MAX_SEQS];
  Sequence seq_slot2[N_MAX_SEQS];
  Sequence seq_slot3[N_MAX_SEQS];
  Sequence seq_slot4[N_MAX_SEQS];
  Sequence seq_slot5[N_MAX_SEQS];
};

struct sequenceState {
};
extern Pattern marcaPasos;
extern Pattern* p;

class MidiPresets {
  private:
    Sequence* s;
  public:
    MidiPresets();
    const byte slotsPin[5] = {38,39,41,43,45};
    const byte loadButton = 47;
    const byte saveButton = 49;
    byte indexSequence;
    bool slot1;
    bool slot2;
    bool slot3;
    bool slot4;
    bool slot5;
    bool loadPresetButton;
    bool savePresetButton;

    Sequence* getActiveSequence() {
      return &marcaPasos.nSequence[indexSequence];
    }
    
    void readPresetsButtons();
    void slotsLoad();
};

extern MidiPresets presetsUI;


#endif