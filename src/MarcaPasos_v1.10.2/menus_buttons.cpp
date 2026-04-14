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
  for(byte i = 0; i < N_MAX_SEQS; i++){
    cambioModo0[i] = false;
    cambioModo1[i] = false;
    cambioModo2[i] = false;
  }

  ultimaConfiguracionModo2[0] = 0;
  ultimaConfiguracionModo2[1] = 2;
  posicionActual = miEncoder.read()/4;
  posicionAnterior = posicionActual;

  index = 0;
  maxIndex = 0;
  scrollOffset = 0;
  cursorPos = 0;
}

// ==============================================================================
//                               FUNCIONES UTILES
// ==============================================================================

bool MenusButtons::timeDebounce(){
  return (tiempoActualMillis - ultimoTiempoBotonMenu >= DEBOUNCE_DELAY_MS);
}

void MenusButtons::syncWithActiveSequence() {
  s = presetsUI.getActiveSequence();
  stp = menusUI.editExtension ? s->ext_steps : s->steps;

  curTotalSteps = menusUI.editExtension ? s->ext_nTotalSteps : s->nTotalSteps;
  curSubdivMode = menusUI.editExtension ? s->ext_subdivMode : s->subdivMode;
  curIndSubdiv  = menusUI.editExtension ? s->ext_indexSubdivisiones : s->indexSubdivisiones;
  curindComplexSubdivY = menusUI.editExtension ? s->ext_indComplexSubdivY : s->indComplexSubdivY;
  curindComplexSubdivX = menusUI.editExtension ? s->ext_indComplexSubdivX : s->indComplexSubdivX;
}

void MenusButtons::commitSequenceChanges() {
  if (editExtension) {
    s->ext_nTotalSteps = curTotalSteps;
    s->ext_subdivMode = curSubdivMode;
    s->ext_indexSubdivisiones = curIndSubdiv;
    s->ext_indComplexSubdivY = curindComplexSubdivY;
    s->ext_indComplexSubdivX = curindComplexSubdivX;
  } else {
    s->nTotalSteps = curTotalSteps;
    s->subdivMode = curSubdivMode;
    s->indexSubdivisiones = curIndSubdiv;
    s->indComplexSubdivY = curindComplexSubdivY;
    s->indComplexSubdivX = curindComplexSubdivX;
  }
}


void MenusButtons::resetEncoder(){
  if(rollDerecha == true || rollIzquierda == true){
    rollDerecha = false;
    rollIzquierda = false;
  }
}

void MenusButtons::aplicarCambiosEncoder(){
  drawUI.updateLCD = true;
  drawUI.updateValues = true;
}

void MenusButtons::aplicarCambiosBotones(){
  drawUI.updateLCD = true;
  ultimoTiempoBotonMenu = tiempoActualMillis;
  repeatedButton = true;
  drawUI.updateValues = true;
}

void MenusButtons::checkSeqMode(){
  s = presetsUI.getActiveSequence();
  if(s->seqMode == 0){
    if(editExtensionHistorial){ editExtension = true;}
    menusUI.menuActual = 0;
    syncWithActiveSequence();
  }
  else if(s->seqMode == 1){
    menusUI.menuActual = 1;
  }
}

// ==============================================================================
//                         CONTROL DE LOS BOTONES
// ==============================================================================

