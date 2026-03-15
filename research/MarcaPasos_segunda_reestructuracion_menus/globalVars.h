#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include "midiInterface.h"

#include <Arduino.h>

#define N_MAX_SEQUENCES 5

extern byte globalNote[N_MAX_SEQUENCES][NUM_POTES];
extern byte globalCCValue[N_MAX_SEQUENCES][NUM_POTES];
extern byte globalVelocity[N_MAX_SEQUENCES][NUM_POTES];
extern byte globalOctave[N_MAX_SEQUENCES][NUM_POTES];
extern bool globalMutes[N_MAX_SEQUENCES][NUM_POTES];

extern byte globalCanal[N_MAX_SEQUENCES];
extern byte globalCCNumber[N_MAX_SEQUENCES];
extern byte globalCCSeqNumber[N_MAX_SEQUENCES];
extern byte globalEscalaSeleccionada[N_MAX_SEQUENCES];
extern byte globalNTotalSteps[N_MAX_SEQUENCES];
extern byte globalSubdivMode[N_MAX_SEQUENCES];
extern byte globalIndexSubdivisiones[N_MAX_SEQUENCES];
extern byte globalIndComplexSubdivY[N_MAX_SEQUENCES];
extern byte globalIndComplexSubdivX[N_MAX_SEQUENCES];
extern bool globalArmed[N_MAX_SEQUENCES];

extern ;

#endif