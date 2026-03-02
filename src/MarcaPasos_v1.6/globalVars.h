#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include <Arduino.h>


extern int globalNote[NUM_POTES];
extern int globalVelocity[NUM_POTES];
extern int globalOctave[NUM_POTES];
extern bool globalMutes[NUM_POTES];

extern int globalCanal;
extern int globalModeSequence;
extern int globalEscalaSeleccionada;
extern int globalNTotalSteps;
extern int globalSubdivMode;
extern int globalIndexSubdivisiones;
extern int globalIndComplexSubdivY;
extern int globalIndComplexSubdivX;

#endif