#ifndef MENUS_BUTTONS_H
#define MENUS_BUTTONS_H

#include <Arduino.h>
//Variables pines
extern const byte pinButton1; // Arriba
extern const byte pinButton2; // OK / Entrar
extern const byte pinButton3; // Abajo
extern const byte pinButton4; // Volver
extern const byte pinButton5; // Izquierda
extern const byte pinButton6; // Derecha

extern const int numPotes;

// Variables Navegacion
extern int menuActual, menuAnterior, seleccion;
extern bool selectNum;
extern bool selectDen;

// Variables indicies subdivisiones
extern int indiceSubdivisionesComplejasNum, indiceSubdivisionesComplejasDen, indComplexSubdivX, indComplexSubdivY;
int ultimaConfiguracionModo0 = 7;
int ultimaConfiguracionModo1 = 7;
int ultimaConfiguracionModo2[2] = {0, 2};
extern bool cambioModo0 = false;
extern bool cambioModo1 = false;
extern bool cambioModo2 = false;
extern int nTotalSteps;

// Variables de control
static bool repeatedButton = false;

// Variables de tiempo real
extern unsigned long tiempoActualMillis, timeShowPotValue;
unsigned long ultimoTiempoBoton;
extern bool play;

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
      if (btnArriba && seleccion > 1) { 
        if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
          seleccion --;
          aplicarCambios(); 
        }
      }
      if (btnAbajo && seleccion < 5) { 
        if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
          seleccion ++;
          aplicarCambios();
        }
      }
      /*if (btnOk) {
        menuActual = (seleccion == 1) ? 1 : 2;
        menuAnterior = menuActual;
        updateLCD = true;
      }*/
      switch(seleccion){
        //selector modo subdivision
        case 3:
          if (btnDerecha) { 
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
              subDivMode ++;
              if(subDivMode > 2){subDivMode = 0;}
              if (subDivMode == 0) {
                if(play){
                  subDivMode = 2;
                  cambioModo0 = true;
                }
                indiceSubdivisiones = ultimaConfiguracionModo0;
                subdivision = subdivisionesArray[indiceSubdivisiones];
                }
              else if (subDivMode == 1) {
                indiceSubdivisiones = ultimaConfiguracionModo1;
                subdivision = subdivisionesArray[indiceSubdivisiones];
                }
              else if (subDivMode == 2){
                if(play){
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
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){    
              subDivMode --;
              if(subDivMode < 0){subDivMode = 2;}
              if (subDivMode == 0) {
                indiceSubdivisiones = ultimaConfiguracionModo0;
                subdivision = subdivisionesArray[indiceSubdivisiones];
                }
              else if (subDivMode == 1) {
                if(play){
                  subDivMode = 2;
                  cambioModo1 = true;
                }
                indiceSubdivisiones = ultimaConfiguracionModo1;
                subdivision = subdivisionesArray[indiceSubdivisiones];
                }
              else if (subDivMode == 2){
                if(play){
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
        //selector subdivision
        case 4:   
          if (btnDerecha) { 
            if(subDivMode == 0){ //Modo binario-ternario
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if (indiceSubdivisiones < 11){
                  indiceSubdivisiones++;
                  ultimaConfiguracionModo0 = indiceSubdivisiones;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 1){ //modo solo binarios
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if (indiceSubdivisiones < 11){
                  indiceSubdivisiones += 2;
                  ultimaConfiguracionModo1 = indiceSubdivisiones;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 2){//modo complejo
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
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
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if (indiceSubdivisiones > 0){
                  indiceSubdivisiones--;
                  ultimaConfiguracionModo0 = indiceSubdivisiones;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 1){
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if (indiceSubdivisiones > 1){
                  indiceSubdivisiones -= 2;
                  ultimaConfiguracionModo1 = indiceSubdivisiones;
                  aplicarCambios();
                }
              }
            }
            else if(subDivMode == 2){//modo complejo
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
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
              if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
                if(selectNum){selectDen = true, selectNum = false;}
                else if(selectDen){selectNum = true, selectDen = false;}
                aplicarCambios();
              }
            }
          }
        case 5:
          if (btnDerecha) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
              if(nTotalSteps < numPotes){
                nTotalSteps++;
                aplicarCambios();
              }
            }
          }
          else if (btnIzquierda) {
            if (!repeatedButton && tiempoActualMillis - ultimoTiempoBoton >= 250){
              if(nTotalSteps > 0){
                nTotalSteps--;
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