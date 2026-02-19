#ifndef MIDIINTERFACE_H
#define MIDIINTERFACE_H

#include <Arduino.h>
#include <MIDI.h>

extern const byte potesPin[];
extern const byte mutesPin[];
extern const byte pinOctPlus;
extern const byte pinOctRest;

extern int potesValue[];
extern bool mutesValue[];
extern int octavaValue[];

extern const int numPotes;
extern const int numMutes;
extern bool mutePulsado[];
extern bool muteActivado;
extern int val, movedPoteValue, movedPoteNumber;
extern int indiceMovedMute;
extern unsigned long timeShowPotValue;
extern unsigned long timeShowOctValue;
extern unsigned long tiempoActualMillis;
extern unsigned long ultimoTiempoBotonMute[];
extern unsigned long TiempoPulsadoBotonMute[];
unsigned long ultimoTiempoBotonOct;
extern bool mute;
extern int menuActual;
extern bool updateLCD;

extern int escalaSeleccionada;
extern const int escalasNotas[5][13];
extern const int nNotasEscalas[];

bool octPlusPulsed, octRestPulsed;
bool pulsedIndividualOctave;

//declaracion de funciones
void checkPotes();
void checkMutes();


void checkMutes() {
  bool octPlus = digitalRead(pinOctPlus);
  bool octRest = digitalRead(pinOctRest);

  if (octPlus && !octPlusPulsed) {
    octPlusPulsed = true;
  }
  if (octRest && !octRestPulsed) {
    octRestPulsed = true;
  }

  for (int i = 0; i < numMutes; i++) {
    mute = digitalRead(mutesPin[i]);
    if (mute) {
      if (mutePulsado[i] == false) {
        mutePulsado[i] = true;
        muteActivado = true;
        TiempoPulsadoBotonMute[i] = tiempoActualMillis;
        indiceMovedMute = i;
      }
      /*
      if(mutePulsado[i][0] && mutePulsado[i][1] && octava < 9 && tiempoActualMillis - ultimoTiempoBotonMute[0] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[1] > 250){
        octava ++;
        ultimoTiempoBotonMute[0] = tiempoActualMillis;
        ultimoTiempoBotonMute[1] = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        menuActual = 4;
        updateLCD = true;
      }
      else if(mutePulsado[i][2] && mutePulsado[i][3] && octava > 0 && tiempoActualMillis - ultimoTiempoBotonMute[2] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[3] > 250){
        octava --;
        ultimoTiempoBotonMute[2] = tiempoActualMillis;
        ultimoTiempoBotonMute[3] = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        menuActual = 4;
        updateLCD = true;
      }
      */
      if (tiempoActualMillis - TiempoPulsadoBotonMute[i] > 100 && tiempoActualMillis - ultimoTiempoBotonMute[i] > 250) {
        ultimoTiempoBotonMute[i] = tiempoActualMillis;
      }
      if (octPlusPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500 && octavaValue[i] < 9) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        octavaValue[i]++;
        menuActual = 4;
        updateLCD = true;
        pulsedIndividualOctave = true;
      } else if (octRestPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500 && octavaValue[i] > 0) {
        ultimoTiempoBotonOct = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        octavaValue[i]--;
        menuActual = 4;
        updateLCD = true;
        pulsedIndividualOctave = true;
      }
    } 
    else {
      if (mutePulsado[i] == true && !pulsedIndividualOctave && !octPlusPulsed && !octRestPulsed) {
        mutesValue[i] = !mutesValue[i];
        updateLCD = true;
        mutePulsado[i] = false;
        muteActivado = false;
      } else if (mutePulsado[i] == true && pulsedIndividualOctave) {
        mutePulsado[i] = false;
        muteActivado = false;
        pulsedIndividualOctave = false;
      }
    }
  }
  if (!octPlus && octPlusPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 250) { octPlusPulsed = false; }
  if (!octRest && octRestPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 250) { octRestPulsed = false; }
  if (!muteActivado && octPlusPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500) {
    ultimoTiempoBotonOct = tiempoActualMillis;
    timeShowOctValue = tiempoActualMillis;
    for (int i = 0; i < numMutes; i++) {
      if(octavaValue[i] < 9){octavaValue[i]++;}
    }
    menuActual = 5;
    updateLCD = true;
  } else if (!muteActivado && octRestPulsed && tiempoActualMillis - ultimoTiempoBotonOct > 500) {
    ultimoTiempoBotonOct = tiempoActualMillis;
    timeShowOctValue = tiempoActualMillis;
    for (int i = 0; i < numMutes; i++) {
      if(octavaValue[i] > 0){octavaValue[i]--;}
    }
    menuActual = 5;
    updateLCD = true;
  }
}



void checkPotes() {
  for (int i = 0; i < numPotes; i++) {
    int indice = constrain(map(analogRead(potesPin[i]), 1018, 30, 0, nNotasEscalas[escalaSeleccionada] - 1), 0, nNotasEscalas[escalaSeleccionada] - 1);
    int val = escalasNotas[escalaSeleccionada][indice];
    potesValue[i] = val;
    if (abs(val - potSaveValue[i]) > 0.5) {  //Solo activamos cuando sea mayor de 0.5;
      potSaveValue[i] = val;
      movedPoteValue = val;
      movedPoteNumber = i;
      timeShowPotValue = tiempoActualMillis;
      menuActual = 3;
      updateLCD = true;
    }
  }
}

#endif