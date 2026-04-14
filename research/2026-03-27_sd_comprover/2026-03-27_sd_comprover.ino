#include <SPI.h>
#include "SdFat.h" // Fíjate que ahora usamos comillas o corchetes para SdFat

// --- CREAR LOS OBJETOS PARA LA LIBRERÍA ---
SdFat sd;         // Este objeto representa tu tarjeta física
File testFile;    // Este objeto representa el archivo que vamos a abrir

// --- CONFIGURACIÓN DE TUS PINES ---
const int chipSelect = 53; 
// MISO=50, MOSI=51, SCK=52 (El Mega los asume automáticamente)

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; } // Esperar a que el monitor serie esté listo

  Serial.println("--- DIAGNÓSTICO DE TARJETA SD (Usando SdFat) ---");

  // Intentar inicializar la tarjeta. 
  // SPI_HALF_SPEED es un buen seguro de vida si los cables no son 100% perfectos.
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
    Serial.println("❌ ERROR: No se pudo encontrar o iniciar la tarjeta.");
    Serial.println("Causas probables:");
    Serial.println(" 1. ¿Están bien los cables?");
    Serial.println(" 2. ¿La SD está bien insertada?");
    
    // Esta línea extra de SdFat te da información técnica detallada si falla
    // sd.initErrorHalt(); 
    return;
  }

  Serial.println("✅ ¡TARJETA DETECTADA CORRECTAMENTE CON SDFAT!");

  // Prueba de escritura rápida
  // Usamos sd.open en minúsculas. FILE_WRITE funciona igual que antes.
  testFile = sd.open("test.txt", FILE_WRITE);
  
  if (testFile) {
    testFile.println("Probando escritura con la nueva libreria SdFat en Arduino Mega...");
    testFile.close();
    Serial.println("✅ Escritura de prueba: OK (Archivo 'test.txt' creado/modificado)");
  } else {
    Serial.println("❌ ERROR: No se pudo escribir en la tarjeta.");
  }

  // Prueba de lectura rápida
  testFile = sd.open("test.txt");
  if (testFile) {
    Serial.print("✅ Lectura de prueba: \n");
    while (testFile.available()) {
      Serial.write(testFile.read());
    }
    testFile.close();
  } else {
    Serial.println("❌ ERROR: No se pudo abrir para leer.");
  }
  
  Serial.println("\n--- FIN DEL DIAGNÓSTICO ---");
}

void loop() {
  // Nada aquí
}