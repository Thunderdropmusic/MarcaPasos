#include "draw_menus.h"
#include "midiProgramming.h"
#include "midiInterface.h"
#include "menus_buttons.h"
#include "midiPresets.h"
#include "globalVars.h"

#include <LiquidCrystal.h>

#define SEQ midiProg[presetsUI.indexSequence]

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

  subdivisionesCharArray{"1/8", "1/6", "1/4", "1/3", "1/2", "3/2", "1/1", "3/4", "2/1", "3/8", "4/1"},
  subdivisionesComplejasNumChar{" 5"," 7"," 9","11","13","15"},
  subdivisionesComplejasDenChar{byte(2),byte(3),byte(4)},
  charEscalasNotas{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C"}
{
  updateLCD = true;
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
  noInterrupts();
  if (menusUI.menuActual == 0) drawMenuPrincipal();
  //else if (menusUI.menuActual == 1) drawMenu1();
  //else if (menusUI.menuActual == 2) drawMenu2();
  else if (menusUI.menuActual == 3) drawScreenPotes();
  else if (menusUI.menuActual == 4) drawScreenOctavas();
  else if (menusUI.menuActual == 5) drawScreenTodasOctavas();
  updateLCD = false; 
  interrupts();
}
inline void DrawMenus::staticScreen1(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(5,0), lcd.print("SEQ: ");
  lcd.setCursor(3,1), lcd.print("MODE: ");
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::staticScreen2(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(globalSubdivMode[presetsUI.indexSequence]);
  if(globalSubdivMode[presetsUI.indexSequence] == 0 || globalSubdivMode[presetsUI.indexSequence] == 1){
    lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[globalIndexSubdivisiones[presetsUI.indexSequence]]);}
  else if(globalSubdivMode[presetsUI.indexSequence] == 2){
    lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[globalIndComplexSubdivX[presetsUI.indexSequence]]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[globalIndComplexSubdivY[presetsUI.indexSequence]]);
  }
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::staticScreen3(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(4,0),  lcd.write(byte(0)), lcd.print("PASOS: "), lcd.print(globalNTotalSteps[presetsUI.indexSequence]), lcd.write(byte(1));
  lcd.setCursor(0,1);
  for (int i = 0; i < globalNTotalSteps[presetsUI.indexSequence]; i++) {
    if (!globalMutes[presetsUI.indexSequence][i]) {
      if (i == SEQ.nStep) {
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
inline void DrawMenus::staticScreen4(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(0,0), lcd.print("CLOCK MODE: "), lcd.print(globalNTotalSteps[presetsUI.indexSequence]);
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::drawMenuPrincipal() {
  if(menusUI.seleccion == 1 || menusUI.seleccion == 2){nScreen = 0;}
  else if(menusUI.seleccion == 3 || menusUI.seleccion == 4){nScreen = 1;}
  else if(menusUI.seleccion == 5){nScreen = 2;}
  else if(menusUI.seleccion == 6){nScreen = 3;}

  switch(menusUI.seleccion){
    case 1:
      if(nScreen != nAnteriorScreen){staticScreen1();}
      lcd.noBlink(), lcd.setCursor(4,0), lcd.write(byte(0)), lcd.setCursor(10,0), lcd.print(presetsUI.indexSequence), lcd.write(byte(1));
      lcd.setCursor(2,1), lcd.print(" ");
      if(globalModeSequence[presetsUI.indexSequence] == 0){lcd.setCursor(9,1), lcd.print("NOTES"), lcd.print(" ");}
      else if(globalModeSequence[presetsUI.indexSequence] == 1){lcd.setCursor(9,1), lcd.print("CC"),lcd.print("   ");}
      break;
    case 2:
      if(nScreen != nAnteriorScreen){staticScreen1();}
      lcd.noBlink(), lcd.setCursor(4,0), lcd.print(" "), lcd.setCursor(10,0), lcd.print(presetsUI.indexSequence), lcd.print(" ");
      lcd.setCursor(2,1), lcd.write(byte(0));
      if(globalModeSequence[presetsUI.indexSequence] == 0){lcd.setCursor(9,1), lcd.print("NOTES"), lcd.write(byte(1));}
      else if(globalModeSequence[presetsUI.indexSequence] == 1){lcd.setCursor(9,1), lcd.print("CC"), lcd.write(byte(1)),lcd.print("   ");}
      break;
    case 3: 
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.setCursor(12,0), lcd.print(globalSubdivMode[presetsUI.indexSequence]), lcd.write(byte(1));
      lcd.setCursor(5,1), lcd.print(" "), lcd.print(subdivisionesCharArray[globalIndexSubdivisiones[presetsUI.indexSequence]]),lcd.print(" ");
      if(globalSubdivMode[presetsUI.indexSequence] == 0 || globalSubdivMode[presetsUI.indexSequence] == 1){
        lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[globalIndexSubdivisiones[presetsUI.indexSequence]]);
        }
      else if(globalSubdivMode[presetsUI.indexSequence] == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[globalIndComplexSubdivX[presetsUI.indexSequence]]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[globalIndComplexSubdivY[presetsUI.indexSequence]]);
      }
      break;
    case 4:
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), lcd.setCursor(3,0), lcd.print(" "), lcd.setCursor(13,0), lcd.print(" ");
      if(globalSubdivMode[presetsUI.indexSequence] == 0 || globalSubdivMode[presetsUI.indexSequence] == 1){
        lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[globalIndexSubdivisiones[presetsUI.indexSequence]]),lcd.write(byte(1));
      }
      else if(globalSubdivMode[presetsUI.indexSequence] == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[globalIndComplexSubdivX[presetsUI.indexSequence]]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[globalIndComplexSubdivY[presetsUI.indexSequence]]);
        if(menusUI.selectNum){lcd.setCursor(6,1), lcd.blink();}
        else if(menusUI.selectDen){lcd.setCursor(8,1), lcd.blink();}
      }
      break;
    case 5: 
      if(nScreen != nAnteriorScreen){staticScreen3();}
      lcd.noBlink(), lcd.setCursor(12,0), lcd.print(globalNTotalSteps[presetsUI.indexSequence]);
      lcd.setCursor(midiUI.indexMovedMute,1);
      if (!globalMutes[presetsUI.indexSequence][midiUI.indexMovedMute] && midiUI.indexMovedMute <= globalNTotalSteps[presetsUI.indexSequence] - 1) {lcd.write(byte(6));}
      else if(globalMutes[presetsUI.indexSequence][midiUI.indexMovedMute] && midiUI.indexMovedMute <= globalNTotalSteps[presetsUI.indexSequence] - 1){lcd.write(byte(5));}
      drawSteps();
    break;
    case 6:
      if(nScreen != nAnteriorScreen){staticScreen4();}
      lcd.noBlink(), lcd.setCursor(0,1), lcd.write(byte(0));
      if(SEQ.modeMidiClock == 0) {lcd.print("External"), lcd.write(byte(1));}
      else if(MidiProgramming::modeMidiClock == 1) {lcd.print("Internal"), lcd.write(byte(1));}
  }
}

inline void DrawMenus::drawScreenPotes(){
  nScreen = 4;
  lcd.clear();
  lcd.print("Pote N");
  lcd.print((char) 223);
  lcd.print(midiUI.movedPoteNumber + 1);
  lcd.print(" : ");
  lcd.setCursor(0,1);
  if(midiUI.muteActivado){
    lcd.print("Velocity = ");
    lcd.print(midiUI.movedPoteValue);
    nAnteriorScreen = 4;
  }
  else{
    if(globalModeSequence[presetsUI.indexSequence] == 0){
      lcd.print(charEscalasNotas[globalNote[presetsUI.indexSequence][midiUI.movedPoteNumber]]);
      lcd.print(int(midiUI.movedPoteValue/12) + globalOctave[presetsUI.indexSequence][midiUI.movedPoteNumber]);
      lcd.print(" ");
      lcd.print(midiUI.movedPoteValue + (12 * globalOctave[presetsUI.indexSequence][midiUI.movedPoteNumber]));
      nAnteriorScreen = 4;
    }
    else if(globalModeSequence[presetsUI.indexSequence] == 1){
      lcd.print("CC Value = ");
      lcd.print(midiUI.movedPoteValue);
      nAnteriorScreen = 4;
    }
  }

}
inline void DrawMenus::drawScreenOctavas() {
  nScreen = 5;
  lcd.clear();
  lcd.print("Octava :");
  lcd.setCursor(0,1);
  lcd.print(charEscalasNotas[globalNote[presetsUI.indexSequence][midiUI.indexMovedMute]]);
  lcd.print(int(globalNote[presetsUI.indexSequence][midiUI.indexMovedMute]/12) + globalOctave[presetsUI.indexSequence][midiUI.indexMovedMute]);
  lcd.print(" ");
  lcd.print(globalNote[presetsUI.indexSequence][midiUI.indexMovedMute] + (12 * globalOctave[presetsUI.indexSequence][midiUI.indexMovedMute]));
  nAnteriorScreen = 5;
}
inline void DrawMenus::drawScreenTodasOctavas() {
  nScreen = 6;
  lcd.clear();
  for(byte i = 0; i < 4; i++){
    lcd.print(charEscalasNotas[globalNote[presetsUI.indexSequence][i]]);
    lcd.print(int(globalNote[presetsUI.indexSequence][i]/12) + globalOctave[presetsUI.indexSequence][i]);
    lcd.print(" ");
  }
  lcd.setCursor(0,1);
  for(byte i = 4; i < NUM_MUTES; i++){
    lcd.print(charEscalasNotas[globalNote[presetsUI.indexSequence][i]]);
    lcd.print(int(globalNote[presetsUI.indexSequence][i]/12) + globalOctave[presetsUI.indexSequence][i]);
    lcd.print(" ");
  }
  nAnteriorScreen = 6;
}
inline void DrawMenus::drawSteps() {
  if(lastNTotalSteps != globalNTotalSteps[presetsUI.indexSequence]){
    lcd.setCursor(globalNTotalSteps[presetsUI.indexSequence] - 1, 1);
    if (!globalMutes[presetsUI.indexSequence][globalNTotalSteps[presetsUI.indexSequence] - 1]) {
      if (globalNTotalSteps[presetsUI.indexSequence] == SEQ.nStep) {
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
    lastNTotalSteps = globalNTotalSteps[presetsUI.indexSequence];
  }
  if(globalMutes[presetsUI.indexSequence][SEQ.nStep]){
    if(globalMutes[presetsUI.indexSequence][nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(SEQ.nStep,1);
      lcd.write(byte(5));
    }
    else if(!globalMutes[presetsUI.indexSequence][nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(SEQ.nStep,1);
      lcd.write(byte(5));
    }
  }
  else if(!globalMutes[presetsUI.indexSequence][SEQ.nStep]){
    if(globalMutes[presetsUI.indexSequence][nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(SEQ.nStep,1);
      lcd.write(byte(7));
    }
    else if(!globalMutes[presetsUI.indexSequence][nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(SEQ.nStep,1);
      lcd.write(byte(7));
      
    }
  }
  nStepAnterior = SEQ.nStep;
}

  
