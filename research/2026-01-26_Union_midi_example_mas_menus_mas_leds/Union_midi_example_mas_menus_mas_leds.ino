#include <LiquidCrystal.h>
#include <MIDI.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

struct HairlessConfig : public midi::DefaultSettings {
  static const long BaudRate = 115200;
};

const int pinButton1 = 22; // Arriba
const int pinButton2 = 24; // OK / Entrar
const int pinButton3 = 26; // Abajo
const int pinButton4 = 28; // Volver
const int pinLeds [3] = {30,32,34}; // Led1

int potesPin[3] = {A0, A1, A2};

const int numPotes = 3;
int potesValue[3];
int potSaveValue[3];
int movedPoteValue;
const char* subdivisionesArray [6] = {"1/8", "1/4", "1/2", "1/1", "2/1", "4/1"};
int indiceSubdivisiones = 0;

unsigned long timeShowPotValue;
bool updateLCD = true; // Variable para saber cuando redibujar
int menuActual = 0;    // 0: Principal, 1: Menu 1, 2: Menu 2, 3: Screen Potes
int menuAnterior;
int seleccion = 1;     // 1 para Opción 1, 3 para Opción 2

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);
int tempo;
int tiempoActual;
int tipo;
unsigned long timeDecayNote;
int note;
unsigned long int decay = 250;
const int analogPin = A0;
int value;
int ultimoValorPote;
int notaTocada;
volatile boolean notaFuera;
int pulso = 0;
int subdivision = 1;
int newSubdivision = 1;
int potencia = 0;
boolean repeated;
int ultimoTiempoBoton;
int nStep;
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  lcd.begin(16, 2);
  pinMode(pinButton1, INPUT);
  pinMode(pinButton2, INPUT);
  pinMode(pinButton3, INPUT);
  pinMode(pinButton4, INPUT); 
  pinMode(pinLeds[0] , OUTPUT);
  pinMode(pinLeds[1] , OUTPUT);
  pinMode(pinLeds[2] , OUTPUT);
  
  for(int i = 0; i < numPotes; i++){
    int lectura = analogRead(potesPin[i]);
    potesValue[i] = constrain(map(lectura, 1018, 8, 48, 60), 0, 127);
    potSaveValue[i] = potesValue[i];
  }
}

void loop() {

  // 1. Lectura de Sensores y Botones
  tiempoActual = millis();
  checkButtons();
  checkPotes();
  if (MIDI.read()){
    midiClock();
  }
  midiNotes();
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
  if (menuActual == 3 && (tiempoActual - timeShowPotValue >= 1500)) {
    menuActual = menuAnterior;
    updateLCD = true;
  }
}

void checkButtons() {
  if (digitalRead(pinButton1) == HIGH) { // Arriba
    if (menuActual == 0 && seleccion != 1) { seleccion = 1; updateLCD = true; } // Debounce simple
    else if (menuActual == 1) {
      if (!repeated && tiempoActual - ultimoTiempoBoton >= 250){
        if (subdivision < 32){
          subdivision = subdivision << 1;//3*2^1
          updateLCD = true;
          repeated = true;
          indiceSubdivisiones++;
          ultimoTiempoBoton = tiempoActual;
        }
      }
    }
  }
  if (digitalRead(pinButton1) == LOW){repeated = false;}
  if (digitalRead(pinButton3) == HIGH){ // Abajo
    if (menuActual == 0 && seleccion != 3) { seleccion = 3; updateLCD = true; }
    else if(menuActual == 1){
      if(!repeated && tiempoActual - ultimoTiempoBoton >= 250){   
        if (subdivision > 1){
          subdivision = subdivision >> 1;//3*2^1
          updateLCD = true;
          repeated = true;
          indiceSubdivisiones--;
          ultimoTiempoBoton = tiempoActual;
        }
      }
    }
  }
  if (digitalRead(pinButton3) == LOW){repeated = false;}
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
      timeShowPotValue = tiempoActual;
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
  lcd.print("SUBDIVISION");
  lcd.setCursor(6,1);
  lcd.print("");
  lcd.print(subdivisionesArray[indiceSubdivisiones]);
}
void drawMenu2() {
  lcd.print("BPM = "); 
}
void drawScreenPotes() {
  lcd.print("Valor Pote:");
  lcd.setCursor(0,1);
  lcd.print(movedPoteValue);
}
