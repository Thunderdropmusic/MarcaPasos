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
  s = presetsUI.getActiveSequence();
  
  stp = menusUI.editExtension ? s->ext_steps : s->steps;
  curTotalSteps = menusUI.editExtension ? s->ext_nTotalSteps : s->nTotalSteps;
  curSubdivMode = menusUI.editExtension ? s->ext_subdivMode : s->subdivMode;
  curIndSubdiv  = menusUI.editExtension ? s->ext_indexSubdivisiones : s->indexSubdivisiones;
  curindComplexSubdivY = menusUI.editExtension ? s->ext_indComplexSubdivY : s->indComplexSubdivY;
  curindComplexSubdivX = menusUI.editExtension ? s->ext_indComplexSubdivX : s->indComplexSubdivX;
// (Y lo mismo para indComplexSubdivX / Y si las usas)
}

void DrawMenus::configurarLCD() {
  lcd.init();
  Wire.setClock(400000);
  lcd.backlight();
  splashScreen();
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

  if (nScreen != nAnteriorScreen || menusUI.menuAnterior != menusUI.menuActual || updateValues) { staticScreenNotes(); }

  switch (menusUI.seleccion) {
    case 1:
      lcd.noBlink(), printAt(0, 0, "["), printAt(4, 0, "]");
      printAt(8, 0, presetsUI.indexSequence),
        printAt(10, 0, " "), printAt(19, 0, " ");
      printAt(5, 1, " "), printAt(15, 1, " ");
      break;
    case 2:
      printAt(0, 0, " "), printAt(4, 0, " "), printAt(10, 0, " "), printAt(19, 0, " ");
      lcd.noBlink(), writeAt(5, 1, byte(0)), printAt(14, 1, curSubdivMode), lcd.write(byte(1));
      printAt(8, 2, " "), lcd.print(subdivisionesCharArray[curIndSubdiv]), lcd.print(F(" "));
      if (curSubdivMode == 0 || curSubdivMode == 1) {
        printAt(9, 2, subdivisionesCharArray[curIndSubdiv]);
      } else if (curSubdivMode == 2) {
        printAt(8, 2, subdivisionesComplejasNumChar[curindComplexSubdivX]), writeAt(11, 2, subdivisionesComplejasDenChar[curindComplexSubdivY]);
      }
      break;
    case 3:
      lcd.noBlink(), printAt(0, 0, " "), printAt(4, 0, " "), printAt(8, 0, presetsUI.indexSequence);
      printAt(5, 1, " "), printAt(15, 1, " ");
      if (curSubdivMode == 0 || curSubdivMode == 1) {
        writeAt(8, 2, byte(0)), lcd.print(subdivisionesCharArray[curIndSubdiv]), lcd.write(byte(1));
      } else if (curSubdivMode == 2) {
        lcd.setCursor(8, 2), lcd.print(subdivisionesComplejasNumChar[curindComplexSubdivX]), lcd.setCursor(11, 2), lcd.write(subdivisionesComplejasDenChar[curindComplexSubdivY]);
        lcd.setCursor(menusUI.selectNum ? 9 : 11, 2);
        lcd.blink();
      }
      break;
    case 4:
      printAt(0, 0, " "), printAt(4, 0, " ");
      lcd.noBlink(), writeAt(10, 0, byte(0)), writeAt(19, 0, byte(1));
      nAnteriorScreen = nScreen;
      lcd.noBlink(), printAt(18, 0, curTotalSteps);
      break;
  }
}

inline void DrawMenus::staticScreenNotes() {
  charactersMenu1();

  for (int i = 1; i < 4; i++) {
    printAt(0, i, "                   "); // borrado de la pantalla
  }

  lcd.noBlink();
  printAt(1, 0, (!menusUI.editExtension) ? "SEQ  " : "EXT  "), lcd.write(126), lcd.print(F(" ")), printAt(9, 0, "  "), lcd.print(F("PASOS: ")), lcd.print(curTotalSteps);
  lcd.noBlink(), printAt(6, 1, "SUBDIV: "), lcd.print(curSubdivMode);
  if (curSubdivMode == 0 || curSubdivMode == 1) {
    printAt(9, 2, subdivisionesCharArray[curIndSubdiv]);
  } else if (curSubdivMode == 2) {
    printAt(8, 2, subdivisionesComplejasNumChar[curindComplexSubdivX]), lcd.print(F("/")), lcd.write(subdivisionesComplejasDenChar[curindComplexSubdivY]);
  }
  nAnteriorScreen = nScreen;
  menusUI.menuAnterior = menusUI.menuActual;
  drawStaticSteps();
}

