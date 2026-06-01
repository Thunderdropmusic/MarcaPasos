//YWROBOT
//Compatible with the Arduino IDE 1.0
//Library version:1.1
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

// Encoder configurado en los pines 18 y 19 (Máximo Rendimiento / Interrupciones)
Encoder myEnc(19,17);  

LiquidCrystal_I2C lcd(0x27,20,4);

const int NUM_POTENCIOMETROS = 16;
const int pinesPot[NUM_POTENCIOMETROS] = {
  A0, A1, A2, A3, A4, A5, A6, A7, 
  A8, A9, A10, A11, A12, A13, A14, A15
};

// Definición de grupos de botones
const byte seqButtons[5] = {22,24,26,28,30};
const byte ledButtons[5] = {13,12,11,10,9}; // Salidas (LEDs)
const byte slotButtons[5] = {32,34,36,38,40};
const byte cruceta[4] = {23,25,27,29};
const byte controlButtons[5] = {31,33,35,37,39};
const byte mutesPin[16] = {47, 43, 45, 41, 3, 49, 14, 48,46,44,42,4,5,6,7,8};
const byte encoderOkButton = 18; // Botón OK del encoder

// Arrays para estados anteriores (Evitan el spam en el monitor serie)
bool antSeq[5];
bool antSlot[5];
bool antCruceta[4];
bool antControl[5];
bool antMutes[16]; 
bool antEncoderOk = HIGH;

// Array para almacenar la última lectura de los potenciómetros
int valoresPotAnteriores[NUM_POTENCIOMETROS];

void setup() {
  Serial.begin(9600);
  
  // Inicialización de Botones (HIGH por ser INPUT_PULLUP)
  for(int i = 0; i < 5; i++){
    pinMode(controlButtons[i], INPUT_PULLUP);
    antControl[i] = HIGH;
    
    pinMode(slotButtons[i], INPUT_PULLUP);
    antSlot[i] = HIGH;
    
    pinMode(seqButtons[i], INPUT_PULLUP);
    antSeq[i] = HIGH;
    
    pinMode(ledButtons[i], OUTPUT); 
  }
  
  for(int i = 0; i < 4; i++){
    pinMode(cruceta[i], INPUT_PULLUP);
    antCruceta[i] = HIGH;
  }

  for(int i = 0; i < 16; i++){
    pinMode(mutesPin[i], INPUT_PULLUP);
    antMutes[i] = HIGH; 
  }
  
  pinMode(encoderOkButton, INPUT_PULLUP);

  // Inicialización de lectura de potenciómetros
  for (int i = 0; i < NUM_POTENCIOMETROS; i++) {
    valoresPotAnteriores[i] = analogRead(pinesPot[i]);
  }

  Serial.println("--- CONSOLA DE DIAGNÓSTICO TOTAL ACTIVA ---");
  Serial.println("Mueve cualquier potenciometro o pulsa un boton para testear...");

  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
}

long oldPosition  = -999;

void loop() {
  
  // ==========================================
  // 1. LECTURA INTELIGENTE DE POTENCIÓMETROS
  // ==========================================
  for (int i = 0; i < NUM_POTENCIOMETROS; i++) {
    int valorActual = analogRead(pinesPot[i]);
    
    // Si el potenciómetro se ha movido más de 2 unidades (filtro de ruido)
    if (abs(valorActual - valoresPotAnteriores[i]) > 20) {
      Serial.print("~ Potenciometro ["); 
      Serial.print(i); 
      Serial.print("] CAMBIÓ | Pin: A"); 
      Serial.print(i); 
      Serial.print(" | Valor: "); 
      Serial.println(valorActual);
      
      valoresPotAnteriores[i] = valorActual; // Actualizamos el valor de referencia
    }
  }

  // ==========================================
  // 2. ESCANEO DE LOS GRUPOS DE BOTONES
  // ==========================================
  for(int i = 0; i < 5; i++){
    bool act = digitalRead(seqButtons[i]);
    if (act == LOW && antSeq[i] == HIGH) {
      Serial.print("-> Boton SEQ ["); Serial.print(i); Serial.print("] PRESIONADO | Pin Fisico: "); Serial.println(seqButtons[i]);
    }
    antSeq[i] = act;
  }

  for(int i = 0; i < 5; i++){
    bool act = digitalRead(slotButtons[i]);
    if (act == LOW && antSlot[i] == HIGH) {
      Serial.print("-> Boton SLOT ["); Serial.print(i); Serial.print("] PRESIONADO | Pin Fisico: "); Serial.println(slotButtons[i]);
    }
    antSlot[i] = act;
  }

  for(int i = 0; i < 4; i++){
    bool act = digitalRead(cruceta[i]);
    if (act == LOW && antCruceta[i] == HIGH) {
      Serial.print("-> CRUCETA ["); Serial.print(i); Serial.print("] PRESIONADA | Pin Fisico: "); Serial.println(cruceta[i]);
    }
    antCruceta[i] = act;
  }

  for(int i = 0; i < 5; i++){
    bool act = digitalRead(controlButtons[i]);
    if (act == LOW && antControl[i] == HIGH) {
      Serial.print("-> Boton CONTROL ["); Serial.print(i); Serial.print("] PRESIONADO | Pin Fisico: "); Serial.println(controlButtons[i]);
    }
    antControl[i] = act;
  }

  for(int i = 0; i < 16; i++){
    bool act = digitalRead(mutesPin[i]);
    if (act == LOW && antMutes[i] == HIGH) {
      Serial.print("-> Boton MUTE ["); Serial.print(i); Serial.print("] PRESIONADO | Pin Fisico: "); Serial.println(mutesPin[i]);
    }
    antMutes[i] = act;
  }
  
  // Botón OK del Encoder
  bool actOk = digitalRead(encoderOkButton);
  if (actOk == LOW && antEncoderOk == HIGH) {
    Serial.print("-> ¡CLICK! Boton OK del Encoder PRESIONADO | Pin Fisico: "); Serial.println(encoderOkButton);
  }
  antEncoderOk = actOk;
  
  // ==========================================
  // 3. LECTURA DEL ENCODER DE GIRO
  // ==========================================
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.print("Encoder posicion: ");
    Serial.println(newPosition);
  }

  // Pequeña pausa de estabilidad general
  delay(10);
}