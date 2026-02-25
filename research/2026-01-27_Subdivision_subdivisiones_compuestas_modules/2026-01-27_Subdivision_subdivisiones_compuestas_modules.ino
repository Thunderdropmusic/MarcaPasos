#include <LiquidCrystal.h>
#include <MIDI.h>
#include "midiPrograming.h"
#include "menus_buttons.h"
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int pinButton1 = 22; // Arriba
const int pinButton2 = 23; // OK / Entrar
const int pinButton3 = 24; // Abajo
const int pinButton4 = 25; // Volver
const int pinButton5 = 26; // Izquierda
const int pinButton6 = 28; // Derecha

int potesPin[3] = {A0, A1, A2};

byte arrowLeft[8] = {
  B00001,
  B00011,
  B00111,
  B01111,
  B01111,
  B00111,
  B00011,
  B00001,
};
byte arrowRight[8] = {
  B10000,
  B11000,
  B11100,
  B11110,
  B11110,
  B11100,
  B11000,
  B10000,
};
byte redonda[8] = {
  B00000,
  B00000,
  B00000,
  B01110,
  B10001,
  B10001,
  B01110,
  B00000,

};
byte blanca[8] = {
  B00010,
  B00010,
  B00010,
  B00010,
  B01110,
  B10010,
  B10010,
  B01100,
};
byte negra[8] = {
  B00010,
  B00010,
  B00010,
  B00010,
  B01110,
  B11110,
  B11110,
  B01100,
};

const int numPotes = 3;
int potesValue[3];
int potSaveValue[3];
int movedPoteValue;

const char* subdivisionesCharArray [12] = {"12/1","1/8", "1/6", "1/4", "1/3","1/2", "3/2", "1/1", "3/4", "2/1", "3/8", "4/1"};
const int subdivisionesArray [12] = {2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96};
//const int numeradorSubdComplejasRedondas [6] = {5, 7, 9, 11, 13, 15};
//const int numeradorSubdComplejasBlancas [6] = {10, 14, 18, 22, 26, 30};
//const int numeradorSubdComplejasNegras [3] = {20, 28, 36};

const int subdivisionesComplejasArray[3][6] = {
  {5, 7, 9, 11, 13, 15},
  {10, 14, 18, 22, 26, 30},
  {20, 28, 36, 0, 0, 0,}
};
const char* subdivisionesComplejasNumChar[6]{"5","7","9","11","13","15"};
const byte subdivisionesComplejasDenChar[3]{byte(2),byte(3),byte(4)};
bool selectNum = false;
bool selectDen = false;

int indiceSubdivisiones = 0;
int subDivMode = 0;
int indComplexSubdivX;
int indComplexSubdivY;
unsigned long timeShowPotValue;
bool updateLCD = true; // Variable para saber cuando redibujar
int menuActual = 0;    // 0: Principal, 1: Menu 1, 2: Menu 2, 3: Screen Potes
int menuAnterior;
int seleccion = 1;     // 1 para Opción 1, 3 para Opción 2

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);
unsigned long tiempoActualMillis;
unsigned long tiempoActualMicros;
const int analogPin = A0;
int ultimoValorPote;
int subdivision;

unsigned long int BPM;

unsigned long int microsSubdivision = 500000000;


void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  pinMode(pinButton1, INPUT);
  pinMode(pinButton2, INPUT);
  pinMode(pinButton3, INPUT);
  pinMode(pinButton4, INPUT); 
  pinMode(pinButton5, INPUT);
  pinMode(pinButton6, INPUT);   

  
  for(int i = 0; i < numPotes; i++){
    int lectura = analogRead(potesPin[i]);
    potesValue[i] = constrain(map(lectura, 1018, 8, 48, 60), 0, 127);
    potSaveValue[i] = potesValue[i];
  }
  lcd.createChar(0,arrowLeft);
  lcd.createChar(1,arrowRight);
  lcd.createChar(2,redonda);
  lcd.createChar(3,blanca);
  lcd.createChar(4,negra);
  lcd.begin(16, 2);
}

void loop() {

  // 1. Lectura de Sensores y Botones
  tiempoActualMillis = millis();
  tiempoActualMicros = micros();
  checkButtons();
  checkPotes();
  midiNotesOn();
  if (MIDI.read()){
    midiClock();
  }

  midiNotesOff();
  // 2. Lógica de Pantalla (Solo imprime si hay cambios)
  if (updateLCD) {
    lcd.clear();
    if (menuActual == 0) drawMenuPrincipal();
    else if (menuActual == 1) drawMenu1();
    else if (menuActual == 2) drawMenu2();
    else if (menuActual == 3) drawScreenPotes();
    updateLCD = false; 
  }

  // 3. nClockMsgrizador para salir de la pantalla de potenciómetro
  if (menuActual == 3 && (tiempoActualMillis - timeShowPotValue >= 1500)) {
    menuActual = menuAnterior;
    updateLCD = true;
  }
}



void checkPotes() {
  for(int i = 0; i < numPotes; i++){
    int val = constrain(map(analogRead(potesPin[i]), 1018, 8, 48, 60), 0, 127);
    potesValue[i] = val;
    // Solo activamos si el cambio es mayor a 1 para evitar ruido
    if (abs(val - potSaveValue[i]) > 0.5) { //esto estará bien para valores CC
      potSaveValue[i] = val;
      movedPoteValue = val;
      timeShowPotValue = tiempoActualMillis;
      menuActual = 3;
      updateLCD = true;
    }
  }
}

void drawMenuPrincipal() {
  lcd.noBlink();
  lcd.setCursor(0,0);
  lcd.print(seleccion == 1 ? "> Menu 1" : "  Menu 1");
  lcd.setCursor(0,1); 
  lcd.print(seleccion == 3 ? "> Menu 2" : "  Menu 2");
}

void drawMenu1() {

  if(seleccion == 1) { 
    lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.print("SUBDIV: "), lcd.print(subDivMode), lcd.write(byte(1));
    if(subDivMode == 0 || subDivMode == 1){
      lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[indiceSubdivisiones]);}
    else if(subDivMode == 2){
      lcd.setCursor(6,1), lcd.print(subdivisionesComplejasNumChar[indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[indComplexSubdivY]);
    }
  }
  else if(seleccion == 3) { 
    lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(subDivMode);
    if(subDivMode == 0 || subDivMode == 1){ 
      lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[indiceSubdivisiones]),lcd.write(byte(1));}
    else if(subDivMode == 2){
      lcd.setCursor(6,1), lcd.print(subdivisionesComplejasNumChar[indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[indComplexSubdivY]);
      if(selectNum){lcd.setCursor(6,1), lcd.blink();}
      else if(selectDen){lcd.setCursor(8,1), lcd.blink();}
    }
  }
}
void drawMenu2() {
  lcd.print("BPM = "); 
}
void drawScreenPotes() {
  lcd.print("Valor Pote:");
  lcd.setCursor(0,1);
  lcd.print(movedPoteValue);
}
