import struct
import os
import tkinter as tk
from tkinter import filedialog, messagebox

# ==============================================================================
# CONFIGURACIÓN Y CONSTANTES DEL SECUENCIADOR ARDUINO
# ==============================================================================
N_MAX_STEPS = 16
BYTES_PER_STEP = 5
FORMAT_STEP = "BBBBB"

def export_seq_to_midi(file_path, bpm=120, ticks_per_beat=480):
    """Lee un archivo .SEQ y exporta cada secuencia/slot como un archivo MIDI (.mid)"""
    
    if not os.path.exists(file_path):
        return

    base_name = os.path.splitext(os.path.basename(file_path))[0]
    output_dir = os.path.dirname(file_path)

    with open(file_path, "rb") as f:
        file_bytes = f.read()

    # Cada bloque Sequence ocupa 176 bytes (80b steps + 80b ext_steps + 16b config)
    BYTES_PER_SEQUENCE = 176
    total_sequences = len(file_bytes) // BYTES_PER_SEQUENCE

    if total_sequences == 0:
        print("El archivo está vacío o dañado.")
        return

    for seq_idx in range(min(total_sequences, 5)):
        offset = seq_idx * BYTES_PER_SEQUENCE
        seq_data = file_bytes[offset : offset + BYTES_PER_SEQUENCE]

        if len(seq_data) < BYTES_PER_SEQUENCE:
            break

        # Metadatos de la secuencia (últimos 16 bytes)
        meta_bytes = seq_data[160:176]
        channel = meta_bytes[0] if len(meta_bytes) > 0 else 0
        cc_num = meta_bytes[2] if len(meta_bytes) > 2 else 7

        # Colección de eventos de notas
        notes_events = []

        # 1. Pasos principales (0 a 15)
        for i in range(N_MAX_STEPS):
            step_bytes = seq_data[i * BYTES_PER_STEP : (i + 1) * BYTES_PER_STEP]
            note, cc_val, vel, curve, bitfield = struct.unpack(FORMAT_STEP, step_bytes)
            mute = bool(bitfield & 0x01)
            octave = (bitfield >> 2) & 0x0F

            if not mute and vel > 0:
                # Si la nota está desfasada por octava, ajustamos el tono MIDI
                midi_note = min(127, max(0, note + (octave * 12) if note < 24 else note))
                notes_events.append((i, midi_note, vel, cc_val, cc_num, channel))

        # 2. Pasos extendidos (16 a 31)
        ext_offset = N_MAX_STEPS * BYTES_PER_STEP
        for i in range(N_MAX_STEPS):
            step_bytes = seq_data[ext_offset + i * BYTES_PER_STEP : ext_offset + (i + 1) * BYTES_PER_STEP]
            note, cc_val, vel, curve, bitfield = struct.unpack(FORMAT_STEP, step_bytes)
            mute = bool(bitfield & 0x01)
            octave = (bitfield >> 2) & 0x0F

            if not mute and vel > 0:
                midi_note = min(127, max(0, note + (octave * 12) if note < 24 else note))
                notes_events.append((i + 16, midi_note, vel, cc_val, cc_num, channel))

        if not notes_events:
            continue

        # Crear estructura de archivo Standard MIDI Type 0
        tag = "Activa" if seq_idx == 0 else f"Slot_{seq_idx}"
        midi_filename = os.path.join(output_dir, f"{base_name}_{tag}.mid")
        
        write_smf0(midi_filename, notes_events, bpm=bpm, ticks_per_beat=ticks_per_beat)

    messagebox.showinfo("Éxito", f"¡Conversión completada!\nArchivos MIDI guardados en:\n{output_dir}")

def write_smf0(output_filename, notes_events, bpm=120, ticks_per_beat=480):
    """Escribe un archivo Standard MIDI (.mid) binario nativo de 1 pista"""
    def write_varlen(val):
        buf = bytearray()
        buf.append(val & 0x7F)
        val >>= 7
        while val > 0:
            buf.insert(0, (val & 0x7F) | 0x80)
            val >>= 7
        return buf

    # Cabecera MThd
    header = bytearray(b'MThd\x00\x00\x00\x06\x00\x00\x00\x01')
    header.extend(ticks_per_beat.to_bytes(2, 'big'))

    track_data = bytearray()

    # Meta-evento Tempo: 120 BPM
    us_per_beat = int(60_000_000 / bpm)
    track_data.extend(b'\x00\xFF\x51\x03')
    track_data.extend(us_per_beat.to_bytes(3, 'big'))

    step_duration_ticks = ticks_per_beat // 4  # Semicorcheas (1/16th)
    events = []

    for step_idx, note, vel, cc_val, cc_num, ch in notes_events:
        start_tick = step_idx * step_duration_ticks
        duration = int(step_duration_ticks * 0.85)  # Gate al 85%
        end_tick = start_tick + duration

        # Evento CC si existe
        if cc_num > 0 and cc_val > 0:
            events.append((start_tick, 0, 0xB0 | (ch & 0x0F), cc_num & 0x7F, cc_val & 0x7F))
        
        # Note On
        events.append((start_tick, 1, 0x90 | (ch & 0x0F), note & 0x7F, vel & 0x7F))
        # Note Off
        events.append((end_tick, 2, 0x80 | (ch & 0x0F), note & 0x7F, 0))

    # Ordenar por tiempo
    events.sort(key=lambda x: (x[0], x[1]))

    last_tick = 0
    for tick, _, status, d1, d2 in events:
        delta_tick = tick - last_tick
        last_tick = tick
        track_data.extend(write_varlen(delta_tick))
        track_data.append(status)
        track_data.append(d1)
        track_data.append(d2)

    # Fin de pista
    track_data.extend(b'\x00\xFF\x2F\x00')

    track_header = bytearray(b'MTrk')
    track_header.extend(len(track_data).to_bytes(4, 'big'))

    with open(output_filename, 'wb') as f:
        f.write(header)
        f.write(track_header)
        f.write(track_data)

def main():
    root = tk.Tk()
    root.withdraw()

    file_path = filedialog.askopenfilename(
        title="Selecciona un archivo .SEQ para convertir a MIDI",
        filetypes=[("Archivos SEQ", "*.SEQ"), ("Todos los archivos", "*.*")]
    )

    if file_path:
        export_seq_to_midi(file_path)

if __name__ == "__main__":
    main()