#include <Arduino.h>
#include <SdFat.h>
#include <SPI.h> 

#include "midiPresets.h"
#include "midiProgramming.h"
#include "menus_buttons.h"
#include "midiInterface.h"
#include "draw_menus.h"
#include "config.h"

MidiPresets presetsUI;

Pattern marcaPasos;
Pattern* p = &marcaPasos;


MidiPresets::MidiPresets() {
  indexSequence = 0;
}

// ==============================================================================
//                                FUNCIONES UTILES
// ==============================================================================

bool MidiPresets::timeDebounce(){
  return (tiempoActualMillis - ultimoTiempoBotonPresets >= DEBOUNCE_DELAY_MS);
}

void MidiPresets::aplicarCambiosBotones(){
  drawUI.updateLCD = true;
  ultimoTiempoBotonPresets = tiempoActualMillis;
  repeatedButton = true;
}

void MidiPresets::sdInit(){
  while (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
    drawUI.sdErrorMsg();
    delay(1000);
  }
  drawUI.sdCheckMsg();
  drawUI.drawLoadProgress(5);
}


void MidiPresets::readSlotsButtons(){
  savePresetButton = !digitalRead(saveButton);
  for(int i = 0; i < 3; i++){
    slots[i] = !digitalRead(slotsPin[i]);
    if (slots[i]) (savePresetButton) ? slotSave(i) : slotLoad(i);
  }
  
  if(!slots[0] && !slots[1] && !slots[2] && !slots[3] && !loadPresetButton && !savePresetButton){repeatedButton = false;}

}

void MidiPresets::saveSeqSD(char* nombreBase, byte tipoGuardado){
  char rutaArchivo[32];   // Necesitamos un array más grande para toda la ruta
  char nombreArchivo[13]; 
  int longitud = 8;
  while(longitud > 0 && (nombreBase[longitud - 1] == ' ' || nombreBase[longitud - 1] == '-')){
    longitud--;
  }
  strncpy(nombreArchivo, nombreBase, longitud);
  nombreArchivo[longitud] = '\0';

  if(tipoGuardado == 0){
    strcat(nombreArchivo, ".SEQ"); 
    if(!sd.exists(F("SEQS"))){
      sd.mkdir(F("SEQS"));
    }
    sprintf(rutaArchivo, "SEQS/%s", nombreArchivo);  
  }
  else {
    strcat(nombreArchivo, ".PAT"); 
    if(!sd.exists(F("PATS"))){
      sd.mkdir(F("PATS"));
    }
    sprintf(rutaArchivo, "PATS/%s", nombreArchivo); 
  }

  // 4. ABRIR Y GUARDAR (¡Fíjate que ahora usamos rutaArchivo!)
  File miArchivo = sd.open(rutaArchivo, O_WRITE | O_CREAT | O_TRUNC);
  
  if(!miArchivo){
    Serial.println(F("Error abriendo/creando el archivo"));
    return;
  }

  if(tipoGuardado == 0){
    Sequence* seqActiva = getActiveSequence(); 
    byte idx = indexSequence; 
      
    // Escribimos los 5 bloques uno detrás de otro en el mismo archivo .SEQ
    miArchivo.write((const uint8_t *)&p->nSequence[idx], sizeof(Sequence));
    for(int i = 0; i < 4; i++){   
      miArchivo.write((const uint8_t *)&p->seq_slots[i][idx], sizeof(Sequence));
    }
  } 
  else{
    miArchivo.write((const uint8_t *)p, sizeof(Pattern));
  }

  miArchivo.close();
}

int MidiPresets::getFileCount(byte tipoCarga){
  int count = 0;
  // Abrimos la carpeta correspondiente
  File dir = sd.open(tipoCarga == 0 ? "SEQS" : "PATS");
  
  if(!dir) return 0; // Si no existe la carpeta, hay 0 archivos

  dir.rewindDirectory(); // Nos aseguramos de empezar desde el principio
  
  while(true){
    File entry = dir.openNextFile();
    if(!entry) break; // Si no hay más archivos, salimos del bucle
    
    // Si es un archivo (no una subcarpeta), sumamos 1
    if(!entry.isDirectory()){
      count++;
    }
    entry.close();
  }
  dir.close();
  return count;
}

