import os
import struct
import time
import threading
import tkinter as tk
from tkinter import ttk, filedialog, messagebox, scrolledtext
import serial
import serial.tools.list_ports
import sys
import subprocess

# ==============================================================================
# 1. CONSTANTES Y TABLAS DEL FIRMWARE
# ==============================================================================
N_MAX_STEPS = 16
BYTES_PER_STEP = 5
BYTES_PER_SEQUENCE = 176
N_MAX_SEQS = 5

CMD_START_TRANSFER = 0xAA
CMD_ACK            = 0x06
CMD_ERROR          = 0x15

SUBDIVISIONES_ARRAY = [3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96]
SUBDIVISIONES_COMPLEJAS = [
    [5, 7, 9, 11, 13, 15],
    [10, 14, 18, 22, 26, 30],
    [20, 28, 36, 0, 0, 0]
]

def resource_path(relative_path):
    """ Obtiene la ruta absoluta de un recurso, compatible con PyInstaller """
    try:
        # PyInstaller crea una carpeta temporal y guarda la ruta en _MEIPASS
        base_path = sys._MEIPASS
    except Exception:
        base_path = os.path.abspath(".")

    return os.path.join(base_path, relative_path)

# ==============================================================================
# 2. FUNCIONES LÓGICAS (MIDI, PAT, SEQ)
# ==============================================================================
def parse_midi_file(filepath):
    with open(filepath, 'rb') as f:
        data = f.read()

    if data[:4] != b'MThd':
        raise ValueError("El archivo seleccionado no es un archivo MIDI (.mid) válido.")

    header_len = int.from_bytes(data[4:8], 'big')
    num_tracks = int.from_bytes(data[10:12], 'big')
    division_raw = struct.unpack('>h', data[12:14])[0]
    division = 96 if division_raw < 0 else division_raw

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

            if status == 0xFF:
                meta_type = data[curr_idx]
                curr_idx += 1
                length, curr_idx = read_varlen(curr_idx)
                curr_idx += length
            elif status in (0xF0, 0xF7):
                length, curr_idx = read_varlen(curr_idx)
                curr_idx += length
            else:
                event_type = status & 0xF0
                if event_type in (0x80, 0x90):
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
    bitfield = (1 if mute else 0) | ((octave & 0x0F) << 2)
    return struct.pack("BBBBB", note, cc_val, vel, 0, bitfield)

def create_sequence_bytes(notes_track, division):
    steps = [create_step_bytes(mute=True) for _ in range(N_MAX_STEPS)]
    ext_steps = [create_step_bytes(mute=True) for _ in range(N_MAX_STEPS)]
    active_notes = [ev for ev in notes_track if ev[1] == 'note' and ev[3] > 0]
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

def calcular_ticks_por_step(subdiv_mode, ind_subdiv, complex_x, complex_y, ticks_per_beat=480):
    if subdiv_mode in (0, 1):
        idx = min(ind_subdiv, len(SUBDIVISIONES_ARRAY) - 1)
        pulsos_clock = SUBDIVISIONES_ARRAY[idx]
        return max(1, int((pulsos_clock / 24.0) * ticks_per_beat))
    elif subdiv_mode == 2:
        y, x = min(complex_y, 2), min(complex_x, 5)
        num_divs = SUBDIVISIONES_COMPLEJAS[y][x] or 4
        return max(1, int((ticks_per_beat * 4) / num_divs))
    return ticks_per_beat // 4