// --- Primer dibujado de todos los pasos en la pantalla ---

inline void DrawMenus::drawStaticSteps() {
  if (midiUI.deviceStop) {
    printAt(2, 3, "      STOP      ");
    return;
  }

  lcd.setCursor(2, 3);
  for (int i = 0; i < curTotalSteps; i++) {
    lcd.write(
      (stp[i].mutes) ? byte(5) : (i == SEQ.nStep) ? byte(7)     // 5 = Mute activo; 6 = Paso activado; 7 = Paso reproduciendose actualmente;
                                                       : byte(6));
  }
  syncWithActiveSequence();
}

// --- Dibujado del mute si cambia de valor ---

inline void DrawMenus::drawMutes() {
  if (midiUI.indexMovedMute <= curTotalSteps - 1) {
    writeAt(midiUI.indexMovedMute + 2, 3, (stp[midiUI.indexMovedMute].mutes) ? byte(5) : byte(6));
  }
}

// --- Dibujado de los pasos cada vez que avanza la secuencia ---

inline void DrawMenus::drawSteps() {
  // Detectamos si el numero de pasos de la secuencia ha cambiado y borramos el paso que hemos eliminado
  if (lastNTotalSteps != curTotalSteps) {
    writeAt(curTotalSteps + 1, 3, stp[curTotalSteps - 1].mutes ? byte(5) : byte(6));
    printAt(curTotalSteps + 2, 3, " ");
    lastNTotalSteps = curTotalSteps;
  }
  if(SEQ.nStep == nStepAnterior) return; // Si seguimos en el mismo paso, terminamos la función.
  
  writeAt(nStepAnterior + 2, 3, stp[nStepAnterior].mutes ? byte(5) : byte(6)); // Dibujado del paso anterior
  writeAt(SEQ.nStep + 2, 3, stp[SEQ.nStep].mutes ? byte(5) : byte(7)); // Dibujado del paso actual
  nStepAnterior = SEQ.nStep;
}

// --- MENU CC ---

inline void DrawMenus::drawMenuCC() {
  if (menusUI.seleccion == 1) nScreen = 0;

  if (!midiUI.deviceStop) drawCCMutes();
  else printAt(0, 3, "        STOP        ");

  if (menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues) { staticScreenCC(); }
  
  lcd.setCursor(0, 2);
  drawCCVisualizer();

  switch (menusUI.seleccion) {
    case 1:
      lcd.noBlink(), printAt(8, 0, presetsUI.indexSequence), printAt(10, 0, " "), printAt(19, 0, " ");
      printAt(0, 0, "["), printAt(3, 0, "] ");
      printAt(0, 1, midiUI.movedPoteValue);
      lcd.print(F("  "));

      break;

    case 2:
      printAt(0, 0, " "), printAt(3, 0, " "),
        lcd.noBlink(), printAt(10, 0, "["), printAt(19, 0, "]");
      nAnteriorScreen = nScreen;
      lcd.noBlink(), printAt(18, 0, curTotalSteps);
      printAt(curTotalSteps, 2, " ");
      printAt(13, 1, " "), lcd.print(subdivisionesCharArray[curIndSubdiv]), printAt(17, 1, " ");
      break;
    case 3:
      lcd.noBlink(), printAt(10, 0, " "), printAt(19, 0, " ");
      nAnteriorScreen = nScreen;
      printAt(13, 1, "["), lcd.print(subdivisionesCharArray[curIndSubdiv]), lcd.print(F("]"));
      break;
  }
}

inline void DrawMenus::staticScreenCC() {
  charactersMenu2();

  for (int i = 1; i < 4; i++) { // Borrado
    for (int j = 0; j < 19; j++) {
      printAt(j, i, " ");
    }
  }

  lcd.noBlink(), writeAt(0, 0, 127), lcd.print(F("CC   ")), lcd.write(126), lcd.print(F(" ")), printAt(9, 0, "  "), lcd.print(F("PASOS: ")), lcd.print(curTotalSteps);
  printAt(14, 1, subdivisionesCharArray[curIndSubdiv]);
  lcd.setCursor(0, 3);
  for (int i = 0; i < curTotalSteps; i++) {
    lcd.print((!stp[i].ccMutes) ? "+" : " ");
  }
  menusUI.menuAnterior = menusUI.menuActual;
  nAnteriorScreen = nScreen;
}

// --- Dibujado de los puntos de la automatización ---

inline void DrawMenus::drawCCMutes() {
  if (midiUI.indexMovedMute <= curTotalSteps - 1) {
    printAt(midiUI.indexMovedMute, 3, (!stp[midiUI.indexMovedMute].ccMutes) ? "+" : " ");
  }
}

