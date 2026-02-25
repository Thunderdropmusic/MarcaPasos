import time

# Variables
subDivMode = 2
pulsoClock = 0
totalInitClocks = 23 # Ajustado para que el bucle for entre desde el inicio
tiempoClock2 = 0
tiempoClock = [0] * 256
SIZE_TIEMPOCLOCK = 256 

def loop_simulado(t_actual_micros):
    global pulsoClock, tiempoClock2, totalInitClocks, microsSubdivision
    
    subdivision = 7
    if pulsoClock == 24:
        pulsoClock = 0
    
    pulsoClock += 1
    tiempoClock1 = t_actual_micros
    
    if tiempoClock2 != 0:
        tiempoClock[pulsoClock] = tiempoClock1 - tiempoClock2
    
    # Simulación fiel de tu lógica de bucle
    limite_for = SIZE_TIEMPOCLOCK - totalInitClocks
    sumaTiempos = 0
    for i in range(0, limite_for):
         # Se reinicia en cada iteración según tu código
        sumaTiempos += tiempoClock[i] + tiempoClock[0] * totalInitClocks    
        microsSubdivision = sumaTiempos / subdivision
    
    if totalInitClocks != 0:
        totalInitClocks -= 1
        
    tiempoClock2 = tiempoClock1
    print(f"Pulso: {pulsoClock:2d} | LCD: {microsSubdivision*0.00168:8.4f} | Tiempo Relativo: {microsSubdivision/1000:.0f}ms")

# --- SIMULACIÓN EN TIEMPO REAL ---
print("Iniciando simulación en tiempo real (debería tardar ~480ms)...")
start_sim = time.time()

for _ in range(48):
    # Forzamos una espera real de 20ms (0.02 segundos)
    time.sleep(0.02083) 
    
    # Calculamos los microsegundos transcurridos reales
    micros_reales = int((time.time() - start_sim) * 1000000)
    loop_simulado(micros_reales)

end_sim = time.time()
print(f"\nSimulación terminada en: {(end_sim - start_sim)*1000:.2f} ms")