#include <MIDI.h>
int clock;
int tempo;
unsigned long tempoMicros;
unsigned long tiempoActualMicros;
unsigned long tiempoActualMillis;
unsigned long tiempoUltimoClock;
int tipoMsgMidi;
int release = 250;
unsigned long tiempoRelease;
bool notaFuera;



struct HairlessConfig : public midi::DefaultSettings {
    static const long BaudRate = 115200; // Debe ser 'static const'
};
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessConfig);
// Declaración de la instancia



void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  // put your setup code here, to run once:
  tempo = 120;
  tempoMicros = int(60000000/(tempo*24.0));
  tipoMsgMidi = 0xFA;
}

void loop() {
  // put your main code here, to run repeatedly:
  tiempoActualMicros = micros();
  tiempoActualMillis = millis();
  midiClock();
  if(tipoMsgMidi == 0xFA){
    MIDI.sendRealTime(midi::Start);

  }
  if(tipoMsgMidi == 0xF8){
    MIDI.sendRealTime(midi::Clock);
  }
  if(clock == 0 && !notaFuera){
    MIDI.sendNoteOn(60,127,1);
    notaFuera = true;
  }
  tipoMsgMidi = 0;
  if(tiempoActualMillis - tiempoRelease > release && notaFuera){
    MIDI.sendNoteOff(60,0,1);
    notaFuera = false;
    tiempoRelease = tiempoActualMillis;
  }
}
