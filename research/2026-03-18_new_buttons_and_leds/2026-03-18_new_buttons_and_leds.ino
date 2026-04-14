


void setup() {
  // put your setup code here, to run once:
  pinMode(seq1PinButton, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  if(seq1Button){
    digitalWrite(seq1PinLed, HIGH);
  }
  else{
    digitalWrite(seq1PinLed, LOW);
  }
    if(seq2Button){
    digitalWrite(seq2PinLed, HIGH);
  }
  else{
    digitalWrite(seq2PinLed, LOW);
  }
    if(seq3Button){
    digitalWrite(seq3PinLed, HIGH);
  }
  else{
    digitalWrite(seq3PinLed, LOW);
  }
  if(seq4Button){
    digitalWrite(seq4PinLed, HIGH);
  }
  else{
    digitalWrite(seq4PinLed, LOW);
  }
  if(seq5Button){
    digitalWrite(seq5PinLed, HIGH);
  }
  else{
    digitalWrite(seq5PinLed, LOW);
  }

}
