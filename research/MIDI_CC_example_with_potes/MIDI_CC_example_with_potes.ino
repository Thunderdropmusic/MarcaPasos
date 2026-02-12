ar#include <MIDI.h>

const int analogPin = A0;
int value;
int posicion1;
int posicion2;
// 1. Definir una configuración personalizada para cambiar la velocidad (BaudRate)
struct HairlessConfig : public midi::DefaultSettings {
  static const long BaudRate = 115200;
};

// 2. Crear la instancia usando esa configuración y el puerto Serial0 (USB)
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);

void setup() {
  // MIDI.begin ya inicializa el puerto Serial a 115200 gracias a HairlessConfig
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void loop() {
  MIDI.read();
  value = analogRead(analogPin);
  posicion1 = map(value, 0, 1020, 127, 0);
  if(posicion1 != posicion2){
    MIDI.sendControlChange(40, posicion1, 1);
  }
  
  posicion2 = map(value, 0, 1020, 127, 0);
}
