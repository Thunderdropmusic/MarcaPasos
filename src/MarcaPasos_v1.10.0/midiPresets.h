#ifndef MIDI_Presets_H
#define MIDI_Presets_H

#include <Arduino.h>
#include <SdFat.h>
#include "config.h"

struct Step {
  byte note;
  byte ccValue;
  byte velocity;
  byte ccSmoothCurve;
  uint8_t mutes : 1;
  uint8_t ccMutes : 1;
  uint8_t octave : 4;
};

struct Sequence {
  Step steps[N_MAX_STEPS];
  byte canal;
  byte seqMode;
  byte ccNumber;
  byte nTotalSteps;
  byte subdivMode;
  byte indexSubdivisiones;
  byte indComplexSubdivY;
  byte indComplexSubdivX;
  byte escalaSeleccionada;
  byte noteTone;
  uint8_t armed : 1;
  uint8_t extensionArmed : 1;
};

struct Pattern {
  Sequence nSequence[N_MAX_SEQS];
  char nombreSequencia[10];
  Sequence seq_slots[3][N_MAX_SEQS];
  Sequence seq_extension[N_MAX_SEQS];
  Sequence seq_extension_slots[3][N_MAX_SEQS];
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
    const byte slotsPin[3] = {47, 45, 43};
    const byte saveButton = 41;
    byte indexSequence;
    bool slots[3];
    byte nSlot;
    bool loadPresetButton;
    bool savePresetButton;

    unsigned long ultimoTiempoBotonPresets;
    bool repeatedButton;

    Sequence* getActiveSequence() {
      return &marcaPasos.nSequence[indexSequence];
    }

    Sequence* getExtensionSequence() {
      return &marcaPasos.seq_extension[indexSequence];
    }

    void sdInit();
    void readSlotsButtons();
    void slotLoad(byte number);
    void slotSave(byte number);
    void saveSeqSD(char* nombreBase, byte tipoGuardado);
    void loadSeqSD(char* nombreBase, byte tipoCarga);
    void deleteSeqSD(char* bufferSalida, byte tipoCarga);
    void getFileRoot(char* bufferSalida, byte tipoCarga, char* rutaDestino);
    int getFileCount(byte tipoCarga);
    void getFileName(int index, byte tipoCarga, char* bufferSalida);
    bool timeDebounce();
    void aplicarCambiosBotones();
};

extern MidiPresets presetsUI;


#endif