#ifndef MIDIINTERFACE_H
#define MIDIINTERFACE_H

#include <Arduino.h>
#include <MIDI.h>

extern const byte potesPin[];
extern const byte mutesPin[];

extern int potesValue[];
extern bool mutesValue[];
extern const int numPotes;
extern const int numMutes;
extern bool mutePulsado[];
extern int val, movedPoteValue, movedPoteNumber;
extern int indiceMovedMute;
extern unsigned long timeShowPotValue;
extern unsigned long timeShowOctValue;
extern unsigned long tiempoActualMillis;
extern unsigned long ultimoTiempoBotonMute[];
extern unsigned long TiempoPulsadoBotonMute[];
extern bool mute;
extern int menuActual;
extern bool updateLCD;

extern int escalaSeleccionada;
extern const int escalasNotas[5][13];
extern const int nNotasEscalas[];
extern int octava;

//declaracion de funciones
void checkPotes();
void checkMutes();


void checkMutes(){
  for(int i = 0; i < numMutes; i++){
    mute = digitalRead(mutesPin[i]);
    if(mute){
      if(mutePulsado[i] == false){
        mutePulsado[i] = true;
        TiempoPulsadoBotonMute[i] = tiempoActualMillis;
        indiceMovedMute = i;
      }
      if(mutePulsado[0] && mutePulsado[1] && octava < 9 && tiempoActualMillis - ultimoTiempoBotonMute[0] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[1] > 250){
        octava ++;
        ultimoTiempoBotonMute[0] = tiempoActualMillis;
        ultimoTiempoBotonMute[1] = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        menuActual = 4;
        updateLCD = true;
      }
      else if(mutePulsado[2] && mutePulsado[3] && octava > 0 && tiempoActualMillis - ultimoTiempoBotonMute[2] > 250 && tiempoActualMillis - ultimoTiempoBotonMute[3] > 250){
        octava --;
        ultimoTiempoBotonMute[2] = tiempoActualMillis;
        ultimoTiempoBotonMute[3] = tiempoActualMillis;
        timeShowOctValue = tiempoActualMillis;
        menuActual = 4;
        updateLCD = true;
      }
      else if(tiempoActualMillis - TiempoPulsadoBotonMute[i] > 100 && tiempoActualMillis - ultimoTiempoBotonMute[i] > 250){
        mutesValue[i] = !mutesValue[i];
        ultimoTiempoBotonMute[i] = tiempoActualMillis;
        updateLCD = true;
      }
    }
    else{
      mutePulsado[i] = false;
    }
  }
}



void checkPotes() {
  for(int i = 0; i < numPotes; i++){
    int indice = constrain(map(analogRead(potesPin[i]), 1018, 30, 0, nNotasEscalas[escalaSeleccionada] - 1), 0, nNotasEscalas[escalaSeleccionada] - 1);
    int val = escalasNotas[escalaSeleccionada][indice];
    potesValue[i] = val;
    if (abs(val - potSaveValue[i]) > 0.5) { //Solo activamos cuando sea mayor de 0.5;
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