// --- Dibujado de la curva de la automatización ---

inline void DrawMenus::drawCCVisualizer() {
  lcd.setCursor(0, 2);
  byte visualBuffer[curTotalSteps];
  calcularCurvaParaPantalla(visualBuffer, 1, 96);

  for (int i = 0; i < curTotalSteps; i++) {
    int nivel = map(visualBuffer[i], 0, 127, 0, 7);
    lcd.write(nivel == 7 ? 255 : byte(nivel));
  }
}

void DrawMenus::calcularCurvaParaPantalla(byte* visualBuffer, int nCharStep, int resolution) { // 
  int totalSteps = curTotalSteps;

  for (int step = 0; step < totalSteps; step++) { // pasamos por todos los pasos para calcular cual es el valor de cada uno de ellos

    // Buscar el punto desde el que partimos
    int prevStep = step;
    while (prevStep > 0 && stp[prevStep].ccMutes) { // buscamos el primer valor no muteado
      prevStep--;
    }

    // Mirar hacia adelante y encontrar el primer punto que no esté muteado
    int nextStep = step + 1;
    bool foundNext = false;
    while (nextStep < totalSteps) {
      if (!stp[nextStep].ccMutes) {
        foundNext = true;
        break;
      }
      nextStep++;
    }

    // Recogemos los valores de los dos puntos y su distancia entre ellos
    int lastValue = stp[prevStep].ccValue;
    int nextValue = 0;
    int distanceSteps = 1;

    if (foundNext) {
      nextValue = stp[nextStep].ccValue;
      distanceSteps = nextStep - prevStep;
    } 
    else {
      int firstStep = 0;
      while (firstStep < totalSteps && stp[firstStep].ccMutes) {
        firstStep++;
      }
      nextValue = stp[firstStep].ccValue;
      distanceSteps = (totalSteps - prevStep) + firstStep;
    }

    // Calcular distancias reales
    int ccStepSize = (nextValue - lastValue); // Distancia total entre los dos mensajes
    int totalResolution = distanceSteps * resolution; // valores totales a calcular entre los dos puntos
    int indexResolution = (step - prevStep) * resolution; // valor en que nos encontramos 
    int divisor = (totalResolution > 1) ? totalResolution : 2; // si hay solo un punto, divisor vale dos para que la operación no este dividiendo entre 0: int indiceLUT = (i * 99) / (divisor - 1);

    // Rellenar el buffer visual para este paso concreto. nCharStep decide cuanto dividimos el step para tener mas resolucion (por defecto 1)
    for (int p = 0; p < nCharStep; p++) { 
      int subReal = (p * resolution) / nCharStep; 
      int indexCurva = indexResolution + subReal;

      long valorCurva = midiProg[presetsUI.indexSequence].ccCurveFunction(indexCurva, divisor, stp[prevStep].ccSmoothCurve);

      int valorCalculado = lastValue + (((long)ccStepSize * valorCurva) / 255);

      visualBuffer[(step * nCharStep) + p] = valorCalculado;
    }
  }
}

// ==============================================================================
//                              PANTALLAS EFIMERAS
// ==============================================================================

inline void DrawMenus::drawScreenPotes() {
  nScreen = 4;
  lcd.clear();
  lcd.print(F("Pote N"));
  lcd.print((char)223);
  lcd.print(midiUI.movedPoteNumber + 1);
  lcd.print(F(" : "));
  lcd.setCursor(0, 1);
  if (s->seqMode == 0) { // Detectamos si estamos en el modo notas
    if (midiUI.muteActivado) { 
      lcd.print(F("Velocity = "));
      lcd.print(midiUI.movedPoteValue);
      nAnteriorScreen = 4;
    } 
    else {
      lcd.print(charEscalasNotas[stp[midiUI.movedPoteNumber].note % 12]);
      lcd.print(int(midiUI.movedPoteValue / 12) + stp[midiUI.movedPoteNumber].octave);
      lcd.print(F(" "));
      lcd.print(midiUI.movedPoteValue + (12 * stp[midiUI.movedPoteNumber].octave));
      nAnteriorScreen = 4;
    }
  }
}
inline void DrawMenus::drawScreenOctavas() {
  nScreen = 5;
  lcd.clear();
  lcd.print(F("Octava :"));
  printAt(0, 1, charEscalasNotas[stp[midiUI.indexMovedMute].note]);
  lcd.print(int(stp[midiUI.indexMovedMute].note / 12) + stp[midiUI.indexMovedMute].octave);
  lcd.print(F(" "));
  lcd.print(stp[midiUI.indexMovedMute].note + (12 * stp[midiUI.indexMovedMute].octave));
  nAnteriorScreen = 5;
}
inline void DrawMenus::drawScreenTodasOctavas() {
  nScreen = 6;
  lcd.clear();
  for (byte i = 0; i < N_MAX_STEPS; i++) { // Escribimos las notas en una tabla de cuatro columnas con 5 caracteres cada una
    byte coordX = 5 * (i % 4);

    lcd.setCursor(coordX, (i < 4) ? 0 : 1);
    lcd.print(charEscalasNotas[p->nSequence[presetsUI.indexSequence].steps[i].note]);
    lcd.print(int(p->nSequence[presetsUI.indexSequence].steps[i].note / 12) + p->nSequence[presetsUI.indexSequence].steps[i].octave);
    lcd.print(F("  "));

    lcd.setCursor(coordX, (i < 4) ? 2 : 3);
    lcd.print(charEscalasNotas[s->ext_steps[i].note]);
    lcd.print(int(s->ext_steps[i].note / 12) + s->ext_steps[i].octave);
    lcd.print(F("  "));
  }

  nAnteriorScreen = 6;
}



