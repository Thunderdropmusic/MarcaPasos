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

DrawMenus::DrawMenus()
  : lcd(0x27, 20, 4),

    // Carácteres
    arrowLeft{ B00001, B00011, B00111, B01111, B01111, B00111, B00011, B00001 },
    arrowRight{ B10000, B11000, B11100, B11110, B11110, B11100, B11000, B10000 },
    redonda{ B00000, B00000, B00000, B01110, B10001, B10001, B01110, B00000 },
    blanca{ B00010, B00010, B00010, B00010, B01110, B10010, B10010, B01100 },
    negra{ B00010, B00010, B00010, B00010, B01110, B11110, B11110, B01100 },
    stepOff{ B00000, B00000, B00000, B01110, B00000, B00000, B00000, B00000 },
    stepOn{ B00000, B01110, B10001, B10001, B10001, B01110, B00000, B00000 },
    stepPass{ B00000, B01110, B11111, B11111, B11111, B01110, B00000, B00000 },

    ccLevel0{ 0, 0, 0, 0, 0, 0, 0, 31 },  //Abajo
    ccLevel1{ 0, 0, 0, 0, 0, 0, 31, 31 },
    ccLevel2{ 0, 0, 0, 0, 0, 31, 31, 31 },
    ccLevel3{ 0, 0, 0, 0, 31, 31, 31, 31 },
    ccLevel4{ 0, 0, 0, 31, 31, 31, 31, 31 },
    ccLevel5{ 0, 0, 31, 31, 31, 31, 31, 31 },
    ccLevel6{ 0, 31, 31, 31, 31, 31, 31, 31 },  // Casi arriba

    subdivisionesCharArray{ "1/8", "1/6", "1/4", "1/3", "1/2", "3/2", "1/1", "3/4", "2/1", "3/8", "4/1" },
    subdivisionesComplejasNumChar{ " 5", " 7", " 9", "11", "13", "15" },
    subdivisionesComplejasDenChar{ byte(2), byte(3), byte(4) },
    charEscalasNotas{ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "C" },
    nombreEscalas{ "Chromatic", "Natural Minor", "Natural Major", "Tone Scale", "Semitone-Tone" } {
  updateLCD = true;
  nAnteriorScreen = 999;
}

// ==============================================================================
//                                FUNCIONES UTILES
// ==============================================================================

void DrawMenus::syncWithActiveSequence() {
  s = (!menusUI.editExtension) ? presetsUI.getActiveSequence() : presetsUI.getExtensionSequence();
}

void DrawMenus::configurarLCD() {
  lcd.init();
  Wire.setClock(400000);
  lcd.backlight();
}

void DrawMenus::printAt(byte x, byte y, byte var) {
  lcd.setCursor(x, y);
  lcd.print(var);
}

void DrawMenus::printAt(byte x, byte y, char text) {
  lcd.setCursor(x, y);
  lcd.print(text);
}

void DrawMenus::printAt(byte x, byte y, const char* text) {
  lcd.setCursor(x, y);
  lcd.print(text);
}

void DrawMenus::writeAt(byte x, byte y, byte numByte) {
  lcd.setCursor(x, y);
  lcd.write(numByte);
}

// ==============================================================================
//                           CARACTERES PERSONALIZADOS
// ==============================================================================

void DrawMenus::charactersMenu1() {
  lcd.createChar(0, arrowLeft);
  lcd.createChar(1, arrowRight);
  lcd.createChar(2, redonda);
  lcd.createChar(3, blanca);
  lcd.createChar(4, negra);
  lcd.createChar(5, stepOff);
  lcd.createChar(6, stepOn);
  lcd.createChar(7, stepPass);
}

void DrawMenus::charactersMenu2() {

  lcd.createChar(0, ccLevel0);
  lcd.createChar(1, ccLevel1);
  lcd.createChar(2, ccLevel2);
  lcd.createChar(3, ccLevel3);
  lcd.createChar(4, ccLevel4);
  lcd.createChar(5, ccLevel5);
  lcd.createChar(6, ccLevel6);
}



// ==============================================================================
//                           FUNCIONES DE DIBUJADO
// ==============================================================================



void DrawMenus::update_LCD() {
  syncWithActiveSequence();
  switch (menusUI.menuActual) {
    case 0: drawMenuNotes(); break;
    case 1: drawMenuCC(); break;
    case 2: drawMenuSettings(); break;
    case 3: drawScreenPotes(); break;
    case 4: drawScreenOctavas(); break;
    case 5: drawScreenTodasOctavas(); break;
    case 6: drawLoadPreset(presetsUI.nSlot); break;
    case 7: drawConfirmationScreen(); break;
  }
  updateLCD = false;
  interrupts();
  updateValues = false;
}

// --- MENU NOTAS ---

