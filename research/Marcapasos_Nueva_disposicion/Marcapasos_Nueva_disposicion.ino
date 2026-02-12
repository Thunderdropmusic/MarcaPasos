#include <LiquidCrystal.h>
#include <MIDI.h>
#include "midiPrograming.h"
#include "menus_buttons.h"

// Instancias librerias
LiquidCrystal lcd(8, 9, 7, 6, 5, 4);
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);

// Pines
const byte pinButton1 = 22; // Arriba
const byte pinButton2 = 23; // OK / Entrar
const byte pinButton4 = 24; // Volver
const byte pinButton3 = 25; // Abajo
const byte pinButton5 = 27; // Izquierda
const byte pinButton6 = 29; // Derecha
const byte potesPin[8] = {A0, A1, A2, A3, A4, A5, A6, A7/*, A8, A9, A10, A11, A12, A13, A14, A15*/};

// Valores potenciometros
const int numPotes = 8;
int potesValue[numPotes]; //Valores detectados
int potSaveValue[numPotes]; //Valores guardados al haber una variacion con los detectados
int movedPoteValue;
int movedPoteNumber;
int nTotalSteps = 8;

// Carácteres
byte arrowLeft[8] = {B00001, B00011, B00111, B01111, B01111, B00111, B00011, B00001};
byte arrowRight[8] = {B10000, B11000, B11100, B11110, B11110, B11100, B11000, B10000};
byte redonda[8] = {B00000, B00000, B00000, B01110, B10001, B10001, B01110, B00000};
byte blanca[8] = {B00010, B00010, B00010, B00010, B01110, B10010, B10010, B01100};
byte negra[8] = {B00010, B00010, B00010, B00010, B01110, B11110, B11110, B01100};

// Variables lógica menús
int menuActual = 0;    // 0: Principal, 1: Menu 1, 2: Menu 2, 3: Screen Potes
int menuAnterior;
int seleccion = 1;     // 1 para Opción 1, 3 para Opción 2
bool updateLCD = true; // Variable para saber cuando redibujar
bool selectNum = false; //Si está seleccionando el numerador de las subdivisiones complejas
bool selectDen = false; //Si está seleccionando el denominador de las subdivisiones complejas

// Subdivisiones estandar
const char* subdivisionesCharArray [12] = {"12/1","1/8", "1/6", "1/4", "1/3","1/2", "3/2", "1/1", "3/4", "2/1", "3/8", "4/1"};
const int subdivisionesArray [12] = {2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96};
bool primeraMedicionSubCompleja;

// Subdivisiones complejas
const int subdivisionesComplejasArray[3][6] = {
  {5, 7, 9, 11, 13, 15},
  {10, 14, 18, 22, 26, 30},
  {20, 28, 36, 0, 0, 0,}
};
const char* subdivisionesComplejasNumChar[6]{" 5"," 7"," 9","11","13","15"};
const byte subdivisionesComplejasDenChar[3]{byte(2),byte(3),byte(4)};

// Indices Navegación
int indiceSubdivisiones = 7;
int subDivMode = 0;
int indComplexSubdivX;
int indComplexSubdivY;

// Variables de tiempo real
unsigned long timeShowPotValue;
unsigned long tiempoActualMillis;
unsigned long tiempoActualMicros;

// Gestion de MIDI y tiempo musical
int subdivision;
unsigned long int microsSubdivision = 500000000;
bool play;

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  pinMode(pinButton1, INPUT);
  pinMode(pinButton2, INPUT);
  pinMode(pinButton3, INPUT);
  pinMode(pinButton4, INPUT); 
  pinMode(pinButton5, INPUT);
  pinMode(pinButton6, INPUT);   

  // Primera lectura potes
  for(int i = 0; i < numPotes; i++){
    int lectura = analogRead(potesPin[i]);
    potesValue[i] = constrain(map(lectura, 1018, 8, 48, 60), 0, 127);
    potSaveValue[i] = potesValue[i];
  }
  // Creación caracteres e inicio de la pantalla
  lcd.createChar(0,arrowLeft);
  lcd.createChar(1,arrowRight);
  lcd.createChar(2,redonda);
  lcd.createChar(3,blanca);
  lcd.createChar(4,negra);
  lcd.begin(16, 2);
}

