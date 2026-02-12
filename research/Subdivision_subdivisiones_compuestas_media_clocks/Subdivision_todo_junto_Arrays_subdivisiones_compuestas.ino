#include <LiquidCrystal.h>
#include <MIDI.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

struct HairlessConfig : public midi::DefaultSettings {
  static const long BaudRate = 115200;
};

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

const int numPotes = 3;
int potesValue[3];
int potSaveValue[3];
int movedPoteValue;

const char* subdivisionesCharArray [12] = {"12/1","1/8", "1/6", "1/4", "1/3","1/2", "3/2", "1/1", "3/4", "2/1", "3/8", "4/1"};
const int subdivisionesArray [12] = {2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96};
int indiceSubdivisiones = 0;
int subDivMode = 0;

unsigned long timeShowPotValue;
bool updateLCD = true; // Variable para saber cuando redibujar
int menuActual = 0;    // 0: Principal, 1: Menu 1, 2: Menu 2, 3: Screen Potes
int menuAnterior;
int seleccion = 1;     // 1 para Opción 1, 3 para Opción 2

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);
bool play;
int tempo;
unsigned long tiempoActualMillis;
unsigned long tiempoActualMicros;
int tipoMsgMidi;
unsigned long timeDecayNote;
int note;
unsigned long int decay = 250;
const int analogPin = A0;
int value;
int ultimoValorPote;
int notaTocada;
volatile boolean notaFuera;
int pulsoClock = 0;
int subdivision;
int potencia = 0;
boolean repeated;
int ultimoTiempoBoton;
int nStep;

unsigned long int tiempoClock[24];
unsigned long int BPM;
unsigned long int sumaTiempos;
int totalInitClocks = 24;
unsigned long int microsSubdivision;
unsigned long int tiempoClock1;
unsigned long int tiempoClock2;
unsigned long int tiempoUltimaNota;
unsigned long int mediaActual;
unsigned long int mediaEstimada;

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
  lcd.begin(16, 2);
}

void loop() {

  // 1. Lectura de Sensores y Botones
  tiempoActualMillis = millis();
  tiempoActualMicros = micros();
  checkButtons();
  checkPotes();
  if(play == true){
  midiNotesOn();
  }
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

  // 3. Temporizador para salir de la pantalla de potenciómetro
  if (menuActual == 3 && (tiempoActualMillis - timeShowPotValue >= 1500)) {
    menuActual = menuAnterior;
    updateLCD = true;
  }
}