void DrawMenus::sdErrorMsg() {
  printAt(3, 2, "X ERROR: No SD");
  delay(2000);
}

void DrawMenus::sdCheckMsg() {
  printAt(1, 2, "CHECK: SD detected");
}

inline void DrawMenus::drawLoadPreset(byte number) {
  lcd.clear();
  nScreen = 6;
  printAt(3, 0, "Guardado Slot ");
  lcd.print(number);
  printAt(5, 2, "COMPLETADO");
  nAnteriorScreen = 6;
}

void DrawMenus::splashScreen() {
  // 1. Definimos los caracteres
  byte notaMusical[8] = {
    0b00100, 0b00110, 0b00101, 0b00101, 
    0b00100, 0b11100, 0b11100, 0b00000
  };

  lcd.createChar(1, notaMusical);
  
  lcd.clear(); // Lienzo en blanco

  for (int i = 0; i <= 9; i++) {
    lcd.setCursor(9 - i, 0); 
    lcd.print(F("=")); // Expande hacia la izquierda
    lcd.setCursor(10 + i, 0); 
    lcd.print(F("=")); // Expande hacia la derecha
    delay(40);      // Velocidad de la animación
  }

  // Pequeño respiro
  delay(150);

  // --- FASE 2: Aparición del título con adornos ---
  const char* titulo = "MARCAPASOS";
  lcd.setCursor(2, 1);
  lcd.write(byte(1)); // Dibuja la nota izquierda
  lcd.print(titulo);
  lcd.write(byte(1)); // Dibuja la nota derecha
  
  delay(300);

  // --- FASE 3: Efecto "máquina de escribir" para el subtítulo ---
  const char* subtitulo = "STEP SEQUENCER";
  lcd.setCursor(3, 2);
  for (unsigned int i = 0; i < strlen(subtitulo); i++) {
    lcd.print(subtitulo[i]);
    delay(50); // Velocidad del tecleo
  }

  delay(1500);
  for (int i = 0; i <= 9; i++) {
    lcd.setCursor(9 - i, 0); 
    lcd.print(F(" ")); // Expande hacia la izquierda
    lcd.setCursor(10 + i, 0); 
    lcd.print(F(" ")); // Expande hacia la derecha
    delay(40);      // Velocidad de la animación
  }
  for (unsigned int i = 0; i < strlen(subtitulo); i++) {
    lcd.print(F(" "));
    delay(50);
  }
  for (unsigned int i = 0; i < strlen(titulo); i++) {
    lcd.print(F(" "));
    delay(50); 
  }
  lcd.clear(); 
}

void DrawMenus::drawLoadProgress(byte loadProgress) {
  for(int i = 0; i < loadProgress; i++) {
    if(progresoActual < 20) {
      lcd.setCursor(progresoActual, 3); // Fila 4 (abajo del todo)
      lcd.write(byte(255)); // Escribe el bloque
      lcd.setCursor(0,2);
      switch(progresoActual){
        case 0:  lcd.print(F("  MIDI INITIALIZED  ")); delay(200); break;
        case 1:  lcd.print(F(" BUTTONS INITIALIZED")); delay(300); break;
        case 2:  lcd.print(F("  LEDS INITIALIZED  ")); delay(100); break;
        case 6:  lcd.print(F("  MUTES INITIALIZED ")); delay(200); break;
        case 10: lcd.print(F("   DEFAULT VALUES   ")); delay(900); break;
      }
      progresoActual++;
      delay(100);
    }
  }
}