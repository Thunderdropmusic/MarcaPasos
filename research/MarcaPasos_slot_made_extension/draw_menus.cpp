#include "draw_menus.h"
#include "midiProgramming.h"
#include "midiInterface.h"
#include "menus_buttons.h"
#include "midiPresets.h"
#include "config.h"

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define SEQ midiProg[presetsUI.indexSequence]

DrawMenus drawUI;

DrawMenus::DrawMenus() : lcd(0x27, 20, 4),

  // Carácteres
  arrowLeft{B00001, B00011, B00111, B01111, B01111, B00111, B00011, B00001},
  arrowRight{B10000, B11000, B11100, B11110, B11110, B11100, B11000, B10000},
  redonda{B00000, B00000, B00000, B01110, B10001, B10001, B01110, B00000},
  blanca{B00010, B00010, B00010, B00010, B01110, B10010, B10010, B01100},
  negra{B00010, B00010, B00010, B00010, B01110, B11110, B11110, B01100},
  stepOff{B00000, B00000, B00000, B01110, B00000, B00000, B00000, B00000},
  stepOn{B00000, B01110, B10001, B10001, B10001, B01110, B00000, B00000},
  stepPass{B00000, B01110, B11111, B11111, B11111, B01110, B00000, B00000},

  ccLevel0{0,0,0,0,0,0,0,31}, //Abajo
  ccLevel1{0,0,0,0,0,0,31,31},
  ccLevel2{0,0,0,0,0,31,31,31},
  ccLevel3{0,0,0,0,31,31,31,31},
  ccLevel4{0,0,0,31,31,31,31,31},
  ccLevel5{0,0,31,31,31,31,31,31},
  ccLevel6{0,31,31,31,31,31,31,31}, // Casi arriba

  subdivisionesCharArray{"1/8", "1/6", "1/4", "1/3", "1/2", "3/2", "1/1", "3/4", "2/1", "3/8", "4/1"},
  subdivisionesComplejasNumChar{" 5"," 7"," 9","11","13","15"},
  subdivisionesComplejasDenChar{byte(2),byte(3),byte(4)},
  charEscalasNotas{"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C"},
  nombreEscalas{"Chromatic", "Natural Minor", "Natural Major", "Tone Scale", "Semitone-Tone"}
{
  updateLCD = true;
  nAnteriorScreen = 999;
}


void DrawMenus::syncWithActiveSequence() {
  if(!menusUI.editExtension){s = presetsUI.getActiveSequence();}
  else{s = presetsUI.getExtensionSequence();}
}

void DrawMenus::configurarLCD(){
  lcd.init();
  Wire.setClock(400000);
  lcd.backlight();
}

void DrawMenus::charactersMenu1(){
  lcd.createChar(0,arrowLeft);
  lcd.createChar(1,arrowRight);
  lcd.createChar(2,redonda);
  lcd.createChar(3,blanca);
  lcd.createChar(4,negra);
  lcd.createChar(5,stepOff);
  lcd.createChar(6,stepOn);
  lcd.createChar(7,stepPass);
}

void DrawMenus::charactersMenu2(){

  lcd.createChar(0,ccLevel0);
  lcd.createChar(1,ccLevel1);
  lcd.createChar(2,ccLevel2);
  lcd.createChar(3,ccLevel3);
  lcd.createChar(4,ccLevel4);
  lcd.createChar(5,ccLevel5);
  lcd.createChar(6,ccLevel6);
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
  if (menusUI.menuActual == 0) drawMenuNotes();
  else if (menusUI.menuActual == 1) drawMenuCC();
  else if (menusUI.menuActual == 2) drawMenuSettings();
  else if (menusUI.menuActual == 3) drawScreenPotes();
  else if (menusUI.menuActual == 4) drawScreenOctavas();
  else if (menusUI.menuActual == 5) drawScreenTodasOctavas();
  else if (menusUI.menuActual == 6) drawLoadPreset(presetsUI.nSlot);
  else if (menusUI.menuActual == 7) drawConfirmationScreen();
  updateLCD = false; 
  interrupts();
  updateValues = false;
}


