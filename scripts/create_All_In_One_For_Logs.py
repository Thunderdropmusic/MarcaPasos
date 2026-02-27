import os

def condensar_proyecto():
    # Pedir la ruta al usuario
    ruta_carpeta = input("Introduce la ruta de la carpeta de tu proyecto: ").strip()
    
    # Limpiar comillas por si el usuario arrastró la carpeta a la terminal
    ruta_carpeta = ruta_carpeta.replace('"', '').replace("'", "")

    if not os.path.isdir(ruta_carpeta):
        print("Error: La ruta proporcionada no es válida.")
        return
    nombre_carpeta = os.path.basename(ruta_carpeta)
    nombre_salida = nombre_carpeta + "_FULLCODE.txt"
    extensiones_validas = ('.ino', '.h', '.cpp')
    
    print(f"\n--- Analizando archivos en: {ruta_carpeta} ---")
    
    try:
        with open(nombre_salida, 'w', encoding='utf-8') as archivo_final:
            encontrados = 0
            # Listar archivos de la carpeta
            for archivo in sorted(os.listdir(ruta_carpeta)):
                if archivo.endswith(extensiones_validas):
                    print(f"Concatenando: {archivo}")
                    ruta_completa = os.path.join(ruta_carpeta, archivo)
                    
                    # Encabezado estético para la IA
                    archivo_final.write(f"\n{'#'*60}\n")
                    archivo_final.write(f"### ARCHIVO: {archivo}\n")
                    archivo_final.write(f"{'#'*60}\n\n")
                    
                    try:
                        with open(ruta_completa, 'r', encoding='utf-8', errors='ignore') as f:
                            archivo_final.write(f.read())
                            archivo_final.write("\n\n")
                        encontrados += 1
                    except Exception as e:
                        print(f"No se pudo leer {archivo}: {e}")

            if encontrados > 0:
                print(f"\n¡Éxito! Se han unido {encontrados} archivos.")
                print(f"Archivo generado: {os.path.abspath(nombre_salida)}")
            else:
                print("\nNo se encontraron archivos .ino, .h o .cpp en esa carpeta.")
                
    except Exception as e:
        print(f"Error al crear el archivo de salida: {e}")

if __name__ == "__main__":
    condensar_proyecto()
    input("\nPresiona Enter para salir...")