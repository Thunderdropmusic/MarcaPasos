#include <Arduino.h>
#include <TimerOne.h>
#include "menus_buttons.h"
#include "MidiProgramming.h"
#include "midiInterface.h"
#include "midiPresets.h"
#include "globalVars.h"
#include "draw_menus.h"

#define SEQ midiProg[presetsUI.indexSequence]

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
        //selector modo SEQ.subdivision
        case 1:
          if (btnDerecha) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              presetsUI.indexSequence++;
              if(presetsUI.indexSequence > 4){presetsUI.indexSequence = 0;}
              aplicarCambios();
            }
          }
          else if (btnIzquierda) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              presetsUI.indexSequence--;
              if(presetsUI.indexSequence < 0){presetsUI.indexSequence = 4;}
              aplicarCambios();
            }
          }
          break;
        case 2:
          if (btnDerecha) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(globalModeSequence[presetsUI.indexSequence] == 0){globalModeSequence[presetsUI.indexSequence]++;}
              aplicarCambios();
            }
          }
          else if (btnIzquierda) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(globalModeSequence[presetsUI.indexSequence] == 1){globalModeSequence[presetsUI.indexSequence]--;}
              aplicarCambios();
            }
          }
          break;
        case 3:
          if (btnDerecha) { 
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              globalSubdivMode[presetsUI.indexSequence] ++;
              if(globalSubdivMode[presetsUI.indexSequence] > 2){globalSubdivMode[presetsUI.indexSequence] = 0;}
              if (globalSubdivMode[presetsUI.indexSequence] == 0) {
                if(SEQ.play){
                  globalSubdivMode[presetsUI.indexSequence] = 2;
                  cambioModo0 = true;
                }
                globalIndexSubdivisiones[presetsUI.indexSequence] = ultimaConfiguracionModo0;
                SEQ.subdivision = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexSequence]];
                }
              else if (globalSubdivMode[presetsUI.indexSequence] == 1) {
                globalIndexSubdivisiones[presetsUI.indexSequence] = ultimaConfiguracionModo1;
                SEQ.subdivision = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexSequence]];
                }
              else if (globalSubdivMode[presetsUI.indexSequence] == 2){
                if(SEQ.play){
                  globalSubdivMode[presetsUI.indexSequence] = 1;
                  cambioModo2 = true;
                }
                globalIndComplexSubdivX[presetsUI.indexSequence] = ultimaConfiguracionModo2[0];
                globalIndComplexSubdivY[presetsUI.indexSequence] = ultimaConfiguracionModo2[1];
                selectNum = true;
                }
              aplicarCambios();
            }
          }
          if (btnIzquierda) { 
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){    
              globalSubdivMode[presetsUI.indexSequence] --;
              if(globalSubdivMode[presetsUI.indexSequence] < 0){globalSubdivMode[presetsUI.indexSequence] = 2;}
              if (globalSubdivMode[presetsUI.indexSequence] == 0) {
                globalIndexSubdivisiones[presetsUI.indexSequence] = ultimaConfiguracionModo0;
                SEQ.subdivision = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexSequence]];
                }
              else if (globalSubdivMode[presetsUI.indexSequence] == 1) {
                if(SEQ.play){
                  globalSubdivMode[presetsUI.indexSequence] = 2;
                  cambioModo1 = true;
                }
                globalIndexSubdivisiones[presetsUI.indexSequence] = ultimaConfiguracionModo1;
                SEQ.subdivision = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexSequence]];
                }
              else if (globalSubdivMode[presetsUI.indexSequence] == 2){
                if(SEQ.play){
                  globalSubdivMode[presetsUI.indexSequence] = 0;
                  cambioModo2 = true;
                }
                globalIndComplexSubdivX[presetsUI.indexSequence] = ultimaConfiguracionModo2[0];
                globalIndComplexSubdivY[presetsUI.indexSequence] = ultimaConfiguracionModo2[1];
                selectNum = true;}
              aplicarCambios();
            }
          }
          break;
        //selector SEQ.subdivision
        case 4:   
          if (btnDerecha) { 
            if(globalSubdivMode[presetsUI.indexSequence] == 0){ //Modo binario-ternario
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (globalIndexSubdivisiones[presetsUI.indexSequence] < 11){
                  globalIndexSubdivisiones[presetsUI.indexSequence]++;
                  ultimaConfiguracionModo0 = globalIndexSubdivisiones[presetsUI.indexSequence];
                  aplicarCambios();
                }
              }
            }
            else if(globalSubdivMode[presetsUI.indexSequence] == 1){ //modo solo binarios
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (globalIndexSubdivisiones[presetsUI.indexSequence] < 11){
                  globalIndexSubdivisiones[presetsUI.indexSequence] += 2;
                  ultimaConfiguracionModo1 = globalIndexSubdivisiones[presetsUI.indexSequence];
                  aplicarCambios();
                }
              }
            }
            else if(globalSubdivMode[presetsUI.indexSequence] == 2){//modo complejo
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(selectNum && globalIndComplexSubdivX[presetsUI.indexSequence] < 5){
                  globalIndComplexSubdivX[presetsUI.indexSequence]++;
                  ultimaConfiguracionModo2[0] = globalIndComplexSubdivX[presetsUI.indexSequence];
                  if(globalIndComplexSubdivY[presetsUI.indexSequence] == 2 && globalIndComplexSubdivX[presetsUI.indexSequence] > 2){globalIndComplexSubdivX[presetsUI.indexSequence] = 2;}
                  aplicarCambios();
                }
                if(selectDen && globalIndComplexSubdivY[presetsUI.indexSequence] < 2){
                  globalIndComplexSubdivY[presetsUI.indexSequence]++;
                  ultimaConfiguracionModo2[1] = globalIndComplexSubdivY[presetsUI.indexSequence];
                  if(globalIndComplexSubdivY[presetsUI.indexSequence] == 2 && globalIndComplexSubdivX[presetsUI.indexSequence] > 2){globalIndComplexSubdivX[presetsUI.indexSequence] = 2;}
                  aplicarCambios();
                }
              }
            }
          }  
          if (btnIzquierda) {
            if(globalSubdivMode[presetsUI.indexSequence] == 0){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (globalIndexSubdivisiones[presetsUI.indexSequence] > 0){
                  globalIndexSubdivisiones[presetsUI.indexSequence]--;
                  ultimaConfiguracionModo0 = globalIndexSubdivisiones[presetsUI.indexSequence];
                  aplicarCambios();
                }
              }
            }
            else if(globalSubdivMode[presetsUI.indexSequence] == 1){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (globalIndexSubdivisiones[presetsUI.indexSequence] > 1){
                  globalIndexSubdivisiones[presetsUI.indexSequence] -= 2;
                  ultimaConfiguracionModo1 = globalIndexSubdivisiones[presetsUI.indexSequence];
                  aplicarCambios();
                }
              }
            }
            else if(globalSubdivMode[presetsUI.indexSequence] == 2){//modo complejo
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(selectNum && globalIndComplexSubdivX[presetsUI.indexSequence] > 0){
                  globalIndComplexSubdivX[presetsUI.indexSequence]--;
                  ultimaConfiguracionModo2[0] = globalIndComplexSubdivX[presetsUI.indexSequence];
                  aplicarCambios();
                }
                if(selectDen && globalIndComplexSubdivY[presetsUI.indexSequence] > 0){
                  ultimaConfiguracionModo2[1] = globalIndComplexSubdivY[presetsUI.indexSequence];
                  globalIndComplexSubdivY[presetsUI.indexSequence]--;
                  aplicarCambios();
                }
              }
            }
          }
          if (btnOk){
            if(globalSubdivMode[presetsUI.indexSequence] == 2 && seleccion == 4){
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
              if(globalNTotalSteps[presetsUI.indexSequence] < NUM_POTES){
                globalNTotalSteps[presetsUI.indexSequence]++;
                aplicarCambios();
              }
            }
          }
          else if (btnIzquierda) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(globalNTotalSteps[presetsUI.indexSequence] > 1){                
                globalNTotalSteps[presetsUI.indexSequence]--;
                aplicarCambios();
              }
            }
          } 
          break;
        case 6:
          if (btnDerecha) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(SEQ.modeMidiClock < 1){
                SEQ.modeMidiClock++;
                Timer1.initialize(SEQ.tempoMicros);
                Timer1.attachInterrupt(funcionInterrupcion); 
                aplicarCambios();
              }
            }
          }
          else if(btnIzquierda) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(SEQ.modeMidiClock > 0){
                Timer1.stop();
                Timer1.detachInterrupt();
                SEQ.modeMidiClock--;
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