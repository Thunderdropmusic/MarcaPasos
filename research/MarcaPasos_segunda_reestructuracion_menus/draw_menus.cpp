#include "draw_menus.h"
#include "midiProgramming.h"
#include "midiInterface.h"
#include "menus_buttons.h"
#include "midiPresets.h"
#include "globalVars.h"

#include <LiquidCrystal.h>

#define SEQ midiProg[presetsUI.indexNotesSequence]

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
  if (menusUI.menuActual == 0) drawMenuNotes();
  else if (menusUI.menuActual == 1) drawMenuCC();
  //else if (menusUI.menuActual == 2) drawMenu2();
  else if (menusUI.menuActual == 3) drawScreenPotes();
  else if (menusUI.menuActual == 4) drawScreenOctavas();
  else if (menusUI.menuActual == 5) drawScreenTodasOctavas();
  updateLCD = false; 
  interrupts();
}
inline void DrawMenus::staticScreen1(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(1,0), lcd.print("SEQ: ");
  lcd.setCursor(10,0), lcd.print("CC: ");
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::staticScreen2(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(globalSubdivMode[presetsUI.indexNotesSequence]);
  if(globalSubdivMode[presetsUI.indexNotesSequence] == 0 || globalSubdivMode[presetsUI.indexNotesSequence] == 1){
    lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[globalIndexSubdivisiones[presetsUI.indexNotesSequence]]);}
  else if(globalSubdivMode[presetsUI.indexNotesSequence] == 2){
    lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[globalIndComplexSubdivX[presetsUI.indexNotesSequence]]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[globalIndComplexSubdivY[presetsUI.indexNotesSequence]]);
  }
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::staticScreen3(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(4,0),  lcd.write(byte(0)), lcd.print("PASOS: "), lcd.print(globalNTotalSteps[presetsUI.indexNotesSequence]), lcd.write(byte(1));
  lcd.setCursor(0,1);
  for (int i = 0; i < globalNTotalSteps[presetsUI.indexNotesSequence]; i++) {
    if (!globalMutes[presetsUI.indexNotesSequence][i]) {
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
  lcd.noBlink(), lcd.setCursor(0,0), lcd.print("CLOCK MODE: ");
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::drawMenuNotes() {
  if(menusUI.seleccion == 1 || menusUI.seleccion == 2){nScreen = 0;}
  else if(menusUI.seleccion == 3 || menusUI.seleccion == 4){nScreen = 1;}
  else if(menusUI.seleccion == 5){nScreen = 2;}
  else if(menusUI.seleccion == 6){nScreen = 3;}

  switch(menusUI.seleccion){
    case 1:
      if(nScreen != nAnteriorScreen){staticScreen1();}
      lcd.noBlink(), lcd.setCursor(0,0), lcd.write(byte(0)), lcd.setCursor(6,0), lcd.print(presetsUI.indexNotesSequence), lcd.write(byte(1));
      lcd.setCursor(9,0), lcd.print(" ");
      if(globalCCSeqNumber[presetsUI.indexNotesSequence] == 0){lcd.setCursor(14,0), lcd.print(globalCCSeqNumber[presetsUI.indexNotesSequence]), lcd.print(" ");}
      else if(globalCCSeqNumber[presetsUI.indexNotesSequence] == 1){lcd.setCursor(14,0), lcd.print(globalCCSeqNumber[presetsUI.indexNotesSequence]),lcd.print(" ");}
      break;
    case 3: 
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.setCursor(12,0), lcd.print(globalSubdivMode[presetsUI.indexNotesSequence]), lcd.write(byte(1));
      lcd.setCursor(5,1), lcd.print(" "), lcd.print(subdivisionesCharArray[globalIndexSubdivisiones[presetsUI.indexNotesSequence]]),lcd.print(" ");
      if(globalSubdivMode[presetsUI.indexNotesSequence] == 0 || globalSubdivMode[presetsUI.indexNotesSequence] == 1){
        lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[globalIndexSubdivisiones[presetsUI.indexNotesSequence]]);
        }
      else if(globalSubdivMode[presetsUI.indexNotesSequence] == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[globalIndComplexSubdivX[presetsUI.indexNotesSequence]]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[globalIndComplexSubdivY[presetsUI.indexNotesSequence]]);
      }
      break;
    case 4:
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), lcd.setCursor(3,0), lcd.print(" "), lcd.setCursor(13,0), lcd.print(" ");
      if(globalSubdivMode[presetsUI.indexNotesSequence] == 0 || globalSubdivMode[presetsUI.indexNotesSequence] == 1){
        lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[globalIndexSubdivisiones[presetsUI.indexNotesSequence]]),lcd.write(byte(1));
      }
      else if(globalSubdivMode[presetsUI.indexNotesSequence] == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[globalIndComplexSubdivX[presetsUI.indexNotesSequence]]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[globalIndComplexSubdivY[presetsUI.indexNotesSequence]]);
        if(menusUI.selectNum){lcd.setCursor(6,1), lcd.blink();}
        else if(menusUI.selectDen){lcd.setCursor(8,1), lcd.blink();}
      }
      break;
    case 5: 
      if(nScreen != nAnteriorScreen){staticScreen3();}
      lcd.noBlink(), lcd.setCursor(12,0), lcd.print(globalNTotalSteps[presetsUI.indexNotesSequence]);
      lcd.setCursor(midiUI.indexMovedMute,1);
      if (!globalMutes[presetsUI.indexNotesSequence][midiUI.indexMovedMute] && midiUI.indexMovedMute <= globalNTotalSteps[presetsUI.indexNotesSequence] - 1) {lcd.write(byte(6));}
      else if(globalMutes[presetsUI.indexNotesSequence][midiUI.indexMovedMute] && midiUI.indexMovedMute <= globalNTotalSteps[presetsUI.indexNotesSequence] - 1){lcd.write(byte(5));}
      drawSteps();
    break;
    case 6:
      if(nScreen != nAnteriorScreen){staticScreen4();}
      lcd.noBlink(), lcd.setCursor(0,1), lcd.write(byte(0));
      if(SEQ.modeMidiClock == 0) {lcd.print("External"), lcd.write(byte(1));}
      else if(MidiProgramming::modeMidiClock == 1) {lcd.print("Internal"), lcd.write(byte(1));}
  }
}

