#include "draw_menus.h"
#include "midiProgramming.h"
#include "midiInterface.h"
#include "menus_buttons.h"
#include "midiPresets.h"
#include "config.h"

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>




inline void DrawMenus::staticScreen3(){
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
    printAt(2,1,"selectCCNumber");
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
    drawClockModemenu();
  }
  nAnteriorScreen = nScreen;
  menusUI.menuAnterior = menusUI.menuActual;
}

void DrawMenus::drawMenuSettings(){
  switch(menusUI.nMenuSettings){
    // Menu settings Principal    
    case 0: 
      if(menusUI.seleccion == 1 || menusUI.seleccion == 2 || menusUI.seleccion == 3 || menusUI.seleccion == 4){nScreen = 0;}
      else if(menusUI.seleccion == 5 || menusUI.seleccion == 6 || menusUI.seleccion == 7 || menusUI.seleccion == 8){nScreen = 1;}      
      switch(menusUI.seleccion){
        case 1:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          writeAt(1, 0, byte(1));
          printAt(1, 1, " ");
          break;
        case 2:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          printAt(1, 0, " ");
          writeAt(1, 1, byte(1));
          printAt(1, 2, " ");
          break;
        case 3:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          printAt(1, 1, " ");
          writeAt(1, 2, byte(1));
          printAt(1, 3, " ");
          break;
        case 4:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          printAt(1, 2, " ");
          writeAt(1, 3, byte(1));
          break;
        case 5:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          writeAt(1, 0, byte(1));
          printAt(1, 1, " ");
          break;
        case 6:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          printAt(1, 0, " ");
          writeAt(1, 1, byte(1));
          printAt(1, 2, " ");
          break;
        case 7:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          printAt(1, 1, " ");
          writeAt(1, 2, byte(1));
          printAt(1, 3, " ");
          break;
        case 8:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          printAt(1, 2, " ");
          writeAt(1, 3, byte(1));
          break;
      }
      break;
    // Save Preset
    case 1:
      nScreen = 2;
      switch(menusUI.seleccion){
        case 1:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          lcd.noBlink();
          if(menusUI.whatSaveSelect == 0) {printAt(5, 1, "Sequence"), lcd.write(byte(1)), printAt(9,2,".seq");}
          else if(menusUI.whatSaveSelect == 1) {writeAt(4, 1, byte(0)), lcd.print("Pattern"), printAt(9,2,".pat");}
          printAt(1,2,midiUI.charNumber);
          break; 

        case 2:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          lcd.noBlink();
          if(menusUI.whatSaveSelect == 0) {printAt(5, 1, "Sequence"), lcd.print(" "), printAt(9,2,".seq");}
          else if(menusUI.whatSaveSelect == 1) {printAt(4, 1, " "), lcd.print("Pattern"),printAt(9,2,".pat");}
          writeAt(0, 2, byte(0)), printAt(1,2,midiUI.charNumber), writeAt(13, 2, byte(0));
          break; 
      }
      break;
    // Load Preset
    case 2:
      nScreen = 3;
      switch(menusUI.seleccion){
        case 1:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          lcd.noBlink();
          if(menusUI.whatSaveSelect == 0) {printAt(8, 1, "Sequence"), lcd.write(byte(1));}
          else if(menusUI.whatSaveSelect == 1) {writeAt(7, 1, byte(0)), lcd.print("Pattern");}
          break; 
          
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
    // Delete Preset
    case 3:
      nScreen = 4;
      switch(menusUI.seleccion){
        case 1:
          // --- PANTALLA DE SELECCIÓN DE TIPO (Sequence o Pattern) ---
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          lcd.noBlink();
          if(menusUI.whatSaveSelect == 0) {printAt(9, 1, "Sequence"), lcd.write(byte(1));}
          else if(menusUI.whatSaveSelect == 1) {writeAt(8, 1, byte(0)), lcd.print("Pattern");}
          break; 
          
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
        case 3:
          if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
          printAt(1, 2, midiUI.charNumber);
          if(!menusUI.deleteConfirm){writeAt(1,3,byte(0)), lcd.print("No"), lcd.write(byte(1)), printAt(11,3," "), lcd.print("Yes"), lcd.print(" ");}
          else if(menusUI.deleteConfirm){printAt(1,3," "), lcd.print("No"), lcd.print(" "), writeAt(11,3,byte(0)), lcd.print("Yes"), lcd.write(byte(1));}
          break;
      }
      break;
    // Mode Midi Clock
    case 4:
      nScreen = 5;
      if(menusUI.menuAnterior != menusUI.menuActual || nScreen != nAnteriorScreen || updateValues){staticScreen3();}
      lcd.noBlink();

      if(MidiProgramming::modeMidiClock == 0) {printAt(5, 1, "External"), lcd.write(byte(1));}
      else if(MidiProgramming::modeMidiClock == 1) {writeAt(4, 1, byte(0)), lcd.print("Internal");}
      break;
  }
}

inline void DrawMenus::drawClockModemenu(){
  lcd.clear();
  lcd.noBlink(), printAt(4,0,"CLOCK MODE: ");
}

void DrawMenus::drawConfirmationScreen(){
  lcd.clear();
  nScreen = 7;
  switch(menusUI.nMenuSettings){
    case 1:
      printAt(3, 0, "Saved");
      printAt(3, 1, midiUI.charNumber);
      if(menusUI.whatSaveSelect == 0) {lcd.print(".seq");}
      else if(menusUI.whatSaveSelect == 1) {lcd.print(".pat");}  
      printAt(5, 2, "SUCCESFULLY");
      nAnteriorScreen = 7;
      break;
    case 2:
      printAt(3, 0, "Load");
      printAt(3, 1, midiUI.charNumber);
      if(menusUI.whatSaveSelect == 0) {lcd.print(".seq");}
      else if(menusUI.whatSaveSelect == 1) {lcd.print(".pat");}  
      printAt(5, 2, "SUCCESFULLY");
      nAnteriorScreen = 7;
      break;
    case 3:
      printAt(3, 0, "Deleted");
      printAt(3, 1, midiUI.charNumber);
      if(menusUI.whatSaveSelect == 0) {lcd.print(".seq");}
      else if(menusUI.whatSaveSelect == 1) {lcd.print(".pat");}  
      printAt(5, 2, "SUCCESFULLY");
      nAnteriorScreen = 7;
      break;
  }
}