#include <Arduino.h>
#include <MIDI.h>
#include "MidiProgramming.h"
#include "menus_buttons.h"
#include "midiInterface.h"
#include "midiPresets.h"
#include "draw_menus.h"


// Declaración de la instancia
extern midi::MidiInterface<midi::SerialMIDI<HardwareSerial, HairlessConfig>> MIDI;

MidiProgramming midiProg[5] = {
    MidiProgramming(0), 
    MidiProgramming(1), 
    MidiProgramming(2), 
    MidiProgramming(3), 
    MidiProgramming(4)
};
byte MidiProgramming::tipoMsgMidi = 0;
byte MidiProgramming::modeMidiClock = 0;
bool MidiProgramming::flagArmed[5] = {false, false, false, false, false};

MidiProgramming::MidiProgramming(byte _id) : 
  id(_id),
  subdivisionesArray{3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96},
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
  microsSubdivision[id] = 50000000;
  tempo = 120;
  tempoMicros = int(60000000/(tempo*24.0));
}

void MidiProgramming::midiSeq(){
  if(tipoMsgMidi == 0xFA){ //Start
    if(modeMidiClock == 1){
      MIDI.sendRealTime(midi::Start);
    }
    notaFuera = false;
    play = true;
    nClockMsg = 0;
    pulsoClock = 0;
    tiempoClock1[id] = 0;
    tiempoClock2[id] = 0;
    lecturaPulsoClock[id] = 24;
    primeraMedicionSubCompleja[id] = true;
    nStep = 0;
    if(p->nSequence[id].subdivMode == 2){
      microsSubdivision[id] = 0;
    }
    else{subdivision[id] = subdivisionesArray[p->nSequence[id].indexSubdivisiones];
    }
  }
  else if(tipoMsgMidi == 0xF8){
    if(p->nSequence[id].subdivMode == 0 || p->nSequence[id].subdivMode == 1){
      modo01Subdivision();
    }
    else if(p->nSequence[id].subdivMode == 2){
      modo2Subdivision();
    }
  }
  else if(tipoMsgMidi == 0xFC){ //Stop
    play = false;
    if(modeMidiClock == 1){MIDI.sendRealTime(midi::Stop);}
  }
}

void MidiProgramming::initMode(){
  nClockMsg = 0;
  pulsoClock = 0;
  tiempoClock1[id] = 0;
  tiempoClock2[id] = 0;
  lecturaPulsoClock[id] = 24;
  primeraMedicionSubCompleja[id] = true;
  nStep = 0;
  microsSubdivision[id] = 0;
}

void MidiProgramming::modo01Subdivision(){
  if (subdivision[id] < 16 || subdivision[id] == 24){
    if(pulsoClock == 24){
      pulsoClock = 0, subdivision[id] = subdivisionesArray[p->nSequence[id].indexSubdivisiones], nClockMsg = 0;
      if(flagArmed[id]){
        p->nSequence[id].armed = true;
        flagArmed[id] = false;
      }
    }
  }
  else{
    if(pulsoClock == 96){
      pulsoClock = 0, subdivision[id] = subdivisionesArray[p->nSequence[id].indexSubdivisiones], nClockMsg = 0;
      if(flagArmed[id]){
        p->nSequence[id].armed = true;
        flagArmed[id] = false;
      }
    }
  }
  if(menusUI.cambioModo2[id] && pulsoClock == 0){
    p->nSequence[id].subdivMode = 2;
    initMode();
    menusUI.cambioModo2[id] = false;
    drawUI.updateLCD = true;
    modo2Subdivision();
    return;
  }
  nClockMsg++;
  pulsoClock++;
  if (nClockMsg == subdivision[id]){
    decay = 10*subdivision[id];
    nClockMsg = 0;
    nStep ++;
    if (nStep > p->nSequence[id].nTotalSteps - 1){
      nStep = 0;
    }
  }
}

void MidiProgramming::modo2Subdivision(){
    if(menusUI.cambioModo0[id] && pulsoClock == 0){
      p->nSequence[id].subdivMode = 0;
      initMode();
      menusUI.cambioModo0[id] = false;
      drawUI.updateLCD = true;
      modo01Subdivision();
      return;
    }
    if(menusUI.cambioModo1[id] && pulsoClock == 0){
      p->nSequence[id].subdivMode = 1;
      initMode();
      menusUI.cambioModo1[id] = false;
      drawUI.updateLCD = true;
      modo01Subdivision();
      return;
    }
    nClockMsg++;
  if(pulsoClock == 0){
    subdivision[id] = subdivisionesComplejasArray[p->nSequence[id].indComplexSubdivY][p->nSequence[id].indComplexSubdivX];
    tiempoClock1[id]  = micros();
    if (tiempoClock2[id] != 0 && nClockMsg > 1) {
      sumaTiempos[id] = tiempoClock1[id] - tiempoClock2[id];
      if(primeraMedicionSubCompleja[id]){microsSubdivision[id] = sumaTiempos[id]*4 / subdivision[id];}
      else{microsSubdivision[id] = sumaTiempos[id] / subdivision[id];}
      tiempoUltimaNota[id] = tiempoClock1[id];
      primerPulso[id] = true;
      primeraMedicionSubCompleja[id] = false;
    }
    tiempoClock2[id] = tiempoClock1[id];
  } 
  pulsoClock++;
  if(pulsoClock >= lecturaPulsoClock[id]){
    pulsoClock = 0;
    lecturaPulsoClock[id] = 96;
  }
}

