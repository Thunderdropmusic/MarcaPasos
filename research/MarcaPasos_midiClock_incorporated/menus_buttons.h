#ifndef MENUS_BUTTONS_H
#define MENUS_BUTTONS_H

#include <Arduino.h>
#include "draw_menus.h"

extern unsigned long tiempoActualMillis;
class MenusButtons {
  public:
  MenusButtons();
  //Variables pines
    const byte pinButton1 = 22; // Arriba
    const byte pinButton2 = 23; // OK / Entrar
    const byte pinButton4 = 24; // Volver
    const byte pinButton3 = 25; // Abajo
    const byte pinButton5 = 27; // Izquierda
    const byte pinButton6 = 29; // Derecha

    // Variables Navegacion
    int menuActual;
    int menuAnterior;
    int seleccion;
    bool selectNum;
    bool selectDen;

    // Variables indicies subdivisiones
    int indiceSubdivisionesComplejasNum;
    int indiceSubdivisionesComplejasDen;
    int indComplexSubdivX;
    int indComplexSubdivY;
    int indiceSubdivisiones;
    int subDivMode;
    int ultimaConfiguracionModo0;
    int ultimaConfiguracionModo1;
    int ultimaConfiguracionModo2[2];
    bool cambioModo0;
    bool cambioModo1;
    bool cambioModo2;

    // Variables de control
    bool repeatedButton;

    // Variables de tiempo real
    unsigned long ultimoTiempoBotonMenu;

    //Declaración funciones
    void aplicarCambios();
    void checkButtons();
    void showPotes();

};
extern MenusButtons menusUI;
extern void funcionInterrupcion();

#endif