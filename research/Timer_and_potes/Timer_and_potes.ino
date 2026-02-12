#include <MIDI.h>
// 1. Definir una configuración personalizada para cambiar la velocidad (BaudRate)
struct HairlessConfig : public midi::DefaultSettings {
  static const long BaudRate = 115200;
};

// 2. Crear la instancia usando esa configuración y el puerto Serial0 (USB)
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);
int tiempo;
int tipo;
unsigned long timer;
int note;
int decay = 250;
const int analogPin = A0;
int value;
int posicion1;
int ultimoValorPote;
int notaTocada;
volatile boolean notaFuera;
void setup() {
  // MIDI.begin ya inicializa el puerto Serial a 115200 gracias a HairlessConfig
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
}
void loop() { 
  if (MIDI.read()){
    tipo = MIDI.getType();
    if(tipo == 0xFA){ //Start
      notaFuera = false;
      boolean play = true;
      tiempo = 0;
    }
    else if(tipo == 0xF8){
      if (tiempo == 0){ 
        notaFuera = true;
        MIDI.sendNoteOn(note,127,1);
        notaTocada = note;
        timer = millis();
      }
      tiempo++;
      if (tiempo == 24){
        tiempo = 0;        
      }
    }
    else if(tipo == 0xFC){ //Stop
      boolean play = false;
    }      
  }
  if (notaFuera && (millis() - timer >= (unsigned long)decay)){
     /*
     for(int i = 0; i < 128; i++){
      MIDI.sendNoteOff(i,127,1); //apaga todas las notas a la vez
     }
     */
     MIDI.sendNoteOff(notaTocada,127,1);
     notaFuera = false;
     timer = 0;
  }
  value = analogRead(analogPin); 
  if(abs(value - ultimoValorPote) > 5){
    value = map(value, 0, 1000, 72, 60); //el pote controla la nota del midi
    note = value;
  }
  
  ultimoValorPote = value;
}
