#include <LiquidCrystal.h>


LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int pinButton1 = 22;
int pinButton2 = 24;
int pinButton3 = 26;
int button1;
int button2;
int button3;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  pinMode(pinButton1, INPUT);
  pinMode(pinButton2, INPUT);
  pinMode(pinButton3, INPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  button1 = digitalRead(pinButton1);
  button2 = digitalRead(pinButton2);
  button3 = digitalRead(pinButton3);
  if(button1 == HIGH){
    lcd.print("Boton derecho");
  }
  if(button2 == HIGH){
    lcd.print("Boton central");
  }
  if(button3 == HIGH){
    lcd.print("Boton izquierdo");
  }
  delay(100);
  lcd.clear();
}
