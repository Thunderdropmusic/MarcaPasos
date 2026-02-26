const byte potesPin[8] = {A0, A1, A2, A3, A4, A5, A6, A7/*, A8, A9, A10, A11, A12, A13, A14, A15*/};
int val[8];
int valor[8];
bool poteMovido;
unsigned long tiempoActual;
unsigned long tiempoPoteMovido;
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 8; i++) {
    valor[i] = analogRead(potesPin[i]);
    Serial.print("pote movido Nª: ");
    Serial.print(i);
    Serial.print(" Valores: ");
    Serial.print(val[i]);
    Serial.print("----->");
    Serial.println(valor[i]);
    val[i] = valor[i];
  }
}

void loop() {
  tiempoActual = millis();
  for (int i = 0; i < 8; i++) {
    valor[i] = analogRead(potesPin[i]);
    if(!poteMovido){
      if(abs(valor[i] - val[i]) > 16){
        poteMovido = true;
        tiempoPoteMovido = tiempoActual;
        cambiarValorPote(i);
      }
    }
    else if(poteMovido && abs(valor[i] - val[i]) > 7){cambiarValorPote(i);}
  }
  if(tiempoActual - tiempoPoteMovido > 500){
    poteMovido = false;
  }
}
void cambiarValorPote(int indiceMovido){
  Serial.print("pote movido Nª: ");
  Serial.print(indiceMovido);
  Serial.print(" Valores: ");
  Serial.print(val[indiceMovido]);
  Serial.print("----->");
  Serial.println(valor[indiceMovido]);
  val[indiceMovido] = valor[indiceMovido];
}
