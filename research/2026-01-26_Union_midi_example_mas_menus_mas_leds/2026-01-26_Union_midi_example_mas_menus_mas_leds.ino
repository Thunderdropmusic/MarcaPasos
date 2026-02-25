void midiClock(){
  tipo = MIDI.getType();
  if(tipo == 0xFA){ //Start
    notaFuera = false;
    boolean play = true;
    tempo = 0;
  }
  else if(tipo == 0xF8){
    if (tempo == 0){ 
      notaFuera = true;
      MIDI.sendNoteOn(note,127,1);
      notaTocada = note;
      timeDecayNote = millis();
    }
    if (pulso == 0){
      newSubdivision = subdivision;
    }
    tempo++;
    pulso++;
    if (tempo >= 3*newSubdivision){
      decay = 31*newSubdivision;
      tempo = 0;
      nStep ++;
      digitalWrite(pinLeds[0],LOW);
      digitalWrite(pinLeds[1],LOW);
      digitalWrite(pinLeds[2],LOW);
      if (nStep > 2){
        nStep = 0;
      }
    }
    if (pulso == 24){
      pulso = 0;
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
  digitalWrite(pinLeds[nStep],HIGH);
}