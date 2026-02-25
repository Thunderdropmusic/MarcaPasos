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
extern bool mutePulsado;
extern int val, movedPoteValue, movedPoteNumber;
extern unsigned long timeShowPotValue;
extern unsigned long tiempoActualMillis;
extern unsigned long ultimoTiempoBoton;
extern bool mute;
extern int menuActual;
extern bool updateLCD;

//declaracion de funciones
void checkPotes();
void checkMutes();


void checkMutes(){
  for(int i = 0; i < 2; i++){
    mute = digitalRead(mutesPin[i]);
    if(mute){
      if(mutePulsado == false && tiempoActualMillis - ultimoTiempoBoton > 250){
        mutesValue[i] = !mutesValue[i];
        mutePulsado = true;
        ultimoTiempoBoton = tiempoActualMillis;
        updateLCD = true;
      }
    }
    else if(!mute){
      mutePulsado = false;
    }
  }
}



void checkPotes() {
  for(int i = 0; i < numPotes; i++){
    int val = constrain(map(analogRead(potesPin[i]), 1018, 30, 48, 61), 48, 60);
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