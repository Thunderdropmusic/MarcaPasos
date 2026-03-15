#include <Encoder.h>

// 1. Configuración de los pines
// CLK en pin 18, DT en pin 19 (Pines de interrupción en el Mega)
Encoder miEncoder(17, 18);

const int pinBoton = 23;    // Pin donde conectamos el SW
long posicionAnterior = -999;



void setup() {
  Serial.begin(9600);
  // El botón del KY-040 necesita una resistencia interna (INPUT_PULLUP)
  pinMode(pinBoton, INPUT_PULLUP);

  Serial.println("--- Sistema Listo ---");
  Serial.println("Gira la perilla o pulsa para resetear.");
}

void loop() {
  // 2. Leer la posición actual
  // Dividimos por 2 para que cada "clic" físico sea exactamente 1 número
  long posicionActual = miEncoder.read()/4;

  // Si la posición cambió, la mostramos
  if (posicionActual != posicionAnterior) {
    posicionAnterior = posicionActual;
    Serial.print("Contador: ");
    Serial.println(posicionActual);
  }

  // 3. Detectar el botón (cuando se pulsa, el valor es LOW)
  if (digitalRead(pinBoton) == LOW) {
    delay(200); // Pequeño rebote para evitar lecturas falsas
    
    Serial.println("¡Botón pulsado! Reseteando contador...");
    miEncoder.write(0); // Esto pone el contador del encoder a 0
    
    // Esperamos a que sueltes el botón para no repetir el reset
    while(digitalRead(pinBoton) == LOW); 
  }
}