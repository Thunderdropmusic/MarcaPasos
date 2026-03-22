#ifndef MIDI_Presets_H
#define MIDI_Presets_H

#include <Arduino.h>



struct Step {
  byte note;
  byte ccValue;
  byte velocity;
  bool mute;
  byte octava;
};

struct Sequence {
  Step steps[8];
  byte canal;
  byte ccNumber;
  byte modeSequence;
  byte escalaSeleccionada;
  byte nTotalSteps;
  byte subdivMode;
  byte indexSubdivisiones;
  byte indComplexSubdivY;
  byte indComplexSubdivX;
  bool armed;
};

struct Pattern {
  Sequence nSequence[5];
  char nombreSequencia[10];
};

struct sequenceState {

};
extern Pattern marcaPasos;


class MidiPresets {
  public:
    MidiPresets();
    byte indexSequence;
/*
    inline byte& SHOW_NOTE();
    inline byte& NOTA_ACTUAL();
    inline byte& CANAL();*/
};

extern MidiPresets presetsUI;

//funciones para el play
byte& NOTA(byte indiceSecuencia, byte stepIndex);
byte& CCVALUE(byte indiceSecuencia, byte stepIndex);
byte& VELOCITY(byte indiceSecuencia, byte stepIndex);
byte& OCTAVA(byte indiceSecuencia, byte stepIndex);
bool& MUTES(byte indiceSecuencia, byte stepIndex);

//funciones de la secuencia
byte& CANAL(byte indiceSecuencia);
byte& CCNUMBER(byte indiceSecuencia);
Sequence& SEQ_ACTUAL(byte indiceSecuencia);
byte& NTOTALSTEPS(byte indiceSecuencia);
byte& SUBDIV_MODE(byte indiceSecuencia);
byte& ESCALA(byte indiceSecuencia);
byte& INDEX_SUBDIVISIONES(byte indiceSecuencia);
byte& INDEX_COMPLEX_Y(byte indiceSecuencia);
byte& INDEX_COMPLEX_X(byte indiceSecuencia);
byte& MODE_SEQUENCE(byte indiceSecuencia);
byte& ARMED(byte indiceSecuencia);
#endif