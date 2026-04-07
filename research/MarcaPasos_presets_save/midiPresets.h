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
};

struct sequenceState {
};
extern Pattern marcaPasos;
extern Pattern* p;

class MidiPresets {
  private:
    Sequence* s;
    SdFat sd;
    const byte chipSelect = 53;
  public:
    MidiPresets();
    const byte slotsPin[4] = {49,47,45,43};
    const byte saveButton = 39;
    const byte loadButton = 38;
    byte indexSequence;
    bool slot1;
    bool slot2;
    bool slot3;
    bool slot4;
    byte nSlot;
    bool loadPresetButton;
    bool savePresetButton;

    unsigned long ultimoTiempoBotonPresets;
    bool repeatedButton;

    Sequence* getActiveSequence() {
      return &marcaPasos.nSequence[indexSequence];
    }

    void sdInit();
    void readPresetsButtons();
    void slotLoad(byte number);
    void slotSave(byte number);
    void saveSeqSD(char* nombreBase, byte tipoGuardado);
    void loadSeqSD(char* nombreBase, byte tipoCarga);
    void deleteSeqSD(char* bufferSalida, byte tipoCarga);
    int getFileCount(byte tipoCarga);
    void getFileName(int index, byte tipoCarga, char* bufferSalida);
    bool timeDebounce();
    void aplicarCambiosBotones();
};

extern MidiPresets presetsUI;


#endif