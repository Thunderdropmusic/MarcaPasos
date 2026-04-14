#include <Arduino.h>
#include <Encoder.h>

#include "menus_buttons.h"
#include "MidiProgramming.h"
#include "midiInterface.h"
#include "midiPresets.h"
#include "draw_menus.h"
#include "config.h"

#define SEQ midiProg[presetsUI.indexSequence]

MenusButtons menusUI;

Encoder miEncoder(19, 18); // Izquierda y derecha

MenusButtons::MenusButtons() : 
  ultimaConfiguracionModo2{0, 2}
{
  menuActual = 0;
  nMenuSettings = 0;
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

  index = 0;
  maxIndex = 0;
  scrollOffset = 0;
  cursorPos = 0;
}

bool MenusButtons::timeDebounce(){
  return (tiempoActualMillis - ultimoTiempoBotonMenu >= DEBOUNCE_DELAY_MS);
}

void MenusButtons::syncWithActiveSequence() {
  if(!menusUI.editExtension){s = presetsUI.getActiveSequence();}
  else{s = presetsUI.getExtensionSequence();}
}


void MenusButtons::resetEncoder(){
  if(rollDerecha == true || rollIzquierda == true){
    rollDerecha = false;
    rollIzquierda = false;
  }
  //repeatedButton = false;
}

void MenusButtons::aplicarCambiosEncoder(){
  drawUI.updateLCD = true;
  //ultimoTiempoBotonMenu = tiempoActualMillis;
  drawUI.updateValues = true;
}

void MenusButtons::aplicarCambiosBotones(){
  drawUI.updateLCD = true;
  ultimoTiempoBotonMenu = tiempoActualMillis;
  repeatedButton = true;
  drawUI.updateValues = true;
}

void MenusButtons::checkSeqMode(){
  syncWithActiveSequence();
  if(s->seqMode == 0){
    menusUI.menuActual = 0;
  }
  else if(s->seqMode == 1){
    menusUI.menuActual = 1;
  }
}

void MenusButtons::checkButtons() {
  syncWithActiveSequence();
  btnArriba = !digitalRead(pinButton1);
  btnOk = !digitalRead(pinButton2);
  btnAbajo = !digitalRead(pinButton3);
  btnIzquierda = !digitalRead(pinButton4);
  btnDerecha = !digitalRead(pinButton5);
  btnSettings = !digitalRead(settingsPin);
  btnExt = !digitalRead(extPin);

  long lecturaBruta = miEncoder.read(); // Leemos sin dividir
  
  // Comprobamos si ha avanzado 4 pulsos enteros (1 click físico real a la derecha)
  if (lecturaBruta - posicionAnterior >= 4) {
    rollDerecha = true;
    posicionAnterior = lecturaBruta; // Guardamos la nueva posición de anclaje
  }
  // Comprobamos si ha retrocedido 4 pulsos (1 click físico real a la izquierda)
  else if (posicionAnterior - lecturaBruta >= 4) {
    rollIzquierda = true;
    posicionAnterior = lecturaBruta;
  }

  if(!btnArriba && !btnOk && !btnAbajo && !btnIzquierda && !btnDerecha){repeatedButton = false;}
  
  switch(menusUI.menuActual){

    //MENÚ SECUENCIA
    case 0:
      gestionarMenu1();

      switch(seleccion){
        //selector modo SEQ.subdivision
        case 1:
          selectorEditMode(); break;
        case 2:
          selectorModoSubdivisiones(); break;
        //selector SEQ.subdivision
        case 3:   
          selectorSubdivisiones(); break;
        case 4: //selector pasos
          selectorPasos(); break;
      }
      break;

      // MENÚ CC
    case 1:
      gestionarMenu2(); //Menu CC
      switch(seleccion){
        case 1:
          selectorNumeroSeqCC(); break;
        case 2:
          selectorPasos(); break;
      }
      break;
    case 2:
      switch(nMenuSettings){
        case 0:
          gestionarMenu3(); break;
        case 1:
          savePreset(); break;
        case 2:
          loadPreset(); break;
        case 3:
          deletePreset();
        case 4:
          selectorClock(); break;
        case 5:
          selectScale(); break;
        case 6:
          selectCCNumber(); break;
        case 7:
          saveConfig();
      }
      break;
  }
}



