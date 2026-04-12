#ifndef MENUS_BUTTONS_H
#define MENUS_BUTTONS_H

#include <Arduino.h>
#include "config.h"

struct Sequence;

extern unsigned long tiempoActualMillis;



class MenusButtons {
  private:
    Sequence* s;

// ==============================================================================
//                         DECLARACION DE FUNCIONES 
// ==============================================================================

    //Funciones del selector y gestion pantallas
    void gestionarMenu1(); //Notas
    void gestionarMenu2(); //CC
    void gestionarMenu3(); //Settings
    
    // --- Menu notas ---
    void selectorNumSeq();
    void selectorModoSubdivisiones();
    void selectorSubdivisiones();
    void gestionSubdivGlobal();
    void gestionSubdivBinaria();
    void gestionSubdivCompleja();
    void selectorPasos();

    // --- Menu CC ---
    void selectorNumSeqCC();
    
    // --- Menu settings ---
    void savePreset();
    void loadPreset();
    void deletePreset();
    void selectorClock();
    void selectScale();
    void selectCCNumber();
    void saveConfig();

    // --- Funciones de control y actualización ---
    bool timeDebounce();
    void comeBack();
    void scroll();
    void syncWithActiveSequence();
    void aplicarCambiosBotones();
    void aplicarCambiosEncoder();
    void checkSeqMode();
    void resetEncoder();
    void showPotes();
    
    
  public:
  MenusButtons();

    // ==============================================================================
    //                                    VARIABLES 
    // ==============================================================================

    //Variables pines
    const byte pinButton1 = 22; // Arriba
    const byte pinButton2 = 23; // OK / Entrar
    const byte pinButton3 = 24; // Izquierda
    const byte pinButton4 = 25; // Abajo
    const byte pinButton5 = 26; // Derecha

    const byte extPin = 49;

    const byte settingsPin = 14;

    // Bool Botones
    bool btnArriba;
    bool btnOk;
    bool btnAbajo;
    bool btnIzquierda;
    bool btnDerecha;
    bool btnSettings;
    bool btnExt;

    // Variables Navegacion
    byte menuActual;
    byte menuAnterior;
    byte nMenuSettings;
    byte seleccion;
    bool selectNum;
    bool selectDen;

    // Varibles Configuracion
    byte whatSaveSelect = 0;
    bool putNameFlag;
    int index;
    int maxIndex;
    int scrollOffset;
    byte cursorPos;
    bool deleteConfirm;
    bool editExtension;

    // Variables indicies subdivisiones
    byte ultimaConfiguracionModo0;
    byte ultimaConfiguracionModo1;
    byte ultimaConfiguracionModo2[2];
    bool cambioModo0[N_MAX_SEQS];
    bool cambioModo1[N_MAX_SEQS];
    bool cambioModo2[N_MAX_SEQS];

    // Variables Enconder
    long posicionActual;
    long posicionAnterior;
    bool rollDerecha;
    bool rollIzquierda;

    // Variables de control
    bool repeatedButton;

    // Variables de tiempo real
    unsigned long ultimoTiempoBotonMenu;
    unsigned long ultimoTiempoBotonPresets;

    //Declaración funciones públicas
    void checkMenuButtons();

};
extern MenusButtons menusUI;
extern void funcionInterrupcion();

#endif