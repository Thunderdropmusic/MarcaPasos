#ifndef MidiInterface_H
#define MidiInterface_H

#define NUM_POTES 8
#define NUM_MUTES 8
#define N_MAX_SEQUENCES 5


#include <Arduino.h>
#include <MIDI.h>
#include "config.h"

struct Sequence;

extern unsigned long tiempoActualMillis;




class MidiInterface {
  private:
    Sequence* s;
  public:
    MidiInterface();
    const byte potesPin[NUM_POTES] = {A0, A1, A2, A3, A4, A5, A6, A7/*, A8, A9, A10, A11, A12, A13, A14, A15*/};
    const byte mutesPin[NUM_MUTES] = {30, 31, 32, 33, 34, 35, 36, 37};
    const byte seqPinLed[N_MAX_SEQS] = {10, 11, 12, 13, 9};
    const byte seqPinsButton[N_MAX_SEQS] = {40, 42, 44, 46, 48};
    const byte pinOctRest = 27;
    const byte pinOctPlus = 29;
    const byte stopPinButton = 39;
    const byte playPinButton = 38;

    int potSaveValue[NUM_POTES];
    bool mutePulsado[NUM_POTES];

    bool muteActivado;
    bool poteBlocked[N_MAX_STEPS];
    bool poteMovidoMutes;
    byte movedPoteValue;
    byte movedPoteNumber;
    byte indexMovedMute;

    unsigned long timeShowPotValue;
    unsigned long timeShowOctValue;
    unsigned long ultimoTiempoBotonMute[NUM_MUTES];
    unsigned long TiempoPulsadoBotonMute[NUM_MUTES];
    unsigned long ultimoTiempoBotonOct;
    unsigned long ultimoTiempoBotonArm;
    unsigned long ultimoTiempoBotonDevicePlay;
    unsigned long tiempoLedPausa;

    bool octPlusPulsed;
    bool octRestPulsed;   
    bool pulsedIndividualOctave;
    bool devicePlay = false;
    bool deviceStop = false;
    bool ledState = true;

    char charNumber[9];
    byte seqLayer = 0;
    bool seqArmPulsed[N_MAX_SEQS];


    bool seqButton[N_MAX_SEQS];
    bool comandoEnviado;

    //declaracion de funciones
    void checkPotes();
    void checkMutes();
    void syncWithActiveSequence();
    bool timeDebounce(unsigned long boton, int time);
    void aplicarCambiosPotes(int sendValue, int nPote);
};

extern MidiInterface midiUI;




#endif