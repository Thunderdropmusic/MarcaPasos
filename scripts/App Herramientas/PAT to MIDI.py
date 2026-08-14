import struct
import os
import tkinter as tk
from tkinter import filedialog, messagebox

# ==============================================================================
# TABLAS DE SUBDIVISIONES EXTRAÍDAS DIRECTAMENTE DE ARDUINO (MidiProgramming.cpp)
# ==============================================================================
SUBDIVISIONES_ARRAY = [3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96]

SUBDIVISIONES_COMPLEJAS = [
    [5, 7, 9, 11, 13, 15],
    [10, 14, 18, 22, 26, 30],
    [20, 28, 36, 0, 0, 0]
]

N_MAX_STEPS = 16
BYTES_PER_STEP = 5
BYTES_PER_SEQUENCE = 176
FORMAT_STEP = "BBBBB"

def calcular_ticks_por_step(subdiv_mode, ind_subdiv, complex_x, complex_y, ticks_per_beat=480):
    """
    Calcula la duración exacta en ticks MIDI de cada paso basándose en la configuración de Arduino.
    ticks_per_beat = 480 ticks por negra (1/4).
    """
    if subdiv_mode in (0, 1): # Modo Global o Binario
        idx = min(ind_subdiv, len(SUBDIVISIONES_ARRAY) - 1)
        pulsos_clock = SUBDIVISIONES_ARRAY[idx]
        # Como 24 pulsos equivale a 1 negra (ticks_per_beat):
        ticks_step = int((pulsos_clock / 24.0) * ticks_per_beat)
        return max(1, ticks_step)

    elif subdiv_mode == 2: # Modo Complejo / Tuplas
        y = min(complex_y, 2)
        x = min(complex_x, 5)
        num_divs = SUBDIVISIONES_COMPLEJAS[y][x]
        if num_divs == 0:
            num_divs = 4
        # La subdivisión compleja divide la redonda (4 negras = 4 * ticks_per_beat)
        ticks_redonda = ticks_per_beat * 4
        ticks_step = int(ticks_redonda / num_divs)
        return max(1, ticks_step)

    # Por defecto: Semicorchea (1/16)
    return ticks_per_beat // 4

def parse_sequence_bytes(seq_data, ch_default=0, ticks_per_beat=480):
    """Extrae las notas, configuraciones y calcula los tiempos de inicio para cada paso"""
    # Metadatos (últimos 16 bytes de la estructura Sequence)
    meta_bytes = seq_data[160:176]
    
    canal = meta_bytes[0] if len(meta_bytes) > 0 and meta_bytes[0] > 0 else ch_default
    cc_num = meta_bytes[2] if len(meta_bytes) > 2 else 7

    n_total_steps = meta_bytes[5] if len(meta_bytes) > 5 and meta_bytes[5] > 0 else 16
    subdiv_mode = meta_bytes[6] if len(meta_bytes) > 6 else 0
    ind_subdiv = meta_bytes[7] if len(meta_bytes) > 7 else 5 # Semicorchea por defecto
    complex_y = meta_bytes[8] if len(meta_bytes) > 8 else 0
    complex_x = meta_bytes[9] if len(meta_bytes) > 9 else 0

    # Tiempos para pasos extendidos
    ext_n_total_steps = meta_bytes[10] if len(meta_bytes) > 10 and meta_bytes[10] > 0 else 16
    ext_subdiv_mode = meta_bytes[11] if len(meta_bytes) > 11 else subdiv_mode
    ext_ind_subdiv = meta_bytes[12] if len(meta_bytes) > 12 else ind_subdiv
    ext_complex_y = meta_bytes[13] if len(meta_bytes) > 13 else complex_y
    ext_complex_x = meta_bytes[14] if len(meta_bytes) > 14 else complex_x

    ticks_step_principal = calcular_ticks_por_step(subdiv_mode, ind_subdiv, complex_x, complex_y, ticks_per_beat)
    ticks_step_ext = calcular_ticks_por_step(ext_subdiv_mode, ext_ind_subdiv, ext_complex_x, ext_complex_y, ticks_per_beat)

    notes_events = []
    current_tick = 0

    # 1. Pasos Principales
    num_steps_main = min(N_MAX_STEPS, n_total_steps)
    for i in range(num_steps_main):
        sb = seq_data[i * BYTES_PER_STEP : (i + 1) * BYTES_PER_STEP]
        note, cc_val, vel, curve, bitfield = struct.unpack(FORMAT_STEP, sb)
        mute = bool(bitfield & 0x01)
        octave = (bitfield >> 2) & 0x0F

        if not mute and vel > 0:
            midi_note = min(127, max(0, note + (octave * 12) if note < 24 else note))
            notes_events.append((current_tick, ticks_step_principal, midi_note, vel, cc_val, cc_num, canal))

        current_tick += ticks_step_principal

    # 2. Pasos Extendidos (si existen)
    ext_offset = N_MAX_STEPS * BYTES_PER_STEP
    num_steps_ext = min(N_MAX_STEPS, ext_n_total_steps)
    for i in range(num_steps_ext):
        sb = seq_data[ext_offset + i * BYTES_PER_STEP : ext_offset + (i + 1) * BYTES_PER_STEP]
        note, cc_val, vel, curve, bitfield = struct.unpack(FORMAT_STEP, sb)
        mute = bool(bitfield & 0x01)
        octave = (bitfield >> 2) & 0x0F

        if not mute and vel > 0:
            midi_note = min(127, max(0, note + (octave * 12) if note < 24 else note))
            notes_events.append((current_tick, ticks_step_ext, midi_note, vel, cc_val, cc_num, canal))

        current_tick += ticks_step_ext

    return notes_events

