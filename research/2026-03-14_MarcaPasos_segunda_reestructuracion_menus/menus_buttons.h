#ifndef MENUS_BUTTONS_H
#define MENUS_BUTTONS_H

#include <Arduino.h>



extern unsigned long tiempoActualMillis;
class MenusButtons {
  public:
  MenusButtons();
  //Variables pines
    const byte pinButton1 = 22; // Arriba
    const byte pinButton2 = 23; // OK / Entrar
    const byte pinButton3 = 25; // Abajo
    const byte pinButton4 = 24; // Volver


    // Variables Navegacion
    byte menuActual;
    byte menuAnterior;
    byte seleccion;
    byte selectNum;
    byte selectDen;
    byte selectSeq;
    byte selectCC;

    // Variables indicies subdivisiones
    byte ultimaConfiguracionModo0;
    byte ultimaConfiguracionModo1;
    byte ultimaConfiguracionModo2[2];
    bool cambioModo0[5];
    bool cambioModo1[5];
    bool cambioModo2[5];

    // Variables Enconder
    long posicionActual;
    long posicionAnterior;
    bool btnDerecha;
    bool btnIzquierda;
    // Variables de control
    bool repeatedButton;

    // Variables de tiempo real
    unsigned long ultimoTiempoBotonMenu;

    //Declaración funciones
    void aplicarCambiosBotones();
    void aplicarCambiosEncoder();
    void resetEncoder();
    void checkButtons();
    void showPotes();

};
extern MenusButtons menusUI;
extern void funcionInterrupcion();

#endif