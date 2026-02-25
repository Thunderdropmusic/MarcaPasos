const int pinLED= 13;    //asignar variable led como 13

void setup()
{
  pinMode(pinLED, OUTPUT);     //definir pin 13 como salida  
}

void loop() {
  digitalWrite(pinLED, HIGH);   // encender LED
  delay(1000);                  // esperar un segundo
  digitalWrite(pinLED, LOW);    // apagar LED
  delay(1000);                  // esperar un segundo
}