def parse_sequence_bytes(seq_data, ch_default=0, ticks_per_beat=480):
    meta_bytes = seq_data[160:176]
    canal = meta_bytes[0] if len(meta_bytes) > 0 and meta_bytes[0] > 0 else ch_default
    cc_num = meta_bytes[2] if len(meta_bytes) > 2 else 7

    n_total_steps = meta_bytes[5] if len(meta_bytes) > 5 and meta_bytes[5] > 0 else 16
    subdiv_mode = meta_bytes[6] if len(meta_bytes) > 6 else 0
    ind_subdiv = meta_bytes[7] if len(meta_bytes) > 7 else 5
    complex_y = meta_bytes[8] if len(meta_bytes) > 8 else 0
    complex_x = meta_bytes[9] if len(meta_bytes) > 9 else 0

    ext_n_total_steps = meta_bytes[10] if len(meta_bytes) > 10 and meta_bytes[10] > 0 else 16
    ext_subdiv_mode = meta_bytes[11] if len(meta_bytes) > 11 else subdiv_mode
    ext_ind_subdiv = meta_bytes[12] if len(meta_bytes) > 12 else ind_subdiv
    ext_complex_y = meta_bytes[13] if len(meta_bytes) > 13 else complex_y
    ext_complex_x = meta_bytes[14] if len(meta_bytes) > 14 else complex_x

    ticks_step_principal = calcular_ticks_por_step(subdiv_mode, ind_subdiv, complex_x, complex_y, ticks_per_beat)
    ticks_step_ext = calcular_ticks_por_step(ext_subdiv_mode, ext_ind_subdiv, ext_complex_x, ext_complex_y, ticks_per_beat)

    notes_events = []
    current_tick = 0

    for i in range(min(N_MAX_STEPS, n_total_steps)):
        sb = seq_data[i * BYTES_PER_STEP : (i + 1) * BYTES_PER_STEP]
        note, cc_val, vel, curve, bitfield = struct.unpack("BBBBB", sb)
        mute = bool(bitfield & 0x01)
        octave = (bitfield >> 2) & 0x0F

        if not mute and vel > 0:
            midi_note = min(127, max(0, note + (octave * 12) if note < 24 else note))
            notes_events.append((current_tick, ticks_step_principal, midi_note, vel, cc_val, cc_num, canal))
        current_tick += ticks_step_principal

    ext_offset = N_MAX_STEPS * BYTES_PER_STEP
    for i in range(min(N_MAX_STEPS, ext_n_total_steps)):
        sb = seq_data[ext_offset + i * BYTES_PER_STEP : ext_offset + (i + 1) * BYTES_PER_STEP]
        note, cc_val, vel, curve, bitfield = struct.unpack("BBBBB", sb)
        mute = bool(bitfield & 0x01)
        octave = (bitfield >> 2) & 0x0F

        if not mute and vel > 0:
            midi_note = min(127, max(0, note + (octave * 12) if note < 24 else note))
            notes_events.append((current_tick, ticks_step_ext, midi_note, vel, cc_val, cc_num, canal))
        current_tick += ticks_step_ext

    return notes_events

def write_multitrack_midi(output_filename, tracks_data, bpm=120, ticks_per_beat=480):
    def write_varlen(val):
        buf = bytearray()
        buf.append(val & 0x7F)
        val >>= 7
        while val > 0:
            buf.insert(0, (val & 0x7F) | 0x80)
            val >>= 7
        return buf

    header = bytearray(b'MThd\x00\x00\x00\x06\x00\x01')
    header.extend(len(tracks_data).to_bytes(2, 'big'))
    header.extend(ticks_per_beat.to_bytes(2, 'big'))
    all_tracks_bytes = bytearray()

    for trk_idx, notes_events in enumerate(tracks_data):
        track_data = bytearray()
        trk_name = f"Secuencia {trk_idx + 1}".encode('utf-8')
        track_data.extend(b'\x00\xFF\x03' + len(trk_name).to_bytes(1, 'big') + trk_name)

        if trk_idx == 0:
            us_per_beat = int(60_000_000 / bpm)
            track_data.extend(b'\x00\xFF\x51\x03' + us_per_beat.to_bytes(3, 'big'))

        events = []
        for start_tick, step_duration, note, vel, cc_val, cc_num, ch in notes_events:
            duration = max(1, int(step_duration * 0.85))
            end_tick = start_tick + duration
            if cc_num > 0 and cc_val > 0:
                events.append((start_tick, 0, 0xB0 | (ch & 0x0F), cc_num & 0x7F, cc_val & 0x7F))
            events.append((start_tick, 1, 0x90 | (ch & 0x0F), note & 0x7F, vel & 0x7F))
            events.append((end_tick, 2, 0x80 | (ch & 0x0F), note & 0x7F, 0))

        events.sort(key=lambda x: (x[0], x[1]))
        last_tick = 0
        for tick, _, status, d1, d2 in events:
            track_data.extend(write_varlen(tick - last_tick))
            track_data.append(status)
            track_data.append(d1)
            track_data.append(d2)
            last_tick = tick

        track_data.extend(b'\x00\xFF\x2F\x00')
        track_header = bytearray(b'MTrk') + len(track_data).to_bytes(4, 'big')
        all_tracks_bytes.extend(track_header + track_data)

    with open(output_filename, 'wb') as f:
        f.write(header + all_tracks_bytes)

