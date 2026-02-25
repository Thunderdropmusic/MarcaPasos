#include <MIDI.h>
// 1. Definir una configuración personalizada para cambiar la velocidad (BaudRate)
struct HairlessConfig : public midi::DefaultSettings {
  static const long BaudRate = 115200;
};

// 2. Crear la instancia usando esa configuración y el puerto Serial0 (USB)
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);
int tiempo;
int tipo;
int timer;
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
        MIDI.sendNoteOn(60,127,1);
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
  if (notaFuera && (millis() - timer >= 250)){
     MIDI.sendNoteOff(60,127,1);
     notaFuera = false;
     timer = 0;
  }
}
