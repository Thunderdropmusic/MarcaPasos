void midiClock(){
  tipo = MIDI.getType();
  if(tipo == 0xFA){ //Start
    notaFuera = false;
    boolean play = true;
    tempo = 0;
    pulso = 0;
    newSubdivision = subdivision;
  }
  else if(tipo == 0xF8){
    if (tempo == 0){ 
      notaFuera = true;
      MIDI.sendNoteOn(note,127,1);
      notaTocada = note;
      timeDecayNote = millis();
    }
    if (newSubdivision <= 24){
      if(pulso == 24){pulso = 0, newSubdivision = subdivision;}
    }
    else{
      if(pulso == 96){pulso = 0, newSubdivision = subdivision;}
    }
    tempo++;
    pulso++;
    if (tempo == newSubdivision){
      decay =  10*newSubdivision;
      tempo = 0;
      nStep ++;
      if (nStep > 2){
        nStep = 0;
      }
    }
  } 
  else if(tipo == 0xFC){ //Stop
    boolean play = false;
  }
}
void midiNotes(){
  if (notaFuera && (millis() - timeDecayNote >= decay)){
    MIDI.sendNoteOff(notaTocada,127,1);
    notaFuera = false;
    timeDecayNote = 0;
  }
  note = potesValue[nStep]; //el pote controla la nota del midi
}