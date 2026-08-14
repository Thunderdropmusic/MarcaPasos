import struct
import tkinter as tk
from tkinter import filedialog

N_MAX_STEPS = 16

# En C++, con el alignment/padding del compilador, Step ocupa 8 bytes en memoria SD
FORMATO_STEP = "<BBBBBBBB" 
TAMANO_STEP = struct.calcsize(FORMATO_STEP)

# Metadatos de la estructura Sequence (16 bytes en total)
FORMATO_SEQ_META = "<BBBBBBBBBBBBBB"

def decodificar_step(datos_bytes):
    unpacked = struct.unpack(FORMATO_STEP, datos_bytes)
    
    note = unpacked[0]
    cc_val = unpacked[1]
    velocity = unpacked[2]
    cc_curve = unpacked[3]
    
    # El byte 4 contiene los bitfields (mutes, ccMutes, octave)
    bitfield = unpacked[4]
    mutes = bool(bitfield & 0x01)
    cc_mutes = bool((bitfield >> 1) & 0x01)
    octave = (bitfield >> 2) & 0x0F

    return {
        "nota": note,
        "cc_val": cc_val,
        "velocity": velocity,
        "cc_curve": cc_curve,
        "mute": mutes,
        "cc_mute": cc_mutes,
        "octava": octave
    }

def seleccionar_y_leer_archivo():
    root = tk.Tk()
    root.withdraw()

    ruta_archivo = filedialog.askopenfilename(
        title="Selecciona un archivo de secuencia (.SEQ)",
        filetypes=[("Archivos SEQ", "*.SEQ"), ("Todos los archivos", "*.*")]
    )

    if not ruta_archivo:
        print("No se seleccionó ningún archivo.")
        return

    print(f"\n==========================================")
    print(f" Leyendo: {ruta_archivo}")
    print(f"==========================================\n")

    with open(ruta_archivo, "rb") as f:
        # El archivo .SEQ guarda 5 bloques de Sequence (1 Activa + 4 Slots)
        for bloque_idx in range(5):
            etiqueta = "SECUENCIA ACTIVA" if bloque_idx == 0 else f"SLOT DE MEMORIA {bloque_idx}"
            print(f"\n--- {etiqueta} ---")

            # 1. Pasos Principales
            print("\n > Pasos Principales:")
            for i in range(N_MAX_STEPS):
                raw_step = f.read(TAMANO_STEP)
                if len(raw_step) < TAMANO_STEP:
                    print(" Fin del archivo alcanzado de forma limpia.")
                    return
                step = decodificar_step(raw_step)
                print(f"   Paso {i+1:02d} | Nota: {step['nota']:3d} | Vel: {step['velocity']:3d} | CC_Val: {step['cc_val']:3d} | Octava: {step['octava']} | Mute: {step['mute']}")

            # 2. Pasos Extendidos
            print("\n > Pasos Extendidos:")
            for i in range(N_MAX_STEPS):
                raw_step = f.read(TAMANO_STEP)
                if len(raw_step) < TAMANO_STEP:
                    print(" Fin del archivo alcanzado.")
                    return
                step = decodificar_step(raw_step)
                print(f"   ExtPaso {i+1:02d} | Nota: {step['nota']:3d} | Vel: {step['velocity']:3d} | CC_Val: {step['cc_val']:3d} | Octava: {step['octava']} | Mute: {step['mute']}")

            # 3. Metadatos / Configuración
            # Leemos los bytes restantes de la estructura Sequence
            raw_meta = f.read(16)
            if len(raw_meta) == 16:
                meta = struct.unpack(FORMATO_SEQ_META, raw_meta)
                print(f"\n > Configuración:")
                print(f"   Canal MIDI: {meta[0]} | Modo: {meta[1]} | Num CC: {meta[2]} | Escala: {meta[3]} | Tono: {meta[4]}")
                print(f"   Total Pasos: {meta[5]} | Subdiv Modo: {meta[6]}")

if __name__ == "__main__":
    seleccionar_y_leer_archivo()