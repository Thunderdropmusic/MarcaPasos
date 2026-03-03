#ifndef MIDI_Presets_H
#define MIDI_Presets_H

#include "midiProgramming.h"
#include "menus_buttons.h"

struct Step {
  byte nota;
  byte velocity;
  bool mute;
}

struct Sequence {
  Step steps[8];
  byte canal;
  byte mode;
  nTotalSteps;
  subdivMode;
  indiceSubdivisiones;
  indComplexSubdivY;
  indComplexSubdivX;
}

struct Pattern {
  Sequence nSequence[5];
  char nombreSequencia[10];
}

Pattern marcaPasos;