void MidiProgramming::midiNotesOn(){
 //el pote controla la nota del midi
  if(play == true && !notaFuera && (p->nSequence[id].subdivMode == 0 || p->nSequence[id].subdivMode == 1)){
    if (nClockMsg == 0){ 
      notaFuera = true;
      if(!p->nSequence[id].steps[nStep].mutes){
        MIDI.sendNoteOn(p->nSequence[id].steps[nStep].note + (12 * p->nSequence[id].steps[nStep].octave), p->nSequence[id].steps[nStep].velocity, id + 1);
        notaTocada[id] = p->nSequence[id].steps[nStep].note;
      }
      timeDecayNote = millis();
    }
  }
  else if(play == true && p->nSequence[id].subdivMode == 2){
    if(primerPulso[id]){
      tiempoUltimaNota[id] = tiempoClock1[id];
      primerPulso[id] = false; 
      dispararNotaMode2();
    }
    else if (microsSubdivision[id] >= 10000 && (tiempoActualMicros - tiempoUltimaNota[id] > microsSubdivision[id])){
      unsigned long tiempoDesdeInicioRedonda = tiempoActualMicros - tiempoClock1[id];
      unsigned long duracionRedondaTeorica = microsSubdivision[id] * subdivision[id];
      if (tiempoDesdeInicioRedonda < (duracionRedondaTeorica - 15000)) {
        tiempoUltimaNota[id] += microsSubdivision[id];
        dispararNotaMode2();
      }
    }
  }
}


void MidiProgramming::dispararNotaMode2(){ 
  notaFuera = true;
  if(!p->nSequence[id].steps[nStep].mutes){
    MIDI.sendNoteOn(p->nSequence[id].steps[nStep].note + (12 * p->nSequence[id].steps[nStep].octave), p->nSequence[id].steps[nStep].velocity, id + 1);
  }
  decay = 0.5*(microsSubdivision[id]/1000);
  notaTocada[id] = p->nSequence[id].steps[nStep].note;
  timeDecayNote = millis();
  nStep ++;
  if (nStep > p->nSequence[id].nTotalSteps - 1){
    nStep = 0;
  }
}

void MidiProgramming::midiNotesOff(){
  if (notaFuera && (millis() - timeDecayNote >= decay)){
    MIDI.sendNoteOff(notaTocada[id] + (12 * p->nSequence[id].steps[nStep].octave), 0, id + 1);
    notaFuera = false;
    timeDecayNote = 0;
  }
}

float MidiProgramming::ccCurveFunction(int i, float subdivisionActual, float tension){
  return pow((float(i)/subdivisionActual), tension);
}

void MidiProgramming::CCSend(){
  if(!play) return;
  else if (tipoMsgMidi != 0xF8 && tipoMsgMidi != 0xFA) return;
  
  if (nClockMsg == 0){
    nMsgCC[id] = 0;
    float valorInicial = p->nSequence[id].steps[0].ccValue;
    float valorActual = p->nSequence[id].steps[nStep].ccValue;
    float subdivisionActual = subdivision[id];
    if(nStep + 1 < p->nSequence[id].nTotalSteps){
      float valorSiguiente = p->nSequence[id].steps[nStep + 1].ccValue;
      float ccStepSize = (valorSiguiente  - valorActual);
      for(int i = 0; i < subdivision[id]; i++){
        CCinterpolation[id][i] = p->nSequence[id].steps[nStep].ccValue + int(ccStepSize * ccCurveFunction(i, subdivisionActual, p->nSequence[id].steps[nStep].ccSmooth));
      }
    }
    else{    
      float ccStepSize = (valorInicial - valorActual);
      for(int i = 0; i < subdivision[id]; i++){
        CCinterpolation[id][i] = p->nSequence[id].steps[nStep].ccValue + int(ccStepSize * ccCurveFunction(i, subdivisionActual, p->nSequence[id].steps[nStep].ccSmooth));
      }
    }
  }
  else{nMsgCC[id]++;}
  MIDI.sendControlChange(p->nSequence[id].ccNumber, CCinterpolation[id][nMsgCC[id]], id + 1);
}



