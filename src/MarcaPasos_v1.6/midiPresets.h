#ifndef MIDI_Presets_H
#define MIDI_Presets_H

#include <Arduino.h>



struct Step {
  byte note;
  byte velocity;
  bool mute;
  byte octava;
};

struct Sequence {
  Step steps[8];
  byte canal;
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
class MidiProgramming;
extern MidiProgramming midiProg;
class MenusButtons;
extern MenusButtons menusUI;
class MidiInterface;
extern MidiInterface midiUI;
class DrawMenus;
extern DrawMenus drawUI;

//funciones para el play
byte& NOTA(int stepIndex);
byte& VELOCITY(int stepIndex);
byte& OCTAVA(int stepIndex);
bool& MUTES(int stepIndex);

//funciones de la secuencia
byte& CANAL();
Sequence& SEQ_ACTUAL();
byte& NTOTALSTEPS();
byte& SUBDIV_MODE();
byte& ESCALA();
byte& INDEX_SUBDIVISIONES();
byte& INDEX_COMPLEX_Y();
byte& INDEX_COMPLEX_X();
byte& MODE_SEQUENCE();
#endif