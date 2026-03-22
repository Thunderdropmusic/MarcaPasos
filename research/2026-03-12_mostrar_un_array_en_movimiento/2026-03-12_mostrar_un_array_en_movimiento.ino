#include <LiquidCrystal.h>

LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

const byte potesPin[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
int potSaveValue[8];
byte globalVelocity[8] = {10, 30, 60, 90, 127, 40, 20, 5};

int offsetVumetro = 0;
unsigned long lastMove = 0;

// Solo usamos 6 huecos (0-5). Reservamos el 6 y 7 por si necesitas flechas luego.
byte b1[] = {0,0,0,0,0,0,31,0}; // Casi abajo
byte b2[] = {0,0,0,0,0,31,0,0};
byte b3[] = {0,0,0,0,31,0,0,0};
byte b4[] = {0,0,0,31,0,0,0,0};
byte b5[] = {0,0,31,0,0,0,0,0};
byte b6[] = {0,31,0,0,0,0,0,0}; // Casi arriba

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(0, b1);
  lcd.createChar(1, b2);
  lcd.createChar(2, b3);
  lcd.createChar(3, b4);
  lcd.createChar(4, b5);
  lcd.createChar(5, b6);
}

void loop() {
  checkPotes();

  
  
  for (int i = 0; i < 8; i++) {
    lcd.setCursor(i, 0);
    int indiceMovido = (i + offsetVumetro) % 8;
    
    // Mapeamos a 8 niveles (0 a 7)
    int nivel = map(globalVelocity[indiceMovido], 0, 127, 0, 7);
    
    // Usamos caracteres estándar para el mínimo y el máximo
    switch(nivel) {
      case 0: lcd.print("_"); break;      // Nivel más bajo: Barra baja estándar
      case 1: lcd.write(byte(0)); break;  // Custom 0
      case 2: lcd.write(byte(1)); break;  // Custom 1
      case 3: lcd.write(byte(2)); break;  // Custom 2
      case 4: lcd.write(byte(3)); break;  // Custom 3
      case 5: lcd.write(byte(4)); break;  // Custom 4
      case 6: lcd.write(byte(5)); break;  // Custom 5
      case 7: lcd.print("-"); break;      // Nivel más alto: Guion estándar (o barra superior si existiera)
    }

  }
  lcd.setCursor(0, 1);
  lcd.print(globalVelocity[offsetVumetro]);
  if (millis() - lastMove > 200) {
    offsetVumetro++;
    if (offsetVumetro >= 8) offsetVumetro = 0;
    lastMove = millis();
  }
}

void checkPotes() {
  for (int i = 0; i < 8; i++) {
    int val = analogRead(potesPin[i]);
    if (abs(val - potSaveValue[i]) > 10) {
      potSaveValue[i] = val;
      globalVelocity[i] = constrain(map(val, 1023, 0, 0, 127), 0, 127);
    }
  }
}