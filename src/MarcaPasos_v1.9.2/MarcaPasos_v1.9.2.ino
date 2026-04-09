#include <MIDI.h>
#include <TimerOne.h>



#include "MidiProgramming.h"
#include "midiInterface.h"
#include "draw_menus.h"
#include "midiPresets.h"
#include "menus_buttons.h"
#include "config.h"


#define SEQ midiProg[presetsUI.indexSequence]

// Instancias librerias
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);

unsigned long tiempoActualMillis;
unsigned long tiempoActualMicros;

volatile bool enviarPulso = false;
byte tareasPesadas;
byte clock;

Sequence* s;

void syncWithActiveSequence() { // para saber la secuencia en que nos encontramos
  s = presetsUI.getActiveSequence();
}

// Gestion de MIDI y tiempo musical
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  //Serial.begin(9600);

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
  
  // Botones de secuencias
  for(int i = 0; i < 5; i++){
    pinMode(midiUI.seqPinsButton[i], INPUT_PULLUP);
  }
  
  // Botones de control
  pinMode(midiUI.playPinButton, INPUT_PULLUP);
  pinMode(midiUI.stopPinButton, INPUT_PULLUP);  

  // Botones de los slots
  for(int i = 0; i < 4; i++){
    pinMode(presetsUI.slotsPin[i], INPUT_PULLUP);
  }
  pinMode(presetsUI.saveButton, INPUT_PULLUP);

  //SD
  pinMode(53, OUTPUT);

  // Leds
  for(int i = 0; i < 5; i++){
    pinMode(midiUI.seqPinLed[i], OUTPUT);
  }

  //Botones Mutes
  for(byte i = 0; i < NUM_MUTES; i++){
    pinMode(midiUI.mutesPin[i], INPUT_PULLUP);
  }

  // Valores predeterminados
  for(byte i = 0; i < N_MAX_SEQUENCES; i++){
    midiUI.mutePulsado[i] = false;
    p->nSequence[i].nTotalSteps = 8;
    p->nSequence[i].escalaSeleccionada = 1;
    p->nSequence[i].noteTone = 0;
    for(byte j = 0; j < NUM_POTES; j++){
      int lectura = analogRead(midiUI.potesPin[j]);
      byte index = constrain(map(lectura, MAX_POTE_VALUE, MIN_POTE_VALUE, 48, 90), 48, 90);
      if(index <= 9){
        midiUI.charNumber[i] = index + 48;
      } 
      else{
        midiUI.charNumber[i] = index + 55;
      }
      byte notaEscala = constrain(map(lectura, MAX_POTE_VALUE, MIN_POTE_VALUE, 0, midiProg[i].nNotasEscalas[s->escalaSeleccionada] - 1), 0, midiProg[i].nNotasEscalas[s->escalaSeleccionada] - 1);
      p->nSequence[i].steps[j].note = midiProg[i].escalasNotas[s->escalaSeleccionada][notaEscala];
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
    p->nSequence[i].nTotalSteps = 8;
    p->nSequence[i].subdivMode = 0;
    p->nSequence[i].ccNumber = 48 + i;
    p->nSequence[i].seqMode = 0;
    p->nSequence[i].armed = false;
    p->nSequence[i].steps[0].ccMutes = false;
    p->nSequence[i].steps[4].ccMutes = false;
  }
  p->nSequence[0].armed = true;

  // Creación caracteres e inicio de la pantalla
  drawUI.configurarLCD();
  presetsUI.sdInit();
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
    for(byte i = 0; i < N_MAX_SEQUENCES; i++){
        midiProg[i].midiSeq();
        midiProg[i].midiNotesOn();
        midiProg[i].midiNotesOff();
    }
    MidiProgramming::tipoMsgMidi = 0;
    return;
  }
  if ((MidiProgramming::tipoMsgMidi == 0xFA || MidiProgramming::tipoMsgMidi == 0xFC) && MidiProgramming::modeMidiClock == 1 && !midiUI.deviceStop) {
    for(byte i = 0; i < N_MAX_SEQUENCES; i++){
      midiProg[i].midiSeq();
    } 
    MidiProgramming::tipoMsgMidi = 0;
  }

  tiempoActualMillis = millis();
  tiempoActualMicros = micros();

  menusUI.checkButtons();
  midiUI.checkMutes();

  // Modo externo del clock
  if(MidiProgramming::modeMidiClock == 0){
    if(MIDI.read()){
      MidiProgramming::tipoMsgMidi = MIDI.getType();
      for(byte i = 0; i < N_MAX_SEQUENCES; i++){
        midiProg[i].midiSeq();
      }
    }
    for(byte i = 0; i < N_MAX_SEQUENCES; i++){
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
      presetsUI.readPresetsButtons();
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
    drawUI.lcd.clear();
    drawUI.updateLCD = true;
  }
  else if ((menusUI.menuActual == 4 || menusUI.menuActual == 5) && (tiempoActualMillis - midiUI.timeShowOctValue >= 1500)) {
    menusUI.menuActual = menusUI.menuAnterior;
    drawUI.lcd.clear();
    drawUI.updateLCD = true;
  }
  else if ((menusUI.menuActual == 6 || menusUI.menuActual == 7) && tiempoActualMillis - presetsUI.ultimoTiempoBotonPresets >= 1500){
    menusUI.menuActual = menusUI.menuAnterior;
    menusUI.nMenuSettings = 0;
    drawUI.lcd.clear();
    drawUI.updateLCD = true;
  }
}