void MenusButtons::checkMenuButtons() {
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

// ==============================================================================
//                                  INDICE SEGÚN MENÚ
// ==============================================================================
  
  switch(menusUI.menuActual){

    // --- MENÚ SECUENCIA ---
    case 0:
      gestionarMenu1();

      switch(seleccion){
        case 1:
          selectorNumSeq(); break;
        case 2:
          selectorModoSubdivisiones(); break;
        case 3:   
          selectorSubdivisiones(); break;
        case 4: 
          selectorPasos(); break;
      }
      break;
    // --- MENÚ CC ---
    case 1:
      gestionarMenu2();
      switch(seleccion){
        case 1:
          selectorNumSeqCC(); break;
        case 2:
          selectorPasos(); break;
        case 3:
          selectorSubdivisiones(); break;
      }
      break;
    // --- MENÚ CONFIGURACIÓN --- 
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

// ==============================================================================
//                        NAVEGACIÓN DIRECCIONAL (D-PAD)
// ==============================================================================


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
      editExtensionHistorial = true;
      syncWithActiveSequence();
      drawUI.updateValues = true;
    }
    else{
      editExtension = false;
      editExtensionHistorial = false;
      syncWithActiveSequence();
      drawUI.updateValues = true;
    }  
    aplicarCambiosBotones();
    return;
  }
  switch(seleccion){
    case 1:
      if(btnAbajo){ seleccion = 2; aplicarCambiosBotones(); }
      else if(btnDerecha){ seleccion = 4; aplicarCambiosBotones(); }
      break;
    case 2:
      if(btnArriba){ seleccion = 1; aplicarCambiosBotones(); }
      if(btnAbajo){ seleccion = 3; aplicarCambiosBotones(); }
      break;
    case 3:
      if(btnArriba) {seleccion = 2; aplicarCambiosBotones(); }
      break;
    case 4:
      if(btnIzquierda){ seleccion = 1; aplicarCambiosBotones(); }
      if(btnAbajo){ seleccion = 2; aplicarCambiosBotones(); }
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
      if(btnDerecha){ seleccion = 2; aplicarCambiosBotones(); }
      break;
    case 2:
      if(btnIzquierda){ seleccion = 1; aplicarCambiosBotones(); }
      else if(btnAbajo){ seleccion = 3; aplicarCambiosBotones(); }
      break;
    case 3:
      if(btnArriba){ seleccion = 2; aplicarCambiosBotones(); }
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
      if(btnAbajo){ seleccion = 2; aplicarCambiosBotones(); }
      else if(btnOk){
        nMenuSettings = 1;
        seleccion = 1;
        strcpy(midiUI.charNumber, "        "); // Limpiamos el nombre  
        aplicarCambiosBotones();
      }
      break;
    case 2:
      if(btnArriba){ seleccion = 1; aplicarCambiosBotones(); }
  if(btnAbajo){ seleccion = 3; aplicarCambiosBotones(); }
      else if(btnOk){ nMenuSettings = 2; seleccion = 1; aplicarCambiosBotones(); }
      break;
    case 3:
      if(btnArriba){ seleccion = 2; aplicarCambiosBotones(); }
      else if(btnAbajo){ seleccion = 4; aplicarCambiosBotones(); }
      else if(btnOk){ nMenuSettings = 3; seleccion = 1; aplicarCambiosBotones(); }
      break;
    case 4:
      if(btnArriba){ seleccion = 3; aplicarCambiosBotones(); }
      else if(btnAbajo){ seleccion = 5; aplicarCambiosBotones(); }
      else if(btnOk){ nMenuSettings = 4; seleccion = 1; aplicarCambiosBotones(); }
      break;
    case 5:
      if(btnArriba){ seleccion = 4; aplicarCambiosBotones(); }
      else if(btnAbajo){ seleccion = 6; aplicarCambiosBotones(); }
      else if(btnOk){ nMenuSettings = 5; seleccion = 1; aplicarCambiosBotones(); }
      break;
    case 6:
      if(btnArriba){ seleccion = 5; aplicarCambiosBotones(); }
      else if(btnAbajo){ seleccion = 7; aplicarCambiosBotones(); }
      else if(btnOk){ nMenuSettings = 6; seleccion = 1; aplicarCambiosBotones(); }
      break;
    case 7:
      if(btnArriba){ seleccion = 6; }
  }
}

// ==============================================================================
//                            MODIFICACIÓN DE VALORES
// ==============================================================================

void MenusButtons::selectorNumSeq(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;


  if (rollDerecha) {
    presetsUI.indexSequence = (presetsUI.indexSequence + 1) % N_MAX_SEQS; // Cuando supera el número máximo de secuencia, vuelve a 0
    checkSeqMode();
    aplicarCambiosEncoder();
  }
  else if (rollIzquierda) {
    presetsUI.indexSequence = (presetsUI.indexSequence - 1 + N_MAX_SEQS) % N_MAX_SEQS; // Cuando supera el número máximo de secuencia, vuelve a N_MAX_SEQS
    checkSeqMode();
    aplicarCambiosEncoder();
  }
  else if (btnOk){
    menuActual = 1;
    s = presetsUI.getActiveSequence();
    s->seqMode = 1;
    curSubdivMode = 0;
    commitSequenceChanges();
    aplicarCambiosBotones();
  }
  
  resetEncoder();
}

