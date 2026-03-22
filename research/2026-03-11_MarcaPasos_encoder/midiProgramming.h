#ifndef MIDI_PROGRAMING_H
#define MIDI_PROGRAMING_H

#include <Arduino.h>
#include <MIDI.h>


// Configuracion libreria MIDI
struct HairlessConfig : public midi::DefaultSettings {
  static const long BaudRate = 115200;
};
// Declaración de la instancia
extern midi::MidiInterface<midi::SerialMIDI<HardwareSerial, HairlessConfig>> MIDI;

extern unsigned long tiempoActualMicros;
class MidiProgramming {
  public:
    byte id;
    //Variables de tiempo real
    unsigned long int tiempoClock1[5];
    unsigned long int tiempoClock2[5]; 
    unsigned long int tiempoUltimaNota[5]; 
    unsigned long int sumaTiempos[5]; 
    unsigned long int timeDecayNote;
    unsigned long int decay;
    unsigned long int tiempoUltimoMidiClock;
    unsigned long int tempoMicros;
    //Control de pulso y mensajes
    byte pulsoClock;
    byte nClockMsg;
    //Variables subdivisiones
    unsigned long int microsSubdivision[5];
    byte subdivision[5];
    //Tabla de subdivisiones
    const byte subdivisionesArray[12];
    const byte subdivisionesComplejasArray[3][6];
    //bool control
    bool primerPulso[5]; 
    bool primeraMedicionSubCompleja[5];
    bool play;
    static bool flagArmed[5];
    volatile bool notaFuera;
    //variables MIDI y notas
    byte nStep;
    byte notaTocada[5];
    byte lecturaPulsoClock[5];
    static byte modeMidiClock;
    static byte tipoMsgMidi; // 0 = extern 1 = intern
    byte CCinterpolation[5][96];
    int tempo;
    const byte escalasNotas[5][13];
    const byte nNotasEscalas[5];

    MidiProgramming(byte _id);

    //Declaracion de funciones
    void modo01Subdivision();
    void modo2Subdivision();
    void dispararNotaMode2();
    void midiSeq();
    void midiNotesOff();
    void midiNotesOn();
    void initMode();
    void CCSend();
};
extern MidiProgramming midiProg[5];

#endif