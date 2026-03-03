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
    int indexSequence;
/*
    inline byte& SHOW_NOTE();
    inline byte& NOTA_ACTUAL();
    inline byte& CANAL();*/
};

extern MidiPresets presetsUI;

//funciones para el play
byte& NOTA(int indiceSecuencia, int stepIndex);
byte& CCVALUE(int indiceSecuencia, int stepIndex);
byte& VELOCITY(int indiceSecuencia, int stepIndex);
byte& OCTAVA(int indiceSecuencia, int stepIndex);
bool& MUTES(int indiceSecuencia, int stepIndex);

//funciones de la secuencia
byte& CANAL(int indiceSecuencia);
byte& CCNUMBER(int indiceSecuencia);
Sequence& SEQ_ACTUAL(int indiceSecuencia);
byte& NTOTALSTEPS(int indiceSecuencia);
byte& SUBDIV_MODE(int indiceSecuencia);
byte& ESCALA(int indiceSecuencia);
byte& INDEX_SUBDIVISIONES(int indiceSecuencia);
byte& INDEX_COMPLEX_Y(int indiceSecuencia);
byte& INDEX_COMPLEX_X(int indiceSecuencia);
byte& MODE_SEQUENCE(int indiceSecuencia);
#endif