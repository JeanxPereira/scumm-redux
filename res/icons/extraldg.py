import re

# Caminho para o arquivo .h de entrada
input_file = 'MaterialSymbols.h'
# Caminho para o arquivo de saída .txt
output_file = 'icon_names.txt'

# Expressão regular para capturar os nomes dos ícones (linhas que começam com "#define ICON_MS_...")
icon_pattern = re.compile(r'#define\s+(ICON_MS_[A-Za-z0-9_]+)\s+"')

icon_names = []

with open(input_file, 'r', encoding='utf-8') as f:
    for line in f:
        match = icon_pattern.match(line)
        if match:
            icon_names.append(match.group(1))

# Salva os nomes no arquivo de saída
with open(output_file, 'w', encoding='utf-8') as f:
    for name in icon_names:
        f.write(name + '\n')

print(f"{len(icon_names)} nomes de ícones extraídos para {output_file}.")
