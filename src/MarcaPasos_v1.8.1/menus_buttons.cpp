#include <Arduino.h>
#include <TimerOne.h>
#include <Encoder.h>

#include "menus_buttons.h"
#include "MidiProgramming.h"
#include "midiInterface.h"
#include "midiPresets.h"
#include "draw_menus.h"
#include "config.h"

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

void MenusButtons::checkButtons() {
  syncWithActiveSequence();
  btnArriba = digitalRead(pinButton1);
  btnOk = digitalRead(pinButton2);
  btnAbajo = digitalRead(pinButton3);
  btnVolver = digitalRead(pinButton4);

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

    //MENÚ SECUENCIA
    case 0:
      gestionarMenu1();

      switch(seleccion){
        //selector modo SEQ.subdivision
        case 1:
          selectorEditMode(); break;
        case 3:
          selectorModoSubdivisiones(); break;
        //selector SEQ.subdivision
        case 4:   
          selectorSubdivisiones(); break;
        case 5: //selector pasos
          selectorPasos(); break;
        case 6:
          selectorClock(); break;
      }
      break;

      // MENÚ CC
      case 1:
        selectorNumeroSeqCC(); break;
  }
}

bool MenusButtons::timeDebounce(){
  return (tiempoActualMillis - ultimoTiempoBotonMenu >= 250);
}

