#include <Arduino.h>
#include <TimerOne.h>
#include "menus_buttons.h"
#include "MidiProgramming.h"
#include "midiInterface.h"
#include "midiPresets.h"

MenusButtons menusUI;

MenusButtons::MenusButtons() : 
  ultimaConfiguracionModo2{0, 2}

{
  menuActual = 0;
  seleccion = 1;
  ultimaConfiguracionModo0 = 7;
  ultimaConfiguracionModo1 = 7;
  
  cambioModo0 = false;
  cambioModo1 = false;
  cambioModo2 = false;
}

void MenusButtons::aplicarCambios(){
  drawUI.updateLCD = true;
  repeatedButton = true;
  ultimoTiempoBotonMenu = tiempoActualMillis;
}

void MenusButtons::checkButtons() {
  bool btnArriba = digitalRead(pinButton1);
  bool btnOk = digitalRead(pinButton2);
  bool btnAbajo = digitalRead(pinButton3);
  bool btnVolver = digitalRead(pinButton4);
  bool btnIzquierda = digitalRead(pinButton5);
  bool btnDerecha = digitalRead(pinButton6);


  if(!btnArriba && !btnOk && !btnAbajo && !btnVolver && !btnIzquierda && !btnDerecha){repeatedButton = false;}
  if (btnVolver) { // Volver
      if (menusUI.menuActual != 0) { menusUI.menuActual = 0; menuAnterior = menusUI.menuActual; drawUI.updateLCD = true; }
  }
  
  switch(menusUI.menuActual){

    //MENÚ PRINCIPAL
    case 0:
      if (btnArriba && seleccion > 1) { 
        if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
          seleccion --;
          aplicarCambios(); 
        }
      }
      if (btnAbajo && seleccion < 6) { 
        if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
          seleccion ++;
          aplicarCambios();
        }
      }
      switch(seleccion){
        //selector modo midiProg.subdivision
        case 1:
          if (btnDerecha) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              presetsUI.indexSequence++;
              if(presetsUI.indexSequence > 0){presetsUI.indexSequence = 0;}
              aplicarCambios();
            }
          }
          else if (btnIzquierda) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              presetsUI.indexSequence--;
              if(presetsUI.indexSequence < 0){presetsUI.indexSequence = 0;}
              aplicarCambios();
            }
          }
          break;
        case 2:
          if (btnDerecha) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(MODE_SEQUENCE() == 0){MODE_SEQUENCE()++;}
              aplicarCambios();
            }
          }
          else if (btnIzquierda) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(MODE_SEQUENCE() == 1){MODE_SEQUENCE()--;}
              aplicarCambios();
            }
          }
          break;
        case 3:
          if (btnDerecha) { 
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              SUBDIV_MODE() ++;
              if(SUBDIV_MODE() > 2){SUBDIV_MODE() = 0;}
              if (SUBDIV_MODE() == 0) {
                if(midiProg.play){
                  SUBDIV_MODE() = 2;
                  cambioModo0 = true;
                }
                INDEX_SUBDIVISIONES() = ultimaConfiguracionModo0;
                midiProg.subdivision = midiProg.subdivisionesArray[INDEX_SUBDIVISIONES()];
                }
              else if (SUBDIV_MODE() == 1) {
                INDEX_SUBDIVISIONES() = ultimaConfiguracionModo1;
                midiProg.subdivision = midiProg.subdivisionesArray[INDEX_SUBDIVISIONES()];
                }
              else if (SUBDIV_MODE() == 2){
                if(midiProg.play){
                  SUBDIV_MODE() = 1;
                  cambioModo2 = true;
                }
                INDEX_COMPLEX_X() = ultimaConfiguracionModo2[0];
                INDEX_COMPLEX_Y() = ultimaConfiguracionModo2[1];
                selectNum = true;
                }
              aplicarCambios();
            }
          }
          if (btnIzquierda) { 
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){    
              SUBDIV_MODE() --;
              if(SUBDIV_MODE() < 0){SUBDIV_MODE() = 2;}
              if (SUBDIV_MODE() == 0) {
                INDEX_SUBDIVISIONES() = ultimaConfiguracionModo0;
                midiProg.subdivision = midiProg.subdivisionesArray[INDEX_SUBDIVISIONES()];
                }
              else if (SUBDIV_MODE() == 1) {
                if(midiProg.play){
                  SUBDIV_MODE() = 2;
                  cambioModo1 = true;
                }
                INDEX_SUBDIVISIONES() = ultimaConfiguracionModo1;
                midiProg.subdivision = midiProg.subdivisionesArray[INDEX_SUBDIVISIONES()];
                }
              else if (SUBDIV_MODE() == 2){
                if(midiProg.play){
                  SUBDIV_MODE() = 0;
                  cambioModo2 = true;
                }
                INDEX_COMPLEX_X() = ultimaConfiguracionModo2[0];
                INDEX_COMPLEX_Y() = ultimaConfiguracionModo2[1];
                selectNum = true;}
              aplicarCambios();
            }
          }
          break;
        //selector midiProg.subdivision
        case 4:   
          if (btnDerecha) { 
            if(SUBDIV_MODE() == 0){ //Modo binario-ternario
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (INDEX_SUBDIVISIONES() < 11){
                  INDEX_SUBDIVISIONES()++;
                  ultimaConfiguracionModo0 = INDEX_SUBDIVISIONES();
                  aplicarCambios();
                }
              }
            }
            else if(SUBDIV_MODE() == 1){ //modo solo binarios
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (INDEX_SUBDIVISIONES() < 11){
                  INDEX_SUBDIVISIONES() += 2;
                  ultimaConfiguracionModo1 = INDEX_SUBDIVISIONES();
                  aplicarCambios();
                }
              }
            }
            else if(SUBDIV_MODE() == 2){//modo complejo
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(selectNum && INDEX_COMPLEX_X < 5){
                  INDEX_COMPLEX_X()++;
                  ultimaConfiguracionModo2[0] = INDEX_COMPLEX_X;
                  if(INDEX_COMPLEX_Y() == 2 && INDEX_COMPLEX_X() > 2){INDEX_COMPLEX_X() = 2;}
                  aplicarCambios();
                }
                if(selectDen && INDEX_COMPLEX_Y < 2){
                  INDEX_COMPLEX_Y()++;
                  ultimaConfiguracionModo2[1] = INDEX_COMPLEX_Y;
                  if(INDEX_COMPLEX_Y() == 2 && INDEX_COMPLEX_X() > 2){INDEX_COMPLEX_X() = 2;}
                  aplicarCambios();
                }
              }
            }
          }  
          if (btnIzquierda) {
            if(SUBDIV_MODE() == 0){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (INDEX_SUBDIVISIONES() > 0){
                  INDEX_SUBDIVISIONES()--;
                  ultimaConfiguracionModo0 = INDEX_SUBDIVISIONES();
                  aplicarCambios();
                }
              }
            }
            else if(SUBDIV_MODE() == 1){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (INDEX_SUBDIVISIONES() > 1){
                  INDEX_SUBDIVISIONES() -= 2;
                  ultimaConfiguracionModo1 = INDEX_SUBDIVISIONES();
                  aplicarCambios();
                }
              }
            }
            else if(SUBDIV_MODE() == 2){//modo complejo
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(selectNum && INDEX_COMPLEX_X() > 0){
                  INDEX_COMPLEX_X()--;
                  ultimaConfiguracionModo2[0] = INDEX_COMPLEX_X();
                  aplicarCambios();
                }
                if(selectDen && INDEX_COMPLEX_Y > 0){
                  ultimaConfiguracionModo2[1] = INDEX_COMPLEX_Y();
                  INDEX_COMPLEX_Y()--;
                  aplicarCambios();
                }
              }
            }
          }
          if (btnOk){
            if(SUBDIV_MODE() == 2 && seleccion == 4){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(selectNum){selectDen = true, selectNum = false;}
                else if(selectDen){selectNum = true, selectDen = false;}
                aplicarCambios();
              }
            }
          }
          break;
        case 5: //selector pasos
          if (btnDerecha) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(NTOTALSTEPS() < NUM_POTES){
                NTOTALSTEPS()++;
                aplicarCambios();
              }
            }
          }
          else if (btnIzquierda) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(NTOTALSTEPS() > 1){                
                NTOTALSTEPS()--;
                aplicarCambios();
              }
            }
          } 
          break;
        case 6:
          if (btnDerecha) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(midiProg.modeMidiClock < 1){
                midiProg.modeMidiClock++;
                Timer1.initialize(midiProg.tempoMicros);
                Timer1.attachInterrupt(funcionInterrupcion); 
                aplicarCambios();
              }
            }
          }
          else if(btnIzquierda) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(midiProg.modeMidiClock > 0){
                Timer1.stop();
                Timer1.detachInterrupt();
                midiProg.modeMidiClock--;
                aplicarCambios();
              }
            }
          }
          break;
      }
      break;
  }
}
void MenusButtons::showPotes(){  // 3. nClockMsgrizador para salir de la pantalla de potenciómetro
  if (menusUI.menuActual == 3 && (tiempoActualMillis - midiUI.timeShowPotValue >= 1500)) {
    menusUI.menuActual = menuAnterior;
    drawUI.updateLCD = true;
  }
}