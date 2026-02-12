#include <MIDI.h>
#include <TimerOne.h>
// 1. Definir una configuración personalizada para cambiar la velocidad (BaudRate)
struct HairlessConfig : public midi::DefaultSettings {
  static const long BaudRate = 115200;
};

// 2. Crear la instancia usando esa configuración y el puerto Serial0 (USB)
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);
int tiempo;
volatile boolean notaFuera;
void setup() {
  // MIDI.begin ya inicializa el puerto Serial a 115200 gracias a HairlessConfig
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
}
void loop() { 
  if (MIDI.read()){
    int tipo = MIDI.getType();
    if (tipo == 0x90){ //tipo nota
      int nota = MIDI.getData1();
      if(nota == 0x30){ // 48
      MIDI.sendNoteOn(60,127,1);        
      }
    }
    else if(tipo == 0xB0){ //Control Change
      MIDI.sendControlChange(80,100,1);
    }
    else if(tipo == 0xFA){ //Start
      notaFuera = false;
      boolean play = true;
      tiempo = 0;
      Timer1.restart();
    }
    else if(tipo == 0xF8){
      if (tiempo == 0){ 
        notaFuera = false;
        MIDI.sendNoteOn(60,127,1); 
        Timer1.restart();
      }
      tiempo++;
      if (tiempo == 24){
        tiempo = 0;        
      }
    }
    else if(tipo == 0xFC){ //Start
      boolean play = false;
    }      
  }
}