# ==============================================================================
# 3. INTERFAZ GRÁFICA UNIFICADA
# ==============================================================================
class AppSecuenciador:
    def __init__(self, root):
        self.root = root
        self.root.title("Control Center - Secuenciador Arduino")
        self.root.geometry("620x480")
        self.root.resizable(False, False)

        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill="both", expand=True, padx=10, pady=10)

        # Crear Pestañas
        self.tab_tools = ttk.Frame(self.notebook)
        self.tab_usb = ttk.Frame(self.notebook)
        self.tab_midi_in = ttk.Frame(self.notebook)
        self.tab_midi_out = ttk.Frame(self.notebook)
        self.tab_inspector = ttk.Frame(self.notebook)

        self.notebook.add(self.tab_tools, text=" 🚀 Entorno MIDI ")
        self.notebook.add(self.tab_usb, text=" 🔌 Enviar a SD ")
        self.notebook.add(self.tab_midi_in, text=" 🎹 MIDI ➔ SEQ/PAT ")
        self.notebook.add(self.tab_midi_out, text=" 🎼 SEQ/PAT ➔ MIDI ")
        self.notebook.add(self.tab_inspector, text=" 🔍 Inspector ")

        self.setup_tab_tools()
        self.setup_tab_usb()
        self.setup_tab_midi_in()
        self.setup_tab_midi_out()
        self.setup_tab_inspector()

           # --- PESTAÑA 1: LANZAR ENTORNO MIDI ---
    def setup_tab_tools(self):
        frame = ttk.LabelFrame(self.tab_tools, text=" Enrutado y Monitorización MIDI ")
        frame.pack(fill="both", expand=True, padx=15, pady=15)

        lbl = ttk.Label(
            frame, 
            text="Lanza la suite completa de herramientas MIDI para comunicación Serie:\n"
                 "• loopMIDI (Puertos virtuales)\n"
                 "• MIDI-OX (Monitor/Router)\n"
                 "• Hairless MIDI<->Serial (Puente Serial)", 
            justify="left",
            font=("Arial", 9)
        )
        lbl.pack(padx=15, pady=15, anchor="w")

        self.btn_launch = ttk.Button(frame, text="🚀 Iniciar Suite MIDI Completa", command=self.lanzar_entorno_midi)
        self.btn_launch.pack(pady=15, ipadx=10, ipady=8)

        self.lbl_status_tools = ttk.Label(frame, text="Estado: Esperando orden de inicio...", font=("Arial", 9))
        self.lbl_status_tools.pack(pady=10)

    def lanzar_entorno_midi(self):
        def proceso_lanzamiento():
            self.btn_launch.config(state="disabled")
            try:
                # 1. loopMIDI
                self.lbl_status_tools.config(text="Abriendo loopMIDI...")
                os.startfile(r"C:\ProgramData\Microsoft\Windows\Start Menu\Programs\loopMIDI\loopMIDI")
                time.sleep(2)

                # 2. MIDI-OX
                self.lbl_status_tools.config(text="Abriendo MIDI-OX...")
                os.startfile(r"C:\ProgramData\Microsoft\Windows\Start Menu\Programs\MIDI-OX\MIDI-OX")
                time.sleep(2)

                # 3. Hairless MIDI-Serial
                self.lbl_status_tools.config(text="Abriendo Hairless MIDI-Serial...")

                # Obtenemos la ruta completa del .exe de Hairless dentro de PyInstaller
                hairless_exe = resource_path(r"hairless-midiserial-0.4-windows\hairless-midiserial\hairless-midiserial.exe")

                # Obtenemos la carpeta donde está guardado Hairless junto a sus DLLs
                hairless_dir = os.path.dirname(hairless_exe)

                # Lo ejecutamos indicando explícitamente su directorio de trabajo (cwd)
                subprocess.Popen([hairless_exe], cwd=hairless_dir)

                self.lbl_status_tools.config(text="¡Todos los programas iniciados correctamente!")
            except Exception as e:
                messagebox.showerror("Error al lanzar entorno", f"Ocurrió un problema al abrir los programas:\n{e}")
                self.lbl_status_tools.config(text="Error al iniciar la suite.")
            finally:
                self.btn_launch.config(state="normal")

        # Se ejecuta en un hilo separado para no congelar la GUI
        threading.Thread(target=proceso_lanzamiento, daemon=True).start()

    # --- PESTAÑA 2: ENVÍO USB ---
    def setup_tab_usb(self):
        frame = ttk.LabelFrame(self.tab_usb, text=" Selección de Puerto USB / COM ")
        frame.pack(fill="x", padx=15, pady=15)

        ttk.Label(frame, text="Puerto COM:").pack(side="left", padx=10, pady=10)
        self.combo_ports = ttk.Combobox(frame, state="readonly", width=25)
        self.combo_ports.pack(side="left", padx=5, pady=10)

        btn_refresh = ttk.Button(frame, text="🔄 Refrescar", command=self.actualizar_puertos)
        btn_refresh.pack(side="left", padx=5, pady=10)

        btn_send = ttk.Button(self.tab_usb, text="📁 Seleccionar y Enviar Archivo (.SEQ / .PAT)", command=self.enviar_a_arduino)
        btn_send.pack(pady=20, ipadx=10, ipady=8)

        self.progress = ttk.Progressbar(self.tab_usb, orient="horizontal", length=400, mode="determinate")
        self.progress.pack(pady=10)

        self.lbl_status_usb = ttk.Label(self.tab_usb, text="Estado: Selecciona el puerto COM y envía un archivo", font=("Arial", 9))
        self.lbl_status_usb.pack(pady=10)

        self.actualizar_puertos()

    def actualizar_puertos(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.combo_ports['values'] = ports
        if ports:
            self.combo_ports.current(0)
        else:
            self.combo_ports.set('')

    def enviar_a_arduino(self):
        puerto_com = self.combo_ports.get()
        if not puerto_com:
            messagebox.showerror("Error", "No has seleccionado ningún puerto COM.")
            return

        ruta_archivo = filedialog.askopenfilename(
            title="Selecciona el archivo .SEQ o .PAT",
            filetypes=[("Archivos Arduino", "*.PAT;*.SEQ"), ("Todos", "*.*")]
        )
        if not ruta_archivo:
            return

        ext = os.path.splitext(ruta_archivo)[1].upper()
        nombre_base = os.path.splitext(os.path.basename(ruta_archivo))[0][:8].upper().ljust(8)
        tipo_guardado = 0 if ext == ".SEQ" else 1

        with open(ruta_archivo, "rb") as f:
            datos = f.read()
        tamano_total = len(datos)

        try:
            self.lbl_status_usb.config(text=f"Conectando a {puerto_com}...")
            self.root.update()

            ser = serial.Serial()
            ser.port = puerto_com
            ser.baudrate = 115200
            ser.timeout = 5
            ser.open()
            ser.setDTR(False)

            time.sleep(2.2)
            ser.reset_input_buffer()

            ser.write(bytes([CMD_START_TRANSFER]))
            time.sleep(0.05)
            ser.write(bytes([tipo_guardado]))
            time.sleep(0.05)
            ser.write(nombre_base.encode('ascii'))
            time.sleep(0.05)
            ser.write(struct.pack(">H", tamano_total))
            ser.flush()

            ack_cabecera = ser.read(1)
            if not ack_cabecera or ack_cabecera[0] != CMD_ACK:
                messagebox.showerror("Error", "El Arduino no respondió ACK a la cabecera.")
                ser.close()
                return

            CHUNK_SIZE = 32
            self.progress['maximum'] = tamano_total
            for i in range(0, tamano_total, CHUNK_SIZE):
                bloque = datos[i : i + CHUNK_SIZE]
                ser.write(bloque)
                ser.flush()
                time.sleep(0.03)
                self.progress['value'] = i + len(bloque)
                self.root.update()

            time.sleep(0.8)
            ser.timeout = 3.0
            ack_final = ser.read(1)
            ser.close()

            if ack_final and ack_final[0] == CMD_ACK:
                self.lbl_status_usb.config(text="¡Transferencia completada con éxito!")
                messagebox.showinfo("¡Éxito!", f"¡Archivo {ext} guardado correctamente en la SD!")
            else:
                messagebox.showerror("Error", "Fallo al confirmar la escritura final en la SD.")

        except Exception as err:
            messagebox.showerror("Error de comunicación", str(err))

 

    # --- PESTAÑA 3: CONVERTIR MIDI A SEQ/PAT ---
    def setup_tab_midi_in(self):
        ttk.Label(self.tab_midi_in, text="Convertir archivos de música MIDI (.mid) al formato de Arduino", font=("Arial", 10)).pack(pady=20)
        btn = ttk.Button(self.tab_midi_in, text="📁 Seleccionar MIDI y Convertir", command=self.convertir_midi)
        btn.pack(pady=10, ipadx=10, ipady=6)

    def convertir_midi(self):
        midi_path = filedialog.askopenfilename(filetypes=[("Archivos MIDI", "*.mid;*.midi")])
        if not midi_path: return
        try:
            tracks_events, division = parse_midi_file(midi_path)
            valid_tracks = [trk for trk in tracks_events if [ev for ev in trk if ev[1] == 'note' and ev[3] > 0]]
            if not valid_tracks:
                messagebox.showwarning("Aviso", "El archivo MIDI no contiene notas.")
                return

            base_name = os.path.splitext(os.path.basename(midi_path))[0][:8].upper()
            output_dir = os.path.dirname(midi_path)

            seq_bytes_active = create_sequence_bytes(valid_tracks[0], division)
            empty_seq_bytes = create_sequence_bytes([], division)

            with open(os.path.join(output_dir, f"{base_name}.SEQ"), 'wb') as f:
                f.write(seq_bytes_active + (empty_seq_bytes * 4))

            n_sequences = [create_sequence_bytes(valid_tracks[i], division) if i < len(valid_tracks) else empty_seq_bytes for i in range(N_MAX_SEQS)]
            pat_name_bytes = base_name.ljust(10).encode('latin1')[:10]

            with open(os.path.join(output_dir, f"{base_name}.PAT"), 'wb') as f:
                f.write(b''.join(n_sequences) + pat_name_bytes + (empty_seq_bytes * (3 * N_MAX_SEQS)))

            messagebox.showinfo("Éxito", f"Convertido con éxito:\n1. {base_name}.SEQ\n2. {base_name}.PAT")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    # --- PESTAÑA 4: CONVERTIR SEQ/PAT A MIDI ---
    def setup_tab_midi_out(self):
        ttk.Label(self.tab_midi_out, text="Exportar archivos de Arduino (.SEQ / .PAT) a canciones MIDI (.mid)", font=("Arial", 10)).pack(pady=20)
        btn = ttk.Button(self.tab_midi_out, text="📁 Seleccionar SEQ/PAT y Exportar", command=self.exportar_midi)
        btn.pack(pady=10, ipadx=10, ipady=6)

    def exportar_midi(self):
        file_path = filedialog.askopenfilename(filetypes=[("Archivos Arduino", "*.PAT;*.SEQ")])
        if not file_path: return
        ext = os.path.splitext(file_path)[1].upper()
        base_name = os.path.splitext(os.path.basename(file_path))[0]
        output_dir = os.path.dirname(file_path)

        with open(file_path, "rb") as f: data = f.read()

        tracks = []
        if ext == ".SEQ":
            for seq_i in range(min(5, len(data) // BYTES_PER_SEQUENCE)):
                events = parse_sequence_bytes(data[seq_i * BYTES_PER_SEQUENCE : (seq_i + 1) * BYTES_PER_SEQUENCE], ch_default=seq_i)
                tracks.append(events)
            out_file = os.path.join(output_dir, f"{base_name}_Subdivisiones.mid")
        else:
            offset = 0
            for seq_i in range(5):
                events = parse_sequence_bytes(data[offset : offset + BYTES_PER_SEQUENCE], ch_default=seq_i)
                offset += BYTES_PER_SEQUENCE
                tracks.append(events)
            out_file = os.path.join(output_dir, f"{base_name}_Pattern_Subdivisiones.mid")

        write_multitrack_midi(out_file, tracks)
        messagebox.showinfo("Éxito", f"¡MIDI exportado con éxito!\nGuardado en:\n{out_file}")

    # --- PESTAÑA 5: INSPECTOR DE SECUENCIAS ---
    def setup_tab_inspector(self):
        btn = ttk.Button(self.tab_inspector, text="📁 Leer y Analizar Archivo .SEQ", command=self.inspeccionar_seq)
        btn.pack(pady=10)

        self.txt_inspector = scrolledtext.ScrolledText(self.tab_inspector, width=70, height=18, font=("Consolas", 8))
        self.txt_inspector.pack(padx=10, pady=5)

    def inspeccionar_seq(self):
        file_path = filedialog.askopenfilename(filetypes=[("Archivos SEQ", "*.SEQ")])
        if not file_path: return

        self.txt_inspector.delete("1.0", tk.END)
        self.txt_inspector.insert(tk.END, f"--- INSPECCIONANDO: {os.path.basename(file_path)} ---\n\n")

        with open(file_path, "rb") as f:
            FORMATO_STEP = "<BBBBBBBB"
            for bloque_idx in range(5):
                etiqueta = "SECUENCIA ACTIVA" if bloque_idx == 0 else f"SLOT DE MEMORIA {bloque_idx}"
                self.txt_inspector.insert(tk.END, f"=== {etiqueta} ===\n")

                for i in range(N_MAX_STEPS):
                    raw_step = f.read(struct.calcsize(FORMATO_STEP))
                    if len(raw_step) < struct.calcsize(FORMATO_STEP): break
                    unpacked = struct.unpack(FORMATO_STEP, raw_step)
                    bitfield = unpacked[4]
                    mute = bool(bitfield & 0x01)
                    octave = (bitfield >> 2) & 0x0F
                    self.txt_inspector.insert(tk.END, f"Step {i+1:02d} | Nota: {unpacked[0]:3d} | Vel: {unpacked[2]:3d} | CC: {unpacked[1]:3d} | Oct: {octave} | Mute: {mute}\n")
                
                f.seek(N_MAX_STEPS * struct.calcsize(FORMATO_STEP) + 16, 1)
                self.txt_inspector.insert(tk.END, "\n")

if __name__ == "__main__":
    root = tk.Tk()
    app = AppSecuenciador(root)
    root.mainloop()