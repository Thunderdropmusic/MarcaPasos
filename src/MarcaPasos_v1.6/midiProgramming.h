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
  MidiProgramming();
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
    volatile bool notaFuera;
    //variables MIDI y notas
    int tipoMsgMidi; 
    int nStep;
    int nStepAnterior;
    int notaTocada;
    int lecturaPulsoClock;
    int modeMidiClock; // 0 = extern 1 = intern
    int clock;
    int tempo;
    const int escalasNotas[5][13];
    const int nNotasEscalas[5];

    //Declaracion de funciones
    void modo01Subdivision();
    void modo2Subdivision();
    void dispararNotaMode2();
    void midiSeq();
    void midiNotesOff();
    void midiNotesOn();
    void initMode();
    void midiClock();
};
extern MidiProgramming midiProg;

#endif