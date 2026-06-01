#ifndef MidiInterface_H
#define MidiInterface_H

// ==============================================================================
//                                CONFIGURACIÓN
// ==============================================================================

#define NUM_POTES 16
#define NUM_MUTES 16

#include <Arduino.h>
#include <MIDI.h>
#include "config.h"

// Declaraciones adelantadas (Forward declarations)
struct Sequence;
struct Step;
extern unsigned long tiempoActualMillis;

// ==============================================================================
//                                CLASE PRINCIPAL
// ==============================================================================

class MidiInterface {
  private:
    Sequence* s; // Puntero a la secuencia activa
    Step* stp;

  public:
    MidiInterface();


    // --- PINES DE HARDWARE (Constantes) ---
    const byte potesPin[NUM_POTES]       = {A0, A1, A2, A3, A4, A5, A6, A7,A8, A9, A10, A11, A12, A13, A14, A15};
    
    const byte mutesPin[NUM_MUTES]       = {47, 43, 45, 41, 3, 49, 14, 48,46,44,42,4,5,6,7,8};
    const byte seqPinsButton[N_MAX_SEQS] = {32, 34, 36, 38, 40};
    const byte pinOctRest                = 22;
    const byte pinOctPlus                = 24;
    const byte stopPinButton             = 28;
    const byte playPinButton             = 26;

    const byte seqPinLed[N_MAX_SEQS]    = {13,12,11,10,9};



    // --- VARIABLES DE ESTADO: TRANSPORTE Y SECUENCIAS ---
    bool devicePlay = false;
    bool deviceFlagPlay = false;
    bool deviceStop = false;
    bool ledState   = true;
    bool comandoEnviado;
    
    byte seqLayer = 0;
    bool seqButton[N_MAX_SEQS];
    bool seqArmPulsed[N_MAX_SEQS];


    // --- VARIABLES DE ESTADO: MUTES Y OCTAVAS ---
    bool muteActivado;
    bool mutePulsado[NUM_POTES];
    byte indexMovedMute;

    bool octPlusPulsed;
    bool octRestPulsed;   
    bool pulsedIndividualOctave;


    // --- VARIABLES DE ESTADO: POTENCIÓMETROS (Soft-Takeover) ---
    int  potSaveValue[NUM_POTES];
    bool poteBlocked[N_MAX_STEPS]; // Asumo que N_MAX_STEPS viene de config.h
    bool poteMovidoMutes;
    byte movedPoteValue;
    byte movedPoteNumber;


    // --- TEMPORIZADORES (Debounce y animaciones) ---
    unsigned long ultimoTiempoBotonMute[NUM_MUTES];
    unsigned long TiempoPulsadoBotonMute[NUM_MUTES];
    unsigned long ultimoTiempoBotonOct;
    unsigned long ultimoTiempoBotonArm;
    unsigned long ultimoTiempoBotonDevicePlay;
    unsigned long tiempoLedPausa;
    unsigned long timeShowPotValue;


    // --- BUFFERS Y DATOS ---
    char charNumber[9]; // Buffer para el nombre de los presets (8 chars + '\0')


    // --- MÉTODOS Y FUNCIONES ---
    void checkPotes();
    void checkMidiButtons();
    void syncWithActiveSequence();
    void aplicarCambiosPotes(int sendValue, int nPote);
    bool timeDebounce(unsigned long boton, int time);
};

extern MidiInterface midiUI;

#endif