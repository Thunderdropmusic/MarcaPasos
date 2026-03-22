#ifndef DRAW_MENUS_H
#define DRAW_MENUS_H

#include <Arduino.h>
#include <MIDI.h>
#include <LiquidCrystal.h>




class DrawMenus {
  public:
  DrawMenus();
    LiquidCrystal lcd;

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

    //variables midiProg

    //Declaracion
    void configurarLCD();
    void update_LCD();
    void drawMenuPrincipal();
    void staticScreen1();
    void staticScreen2();
    void staticScreen3();
    void staticScreen4();
    void drawScreenPotes();
    void drawScreenOctavas();
    void drawScreenTodasOctavas();
    void drawSteps();
};

extern DrawMenus drawUI;

//Funciones

#endif