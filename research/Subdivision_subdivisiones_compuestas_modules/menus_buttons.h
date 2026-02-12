#ifndef MENUS_BUTTONS_H
#define MENUS_BUTTONS_H

#include <Arduino.h>
//Variables pines
extern const int pinButton1; // Arriba
extern const int pinButton2; // OK / Entrar
extern const int pinButton3; // Abajo
extern const int pinButton4; // Volver
extern const int pinButton5; // Izquierda
extern const int pinButton6; // Derecha

//Variables Navegacion
extern int menuActual, menuAnterior, seleccion, indiceSubdivisionesComplejasNum, indiceSubdivisionesComplejasDen, indComplexSubdivX, indComplexSubdivY;

//Variables de control
static bool repeatedButton = false;
extern unsigned long tiempoActualMillis, timeShowPotValue;
unsigned long int ultimoTiempoBoton;

extern bool selectNum;
extern bool selectDen;

void aplicarCambios(){
  updateLCD = true;
  repeatedButton = true;
  ultimoTiempoBoton = tiempoActualMillis;
}

void checkButtons() {
  bool btnArriba = digitalRead(pinButton1);
  bool btnOk = digitalRead(pinButton2);
  bool btnAbajo = digitalRead(pinButton3);
  bool btnVolver = digitalRead(pinButton4);
  bool btnIzquierda = digitalRead(pinButton5);
  bool btnDerecha = digitalRead(pinButton6);


  if(!btnArriba && !btnOk && !btnAbajo && !btnVolver && !btnIzquierda && !btnDerecha){repeatedButton = false;}
  if (btnVolver) { // Volver
      if (menuActual != 0) { menuActual = 0; menuAnterior = menuActual; updateLCD = true; }
  }
  
  switch(menuActual){

    //MENÚ PRINCIPAL
    case 0:
      if (btnArriba && seleccion != 1) { seleccion = 1; updateLCD = true; }
      if (btnAbajo && seleccion != 3) { seleccion = 3; updateLCD = true; } 
      if (btnOk) {
        menuActual = (seleccion == 1) ? 1 : 2;
        menuAnterior = menuActual;
        updateLCD = true;
      }
      break;

    //MENÚ SUBDIVISIONES
    case 1:
      if (btnArriba && seleccion != 1) { seleccion = 1; updateLCD = true; }
      if (btnAbajo && seleccion != 3) { seleccion = 3; updateLCD = true; }

      switch(seleccion){
        //selector modo subdivision
        case 1:
          if (btnDerecha) { 
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
              if(subDivMode < 2){
                subDivMode ++;
                aplicarCambios();
                if (subDivMode == 1 && indiceSubdivisiones % 2 == 0) {indiceSubdivisiones = 7;}
                if (subDivMode == 2){indComplexSubdivX = 0, indComplexSubdivY = 2, selectNum = true;}
              }
            }
          }
          if (btnIzquierda) { 
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){    
              if(subDivMode > 0){
                subDivMode --;
                aplicarCambios();
                if (subDivMode == 1 && indiceSubdivisiones % 2 == 0) {indiceSubdivisiones = 7;}
              }
            }
          }
          break;
        //selector subdivision
        case 3:   
          if (btnDerecha) { 
            if(subDivMode == 0){ //Modo binario-ternario
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if (indiceSubdivisiones < 11){
                  indiceSubdivisiones++;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 1){ //modo solo binarios
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if (indiceSubdivisiones < 11){
                  indiceSubdivisiones += 2;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 2){//modo complejo
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if(selectNum && indComplexSubdivX < 5){
                  indComplexSubdivX++;
                  aplicarCambios();
                }
                if(selectDen && indComplexSubdivY < 2){
                  indComplexSubdivY++;
                  aplicarCambios();
                }
              }
            }
          }  
          if (btnIzquierda) {
            if(subDivMode == 0){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if (indiceSubdivisiones > 0){
                  indiceSubdivisiones--;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 1){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if (indiceSubdivisiones > 1){
                  indiceSubdivisiones -= 2;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 2){//modo complejo
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if(selectNum && indComplexSubdivX > 0){
                  indComplexSubdivX--;
                  aplicarCambios();
                }
                if(selectDen && indComplexSubdivY > 0){
                  indComplexSubdivY--;
                  aplicarCambios();
                }
              }
            }
          }
          if (btnOk){
            if(subDivMode == 2 && seleccion == 3){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if(selectNum){selectDen = true, selectNum = false;}
                else if(selectDen){selectNum = true, selectDen = false;}
                aplicarCambios();
              }
            }
          }
          break;
      }
      break;
  }
}
void showPotes(){  // 3. nClockMsgrizador para salir de la pantalla de potenciómetro
  if (menuActual == 3 && (tiempoActualMillis - timeShowPotValue >= 1500)) {
    menuActual = menuAnterior;
    updateLCD = true;
  }
}

#endif