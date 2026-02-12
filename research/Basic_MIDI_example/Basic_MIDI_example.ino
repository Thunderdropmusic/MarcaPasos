#include <MIDI.h>

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
  MIDI.sendNoteOn(60, 100, 1);   // Nota Do (C4)
  delay(250);
  MIDI.sendNoteOff(60, 0, 1);    // Apagar Do (C4)
  delay(250);
}