def write_multitrack_midi(output_filename, tracks_data, bpm=120, ticks_per_beat=480):
    """Escribe un archivo MIDI (.mid) respetando los tiempos variables calculados"""
    def write_varlen(val):
        buf = bytearray()
        buf.append(val & 0x7F)
        val >>= 7
        while val > 0:
            buf.insert(0, (val & 0x7F) | 0x80)
            val >>= 7
        return buf

    num_tracks = len(tracks_data)
    header = bytearray(b'MThd\x00\x00\x00\x06\x00\x01') # Tipo 1
    header.extend(num_tracks.to_bytes(2, 'big'))
    header.extend(ticks_per_beat.to_bytes(2, 'big'))

    all_tracks_bytes = bytearray()

    for trk_idx, notes_events in enumerate(tracks_data):
        track_data = bytearray()
        
        trk_name = f"Secuencia {trk_idx + 1}".encode('utf-8')
        track_data.extend(b'\x00\xFF\x03')
        track_data.extend(len(trk_name).to_bytes(1, 'big'))
        track_data.extend(trk_name)

        if trk_idx == 0:
            us_per_beat = int(60_000_000 / bpm)
            track_data.extend(b'\x00\xFF\x51\x03')
            track_data.extend(us_per_beat.to_bytes(3, 'big'))

        events = []
        for start_tick, step_duration, note, vel, cc_val, cc_num, ch in notes_events:
            duration = max(1, int(step_duration * 0.85)) # Gate al 85%
            end_tick = start_tick + duration

            if cc_num > 0 and cc_val > 0:
                events.append((start_tick, 0, 0xB0 | (ch & 0x0F), cc_num & 0x7F, cc_val & 0x7F))
            events.append((start_tick, 1, 0x90 | (ch & 0x0F), note & 0x7F, vel & 0x7F))
            events.append((end_tick, 2, 0x80 | (ch & 0x0F), note & 0x7F, 0))

        events.sort(key=lambda x: (x[0], x[1]))

        last_tick = 0
        for tick, _, status, d1, d2 in events:
            delta_tick = tick - last_tick
            last_tick = tick
            track_data.extend(write_varlen(delta_tick))
            track_data.append(status)
            track_data.append(d1)
            track_data.append(d2)

        track_data.extend(b'\x00\xFF\x2F\x00')

        track_header = bytearray(b'MTrk')
        track_header.extend(len(track_data).to_bytes(4, 'big'))
        all_tracks_bytes.extend(track_header)
        all_tracks_bytes.extend(track_data)

    with open(output_filename, 'wb') as f:
        f.write(header)
        f.write(all_tracks_bytes)

def convert_file():
    root = tk.Tk()
    root.withdraw()

    file_path = filedialog.askopenfilename(
        title="Selecciona un archivo .SEQ o .PAT",
        filetypes=[("Archivos Arduino", "*.PAT;*.SEQ"), ("Todos los archivos", "*.*")]
    )

    if not file_path:
        return

    ext = os.path.splitext(file_path)[1].upper()
    base_name = os.path.splitext(os.path.basename(file_path))[0]
    output_dir = os.path.dirname(file_path)

    with open(file_path, "rb") as f:
        data = f.read()

    if ext == ".SEQ":
        tracks = []
        for seq_i in range(min(5, len(data) // BYTES_PER_SEQUENCE)):
            seq_bytes = data[seq_i * BYTES_PER_SEQUENCE : (seq_i + 1) * BYTES_PER_SEQUENCE]
            events = parse_sequence_bytes(seq_bytes, ch_default=seq_i)
            tracks.append(events)
        out_file = os.path.join(output_dir, f"{base_name}_Subdivisiones.mid")
        write_multitrack_midi(out_file, tracks)

    elif ext == ".PAT":
        tracks = []
        offset = 0
        for seq_i in range(5):
            seq_bytes = data[offset : offset + BYTES_PER_SEQUENCE]
            offset += BYTES_PER_SEQUENCE
            events = parse_sequence_bytes(seq_bytes, ch_default=seq_i)
            tracks.append(events)
        out_file = os.path.join(output_dir, f"{base_name}_Pattern_Subdivisiones.mid")
        write_multitrack_midi(out_file, tracks)

    messagebox.showinfo("Éxito", f"¡Archivo MIDI generado con subdivisiones exactas!\n\nGuardado en:\n{out_file}")

if __name__ == "__main__":
    convert_file()