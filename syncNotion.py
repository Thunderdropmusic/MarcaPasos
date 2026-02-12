import os
import requests

token = os.environ['NOTION_TOKEN']
page_id = os.environ['NOTION_PAGE_ID']

headers = {
    "Authorization": f"Bearer {token}",
    "Notion-Version": "2022-06-28"
}

def get_text(block):
    b_type = block['type']
    rich_text = block[b_type].get('rich_text', [])
    text = ""
    for rt in rich_text:
        plain = rt['plain_text']
        href = rt.get('href')
        annotations = rt.get('annotations', {})

        # Aplicar formato de Notion a Markdown
        if annotations.get('bold'):
            plain = f"**{plain}**"
        if annotations.get('italic'):
            plain = f"*{plain}*"
        
        # Si hay un enlace, comprobamos si es interno o externo
        if href:
            if href.startswith('/'):
                # Si empieza por '/', es un link interno de Notion. 
                # Le añadimos el dominio para que sea clicable desde fuera.
                href = f"https://www.notion.so{href}"
            
            plain = f"[{plain}]({href})"
            
        text += plain
    return text

def process_blocks(block_id, indent=0):
    markdown = ""
    url = f"https://api.notion.com/v1/blocks/{block_id}/children"
    response = requests.get(url, headers=headers)
    data = response.json()
    
    prefix = "  " * indent

    for block in data.get('results', []):
        b_type = block['type']
        has_children = block.get('has_children', False)
        
        line = ""
        if b_type == 'paragraph':
            line = f"{get_text(block)}\n\n"
        elif b_type == 'heading_1':
            line = f"# {get_text(block)}\n\n"
        elif b_type == 'heading_2':
            line = f"## {get_text(block)}\n\n"
        elif b_type == 'bulleted_list_item':
            line = f"{prefix}* {get_text(block)}\n"
        elif b_type == 'numbered_list_item':
            line = f"{prefix}1. {get_text(block)}\n"
        elif b_type == 'to_do':
            check = "x" if block['to_do']['checked'] else " "
            line = f"{prefix}- [{check}] {get_text(block)}\n"
            
        markdown += line
        
        if has_children and b_type != 'child_page':
            markdown += process_blocks(block['id'], indent + 1)
            
    return markdown

def update_readme():
    print("Leyendo contenido de Notion...")
    notion_markdown = process_blocks(page_id)
    
    if not os.path.exists("README.md"):
        print("Error: El archivo README.md no existe.")
        return

    with open("README.md", "r", encoding="utf-8") as f:
        readme = f.read()

    # Marcadores con comillas (aquí en Python)
    start_marker = "<!-- START_NOTION -->"
    end_marker = "<!-- END_NOTION -->"
    
    if start_marker in readme and end_marker in readme:
        header = readme.split(start_marker)[0]
        footer = readme.split(end_marker)[1]
        
        # Reconstrucción limpia
        new_readme = f"{header}{start_marker}\n\n{notion_markdown}\n\n{end_marker}{footer}"
        
        with open("README.md", "w", encoding="utf-8") as f:
            f.write(new_readme)
        print("✅ README actualizado correctamente.")
    else:
        print("❌ Error: No se encontraron los marcadores en el README.")

if __name__ == "__main__":
    update_readme()