void loop() {

  tiempoActualMillis = millis();
  tiempoActualMicros = micros();
  checkButtons();
  checkPotes();

  if (MIDI.read()){
    midiClock();
  }
  midiNotesOn();
  midiNotesOff();
  // Reset pantalla
  if (updateLCD) {
    lcd.clear();
    if (menuActual == 0) drawMenuPrincipal();
    //else if (menuActual == 1) drawMenu1();
    //else if (menuActual == 2) drawMenu2();
    else if (menuActual == 3) drawScreenPotes();
    updateLCD = false; 
  }

  // Clock para salir de la pantalla de potenciómetro
  if (menuActual == 3 && (tiempoActualMillis - timeShowPotValue >= 1500)) {
    menuActual = menuAnterior;
    updateLCD = true;
  }
}



void checkPotes() {
  for(int i = 0; i < numPotes; i++){
    int val = constrain(map(analogRead(potesPin[i]), 1018, 30, 48, 61), 48, 60);
    potesValue[i] = val;
    if (abs(val - potSaveValue[i]) > 0.5) { //Solo activamos cuando sea mayor de 0.5;
      potSaveValue[i] = val;
      movedPoteValue = val;
      movedPoteNumber = i;
      timeShowPotValue = tiempoActualMillis;
      menuActual = 3;
      updateLCD = true;
    }
  }
}

void drawMenuPrincipal() {
  if(seleccion == 1) {
    lcd.noBlink(), lcd.setCursor(4,0), lcd.write(byte(0)), lcd.print("SEQ: "), lcd.print("1"), lcd.write(byte(1));
    lcd.setCursor(3,1), lcd.print("MODE: "), lcd.print("NOTE");
  } 
  else if(seleccion == 2) {
    lcd.noBlink(), lcd.setCursor(5,0), lcd.print("SEQ: "), lcd.print("1");
    lcd.setCursor(2,1), lcd.write(byte(0)), lcd.print("MODE: "), lcd.print("NOTE"),lcd.write(byte(1)); 
  }   
  else if(seleccion == 3) { 
    lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.print("SUBDIV: "), lcd.print(subDivMode), lcd.write(byte(1));
    if(subDivMode == 0 || subDivMode == 1){
      lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[indiceSubdivisiones]);}
    else if(subDivMode == 2){
      lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[indComplexSubdivY]);
    }
  }
  else if(seleccion == 4) { 
    lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(subDivMode);
    if(subDivMode == 0 || subDivMode == 1){ 
      lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[indiceSubdivisiones]),lcd.write(byte(1));}
    else if(subDivMode == 2){
      lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[indComplexSubdivY]);
      if(selectNum){lcd.setCursor(6,1), lcd.blink();}
      else if(selectDen){lcd.setCursor(8,1), lcd.blink();}
    }
  }
  else if(seleccion == 5) { 
    lcd.noBlink(), lcd.setCursor(4,0),  lcd.write(byte(0)), lcd.print("PASOS: "), lcd.print(nTotalSteps), lcd.write(byte(1));
  }
}

/*
void drawMenu1() {
  if(seleccion == 1) { 
    lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.print("SUBDIV: "), lcd.print(subDivMode), lcd.write(byte(1));
    if(subDivMode == 0 || subDivMode == 1){
      lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[indiceSubdivisiones]);}
    else if(subDivMode == 2){
      lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[indComplexSubdivY]);
    }
  }
  else if(seleccion == 2) { 
    lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(subDivMode);
    if(subDivMode == 0 || subDivMode == 1){ 
      lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[indiceSubdivisiones]),lcd.write(byte(1));}
    else if(subDivMode == 2){
      lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[indComplexSubdivY]);
      if(selectNum){lcd.setCursor(6,1), lcd.blink();}
      else if(selectDen){lcd.setCursor(8,1), lcd.blink();}
    }
  }
}
void drawMenu2() {
  lcd.print("BPM = "); 
}*/
void drawScreenPotes() {
  lcd.print("Pote N");
  lcd.print((char) 223);
  lcd.print(movedPoteNumber + 1);
  lcd.print(" : ");
  lcd.setCursor(0,1);
  lcd.print(movedPoteValue);
}