void MenusButtons::syncWithActiveSequence() {
  s = presetsUI.getActiveSequence();
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

void MenusButtons::gestionarMenu1(){
  if (!timeDebounce()) return;

  if (btnArriba && seleccion > 1) { 
    if (!repeatedButton && timeDebounce()){
      seleccion --;
      if(seleccion == 2){seleccion = 1;}
      aplicarCambiosBotones(); 
    }
  }
  if (btnAbajo && seleccion < 6) { 
    if (!repeatedButton && timeDebounce()){
      seleccion ++;
      if(seleccion == 2){seleccion = 3;}
      aplicarCambiosBotones();
    }
  }
}

void MenusButtons::selectorEditMode(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;


  if (btnDerecha) {
    presetsUI.indexSequence++;
    if(presetsUI.indexSequence > 4){presetsUI.indexSequence = 0;}
    aplicarCambiosEncoder();
  }
  else if (btnIzquierda) {
    presetsUI.indexSequence--;
    if(presetsUI.indexSequence == 255){presetsUI.indexSequence = 4;}
    aplicarCambiosEncoder();
  }
  else if (!btnOk){
    if(seleccion == 1){menusUI.menuActual = 1;}
    selectNotes = 0;
    selectCC = 1;
    aplicarCambiosBotones();
  }
  resetEncoder();
}

void MenusButtons::selectorModoSubdivisiones(){
  if (!timeDebounce()) return;  

  if (btnDerecha) { 
    s->subdivMode ++;
    if(s->subdivMode > 2){s->subdivMode = 0;}
    if (s->subdivMode == 0) {
      if(SEQ.play){
        s->subdivMode = 2;
        cambioModo0[presetsUI.indexSequence] = true;
      }
      s->indexSubdivisiones = ultimaConfiguracionModo0;
      SEQ.subdivision = SEQ.subdivisionesArray[s->indexSubdivisiones];
      }
    else if (s->subdivMode == 1) {
      s->indexSubdivisiones = ultimaConfiguracionModo1;
      SEQ.subdivision = SEQ.subdivisionesArray[s->indexSubdivisiones];
      }
    else if (s->subdivMode == 2){
      if(SEQ.play){
        s->subdivMode = 1;
        cambioModo2[presetsUI.indexSequence] = true;
      }
      s->indComplexSubdivX = ultimaConfiguracionModo2[0];
      s->indComplexSubdivY = ultimaConfiguracionModo2[1];
      selectNum = true;
      }
    aplicarCambiosEncoder();
  }
  if (btnIzquierda) { 
    s->subdivMode --;
    if(s->subdivMode == 255){s->subdivMode = 2;}
    if (s->subdivMode == 0) {
      s->indexSubdivisiones = ultimaConfiguracionModo0;
      SEQ.subdivision = SEQ.subdivisionesArray[s->indexSubdivisiones];
      }
    else if (s->subdivMode == 1) {
      if(SEQ.play){
        s->subdivMode = 2;
        cambioModo1[presetsUI.indexSequence] = true;
      }
      s->indexSubdivisiones = ultimaConfiguracionModo1;
      SEQ.subdivision = SEQ.subdivisionesArray[s->indexSubdivisiones];
      }
    else if (s->subdivMode == 2){
      if(SEQ.play){
        s->subdivMode = 0;
        cambioModo2[presetsUI.indexSequence] = true;
      }
      s->indComplexSubdivX = ultimaConfiguracionModo2[0];
      s->indComplexSubdivY = ultimaConfiguracionModo2[1];
      selectNum = true;}
    aplicarCambiosEncoder();
  }
  resetEncoder();
}

void MenusButtons::selectorSubdivisiones(){
  if (!timeDebounce()) return;
  switch(s->subdivMode){
    case 0: gestionSubdivGlobal(); break;
    case 1: gestionSubdivBinaria(); break;
    case 2: gestionSubdivCompleja(); break;
  }
}

void MenusButtons::gestionSubdivGlobal(){
  if (btnDerecha) {
    if (s->indexSubdivisiones < 10){
      s->indexSubdivisiones++;
      ultimaConfiguracionModo0 = s->indexSubdivisiones;
      aplicarCambiosEncoder();
    }
  }
  else if (btnIzquierda) {
    if(s->subdivMode == 0){
      if (!timeDebounce()) return;{
        if (s->indexSubdivisiones > 0){
          s->indexSubdivisiones--;
          ultimaConfiguracionModo0 = s->indexSubdivisiones;
          aplicarCambiosEncoder();
        }
      }
    }
  }
  resetEncoder();
}

void MenusButtons::gestionSubdivBinaria(){
  if (btnDerecha) {
    if (s->indexSubdivisiones < 10){
      s->indexSubdivisiones += 2;
      ultimaConfiguracionModo1 = s->indexSubdivisiones;
      aplicarCambiosEncoder();
    }
  }
  else if (btnIzquierda) {
    if (s->indexSubdivisiones > 1){
      s->indexSubdivisiones -= 2;
      ultimaConfiguracionModo1 = s->indexSubdivisiones;
      aplicarCambiosEncoder();
    }
  }
  resetEncoder();
}

void MenusButtons::gestionSubdivCompleja(){
  if (btnDerecha) {
    if(selectNum && s->indComplexSubdivX < 5){
      s->indComplexSubdivX++;
      ultimaConfiguracionModo2[0] = s->indComplexSubdivX;
      if(s->indComplexSubdivY == 2 && s->indComplexSubdivX > 2){s->indComplexSubdivX = 2;}
      aplicarCambiosEncoder();
    }
    if(selectDen && s->indComplexSubdivY < 2){
      s->indComplexSubdivY++;
      ultimaConfiguracionModo2[1] = s->indComplexSubdivY;
      if(s->indComplexSubdivY == 2 && s->indComplexSubdivX > 2){s->indComplexSubdivX = 2;}
      aplicarCambiosEncoder();
    }
  }
  else if (btnIzquierda) {
    if(selectNum && s->indComplexSubdivX > 0){
      s->indComplexSubdivX--;
      ultimaConfiguracionModo2[0] = s->indComplexSubdivX;
      aplicarCambiosEncoder();
    }
    if(selectDen && s->indComplexSubdivY > 0){
      ultimaConfiguracionModo2[1] = s->indComplexSubdivY;
      s->indComplexSubdivY--;
      aplicarCambiosEncoder();
    }
  }
  else if (!btnOk){
    if(s->subdivMode == 2 && seleccion == 4){
      if(selectNum){selectDen = true, selectNum = false;}
      else if(selectDen){selectNum = true, selectDen = false;}
      aplicarCambiosBotones(); 
    }
  }
  resetEncoder();
}

void MenusButtons::selectorPasos(){
  if (!timeDebounce()) return;
  if (btnDerecha) {

    if(s->nTotalSteps < NUM_POTES){
      s->nTotalSteps++;
      aplicarCambiosEncoder();
    }
  }
  else if (btnIzquierda) {

    if(s->nTotalSteps > 1){                
      s->nTotalSteps--;
      aplicarCambiosEncoder();
    }
  } 
  resetEncoder();
}

void MenusButtons::selectorClock(){
  if (btnDerecha) {
    if (!timeDebounce()) return;

    if(SEQ.modeMidiClock == 0){
      SEQ.modeMidiClock = 1;
      Timer1.initialize(SEQ.tempoMicros);
      Timer1.attachInterrupt(funcionInterrupcion); 
      aplicarCambiosEncoder();
    }
  }
  else if(btnIzquierda) {
    if (!timeDebounce()) return;

    if(SEQ.modeMidiClock == 1){
      Timer1.stop();
      Timer1.detachInterrupt();
      SEQ.modeMidiClock = 0;
      aplicarCambiosEncoder();
    }
    
  }
  resetEncoder();
}

void MenusButtons::selectorNumeroSeqCC(){
  if (!timeDebounce()) return;


  if (btnDerecha) {      
    presetsUI.indexSequence++;
    if(presetsUI.indexSequence > 4){presetsUI.indexSequence = 0;}
    aplicarCambiosEncoder();
  }
  else if (btnIzquierda) {    
    presetsUI.indexSequence--;
    if(presetsUI.indexSequence == 255){presetsUI.indexSequence = 4;}
    aplicarCambiosEncoder();
  }
  else if (!btnOk){
    if(seleccion == 1){menusUI.menuActual = 0;}
      selectNotes = 1;
      selectCC = 0;
      aplicarCambiosBotones();
  }
  resetEncoder();
}
void MenusButtons::showPotes(){  // 3. nClockMsgrizador para salir de la pantalla de potenciómetro
  if (menusUI.menuActual == 3 && (tiempoActualMillis - midiUI.timeShowPotValue >= 1500)) {
    menusUI.menuActual = menuAnterior;
    drawUI.updateLCD = true;
  }
}