inline void DrawMenus::drawMenuNotes() {
  if(menusUI.seleccion == 1 || menusUI.seleccion == 2 || menusUI.seleccion == 3 || menusUI.seleccion == 4){nScreen = 0;}
  if(midiUI.deviceStop){
    printAt(2,3,"      STOP      ");
  }
  else{
    if(updateSteps){
      drawStaticSteps();
    }
    drawMutes();
    drawSteps(); 
  }
  if(nScreen != nAnteriorScreen || menusUI.menuAnterior != menusUI.menuActual || updateValues){staticScreen1();}
  switch(menusUI.seleccion){
    case 1:
      
      lcd.noBlink(), printAt(0, 0, "["), printAt(4,0,"]"); printAt(8,0, presetsUI.indexSequence), 
      printAt(10, 0," "), printAt(19, 0," ");
      printAt(5, 1, " "), printAt(15, 1, " ");
      break;
    case 2: 
      printAt(0, 0, " "), printAt(4, 0, " ");
      lcd.noBlink(), writeAt(5,1,byte(0)), printAt(14, 1, s->subdivMode), lcd.write(byte(1));
      printAt(8, 2, " "), lcd.print(subdivisionesCharArray[s->indexSubdivisiones]),lcd.print(" ");
      if(s->subdivMode == 0 || s->subdivMode == 1){
        printAt(9, 2, subdivisionesCharArray[s->indexSubdivisiones]);
        }
      else if(s->subdivMode == 2){
        printAt(8, 2, subdivisionesComplejasNumChar[s->indComplexSubdivX]), writeAt(11, 2, subdivisionesComplejasDenChar[s->indComplexSubdivY]);
      }
      break;
    case 3:
      lcd.noBlink(), printAt(0, 0, " "), printAt(4, 0, " "), printAt(8,0, presetsUI.indexSequence);
      printAt(5, 1, " "), printAt(15, 1, " ");
      if(s->subdivMode == 0 || s->subdivMode == 1){
        writeAt(8, 2, byte(0)), lcd.print(subdivisionesCharArray[s->indexSubdivisiones]), lcd.write(byte(1));
      }
      else if(s->subdivMode == 2){
        lcd.setCursor(8,2), lcd.print(subdivisionesComplejasNumChar[s->indComplexSubdivX]), lcd.setCursor(11,2), lcd.write(subdivisionesComplejasDenChar[s->indComplexSubdivY]);
        if(menusUI.selectNum){lcd.setCursor(9,2), lcd.blink();}
        else if(menusUI.selectDen){lcd.setCursor(11,2), lcd.blink();}
      }
      break;
    case 4: 
      printAt(0, 0, " "), printAt(4, 0, " ");
      lcd.noBlink(), writeAt(10, 0, byte(0)), writeAt(19, 0, byte(1));
      nAnteriorScreen = nScreen;
      lcd.noBlink(), printAt(18, 0, s->nTotalSteps);
      break;
  }
}

inline void DrawMenus::staticScreen1(){
  charactersMenu1();
  for(int i = 1; i < 4; i++){ // limpiado de la pantalla
    printAt(0, i, "                   ");
  }
  lcd.noBlink();
  if(!menusUI.editExtension){printAt(1, 0, "SEQ  ");}
  else{printAt(1, 0, "EXT  ");}
  lcd.write(126), lcd.print(" ");
  printAt(9, 0, "  "), lcd.print("PASOS: "), lcd.print(s->nTotalSteps);
  nAnteriorScreen = nScreen;
  lcd.noBlink(), printAt(6, 1, "SUBDIV: "), lcd.print(s->subdivMode);
  if(s->subdivMode == 0 || s->subdivMode == 1){
    printAt(9, 2, subdivisionesCharArray[s->indexSubdivisiones]);}
  else if(s->subdivMode == 2){
    printAt(8, 2, subdivisionesComplejasNumChar[s->indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[s->indComplexSubdivY]);
  }
  nAnteriorScreen = nScreen;
  menusUI.menuAnterior = menusUI.menuActual;
  drawStaticSteps();
}

inline void DrawMenus::drawStaticSteps(){
  if(SEQ.inExtension){s = presetsUI.getExtensionSequence();}
  if(midiUI.deviceStop){
    printAt(2,3,"      STOP      ");
    return;
  }
  lcd.setCursor(2,3);
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
  syncWithActiveSequence();
  updateSteps = false;
}
inline void DrawMenus::drawMenuCC(){
  if(menusUI.seleccion == 1) {nScreen = 0;}
  
  if(!midiUI.deviceStop){
    drawCCMutes();
  }

  else{
    if(midiUI.deviceStop){
      printAt(0,3,"        STOP        ");
    }
  }
  
  if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen2();}
  switch(menusUI.seleccion){
    case 1:
      lcd.noBlink(), printAt(8,0, presetsUI.indexSequence), printAt(10, 0, " "), printAt(19, 0, " ");
      printAt(0, 0, "["), printAt(3, 0, "] ");
      printAt(0, 1, midiUI.movedPoteValue);
      lcd.print("  ");
      lcd.setCursor(0, 2);
      drawCCVisualizer();
      break;

    case 2:
      printAt(0, 0, " "), printAt(3, 0, " "), 
      lcd.noBlink(), printAt(10, 0, "["), printAt(19, 0, "]");
      nAnteriorScreen = nScreen;
      lcd.noBlink(), printAt(18, 0, s->nTotalSteps);
      printAt(s->nTotalSteps, 2, " ");
      lcd.setCursor(0, 2);
      drawCCVisualizer();
      break;
  }
}

