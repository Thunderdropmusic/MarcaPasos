#include <LiquidCrystal.h>


extern volatile unsigned long timer0_millis;
//pines del lcd
byte stepOff[8] = {
  B00000,
  B00000,
  B00000,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte stepOn[8] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000,
  B00000,
};
byte stepPass[8] = {
  B00000,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000,
  B00000,
};




int nPasos;
int valuePot;
int potsValues [2];
const int potePins [2] = {A0, A1};
const int nPasosPote = A2;
int nPasosPoteValue;
boolean mutes [16] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
int stepPosition;
const int nPotes = 2;
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {
  // put your setup code here, to run once:
  //tamaño de la pantalla
  lcd.createChar(0, stepOff);
  lcd.createChar(1, stepOn);
  lcd.createChar(2, stepPass);
  lcd.begin(16, 2);



}
void loop() {
  stepPosition = millis() / 250;
  if (stepPosition == nPasos) {
    stepPosition = 0;
    setMillis(0);
  }
  // put your main code here, to run repeatedly:
  for (int i = 0; i < nPotes; i++) {
    potsValues[i] = analogRead(potePins[i]);
    mutes[i] = potsValues[i];
    if (potsValues[i] < 500) {
      valuePot = true;
    }
    else {
      valuePot = false;
    }
    mutes[i] = valuePot;
  }
  //lcd.scrollDisplayLeft(); //Desplazamiento a la izquierda
  nPasosPoteValue = analogRead(nPasosPote);
  nPasos = constrain(map(nPasosPoteValue, 8, 1010, 16, 1),1, 16); //constrain limita los valores maximos y minimos para que no sobrepasen 0 y 127


  if (millis() % 100 == 0) {
    lcd.clear();  //borra la pantalla y pone el cursor en 0,0
    screenWrite();
  }
}

void setMillis(unsigned long new_millis) {
  uint8_t oldSREG = SREG;
  cli();
  timer0_millis = new_millis;
  SREG = oldSREG;
}
void screenWrite() {

  for (int i = 0; i < nPasos; i++) {
    if (mutes[i] == true) {
      if (i == stepPosition) {
        lcd.write(byte(2));
      }
      else {
        lcd.write(byte(1));
      }
    }
    else {
      lcd.write(byte(0));
    }
  }
  /*
    //lcd.print("valor N"); //comando para escribir
    //lcd.write(char(223)); //escribir un caracter
    //lcd.print(" 1: ");
    //lcd.setCursor(2,1); //donde inicia a escribir
    //lcd.print(value);
  */
}
