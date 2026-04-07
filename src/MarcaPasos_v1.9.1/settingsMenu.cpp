#include <TimerOne.h>

#include "menus_buttons.h"
#include "MidiProgramming.h"
#include "midiInterface.h"
#include "midiPresets.h"
#include "draw_menus.h"
#include "config.h"

#define SEQ midiProg[presetsUI.indexSequence]

void MenusButtons::comeBack(){
  if(btnSettings){
    putNameFlag = false;
    menuActual = 2;
    nMenuSettings = 0;
    aplicarCambiosBotones();
    return;
  }
}

void MenusButtons::scroll(){
  if(rollDerecha && (scrollOffset + cursorPos) < (maxIndex - 1)){
    if(cursorPos < 3) {cursorPos++;} 
    else {scrollOffset++;}
    drawUI.updateLCD = true;
    aplicarCambiosEncoder();
  }
  else if(rollIzquierda && (scrollOffset + cursorPos) > 0){
    if(cursorPos > 0) {cursorPos--;} 
    else {scrollOffset--;}
    drawUI.updateLCD = true;
    aplicarCambiosEncoder();
  }
}



void MenusButtons::savePreset(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;

  comeBack();
  switch(seleccion){
    case 1:
      if(btnDerecha && whatSaveSelect == 0){
        whatSaveSelect = 1;
        aplicarCambiosBotones(); 
      }
      else if(btnOk){
        seleccion = 2;
        aplicarCambiosBotones(); 
      }
      else if(btnIzquierda && whatSaveSelect == 1){
        whatSaveSelect = 0;
        aplicarCambiosBotones(); 
      }
      break;
    case 2: 
      putNameFlag = true;
      midiUI.charNumber[8] = '\0';
      if(btnOk){
        int len = strlen(midiUI.charNumber);
        while(len > 0 && midiUI.charNumber[len - 1] == ' '){
          midiUI.charNumber[len - 1] = '\0';
          len--;
        }
        bool nombreValido = false;
        for(int i = 0; i < 8; i++){
          if(midiUI.charNumber[i] != '-' && midiUI.charNumber[i] != ' ' && midiUI.charNumber[i] != '\0'){
            nombreValido = true;
            break;
          }
        }
        if(nombreValido){
          putNameFlag = false; 
          presetsUI.saveSeqSD(midiUI.charNumber, whatSaveSelect);
          menuActual = 7;
          presetsUI.ultimoTiempoBotonPresets = tiempoActualMillis;  
          aplicarCambiosBotones(); 
        }
      }
      drawUI.updateLCD = true;
      break;
  }  
}

void MenusButtons::loadPreset(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;

  comeBack();

  switch(seleccion){
    case 1:
      if(btnDerecha && whatSaveSelect == 0){
        whatSaveSelect = 1;
        aplicarCambiosBotones(); 
      }
      else if(btnIzquierda && whatSaveSelect == 1){
        whatSaveSelect = 0;
        aplicarCambiosBotones(); 
      }
      else if(btnOk){
        seleccion = 2;
        maxIndex = presetsUI.getFileCount(whatSaveSelect);
        scrollOffset = 0;
        cursorPos = 0;

        drawUI.updateLCD = true; 
        aplicarCambiosBotones(); 
      }
      break;

    case 2:
      scroll();
      if(btnOk){
        if(maxIndex > 0){
          index = scrollOffset + cursorPos;
          presetsUI.getFileName(index, whatSaveSelect, midiUI.charNumber);
          presetsUI.loadSeqSD(midiUI.charNumber, whatSaveSelect);
        }
        menuActual = 7;
        presetsUI.ultimoTiempoBotonPresets = tiempoActualMillis;
        aplicarCambiosBotones();
      }
      resetEncoder();
      break;
  }
}

void MenusButtons::deletePreset(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;

  comeBack();

  switch(seleccion){
    case 1:
      if(btnDerecha && whatSaveSelect == 0){
        whatSaveSelect = 1;
        aplicarCambiosBotones(); 
      }
      else if(btnIzquierda && whatSaveSelect == 1){
        whatSaveSelect = 0;
        aplicarCambiosBotones(); 
      }
      else if(btnOk){
        seleccion = 2;
        maxIndex = presetsUI.getFileCount(whatSaveSelect);
        scrollOffset = 0;
        cursorPos = 0;

        drawUI.updateLCD = true; 
        aplicarCambiosBotones(); 
      }
      break;
    case 2:
      scroll();
      if(btnOk){
        index = scrollOffset + cursorPos;
        presetsUI.getFileName(index, whatSaveSelect, midiUI.charNumber);
        seleccion = 3;
        deleteConfirm = false;
        aplicarCambiosBotones(); 
      }
      resetEncoder();
      break;
    case 3:
      if(btnDerecha){
        deleteConfirm = true;
        aplicarCambiosBotones(); 
      }
      else if(btnIzquierda){
        deleteConfirm = false;
        aplicarCambiosBotones(); 
      }
      if(btnOk){
        if (deleteConfirm){
          if(maxIndex > 0){
            presetsUI.getFileName(index, whatSaveSelect, midiUI.charNumber);
            presetsUI.deleteSeqSD(midiUI.charNumber, whatSaveSelect);
          }
          menuActual = 7;
          presetsUI.ultimoTiempoBotonPresets = tiempoActualMillis;
          resetEncoder();
          aplicarCambiosBotones();
        }
        else{
          seleccion = 2;
          aplicarCambiosBotones();
        }
      }
      break;
  }
}

