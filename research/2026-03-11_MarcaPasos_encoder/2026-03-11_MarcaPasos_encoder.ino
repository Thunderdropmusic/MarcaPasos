#include <MIDI.h>
#include <TimerOne.h>

#include "MidiProgramming.h"
#include "midiInterface.h"
#include "draw_menus.h"
#include "midiPresets.h"
#include "menus_buttons.h"
#include "globalVars.h"


#define SEQ midiProg[presetsUI.indexSequence]

// Instancias librerias
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);

unsigned long tiempoActualMillis;
unsigned long tiempoActualMicros;

volatile bool enviarPulso = false;
byte tareasPesadas;

byte clock;

byte globalNote[N_MAX_SEQUENCES][NUM_POTES];
byte globalCCValue[N_MAX_SEQUENCES][NUM_POTES];
byte globalVelocity[N_MAX_SEQUENCES][NUM_POTES];
byte globalOctave[N_MAX_SEQUENCES][NUM_POTES];
bool globalMutes[N_MAX_SEQUENCES][NUM_POTES];

byte globalCanal[N_MAX_SEQUENCES];
byte globalCCNumber[N_MAX_SEQUENCES] = {48, 49, 50, 51, 52};
byte globalModeSequence[N_MAX_SEQUENCES];
byte globalEscalaSeleccionada[N_MAX_SEQUENCES];
byte globalNTotalSteps[N_MAX_SEQUENCES];
byte globalSubdivMode[N_MAX_SEQUENCES];
byte globalIndexSubdivisiones[N_MAX_SEQUENCES];
byte globalIndComplexSubdivY[N_MAX_SEQUENCES];
byte globalIndComplexSubdivX[N_MAX_SEQUENCES];
bool globalArmed[N_MAX_SEQUENCES] = {true, false, false, false, false};




// Gestion de MIDI y tiempo musical
void variablesGlobales();

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  //Botones Menus
  pinMode(menusUI.pinButton1, INPUT);
  pinMode(menusUI.pinButton2, INPUT_PULLUP);
  pinMode(menusUI.pinButton3, INPUT);
  pinMode(menusUI.pinButton4, INPUT);

  pinMode(midiUI.pinOctPlus, INPUT);
  pinMode(midiUI.pinOctRest, INPUT);

  //Botones Mutes
  /*
  for(byte i = 0; i < NUM_MUTES; i++){
    pinMode(midiUI.mutesPin[i], INPUT);
  }
  */
  for(byte i = 0; i < NUM_MUTES; i++){
    pinMode(midiUI.mutesPin[i], INPUT);
  }

  // Primera lectura potes
  for(byte i = 0; i < N_MAX_SEQUENCES; i++){
    midiUI.mutePulsado[i] = false;
    globalNTotalSteps[i] = 8;
    for(byte j = 0; j < NUM_POTES; j++){
      int lectura = analogRead(midiUI.potesPin[j]);
      int notaEscala = constrain(map(lectura, 1018, 30, 0, midiProg[i].nNotasEscalas[globalEscalaSeleccionada[i]] - 1), 0, midiProg[i].nNotasEscalas[globalEscalaSeleccionada[i]] - 1);
      globalNote[i][j] = midiProg[i].escalasNotas[globalEscalaSeleccionada[i]][notaEscala];
      globalVelocity[i][j] = 127;
      globalOctave[i][j] = 4;
      globalMutes[i][j] = false;
      int valorCC = constrain(map(lectura, 1018, 30, 0, 127), 0, 127);
      globalCCValue[presetsUI.indexSequence][i] = valorCC;
    }
    globalCanal[i] = i;
    globalSubdivMode[i] = 0;           // Modo de subdivisión por defecto
    globalIndexSubdivisiones[i] = 6;   // Empezar en la primera subdivisión (ej. negras)
    globalIndComplexSubdivY[i] = 0;    // Reset de índices complejos
    globalIndComplexSubdivX[i] = 0;
    globalNTotalSteps[i] = 8;
    globalEscalaSeleccionada[i] = 1;
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
    for(byte i = 0; i < N_MAX_SEQUENCES; i++){
        midiProg[i].midiSeq();
        midiProg[i].midiNotesOn();
        midiProg[i].midiNotesOff();
    }
    MidiProgramming::tipoMsgMidi = 0;
    return;
  }
  for(byte i = 0; i < N_MAX_SEQUENCES; i++)
  if ((MidiProgramming::tipoMsgMidi == 0xFA || MidiProgramming::tipoMsgMidi == 0xFC) && MidiProgramming::modeMidiClock == 1) {
    for(byte i = 0; i < N_MAX_SEQUENCES; i++){
      midiProg[i].midiSeq();
    } 
    MidiProgramming::tipoMsgMidi = 0;
  }
  tiempoActualMillis = millis();
  tiempoActualMicros = micros();
  menusUI.checkButtons();
  midiUI.checkMutes();
  if(MidiProgramming::modeMidiClock == 0){
    if(MIDI.read()){
      if(MidiProgramming::modeMidiClock == 0){MidiProgramming::tipoMsgMidi = MIDI.getType();}  
      for(byte i = 0; i < N_MAX_SEQUENCES; i++){
        midiProg[i].midiSeq();
      }
    }
    for(byte i = 0; i < N_MAX_SEQUENCES; i++){
      if(globalArmed[i]){
        if(globalModeSequence[i] == 0){
          midiProg[i].midiNotesOn();
        }
        else if(globalModeSequence[i] == 1){
          midiProg[i].CCSend();
        }
      }
      midiProg[i].midiNotesOff();
    }
    MidiProgramming::tipoMsgMidi = 0;
  }


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
  for(byte i = 0; i < N_MAX_SEQUENCES; i++){
    for(byte j = 0; j < NUM_POTES; j++){
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
void drawMenu2() {
  lcd.print("BPM = "); 
}
*/
