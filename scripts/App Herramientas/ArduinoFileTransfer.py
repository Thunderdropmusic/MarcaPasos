import os
import time
import struct
import tkinter as tk
from tkinter import filedialog, messagebox, simpledialog
import serial
import serial.tools.list_ports

CMD_START_TRANSFER = 0xAA
CMD_ACK            = 0x06
CMD_ERROR          = 0x15

def enviar_a_arduino():
    root = tk.Tk()
    root.withdraw()

    puertos = [p.device for p in serial.tools.list_ports.comports()]
    if not puertos:
        messagebox.showerror("Error", "No se detectó ningún Arduino conectado por USB.")
        return

    puerto_com = puertos[0]
    if len(puertos) > 1:
        puerto_com = simpledialog.askstring("Puerto", f"Puertos: {puertos}\nIntroduce el puerto:", initialvalue=puertos[1])
        if not puerto_com:
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

    if not messagebox.askyesno("Aviso", "Se va a reiniciar el Arduino para enviar el archivo automáticamente. ¿Continuar?"):
        return

    print(f"Conectando a {puerto_com}...")
    try:
        ser = serial.Serial()
        ser.port = puerto_com
        ser.baudrate = 115200
        ser.timeout = 5
        ser.dsrdtr = False
        ser.rtscts = False
        ser.open()
        ser.setDTR(False)
    except Exception as e:
        messagebox.showerror("Error de Conexión", f"No se pudo conectar a {puerto_com}:\n{e}")
        return

    # Damos tiempo a que el Arduino Mega termine de arrancar el bootloader (dentro de los 6s de ventana)
    time.sleep(2.2)

    print("Enviando archivo...")
    try:
        ser.reset_input_buffer()

        # Mandamos la cabecera
        ser.write(bytes([CMD_START_TRANSFER]))
        time.sleep(0.05)
        ser.write(bytes([tipo_guardado]))
        time.sleep(0.05)
        ser.write(nombre_base.encode('ascii'))
        time.sleep(0.05)
        ser.write(struct.pack(">H", tamano_total))
        ser.flush()

        # Esperar ACK de cabecera
        ack_cabecera = ser.read(1)
        if not ack_cabecera or ack_cabecera[0] != CMD_ACK:
            messagebox.showerror("Error", "El Arduino no respondió a la cabecera (posible timeout de sincronización).")
            ser.close()
            return

        print(f"Transmitiendo {tamano_total} bytes...")
        CHUNK_SIZE = 32
        for i in range(0, tamano_total, CHUNK_SIZE):
            bloque = datos[i : i + CHUNK_SIZE]
            ser.write(bloque)
            ser.flush()
            time.sleep(0.03) # Pausa segura para la SD

        print("Esperando confirmación de escritura en la SD...")
        time.sleep(0.8) 
        
        ser.timeout = 3.0
        ack_final = ser.read(1)
        ser.close()

        if ack_final and ack_final[0] == CMD_ACK:
            messagebox.showinfo("¡Éxito!", f"¡Archivo {ext} enviado y guardado correctamente en la SD!")
        else:
            code_received = f"0x{ack_final[0]:02X}" if ack_final else "Ninguno (Timeout)"
            messagebox.showerror("Error", f"Fallo al confirmar la escritura final en la SD.\nCódigo recibido: {code_received}")

    except Exception as err:
        messagebox.showerror("Error", f"Ocurrió un problema durante el envío:\n{err}")
        if ser.is_open:
            ser.close()

if __name__ == "__main__":
    enviar_a_arduino()