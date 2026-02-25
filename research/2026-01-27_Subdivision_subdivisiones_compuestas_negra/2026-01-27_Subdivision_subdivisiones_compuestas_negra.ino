void midiClock(){
  tipoMsgMidi = MIDI.getType();
  if(tipoMsgMidi == 0xFA){ //Start
    notaFuera = false;
    play = true;
    tempo = 0;
    pulsoClock = 0;
    tiempoClock1 = 0;
    tiempoClock2 = micros();
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
      tempo++;
      pulsoClock++;
      if (tempo == subdivision){
        decay =  10*subdivision;
        tempo = 0;
        nStep ++;
        if (nStep > 2){
          nStep = 0;
        }
      }
    }

    else if(subDivMode == 2){
      subdivision = 7;
      if(pulsoClock == 0){
        tiempoClock1  = micros();
        if (tiempoClock2 != 0) {
          sumaTiempos = tiempoClock1 - tiempoClock2;
          microsSubdivision = sumaTiempos / subdivision;
        }
        updateLCD = true;
        tiempoClock2 = tiempoClock1;
      } 
      pulsoClock++;
      if(pulsoClock >= 24){
        pulsoClock = 0;
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
  }
  note = potesValue[nStep]; //el pote controla la nota del midi
}
void midiNotesOn(){
  if(subDivMode == 0 || subDivMode == 1){
    if (tempo == 0){ 
      notaFuera = true;
      MIDI.sendNoteOn(note,127,1);
      notaTocada = note;
      timeDecayNote = millis();
    }
  }
  else if(play == true && subDivMode == 2){
    if (microsSubdivision >= 5000 && (tiempoActualMicros - tiempoUltimaNota > microsSubdivision)){
      tiempoUltimaNota = micros();
      notaFuera = true;
      MIDI.sendNoteOn(note,127,1);
      decay = 0.5*(microsSubdivision/1000);
      notaTocada = note;
      timeDecayNote = millis();
      tempo = 1;
      nStep ++;
      if (nStep > 2){
        nStep = 0;
      }
    }
  }
}