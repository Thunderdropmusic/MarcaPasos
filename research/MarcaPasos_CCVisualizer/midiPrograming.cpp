#include <Arduino.h>
#include <MIDI.h>
#include "MidiProgramming.h"
#include "menus_buttons.h"
#include "midiInterface.h"
#include "midiPresets.h"
#include "draw_menus.h"
#include "config.h"


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

const byte MidiProgramming::escalasNotas[5][13]{
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}, //Escala cromática
  {0, 2, 3, 5, 7, 8, 10, 12, 0, 0, 0, 0, 0}, //Escala menor Natural
  {0, 2, 4, 5, 7, 9, 11, 12, 0, 0, 0, 0, 0}, //Escala Mayor
  {0, 2, 4, 6, 8, 10, 12, 0, 0, 0, 0, 0, 0}, //Escala de tonos
  {0, 1, 3, 4, 6, 7, 9, 10, 12, 0, 0, 0, 0} //Escala Semitono-tono
};

const byte MidiProgramming::subdivisionesArray[12] {3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96};

const byte MidiProgramming::subdivisionesComplejasArray[3][6] {
  {5, 7, 9, 11, 13, 15},
  {10, 14, 18, 22, 26, 30},
  {20, 28, 36, 0, 0, 0,}
};
const byte MidiProgramming::nNotasEscalas[5]{13, 8, 8, 7, 9};

MidiProgramming::MidiProgramming(byte _id) : 
  id(_id)


  //variables MIDI y notas
{
  decay = 250;
  microsSubdivision = 50000000;
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
    tiempoClock1 = 0;
    tiempoClock2 = 0;
    lecturaPulsoClock = 24;
    primeraMedicionSubCompleja = true;
    nStep = 0;
    if(p->nSequence[id].subdivMode == 2){
      microsSubdivision = 0;
    }
    else{subdivision = subdivisionesArray[p->nSequence[id].indexSubdivisiones];
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
  tiempoClock1 = 0;
  tiempoClock2 = 0;
  lecturaPulsoClock = 24;
  primeraMedicionSubCompleja = true;
  nStep = 0;
  microsSubdivision = 0;
}

void MidiProgramming::modo01Subdivision(){
  if (subdivision < 16 || subdivision == 24){
    if(pulsoClock == 24){
      pulsoClock = 0, subdivision = subdivisionesArray[p->nSequence[id].indexSubdivisiones], nClockMsg = 0;
      if(flagArmed[id]){
        p->nSequence[id].armed = true;
        flagArmed[id] = false;
      }
    }
  }
  else{
    if(pulsoClock == 96){
      pulsoClock = 0, subdivision = subdivisionesArray[p->nSequence[id].indexSubdivisiones], nClockMsg = 0;
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
  if (nClockMsg == subdivision){
    decay = 10*subdivision;
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
    subdivision = subdivisionesComplejasArray[p->nSequence[id].indComplexSubdivY][p->nSequence[id].indComplexSubdivX];
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

void MidiProgramming::midiNotesOn(){
 //el pote controla la nota del midi
  if(play == true && !notaFuera && (p->nSequence[id].subdivMode == 0 || p->nSequence[id].subdivMode == 1)){
    if (nClockMsg == 0){ 
      notaFuera = true;
      if(!p->nSequence[id].steps[nStep].mutes){
        MIDI.sendNoteOn(p->nSequence[id].steps[nStep].note + (12 * p->nSequence[id].steps[nStep].octave), p->nSequence[id].steps[nStep].velocity, id + 1);
        notaTocada = p->nSequence[id].steps[nStep].note;
      }
      timeDecayNote = millis();
    }
  }
  else if(play == true && p->nSequence[id].subdivMode == 2){
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


void MidiProgramming::dispararNotaMode2(){ 
  notaFuera = true;
  if(!p->nSequence[id].steps[nStep].mutes){
    MIDI.sendNoteOn(p->nSequence[id].steps[nStep].note + (12 * p->nSequence[id].steps[nStep].octave), p->nSequence[id].steps[nStep].velocity, id + 1);
  }
  decay = (microsSubdivision/2000);
  notaTocada = p->nSequence[id].steps[nStep].note;
  timeDecayNote = millis();
  nStep ++;
  if (nStep > p->nSequence[id].nTotalSteps - 1){
    nStep = 0;
  }
}

void MidiProgramming::midiNotesOff(){
  if (notaFuera && (millis() - timeDecayNote >= decay)){
    MIDI.sendNoteOff(notaTocada + (12 * p->nSequence[id].steps[nStep].octave), 0, id + 1);
    notaFuera = false;
    timeDecayNote = 0;
  }
}

int MidiProgramming::ccCurveFunction(int i, int subdivisionActual, int tensionMode){
  int indiceLUT = (i * 99) / (subdivisionActual - 1);
  byte factorCurva = pgm_read_byte(&(smoothCurves[tensionMode][indiceLUT]));
  return factorCurva;
}

void MidiProgramming::CCSend(){
  if(!play) return;
  else if (tipoMsgMidi != 0xF8 && tipoMsgMidi != 0xFA) return;
  
  if (nClockMsg == 0){
    nMsgCC[id] = 0;
    int prevStep = nStep;
    while (prevStep > 0 && p->nSequence[id].steps[prevStep].mutes) {
      prevStep--;
    }

    // 2. Definir a dónde vamos (buscando hacia adelante)
    int nextStep = nStep + 1;
    bool foundNext = false;
    while (nextStep < p->nSequence[id].nTotalSteps) {
      if (!p->nSequence[id].steps[nextStep].mutes) {
        foundNext = true;
        break;
      }
      nextStep++;
    }
    int valorActual = p->nSequence[id].steps[prevStep].ccValue;
    int valorSiguiente = 0;
    int distanceSteps = 1;
    if (foundNext) {
      // Lo que antes era tu "if(nStep + 1 < p->nSequence[id].nTotalSteps)"
      valorSiguiente = p->nSequence[id].steps[nextStep].ccValue;
      distanceSteps = nextStep - prevStep;
    }
    else {    
      // Lo que antes era tu "else" (el final del loop)
      int firstStep = 0;
      while (firstStep < p->nSequence[id].nTotalSteps && p->nSequence[id].steps[firstStep].mutes) {
        firstStep++;
      }
      int valorInicial = p->nSequence[id].steps[firstStep].ccValue;
      
      valorSiguiente = valorInicial;
      distanceSteps = (p->nSequence[id].nTotalSteps - prevStep) + firstStep;
    }
    int ccStepSize = (valorSiguiente - valorActual);
    int subdivisionActual = distanceSteps * subdivision; // La curva ahora dura la distancia total
    int offset = (nStep - prevStep) * subdivision;       // Offset para saber en qué paso estamos

    // 5. TU BUCLE FOR (unificado)
    int divisor = (subdivisionActual > 1) ? subdivisionActual : 2; // Protección para la LUT
    
    for(int i = 0; i < subdivision; i++){
      int indexCurva = offset + i; 
      // Usamos tu misma línea de interpolación
      CCinterpolation[i] = valorActual + ccStepSize * ccCurveFunction(indexCurva, divisor, p->nSequence[id].steps[prevStep].ccSmoothCurve) / 255;
    }
  }
  else {
    nMsgCC[id]++;
  }
  
  MIDI.sendControlChange(p->nSequence[id].ccNumber, CCinterpolation[nMsgCC[id]], id + 1);
}



