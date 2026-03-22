  #include <Arduino.h>
  #include <TimerOne.h>
  #include <Encoder.h>

  #include "globalVars.h"
  #include "menus_buttons.h"
  #include "MidiProgramming.h"
  #include "midiInterface.h"
  #include "midiPresets.h"
  #include "draw_menus.h"

  #define SEQ midiProg[presetsUI.indexSequence]

  MenusButtons menusUI;

  Encoder miEncoder(17, 18); // Izquierda y derecha

  MenusButtons::MenusButtons() : 
    ultimaConfiguracionModo2{0, 2}

  {
    menuActual = 0;
    seleccion = 1;
    ultimaConfiguracionModo0 = 6;
    ultimaConfiguracionModo1 = 6;
    for(byte i = 0; i < N_MAX_SEQUENCES; i++){
      cambioModo0[i] = false;
      cambioModo1[i] = false;
      cambioModo2[i] = false;
    }
    posicionActual = miEncoder.read()/4;
    posicionAnterior = posicionActual;
  }

  void MenusButtons::resetEncoder(){
    if(btnDerecha == true || btnIzquierda == true){
      btnDerecha = false;
      btnIzquierda = false;
    }
    repeatedButton = false;
  }

  void MenusButtons::aplicarCambiosEncoder(){
    drawUI.updateLCD = true;
    ultimoTiempoBotonMenu = tiempoActualMillis;
  }

  void MenusButtons::aplicarCambiosBotones(){
    drawUI.updateLCD = true;
    ultimoTiempoBotonMenu = tiempoActualMillis;
    repeatedButton = true;
  }

  void MenusButtons::checkButtons() {
    bool btnArriba = digitalRead(pinButton1);
    bool btnOk = digitalRead(pinButton2);
    bool btnAbajo = digitalRead(pinButton3);
    bool btnVolver = digitalRead(pinButton4);

    posicionActual = miEncoder.read()/4;
    if (posicionActual != posicionAnterior) {
      if(posicionActual > posicionAnterior){
        btnDerecha = true;
      }
      else if(posicionActual < posicionAnterior){
        btnIzquierda = true;
      }
      posicionAnterior = posicionActual;
    }

    if(!btnArriba && btnOk && !btnAbajo && !btnVolver){repeatedButton = false;}
     
    if (btnVolver) { // Volver
        if (menusUI.menuActual != 0) { menusUI.menuActual = 0; menuAnterior = menusUI.menuActual; drawUI.updateLCD = true; }
    }
    
    switch(menusUI.menuActual){

      //MENÚ PRINCIPAL
      case 0:
        if (btnArriba && seleccion > 1) { 
          if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
            seleccion --;
            aplicarCambiosBotones(); 
          }
        }
        if (btnAbajo && seleccion < 6) { 
          if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
            seleccion ++;
            aplicarCambiosBotones();
          }
        }
        switch(seleccion){
          //selector modo SEQ.subdivision
          case 1:
            if (btnDerecha) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                presetsUI.indexSequence++;
                if(presetsUI.indexSequence > 4){presetsUI.indexSequence = 0;}
                aplicarCambiosEncoder();
              }
            }
            else if (btnIzquierda) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                presetsUI.indexSequence--;
                if(presetsUI.indexSequence < 0){presetsUI.indexSequence = 4;}
                aplicarCambiosEncoder();
              }
            }
            resetEncoder();
            break;
          case 2:
            if (btnDerecha) {
              if (!tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(globalModeSequence[presetsUI.indexSequence] == 0){globalModeSequence[presetsUI.indexSequence]++;}
                aplicarCambiosEncoder();
              }
            }
            else if (btnIzquierda) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(globalModeSequence[presetsUI.indexSequence] == 1){globalModeSequence[presetsUI.indexSequence]--;}
                aplicarCambiosEncoder();
              }
            }
            resetEncoder();
            break;
          case 3:
            if (btnDerecha) { 
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                globalSubdivMode[presetsUI.indexSequence] ++;
                if(globalSubdivMode[presetsUI.indexSequence] > 2){globalSubdivMode[presetsUI.indexSequence] = 0;}
                if (globalSubdivMode[presetsUI.indexSequence] == 0) {
                  if(SEQ.play){
                    globalSubdivMode[presetsUI.indexSequence] = 2;
                    cambioModo0[presetsUI.indexSequence] = true;
                  }
                  globalIndexSubdivisiones[presetsUI.indexSequence] = ultimaConfiguracionModo0;
                  SEQ.subdivision[presetsUI.indexSequence] = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexSequence]];
                  }
                else if (globalSubdivMode[presetsUI.indexSequence] == 1) {
                  globalIndexSubdivisiones[presetsUI.indexSequence] = ultimaConfiguracionModo1;
                  SEQ.subdivision[presetsUI.indexSequence] = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexSequence]];
                  }
                else if (globalSubdivMode[presetsUI.indexSequence] == 2){
                  if(SEQ.play){
                    globalSubdivMode[presetsUI.indexSequence] = 1;
                    cambioModo2[presetsUI.indexSequence] = true;
                  }
                  globalIndComplexSubdivX[presetsUI.indexSequence] = ultimaConfiguracionModo2[0];
                  globalIndComplexSubdivY[presetsUI.indexSequence] = ultimaConfiguracionModo2[1];
                  selectNum = true;
                  }
                aplicarCambiosEncoder();
              }
            }
            if (btnIzquierda) { 
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){    
                globalSubdivMode[presetsUI.indexSequence] --;
                if(globalSubdivMode[presetsUI.indexSequence] < 0){globalSubdivMode[presetsUI.indexSequence] = 2;}
                if (globalSubdivMode[presetsUI.indexSequence] == 0) {
                  globalIndexSubdivisiones[presetsUI.indexSequence] = ultimaConfiguracionModo0;
                  SEQ.subdivision[presetsUI.indexSequence] = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexSequence]];
                  }
                else if (globalSubdivMode[presetsUI.indexSequence] == 1) {
                  if(SEQ.play){
                    globalSubdivMode[presetsUI.indexSequence] = 2;
                    cambioModo1[presetsUI.indexSequence] = true;
                  }
                  globalIndexSubdivisiones[presetsUI.indexSequence] = ultimaConfiguracionModo1;
                  SEQ.subdivision[presetsUI.indexSequence] = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexSequence]];
                  }
                else if (globalSubdivMode[presetsUI.indexSequence] == 2){
                  if(SEQ.play){
                    globalSubdivMode[presetsUI.indexSequence] = 0;
                    cambioModo2[presetsUI.indexSequence] = true;
                  }
                  globalIndComplexSubdivX[presetsUI.indexSequence] = ultimaConfiguracionModo2[0];
                  globalIndComplexSubdivY[presetsUI.indexSequence] = ultimaConfiguracionModo2[1];
                  selectNum = true;}
                aplicarCambiosEncoder();
              }
            }
            resetEncoder();
            break;
          //selector SEQ.subdivision
          case 4:   
            if (btnDerecha) { 
              if(globalSubdivMode[presetsUI.indexSequence] == 0){ //Modo binario-ternario
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if (globalIndexSubdivisiones[presetsUI.indexSequence] < 10){
                    globalIndexSubdivisiones[presetsUI.indexSequence]++;
                    ultimaConfiguracionModo0 = globalIndexSubdivisiones[presetsUI.indexSequence];
                    aplicarCambiosEncoder();
                  }
                }
              }
              else if(globalSubdivMode[presetsUI.indexSequence] == 1){ //modo solo binarios
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if (globalIndexSubdivisiones[presetsUI.indexSequence] < 10){
                    globalIndexSubdivisiones[presetsUI.indexSequence] += 2;
                    ultimaConfiguracionModo1 = globalIndexSubdivisiones[presetsUI.indexSequence];
                    aplicarCambiosEncoder();
                  }
                }
              }
              else if(globalSubdivMode[presetsUI.indexSequence] == 2){//modo complejo
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if(selectNum && globalIndComplexSubdivX[presetsUI.indexSequence] < 5){
                    globalIndComplexSubdivX[presetsUI.indexSequence]++;
                    ultimaConfiguracionModo2[0] = globalIndComplexSubdivX[presetsUI.indexSequence];
                    if(globalIndComplexSubdivY[presetsUI.indexSequence] == 2 && globalIndComplexSubdivX[presetsUI.indexSequence] > 2){globalIndComplexSubdivX[presetsUI.indexSequence] = 2;}
                    aplicarCambiosEncoder();
                  }
                  if(selectDen && globalIndComplexSubdivY[presetsUI.indexSequence] < 2){
                    globalIndComplexSubdivY[presetsUI.indexSequence]++;
                    ultimaConfiguracionModo2[1] = globalIndComplexSubdivY[presetsUI.indexSequence];
                    if(globalIndComplexSubdivY[presetsUI.indexSequence] == 2 && globalIndComplexSubdivX[presetsUI.indexSequence] > 2){globalIndComplexSubdivX[presetsUI.indexSequence] = 2;}
                    aplicarCambiosEncoder();
                  }
                }
              }
            }  
            if (btnIzquierda) {
              if(globalSubdivMode[presetsUI.indexSequence] == 0){
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if (globalIndexSubdivisiones[presetsUI.indexSequence] > 0){
                    globalIndexSubdivisiones[presetsUI.indexSequence]--;
                    ultimaConfiguracionModo0 = globalIndexSubdivisiones[presetsUI.indexSequence];
                    aplicarCambiosEncoder();
                  }
                }
              }
              else if(globalSubdivMode[presetsUI.indexSequence] == 1){
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if (globalIndexSubdivisiones[presetsUI.indexSequence] > 1){
                    globalIndexSubdivisiones[presetsUI.indexSequence] -= 2;
                    ultimaConfiguracionModo1 = globalIndexSubdivisiones[presetsUI.indexSequence];
                    aplicarCambiosEncoder();
                  }
                }
              }
              else if(globalSubdivMode[presetsUI.indexSequence] == 2){//modo complejo
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if(selectNum && globalIndComplexSubdivX[presetsUI.indexSequence] > 0){
                    globalIndComplexSubdivX[presetsUI.indexSequence]--;
                    ultimaConfiguracionModo2[0] = globalIndComplexSubdivX[presetsUI.indexSequence];
                    aplicarCambiosEncoder();
                  }
                  if(selectDen && globalIndComplexSubdivY[presetsUI.indexSequence] > 0){
                    ultimaConfiguracionModo2[1] = globalIndComplexSubdivY[presetsUI.indexSequence];
                    globalIndComplexSubdivY[presetsUI.indexSequence]--;
                    aplicarCambiosEncoder();
                  }
                }
              }
            }
            if (!btnOk){
              if(globalSubdivMode[presetsUI.indexSequence] == 2 && seleccion == 4){
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if(selectNum){selectDen = true, selectNum = false;}
                  else if(selectDen){selectNum = true, selectDen = false;}
                  aplicarCambiosBotones();
                }
              }
            }
            resetEncoder();
            break;
          case 5: //selector pasos
            if (btnDerecha) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(globalNTotalSteps[presetsUI.indexSequence] < NUM_POTES){
                  globalNTotalSteps[presetsUI.indexSequence]++;
                  aplicarCambiosEncoder();
                }
              }
            }
            else if (btnIzquierda) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(globalNTotalSteps[presetsUI.indexSequence] > 1){                
                  globalNTotalSteps[presetsUI.indexSequence]--;
                  aplicarCambiosEncoder();
                }
              }
            } 
            resetEncoder();
            break;
          case 6:
            if (btnDerecha) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(SEQ.modeMidiClock == 0){
                  SEQ.modeMidiClock = 1;
                  Timer1.initialize(SEQ.tempoMicros);
                  Timer1.attachInterrupt(funcionInterrupcion); 
                  aplicarCambiosEncoder();
                }
              }
            }
            else if(btnIzquierda) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(SEQ.modeMidiClock == 1){
                  Timer1.stop();
                  Timer1.detachInterrupt();
                  SEQ.modeMidiClock = 0;
                  aplicarCambiosEncoder();
                }
              }
            }
            resetEncoder();
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