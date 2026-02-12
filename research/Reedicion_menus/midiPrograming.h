#ifndef MIDI_PROGRAMING_H
#define MIDI_PROGRAMING_H

#include <Arduino.h>
#include <MIDI.h>

// Configuracion libreria MIDI
struct HairlessConfig : public midi::DefaultSettings {
  static const long BaudRate = 115200;
};
// Declaración de la instancia
extern midi::MidiInterface<midi::SerialMIDI<HardwareSerial, HairlessConfig>> MIDI;

//Valores potenciometros
extern int potSaveValue[];
extern int potesValue[];

//Variables de tiempo real
static unsigned long int tiempoClock1, tiempoClock2; 
static unsigned long int tiempoUltimaNota; 
static unsigned long int sumaTiempos; 
static unsigned long int timeDecayNote;
extern unsigned long int tiempoActualMicros;
static unsigned long int decay = 250;

//Control de pulso y mensajes
static int pulsoClock;
static int nClockMsg;

//Variables subdivisiones
extern unsigned long int microsSubdivision;
extern int subdivision; 
extern int indiceSubdivisiones;
extern int subDivMode;
extern int indComplexSubdivY;
extern int indComplexSubdivX;

//Tabla de subdivisiones
extern const int subdivisionesArray[];
extern const int subdivisionesComplejasArray[3][6];

//bool control
bool play; 
static bool primerPulso; 
static bool primeraMedicion;
static volatile bool notaFuera;

//variables MIDI y notas
static int tipoMsgMidi; 
static int nStep;
static int note;
static int notaTocada;
static int lecturaPulsoClock;

//variables LCD
extern bool updateLCD;

void dispararNotaMode2(){ 
  notaFuera = true;
  MIDI.sendNoteOn(note,127,1);
  decay = 0.5*(microsSubdivision/1000);
  notaTocada = note;
  timeDecayNote = millis();
  nStep ++;
  if (nStep > 2){
    nStep = 0;
  }
}

void midiClock(){
  tipoMsgMidi = MIDI.getType();
  if(tipoMsgMidi == 0xFA){ //Start
    notaFuera = false;
    play = true;
    nClockMsg = 0;
    pulsoClock = 0;
    tiempoClock1 = 0;
    tiempoClock2 = 0;
    lecturaPulsoClock = 24;
    primeraMedicion = true;
    if(subDivMode == 2){
      microsSubdivision = 0;
    }
    else{subdivision = subdivisionesArray[indiceSubdivisiones];}
  }
  else if(tipoMsgMidi == 0xF8){
    if(subDivMode == 0 || subDivMode == 1){
      if (subdivision <= 24){
        if(pulsoClock == 24){pulsoClock = 0, subdivision = subdivisionesArray[indiceSubdivisiones];}
      }
      else{
        if(pulsoClock == 96){pulsoClock = 0, subdivision = subdivisionesArray[indiceSubdivisiones];}
      }
      nClockMsg++;
      pulsoClock++;
      if (nClockMsg == subdivision){
        decay =  10*subdivision;
        nClockMsg = 0;
        nStep ++;
        if (nStep > 2){
          nStep = 0;
        }
      }
    }

    else if(subDivMode == 2){
      
      nClockMsg++;
      if(pulsoClock == 0){
        subdivision = subdivisionesComplejasArray[indComplexSubdivY][indComplexSubdivX];
        tiempoClock1  = micros();
        if (tiempoClock2 != 0 && nClockMsg > 1) {
          sumaTiempos = tiempoClock1 - tiempoClock2;
          if(primeraMedicion){microsSubdivision = sumaTiempos*4 / subdivision;}
          else{microsSubdivision = sumaTiempos / subdivision;}
          tiempoUltimaNota = tiempoClock1;
          primerPulso = true;
          primeraMedicion = false;
          
        }
        updateLCD = true;
        tiempoClock2 = tiempoClock1;
      } 
      pulsoClock++;
      if(pulsoClock >= lecturaPulsoClock){
        pulsoClock = 0;
        lecturaPulsoClock = 96;
      }

    }
  }
  else if(tipoMsgMidi == 0xFC){ //Stop
    play = false;
  }
}
void midiNotesOff(){
  if (notaFuera && (millis() - timeDecayNote >= decay)){
    MIDI.sendNoteOff(notaTocada,127,1);
    notaFuera = false;
    timeDecayNote = 0;
    note = potesValue[nStep];
  }
}
void midiNotesOn(){
 //el pote controla la nota del midi
  if(play == true && !notaFuera && (subDivMode == 0 || subDivMode == 1)){
    if (nClockMsg == 0){ 
      notaFuera = true;
      MIDI.sendNoteOn(note,127,1);
      notaTocada = note;
      timeDecayNote = millis();
    }
  }
  else if(play == true && subDivMode == 2){
    if(primerPulso){
      tiempoUltimaNota = tiempoClock1;
      primerPulso = false; 
      dispararNotaMode2();
    }
    else if (microsSubdivision >= 10000 && (tiempoActualMicros - tiempoUltimaNota > microsSubdivision)){
      unsigned long tiempoDesdeInicioRedonda = tiempoActualMicros - tiempoClock1;
      unsigned long duracionRedondaTeorica = microsSubdivision * subdivision;
      if (tiempoDesdeInicioRedonda < (duracionRedondaTeorica - 15000)) {
          tiempoUltimaNota += microsSubdivision;
          dispararNotaMode2();
      }
    }
  }
}

#endif