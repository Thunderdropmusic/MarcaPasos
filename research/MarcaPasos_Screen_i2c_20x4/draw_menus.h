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
    LiquidCrystal_I2C lcd;;

    // Carácteres
    byte arrowLeft[8];
    byte arrowRight[8];
    byte redonda[8];
    byte blanca[8];
    byte negra[8];
    byte stepOff[8];
    byte stepOn[8];
    byte stepPass[8];
    const char* subdivisionesCharArray [11];
    const char* subdivisionesComplejasNumChar[6];
    const byte subdivisionesComplejasDenChar[3];
    const char* charEscalasNotas[13];
    byte nScreen;
    byte nAnteriorScreen;
    bool updateLCD;
    byte lastNTotalSteps;
    byte nStepAnterior;

    //Declaracion
    void syncWithActiveSequence();
    void configurarLCD();
    void update_LCD();
    void drawMenuNotes();
    void drawMenuCC();
    void staticScreen1();
    void staticScreen2();
    void staticScreen3();
    void staticScreen4();
    void drawScreenPotes();
    void drawScreenOctavas();
    void drawScreenTodasOctavas();
    void drawMutes();
    void drawSteps();

    void printAt(byte x, byte y, char text);
    void printAt(byte x, byte y, const char* text);
    void printAt(byte x, byte y, byte var);
    void writeAt(byte x, byte y, byte numByte);
};

extern DrawMenus drawUI;

//Funciones

#endif