inline void DrawMenus::drawMenuNotes() {
  if (menusUI.seleccion >= 1 && menusUI.seleccion <= 4) { nScreen = 0; }

  if (midiUI.deviceStop) {
    printAt(2, 3, "      STOP      ");
  }
  if (!midiUI.deviceStop) {
    drawMutes();
    drawSteps();
  }

  if (nScreen != nAnteriorScreen || menusUI.menuAnterior != menusUI.menuActual || updateValues) { staticScreen1(); }

  switch (menusUI.seleccion) {
    case 1:
      lcd.noBlink(), printAt(0, 0, "["), printAt(4, 0, "]");
      printAt(8, 0, presetsUI.indexSequence),
        printAt(10, 0, " "), printAt(19, 0, " ");
      printAt(5, 1, " "), printAt(15, 1, " ");
      break;
    case 2:
      printAt(0, 0, " "), printAt(4, 0, " "), printAt(10, 0, " "), printAt(19, 0, " ");
      lcd.noBlink(), writeAt(5, 1, byte(0)), printAt(14, 1, s->subdivMode), lcd.write(byte(1));
      printAt(8, 2, " "), lcd.print(subdivisionesCharArray[s->indexSubdivisiones]), lcd.print(" ");
      if (s->subdivMode == 0 || s->subdivMode == 1) {
        printAt(9, 2, subdivisionesCharArray[s->indexSubdivisiones]);
      } else if (s->subdivMode == 2) {
        printAt(8, 2, subdivisionesComplejasNumChar[s->indComplexSubdivX]), writeAt(11, 2, subdivisionesComplejasDenChar[s->indComplexSubdivY]);
      }
      break;
    case 3:
      lcd.noBlink(), printAt(0, 0, " "), printAt(4, 0, " "), printAt(8, 0, presetsUI.indexSequence);
      printAt(5, 1, " "), printAt(15, 1, " ");
      if (s->subdivMode == 0 || s->subdivMode == 1) {
        writeAt(8, 2, byte(0)), lcd.print(subdivisionesCharArray[s->indexSubdivisiones]), lcd.write(byte(1));
      } else if (s->subdivMode == 2) {
        lcd.setCursor(8, 2), lcd.print(subdivisionesComplejasNumChar[s->indComplexSubdivX]), lcd.setCursor(11, 2), lcd.write(subdivisionesComplejasDenChar[s->indComplexSubdivY]);
        lcd.setCursor(menusUI.selectNum ? 9 : 11, 2);
        lcd.blink();
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

inline void DrawMenus::staticScreen1() {
  charactersMenu1();

  for (int i = 1; i < 4; i++) {
    printAt(0, i, "                   ");
  }

  lcd.noBlink();
  printAt(1, 0, (!menusUI.editExtension) ? "SEQ  " : "EXT  "), lcd.write(126), lcd.print(" "), printAt(9, 0, "  "), lcd.print("PASOS: "), lcd.print(s->nTotalSteps);
  lcd.noBlink(), printAt(6, 1, "SUBDIV: "), lcd.print(s->subdivMode);
  if (s->subdivMode == 0 || s->subdivMode == 1) {
    printAt(9, 2, subdivisionesCharArray[s->indexSubdivisiones]);
  } else if (s->subdivMode == 2) {
    printAt(8, 2, subdivisionesComplejasNumChar[s->indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[s->indComplexSubdivY]);
  }
  nAnteriorScreen = nScreen;
  menusUI.menuAnterior = menusUI.menuActual;
  drawStaticSteps();
}

inline void DrawMenus::drawStaticSteps() {
  if (SEQ.inExtension) { s = presetsUI.getExtensionSequence(); }
  if (midiUI.deviceStop) {
    printAt(2, 3, "      STOP      ");
    return;
  }

  lcd.setCursor(2, 3);
  for (int i = 0; i < s->nTotalSteps; i++) {
    lcd.write(
      (s->steps[i].mutes) ? byte(5) : (i == SEQ.nStep) ? byte(7)
                                                       : byte(6));
  }
  syncWithActiveSequence();
}

inline void DrawMenus::drawMutes() {
  if (midiUI.indexMovedMute <= s->nTotalSteps - 1) {
    writeAt(midiUI.indexMovedMute + 2, 3, (s->steps[midiUI.indexMovedMute].mutes) ? byte(5) : byte(6));
  }
}

inline void DrawMenus::drawSteps() {
  if (lastNTotalSteps != s->nTotalSteps) {
    writeAt(s->nTotalSteps - 1 + 2, 3, s->steps[s->nTotalSteps - 1].mutes ? byte(5) : byte(6));
    printAt(s->nTotalSteps + 2, 3, " ");
    lastNTotalSteps = s->nTotalSteps;
  }
  writeAt(nStepAnterior + 2, 3, s->steps[nStepAnterior].mutes ? byte(5) : byte(6));
  writeAt(SEQ.nStep + 2, 3, s->steps[SEQ.nStep].mutes ? byte(5) : byte(7));
  nStepAnterior = SEQ.nStep;
}

// --- MENU CC ---

inline void DrawMenus::drawMenuCC() {
  if (menusUI.seleccion == 1) nScreen = 0;

  if (!midiUI.deviceStop) drawCCMutes();
  else printAt(0, 3, "        STOP        ");

  if (menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues) { staticScreen2(); }

  switch (menusUI.seleccion) {
    case 1:
      lcd.noBlink(), printAt(8, 0, presetsUI.indexSequence), printAt(10, 0, " "), printAt(19, 0, " ");
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

inline void DrawMenus::staticScreen2() {
  charactersMenu2();

  for (int i = 1; i < 4; i++) { // borrado
    for (int j = 0; j < 19; j++) {
      printAt(j, i, " ");
    }
  }

  lcd.noBlink(), writeAt(0, 0, 127), lcd.print("CC   "), lcd.write(126), lcd.print(" "), printAt(9, 0, "  "), lcd.print("PASOS: "), lcd.print(s->nTotalSteps);
  lcd.setCursor(0, 3);
  for (int i = 0; i < s->nTotalSteps; i++) {
    lcd.print((!s->steps[i].ccMutes) ? "+" : " ");
  }
  menusUI.menuAnterior = menusUI.menuActual;
  nAnteriorScreen = nScreen;
}

inline void DrawMenus::drawCCMutes() {
  if (midiUI.indexMovedMute <= s->nTotalSteps - 1) {
    printAt(midiUI.indexMovedMute, 3, (!s->steps[midiUI.indexMovedMute].ccMutes) ? "+" : " ");
  }
}

inline void DrawMenus::drawCCVisualizer() {
  lcd.setCursor(0, 2);
  byte visualBuffer[s->nTotalSteps];
  calcularCurvaParaPantalla(visualBuffer, 1, 96);

  for (int i = 0; i < s->nTotalSteps; i++) {
    int nivel = map(visualBuffer[i], 0, 127, 0, 7);
    lcd.write(nivel == 7 ? 255 : byte(nivel));
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

// ==============================================================================
//                              PANTALLAS EFIMERAS
// ==============================================================================

inline void DrawMenus::drawScreenPotes() {
  nScreen = 4;
  lcd.clear();
  lcd.print("Pote N");
  lcd.print((char)223);
  lcd.print(midiUI.movedPoteNumber + 1);
  lcd.print(" : ");
  lcd.setCursor(0, 1);
  if (s->seqMode == 0) {
    if (midiUI.muteActivado) {
      lcd.print("Velocity = ");
      lcd.print(midiUI.movedPoteValue);
      nAnteriorScreen = 4;
    } else {
      lcd.print(charEscalasNotas[s->steps[midiUI.movedPoteNumber].note % 12]);
      lcd.print(int(midiUI.movedPoteValue / 12) + s->steps[midiUI.movedPoteNumber].octave);
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
  lcd.print(int(s->steps[midiUI.indexMovedMute].note / 12) + s->steps[midiUI.indexMovedMute].octave);
  lcd.print(" ");
  lcd.print(s->steps[midiUI.indexMovedMute].note + (12 * s->steps[midiUI.indexMovedMute].octave));
  nAnteriorScreen = 5;
}
inline void DrawMenus::drawScreenTodasOctavas() {
  nScreen = 6;
  lcd.clear();
  for (byte i = 0; i < N_MAX_STEPS; i++) {
    byte coordX = 5 * (i % 4);

    lcd.setCursor(coordX, (i < 4) ? 0 : 1);
    lcd.print(charEscalasNotas[p->nSequence[presetsUI.indexSequence].steps[i].note]);
    lcd.print(int(p->nSequence[presetsUI.indexSequence].steps[i].note / 12) + p->nSequence[presetsUI.indexSequence].steps[i].octave);
    lcd.print("  ");

    lcd.setCursor(coordX, (i < 4) ? 2 : 3);
    lcd.print(charEscalasNotas[p->seq_extension[presetsUI.indexSequence].steps[i].note]);
    lcd.print(int(p->seq_extension[presetsUI.indexSequence].steps[i].note / 12) + p->seq_extension[presetsUI.indexSequence].steps[i].octave);
    lcd.print("  ");
  }

  nAnteriorScreen = 6;
}



void DrawMenus::sdErrorMsg() {
  printAt(3, 0, "X ERROR: No SD");
  delay(2000);
}

void DrawMenus::sdCheckMsg() {
  printAt(1, 0, "CHECK: SD detected");
  delay(2000);
}

inline void DrawMenus::drawLoadPreset(byte number) {
  lcd.clear();
  nScreen = 6;
  printAt(3, 0, "Guardado Slot ");
  lcd.print(number);
  printAt(5, 2, "COMPLETADO");
  nAnteriorScreen = 6;
}