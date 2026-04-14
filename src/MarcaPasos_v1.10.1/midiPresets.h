#ifndef MIDI_Presets_H
#define MIDI_Presets_H

#include <Arduino.h>
#include <SdFat.h>
#include "config.h"

// ==============================================================================
//                           ESTRUCTURA DEL PATRÓN
// ==============================================================================

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

// ==============================================================================
//                                CLASE PRINCIPAL
// ==============================================================================

class MidiPresets {
  private:
    Sequence* s;
    SdFat sd;
    const byte chipSelect = 53; // Pin del lector de SDs
  public:
    MidiPresets();

// --- PINES ---
    const byte slotsPin[3] = {47, 45, 43};
    const byte saveButton = 41;

// --- Nº DE SECUENCIA ---
    byte indexSequence;

// --- SLOTS ---
    bool slots[3];
    byte nSlot;

// --- CONTROL DE LOS BOTONES Y TIEMPO ---
    bool loadPresetButton;
    bool savePresetButton;
    unsigned long ultimoTiempoBotonPresets;
    bool repeatedButton;

// --- PUNTEROS DE MEMORIA ---
    Sequence* getActiveSequence(){ return &marcaPasos.nSequence[indexSequence]; }

    Sequence* getExtensionSequence(){ return &marcaPasos.seq_extension[indexSequence]; }

// ==============================================================================
//                         DECLARACION DE FUNCIONES 
// ==============================================================================

// --- FUNCIONES UTILES ---
    bool timeDebounce();
    void aplicarCambiosBotones();
    void sdInit();
    void readSlotsButtons();

// --- FUNCIONES DE GUARDADO DE SECUENCIAS ---
    void saveSeqSD(char* nombreBase, byte tipoGuardado);
    void loadSeqSD(char* nombreBase, byte tipoCarga);
    void deleteSeqSD(char* bufferSalida, byte tipoCarga);
    void slotLoad(byte number);
    void slotSave(byte number);

// --- EXPLORADOR DE ARCHIVOS ---    
    int getFileCount(byte tipoCarga);
    void getFileRoot(char* bufferSalida, byte tipoCarga, char* rutaDestino);
    void getFileName(int index, byte tipoCarga, char* bufferSalida);
};

extern MidiPresets presetsUI;

#endif