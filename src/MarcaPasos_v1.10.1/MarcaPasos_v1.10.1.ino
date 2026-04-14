#include <MIDI.h>
#include <TimerOne.h>

#include "MidiProgramming.h"
#include "midiInterface.h"
#include "draw_menus.h"
#include "midiPresets.h"
#include "menus_buttons.h"
#include "config.h"

#define SEQ midiProg[presetsUI.indexSequence]

// ==============================================================================
//                INSTACIAS DE LAS LIBRERIAS Y VARIABLES GLOBALES
// ==============================================================================
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);

unsigned long tiempoActualMillis;
unsigned long tiempoActualMicros;

volatile bool enviarPulso = false;
byte tareasPesadas;
byte clock;

Sequence* s;

void syncWithActiveSequence() {
  s = presetsUI.getActiveSequence();
}

void initValuesSeq(){
  for(byte i = 0; i < N_MAX_SEQS; i++){
    p->nSequence[i].escalaSeleccionada = 1;
    p->nSequence[i].noteTone = 0;
    midiUI.mutePulsado[i] = false;
    for(byte j = 0; j < NUM_POTES; j++){
      int lectura = analogRead(midiUI.potesPin[j]);
      byte index = constrain(map(lectura, MAX_POTE_VALUE, MIN_POTE_VALUE, 48, 90), 48, 90);

      midiUI.charNumber[i] = (index <= 9) ? (index + '0') : (index -10 + 'A');

      byte notaEscala = constrain(map(lectura, MAX_POTE_VALUE, MIN_POTE_VALUE, 0, midiProg[i].nNotasEscalas[p->nSequence[i].escalaSeleccionada] - 1), 0, midiProg[i].nNotasEscalas[p->nSequence[i].escalaSeleccionada] - 1);
      p->nSequence[i].steps[j].note = midiProg[i].escalasNotas[p->nSequence[i].escalaSeleccionada][notaEscala];
      int valorCC = constrain(map(lectura, MAX_POTE_VALUE, 30, 0, 127), 0, 127);
      p->nSequence[i].steps[j].ccValue = valorCC;
      p->nSequence[i].steps[j].velocity = 127;
      p->nSequence[i].steps[j].ccSmoothCurve = 6;
      p->nSequence[i].steps[j].octave = 4;
      p->nSequence[i].steps[j].mutes = false;
      p->nSequence[i].steps[j].ccMutes = true;
    }
    p->nSequence[i].canal = i;
    p->nSequence[i].subdivMode = 0;
    p->nSequence[i].indexSubdivisiones = 6;   // Pred. Negras
    p->nSequence[i].indComplexSubdivY = 0;  
    p->nSequence[i].indComplexSubdivX = 3;
    p->nSequence[i].nTotalSteps = N_MAX_STEPS;
    p->nSequence[i].ccNumber = 48 + i;
    p->nSequence[i].seqMode = 0;
    p->nSequence[i].armed = false;
    p->nSequence[i].steps[0].ccMutes = false;
    p->nSequence[i].steps[4].ccMutes = false;
  }
  p->nSequence[0].armed = true;
  for(byte i = 0; i < N_MAX_SEQS; i++){
    p->seq_extension[i].escalaSeleccionada = 1;
    p->seq_extension[i].noteTone = 0;
    midiUI.mutePulsado[i] = false;
    for(byte j = 0; j < NUM_POTES; j++){
      int lectura = analogRead(midiUI.potesPin[j]);
      byte index = constrain(map(lectura, MAX_POTE_VALUE, MIN_POTE_VALUE, 48, 90), 48, 90);

      midiUI.charNumber[i] = (index <= 9) ? (index + '0') : (index -10 + 'A');

      byte notaEscala = constrain(map(lectura, MAX_POTE_VALUE, MIN_POTE_VALUE, 0, midiProg[i].nNotasEscalas[p->seq_extension[i].escalaSeleccionada] - 1), 0, midiProg[i].nNotasEscalas[p->seq_extension[i].escalaSeleccionada] - 1);
      p->seq_extension[i].steps[j].note = midiProg[i].escalasNotas[p->seq_extension[i].escalaSeleccionada][notaEscala];
      int valorCC = constrain(map(lectura, MAX_POTE_VALUE, 30, 0, 127), 0, 127);
      p->seq_extension[i].steps[j].ccValue = valorCC;
      p->seq_extension[i].steps[j].velocity = 127;
      p->seq_extension[i].steps[j].ccSmoothCurve = 6;
      p->seq_extension[i].steps[j].octave = 4;
      p->seq_extension[i].steps[j].mutes = false;
      p->seq_extension[i].steps[j].ccMutes = true;
    }
    p->seq_extension[i].canal = i;
    p->seq_extension[i].subdivMode = 0;
    p->seq_extension[i].indexSubdivisiones = 6;   // Pred. Negras
    p->seq_extension[i].indComplexSubdivY = 0;  
    p->seq_extension[i].indComplexSubdivX = 3;
    p->seq_extension[i].nTotalSteps = N_MAX_STEPS;
    p->seq_extension[i].ccNumber = 48 + i;
    p->seq_extension[i].seqMode = 0;
    p->seq_extension[i].armed = false;
    p->seq_extension[i].steps[0].ccMutes = false;
    p->seq_extension[i].steps[4].ccMutes = false;
  }
  p->nSequence[0].armed = true;
}



