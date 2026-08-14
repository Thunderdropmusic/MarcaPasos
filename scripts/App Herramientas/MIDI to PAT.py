import os
import struct
import tkinter as tk
from tkinter import filedialog, messagebox

# ==============================================================================
# CONFIGURACIÓN Y CONSTANTES DEL FIRMWARE ARDUINO
# ==============================================================================
N_MAX_STEPS = 16
BYTES_PER_STEP = 5
BYTES_PER_SEQUENCE = 176
N_MAX_SEQS = 5

def parse_midi_file(filepath):
    """Lector MIDI nativo blindado contra SysEx, Meta Events y Running Status"""
    with open(filepath, 'rb') as f:
        data = f.read()

    if data[:4] != b'MThd':
        raise ValueError("El archivo seleccionado no es un archivo MIDI (.mid) válido.")

    header_len = int.from_bytes(data[4:8], 'big')
    fmt = int.from_bytes(data[8:10], 'big')
    num_tracks = int.from_bytes(data[10:12], 'big')
    
    division_raw = struct.unpack('>h', data[12:14])[0]
    
    if division_raw < 0:
        division = 96
    else:
        division = division_raw

    tracks_events = []
    idx = 8 + header_len

    def read_varlen(offset):
        value = 0
        while True:
            byte = data[offset]
            offset += 1
            value = (value << 7) | (byte & 0x7F)
            if not (byte & 0x80):
                break
        return value, offset

    for _ in range(num_tracks):
        if idx >= len(data) or data[idx:idx+4] != b'MTrk':
            break
        trk_len = int.from_bytes(data[idx+4:idx+8], 'big')
        trk_end = idx + 8 + trk_len
        curr_idx = idx + 8

        abs_tick = 0
        running_status = 0
        events = []

        while curr_idx < trk_end:
            delta, curr_idx = read_varlen(curr_idx)
            abs_tick += delta

            if curr_idx >= trk_end:
                break

            byte = data[curr_idx]

            if byte >= 0x80:
                status = byte
                curr_idx += 1
                if status < 0xF0:
                    running_status = status
            else:
                status = running_status

            if status == 0xFF:  # Meta Event
                meta_type = data[curr_idx]
                curr_idx += 1
                length, curr_idx = read_varlen(curr_idx)
                curr_idx += length
            elif status in (0xF0, 0xF7):  # SysEx Event
                length, curr_idx = read_varlen(curr_idx)
                curr_idx += length
            else:
                event_type = status & 0xF0
                if event_type in (0x80, 0x90):  # Note Off / Note On
                    note = data[curr_idx]
                    vel = data[curr_idx+1]
                    curr_idx += 2
                    is_on = (event_type == 0x90) and (vel > 0)
                    events.append((abs_tick, 'note', note, vel if is_on else 0, status & 0x0F))
                elif event_type in (0xA0, 0xB0, 0xE0):
                    curr_idx += 2
                elif event_type in (0xC0, 0xD0):
                    curr_idx += 1
                else:
                    curr_idx += 1

        tracks_events.append(events)
        idx = trk_end

    return tracks_events, division

def create_step_bytes(note=0, vel=0, cc_val=0, mute=True, octave=4):
    """Empaqueta los datos de un paso en exactamente 5 bytes para Arduino"""
    bitfield = (1 if mute else 0) | ((octave & 0x0F) << 2)
    return struct.pack("BBBBB", note, cc_val, vel, 0, bitfield)

