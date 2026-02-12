#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

const int pinButton1 = 22; // Arriba
const int pinButton2 = 24; // OK / Entrar
const int pinButton3 = 26; // Abajo
const int pinButton4 = 28; // Volver
int potesPin[3] = {A0, A1, A2};

const int numPotes = 3;
int potesValue[3];
int potSaveValue[3];
int movedPoteValue;

unsigned long timeShowPotValue;
bool updateLCD = true; // Variable para saber cuando redibujar
int menuActual = 0;    // 0: Principal, 1: Menu 1, 2: Menu 2, 3: Screen Potes
int seleccion = 1;     // 1 para Opción 1, 3 para Opción 2

void setup() {
  lcd.begin(16, 2);
  pinMode(pinButton1, INPUT);
  pinMode(pinButton2, INPUT);
  pinMode(pinButton3, INPUT);
  pinMode(pinButton4, INPUT);
  
  for(int i = 0; i < numPotes; i++){
    int lectura = analogRead(potesPin[i]);
    potesValue[i] = constrain(map(lectura, 1018, 8, 0, 127), 0, 127);
    potSaveValue[i] = potesValue[i];
  }
}

void loop() {
  // 1. Lectura de Sensores y Botones
  checkButtons();
  checkPotes();

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
  if (menuActual == 3 && (millis() - timeShowPotValue >= 1500)) {
    menuActual = 0;
    updateLCD = true;
  }
}

void checkButtons() {
  if (digitalRead(pinButton1) == HIGH) { // Arriba
    if (menuActual == 0 && seleccion != 1) { seleccion = 1; updateLCD = true; } // Debounce simple
  }
  if (digitalRead(pinButton3) == HIGH) { // Abajo
    if (menuActual == 0 && seleccion != 3) { seleccion = 3; updateLCD = true; }
  }
  if (digitalRead(pinButton2) == HIGH) { // OK
    if (menuActual == 0) {
      menuActual = (seleccion == 1) ? 1 : 2;
      updateLCD = true;
    }
  }
  if (digitalRead(pinButton4) == HIGH) { // Volver
    if (menuActual != 0) { menuActual = 0; updateLCD = true; }
  }
}

void checkPotes() {
  for(int i = 0; i < numPotes; i++){
    int val = constrain(map(analogRead(potesPin[i]), 1018, 8, 0, 127), 0, 127);
    // Solo activamos si el cambio es mayor a 1 para evitar ruido
    if (abs(val - potSaveValue[i]) > 1) {
      potSaveValue[i] = val;
      movedPoteValue = val;
      timeShowPotValue = millis();
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

void drawMenu1() { lcd.print("Estas en Menu 1"); }
void drawMenu2() { lcd.print("Estas en Menu 2"); }
void drawScreenPotes() {
  lcd.print("Valor Pote:");
  lcd.setCursor(0,1);
  lcd.print(movedPoteValue);
}
