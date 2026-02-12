#include <LiquidCrystal.h>
extern volatile unsigned long timer0_millis;
LiquidCrystal lcd(8,9,4,5,6,7); //pines del lcd

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
boolean valuePot1;
const int potePin = A0;
boolean mutes [16] = {true,true,true,false,true,true,true,true,true,true,true,true,true,true,true,true};
int stepPosition;
void setup() {
  // put your setup code here, to run once:
 //tamaño de la pantalla
 Serial.begin(9600);
  lcd.createChar(0,stepOff);
  lcd.createChar(1,stepOn);
  lcd.createChar(2,stepPass);
  lcd.begin(16,2);

}

void loop() {
  stepPosition = millis()/1000;
  if (stepPosition > 15){
    stepPosition = 0;
    setMillis(0);
  }
  // put your main code here, to run repeatedly:
  int pot1=analogRead(potePin);
  //lcd.scrollDisplayLeft(); //Desplazamiento a la izquierda
  //int value = constrain(map(pot1, 8, 1010, 127, 0),0, 127); //constrain limita los valores maximos y minimos para que no sobrepasen 0 y 127
  if(pot1<500){valuePot1 = true;}
  else{valuePot1 = false;}
  mutes[1] = valuePot1;
  for (int i; i < sizeof(mutes); i++){
    Serial.print(i);
    if (mutes[i] == true){
      Serial.print("Mute true");
      if(i == stepPosition){
        lcd.write(byte(2));
        Serial.print("2");
        }
      else{
        lcd.write(byte(1));
        Serial.print("1");
      }
    }
    else{lcd.write(byte(0));}
        Serial.print("0");
  }
  Serial.println();
  /*
  //lcd.print("valor N"); //comando para escribir
  //lcd.write(char(223)); //escribir un caracter 
  //lcd.print(" 1: ");
  //lcd.setCursor(2,1); //donde inicia a escribir
  //lcd.print(value);
  */
  lcd.setCursor(0,1);
  lcd.print(pot1);
  lcd.print(valuePot1);
  delay(100); 
  lcd.clear(); //borra la pantalla y pone el cursor en 0,0 
}

void setMillis(unsigned long new_millis){
  uint8_t oldSREG = SREG;
  cli();
  timer0_millis = new_millis;
  SREG = oldSREG;
}
