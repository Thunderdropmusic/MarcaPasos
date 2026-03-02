#include <Arduino.h>
#include <MIDI.h>
#include "MidiProgramming.h"
#include "menus_buttons.h"
#include "midiInterface.h"
#include "midiPresets.h"
#include "globalVars.h"


// Declaración de la instancia
extern midi::MidiInterface<midi::SerialMIDI<HardwareSerial, HairlessConfig>> MIDI;

MidiProgramming midiProg;

MidiProgramming::MidiProgramming() :

  subdivisionesArray{2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96},
  subdivisionesComplejasArray{
    {5, 7, 9, 11, 13, 15},
    {10, 14, 18, 22, 26, 30},
    {20, 28, 36, 0, 0, 0,}
  },

  escalasNotas{
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, //Escala cromática
    {0, 2, 3, 5, 7, 8, 10, 12, 0, 0, 0, 0, 0}, //Escala menor Natural
    {0, 2, 4, 5, 7, 9, 11, 12, 0, 0, 0, 0, 0}, //Escala Mayor
    {0, 2, 4, 6, 8, 10, 12, 0, 0, 0, 0, 0, 0}, //Escala de tonos
    {0, 1, 3, 4, 6, 7, 9, 10, 12, 0, 0, 0, 0} //Escala Semitono-tono
  },
  nNotasEscalas{13, 8, 8, 7, 9}
  //variables MIDI y notas
{
  decay = 250;
  microsSubdivision = 50000000;
  modeMidiClock = 0;
  tempo = 120;
  tempoMicros = int(60000000/(tempo*24.0));
}


void MidiProgramming::modo01Subdivision(){
  if (subdivision < 16 || subdivision == 24){
    if(pulsoClock == 24){pulsoClock = 0, subdivision = subdivisionesArray[globalIndexSubdivisiones], nClockMsg = 0;}
  }
  else{
    if(pulsoClock == 96){pulsoClock = 0, subdivision = subdivisionesArray[globalIndexSubdivisiones], nClockMsg = 0;}
  }
  if(menusUI.cambioModo2 && pulsoClock == 0){
    globalSubdivMode = 2;
    initMode();
    menusUI.cambioModo2= false;
    drawUI.updateLCD = true;
    modo2Subdivision();
    return;
  }
  nClockMsg++;
  pulsoClock++;
  if (nClockMsg == subdivision){
    decay = 10*subdivision;
    nClockMsg = 0;
    nStep ++;
    if (nStep > globalNTotalSteps - 1){
      nStep = 0;
    }
  }
}

void MidiProgramming::modo2Subdivision(){
    if(menusUI.cambioModo0 && pulsoClock == 0){
      globalSubdivMode = 0;
      initMode();
      menusUI.cambioModo0= false;
      drawUI.updateLCD = true;
      modo01Subdivision();
      return;
    }
    if(menusUI.cambioModo1 && pulsoClock == 0){
      globalSubdivMode = 1;
      initMode();
      menusUI.cambioModo1= false;
      drawUI.updateLCD = true;
      modo01Subdivision();
      return;
    }
    nClockMsg++;
  if(pulsoClock == 0){
    subdivision = subdivisionesComplejasArray[globalIndComplexSubdivY][INDEX_COMPLEX_X()];
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

void MidiProgramming::dispararNotaMode2(){ 
  notaFuera = true;
  if(!globalMutes[nStep]){
    MIDI.sendNoteOn(globalNote[nStep] + (12 * globalOctave[nStep]), globalVelocity[nStep], globalCanal);
  }
  decay = 0.5*(microsSubdivision/1000);
  notaTocada = globalNote[nStep];
  timeDecayNote = millis();
  nStep ++;
  if (nStep > globalNTotalSteps - 1){
    nStep = 0;
  }
}

void MidiProgramming::midiSeq(){
  if(midiProg.modeMidiClock == 0){tipoMsgMidi = MIDI.getType();}  
  if(tipoMsgMidi == 0xFA){ //Start
    if(modeMidiClock == 1){
      MIDI.sendRealTime(midi::Start);
    }
    notaFuera = false;
    play = true;
    nClockMsg = 0;
    pulsoClock = 0;
    tiempoClock1 = 0;
    tiempoClock2 = 0;
    lecturaPulsoClock = 24;
    primeraMedicionSubCompleja = true;
    nStep = 0;
    if(globalSubdivMode == 2){
      microsSubdivision = 0;
    }
    else{subdivision = subdivisionesArray[globalIndexSubdivisiones];
    }
  }
  else if(tipoMsgMidi == 0xF8){
    if(globalSubdivMode == 0 || globalSubdivMode == 1){
      modo01Subdivision();
    }
    else if(globalSubdivMode == 2){
      modo2Subdivision();
    }
  }
  else if(tipoMsgMidi == 0xFC){ //Stop
    play = false;
    if(modeMidiClock == 1){MIDI.sendRealTime(midi::Stop);}
  }
}
void MidiProgramming::midiNotesOff(){
  if (notaFuera && (millis() - timeDecayNote >= decay)){
    MIDI.sendNoteOff(notaTocada + (12 * globalOctave[nStep]), 0, globalCanal);
    notaFuera = false;
    timeDecayNote = 0;
  }
}
void MidiProgramming::midiNotesOn(){
  Sequence &playTrack = marcaPasos.nSequence[presetsUI.indexSequence];
 //el pote controla la nota del midi
  if(play == true && !notaFuera && (globalSubdivMode == 0 || globalSubdivMode == 1)){
    if (nClockMsg == 0){ 
      notaFuera = true;
      if(!globalMutes[nStep]){
        MIDI.sendNoteOn(globalNote[nStep] + (12 * globalOctave[nStep]), globalVelocity[nStep], globalCanal);
      }
      notaTocada = globalNote[nStep];
      timeDecayNote = millis();
    }
  }
  else if(play == true && globalSubdivMode == 2){
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

void MidiProgramming::initMode(){
  nClockMsg = 0;
  pulsoClock = 0;
  tiempoClock1 = 0;
  tiempoClock2 = 0;
  lecturaPulsoClock = 24;
  primeraMedicionSubCompleja = true;
  nStep = 0;
  microsSubdivision = 0;
}
void MidiProgramming::midiClock(){
  
}