inline void DrawMenus::drawMenuCC(){
  if(nScreen != nAnteriorScreen){staticScreen1();}
  lcd.noBlink(), lcd.setCursor(0,0), lcd.print(" "), lcd.setCursor(6,0), lcd.print(presetsUI.indexNotesSequence), lcd.print(" ");
  lcd.setCursor(9,0), lcd.write(byte(0));
  if(globalCCSeqNumber[presetsUI.indexNotesSequence] < 4){lcd.setCursor(14,0), lcd.print(globalCCSeqNumber[presetsUI.indexNotesSequence]), lcd.write(byte(1));}
  else if(globalCCSeqNumber[presetsUI.indexNotesSequence] > 0){lcd.setCursor(14,0), lcd.print(globalCCSeqNumber[presetsUI.indexNotesSequence]), lcd.write(byte(1)),lcd.print(" ");}
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
    if(globalCCSeqNumber[presetsUI.indexNotesSequence] == 0){
      lcd.print(charEscalasNotas[globalNote[presetsUI.indexNotesSequence][midiUI.movedPoteNumber]]);
      lcd.print(int(midiUI.movedPoteValue/12) + globalOctave[presetsUI.indexNotesSequence][midiUI.movedPoteNumber]);
      lcd.print(" ");
      lcd.print(midiUI.movedPoteValue + (12 * globalOctave[presetsUI.indexNotesSequence][midiUI.movedPoteNumber]));
      nAnteriorScreen = 4;
    }
    else if(globalCCSeqNumber[presetsUI.indexNotesSequence] == 1){
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
  lcd.print(charEscalasNotas[globalNote[presetsUI.indexNotesSequence][midiUI.indexMovedMute]]);
  lcd.print(int(globalNote[presetsUI.indexNotesSequence][midiUI.indexMovedMute]/12) + globalOctave[presetsUI.indexNotesSequence][midiUI.indexMovedMute]);
  lcd.print(" ");
  lcd.print(globalNote[presetsUI.indexNotesSequence][midiUI.indexMovedMute] + (12 * globalOctave[presetsUI.indexNotesSequence][midiUI.indexMovedMute]));
  nAnteriorScreen = 5;
}
inline void DrawMenus::drawScreenTodasOctavas() {
  nScreen = 6;
  lcd.clear();
  for(byte i = 0; i < 4; i++){
    lcd.print(charEscalasNotas[globalNote[presetsUI.indexNotesSequence][i]]);
    lcd.print(int(globalNote[presetsUI.indexNotesSequence][i]/12) + globalOctave[presetsUI.indexNotesSequence][i]);
    lcd.print(" ");
  }
  lcd.setCursor(0,1);
  for(byte i = 4; i < NUM_MUTES; i++){
    lcd.print(charEscalasNotas[globalNote[presetsUI.indexNotesSequence][i]]);
    lcd.print(int(globalNote[presetsUI.indexNotesSequence][i]/12) + globalOctave[presetsUI.indexNotesSequence][i]);
    lcd.print(" ");
  }
  nAnteriorScreen = 6;
}
inline void DrawMenus::drawSteps() {
  if(lastNTotalSteps != globalNTotalSteps[presetsUI.indexNotesSequence]){
    lcd.setCursor(globalNTotalSteps[presetsUI.indexNotesSequence] - 1, 1);
    if (!globalMutes[presetsUI.indexNotesSequence][globalNTotalSteps[presetsUI.indexNotesSequence] - 1]) {
      if (globalNTotalSteps[presetsUI.indexNotesSequence] == SEQ.nStep) {
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
    lastNTotalSteps = globalNTotalSteps[presetsUI.indexNotesSequence];
  }
  if(globalMutes[presetsUI.indexNotesSequence][SEQ.nStep]){
    if(globalMutes[presetsUI.indexNotesSequence][nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(SEQ.nStep,1);
      lcd.write(byte(5));
    }
    else if(!globalMutes[presetsUI.indexNotesSequence][nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(SEQ.nStep,1);
      lcd.write(byte(5));
    }
  }
  else if(!globalMutes[presetsUI.indexNotesSequence][SEQ.nStep]){
    if(globalMutes[presetsUI.indexNotesSequence][nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(SEQ.nStep,1);
      lcd.write(byte(7));
    }
    else if(!globalMutes[presetsUI.indexNotesSequence][nStepAnterior]){
      lcd.setCursor(nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(SEQ.nStep,1);
      lcd.write(byte(7));
      
    }
  }
  nStepAnterior = SEQ.nStep;
}

  
