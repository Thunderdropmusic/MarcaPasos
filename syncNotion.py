import os
import requests

# Variables de entorno
token = os.environ['NOTION_TOKEN']
page_id = os.environ['NOTION_MEMORIA_ID']

headers = {
    "Authorization": f"Bearer {token}",
    "Notion-Version": "2022-06-28"
}

def download_image(url, block_id):
    if not os.path.exists("docs/assets"):
        os.makedirs("docs/assets", exist_ok=True)
    
    filename = f"img_{block_id}.png"
    path = os.path.join("docs/assets", filename)
    
    r = requests.get(url, stream=True)
    if r.status_code == 200:
        with open(path, 'wb') as f:
            for chunk in r:
                f.write(chunk)
        return f"docs/assets/{filename}"
    return None

def get_text(block):
    b_type = block['type']
    
    # Manejo de bloques que son enlaces a páginas hijos
    if b_type == 'child_page':
        title = block['child_page'].get('title', 'Página sin título')
        clean_id = block['id'].replace('-', '')
        return f"📄 [{title}](https://www.notion.so/{clean_id})"

    rich_text = block[b_type].get('rich_text', [])
    text = ""
    for rt in rich_text:
        plain = rt['plain_text']
        href = rt.get('href')
        
        # Lógica para Menciones de Páginas (@página)
        if rt.get('type') == 'mention' and 'page' in rt['mention']:
            page_id_mention = rt['mention']['page']['id'].replace('-', '')
            href = f"https://www.notion.so/{page_id_mention}"
            plain = f"📄 {plain}" # Añadimos un icono para que sepa que es una página

        # Aplicamos estilos si no es un bloque de código
        if b_type != 'code':
            if rt.get('annotations', {}).get('bold'): plain = f"**{plain}**"
            if rt.get('annotations', {}).get('italic'): plain = f"*{plain}*"
            
            # Gestión de hipervínculos
            if href:
                if href.startswith('/'): # Link interno de Notion
                    href = f"https://www.notion.so{href}"
                plain = f"[{plain}]({href})"
        
        text += plain
    return text

def process_blocks(block_id):
    markdown = ""
    url = f"
