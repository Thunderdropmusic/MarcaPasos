#include <LiquidCrystal.h>
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

byte bassClef1[8] = {B00000, B00000, B00000, B00000, B01110, B10001, B10001, B01001};
byte bassClef2[8] = {B00001, B00010, B00100, B11000, B00000, B00000, B00000, B00000};
byte trebleClef1[8] = {B00000, B00000, B00010, B00101, B00101, B00110, B00100, B01100};
byte trebleClef2[8] = {B10110, B11101, B11101, B10101, B01110, B00100, B01100, B00000};
byte Score1[8] = {B00000, B00000, B00000, B00000, B00000, B11111, B00000, B11111};
byte Score2[8] = {B11111, B00000, B11111, B00000, B11111, B00000, B00000, B00000};
void setup() {
  // put your setup code here, to run once:
  lcd.createChar(0,bassClef1);
  lcd.createChar(1,bassClef2);
  lcd.createChar(2,Score1);
  lcd.createChar(3,Score2);
  lcd.createChar(4,trebleClef1);
  lcd.createChar(5,trebleClef2);
  lcd.begin(16, 2);
  lcd.write(byte(0));
  lcd.write(byte(2));
  lcd.write(byte(2));
  lcd.write(byte(4));
  lcd.write(byte(2));
  lcd.setCursor(0,1);
  lcd.write(byte(1));
  lcd.write(byte(3));
  lcd.write(byte(3));
  lcd.write(byte(5));
  lcd.write(byte(3));
}

void loop() {
  // put your main code here, to run repeatedly:
}