void MenusButtons::gestionarMenu1(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;
  if(btnSettings){
    menuActual = 2;
    seleccion = 1;
    nMenuSettings = 0;
    aplicarCambiosBotones();
    return;
  }
else if (btnExt){
    if(!editExtension){
      editExtension = true;
      syncWithActiveSequence();
      drawUI.updateValues = true;
    }
    else{
      editExtension = false;
      syncWithActiveSequence();
      drawUI.updateValues = true;
    }  
    aplicarCambiosBotones();
    return;
  }
  switch(seleccion){
    case 1:
      if(btnAbajo){
        seleccion = 2;
        aplicarCambiosBotones(); 
      }
      else if(btnDerecha){
        seleccion = 4;
        aplicarCambiosBotones(); 
      }
      break;
    case 2:
      if(btnArriba){
        seleccion = 1;
        aplicarCambiosBotones(); 
      }
      if(btnAbajo){
        seleccion = 3;
        aplicarCambiosBotones(); 
      }
      break;
    case 3:
      if(btnArriba){
        seleccion = 2;
        aplicarCambiosBotones(); 
      }
      break;
    case 4:
      if(btnIzquierda){
        seleccion = 1;
        aplicarCambiosBotones(); 
      }
      if(btnAbajo){
        seleccion = 2;
        aplicarCambiosBotones(); 
      }
      break;
  }
}

void MenusButtons::gestionarMenu2(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;

  if(btnSettings){
    menuActual = 2;
    seleccion = 1;
    nMenuSettings = 0;
    aplicarCambiosBotones();
    return;
  }

  switch(seleccion){
    case 1:
      if(btnDerecha){
        seleccion = 2;
        aplicarCambiosBotones(); 
      }
      break;
    case 2:
      if(btnIzquierda){
        seleccion = 1;
        aplicarCambiosBotones(); 
      }
      break;
  }
}

void MenusButtons::gestionarMenu3(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;
  if(btnSettings){
    if((menuActual == 2) && nMenuSettings == 0){
      putNameFlag = false;
      if(s->seqMode == 0){menuActual = 0;}
      else if(s->seqMode == 1){menuActual = 1;}
      aplicarCambiosBotones();
      return;
    }
  }
  switch(seleccion){
    case 1:
      if(btnAbajo){
        seleccion = 2;
        aplicarCambiosBotones(); 
      }
      else if(btnOk){
        nMenuSettings = 1;
        seleccion = 1;
        strcpy(midiUI.charNumber, "        ");
        aplicarCambiosBotones();
      }
      break;
    case 2:
      if(btnArriba){
        seleccion = 1;
        aplicarCambiosBotones(); 
      }
      else if(btnOk){
        nMenuSettings = 2;
        seleccion = 1;
        aplicarCambiosBotones();
      }
      else if(btnAbajo){
        seleccion = 3;
        aplicarCambiosBotones(); 
      }
      break;
    case 3:
      if(btnArriba){
        seleccion = 2;
        aplicarCambiosBotones(); 
      }
      else if(btnAbajo){
        seleccion = 4;
        aplicarCambiosBotones(); 
      }
      else if(btnOk){
        nMenuSettings = 3;
        seleccion = 1;
        aplicarCambiosBotones();
      }
      break;
    case 4:
      if(btnArriba){
        seleccion = 3;
        aplicarCambiosBotones(); 
      }
      else if(btnOk){
        nMenuSettings = 4;
        seleccion = 1;
        aplicarCambiosBotones();
      }
      else if(btnAbajo){
        seleccion = 5;
        aplicarCambiosBotones(); 
      }
      break;
    case 5:
      if(btnArriba){
        seleccion = 4;
        aplicarCambiosBotones(); 
      }
      else if(btnOk){
        nMenuSettings = 5;
        seleccion = 1;
        aplicarCambiosBotones();
      }
      else if(btnAbajo){
        seleccion = 6;
        aplicarCambiosBotones(); 
      }
      break;
    case 6:
      if(btnArriba){
        seleccion = 5;
        aplicarCambiosBotones(); 
      }
      else if(btnOk){
        nMenuSettings = 6;
        seleccion = 1;
        aplicarCambiosBotones();
      }
      else if(btnAbajo){
        seleccion = 7;
        aplicarCambiosBotones(); 
      }
      break;
    case 7:
      if(btnArriba){
        seleccion = 6;
        aplicarCambiosBotones(); 
      }
  }
}

