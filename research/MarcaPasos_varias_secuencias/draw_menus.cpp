#include "draw_menus.h"
#include "midiProgramming.h"
#include "midiInterface.h"
#include "menus_buttons.h"
#include "midiPresets.h"

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
  lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(SUBDIV_MODE());
  if(SUBDIV_MODE() == 0 || SUBDIV_MODE() == 1){
    lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[INDEX_SUBDIVISIONES()]);}
  else if(SUBDIV_MODE() == 2){
    lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[INDEX_COMPLEX_X()]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[INDEX_COMPLEX_Y()]);
  }
  nAnteriorScreen = nScreen;
}
inline void DrawMenus::staticScreen3(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(4,0),  lcd.write(byte(0)), lcd.print("PASOS: "), lcd.print(NTOTALSTEPS()), lcd.write(byte(1));
  lcd.setCursor(0,1);
  for (int i = 0; i < NTOTALSTEPS(); i++) {
    if (!MUTES(i)) {
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
inline void DrawMenus::staticScreen4(){
  lcd.clear();
  lcd.noBlink(), lcd.setCursor(0,0), lcd.print("CLOCK MODE: "), lcd.print(NTOTALSTEPS());
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
      if(MODE_SEQUENCE() == 0){lcd.setCursor(9,1), lcd.print("NOTES"), lcd.print(" ");}
      else if(MODE_SEQUENCE() == 1){lcd.setCursor(9,1), lcd.print("CC"),lcd.print("   ");}
      break;
    case 2:
      if(nScreen != nAnteriorScreen){staticScreen1();}
      lcd.noBlink(), lcd.setCursor(4,0), lcd.print(" "), lcd.setCursor(10,0), lcd.print(presetsUI.indexSequence), lcd.print(" ");
      lcd.setCursor(2,1), lcd.write(byte(0));
      if(MODE_SEQUENCE() == 0){lcd.setCursor(9,1), lcd.print("NOTES"), lcd.write(byte(1));}
      else if(MODE_SEQUENCE() == 1){lcd.setCursor(9,1), lcd.print("CC"), lcd.write(byte(1)),lcd.print("   ");}
      break;
    case 3: 
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.setCursor(12,0), lcd.print(SUBDIV_MODE()), lcd.write(byte(1));
      lcd.setCursor(5,1), lcd.print(" "), lcd.print(subdivisionesCharArray[INDEX_SUBDIVISIONES()]),lcd.print(" ");
      if(SUBDIV_MODE() == 0 || SUBDIV_MODE() == 1){
        lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[INDEX_SUBDIVISIONES()]);
        }
      else if(SUBDIV_MODE() == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[INDEX_COMPLEX_X()]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[INDEX_COMPLEX_Y()]);
      }
      break;
    case 4:
      if(nScreen != nAnteriorScreen){staticScreen2();}
      lcd.noBlink(), lcd.setCursor(3,0), lcd.print(" "), lcd.setCursor(13,0), lcd.print(" ");
      if(SUBDIV_MODE() == 0 || SUBDIV_MODE() == 1){
        lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[INDEX_SUBDIVISIONES()]),lcd.write(byte(1));
      }
      else if(SUBDIV_MODE() == 2){
        lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[INDEX_COMPLEX_X()]), lcd.setCursor(8,1), lcd.write(subdivisionesComplejasDenChar[INDEX_COMPLEX_Y()]);
        if(menusUI.selectNum){lcd.setCursor(6,1), lcd.blink();}
        else if(menusUI.selectDen){lcd.setCursor(8,1), lcd.blink();}
      }
      break;
    case 5: 
      if(nScreen != nAnteriorScreen){staticScreen3();}
      lcd.noBlink(), lcd.setCursor(12,0), lcd.print(NTOTALSTEPS());
      lcd.setCursor(midiUI.indexMovedMute,1);
      if (!MUTES(midiUI.indexMovedMute) && midiUI.indexMovedMute <= NTOTALSTEPS() - 1) {lcd.write(byte(6));}
      else if(MUTES(midiUI.indexMovedMute) && midiUI.indexMovedMute <= NTOTALSTEPS() - 1){lcd.write(byte(5));}
      drawSteps();
    break;
    case 6:
      if(nScreen != nAnteriorScreen){staticScreen4();}
      lcd.noBlink(), lcd.setCursor(0,1), lcd.write(byte(0));
      if(midiProg.modeMidiClock == 0) {lcd.print("External"), lcd.write(byte(1));}
      else if(midiProg.modeMidiClock == 1) {lcd.print("Internal"), lcd.write(byte(1));}
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
    lcd.print(charEscalasNotas[NOTA(midiUI.movedPoteNumber)]);
    lcd.print(int(midiUI.movedPoteValue/12) + OCTAVA(midiUI.movedPoteNumber));
    lcd.print(" ");
    lcd.print(midiUI.movedPoteValue + (12 * OCTAVA(midiUI.movedPoteNumber)));
    nAnteriorScreen = 4;
  }

}
inline void DrawMenus::drawScreenOctavas() {
  nScreen = 5;
  lcd.clear();
  lcd.print("Octava :");
  lcd.setCursor(0,1);
  lcd.print(charEscalasNotas[OCTAVA(midiUI.indexMovedMute)]);
  lcd.print(int(OCTAVA(midiUI.indexMovedMute)/12) + OCTAVA(midiUI.indexMovedMute));
  lcd.print(" ");
  lcd.print(OCTAVA(midiUI.indexMovedMute) + (12 * OCTAVA(midiUI.indexMovedMute)));
  nAnteriorScreen = 5;
}
inline void DrawMenus::drawScreenTodasOctavas() {
  nScreen = 6;
  lcd.clear();
  for(int i = 0; i < 4; i++){
    byte &NOTA_OCTAVA = SEQ_ACTUAL().steps[i].note;
    lcd.print(charEscalasNotas[NOTA_OCTAVA]);
    lcd.print(int(NOTA_OCTAVA/12) + OCTAVA(i));
    lcd.print(" ");
  }
  lcd.setCursor(0,1);
  for(int i = 4; i < NUM_MUTES; i++){
    byte &NOTA_OCTAVA = SEQ_ACTUAL().steps[i].note;
    lcd.print(charEscalasNotas[NOTA_OCTAVA]);
    lcd.print(int(NOTA_OCTAVA/12) + OCTAVA(i));
    lcd.print(" ");
  }
  nAnteriorScreen = 6;
}
inline void DrawMenus::drawSteps() {
  if(lastNTotalSteps != NTOTALSTEPS()){
    lcd.setCursor(NTOTALSTEPS() - 1,1);
    if (MUTES(NTOTALSTEPS() - 1)) {
      if (NTOTALSTEPS() == midiProg.nStep) {
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
    lastNTotalSteps = NTOTALSTEPS();
  }
  if(MUTES(midiProg.nStep)){
    if(MUTES(midiProg.nStepAnterior)){
      lcd.setCursor(midiProg.nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(midiProg.nStep,1);
      lcd.write(byte(5));
    }
    else if(!MUTES(midiProg.nStepAnterior)){
      lcd.setCursor(midiProg.nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(midiProg.nStep,1);
      lcd.write(byte(5));
    }
  }
  else if(!MUTES(midiProg.nStep)){
    if(MUTES(midiProg.nStepAnterior)){
      lcd.setCursor(midiProg.nStepAnterior, 1);
      lcd.write(byte(5));
      lcd.setCursor(midiProg.nStep,1);
      lcd.write(byte(7));
    }
    else if(!MUTES(midiProg.nStepAnterior)){
      lcd.setCursor(midiProg.nStepAnterior, 1);
      lcd.write(byte(6));
      lcd.setCursor(midiProg.nStep,1);
      lcd.write(byte(7));
      
    }
  }
  midiProg.nStepAnterior = midiProg.nStep;
}

  