void MenusButtons::selectorClock(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;

  comeBack();

  if (rollDerecha) {

    if(SEQ.modeMidiClock == 0){
      SEQ.modeMidiClock = 1;
      Timer1.initialize(SEQ.tempoMicros);
      Timer1.attachInterrupt(funcionInterrupcion); 
      aplicarCambiosEncoder();
    }
  }
  else if(rollIzquierda) {

    if(SEQ.modeMidiClock == 1){
      Timer1.stop();
      Timer1.detachInterrupt();
      SEQ.modeMidiClock = 0;
      aplicarCambiosEncoder();
    }
    
  }
  resetEncoder();
}

void MenusButtons::selectScale(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;

  comeBack();

  switch(seleccion){
    case 1:
      if(btnAbajo){
        seleccion = 2;
        aplicarCambiosBotones();
      }
      else if(rollDerecha){
        s->escalaSeleccionada ++;
        if(s->escalaSeleccionada > 5){s->escalaSeleccionada = 0;}
        for(int i = 0; i < N_MAX_SEQS; i++){
          p->nSequence[i].escalaSeleccionada = s->escalaSeleccionada;
        }
        aplicarCambiosEncoder();
      }
      else if(rollIzquierda){
        s->escalaSeleccionada --;
        if(s->escalaSeleccionada > 5){s->escalaSeleccionada = 5;}
        for(int i = 0; i < N_MAX_SEQS; i++){
          p->nSequence[i].escalaSeleccionada = s->escalaSeleccionada;
        }
        aplicarCambiosEncoder();
      }
      resetEncoder();
      break;
    case 2:
      if(btnArriba){
        seleccion = 1;
        aplicarCambiosBotones();
      }
      else if(rollDerecha){
        s->noteTone++;
        if(s->noteTone > 11){s->noteTone = 0;}
        for(int i = 0; i < N_MAX_SEQS; i++){
          p->nSequence[i].noteTone = s->noteTone;
        }
        aplicarCambiosEncoder();
      }
      else if(rollIzquierda){
        s->noteTone--;
        if(s->noteTone > 11){s->noteTone = 11;}
        aplicarCambiosEncoder();
        for(int i = 0; i < N_MAX_SEQS; i++){
          p->nSequence[i].noteTone = s->noteTone;
        }
      }
      resetEncoder();
      break;
  }
}
void MenusButtons::selectCCNumber(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;

  comeBack();
  if(btnDerecha){
    seleccion++;
    presetsUI.indexSequence++;
    if(seleccion > 5){
      seleccion = 1;
      presetsUI.indexSequence = 0;
    }
    aplicarCambiosBotones();
  }
  else if(btnIzquierda){
    seleccion--;
    presetsUI.indexSequence--;
    if(seleccion < 1){
      seleccion = 5;
      presetsUI.indexSequence = 4;
    }
    aplicarCambiosBotones();
  }
  else if(rollDerecha){
    s->ccNumber ++;
    if(s->ccNumber > 127){s->ccNumber = 0;}
    for(int i = 0; i < N_MAX_SEQS; i++){
      if(i != presetsUI.indexSequence){
        if(s->ccNumber == p->nSequence[i].ccNumber){
          s->ccNumber++;
          if(s->ccNumber > 127){s->ccNumber = 0;}
          i = -1;
        }
      }
    }
    aplicarCambiosEncoder();
  }
  else if(rollIzquierda){
    s->ccNumber --;
    if(s->ccNumber > 127){s->ccNumber = 127;}
    for(int i = 0; i < N_MAX_SEQS; i++){
      if(i != presetsUI.indexSequence){
        if(s->ccNumber == p->nSequence[i].ccNumber){
          s->ccNumber--;
          if(s->ccNumber > 127){s->ccNumber = 127;}
          i = -1;
        }
      }
    }
    aplicarCambiosEncoder();
  }
  resetEncoder();
}
void MenusButtons::saveConfig(){
  if (!timeDebounce()) return;
  if (repeatedButton) return;

  comeBack();
}

