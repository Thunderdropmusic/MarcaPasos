#include "draw_menus.h"
#include "midiProgramming.h"
#include "midiInterface.h"
#include "menus_buttons.h"

#include <LiquidCrystal.h>

DrawMenus drawUI;

DrawMenus::DrawMenus() : lcd (9, 8, 7, 6, 5, 4),

  // Carácteres
  arrowLeft{B00001, B00011, B00111, B01111, B01111, B00111, B00011, B00001},
  arrowRight{B10000, B11000, B11100, B11110, B11110, B11100, B11000, B10000},
  redonda{B00000, B00000, B00000, B01110, B10001, B10001, B01110, B00000},
  blanca{B00010, B00010, B00010, B00010, B01110, B10010, B10010, B01100},
  negra{B00010, B00010, B00010, B00010, B01110, B11110, B11110, B01100},
  stepOff{B00000, B00000, B00000, B01110, B00000, B00000, B00000, B00000},
  stepOn{B00000, B01110, B10001, B10001, B10001, B01110, B00000, B00000},
  stepPass{B00000, B01110, B11111, B11111, B11111, B01110, B00000, B00000},

  subdivisionesCharArray{"12/1","1/8", "1/6", "1/4", "1/3","1/2", "3/2", "1/1", "3/4", "2/1", "3/8", "4/1"},
  subdivisionesComplejasNumChar{" 5"," 7"," 9","11","13","15"},
  subdivisionesComplejasDenChar{byte(2),byte(3),byte(4)},
  charEscalasNotas{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C"}
{
  bool updateLCD = true;
}

void DrawMenus::configurarLCD(){
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
void DrawMenus::update_LCD(){
  if (menusUI.menuActual == 0) drawMenuPrincipal();
  //else if (menusUI.menuActual == 1) drawMenu1();
  //else if (menusUI.menuActual == 2) drawMenu2();
  else if (menusUI.menuActual == 3) drawScreenPotes();
  else if (menusUI.menuActual == 4) drawScreenOctavas();
  else if (menusUI.menuActual == 5) drawScreenTodasOctavas();
  updateLCD = false; 
}
inline void DrawMenus::staticScreen1(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(5,0), lcd.print("SEQ: "), lcd.print("1");
  lcd.setCursor(3,1), lcd.print("MODE: "), lcd.print("NOTE");
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::staticScreen2(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(menusUI.subDivMode);
  if(menusUI.subDivMode == 0 || menusUI.subDivMode == 1){
    lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[menusUI.indiceSubdivisiones]);}
  else if(menusUI.subDivMode == 2){
    lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[menusUI.indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[menusUI.indComplexSubdivY]);
  }
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::staticScreen3(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(4,0),  lcd.write(byte(0)), lcd.print("PASOS: "), lcd.print(midiProg.nTotalSteps), lcd.write(byte(1));
  lcd.setCursor(0,1);
  for (int i = 0; i < midiProg.nTotalSteps; i++) {
    if (!midiUI.mutesValue[i]) {
      if (i == midiProg.nStep) {
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
inline void DrawMenus::drawMenuPrincipal() {
  if(menusUI.seleccion == 1 || menusUI.seleccion == 2){nScreen = 0;}
  else if(menusUI.seleccion == 3 || menusUI.seleccion == 4){nScreen = 1;}
  else if(menusUI.seleccion == 5){nScreen = 2;}

  switch(menusUI.seleccion){
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
      lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.setCursor(12,0), lcd.print(menusUI.subDivMode), lcd.write(byte(1));
      lcd.setCursor(5,1), lcd.print(" "), lcd.print(subdivisionesCharArray[menusUI.indiceSubdivisiones]),lcd.print(" ");
      if(menusUI.subDivMode == 0 || menusUI.subDivMode == 1){
        lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[menusUI.indiceSubdivisiones]);
        }
      else if(menusUI.subDivMode == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[menusUI.indComplexSubdivX]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[menusUI.indComplexSubdivY]);
      }
      break;
    case 4:
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), lcd.setCursor(3,0), lcd.print(" "), lcd.setCursor(13,0), lcd.print(" ");
      if(menusUI.subDivMode == 0 || menusUI.subDivMode == 1){
        lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[menusUI.indiceSubdivisiones]),lcd.write(byte(1));
        }
      else if(menusUI.subDivMode == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[menusUI.indComplexSubdivX]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[menusUI.indComplexSubdivY]);
        if(menusUI.selectNum){lcd.setCursor(6,1), lcd.blink();}
        else if(menusUI.selectDen){lcd.setCursor(8,1), lcd.blink();}
      }
      break;
    case 5: 
      if(nScreen != nAnteriorScreen){staticScreen3();}
      lcd.noBlink(), lcd.setCursor(12,0), lcd.print(midiProg.nTotalSteps);
      lcd.setCursor(midiUI.indexMovedMute,1);
      if (!midiUI.mutesValue[midiUI.indexMovedMute] && midiUI.indexMovedMute <= midiProg.nTotalSteps - 1) {lcd.write(byte(6));}
      else if(midiUI.mutesValue[midiUI.indexMovedMute] && midiUI.indexMovedMute <= midiProg.nTotalSteps - 1){lcd.write(byte(5));}
      drawSteps();
    break;
  }
}

inline void DrawMenus::drawScreenPotes() {
  nScreen = 3;
  lcd.clear();
  lcd.print("Pote N");
  lcd.print((char) 223);
  lcd.print(midiUI.movedPoteNumber + 1);
  lcd.print(" : ");
  lcd.setCursor(0,1);
  if(midiUI.muteActivado){
    lcd.print("Velocity = ");
    lcd.print(midiUI.movedPoteValue);
    nAnteriorScreen = 5;
  }
  else{
    lcd.print(charEscalasNotas[midiUI.notesValue[int(midiUI.movedPoteNumber)]]);
    lcd.print(int(midiUI.movedPoteValue/12) + midiUI.octavaValue[midiUI.movedPoteNumber]);
    lcd.print(" ");
    lcd.print(midiUI.movedPoteValue + (12 * midiUI.octavaValue[midiUI.movedPoteNumber]));
    nAnteriorScreen = 5;
  }

}
inline void DrawMenus::drawScreenOctavas() {
  nScreen = 4;
  lcd.clear();
  lcd.print("Octava :");
  lcd.setCursor(0,1);
  lcd.print(charEscalasNotas[midiUI.notesValue[int(midiUI.movedPoteNumber)]]);
  lcd.print(int(midiUI.movedPoteValue/12) + midiUI.octavaValue[midiUI.indexMovedMute]);
  lcd.print(" ");
  lcd.print(midiUI.movedPoteValue + (12 * midiUI.octavaValue[midiUI.indexMovedMute]));
  nAnteriorScreen = 4;
}
inline void DrawMenus::drawScreenTodasOctavas() {
  nScreen = 5;
  lcd.clear();
  for(int i = 0; i < 4; i++){
    lcd.print(charEscalasNotas[midiUI.notesValue[i]]);
    lcd.print(int(midiUI.movedPoteValue/12) + midiUI.octavaValue[i]);
    lcd.print(" ");
  }
  lcd.setCursor(0,1);
  for(int i = 4; i < NUM_MUTES; i++){
    lcd.print(charEscalasNotas[midiUI.notesValue[i]]);
    lcd.print(int(midiUI.movedPoteValue/12) + midiUI.octavaValue[i]);
    lcd.print(" ");
  }
  nAnteriorScreen = 5;
}
inline void DrawMenus::drawSteps() {
  if(midiProg.lastNTotalSteps != midiProg.nTotalSteps){
    lcd.setCursor(midiProg.nTotalSteps - 1,1);
    if (!midiUI.mutesValue[midiProg.nTotalSteps - 1]) {
      if (midiProg.nTotalSteps == midiProg.nStep) {
        lcd.write(byte(7));
      }
      else {
        lcd.write(byte(6));
      }
    }
    else {
      lcd.write(byte(5));
    }
    lcd.print(" ");
    midiProg.lastNTotalSteps = midiProg.nTotalSteps;
  }
  if(midiUI.mutesValue[midiProg.nStep]){
    if(midiUI.mutesValue[midiProg.nStepAnterior]){
      lcd.setCursor(midiProg.nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(midiProg.nStep,1);
      lcd.write(byte(5));
    }
    else if(!midiUI.mutesValue[midiProg.nStepAnterior]){
      lcd.setCursor(midiProg.nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(midiProg.nStep,1);
      lcd.write(byte(5));
    }
  }
  else if(!midiUI.mutesValue[midiProg.nStep]){
    if(midiUI.mutesValue[midiProg.nStepAnterior]){
      lcd.setCursor(midiProg.nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(midiProg.nStep,1);
      lcd.write(byte(7));
    }
    else if(!midiUI.mutesValue[midiProg.nStepAnterior]){
      lcd.setCursor(midiProg.nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(midiProg.nStep,1);
      lcd.write(byte(7));
      
    }
  }
  midiProg.nStepAnterior = midiProg.nStep;
}

  
