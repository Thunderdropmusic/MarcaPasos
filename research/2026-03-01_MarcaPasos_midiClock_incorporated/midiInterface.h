#ifndef MidiInterface_H
#define MidiInterface_H

#define NUM_POTES 8
#define NUM_MUTES 8

#include <Arduino.h>
#include <MIDI.h>

extern unsigned long tiempoActualMillis;
class MidiInterface {
  public:
  MidiInterface();
    const byte potesPin[NUM_POTES] = {A0, A1, A2, A3, A4, A5, A6, A7/*, A8, A9, A10, A11, A12, A13, A14, A15*/};
    const byte mutesPin[NUM_MUTES] = {30, 31, 32, 33, 34, 35, 36, 37};
    const byte pinOctRest = 38;
    const byte pinOctPlus = 39;
    int potSaveValue[NUM_POTES];
    int notesValue[NUM_POTES];
    bool mutesValue[NUM_POTES];
    int octavaValue[NUM_POTES];
    int velocityValue[NUM_POTES];
    bool mutePulsado[NUM_POTES];
    bool muteActivado;
    bool poteMovido;
    bool poteMovidoMutes;
    int val;
    int movedPoteValue;
    int lastNoteValue;
    int movedPoteNumber;
    int indexMovedMute;
    unsigned long timeShowPotValue;
    unsigned long timeShowOctValue;
    unsigned long ultimoTiempoBotonMute[NUM_MUTES];
    unsigned long TiempoPulsadoBotonMute[NUM_MUTES];
    unsigned long ultimoTiempoBotonOct;
    bool mute;
    bool octPlusPulsed;
    bool octRestPulsed;
    bool pulsedIndividualOctave;
    bool comandoEnviado;

    //declaracion de funciones
    void checkPotes();
    void checkMutes();
};

extern MidiInterface midiUI;




#endif