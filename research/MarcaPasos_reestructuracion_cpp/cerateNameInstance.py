import os
import re

# CONFIGURACIÓN
nombre_instancia = "menusUI"  # Cambiado a menusUI para tu nueva clase
# Archivos que SÍ quieres modificar (donde se USAN las variables)
archivos_a_modificar = [
    "MarcaPasos_reestructuracion_cpp.ino", 
    "draw_menus.cpp", 
    "MidiProgramming.cpp"
]

# LISTA NEGRA: Archivos que NUNCA debe tocar el script
archivos_protegidos = [
    "menus_buttons.h", 
    "menus_buttons.cpp"
]

# Bloque de texto con las variables de la nueva clase
bloque_texto = """
    const byte pinButton1 = 22; // Arriba
    const byte pinButton2 = 23; // OK / Entrar
    const byte pinButton4 = 24; // Volver
    const byte pinButton3 = 25; // Abajo
    const byte pinButton5 = 27; // Izquierda
    const byte pinButton6 = 29; // Derecha


    int menusUI.menuActual, menuAnterior, menusUI.seleccion;
    bool selectNum;
    bool selectDen;


    int indiceSubdivisionesComplejasNum, indiceSubdivisionesComplejasDen, menusUI.indComplexSubdivX, menusUI.indComplexSubdivY;
    int indiceSubdivisiones;
    int ultimaConfiguracionModo0;
    int ultimaConfiguracionModo1;
    int ultimaConfiguracionModo2[2];
    bool cambioModo0;
    bool cambioModo1;
    bool cambioModo2;


    bool repeatedButton;

    unsigned long ultimoTiempoBotonMenu;
    bool midiProg.play;


    void aplicarCambios();
    void checkButtons();
    void showPotes();
"""

# EXTRAER NOMBRES (Mejorado para detectar variables con o sin 'static')
patron_extraer = r'(?:[\w\s]+\s+)(\w+)(?:\s*\[|\s*=|;)'
variables = re.findall(patron_extraer, bloque_texto)

def procesar_archivos():
    print(f"--- Iniciando conversión a {nombre_instancia}.variable ---")
    
    # Filtrar archivos para asegurar que no tocamos los protegidos
    lista_final = [f for f in archivos_a_modificar if f not in archivos_protegidos]
    
    for nombre_archivo in lista_final:
        if not os.path.exists(nombre_archivo):
            print(f"⚠️ Saltando {nombre_archivo} (no encontrado)")
            continue
            
        with open(nombre_archivo, 'r', encoding='utf-8') as f:
            contenido = f.read()

        nuevo_contenido = contenido
        conteo_cambios = 0
        
        # Ordenar por longitud para no romper nombres compuestos
        for var in sorted(variables, key=len, reverse=True):
            if var in ["int", "bool", "byte", "const"]: continue # Seguridad
            
            patron_reemplazo = r'\b' + re.escape(var) + r'\b'
            reemplazo = f"{nombre_instancia}.{var}"
            
            # Solo reemplaza si la palabra existe y no tiene ya el punto delante
            # (evita convertir menusUI.menusUI.menuActual en menusUI.menusUI.menusUI.menuActual)
            if re.search(patron_reemplazo, nuevo_contenido):
                # Usamos una función de reemplazo negativa para no tocar lo que ya tiene punto
                patron_especifico = r'(?<!\.)\b' + re.escape(var) + r'\b'
                nuevo_contenido, n = re.subn(patron_especifico, reemplazo, nuevo_contenido)
                conteo_cambios += n

        with open(nombre_archivo, 'w', encoding='utf-8') as f:
            f.write(nuevo_contenido)
        
        print(f"✅ {nombre_archivo}: {conteo_cambios} cambios realizados.")

if __name__ == "__main__":
    procesar_archivos()
    print("--- Proceso finalizado ---")