void MenusButtons::selectorEditMode(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;


  if (rollDerecha) {
    presetsUI.indexSequence = (presetsUI.indexSequence + 1) % N_MAX_SEQS;
    checkSeqMode();
    aplicarCambiosEncoder();
  }
  else if (rollIzquierda) {
    presetsUI.indexSequence = (presetsUI.indexSequence - 1 + N_MAX_SEQS) % N_MAX_SEQS;
    checkSeqMode();
    aplicarCambiosEncoder();
  }
  else if (btnOk){
    menuActual = 1;
    s->seqMode = 1;
    aplicarCambiosBotones();
  }
  
  resetEncoder();
}

void MenusButtons::selectorModoSubdivisiones(){
  if (!timeDebounce()) return;  

  if (rollDerecha) { 
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
  if (rollIzquierda) { 
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
  if (rollDerecha) {
    if (s->indexSubdivisiones < 10){
      s->indexSubdivisiones++;
      ultimaConfiguracionModo0 = s->indexSubdivisiones;
      aplicarCambiosEncoder();
    }
  }
  else if (rollIzquierda) {
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
  if (rollDerecha) {
    if (s->indexSubdivisiones < 10){
      s->indexSubdivisiones += 2;
      ultimaConfiguracionModo1 = s->indexSubdivisiones;
      aplicarCambiosEncoder();
    }
  }
  else if (rollIzquierda) {
    if (s->indexSubdivisiones > 1){
      s->indexSubdivisiones -= 2;
      ultimaConfiguracionModo1 = s->indexSubdivisiones;
      aplicarCambiosEncoder();
    }
  }
  resetEncoder();
}

void MenusButtons::gestionSubdivCompleja(){
  if (rollDerecha) {
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
  else if (rollIzquierda) {
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
  else if (btnOk){
    if(s->subdivMode == 2 && seleccion == 3){
      if(selectNum){selectDen = true, selectNum = false;}
      else if(selectDen){selectNum = true, selectDen = false;}
      aplicarCambiosBotones(); 
    }
  }
  resetEncoder();
}

void MenusButtons::selectorPasos(){
  if (!timeDebounce()) return;
  if (rollDerecha) {

    if(s->nTotalSteps < N_MAX_STEPS){
      s->nTotalSteps++;
      aplicarCambiosEncoder();
    }
  }
  else if (rollIzquierda) {

    if(s->nTotalSteps > 1){                
      s->nTotalSteps--;
      aplicarCambiosEncoder();
    }
  } 
  resetEncoder();
}


void MenusButtons::selectorNumeroSeqCC(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;

  if (rollDerecha) {
    presetsUI.indexSequence = (presetsUI.indexSequence + 1) % N_MAX_SEQS;
    checkSeqMode();
    aplicarCambiosEncoder();
  }
  else if (rollIzquierda) {    
    presetsUI.indexSequence = (presetsUI.indexSequence - 1 + N_MAX_SEQS) % N_MAX_SEQS;
    checkSeqMode();
    aplicarCambiosEncoder();
  }
  else if (btnOk){
    menusUI.menuActual = 0;
    s->seqMode = 0;
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