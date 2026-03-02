#include <MIDI.h>
#include <TimerOne.h>

#include "MidiProgramming.h"
#include "menus_buttons.h"
#include "midiInterface.h"
#include "draw_menus.h"
#include "midiPresets.h"

// Instancias librerias
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);

unsigned long tiempoActualMillis;
unsigned long tiempoActualMicros;

volatile bool enviarPulso = false;
int tareasPesadas;


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
    NOTA(i) = constrain(map(lectura, 1018, 8, 48, 60), 48, 60);
    VELOCITY(i) = constrain(map(lectura, 1018, 8, 0, 127), 0, 127);;
  }
// Creación caracteres e inicio de la pantalla
  drawUI.configurarLCD();
}
void funcionInterrupcion() {
  enviarPulso = true; 
}

void loop() {
  if (enviarPulso && midiProg.modeMidiClock == 1) {
    enviarPulso = false; // Bajamos la bandera
    MIDI.sendRealTime(midi::Clock);
    midiProg.clock++;
    midiProg.tipoMsgMidi = 0xF8;
    if(midiProg.clock > 23){midiProg.clock = 0;}
    midiProg.midiSeq();
    midiProg.midiNotesOn();
    midiProg.midiNotesOff();
    return;
  }
  if ((midiProg.tipoMsgMidi == 0xFA || midiProg.tipoMsgMidi == 0xFC) && midiProg.modeMidiClock == 1) {
      midiProg.midiSeq(); 
      midiProg.tipoMsgMidi = 0;
  }
  tiempoActualMillis = millis();
  tiempoActualMicros = micros();
  menusUI.checkButtons();
  midiUI.checkMutes();
  if(midiProg.modeMidiClock == 0 && MIDI.read()){
    for(CANAL() = 1, presetsUI.indexSequence = 0; presetsUI.indexSequence < 5; presetsUI.indexSequence++, CANAL()++){
      midiProg.midiSeq();
      midiProg.midiNotesOn();
      midiProg.midiNotesOff();
      midiProg.tipoMsgMidi = 0;
    }
  }


  switch(tareasPesadas){
    case 0:
      midiUI.checkPotes();
      tareasPesadas++;
      break;
    case 1:
      if (drawUI.updateLCD) {
        drawUI.update_LCD();
      }
      tareasPesadas = 0;
      break;
  }


  // Reset pantalla
  if(menusUI.menuActual == 0 && menusUI.seleccion == 5){
    if(midiProg.nStep != midiProg.nStepAnterior){
      drawUI.updateLCD = true;
    }
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
    lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.print("SUBDIV: "), lcd.print(SUBDIV_MODE()), lcd.write(byte(1));
    if(SUBDIV_MODE() == 0 || SUBDIV_MODE() == 1){
      lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[menusUI.INDEX_SUBDIVISIONES()]);}
    else if(SUBDIV_MODE() == 2){
      lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[menusUI.INDEX_COMPLEX_X]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[menusUI.INDEX_COMPLEX_Y]);
    }
  }
  else if(menusUI.seleccion == 2) { 
    lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(SUBDIV_MODE());
    if(SUBDIV_MODE() == 0 || SUBDIV_MODE() == 1){ 
      lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[menusUI.INDEX_SUBDIVISIONES()]),lcd.write(byte(1));}
    else if(SUBDIV_MODE() == 2){
      lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[menusUI.INDEX_COMPLEX_X]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[menusUI.INDEX_COMPLEX_Y]);
      if(menusUI.selectNum){lcd.setCursor(6,1), lcd.blink();}
      else if(menusUI.selectDen){lcd.setCursor(8,1), lcd.blink();}
    }
  }
}
void drawMenu2() {
  lcd.print("BPM = "); 
}*/
