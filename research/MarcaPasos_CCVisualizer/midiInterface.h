#ifndef MidiInterface_H
#define MidiInterface_H

#define NUM_POTES 8
#define NUM_MUTES 8
#define N_MAX_SEQUENCES 5


#include <Arduino.h>
#include <MIDI.h>

struct Sequence;

extern unsigned long tiempoActualMillis;




class MidiInterface {
  private:
    Sequence* s;
  public:
    MidiInterface();
    const byte potesPin[NUM_POTES] = {A0, A1, A2, A3, A4, A5, A6, A7/*, A8, A9, A10, A11, A12, A13, A14, A15*/};
    const byte mutesPin[NUM_MUTES] = {30, 31, 32, 33, 34, 35, 36, 37};
    byte seq1PinButton = 41;
    const byte seqPinLed[5] = {10, 11, 12, 13, 3};
    const byte seq2PinButton = 43;
    const byte seq3PinButton = 45;
    const byte seq4PinButton = 47;
    const byte seq5PinButton = 49;
    const byte pinOctRest = 38;
    const byte pinOctPlus = 39;
    int potSaveValue[NUM_POTES];
    bool mutePulsado[NUM_POTES];
    bool muteActivado;
    bool poteMovido;
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
    bool octPlusPulsed;
    bool octRestPulsed;   
    bool pulsedIndividualOctave;

    bool seqArmPulsed[5];

    bool seqButton[5];

    bool comandoEnviado;

    //declaracion de funciones
    void checkPotes();
    void checkMutes();
    void syncWithActiveSequence();
};

extern MidiInterface midiUI;




#endif