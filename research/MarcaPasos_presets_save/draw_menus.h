#ifndef DRAW_MENUS_H
#define DRAW_MENUS_H

#include <Arduino.h>
#include <MIDI.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


struct Sequence;

class DrawMenus {
  private:
    Sequence* s;
  public:
  DrawMenus();
    LiquidCrystal_I2C lcd;

    // Carácteres
    byte arrowLeft[8];
    byte arrowRight[8];
    byte redonda[8];
    byte blanca[8];
    byte negra[8];
    byte stepOff[8];
    byte stepOn[8];
    byte stepPass[8];
    
    byte ccLevel0[8];
    byte ccLevel1[8];
    byte ccLevel2[8];
    byte ccLevel3[8];
    byte ccLevel4[8];
    byte ccLevel5[8];
    byte ccLevel6[8];
    const char* subdivisionesCharArray [11];
    const char* subdivisionesComplejasNumChar[6];
    const byte subdivisionesComplejasDenChar[3];
    const char* charEscalasNotas[13];
    byte nScreen;
    byte nAnteriorScreen;
    bool updateLCD;
    byte lastNTotalSteps;
    byte nStepAnterior;
    bool updateValues;

    //Declaracion
    void syncWithActiveSequence();
    void configurarLCD();
    void charactersMenu1();
    void charactersMenu2();
    void update_LCD();

    void drawMenuNotes();
    void drawMenuCC();
    void drawMenuSettings();

    void staticScreen1(); //Notas
    void staticScreen2(); //CC
    void staticScreen3(); //Settings

    void drawClockModemenu();
    void drawScreenPotes();
    void drawScreenOctavas();
    void drawScreenTodasOctavas();
    void drawMutes();
    void drawCCMutes();
    void drawSteps();
    void drawCCVisualizer();
    void calcularCurvaParaPantalla(byte* visualBuffer, int puntosPorPaso, int subdivision);
    void sdErrorMsg();
    void sdCheckMsg();
    void drawLoadPreset(byte number);
    void drawConfirmationScreen();

    

    void printAt(byte x, byte y, char text);
    void printAt(byte x, byte y, const char* text);
    void printAt(byte x, byte y, byte var);
    void writeAt(byte x, byte y, byte numByte);
};

extern DrawMenus drawUI;

//Funciones

#endif