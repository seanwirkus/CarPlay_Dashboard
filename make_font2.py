import sys
from PIL import Image, ImageDraw, ImageFont
import math

def generate_font_c_array(font_path, font_size, name):
    try:
        font = ImageFont.truetype(font_path, font_size)
    except:
        font = ImageFont.load_default()

    max_w = int(math.ceil(font.getlength("W"))) + 2
    max_h = font_size + 4
    bytes_per_row = (max_w + 7) // 8

    out = f'#include "fonts.h"\n\nconst uint8_t {name}_Table[] = {{\n'
    for char_code in range(32, 127):
        char = chr(char_code)
        img = Image.new('1', (max_w, max_h), 0)
        draw = ImageDraw.Draw(img)
        draw.text((1, 0), char, font=font, fill=1)
        
        out += f'    // @{char_code} \'{char}\' ({max_w}x{max_h})\n'
        for y in range(max_h):
            row_bytes = []
            for b in range(bytes_per_row):
                byte_val = 0
                for bit in range(8):
                    x = b * 8 + bit
                    if x < max_w:
                        pixel = img.getpixel((x, y))
                        if pixel > 0:
                            byte_val |= (1 << (7 - bit))
                row_bytes.append(f'0x{byte_val:02X}')
            out += '    ' + ', '.join(row_bytes) + ',\n'
    out += '};\n\n'
    out += f'sFONT {name} = {{\n  {name}_Table,\n  {max_w},\n  {max_h},\n}};\n'
    with open(f'{name.lower()}.cpp', 'w') as f:
        f.write(out)
    print(f"Generated {name}")

generate_font_c_array('/System/Library/Fonts/Supplemental/Futura.ttc', 120, 'Font_Futura120')
generate_font_c_array('/System/Library/Fonts/Supplemental/Futura.ttc', 24, 'Font_Futura24')
