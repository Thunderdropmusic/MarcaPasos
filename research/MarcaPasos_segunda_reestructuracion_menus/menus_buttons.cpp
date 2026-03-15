  #include <Arduino.h>
  #include <TimerOne.h>
  #include <Encoder.h>

  #include "globalVars.h"
  #include "menus_buttons.h"
  #include "MidiProgramming.h"
  #include "midiInterface.h"
  #include "midiPresets.h"
  #include "draw_menus.h"

  #define SEQ midiProg[presetsUI.indexNotesSequence]

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
            if(seleccion == 2){seleccion = 1;}
            aplicarCambiosBotones(); 
          }
        }
        if (btnAbajo && seleccion < 6) { 
          if (!repeatedButton && tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
            seleccion ++;
            if(seleccion == 2){seleccion = 3;}
            aplicarCambiosBotones();
          }
        }
        switch(seleccion){
          //selector modo SEQ.subdivision
          case 1:
            if (btnDerecha) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                presetsUI.indexNotesSequence++;
                if(presetsUI.indexNotesSequence > 4){presetsUI.indexNotesSequence = 0;}
                aplicarCambiosEncoder();
              }
            }
            else if (btnIzquierda) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                presetsUI.indexNotesSequence--;
                if(presetsUI.indexNotesSequence < 0){presetsUI.indexNotesSequence = 4;}
                aplicarCambiosEncoder();
              }
            }
            else if (!btnOk){
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(seleccion == 1){menusUI.menuActual = 1;}
                aplicarCambiosBotones();
              }
            }
            resetEncoder();
            break;
          case 3:
            if (btnDerecha) { 
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                globalSubdivMode[presetsUI.indexNotesSequence] ++;
                if(globalSubdivMode[presetsUI.indexNotesSequence] > 2){globalSubdivMode[presetsUI.indexNotesSequence] = 0;}
                if (globalSubdivMode[presetsUI.indexNotesSequence] == 0) {
                  if(SEQ.play){
                    globalSubdivMode[presetsUI.indexNotesSequence] = 2;
                    cambioModo0[presetsUI.indexNotesSequence] = true;
                  }
                  globalIndexSubdivisiones[presetsUI.indexNotesSequence] = ultimaConfiguracionModo0;
                  SEQ.subdivision[presetsUI.indexNotesSequence] = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexNotesSequence]];
                  }
                else if (globalSubdivMode[presetsUI.indexNotesSequence] == 1) {
                  globalIndexSubdivisiones[presetsUI.indexNotesSequence] = ultimaConfiguracionModo1;
                  SEQ.subdivision[presetsUI.indexNotesSequence] = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexNotesSequence]];
                  }
                else if (globalSubdivMode[presetsUI.indexNotesSequence] == 2){
                  if(SEQ.play){
                    globalSubdivMode[presetsUI.indexNotesSequence] = 1;
                    cambioModo2[presetsUI.indexNotesSequence] = true;
                  }
                  globalIndComplexSubdivX[presetsUI.indexNotesSequence] = ultimaConfiguracionModo2[0];
                  globalIndComplexSubdivY[presetsUI.indexNotesSequence] = ultimaConfiguracionModo2[1];
                  selectNum = true;
                  }
                aplicarCambiosEncoder();
              }
            }
            if (btnIzquierda) { 
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){    
                globalSubdivMode[presetsUI.indexNotesSequence] --;
                if(globalSubdivMode[presetsUI.indexNotesSequence] < 0){globalSubdivMode[presetsUI.indexNotesSequence] = 2;}
                if (globalSubdivMode[presetsUI.indexNotesSequence] == 0) {
                  globalIndexSubdivisiones[presetsUI.indexNotesSequence] = ultimaConfiguracionModo0;
                  SEQ.subdivision[presetsUI.indexNotesSequence] = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexNotesSequence]];
                  }
                else if (globalSubdivMode[presetsUI.indexNotesSequence] == 1) {
                  if(SEQ.play){
                    globalSubdivMode[presetsUI.indexNotesSequence] = 2;
                    cambioModo1[presetsUI.indexNotesSequence] = true;
                  }
                  globalIndexSubdivisiones[presetsUI.indexNotesSequence] = ultimaConfiguracionModo1;
                  SEQ.subdivision[presetsUI.indexNotesSequence] = SEQ.subdivisionesArray[globalIndexSubdivisiones[presetsUI.indexNotesSequence]];
                  }
                else if (globalSubdivMode[presetsUI.indexNotesSequence] == 2){
                  if(SEQ.play){
                    globalSubdivMode[presetsUI.indexNotesSequence] = 0;
                    cambioModo2[presetsUI.indexNotesSequence] = true;
                  }
                  globalIndComplexSubdivX[presetsUI.indexNotesSequence] = ultimaConfiguracionModo2[0];
                  globalIndComplexSubdivY[presetsUI.indexNotesSequence] = ultimaConfiguracionModo2[1];
                  selectNum = true;}
                aplicarCambiosEncoder();
              }
            }
            resetEncoder();
            break;
          //selector SEQ.subdivision
          case 4:   
            if (btnDerecha) { 
              if(globalSubdivMode[presetsUI.indexNotesSequence] == 0){ //Modo binario-ternario
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if (globalIndexSubdivisiones[presetsUI.indexNotesSequence] < 10){
                    globalIndexSubdivisiones[presetsUI.indexNotesSequence]++;
                    ultimaConfiguracionModo0 = globalIndexSubdivisiones[presetsUI.indexNotesSequence];
                    aplicarCambiosEncoder();
                  }
                }
              }
              else if(globalSubdivMode[presetsUI.indexNotesSequence] == 1){ //modo solo binarios
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if (globalIndexSubdivisiones[presetsUI.indexNotesSequence] < 10){
                    globalIndexSubdivisiones[presetsUI.indexNotesSequence] += 2;
                    ultimaConfiguracionModo1 = globalIndexSubdivisiones[presetsUI.indexNotesSequence];
                    aplicarCambiosEncoder();
                  }
                }
              }
              else if(globalSubdivMode[presetsUI.indexNotesSequence] == 2){//modo complejo
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if(selectNum && globalIndComplexSubdivX[presetsUI.indexNotesSequence] < 5){
                    globalIndComplexSubdivX[presetsUI.indexNotesSequence]++;
                    ultimaConfiguracionModo2[0] = globalIndComplexSubdivX[presetsUI.indexNotesSequence];
                    if(globalIndComplexSubdivY[presetsUI.indexNotesSequence] == 2 && globalIndComplexSubdivX[presetsUI.indexNotesSequence] > 2){globalIndComplexSubdivX[presetsUI.indexNotesSequence] = 2;}
                    aplicarCambiosEncoder();
                  }
                  if(selectDen && globalIndComplexSubdivY[presetsUI.indexNotesSequence] < 2){
                    globalIndComplexSubdivY[presetsUI.indexNotesSequence]++;
                    ultimaConfiguracionModo2[1] = globalIndComplexSubdivY[presetsUI.indexNotesSequence];
                    if(globalIndComplexSubdivY[presetsUI.indexNotesSequence] == 2 && globalIndComplexSubdivX[presetsUI.indexNotesSequence] > 2){globalIndComplexSubdivX[presetsUI.indexNotesSequence] = 2;}
                    aplicarCambiosEncoder();
                  }
                }
              }
            }  
            if (btnIzquierda) {
              if(globalSubdivMode[presetsUI.indexNotesSequence] == 0){
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if (globalIndexSubdivisiones[presetsUI.indexNotesSequence] > 0){
                    globalIndexSubdivisiones[presetsUI.indexNotesSequence]--;
                    ultimaConfiguracionModo0 = globalIndexSubdivisiones[presetsUI.indexNotesSequence];
                    aplicarCambiosEncoder();
                  }
                }
              }
              else if(globalSubdivMode[presetsUI.indexNotesSequence] == 1){
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if (globalIndexSubdivisiones[presetsUI.indexNotesSequence] > 1){
                    globalIndexSubdivisiones[presetsUI.indexNotesSequence] -= 2;
                    ultimaConfiguracionModo1 = globalIndexSubdivisiones[presetsUI.indexNotesSequence];
                    aplicarCambiosEncoder();
                  }
                }
              }
              else if(globalSubdivMode[presetsUI.indexNotesSequence] == 2){//modo complejo
                if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                  if(selectNum && globalIndComplexSubdivX[presetsUI.indexNotesSequence] > 0){
                    globalIndComplexSubdivX[presetsUI.indexNotesSequence]--;
                    ultimaConfiguracionModo2[0] = globalIndComplexSubdivX[presetsUI.indexNotesSequence];
                    aplicarCambiosEncoder();
                  }
                  if(selectDen && globalIndComplexSubdivY[presetsUI.indexNotesSequence] > 0){
                    ultimaConfiguracionModo2[1] = globalIndComplexSubdivY[presetsUI.indexNotesSequence];
                    globalIndComplexSubdivY[presetsUI.indexNotesSequence]--;
                    aplicarCambiosEncoder();
                  }
                }
              }
            }
            if (!btnOk){
              if(globalSubdivMode[presetsUI.indexNotesSequence] == 2 && seleccion == 4){
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
                if(globalNTotalSteps[presetsUI.indexNotesSequence] < NUM_POTES){
                  globalNTotalSteps[presetsUI.indexNotesSequence]++;
                  aplicarCambiosEncoder();
                }
              }
            }
            else if (btnIzquierda) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(globalNTotalSteps[presetsUI.indexNotesSequence] > 1){                
                  globalNTotalSteps[presetsUI.indexNotesSequence]--;
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
        case 1:
            if (btnDerecha) {
              if (!tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                globalCCSeqNumber[presetsUI.indexNotesSequence]++;
                if(globalCCSeqNumber[presetsUI.indexNotesSequence] > 4){globalCCSeqNumber[presetsUI.indexNotesSequence] = 0;}
                aplicarCambiosEncoder();
              }
            }
            else if (btnIzquierda) {
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                globalCCSeqNumber[presetsUI.indexNotesSequence]--;
                if(globalCCSeqNumber[presetsUI.indexNotesSequence] < 0){globalCCSeqNumber[presetsUI.indexNotesSequence] = 4;}
                aplicarCambiosEncoder();
              }
            }
            else if (!btnOk){
              if (tiempoActualMillis - ultimoTiempoBotonMenu >= 250){
                if(menusUI.menuActual == 1){menusUI.menuActual = 0;}
                aplicarCambiosBotones();
              }
            }
            resetEncoder();
            break;
        break;
    }
  }
  void MenusButtons::showPotes(){  // 3. nClockMsgrizador para salir de la pantalla de potenciómetro
    if (menusUI.menuActual == 3 && (tiempoActualMillis - midiUI.timeShowPotValue >= 1500)) {
      menusUI.menuActual = menuAnterior;
      drawUI.updateLCD = true;
    }
  }