#include <MIDI.h>
#include <TimerOne.h>

#include "MidiProgramming.h"
#include "menus_buttons.h"
#include "midiInterface.h"
#include "draw_menus.h"
#include "midiPresets.h"
#include "globalVars.h"



#define SEQ midiProg[presetsUI.indexSequence]

// Instancias librerias
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);

unsigned long tiempoActualMillis;
unsigned long tiempoActualMicros;

volatile bool enviarPulso = false;
int tareasPesadas;

int clock;

int globalNote[N_MAX_SEQUENCES][NUM_POTES];
int globalCCValue[N_MAX_SEQUENCES][NUM_POTES];
int globalVelocity[N_MAX_SEQUENCES][NUM_POTES];
int globalOctave[N_MAX_SEQUENCES][NUM_POTES];
bool globalMutes[N_MAX_SEQUENCES][NUM_POTES];

int globalCanal[N_MAX_SEQUENCES];
int globalCCNumber[N_MAX_SEQUENCES];
int globalModeSequence[N_MAX_SEQUENCES];
int globalEscalaSeleccionada[N_MAX_SEQUENCES];
int globalNTotalSteps[N_MAX_SEQUENCES];
int globalSubdivMode[N_MAX_SEQUENCES];
int globalIndexSubdivisiones[N_MAX_SEQUENCES];
int globalIndComplexSubdivY[N_MAX_SEQUENCES];
int globalIndComplexSubdivX[N_MAX_SEQUENCES];
int globalArmed[N_MAX_SEQUENCES] = {true, false, false, false, false};




// Gestion de MIDI y tiempo musical
void variablesGlobales();

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
  for(int i = 0; i < N_MAX_SEQUENCES; i++){
    midiUI.mutePulsado[i] = false;
    globalNTotalSteps[i] = 8;
    for(int j = 0; j < NUM_POTES; j++){
      int lectura = analogRead(midiUI.potesPin[j]);
      int notaEscala = constrain(map(lectura, 1018, 30, 0, midiProg[i].nNotasEscalas[globalEscalaSeleccionada[i]] - 1), 0, midiProg[i].nNotasEscalas[globalEscalaSeleccionada[i]] - 1);
      globalNote[i][j] = midiProg[i].escalasNotas[globalEscalaSeleccionada[i]][notaEscala];
      globalVelocity[i][j] = 127;
      globalOctave[i][j] = 4;
      globalMutes[i][j] = false;
    }
    globalCanal[i] = i;
    globalSubdivMode[i] = 0;           // Modo de subdivisión por defecto
    globalIndexSubdivisiones[i] = 7;   // Empezar en la primera subdivisión (ej. negras)
    globalIndComplexSubdivY[i] = 0;    // Reset de índices complejos
    globalIndComplexSubdivX[i] = 0;
    globalNTotalSteps[i] = 8;
    globalEscalaSeleccionada[i] = 1;
    globalIndexSubdivisiones[i] = 7;
    globalSubdivMode[i] = 0;
  }

// Creación caracteres e inicio de la pantalla
  drawUI.configurarLCD();
}
void funcionInterrupcion() {
  enviarPulso = true; 
}

