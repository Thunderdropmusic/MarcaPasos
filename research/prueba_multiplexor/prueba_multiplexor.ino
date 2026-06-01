// Definición de pines (El pin 35 ya no se usa, EN va a GND)
const int PIN_SIG = 30; 
const int PIN_S0  = 31;
const int PIN_S1  = 32;
const int PIN_S2  = 33;
const int PIN_S3  = 34;

void setup() {
  Serial.begin(9600);
  
  // Configurar pines de control del multiplexor
  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_S3, OUTPUT);
  
  // OBLIGATORIO: Al no tener resistencias físicas, usamos la del Arduino
  pinMode(PIN_SIG, INPUT_PULLUP); 

  Serial.println("=== PRUEBA DE BOTONES (EN a GND) ===");
}

void loop() {
  for (int canal = 0; canal < 16; canal++) {
    
    // Seleccionar el canal en el multiplexor
    digitalWrite(PIN_S0, bitRead(canal, 0));
    digitalWrite(PIN_S1, bitRead(canal, 1));
    digitalWrite(PIN_S2, bitRead(canal, 2));
    digitalWrite(PIN_S3, bitRead(canal, 3));
    
    // Estabilización de señal
    delayMicroseconds(500);
    
    // Leer el botón (LOW = Pulsado)
    if (digitalRead(PIN_SIG) == LOW) {
      
      // Filtro antirrebote: Esperamos 15ms y volvemos a comprobar
      delay(15); 
      if (digitalRead(PIN_SIG) == LOW) {
        Serial.print("Botón presionado en CANAL: ");
        Serial.println(canal);
        
        // Esperar a que sueltes el botón para que no repita el mensaje en bucle
        while(digitalRead(PIN_SIG) == LOW) {
          delay(10); 
        }
      }
    }
  }
}