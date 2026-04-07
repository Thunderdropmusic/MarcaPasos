#ifndef CONFIG_H
#define CONFIG_H


const byte MAX_POTE_VALUE = 1018;
const byte DEBOUNCE_DELAY_MS = 250;
const byte N_MAX_STEPS = 8;
const byte N_MAX_SEQS = 8;

// mensajes midi

// Matriz de curvas precalculadas en la memoria Flash
// 13 tipos de curva x 100 pasos de resolución (valores de 0 a 255)

extern unsigned long tiempoActualMillis;
extern unsigned long tiempoActualMicros;



#endif