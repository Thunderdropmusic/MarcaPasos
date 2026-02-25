   //asignar variable led como 13


void ONOFF(){
  digitalWrite(pinLED, HIGH);   // encender LED
  delay(1000);                  // esperar un segundo
  digitalWrite(pinLED, LOW);    // apagar LED
  delay(1000);                  // esperar un segundo
}
