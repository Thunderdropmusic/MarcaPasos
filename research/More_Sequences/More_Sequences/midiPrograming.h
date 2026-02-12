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
extern const int numPotes;

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
extern bool cambioModo0;
extern bool cambioModo1;
extern bool cambioModo2;
extern int indComplexSubdivY;
extern int indComplexSubdivX;

//Tabla de subdivisiones
extern const int subdivisionesArray[];
extern const int subdivisionesComplejasArray[3][6];

//bool control
extern bool play;
static bool primerPulso; 
extern bool primeraMedicionSubCompleja;
static volatile bool notaFuera;

//variables MIDI y notas
static int tipoMsgMidi; 
static int nStep;
static int note;
static int notaTocada;
static int lecturaPulsoClock;

//variables LCD
extern bool updateLCD;

//Declaracion de funciones
void modo01Subdivision();
void modo2Subdivision();
void dispararNotaMode2();
void midiClock();
void midiNotesOff();
void midiNotesOn();
void initMode();


void modo01Subdivision(){
  if (subdivision < 16 || subdivision == 24){
    if(pulsoClock == 24){pulsoClock = 0, subdivision = subdivisionesArray[indiceSubdivisiones], nClockMsg = 0;}
  }
  else{
    if(pulsoClock == 96){pulsoClock = 0, subdivision = subdivisionesArray[indiceSubdivisiones], nClockMsg = 0;}
  }
  if(cambioModo2 && pulsoClock == 0){
    subDivMode = 2;
    initMode();
    cambioModo2= false;
    updateLCD = true;
    modo2Subdivision();
    return;
  }
  nClockMsg++;
  pulsoClock++;
  if (nClockMsg == subdivision){
    decay =  10*subdivision;
    nClockMsg = 0;
    nStep ++;
    if (nStep > numPotes - 1){
      nStep = 0;
    }
  }
}

void modo2Subdivision(){
    if(cambioModo0 && pulsoClock == 0){
      subDivMode = 0;
      initMode();
      cambioModo0= false;
      updateLCD = true;
      modo01Subdivision();
      return;
    }
    if(cambioModo1 && pulsoClock == 0){
      subDivMode = 1;
      initMode();
      cambioModo1= false;
      updateLCD = true;
      modo01Subdivision();
      return;
    }
    nClockMsg++;
  if(pulsoClock == 0){
    subdivision = subdivisionesComplejasArray[indComplexSubdivY][indComplexSubdivX];
    tiempoClock1  = micros();
    if (tiempoClock2 != 0 && nClockMsg > 1) {
      sumaTiempos = tiempoClock1 - tiempoClock2;
      if(primeraMedicionSubCompleja){microsSubdivision = sumaTiempos*4 / subdivision;}
      else{microsSubdivision = sumaTiempos / subdivision;}
      tiempoUltimaNota = tiempoClock1;
      primerPulso = true;
      primeraMedicionSubCompleja = false;
    }
    tiempoClock2 = tiempoClock1;
  } 
  pulsoClock++;
  if(pulsoClock >= lecturaPulsoClock){
    pulsoClock = 0;
    lecturaPulsoClock = 96;
  }
}

void dispararNotaMode2(){ 
  notaFuera = true;
  MIDI.sendNoteOn(note,127,1);
  decay = 0.5*(microsSubdivision/1000);
  notaTocada = note;
  timeDecayNote = millis();
  nStep ++;
  if (nStep > numPotes - 1){
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
    primeraMedicionSubCompleja = true;
    nStep = 0;
    if(subDivMode == 2){
      microsSubdivision = 0;
    }
    else{subdivision = subdivisionesArray[indiceSubdivisiones];}
  }
  else if(tipoMsgMidi == 0xF8){
    if(subDivMode == 0 || subDivMode == 1){
      modo01Subdivision();
    }
    else if(subDivMode == 2){
      modo2Subdivision();
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
  }
}
void midiNotesOn(){
  note = potesValue[nStep]; // el pote controla la nota del midi
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

void initMode(){
  nClockMsg = 0;
  pulsoClock = 0;
  tiempoClock1 = 0;
  tiempoClock2 = 0;
  lecturaPulsoClock = 24;
  primeraMedicionSubCompleja = true;
  nStep = 0;
  microsSubdivision = 0;
}

#endif