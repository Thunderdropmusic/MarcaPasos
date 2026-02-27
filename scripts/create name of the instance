import os
import re

# CONFIGURACIÓN
nombre_instancia = "midiUI"
# Añade aquí todos los archivos .cpp y .ino de tu carpeta donde uses estas variables
archivos_a_modificar = ["MarcaPasos_reestructuracion_cpp.ino", "draw_menus.cpp", "menus_buttons.cpp", "midiPrograming.cpp"]

# Pegamos aquí el bloque de texto que me has pasado
bloque_texto = """
    static const byte potesPin[NUM_POTES];
    static const byte mutesPin[NUM_POTES];
    static const byte pinOctPlus;
    static const byte pinOctRest;
    static int notesValue[NUM_POTES];
    static bool mutesValue[NUM_POTES];
    static int octavaValue[NUM_POTES];
    static int velocityValue[NUM_POTES];
    static bool mutePulsado[NUM_POTES];
    static bool muteActivado;
    static bool poteMovido;
    static bool poteMovidoMutes;
    static int val;
    static int movedPoteValue;
    static int movedPoteNumber;
    static int indexMovedMute;
    static unsigned long timeShowPotValue;
    static unsigned long timeShowOctValue;
    static unsigned long ultimoTiempoBotonMute[NUM_MUTES];
    static unsigned long TiempoPulsadoBotonMute[NUM_MUTES];
    static unsigned long ultimoTiempoBotonOct;
    static bool mute;
    static bool octPlusPulsed;
    static bool octRestPulsed;
    static bool pulsedIndividualOctave;
"""

# EXTRAER NOMBRES DE VARIABLES AUTOMÁTICAMENTE
# Esta regex busca la palabra que está justo antes del ';' o del '['
patron_extraer = r'(?:static\s+[\w\s]+\s+)(\w+)(?:\[|;)'
variables = re.findall(patron_extraer, bloque_texto)

def procesar_archivos():
    print(f"--- Iniciando conversión a {nombre_instancia}.variable ---")
    
    for nombre_archivo in archivos_a_modificar:
        if not os.path.exists(nombre_archivo):
            print(f"⚠️ Saltando {nombre_archivo} (no encontrado en la carpeta)")
            continue
            
        with open(nombre_archivo, 'r', encoding='utf-8') as f:
            contenido = f.read()

        nuevo_contenido = contenido
        conteo_cambios = 0
        
        # Ordenamos por longitud (de más largo a más corto) para evitar errores 
        # (ej: que no cambie parte de una variable más larga)
        for var in sorted(variables, key=len, reverse=True):
            # No tocar 'val' si prefieres manejarlo como variable local
            if var == "val": continue 
            
            # Buscar palabra exacta
            patron_reemplazo = r'\b' + re.escape(var) + r'\b'
            reemplazo = f"{nombre_instancia}.{var}"
            
            # Solo reemplazamos si no tiene ya el prefijo delante
            if f"{nombre_instancia}.{var}" not in contenido:
                nuevo_contenido, n = re.subn(patron_reemplazo, reemplazo, nuevo_contenido)
                conteo_cambios += n

        with open(nombre_archivo, 'w', encoding='utf-8') as f:
            f.write(nuevo_contenido)
        
        print(f"✅ {nombre_archivo}: {conteo_cambios} cambios realizados.")

if __name__ == "__main__":
    procesar_archivos()
    print("--- Proceso finalizado. ¡Recuerda compilar para verificar! ---")