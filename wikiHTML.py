import markdown
import os
import re

archivo_entrada = 'README.md'
archivo_salida = 'README_SIMPLE.html'

def simplificar_al_maximo(html):
    # 1. Convertir H1 y H2 en simples negritas con salto de línea
    html = re.sub(r'<h1.*?>(.*?)</h1>', r'<p><strong>\1</strong></p>', html)
    html = re.sub(r'<h2.*?>(.*?)</h2>', r'<p><strong>\1</strong></p>', html)
    
    # 2. Eliminar etiquetas de lista pero mantener el contenido
    # Moodle a veces bloquea las etiquetas <ul>/<li> en combinación con links
    html = html.replace('<ul>', '').replace('</ul>', '').replace('<ol>', '').replace('</ol>', '')
    html = html.replace('<li>', '<p>• ').replace('</li>', '</p>')

    # 3. Eliminar CUALQUIER comentario HTML (como los de Notion)
    html = re.sub(r'', '', html, flags=re.DOTALL)

    # 4. Eliminar TODOS los atributos (id, class, etc.)
    html = re.sub(r'\s+([a-zA-Z-]+)=".*?"', '', html)
    
    return html

def ejecutar():
    if not os.path.exists(archivo_entrada): return
    
    with open(archivo_entrada, 'r', encoding='utf-8') as f:
        contenido = f.read()

    # Convertir a HTML muy básico
    html = markdown.markdown(contenido)
    
    # Aplicar la cirugía estética para Moodle
    html_moodle = simplificar_al_maximo(html)

    with open(archivo_salida, 'w', encoding='utf-8') as f:
        f.write(html_moodle)
    
    print(f"✅ Generado: {archivo_salida}")

if __name__ == "__main__":
    ejecutar()