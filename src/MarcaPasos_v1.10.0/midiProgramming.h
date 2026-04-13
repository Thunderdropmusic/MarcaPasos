#ifndef MIDI_PROGRAMING_H
#define MIDI_PROGRAMING_H

#include <Arduino.h>
#include <MIDI.h>
#include "config.h"

struct Sequence;

// Configuracion libreria MIDI
struct HairlessConfig : public midi::DefaultSettings {
  static const long BaudRate = 115200;
};
// Declaración de la instancia
extern midi::MidiInterface<midi::SerialMIDI<HardwareSerial, HairlessConfig>> MIDI;

extern unsigned long tiempoActualMicros;

// Matriz de curvas precalculadas en la memoria Flash
// Índice 0: Logarítmica extrema (Ataque muy rápido, caída lenta)
// Índice 6: Lineal pura (Centro)
// Índice 12: Exponencial extrema (Ataque muy lento, subida abrupta al final)
extern const byte smoothCurves[13][100] PROGMEM;

// ==============================================================================
//                                CLASE PRINCIPAL
// ==============================================================================

class MidiProgramming {
  private:
    Sequence* seqActual;
  public:
    byte id;

    // --- Variables de tiempo real ---
    unsigned long int tiempoClock1;
    unsigned long int tiempoClock2; 
    unsigned long int tiempoUltimaNota; 
    unsigned long int sumaTiempos; 
    unsigned long int timeDecayNote;
    unsigned long int decay;
    unsigned long int tiempoUltimoMidiClock;
    unsigned long int tempoMicros;

    // --- Control de pulso y mensajes ---
    byte pulsoClock;
    byte nClockMsg;

    // --- Variables subdivisiones ---
    unsigned long int microsSubdivision;
    int subdivision;

    // --- Tabla de subdivisiones ---
    static const byte subdivisionesArray[12];
    static const byte subdivisionesComplejasArray[3][6];

    // --- Bool control ---
    bool primerPulso; 
    bool primeraMedicionSubCompleja;
    bool play;
    static bool flagArmed[N_MAX_SEQS];
    volatile bool notaFuera;
    bool inExtension;

    // --- Variables MIDI y notas ---
    byte nStep;
    byte notaTocada;
    byte lecturaPulsoClock;
    static byte modeMidiClock; // 0 = extern 1 = intern
    static byte tipoMsgMidi; 

    byte CCinterpolation[96];

    int tempo;
    static const byte escalasNotas[5][13];
    static const byte nNotasEscalas[5];

    byte nMsgCC[N_MAX_SEQS];

    MidiProgramming(byte _id);

    // --- Declaracion de funciones ---
    void modo01Subdivision();
    void modo2Subdivision();
    void dispararNotaMode2();
    void midiSeq();
    void midiNotesOff();
    void midiNotesOn();
    void initMode();
    void CCSend();
    int ccCurveFunction(int i, int subdivisionActual, int tensionMode);
};
extern MidiProgramming midiProg[N_MAX_SEQS];

#endif