void midiClock(){
  tipoMsgMidi = MIDI.getType();
  if(tipoMsgMidi == 0xFA){ //Start
    notaFuera = false;
    play = true;
    tempo = 0;
    pulsoClock = 0;
    totalInitClocks = 23;
    subdivision = subdivisionesArray[indiceSubdivisiones];
    tiempoClock1 = 0;
    tiempoClock2 = micros();
  }
  else if(tipoMsgMidi == 0xF8){
    if(subDivMode == 0 || subDivMode == 1){
      if (tempo == 0){ 
        notaFuera = true;
        MIDI.sendNoteOn(note,127,1);
        notaTocada = note;
        timeDecayNote = millis();
      } 
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
      subdivision = 4;
      int pulsosRecibidos = 24 -totalInitClocks;
      if(pulsoClock == 24){

        pulsoClock = 0;
        updateLCD = true;
      }

      tiempoClock1  = micros();
      tiempoClock[pulsoClock] = tiempoClock1 - tiempoClock2;
      sumaTiempos = 0;
      for(int i = 0; i < pulsosRecibidos; i++){
        sumaTiempos += tiempoClock[i];    
      }
      mediaActual = sumaTiempos / pulsosRecibidos;
      mediaEstimada = mediaActual*24;
      microsSubdivision = mediaEstimada / subdivision;
      if(pulsoClock == 0){
        tempo = 0;
        tiempoUltimaNota = micros();
        nStep ++;
        if (nStep > 2){nStep = 0;}
      }
      else if(tempo == 1 && (micros() - tiempoUltimaNota >= microsSubdivision)){
        tiempoUltimaNota += microsSubdivision;
        tempo = 0;
        nStep ++;
        if (nStep > 2){nStep = 0;}
      }
      if(pulsoClock >= 23){ // El pulso 24 es el índice 23
        pulsoClock = 0;
        totalInitClocks = 23; // Reseteamos para que el for empiece de nuevo
        updateLCD = true;
      } 
      else {
        pulsoClock++;
      }
      if(totalInitClocks != 0){
        totalInitClocks--;
      }
      tiempoClock2 = tiempoClock1;
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
  if(subDivMode == 2 && play){ 
    if (tempo == 0 && microsSubdivision > 1000){
      notaFuera = true;
      MIDI.sendNoteOn(note,127,1);
      decay = 0.5*(microsSubdivision/1000);
      notaTocada = note;
      timeDecayNote = millis();
      tempo = 1;
    } 
  }
}