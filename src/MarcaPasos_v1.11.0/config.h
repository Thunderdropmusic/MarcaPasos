#ifndef CONFIG_H
#define CONFIG_H

#define CMD_READY 0xCC
#define CMD_START_TRANSFER 0xAA
#define CMD_ACK 0x06
#define CMD_ERROR 0x15


const int MAX_POTE_VALUE = 1018;
const byte MIN_POTE_VALUE = 30;
const byte DEBOUNCE_DELAY_MS = 250;
const byte N_MAX_STEPS = 16;
const byte N_MAX_SEQS = 5;

extern unsigned long tiempoActualMillis;
extern unsigned long tiempoActualMicros;



#endif