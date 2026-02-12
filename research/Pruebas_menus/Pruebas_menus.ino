#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int i;
int linea = 1;
void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);
  Serial.begin(9600);
}

void loop() {
  lcd.clear();
  // put your main code here, to run repeatedly:
  if (millis()/1000 == 0){
    linea *=-1;
    Serial.println(linea);
    }
  Serial.println(millis()/1000);
  lcd.setCursor(1,0);
  lcd.print("SEQ 1");
  lcd.setCursor(1,1);
  //lcd.print("SEQ 2");
  lcd.setCursor(1,1);

  if (linea == 1){

    while(i<5){
      lcd.write(0b11111111);
      i++;
    }
  }
  i = 0;
  delay(100);

}
