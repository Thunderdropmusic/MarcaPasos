#include "draw_menus.h"
#include "midiProgramming.h"
#include "midiInterface.h"
#include "menus_buttons.h"
#include "midiPresets.h"
#include "config.h"

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// ==============================================================================
//                         DIBUJADO DE LA CONFIGURACIÓN
// ==============================================================================

void DrawMenus::drawMenuSettings(){
  switch(menusUI.nMenuSettings){
    // MENU PRINCIPAL DE CONFIGURACION    
    case 0: 
      if(menusUI.seleccion == 1 || menusUI.seleccion == 2 || menusUI.seleccion == 3 || menusUI.seleccion == 4){nScreen = 0;}
      else if(menusUI.seleccion == 5 || menusUI.seleccion == 6 || menusUI.seleccion == 7 || menusUI.seleccion == 8){nScreen = 1;}  
      if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreenSettings();}    
      
      for(int i = 0; i < 4; i++){
        printAt(1, i, " ");
      }
      writeAt(1, (menusUI.seleccion - 1) % 4, byte(1));
      break;
    // --- SAVE PRESET ---
    case 1:
      nScreen = 2;
      switch(menusUI.seleccion){
        // PANTALLA DE SELECCIÓN DE TIPO (Sequence o Pattern)
        case 1:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreenSettings();}
          lcd.noBlink();
          if(menusUI.whatSaveSelect == 0) {printAt(5, 1, "Sequence"), lcd.write(byte(1)), printAt(9,2,".seq");}
          else if(menusUI.whatSaveSelect == 1) {writeAt(4, 1, byte(0)), lcd.print("Pattern"), printAt(9,2,".pat");}
          printAt(1,2,midiUI.charNumber);
          break;
        // PANTALLA DE ESCRITURA DEL NOMBRE DEL ARCHIVO
        case 2:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreenSettings();}
          lcd.noBlink();
          if(menusUI.whatSaveSelect == 0) {printAt(5, 1, "Sequence"), lcd.print(" "), printAt(9,2,".seq");}
          else if(menusUI.whatSaveSelect == 1) {printAt(4, 1, " "), lcd.print("Pattern"),printAt(9,2,".pat");}
          writeAt(0, 2, byte(0)), printAt(1,2,midiUI.charNumber), writeAt(13, 2, byte(0));
          break;
      }
      break;
    // --- LOAD PRESET ---
    case 2:
      nScreen = 3;
      switch(menusUI.seleccion){
        // PANTALLA DE SELECCIÓN DE TIPO (Sequence o Pattern)
        case 1:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreenSettings();}
          lcd.noBlink();
          if(menusUI.whatSaveSelect == 0) {printAt(8, 1, "Sequence"), lcd.write(byte(1));}
          else if(menusUI.whatSaveSelect == 1) {writeAt(7, 1, byte(0)), lcd.print("Pattern");}
          break; 
        // PANTALLA DE SELECCIÓN DE ARCHIVO
        case 2:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){
            lcd.clear(); 
            nAnteriorScreen = nScreen;
            menusUI.menuAnterior = menusUI.menuActual;
            updateValues = false;
          }

          for(int i = 0; i < 4; i++){
            if(i == menusUI.cursorPos){writeAt(0, i, byte(1));} 
            else {printAt(0, i, " ");}
            if((menusUI.scrollOffset + i) < menusUI.maxIndex){
              presetsUI.getFileName(menusUI.scrollOffset + i, menusUI.whatSaveSelect, midiUI.charNumber);
              printAt(1, i, midiUI.charNumber);
              if(menusUI.whatSaveSelect == 0) {lcd.print(".seq");}
              else if(menusUI.whatSaveSelect == 1) {lcd.print(".pat");}    
            } 
            else {printAt(1, i, "            ");}
          }
          break;
      }
      break;
    // --- DELETE PRESET ---
    case 3:
      nScreen = 4;
      switch(menusUI.seleccion){
        case 1:
          // PANTALLA DE SELECCIÓN DE TIPO (Sequence o Pattern)
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreenSettings();}
          lcd.noBlink();
          if(menusUI.whatSaveSelect == 0) {printAt(9, 1, "Sequence"), lcd.write(byte(1));}
          else if(menusUI.whatSaveSelect == 1) {writeAt(8, 1, byte(0)), lcd.print("Pattern");}
          break; 
          // PANTALLA DE SELECCIÓN DE ARCHIVO
        case 2:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){
            lcd.clear(); 
            nAnteriorScreen = nScreen;
            menusUI.menuAnterior = menusUI.menuActual;
            updateValues = false;
          }

          for(int i = 0; i < 4; i++){
            if(i == menusUI.cursorPos){writeAt(0, i, byte(1));} 
            else {printAt(0, i, " ");}
            if((menusUI.scrollOffset + i) < menusUI.maxIndex){
              presetsUI.getFileName(menusUI.scrollOffset + i, menusUI.whatSaveSelect, midiUI.charNumber);
              printAt(1, i, midiUI.charNumber);
              if(menusUI.whatSaveSelect == 0) {lcd.print(".seq");}
              else if(menusUI.whatSaveSelect == 1) {lcd.print(".pat");}  
            } 
            else {printAt(1, i, "            ");}
          }
          break;
        // PANTALLA DE CONFIRMACION
        case 3:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreenSettings();}
          printAt(1, 2, midiUI.charNumber);
          if(!menusUI.deleteConfirm){writeAt(1,3,byte(0)), lcd.print("No"), lcd.write(byte(1)), printAt(11,3," "), lcd.print("Yes"), lcd.print(" ");}
          else if(menusUI.deleteConfirm){printAt(1,3," "), lcd.print("No"), lcd.print(" "), writeAt(11,3,byte(0)), lcd.print("Yes"), lcd.write(byte(1));}
          break;
      }
      break;
    // --- MODO MIDI CLOCK ---
    case 4:
      nScreen = 5;
      if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreenSettings();}
      lcd.noBlink();

      if(MidiProgramming::modeMidiClock == 0) {printAt(5, 1, "External"), lcd.write(byte(1));}
      else if(MidiProgramming::modeMidiClock == 1) {writeAt(4, 1, byte(0)), lcd.print("Internal");}
      break;

    // --- SCALE SELECTOR ---
    case 5:
      nScreen = 6;
      if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreenSettings();}
      lcd.noBlink();

      switch(menusUI.seleccion){
        case 1:
          writeAt(0,1,byte(0)), lcd.print(nombreEscalas[s->escalaSeleccionada]), lcd.write(byte(1));
          printAt(0, 2, " "), printAt(12, 2, charEscalasNotas[s->noteTone]), lcd.print(" ");
          break;
        case 2:
          printAt(0, 1, " "), lcd.print(nombreEscalas[s->escalaSeleccionada]), lcd.print(" ");
          writeAt(0, 2, byte(0)), printAt(12, 2, charEscalasNotas[s->noteTone]), lcd.write(byte(1));
          break;
      } 
      break;  
    // --- VALORES MIDI CC ---
    case 6:
      nScreen = 7;
      if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreenSettings();}
      lcd.noBlink();
      switch(menusUI.seleccion){
        case 1:
          writeAt(0,3,byte(1)), printAt(1, 3, s->ccNumber);
          break;
        case 2:
          writeAt(4,3,byte(1)), printAt(5, 3, s->ccNumber);
          break;
        case 3:
          writeAt(8,3,byte(1)), printAt(9, 3, s->ccNumber);
          break;
        case 4:
          writeAt(12,3,byte(1)), printAt(13, 3, s->ccNumber);
          break;
        case 5:
          writeAt(16,3,byte(1)), printAt(17, 3, s->ccNumber);
          break;
      }
      break;
  }
}

