#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include "midiInterface.h"

#include <Arduino.h>

#define N_MAX_SEQUENCES 5

extern int globalNote[N_MAX_SEQUENCES][NUM_POTES];
extern int globalCCValue[N_MAX_SEQUENCES][NUM_POTES];
extern int globalVelocity[N_MAX_SEQUENCES][NUM_POTES];
extern int globalOctave[N_MAX_SEQUENCES][NUM_POTES];
extern bool globalMutes[N_MAX_SEQUENCES][NUM_POTES];

extern int globalCanal[N_MAX_SEQUENCES];
extern int globalCCNumber[N_MAX_SEQUENCES];
extern int globalModeSequence[N_MAX_SEQUENCES];
extern int globalEscalaSeleccionada[N_MAX_SEQUENCES];
extern int globalNTotalSteps[N_MAX_SEQUENCES];
extern int globalSubdivMode[N_MAX_SEQUENCES];
extern int globalIndexSubdivisiones[N_MAX_SEQUENCES];
extern int globalIndComplexSubdivY[N_MAX_SEQUENCES];
extern int globalIndComplexSubdivX[N_MAX_SEQUENCES];
extern int globalArmed[N_MAX_SEQUENCES];

extern ;

#endif