// Gestion de MIDI y tiempo musical
void setup() {
  drawUI.configurarLCD();
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  //Serial.begin(9600);
  drawUI.drawLoadProgress(1); // 1
  //Botones Menus
  pinMode(menusUI.pinButton1, INPUT_PULLUP);
  pinMode(menusUI.pinButton2, INPUT_PULLUP);
  pinMode(menusUI.pinButton3, INPUT_PULLUP);
  pinMode(menusUI.pinButton4, INPUT_PULLUP);
  pinMode(menusUI.pinButton5, INPUT_PULLUP);
  pinMode(menusUI.settingsPin, INPUT_PULLUP);

  // Botones octavas
  pinMode(midiUI.pinOctPlus, INPUT_PULLUP);
  pinMode(midiUI.pinOctRest, INPUT_PULLUP);
  
  
  drawUI.drawLoadProgress(2); // 3
  // Botones de secuencias
  for(int i = 0; i < N_MAX_SEQS; i++){
    pinMode(midiUI.seqPinsButton[i], INPUT_PULLUP);
  }

  // Botones de control
  pinMode(midiUI.playPinButton, INPUT_PULLUP);
  pinMode(midiUI.stopPinButton, INPUT_PULLUP);  

  // Botones de los slots
  for(int i = 0; i < 3; i++){
    pinMode(presetsUI.slotsPin[i], INPUT_PULLUP);
  }
  pinMode(presetsUI.saveButton, INPUT_PULLUP);
  pinMode(menusUI.extPin, INPUT_PULLUP);

  //SD
  pinMode(53, OUTPUT);
  // Leds
  for(int i = 0; i < N_MAX_SEQS; i++){
    pinMode(midiUI.seqPinLed[i], OUTPUT);
  }
  drawUI.drawLoadProgress(3); // 6

  //Botones Mutes
  for(byte i = 0; i < NUM_MUTES; i++){
    pinMode(midiUI.mutesPin[i], INPUT_PULLUP);
  }
  drawUI.drawLoadProgress(4); // 10

  // Valores predeterminados
  initValuesSeq();
  drawUI.drawLoadProgress(5); // 15
  // Creación caracteres e inicio de la pantalla
  presetsUI.sdInit();
  delay(2000);
}


void funcionInterrupcion() { // Flag para el modo interno del clock
  enviarPulso = true; 
}



void loop() {
  // Funcion para el modo interno del clock
  if (enviarPulso) {
    enviarPulso = false; // Bajamos la bandera
    MIDI.sendRealTime(midi::Clock);
    clock++;
    MidiProgramming::tipoMsgMidi = 0xF8;
    if(clock > 23){clock = 0;}
    for(byte i = 0; i < N_MAX_SEQS; i++){
        midiProg[i].midiSeq();
        midiProg[i].midiNotesOn();
        midiProg[i].midiNotesOff();
    }
    MidiProgramming::tipoMsgMidi = 0;
    return;
  }
  if ((MidiProgramming::tipoMsgMidi == 0xFA || MidiProgramming::tipoMsgMidi == 0xFC) && MidiProgramming::modeMidiClock == 1 && !midiUI.deviceStop) {
    for(byte i = 0; i < N_MAX_SEQS; i++){
      midiProg[i].midiSeq();
    } 
    MidiProgramming::tipoMsgMidi = 0;
  }

  tiempoActualMillis = millis();
  tiempoActualMicros = micros();

  menusUI.checkMenuButtons();
  midiUI.checkMidiButtons();

  // Modo externo del clock
  if(MidiProgramming::modeMidiClock == 0){
    if(MIDI.read()){
      MidiProgramming::tipoMsgMidi = MIDI.getType();
      for(byte i = 0; i < N_MAX_SEQS; i++){
        midiProg[i].midiSeq();
      }
    }
    for(byte i = 0; i < N_MAX_SEQS; i++){
      if(p->nSequence[i].armed && midiUI.devicePlay){
        if(p->nSequence[i].seqMode == 0){
          midiProg[i].midiNotesOn();
        }
        else if(p->nSequence[i].seqMode == 1){
          midiProg[i].CCSend();
        }
      }
      midiProg[i].midiNotesOff();
    }
    MidiProgramming::tipoMsgMidi = 0;
  }

  // Ahorro del buffer
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
      tareasPesadas++;
      break;
    case 2:
      presetsUI.readSlotsButtons();
      tareasPesadas = 0;
      break;
  }


  // Reset pantalla
  if(menusUI.menuActual == 0){
    if(SEQ.nStep != drawUI.nStepAnterior){
      drawUI.updateLCD = true;
    }
  } 


  // Clock para salir de la pantalla de potenciómetro
  if (menusUI.menuActual == 3 && (tiempoActualMillis - midiUI.timeShowPotValue >= 1500)) {
    menusUI.menuActual = menusUI.menuAnterior;
    midiUI.poteBlocked[midiUI.movedPoteNumber] = true;
    drawUI.updateLCD = true;
  }
  else if ((menusUI.menuActual == 4 || menusUI.menuActual == 5) && (tiempoActualMillis - midiUI.ultimoTiempoBotonOct >= 1500)) {
    menusUI.menuActual = menusUI.menuAnterior;
    drawUI.updateLCD = true;
  }
  else if ((menusUI.menuActual == 6 || menusUI.menuActual == 7) && tiempoActualMillis - presetsUI.ultimoTiempoBotonPresets >= 1500){
    menusUI.menuActual = menusUI.menuAnterior;
    menusUI.nMenuSettings = 0;
    drawUI.updateLCD = true;
  }
}
