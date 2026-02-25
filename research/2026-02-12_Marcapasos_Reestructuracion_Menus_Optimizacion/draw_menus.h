#ifndef DRAW_MENUS
#define DRAW_MENUS

#include <Arduino.h>
#include <MIDI.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

// Carácteres
byte arrowLeft[8] = {B00001, B00011, B00111, B01111, B01111, B00111, B00011, B00001};
byte arrowRight[8] = {B10000, B11000, B11100, B11110, B11110, B11100, B11000, B10000};

byte redonda[8] = {B00000, B00000, B00000, B01110, B10001, B10001, B01110, B00000};
byte blanca[8] = {B00010, B00010, B00010, B00010, B01110, B10010, B10010, B01100};
byte negra[8] = {B00010, B00010, B00010, B00010, B01110, B11110, B11110, B01100};

byte stepOff[8] = {B00000, B00000, B00000, B01110, B00000, B00000, B00000, B00000};
byte stepOn[8] = {B00000, B01110, B10001, B10001, B10001, B01110, B00000, B00000};
byte stepPass[8] = {B00000, B01110, B11111, B11111, B11111, B01110, B00000, B00000};

const char* subdivisionesCharArray [12] = {"12/1","1/8", "1/6", "1/4", "1/3","1/2", "3/2", "1/1", "3/4", "2/1", "3/8", "4/1"};
const char* subdivisionesComplejasNumChar[6]{" 5"," 7"," 9","11","13","15"};
const byte subdivisionesComplejasDenChar[3]{byte(2),byte(3),byte(4)};

extern int movedPoteNumber;
extern int movedPoteValue;
extern int indiceMovedMute;
extern int octava;

extern int menuActual;
extern int seleccion;
extern int subDivMode;
extern int indComplexSubDivX;
extern int indComplexSubDivY;
extern bool selectDen;
extern bool selectNum;

extern int nTotalSteps;

static int nScreen;
static int nAnteriorScreen;
extern int nStep;
extern int nStepAnterior;


//Declaracion
void configurarLCD();
void update_LCD();
void drawMenuPrincipal();
void staticScreen1();
void staticScreen2();
void staticScreen3();
void drawScreenPotes();
void drawScreenMutes();
void drawSteps();

//Funciones
inline void configurarLCD(){
  lcd.createChar(0,arrowLeft);
  lcd.createChar(1,arrowRight);
  lcd.createChar(2,redonda);
  lcd.createChar(3,blanca);
  lcd.createChar(4,negra);
  lcd.createChar(5,stepOff);
  lcd.createChar(6,stepOn);
  lcd.createChar(7,stepPass);
  lcd.begin(16, 2);
}
inline void update_LCD(){
  if (menuActual == 0) drawMenuPrincipal();
  //else if (menuActual == 1) drawMenu1();
  //else if (menuActual == 2) drawMenu2();
  else if (menuActual == 3) drawScreenPotes();
  else if (menuActual == 4) drawScreenMutes();
  updateLCD = false; 
}
inline void staticScreen1(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(5,0), lcd.print("SEQ: "), lcd.print("1");
  lcd.setCursor(3,1), lcd.print("MODE: "), lcd.print("NOTE");
  nAnteriorScreen = nScreen;
}
inline void staticScreen2(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(subDivMode);
  if(subDivMode == 0 || subDivMode == 1){
    lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[indiceSubdivisiones]);}
  else if(subDivMode == 2){
    lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[indComplexSubdivY]);
  }
  nAnteriorScreen = nScreen;
}
inline void staticScreen3(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(4,0),  lcd.write(byte(0)), lcd.print("PASOS: "), lcd.print(nTotalSteps), lcd.write(byte(1));
  lcd.setCursor(0,1);
  for (int i = 0; i < nTotalSteps; i++) {
    if (!mutesValue[i]) {
      if (i == nStep) {
        lcd.write(byte(7));
      }
      else {
        lcd.write(byte(6));
      }
    }
    else {
      lcd.write(byte(5));
    }
  }
  nAnteriorScreen = nScreen;
}
inline void drawMenuPrincipal() {
  if(seleccion == 1 || seleccion == 2){nScreen = 0;}
  else if(seleccion == 3 || seleccion == 4){nScreen = 1;}
  else if(seleccion == 5){nScreen = 2;}

  switch(seleccion){
    case 1:
      if(nScreen != nAnteriorScreen){staticScreen1();}
      lcd.noBlink(), lcd.setCursor(4,0), lcd.write(byte(0)), lcd.setCursor(11,0), lcd.write(byte(1));
      lcd.setCursor(2,1), lcd.print(" "), lcd.setCursor(14,1) ,lcd.print(" "); 
      break;
    case 2:
      if(nScreen != nAnteriorScreen){staticScreen1();}
      lcd.noBlink(), lcd.setCursor(4,0), lcd.print(" "), lcd.setCursor(11,0), lcd.print(" ");
      lcd.setCursor(2,1), lcd.write(byte(0)), lcd.setCursor(14,1) ,lcd.write(byte(1)); 
      break;
    case 3: 
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.setCursor(12,0), lcd.print(subDivMode), lcd.write(byte(1));
      lcd.setCursor(5,1), lcd.print(" "), lcd.print(subdivisionesCharArray[indiceSubdivisiones]),lcd.print(" ");
      if(subDivMode == 0 || subDivMode == 1){
        lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[indiceSubdivisiones]);
        }
      else if(subDivMode == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[indComplexSubdivX]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[indComplexSubdivY]);
      }
      break;
    case 4:
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), lcd.setCursor(3,0), lcd.print(" "), lcd.setCursor(13,0), lcd.print(" ");
      if(subDivMode == 0 || subDivMode == 1){
        lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[indiceSubdivisiones]),lcd.write(byte(1));
        }
      else if(subDivMode == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[indComplexSubdivX]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[indComplexSubdivY]);
        if(selectNum){lcd.setCursor(6,1), lcd.blink();}
        else if(selectDen){lcd.setCursor(8,1), lcd.blink();}
      }
      break;
    case 5: 
      if(nScreen != nAnteriorScreen){staticScreen3();}
      lcd.noBlink(), lcd.setCursor(12,0), lcd.print(nTotalSteps);
      lcd.setCursor(indiceMovedMute,1);
      if (!mutesValue[indiceMovedMute]) {lcd.write(byte(6));}
      else {lcd.write(byte(5));}
      drawSteps();
    break;
  }
}

inline void drawScreenPotes() {
  nScreen = 3;
  lcd.clear();
  lcd.print("Pote N");
  lcd.print((char) 223);
  lcd.print(movedPoteNumber + 1);
  lcd.print(" : ");
  lcd.setCursor(0,1);
  lcd.print(movedPoteValue + (12 * octava));
  nAnteriorScreen = 3;
}
inline void drawScreenMutes() {
  nScreen = 4;
  lcd.clear();
  lcd.print("Octava");
  lcd.print(" : ");
  lcd.setCursor(0,1);
  lcd.print(12 * octava);
  nAnteriorScreen = 4;
}
inline void drawSteps() {
  if(mutesValue[nStep]){
    if(mutesValue[nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(nStep,1);
      lcd.write(byte(5));
    }
    else if(!mutesValue[nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(nStep,1);
      lcd.write(byte(5));
    }
  }
  else if(!mutesValue[nStep]){
    if(mutesValue[nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(nStep,1);
      lcd.write(byte(7));
    }
    else if(!mutesValue[nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(nStep,1);
      lcd.write(byte(7));
      
    }
  }
  nStepAnterior = nStep;
}

  

#endif