def create_sequence_bytes(notes_track, division):
    """Mapea los eventos a la cuadrícula de 16 pasos del secuenciador"""
    steps = [create_step_bytes(mute=True) for _ in range(N_MAX_STEPS)]
    ext_steps = [create_step_bytes(mute=True) for _ in range(N_MAX_STEPS)]

    active_notes = notes_events_only(notes_track)
    
    # INDICE DE SUBDIVISIÓN:
    # 6 = Negras (1/4) | 4 = Corcheas (1/8) | 2 = Semicorcheas (1/16)
    ind_subdiv = 2

    if not active_notes:
        meta_default = struct.pack("BBBBBBBBBBBBBBBB", 0, 0, 7, 0, 0, N_MAX_STEPS, 0, ind_subdiv, 0, 0, 16, 0, ind_subdiv, 0, 0, 0)
        return b''.join(steps) + b''.join(ext_steps) + meta_default

    min_tick = min(ev[0] for ev in active_notes)
    ticks_per_step = division // 4 if division >= 4 else 24

    max_step_index = 0
    channel = 0

    for tick, ev_type, note_num, vel, ch in active_notes:
        channel = ch
        rel_tick = tick - min_tick
        step_idx = round(rel_tick / ticks_per_step)
        
        if step_idx >= (N_MAX_STEPS * 2):
            step_idx = step_idx % (N_MAX_STEPS * 2)
        
        octave = max(0, min(9, note_num // 12))
        base_note = note_num % 12

        step_data = create_step_bytes(note=base_note, vel=vel, mute=False, octave=octave)

        if step_idx < N_MAX_STEPS:
            steps[step_idx] = step_data
        elif step_idx < N_MAX_STEPS * 2:
            ext_steps[step_idx - N_MAX_STEPS] = step_data
            
        max_step_index = max(max_step_index, step_idx + 1)

    total_steps = min(N_MAX_STEPS, max(1, max_step_index if max_step_index <= 16 else 16))
    ext_total_steps = max(1, max_step_index - 16) if max_step_index > 16 else 16

    meta = struct.pack(
        "BBBBBBBBBBBBBBBB",
        channel, 0, 7, 0, 0, total_steps, 0, ind_subdiv, 0, 0,
        ext_total_steps, 0, ind_subdiv, 0, 0, 0
    )

    return b''.join(steps) + b''.join(ext_steps) + meta

def notes_events_only(events):
    return [ev for ev in events if ev[1] == 'note' and ev[3] > 0]

def convert_midi_to_arduino():
    root = tk.Tk()
    root.withdraw()

    midi_path = filedialog.askopenfilename(
        title="Selecciona el archivo MIDI (.mid) a convertir",
        filetypes=[("Archivos MIDI", "*.mid;*.midi"), ("Todos los archivos", "*.*")]
    )

    if not midi_path:
        return

    try:
        tracks_events, division = parse_midi_file(midi_path)
    except Exception as e:
        messagebox.showerror("Error", f"No se pudo leer el archivo MIDI:\n{e}")
        return

    base_name = os.path.splitext(os.path.basename(midi_path))[0][:8].upper()
    output_dir = os.path.dirname(midi_path)

    valid_tracks = [trk for trk in tracks_events if notes_events_only(trk)]

    if not valid_tracks:
        messagebox.showwarning("Aviso", "El archivo MIDI no contiene notas reproducibles.")
        return

    # 1. GENERAR ARCHIVO .SEQ
    seq_bytes_active = create_sequence_bytes(valid_tracks[0], division)
    empty_seq_bytes = create_sequence_bytes([], division)

    seq_file_content = seq_bytes_active + (empty_seq_bytes * 4)

    seq_out_path = os.path.join(output_dir, f"{base_name}.SEQ")
    with open(seq_out_path, 'wb') as f:
        f.write(seq_file_content)

    # 2. GENERAR ARCHIVO .PAT
    n_sequences = []
    for i in range(N_MAX_SEQS):
        if i < len(valid_tracks):
            n_sequences.append(create_sequence_bytes(valid_tracks[i], division))
        else:
            n_sequences.append(empty_seq_bytes)

    pat_name_bytes = base_name.ljust(10).encode('latin1')[:10]
    empty_slots_bytes = empty_seq_bytes * (3 * N_MAX_SEQS)

    pat_file_content = b''.join(n_sequences) + pat_name_bytes + empty_slots_bytes

    pat_out_path = os.path.join(output_dir, f"{base_name}.PAT")
    with open(pat_out_path, 'wb') as f:
        f.write(pat_file_content)

    messagebox.showinfo(
        "¡Conversión Exitosa!",
        f"Se han procesado {len(valid_tracks)} pista(s) en métrica de Semicorcheas (1/16).\n\n"
        f"1. {seq_out_path}\n"
        f"2. {pat_out_path}"
    )

if __name__ == "__main__":
    convert_midi_to_arduino()