void MidiPresets::getFileName(int index, byte tipoCarga, char* bufferSalida){
  File dir = sd.open(tipoCarga == 0 ? "SEQS" : "PATS");
  
  if(!dir){
    strcpy(bufferSalida, "EMPTY   "); 
    return;
  }

  dir.rewindDirectory();
  int currentIndex = 0;
  
  while(true){
    File entry = dir.openNextFile();
    if(!entry){
      strcpy(bufferSalida, "EMPTY   "); 
      break;
    }
    
    // 1. EL ESCUDO: Sacamos el nombre DE FORMA SEGURA antes de comprobar nada
    char nombreTemporal[20];
    entry.getName(nombreTemporal, 20);
    
    // 2. Comprobamos que no sea carpeta y que no sea un archivo oculto de Mac (punto inicial)
    if(!entry.isDirectory() && nombreTemporal[0] != '.'){
      
      if(currentIndex == index){
        // 3. Quitamos la extensión buscando el punto y cortando el texto ahí
        char* punto = strchr(nombreTemporal, '.');
        if(punto) *punto = '\0'; 
        
        // 4. COPIA SEGURA: Pasamos a tu bufferSalida (midiUI) exactamente 8 letras.
        int longitudNombre = strlen(nombreTemporal);
        for(int i = 0; i < 8; i++){
          if(i < longitudNombre){
            bufferSalida[i] = nombreTemporal[i];
          } else {
            bufferSalida[i] = ' '; // Relleno de espacios
          }
        }
        bufferSalida[8] = '\0'; // Cerramos el string de la interfaz con total seguridad
        
        entry.close();
        break; 
      }
      currentIndex++;
    }
    entry.close();
  }
  dir.close();
  int len = strlen(bufferSalida);
  while(len > 0 && bufferSalida[len - 1] == ' '){
    bufferSalida[len - 1] = '\0';
    len--;
  }
}

void MidiPresets::loadSeqSD(char* nombreBase, byte tipoCarga){
  char rutaArchivo[30];
  getFileRoot(nombreBase, tipoCarga, rutaArchivo);
  if(!sd.exists(rutaArchivo)){
    return;
  }
  File miArchivo = sd.open(rutaArchivo, O_READ);
  
  if(!miArchivo){
    return;
  }
  if(tipoCarga == 0){
    Sequence* seqActiva = getActiveSequence(); 
    byte idx = indexSequence;
    miArchivo.read((uint8_t *)&p->nSequence[idx], sizeof(Sequence));
    for(int i = 0; i < 4; i++){ 
      miArchivo.read((uint8_t *)&p->seq_slots[i][idx], sizeof(Sequence));
    }
  } 
  else {
    miArchivo.read((uint8_t *)p, sizeof(Pattern));
  }

  miArchivo.close();

  drawUI.updateLCD = true;
  drawUI.updateValues = true; 
}

void MidiPresets::deleteSeqSD(char* bufferSalida, byte tipoCarga){
  char rutaArchivo[30];
  getFileRoot(bufferSalida, tipoCarga, rutaArchivo);

  if(sd.exists(rutaArchivo)) sd.remove(rutaArchivo);
}

void MidiPresets::slotLoad(byte number){
  if (!timeDebounce()) return;
  if (repeatedButton) return;
  p->nSequence[indexSequence] = p->seq_slots[number][indexSequence];
  aplicarCambiosBotones();
}

void MidiPresets::slotSave(byte number){
  if (!timeDebounce()) return;
  if (repeatedButton) return;
  p->seq_slots[number][indexSequence] = p->nSequence[indexSequence];
  menusUI.menuActual = 6;
  nSlot = number + 1;
  aplicarCambiosBotones();
}

void MidiPresets::getFileRoot(char* bufferSalida, byte tipoCarga, char* rutaDestino){
  char nombreArchivo[13];
  // 1. Limpiamos los espacios en blanco del nombre (igual que al cargar)
  strcpy(nombreArchivo, bufferSalida);
  int longitud = strlen(nombreArchivo);
  while(longitud > 0 && nombreArchivo[longitud - 1] == ' '){
    longitud--;
  }
  nombreArchivo[longitud] = '\0';
  
  // 2. Montamos la ruta correcta
  if(tipoCarga == 0){
    strcat(nombreArchivo, ".SEQ"); 
    sprintf(rutaDestino, "SEQS/%s", nombreArchivo); 
  } 
  else {
    strcat(nombreArchivo, ".PAT"); 
    sprintf(rutaDestino, "PATS/%s", nombreArchivo); 
  }
}
