#ifndef DRAW_MENUS_H
#define DRAW_MENUS_H

#include <Arduino.h>
#include <MIDI.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


struct Sequence;
struct Step;

// ==============================================================================
//                                CLASE PRINCIPAL
// ==============================================================================

class DrawMenus {
  private:
    Sequence* s;
    Step* stp;
  public:
  DrawMenus();
    LiquidCrystal_I2C lcd;

// ==============================================================================
//                         CARACTERES Y VARIABLES
// ==============================================================================

    // --- Caracteres personalizados ---
    byte arrowLeft[8];
    byte arrowRight[8];
    byte redonda[8];
    byte blanca[8];
    byte negra[8];
    byte stepOff[8];
    byte stepOn[8];
    byte stepPass[8];
    
    // --- Barra de nivel modo CC ---
    byte ccLevel0[8];
    byte ccLevel1[8];
    byte ccLevel2[8];
    byte ccLevel3[8];
    byte ccLevel4[8];
    byte ccLevel5[8];
    byte ccLevel6[8];

    // --- Arrays de caracteres ---
    const char* subdivisionesCharArray [11];
    const char* subdivisionesComplejasNumChar[6];
    const byte subdivisionesComplejasDenChar[3];
    const char* charEscalasNotas[13];
    const char* nombreEscalas[5];
    int progresoActual;

    // --- Variables de estado ---
    byte nScreen;
    byte nAnteriorScreen;
    byte lastNTotalSteps;
    byte nStepAnterior;

    // --- Flags de actualización de la pantalla ---
    bool updateLCD;
    bool updateValues;

    // --- Variables de la secuencia ---
    byte curTotalSteps;
    byte curSubdivMode;
    byte curIndSubdiv;
    byte curindComplexSubdivY;
    byte curindComplexSubdivX;

// ==============================================================================
//                         DECLARACION DE FUNCIONES
// ==============================================================================

    // --- Funciones utiles ---
    void configurarLCD();
    void printAt(byte x, byte y, char text);
    void printAt(byte x, byte y, const char* text);
    void printAt(byte x, byte y, byte var);
    void writeAt(byte x, byte y, byte numByte);
    void syncWithActiveSequence();
    void charactersMenu1();
    void charactersMenu2();
    
    // --- Indice principal ---
    void update_LCD();

    // --- Funciones de dibujado - Fijas ---
    void drawMenuNotes();
    void drawMenuCC();
    void drawMenuSettings();
    void staticScreenNotes();
    void staticScreenCC();
    void staticScreenSettings();

    // --- Funciones de dibujado - Pop-Ups ---
    void drawScreenPotes();
    void drawScreenOctavas();
    void drawScreenTodasOctavas();

    // --- Acualización de simbolos ---
    void drawMutes();
    void drawCCMutes();
    void drawSteps();
    void drawStaticSteps();
    void sdErrorMsg();
    void sdCheckMsg();
    void drawLoadPreset(byte number);
    void drawConfirmationScreen();
    void splashScreen();
    void drawLoadProgress(byte loadProgress);

    // --- Curva CC ---
    void drawCCVisualizer();
    void calcularCurvaParaPantalla(byte* visualBuffer, int nCharStep, int resolution);

};

extern DrawMenus drawUI;

#endif