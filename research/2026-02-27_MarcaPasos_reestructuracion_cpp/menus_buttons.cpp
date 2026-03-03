#include <Arduino.h>
#include "menus_buttons.h"
#include "MidiProgramming.h"
#include "midiInterface.h"

MenusButtons menusUI;

MenusButtons::MenusButtons() : 
  ultimaConfiguracionModo2{0, 2}

{
  menuActual = 0;
  seleccion = 1;
  indiceSubdivisiones = 7;
  subDivMode = 0;
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
      if (btnAbajo && seleccion < 5) { 
        if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
          seleccion ++;
          aplicarCambios();
        }
      }
      switch(seleccion){
        //selector modo midiProg.subdivision
        case 3:
          if (btnDerecha) { 
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              subDivMode ++;
              if(subDivMode > 2){subDivMode = 0;}
              if (subDivMode == 0) {
                if(midiProg.play){
                  subDivMode = 2;
                  cambioModo0 = true;
                }
                indiceSubdivisiones = ultimaConfiguracionModo0;
                midiProg.subdivision = midiProg.subdivisionesArray[indiceSubdivisiones];
                }
              else if (subDivMode == 1) {
                indiceSubdivisiones = ultimaConfiguracionModo1;
                midiProg.subdivision = midiProg.subdivisionesArray[indiceSubdivisiones];
                }
              else if (subDivMode == 2){
                if(midiProg.play){
                  subDivMode = 1;
                  cambioModo2 = true;
                }
                indComplexSubdivX = ultimaConfiguracionModo2[0];
                indComplexSubdivY = ultimaConfiguracionModo2[1];
                selectNum = true;
                }
              aplicarCambios();
            }
          }
          if (btnIzquierda) { 
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){    
              subDivMode --;
              if(subDivMode < 0){subDivMode = 2;}
              if (subDivMode == 0) {
                indiceSubdivisiones = ultimaConfiguracionModo0;
                midiProg.subdivision = midiProg.subdivisionesArray[indiceSubdivisiones];
                }
              else if (subDivMode == 1) {
                if(midiProg.play){
                  subDivMode = 2;
                  cambioModo1 = true;
                }
                indiceSubdivisiones = ultimaConfiguracionModo1;
                midiProg.subdivision = midiProg.subdivisionesArray[indiceSubdivisiones];
                }
              else if (subDivMode == 2){
                if(midiProg.play){
                  subDivMode = 0;
                  cambioModo2 = true;
                }
                indComplexSubdivX = ultimaConfiguracionModo2[0];
                indComplexSubdivY = ultimaConfiguracionModo2[1];
                selectNum = true;}
              aplicarCambios();
            }
          }
          break;
        //selector midiProg.subdivision
        case 4:   
          if (btnDerecha) { 
            if(subDivMode == 0){ //Modo binario-ternario
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (indiceSubdivisiones < 11){
                  indiceSubdivisiones++;
                  ultimaConfiguracionModo0 = indiceSubdivisiones;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 1){ //modo solo binarios
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (indiceSubdivisiones < 11){
                  indiceSubdivisiones += 2;
                  ultimaConfiguracionModo1 = indiceSubdivisiones;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 2){//modo complejo
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(selectNum && indComplexSubdivX < 5){
                  indComplexSubdivX++;
                  ultimaConfiguracionModo2[0] = indComplexSubdivX;
                  if(indComplexSubdivY == 2 && indComplexSubdivX > 2){indComplexSubdivX = 2;}
                  aplicarCambios();
                }
                if(selectDen && indComplexSubdivY < 2){
                  indComplexSubdivY++;
                  ultimaConfiguracionModo2[1] = indComplexSubdivY;
                  if(indComplexSubdivY == 2 && indComplexSubdivX > 2){indComplexSubdivX = 2;}
                  aplicarCambios();
                }
              }
            }
          }  
          if (btnIzquierda) {
            if(subDivMode == 0){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (indiceSubdivisiones > 0){
                  indiceSubdivisiones--;
                  ultimaConfiguracionModo0 = indiceSubdivisiones;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 1){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if (indiceSubdivisiones > 1){
                  indiceSubdivisiones -= 2;
                  ultimaConfiguracionModo1 = indiceSubdivisiones;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 2){//modo complejo
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(selectNum && indComplexSubdivX > 0){
                  indComplexSubdivX--;
                  ultimaConfiguracionModo2[0] = indComplexSubdivX;
                  aplicarCambios();
                }
                if(selectDen && indComplexSubdivY > 0){
                  ultimaConfiguracionModo2[1] = indComplexSubdivY;
                  indComplexSubdivY--;
                  aplicarCambios();
                }
              }
            }
          }
          if (btnOk){
            if(subDivMode == 2 && seleccion == 4){
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
              if(midiProg.nTotalSteps < NUM_POTES){
                midiProg.nTotalSteps++;
                aplicarCambios();
              }
            }
          }
          else if (btnIzquierda) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
              if(midiProg.nTotalSteps > 1){
                
                midiProg.nTotalSteps--;
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