inline void DrawMenus::staticScreen2(){
  charactersMenu2();
  for(int i = 1; i < 4; i++){
    for(int j = 0; j < 19; j++){
      printAt(j,i, " ");
    }
  }
  lcd.noBlink(), writeAt(0, 0, 127), lcd.print("CC   "), lcd.write(126), lcd.print(" ");
  printAt(9, 0, "  "), lcd.print("PASOS: "), lcd.print(s->nTotalSteps);
  nAnteriorScreen = nScreen;
  menusUI.menuAnterior = menusUI.menuActual;
  lcd.setCursor(0,3);
  for (int i = 0; i < s->nTotalSteps; i++) {
    if (!s->steps[i].ccMutes) {
      lcd.print("+");
    }
    else {
      lcd.print(" ");
    }
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
    if(s->seqMode == 0){
      lcd.print("Velocity = ");
      lcd.print(midiUI.movedPoteValue);
      nAnteriorScreen = 4;
    }
  }
  else{
    if(s->seqMode == 0){
      lcd.print(charEscalasNotas[s->steps[midiUI.movedPoteNumber].note % 12]);
      lcd.print(int(midiUI.movedPoteValue/12) + s->steps[midiUI.movedPoteNumber].octave);
      lcd.print(" ");
      lcd.print(midiUI.movedPoteValue + (12 * s->steps[midiUI.movedPoteNumber].octave));
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
  for(byte i = 4; i < N_MAX_STEPS; i++){
    lcd.print(charEscalasNotas[s->steps[i].note]);
    lcd.print(int(s->steps[i].note/12) + s->steps[i].octave);
    lcd.print(" ");
  }
  nAnteriorScreen = 6;
}

inline void DrawMenus::drawMutes() {
  lcd.setCursor(midiUI.indexMovedMute + 2,3);
  if (!s->steps[midiUI.indexMovedMute].mutes && midiUI.indexMovedMute <= s->nTotalSteps - 1) {lcd.write(byte(6));}
  else if(s->steps[midiUI.indexMovedMute].mutes && midiUI.indexMovedMute <= s->nTotalSteps - 1){lcd.write(byte(5));}
}

inline void DrawMenus::drawCCMutes() {
  lcd.setCursor(midiUI.indexMovedMute,3);
  if (!s->steps[midiUI.indexMovedMute].ccMutes && midiUI.indexMovedMute <= s->nTotalSteps - 1) {lcd.print("+");}
  else if(s->steps[midiUI.indexMovedMute].ccMutes && midiUI.indexMovedMute <= s->nTotalSteps - 1){lcd.print(" ");}
}

inline void DrawMenus::drawSteps() {
  lcd.setCursor(2,3);
  if(lastNTotalSteps != s->nTotalSteps){
    lcd.setCursor(s->nTotalSteps - 1 + 2, 3);
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
      writeAt(nStepAnterior + 2, 3, byte(5));
      writeAt(SEQ.nStep + 2, 3, byte(5));
    }
    else if(!s->steps[nStepAnterior].mutes){
      writeAt(nStepAnterior + 2, 3, byte(6));
      writeAt(SEQ.nStep + 2, 3, byte(5));
    }
  }
  else if(!s->steps[SEQ.nStep].mutes){
    if(s->steps[nStepAnterior].mutes){
      writeAt(nStepAnterior + 2, 3, byte(5));
      writeAt(SEQ.nStep + 2, 3, byte(7));
    }
    else if(!s->steps[nStepAnterior].mutes){
      writeAt(nStepAnterior + 2, 3, byte(6));
      writeAt(SEQ.nStep + 2, 3, byte(7));
      
    }
  }
  nStepAnterior = SEQ.nStep;
}

inline void DrawMenus::drawCCVisualizer(){
  lcd.setCursor(0, 2);
  byte visualBuffer[s->nTotalSteps];
  calcularCurvaParaPantalla(visualBuffer, 1, 96);
  for (int i = 0; i < s->nTotalSteps; i++) {
    int nivel = map(visualBuffer[i], 0, 127, 0, 7);
    switch(nivel) {
    case 0: lcd.write(byte(0)); break;  // Nivel más bajo: Barra baja estándar
    case 1: lcd.write(byte(1)); break;  // Custom 0
    case 2: lcd.write(byte(2)); break;  // Custom 1
    case 3: lcd.write(byte(3)); break;  // Custom 2
    case 4: lcd.write(byte(4)); break;  // Custom 3
    case 5: lcd.write(byte(5)); break;  // Custom 4
    case 6: lcd.write(byte(6)); break;  // Custom 5
    case 7: lcd.write(255); break;      // Nivel más alto: Guion estándar (o barra superior si existiera)
    }
  }  
} 

void DrawMenus::calcularCurvaParaPantalla(byte* visualBuffer, int puntosPorPaso, int subdivision) {
  int totalSteps = s->nTotalSteps;

  for (int step = 0; step < totalSteps; step++) {
    
    // Buscar el punto desde el que partimos
    int prevStep = step;
    while (prevStep > 0 && s->steps[prevStep].ccMutes) {
      prevStep--;
    }

    // 2. Mirar hacia adelante
    int nextStep = step + 1;
    bool foundNext = false;
    while (nextStep < totalSteps) {
      if (!s->steps[nextStep].ccMutes) {
        foundNext = true;
        break;
      }
      nextStep++;
    }

    // 3. Coger los valores de las anclas
    int valorPrev = s->steps[prevStep].ccValue;
    int valorNext = 0;
    int distanceSteps = 1;

    if (foundNext) {
      valorNext = s->steps[nextStep].ccValue;
      distanceSteps = nextStep - prevStep;
    } else {
      int firstStep = 0;
      while (firstStep < totalSteps && s->steps[firstStep].ccMutes) {
        firstStep++;
      }
      valorNext = s->steps[firstStep].ccValue;
      distanceSteps = (totalSteps - prevStep) + firstStep;
    }

    // 4. Calcular distancias reales
    int ccStepSize = (valorNext - valorPrev);
    int totalSubdivisions = distanceSteps * subdivision;
    int offsetSubdivision = (step - prevStep) * subdivision;
    int divisor = (totalSubdivisions > 1) ? totalSubdivisions : 2;

    // 5. Rellenar el buffer visual para este paso concreto
    for (int p = 0; p < puntosPorPaso; p++) {
      int subReal = (p * subdivision) / puntosPorPaso; 
      int indexCurva = offsetSubdivision + subReal;

      long valorCurva = midiProg[presetsUI.indexSequence].ccCurveFunction(indexCurva, divisor, s->steps[prevStep].ccSmoothCurve);
      
      int valorCalculado = valorPrev + (((long)ccStepSize * valorCurva) / 255);
      
      visualBuffer[(step * puntosPorPaso) + p] = valorCalculado;
    }
  }
}

void DrawMenus::sdErrorMsg(){
  printAt(3, 0, "X ERROR: No SD");
  delay(2000);
}

void DrawMenus::sdCheckMsg(){
  printAt(1, 0, "CHECK: SD detected");
  delay(2000); 
}

inline void DrawMenus::drawLoadPreset(byte number){
  lcd.clear();
  nScreen = 6;
  printAt(3, 0, "Guardado Slot ");
  lcd.print(number);
  printAt(5, 2, "COMPLETADO");
  nAnteriorScreen = 6;
}



