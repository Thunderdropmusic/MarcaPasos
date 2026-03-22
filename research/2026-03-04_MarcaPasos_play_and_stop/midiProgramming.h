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
    int id;
    //Variables de tiempo real
    unsigned long int tiempoClock1;
    unsigned long int tiempoClock2; 
    unsigned long int tiempoUltimaNota; 
    unsigned long int sumaTiempos; 
    unsigned long int timeDecayNote;
    unsigned long int decay;
    unsigned long int tiempoUltimoMidiClock;
    unsigned long int tempoMicros;
    //Control de pulso y mensajes
    int pulsoClock;
    int nClockMsg;
    //Variables subdivisiones
    unsigned long int microsSubdivision;
    int subdivision;
    //Tabla de subdivisiones
    const int subdivisionesArray[12];
    const int subdivisionesComplejasArray[3][6];
    //bool control
    bool primerPulso; 
    bool primeraMedicionSubCompleja;
    bool play;
    static bool flagArmed[5];
    volatile bool notaFuera;
    //variables MIDI y notas
    int nStep;
    int notaTocada;
    int lecturaPulsoClock;
    static int modeMidiClock;
    static int tipoMsgMidi; // 0 = extern 1 = intern
    int tempo;
    const int escalasNotas[5][13];
    const int nNotasEscalas[5];

    MidiProgramming(int _id);

    //Declaracion de funciones
    void modo01Subdivision();
    void modo2Subdivision();
    void dispararNotaMode2();
    void midiSeq();
    void midiNotesOff();
    void midiNotesOn();
    void initMode();
};
extern MidiProgramming midiProg[5];

#endif