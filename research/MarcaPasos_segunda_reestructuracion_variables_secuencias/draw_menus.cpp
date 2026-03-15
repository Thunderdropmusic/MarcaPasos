#include "draw_menus.h"
#include "midiProgramming.h"
#include "midiInterface.h"
#include "menus_buttons.h"
#include "midiPresets.h"

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
  nAnteriorScreen = 999;
}


void DrawMenus::syncWithActiveSequence() {
  s = presetsUI.getActiveSequence();
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

void DrawMenus::printAt(byte x, byte y, byte var){
  lcd.setCursor(x, y);
  lcd.print(var);
}

void DrawMenus::printAt(byte x, byte y, char text){
  lcd.setCursor(x, y);
  lcd.print(text);
}

void DrawMenus::printAt(byte x, byte y, const char* text){
  lcd.setCursor(x, y);
  lcd.print(text);
}

void DrawMenus::writeAt(byte x, byte y, byte numByte){
  lcd.setCursor(x, y);
  lcd.write(numByte);
}

void DrawMenus::update_LCD(){
  syncWithActiveSequence();
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
  lcd.noBlink(), printAt(1, 0, "SEQ  "), lcd.write(126);
  writeAt(10, 0, 127), lcd.print("  CC");
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::staticScreen2(){
  lcd.clear();
  lcd.noBlink(), printAt(4, 0, "SUBDIV: "), lcd.print(s->subdivMode);
  if(s->subdivMode == 0 || s->subdivMode == 1){
    printAt(6, 1, subdivisionesCharArray[s->indexSubdivisiones]);}
  else if(s->subdivMode == 2){
    printAt(5, 1, subdivisionesComplejasNumChar[s->indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[s->indComplexSubdivY]);
  }
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::staticScreen3(){
  lcd.clear();
  lcd.noBlink(), writeAt(4, 0, byte(0)), lcd.print("PASOS: "), lcd.print(s->nTotalSteps), lcd.write(byte(1));
  lcd.setCursor(0,1);
  for (int i = 0; i < s->nTotalSteps; i++) {
    if (!s->steps[i].mutes) {
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
  lcd.noBlink(), printAt(0,0,"CLOCK MODE: ");
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
      lcd.noBlink(), printAt(0, 0, "["), printAt(4,0,"]"); printAt(8,0, presetsUI.indexSequence), 
      printAt(12, 0," "), printAt(15, 0," ");
      break;
    case 3: 
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), writeAt(3,0,byte(0)), printAt(12, 0, s->subdivMode), lcd.write(byte(1));
      printAt(5, 1, " "), lcd.print(subdivisionesCharArray[s->indexSubdivisiones]),lcd.print(" ");
      if(s->subdivMode == 0 || s->subdivMode == 1){
        printAt(6, 1, subdivisionesCharArray[s->indexSubdivisiones]);
        }
      else if(s->subdivMode == 2){
        printAt(5, 1, subdivisionesComplejasNumChar[s->indComplexSubdivX]), writeAt(8, 1, subdivisionesComplejasDenChar[s->indComplexSubdivY]);
      }
      break;
    case 4:
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), printAt(3, 0, " "), printAt(13, 0, " ");
      if(s->subdivMode == 0 || s->subdivMode == 1){
        writeAt(5, 1, byte(0)), lcd.print(subdivisionesCharArray[s->indexSubdivisiones]), lcd.write(byte(1));
      }
      else if(s->subdivMode == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[s->indComplexSubdivX]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[s->indComplexSubdivY]);
        if(menusUI.selectNum){lcd.setCursor(6,1), lcd.blink();}
        else if(menusUI.selectDen){lcd.setCursor(8,1), lcd.blink();}
      }
      break;
    case 5: 
      if(nScreen != nAnteriorScreen){staticScreen3();}
      lcd.noBlink(), lcd.setCursor(12,0), lcd.print(s->nTotalSteps);
      lcd.setCursor(midiUI.indexMovedMute,1);
      if (!s->steps[midiUI.indexMovedMute].mutes && midiUI.indexMovedMute <= s->nTotalSteps - 1) {lcd.write(byte(6));}
      else if(s->steps[midiUI.indexMovedMute].mutes && midiUI.indexMovedMute <= s->nTotalSteps - 1){lcd.write(byte(5));}
      drawSteps();
    break;
    case 6:
      if(nScreen != nAnteriorScreen){staticScreen4();}
      lcd.noBlink(), writeAt(0, 1, byte(0));
      if(SEQ.modeMidiClock == 0) {lcd.print("External"), lcd.write(byte(1));}
      else if(MidiProgramming::modeMidiClock == 1) {lcd.print("Internal"), lcd.write(byte(1));}
  }
}

inline void DrawMenus::drawMenuCC(){
  if(nScreen != nAnteriorScreen){staticScreen1();}
  lcd.noBlink(), printAt(0, 0, " "), printAt(4, 0, " "), printAt(8,0, presetsUI.indexSequence);
  printAt(12, 0, "["), printAt(15, 0, "]");
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
    if(menusUI.selectCC == 0){
      lcd.print(charEscalasNotas[s->steps[midiUI.movedPoteNumber].note]);
      lcd.print(int(midiUI.movedPoteValue/12) + s->steps[midiUI.movedPoteNumber].octave);
      lcd.print(" ");
      lcd.print(midiUI.movedPoteValue + (12 * s->steps[midiUI.movedPoteNumber].octave));
      nAnteriorScreen = 4;
    }
    else if(menusUI.selectCC == 1){
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
  printAt(0, 1, charEscalasNotas[s->steps[midiUI.indexMovedMute].note]);
  lcd.print(int(s->steps[midiUI.indexMovedMute].note/12) + s->steps[midiUI.indexMovedMute].octave);
  lcd.print(" ");
  lcd.print(s->steps[midiUI.indexMovedMute].note + (12 * s->steps[midiUI.indexMovedMute].octave));
  nAnteriorScreen = 5;
}
inline void DrawMenus::drawScreenTodasOctavas() {
  nScreen = 6;
  lcd.clear();
  for(byte i = 0; i < 4; i++){
    lcd.print(charEscalasNotas[s->steps[i].note]);
    lcd.print(int(s->steps[i].note/12) + s->steps[i].octave);
    lcd.print(" ");
  }
  lcd.setCursor(0,1);
  for(byte i = 4; i < NUM_MUTES; i++){
    lcd.print(charEscalasNotas[s->steps[i].note]);
    lcd.print(int(s->steps[i].note/12) + s->steps[i].octave);
    lcd.print(" ");
  }
  nAnteriorScreen = 6;
}
inline void DrawMenus::drawSteps() {
  if(lastNTotalSteps != s->nTotalSteps){
    lcd.setCursor(s->nTotalSteps - 1, 1);
    if (!s->steps[s->nTotalSteps - 1].mutes) {
      if (s->nTotalSteps == SEQ.nStep) {
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
    lastNTotalSteps = s->nTotalSteps;
  }
  if(s->steps[SEQ.nStep].mutes){
    if(s->steps[nStepAnterior].mutes){
      writeAt(nStepAnterior, 1, byte(5));
      writeAt(SEQ.nStep, 1, byte(5));
    }
    else if(!s->steps[nStepAnterior].mutes){
      writeAt(nStepAnterior, 1, byte(6));
      writeAt(SEQ.nStep, 1, byte(5));
    }
  }
  else if(!s->steps[SEQ.nStep].mutes){
    if(s->steps[nStepAnterior].mutes){
      writeAt(nStepAnterior, 1, byte(5));
      writeAt(SEQ.nStep, 1, byte(7));
    }
    else if(!s->steps[nStepAnterior].mutes){
      writeAt(nStepAnterior, 1, byte(6));
      writeAt(SEQ.nStep, 1, byte(7));
      
    }
  }
  nStepAnterior = SEQ.nStep;
}

  