void checkButtons() {
  if (digitalRead(pinButton1) == HIGH) { // Arriba
    if (menuActual == 0 && seleccion != 1) { seleccion = 1; updateLCD = true; }
    else if (menuActual == 1 && seleccion != 1) { seleccion = 1; updateLCD = true; }
  }

  if (digitalRead(pinButton3) == HIGH){ // Abajo
    if (menuActual == 0 && seleccion != 3) { seleccion = 3; updateLCD = true; }
    else if (menuActual == 1 && seleccion != 3) { seleccion = 3; updateLCD = true; }
  }
  

  if (digitalRead(pinButton5) == HIGH) { // Izquierda
    if (menuActual == 1 && seleccion == 1) { 
      if (!repeated && tiempoActualMillis - ultimoTiempoBoton >= 250){
        if(subDivMode < 2){
          subDivMode ++;
          updateLCD = true;
          repeated = true;
          ultimoTiempoBoton = tiempoActualMillis;
          if (subDivMode == 1 && indiceSubdivisiones % 2 == 0) {indiceSubdivisiones = 7;}
          }
        }
      }
    else if (menuActual == 1 && seleccion == 3) { //menu subdivisiones seleccion 2:
      if(subDivMode == 0){ //Modo binario-ternario
        if (!repeated && tiempoActualMillis - ultimoTiempoBoton >= 250){
          if (indiceSubdivisiones < 11){
            indiceSubdivisiones++;
            updateLCD = true;
            repeated = true;
            ultimoTiempoBoton = tiempoActualMillis;
          }
        }
      }
      else if(subDivMode == 1){ //modo solo binarios
        if (!repeated && tiempoActualMillis - ultimoTiempoBoton >= 250){
          if (indiceSubdivisiones < 11){
            indiceSubdivisiones += 2;
            updateLCD = true;
            repeated = true;
            ultimoTiempoBoton = tiempoActualMillis;
          }
        }
      }
      else if(subDivMode == 2){ //modo compuesto
        if (!repeated && tiempoActualMillis - ultimoTiempoBoton >= 250){
          if (indiceSubdivisiones < 11){
            indiceSubdivisiones += 2;
            updateLCD = true;
            repeated = true;
            ultimoTiempoBoton = tiempoActualMillis;
          }
        }
      }    
    }
  }
  if (digitalRead(pinButton5) == LOW){repeated = false;}
  if (digitalRead(pinButton6) == HIGH) { // Derecha
    if (menuActual == 1 && seleccion == 1) { 
      if (!repeated && tiempoActualMillis - ultimoTiempoBoton >= 250){    
        if(subDivMode > 0){
          subDivMode --;
          updateLCD = true;
          repeated = true;
          ultimoTiempoBoton = tiempoActualMillis;
          if (subDivMode == 1 && indiceSubdivisiones % 2 == 0) {indiceSubdivisiones = 7;}
          }
        }
      }
    else if (menuActual == 1 && seleccion == 3) {
      if(subDivMode == 0){
        if (!repeated && tiempoActualMillis - ultimoTiempoBoton >= 250){
          if (indiceSubdivisiones > 0){
            indiceSubdivisiones--;
            updateLCD = true;
            repeated = true;
            ultimoTiempoBoton = tiempoActualMillis;
          }
        }
      }
      else if(subDivMode == 1){
        if (!repeated && tiempoActualMillis - ultimoTiempoBoton >= 250){
          if (indiceSubdivisiones > 1){
            indiceSubdivisiones -= 2;
            updateLCD = true;
            repeated = true;
            ultimoTiempoBoton = tiempoActualMillis;
          }
        }
      }
    }
  }
  if (digitalRead(pinButton6) == LOW){repeated = false;}

  if (digitalRead(pinButton2) == HIGH) { // OK
    if (menuActual == 0) {
      menuActual = (seleccion == 1) ? 1 : 2;
      menuAnterior = menuActual;
      updateLCD = true;
    }
  }
  if (digitalRead(pinButton4) == HIGH) { // Volver
      if (menuActual != 0) { menuActual = 0; menuAnterior = menuActual; updateLCD = true; }
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
  lcd.setCursor(0,0);
  lcd.print(seleccion == 1 ? "> Menu 1" : "  Menu 1");
  lcd.setCursor(0,1); 
  lcd.print(seleccion == 3 ? "> Menu 2" : "  Menu 2");
}

void drawMenu1() {
  if(seleccion == 1) { 
    lcd.setCursor(3,0), lcd.write(byte(0)), lcd.print("SUBDIV: "), lcd.print(subDivMode), lcd.write(byte(1));
    if(subDivMode == 0 || subDivMode == 1){
      lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[indiceSubdivisiones]);}
    else if(subDivMode == 2){
      lcd.setCursor(0,1),lcd.print(microsSubdivision*0.00024*subdivision), lcd.print(" "),lcd.print(microsSubdivision);}
  }
  else if(seleccion == 3) { 
    lcd.setCursor(4,0), lcd.print("SUBDIV: ");
    if(subDivMode == 0 || subDivMode == 1){ 
      lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[indiceSubdivisiones]),lcd.write(byte(1));}
    else if(subDivMode == 2){
      lcd.setCursor(0,1),lcd.print(microsSubdivision*0.00024*subdivision), lcd.print(" "),lcd.print(microsSubdivision/1000);}
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
