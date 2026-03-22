#ifndef MENUS_BUTTONS_H
#define MENUS_BUTTONS_H

#include <Arduino.h>

struct Sequence;

extern unsigned long tiempoActualMillis;



class MenusButtons {
  private:
    Sequence* s;

    //Funciones del selector y gestion pantallas
    void gestionarMenu1();
    void selectorEditMode();
    void selectorModoSubdivisiones();
    void selectorSubdivisiones();
    void gestionSubdivGlobal();
    void gestionSubdivBinaria();
    void gestionSubdivCompleja();
    void selectorPasos();
    void selectorClock();

    void selectorNumeroSeqCC();
  public:
  MenusButtons();
  //Variables pines
    const byte pinButton1 = 22; // Arriba
    const byte pinButton2 = 23; // OK / Entrar
    const byte pinButton3 = 25; // Abajo
    const byte pinButton4 = 24; // Volver

    // Bool Botones
    bool btnArriba;
    bool btnOk;
    bool btnAbajo;
    bool btnVolver;



    // Variables Navegacion
    byte menuActual;
    byte menuAnterior;
    byte seleccion;
    byte selectNum;
    byte selectDen;
    byte selectNotes;
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
    bool timeDebounce();
    void syncWithActiveSequence();
    void aplicarCambiosBotones();
    void aplicarCambiosEncoder();
    void resetEncoder();
    void checkButtons();
    void showPotes();


};
extern MenusButtons menusUI;
extern void funcionInterrupcion();

#endif