inline void DrawMenus::staticScreenSettings(){
  charactersMenu1();
  lcd.clear();
  if(nScreen == 0){
    printAt(2,0,"Save Preset");
    printAt(2,1,"Load Preset");
    printAt(2,2,"Delete Preset");
    printAt(2,3,"Clock Mode");
  }
  else if(nScreen == 1){
    printAt(2,0,"Select Scale");
    printAt(2,1,"select CCNumber");
    printAt(2,2,"Save Config");
  }
  else if(nScreen == 2){
    printAt(1, 2, "Save: ");
  }
  else if(nScreen == 3){
    if(menusUI.seleccion == 1){printAt(1, 1, "Load: ");}
    else if(menusUI.seleccion == 2){printAt(1, 1, "File Name: ");}
  }
  else if(nScreen == 4){
    if(menusUI.seleccion == 1){printAt(1, 1, "Delete: ");}
    else if(menusUI.seleccion == 2){printAt(1, 1, "File Name: ");}
    else if(menusUI.seleccion == 3){printAt(1,1, "Delete"), printAt(14,2,"?");}
  }
  else if(nScreen == 5){
    printAt(4,0,"CLOCK MODE: ");
  }
  else if(nScreen == 6){
    printAt(1,0,"Scale Type: ");
    printAt(1,2,"Tone Note: ");
  }
  else if(nScreen == 7){
    printAt(8, 0, "N"), lcd.print((char) 223), lcd.print(" SEQ");
    lcd.setCursor(0,1);
    for(int i = 0; i < N_MAX_SEQS; i++){
      lcd.print("  "), lcd.print(i), lcd.print(" ");
    }
    printAt(8, 2, "N"), lcd.print((char) 223), lcd.print(" CC");
    for(int i = 0; i < N_MAX_SEQS; i++){
      printAt((4*i)+1, 3, p->nSequence[i].ccNumber);
    }
  }
  nAnteriorScreen = nScreen;
  menusUI.menuAnterior = menusUI.menuActual;
}

void DrawMenus::drawConfirmationScreen(){
  lcd.clear();
  nScreen = 7;

  if(menusUI.nMenuSettings == 1){printAt(3, 0, "Saved");}
  else if(menusUI.nMenuSettings == 2){printAt(3, 0, "Load");}
  else if(menusUI.nMenuSettings == 3){printAt(3, 0, "Deleted");}

  printAt(3, 1, midiUI.charNumber);
  if(menusUI.whatSaveSelect == 0) {lcd.print(".seq");}
  else if(menusUI.whatSaveSelect == 1) {lcd.print(".pat");}  
  printAt(5, 2, "SUCCESFULLY");
  nAnteriorScreen = 7;
}