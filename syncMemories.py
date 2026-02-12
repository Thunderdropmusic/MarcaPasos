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
    # Crea la ruta de carpetas si no existe
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
    rich_text = block[b_type].get('rich_text', [])
    text = ""
    for rt in rich_text:
        plain = rt['plain_text']
        if rt.get('annotations', {}).get('bold'): plain = f"**{plain}**"
        if rt.get('annotations', {}).get('italic'): plain = f"*{plain}*"
        text += plain
    return text

def process_blocks(block_id):
    markdown = ""
    url = f"https://api.notion.com/v1/blocks/{block_id}/children"
    data = requests.get(url, headers=headers).json()

    for block in data.get('results', []):
        b_type = block['type']
        line = ""
        
        if b_type == 'paragraph':
            line = f"{get_text(block)}\n\n"
        elif b_type == 'heading_1':
            line = f"# {get_text(block)}\n\n"
        elif b_type == 'heading_2':
            line = f"## {get_text(block)}\n\n"
        elif b_type == 'image':
                    image_data = block['image']
                    img_url = image_data['file']['url'] if 'file' in image_data else image_data['external']['url']
                    local_path = download_image(img_url, block['id'])
                    
                    caption_list = image_data.get('caption', [])
                    caption_text = "".join([c.get('plain_text', '') for c in caption_list])

                    if local_path:
                        # Usamos un div para agrupar imagen y pie sin separarlos
                        line = f'<div style="margin-bottom: 20px;">'
                        line += f'<img src="{local_path}" width="400" style="display: block; margin-bottom: 5px;" alt="{caption_text}" />'
                        
                        if caption_text:
                            # El pie de foto sale justo debajo, sin apenas margen superior
                            line += f'<p style="margin-top: 0px;"><sub><i>{caption_text}</i></sub></p>'
                        
                        line += '</div>\n\n'
        elif b_type == 'bulleted_list_item':
            line = f"* {get_text(block)}\n"
        
        markdown += line
    return markdown

def update_memoria():
    print("⏳ Obteniendo contenido de Notion...")
    notion_markdown = process_blocks(page_id)
    file_path = "memoria.md"
    
    # Escribimos (o sobreescribimos) el archivo directamente con el contenido de Notion
    with open(file_path, "w", encoding="utf-8") as f:
        f.write(notion_markdown)
    
    print(f"✅ Memoria actualizada por completo en {file_path}")

if __name__ == "__main__":
    update_memoria()