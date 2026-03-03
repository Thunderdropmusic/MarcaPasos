#include <MIDI.h>
#include "MidiProgramming.h"
#include "menus_buttons.h"
#include "midiInterface.h"
#include "draw_menus.h"

// Instancias librerias
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);

unsigned long tiempoActualMillis;
unsigned long tiempoActualMicros;


// Gestion de MIDI y tiempo musical


void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  //Botones Menus
  pinMode(menusUI.pinButton1, INPUT);
  pinMode(menusUI.pinButton2, INPUT);
  pinMode(menusUI.pinButton3, INPUT);
  pinMode(menusUI.pinButton4, INPUT);
  pinMode(menusUI.pinButton5, INPUT);
  pinMode(menusUI.pinButton6, INPUT);

  pinMode(midiUI.pinOctPlus, INPUT);
  pinMode(midiUI.pinOctRest, INPUT);

  //Botones Mutes
  /*
  for(int i = 0; i < NUM_MUTES; i++){
    pinMode(midiUI.mutesPin[i], INPUT);
  }
  */
  for(int i = 0; i < NUM_MUTES; i++){
    pinMode(midiUI.mutesPin[i], INPUT);
  }

  // Primera lectura potes
  for(int i = 0; i < NUM_POTES; i++){
    int lectura = analogRead(midiUI.potesPin[i]);
    midiUI.potSaveValue[i] = constrain(map(lectura, 1018, 8, 48, 60), 0, 127);
  }
  for (int i = 0; i < NUM_POTES; i++) {
    int val = constrain(map(analogRead(midiUI.potesPin[i]), 1018, 30, 0, 127), 0, 127);
    midiUI.velocityValue[i] = val;
  }
// Creación caracteres e inicio de la pantalla
  drawUI.configurarLCD();
}

void loop() {
  tiempoActualMillis = millis();
  tiempoActualMicros = micros();
  menusUI.checkButtons();
  midiUI.checkPotes();
  midiUI.checkMutes();

  if (MIDI.read()){
    midiProg.midiClock();
  }
  midiProg.midiNotesOn();
  midiProg.midiNotesOff();


  // Reset pantalla
  if(menusUI.menuActual == 0 && menusUI.seleccion == 5){
    if(midiProg.nStep != midiProg.nStepAnterior){
      drawUI.updateLCD = true;
    }
  }
  if (drawUI.updateLCD) {
    drawUI.update_LCD();
  }

  // Clock para salir de la pantalla de potenciómetro
  if (menusUI.menuActual == 3 && (tiempoActualMillis - midiUI.timeShowPotValue >= 1500)) {
    menusUI.menuActual = menusUI.menuAnterior;
    drawUI.lcd.clear();
    drawUI.updateLCD = true;
  }
  else if ((menusUI.menuActual == 4 || menusUI.menuActual == 5) && (tiempoActualMillis - midiUI.timeShowOctValue >= 1500)) {
    menusUI.menuActual = menusUI.menuAnterior;
    drawUI.lcd.clear();
    drawUI.updateLCD = true;
  }
}



/*
void drawMenu1() {
  if(menusUI.seleccion == 1) { 
    lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.print("SUBDIV: "), lcd.print(menusUI.subDivMode), lcd.write(byte(1));
    if(menusUI.subDivMode == 0 || menusUI.subDivMode == 1){
      lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[menusUI.indiceSubdivisiones]);}
    else if(menusUI.subDivMode == 2){
      lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[menusUI.indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[menusUI.indComplexSubdivY]);
    }
  }
  else if(menusUI.seleccion == 2) { 
    lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(menusUI.subDivMode);
    if(menusUI.subDivMode == 0 || menusUI.subDivMode == 1){ 
      lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[menusUI.indiceSubdivisiones]),lcd.write(byte(1));}
    else if(menusUI.subDivMode == 2){
      lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[menusUI.indComplexSubdivX]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[menusUI.indComplexSubdivY]);
      if(menusUI.selectNum){lcd.setCursor(6,1), lcd.blink();}
      else if(menusUI.selectDen){lcd.setCursor(8,1), lcd.blink();}
    }
  }
}
void drawMenu2() {
  lcd.print("BPM = "); 
}*/