void MenusButtons::selectorModoSubdivisiones(){
  if (!timeDebounce()) return;  

  if (rollDerecha) { 
    curSubdivMode ++;
    if(curSubdivMode > 2){ curSubdivMode = 0; }
    
    // Logica de adaptación de valores
    if (curSubdivMode == 0) {
      if(SEQ.play){
        curSubdivMode = 2;
        cambioModo0[presetsUI.indexSequence] = true;
      }
      curIndSubdiv = ultimaConfiguracionModo0;
      SEQ.subdivision = SEQ.subdivisionesArray[curIndSubdiv];
    }

    else if (curSubdivMode == 1) {
      curIndSubdiv = ultimaConfiguracionModo1;
      SEQ.subdivision = SEQ.subdivisionesArray[curIndSubdiv];
    }

    else if (curSubdivMode == 2){
      if(SEQ.play){
        curSubdivMode = 1;
        cambioModo2[presetsUI.indexSequence] = true;
      }
      curindComplexSubdivX = ultimaConfiguracionModo2[0];
      curindComplexSubdivY = ultimaConfiguracionModo2[1];
      selectNum = true;
    }
    aplicarCambiosEncoder();
  }

  if (rollIzquierda) { 
    curSubdivMode --;
    if(curSubdivMode == 255){curSubdivMode = 2;} // 255 es el Underflow de un byte al restar de 0
    
    // Logica de adaptación de valores
    if (curSubdivMode == 0) {
      curIndSubdiv = ultimaConfiguracionModo0;
      SEQ.subdivision = SEQ.subdivisionesArray[curIndSubdiv];
    }

    else if (curSubdivMode == 1) {
      if(SEQ.play){
        curSubdivMode = 2;
        cambioModo1[presetsUI.indexSequence] = true;
      }
      curIndSubdiv = ultimaConfiguracionModo1;
      SEQ.subdivision = SEQ.subdivisionesArray[curIndSubdiv];
    }

    else if (curSubdivMode == 2){
      if(SEQ.play){
        curSubdivMode = 0;
        cambioModo2[presetsUI.indexSequence] = true;
      }
      curindComplexSubdivX = ultimaConfiguracionModo2[0];
      curindComplexSubdivY = ultimaConfiguracionModo2[1];
      selectNum = true;}
    aplicarCambiosEncoder();
  }

  commitSequenceChanges();
  resetEncoder();
}

void MenusButtons::selectorSubdivisiones(){
  if (!timeDebounce()) return;
  switch(curSubdivMode){
    case 0: gestionSubdivGlobal(); break;
    case 1: gestionSubdivBinaria(); break;
    case 2: gestionSubdivCompleja(); break;
  }
}

void MenusButtons::gestionSubdivGlobal(){
  if (rollDerecha) {
    if (curIndSubdiv < 10){
      curIndSubdiv++;
      ultimaConfiguracionModo0 = curIndSubdiv;
      aplicarCambiosEncoder();
    }
  }
  else if (rollIzquierda) {
    if (!timeDebounce()) return;{
      if (curIndSubdiv > 0){
        curIndSubdiv--;
        ultimaConfiguracionModo0 = curIndSubdiv;
        aplicarCambiosEncoder();
      }
    }
  }
  commitSequenceChanges();
  resetEncoder();
}

void MenusButtons::gestionSubdivBinaria(){
  if (rollDerecha) {
    if (curIndSubdiv < 10){
      curIndSubdiv += 2;
      ultimaConfiguracionModo1 = curIndSubdiv;
      aplicarCambiosEncoder();
    }
  }
  else if (rollIzquierda) {
    if (curIndSubdiv > 1){
      curIndSubdiv -= 2;
      ultimaConfiguracionModo1 = curIndSubdiv;
      aplicarCambiosEncoder();
    }
  }
  commitSequenceChanges();
  resetEncoder();
}

void MenusButtons::gestionSubdivCompleja(){
  if (rollDerecha) {
    if(selectNum && curindComplexSubdivX < 5){
      curindComplexSubdivX++;
      ultimaConfiguracionModo2[0] = curindComplexSubdivX;
      if(curindComplexSubdivY == 2 && curindComplexSubdivX > 2){curindComplexSubdivX = 2;}
      aplicarCambiosEncoder();
    }
    if(selectDen && curindComplexSubdivY < 2){
      curindComplexSubdivY++;
      ultimaConfiguracionModo2[1] = curindComplexSubdivY;
      if(curindComplexSubdivY == 2 && curindComplexSubdivX > 2){curindComplexSubdivX = 2;}
      aplicarCambiosEncoder();
    }
  }
  else if (rollIzquierda) {
    if(selectNum && curindComplexSubdivX > 0){
      curindComplexSubdivX--;
      ultimaConfiguracionModo2[0] = curindComplexSubdivX;
      aplicarCambiosEncoder();
    }
    if(selectDen && curindComplexSubdivY > 0){
      ultimaConfiguracionModo2[1] = curindComplexSubdivY;
      curindComplexSubdivY--;
      aplicarCambiosEncoder();
    }
  }
  else if (btnOk){
    if(curSubdivMode == 2 && seleccion == 3){
      selectNum = !selectNum;
      selectDen = !selectDen;
      aplicarCambiosBotones(); 
    }
  }
  commitSequenceChanges();
  resetEncoder();
}

void MenusButtons::selectorPasos(){
  if (!timeDebounce()) return;

  if (rollDerecha) {
    if(curTotalSteps < N_MAX_STEPS){ curTotalSteps++; aplicarCambiosEncoder(); }
  }

  else if (rollIzquierda) {
    if(curTotalSteps > 1){ curTotalSteps--; aplicarCambiosEncoder(); }
  }
  commitSequenceChanges();
  resetEncoder();
}


void MenusButtons::selectorNumSeqCC(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;
  editExtension = false;

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
    if(editExtensionHistorial){ editExtension = true; }
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