void loop() {
  
  if (enviarPulso) {
    enviarPulso = false; // Bajamos la bandera
    MIDI.sendRealTime(midi::Clock);
    clock++;
    MidiProgramming::tipoMsgMidi = 0xF8;
    if(clock > 23){clock = 0;}
    for(int i = 0; i < N_MAX_SEQUENCES; i++){
        midiProg[i].midiSeq();
        midiProg[i].midiNotesOn();
        midiProg[i].midiNotesOff();
    }
    return;
  }
  for(int i = 0; i < N_MAX_SEQUENCES; i++)
  if ((MidiProgramming::tipoMsgMidi == 0xFA || MidiProgramming::tipoMsgMidi == 0xFC) && MidiProgramming::modeMidiClock == 1) {
    for(int i = 0; i < N_MAX_SEQUENCES; i++){
      midiProg[i].midiSeq();
    } 
    MidiProgramming::tipoMsgMidi = 0;
  }
  tiempoActualMillis = millis();
  tiempoActualMicros = micros();
  menusUI.checkButtons();
  midiUI.checkMutes();

  if(MidiProgramming::modeMidiClock == 0 && MIDI.read()){
    if(MidiProgramming::modeMidiClock == 0){MidiProgramming::tipoMsgMidi = MIDI.getType();}  
    for(int i = 0; i < N_MAX_SEQUENCES; i++){
      midiProg[i].midiSeq();
    }
  }
  for(int i = 0; i < N_MAX_SEQUENCES; i++){
    if(globalArmed[i]){
      midiProg[i].midiNotesOn();
    }
    midiProg[i].midiNotesOff();
  }
  MidiProgramming::tipoMsgMidi = 0;


  switch(tareasPesadas){
    case 0:
      midiUI.checkPotes();
      tareasPesadas++;
      break;
    case 1:
      if (drawUI.updateLCD) {
        drawUI.update_LCD();
        drawUI.updateLCD = false;
      }
      tareasPesadas = 0;
      break;
  }


  // Reset pantalla
  if(menusUI.menuActual == 0 && menusUI.seleccion == 5){
    if(SEQ.nStep != drawUI.nStepAnterior){
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

void variablesGlobales(){
  for(int i = 0; i < N_MAX_SEQUENCES; i++){
    for(int j = 0; j < NUM_POTES; j++){
      globalNote[i][j] = NOTA(i, j);
      globalCCValue[i][j] = CCVALUE(i, j);
      globalVelocity[i][j] = VELOCITY(i, j);
      globalOctave[i][j] = OCTAVA(i, j);
      globalMutes[i][j] = MUTES(i, j);
    }
    globalCanal[i] = CANAL(i);
    globalCCNumber[i] = CCNUMBER(i);
    globalModeSequence[i] = MODE_SEQUENCE(i);
    globalEscalaSeleccionada[i] = ESCALA(i);
    globalSubdivMode[i] = SUBDIV_MODE(i);
    globalNTotalSteps[i] = NTOTALSTEPS(i);
    globalIndexSubdivisiones[i] = INDEX_SUBDIVISIONES(i);
    globalIndComplexSubdivY[i] = INDEX_COMPLEX_Y(i);
    globalIndComplexSubdivX[i] = INDEX_COMPLEX_X(i);
    globalArmed[i] = ARMED(i);
  }
}



/*
void drawMenu1() {
  if(menusUI.seleccion == 1) { 
    lcd.noBlink(), lcd.setCursor(3,0), lcd.write(byte(0)), lcd.print("SUBDIV: "), lcd.print(globalSubdivMode), lcd.write(byte(1));
    if(globalSubdivMode == 0 || globalSubdivMode == 1){
      lcd.setCursor(6,1), lcd.print(subdivisionesCharArray[menusUI.globalIndexSubdivisiones]);}
    else if(globalSubdivMode == 2){
      lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[menusUI.INDEX_COMPLEX_X]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[menusUI.INDEX_COMPLEX_Y]);
    }
  }
  else if(menusUI.seleccion == 2) { 
    lcd.noBlink(), lcd.setCursor(4,0), lcd.print("SUBDIV: "), lcd.print(globalSubdivMode);
    if(globalSubdivMode == 0 || globalSubdivMode == 1){ 
      lcd.setCursor(5,1), lcd.write(byte(0)), lcd.print(subdivisionesCharArray[menusUI.globalIndexSubdivisiones]),lcd.write(byte(1));}
    else if(globalSubdivMode == 2){
      lcd.setCursor(5,1), lcd.print(subdivisionesComplejasNumChar[menusUI.INDEX_COMPLEX_X]), lcd.print("/"), lcd.write(subdivisionesComplejasDenChar[menusUI.INDEX_COMPLEX_Y]);
      if(menusUI.selectNum){lcd.setCursor(6,1), lcd.blink();}
      else if(menusUI.selectDen){lcd.setCursor(8,1), lcd.blink();}
    }
  }
}
void drawMenu2() {
  lcd.print("BPM = "); 
}*/
