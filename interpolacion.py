class MidiProgramming:
    def __init__(self):
        # Variables de estado
        self.id = 0 
        self.nClockMsg = 0
        self.nStep = 0
        self.subdivision = 4
        
        # Datos (ajusta los valores según tus pruebas)
        self.globalNTotalSteps = {0: 8} 
        self.globalCCValue = {0: [10, 20, 30, 40, 50, 60, 70, 80, 90]} 
        self.globalCCNumber = {0: 1} 
        
        # Matriz de interpolación
        self.CCinterpolation = {0: [0] * 16} 

    def midi_send_control_change(self, cc_num, value, channel):
        print(f"MIDI SEND: CC {cc_num}, Value {value}, Channel {channel}")

    def CCSend(self):
        # Mantenemos tu lógica original de Arduino
        if self.nClockMsg == 0:
            for i in range(self.subdivision):
                # Usamos exactamente los nombres de tu código
                if self.nStep + 1 <= self.globalNTotalSteps[self.id]:
                    # La división // en Python asegura resultado entero como en C++
                    self.CCinterpolation[self.id][i] = abs(self.globalCCValue[self.id][self.nStep + 1] - self.globalCCValue[self.id][self.nStep]) // self.subdivision
                else:
                    self.CCinterpolation[self.id][i] = abs(self.globalCCValue[self.id][self.nStep - 8] - self.globalCCValue[self.id][self.nStep]) // self.subdivision
            
            # Envío MIDI
            self.midi_send_control_change(
                self.globalCCNumber[self.id], 
                self.CCinterpolation[self.id][self.nClockMsg], 
                self.id
            )

# --- Ejecución de prueba ---
test = MidiProgramming()
test.CCSend()