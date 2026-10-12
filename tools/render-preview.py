"""Editable stereo-layout reference; original control texture embedded unchanged."""
import base64
from html import escape
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
TEXTURE = base64.b64encode((ROOT / 'assets/skins/blue-steel/assets/button.png').read_bytes()).decode()

def text(x, y, value, size=14, color='#c8deec', extra=''):
    return f'<text x="{x}" y="{y}" font-size="{size}" fill="{color}" {extra}>{escape(value)}</text>'

def rect(x, y, w, h, fill, stroke='none', radius=0):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{radius}" fill="{fill}" stroke="{stroke}"/>'

def material(x, y, w, h):
    # Nine-slice drawing preserves bevel corners; no raster resampling/edit output.
    result = ''
    source = [0, 160, 1120, 1280]
    dx = [x, x + 6, x + w - 6, x + w]
    dy = [y, y + 6, y + h - 6, y + h]
    for row in range(3):
        for col in range(3):
            a, b = source[col], source[row]
            sw, sh = source[col + 1] - a, source[row + 1] - b
            result += (f'<svg x="{dx[col]}" y="{dy[row]}" width="{dx[col+1]-dx[col]}" '
                       f'height="{dy[row+1]-dy[row]}" viewBox="{a} {b} {sw} {sh}" preserveAspectRatio="none">'
                       f'<use href="#texture"/></svg>')
    return result

def panel(x, y, w, h, title, index):
    result = material(x, y, w, h)
    result += rect(x + 10, y + 32, w - 20, h - 44, '#10273b', '#416d88', 3)
    for sx in (x + 7, x + w - 7):
        for sy in (y + 7, y + h - 7):
            result += f'<circle cx="{sx}" cy="{sy}" r="2.4" fill="#b4cedc" stroke="#07121c"/>'
    result += material(x + 2, y + 2, w - 4, 29)
    result += text(x + 14, y + 22, title, 13, '#e4f4fc', 'font-weight="bold" letter-spacing="1.4"')
    result += text(x + w - 37, y + 21, index, 11, '#9bbcd2')
    return result

def button(x, y, w, label, focused=False, disabled=False):
    value = material(x, y, w, 35)
    if focused:
        value += rect(x - 2, y - 2, w + 4, 39, 'none', '#a7e9ff', 3)
    value += text(x + w / 2, y + 23, label, 13,
                  '#6e8694' if disabled else '#e1f4ff', 'text-anchor="middle"')
    return value

def player(x, y):
    s = panel(x, y, 480, 224, 'E-JAMZ  /  STEREO PLAYER', '01')
    s += material(x + 14, y + 40, 452, 94)
    s += rect(x + 22, y + 47, 436, 80, 'url(#lcd)', '#4a7086', 2)
    for line_y in range(y + 49, y + 126, 3):
        s += f'<path d="M{x+24} {line_y}h432" stroke="#294b61" stroke-opacity=".06"/>'
    s += text(x + 28, y + 63, 'LOCAL AUDIO    •    FLAC / 44.1 kHz', 10, '#3a6276', 'letter-spacing="1"')
    s += text(x + 28, y + 88, 'Night Drive', 23, '#173c52', 'font-weight="bold"')
    s += text(x + 28, y + 112, 'Demo Artist  /  Blue Hour', 12, '#345c70')
    s += text(x + 450, y + 88, '02:34', 25, '#173c52', 'text-anchor="end" font-family="monospace"')
    s += text(x + 450, y + 113, 'PLAYING    −01:46', 11, '#345c70', 'text-anchor="end"')
    s += rect(x + 18, y + 142, 444, 5, '#07121e', '#2a5673', 2)
    s += rect(x + 18, y + 142, 264, 5, '#8bcde8', radius=2)
    s += rect(x + 278, y + 137, 7, 15, '#c9e7f6', '#477a97', 2)
    for offset, label, width in [(18, '◀◀', 45), (70, '▶', 51), (128, 'Ⅱ', 45), (180, '■', 45), (232, '▶▶', 45)]:
        s += button(x + offset, y + 164, width, label, focused=label == '▶')
    s += text(x + 304, y + 177, 'VOLUME', 9, '#9bb9cc')
    s += rect(x + 304, y + 187, 140, 5, '#061422', '#315776', 2)
    s += rect(x + 304, y + 187, 92, 5, '#95cce2', radius=2)
    s += text(x + 20, y + 216, 'UP NEXT    SOUND    INFO', 10, '#b6d2e5', 'letter-spacing="1.2"')
    s += text(x + 459, y + 216, 'SHUFFLE OFF   /   REPEAT OFF', 9, '#8aaabd', 'text-anchor="end"')
    return s

def rack(x, y):
    s = panel(x, y, 480, 207, 'SOUND RACK', '02')
    s += text(x + 18, y + 53, 'PRESET  /  FLAT', 11, '#b6d6e7')
    s += text(x + 461, y + 53, 'BYPASS ○', 11, '#a5d5eb', 'text-anchor="end"')
    for i, label in enumerate(['PRE', '31', '62', '125', '250', '500', '1k', '2k', '4k', '8k', '16k']):
        xx = x + 27 + i * 42
        s += rect(xx, y + 70, 4, 79, '#051320', '#355570', 2)
        s += rect(xx - 6, y + 104, 16, 9, '#b6d7e9', '#507a93', 2)
        s += text(xx + 2, y + 167, label, 9, '#9dbed0', 'text-anchor="middle"')
    s += text(x + 19, y + 192, 'BASS  0 dB      TREBLE  0 dB      BALANCE  CENTER', 10, '#b6cede')
    return s

def queue(x, y):
    s = panel(x, y, 480, 209, 'UP NEXT  /  PLAYLIST', '03')
    s += text(x + 18, y + 52, 'BLUE HOUR', 11, '#92c5df', 'letter-spacing="1"')
    s += text(x + 461, y + 52, '4 TRACKS  •  17:42', 10, '#8dafc4', 'text-anchor="end"')
    for i, (name, duration) in enumerate([('Night Drive', '4:20'), ('Coastal Lights', '5:04'), ('After Midnight', '3:38'), ('Home Again', '4:40')]):
        yy = y + 65 + i * 27
        s += rect(x + 13, yy, 454, 25, '#264d65' if i == 0 else '#0b2031', radius=2)
        s += text(x + 24, yy + 17, '▶' if i == 0 else f'{i+1:02}', 11, '#a7dcf0')
        s += text(x + 56, yy + 17, name, 12)
        s += text(x + 453, yy + 17, duration, 11, '#99bace', 'text-anchor="end"')
    s += text(x + 20, y + 194, '+ ADD     REMOVE     SAVE', 10, '#b5d3e5')
    s += text(x + 460, y + 194, 'CHAIN PLAYLIST…', 10, '#b5d3e5', 'text-anchor="end"')
    return s

def info(x, y):
    s = panel(x, y, 240, 435, 'TRACK INFO', '04')
    s += rect(x + 22, y + 48, 196, 153, '#173548', '#355e78', 3)
    s += text(x + 120, y + 123, 'BLUE HOUR', 18, '#b5d9e8', 'text-anchor="middle" letter-spacing="2"')
    s += text(x + 120, y + 148, 'ARTWORK PLACEHOLDER', 9, '#769aac', 'text-anchor="middle"')
    for dy, value, size, color in [(231, 'Night Drive', 20, '#deedf6'), (255, 'Demo Artist', 13, '#adc7d7'),
                                   (279, 'Blue Hour  ·  2026', 12, '#8aaebf'), (316, 'FLAC  /  stereo  /  44.1 kHz', 11, '#9bb9ca'),
                                   (352, 'DETAILS    LYRICS', 11, '#b2d8ec'), (382, 'No local lyrics attached.', 12, '#8ea9bc'),
                                   (412, 'Optional dock · follows current track', 10, '#7295a9')]:
        s += text(x + 22, y + dy, value, size, color)
    return s

def document(width, height, body):
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">'
            '<defs><linearGradient id="lcd" x2="0" y2="1"><stop stop-color="#d6e5ed"/><stop offset="1" stop-color="#94b9ce"/></linearGradient>'
            f'<image id="texture" width="1280" height="1280" href="data:image/png;base64,{TEXTURE}"/></defs>'
            '<g font-family="DejaVu Sans, sans-serif">' + rect(0, 0, width, height, '#08131f') + body + '</g></svg>')

out = ROOT / 'docs/images'
out.mkdir(exist_ok=True)
body = text(30, 36, 'E-JAMZ', 22, '#e0f0f9', 'letter-spacing="4"')
body += text(30, 58, 'Stereo concept 02  /  editable layout preview', 12, '#8eacbe')
body += player(30, 80) + rack(30, 320) + queue(30, 543) + info(530, 80)
body += text(532, 555, 'Independent companion panels', 14, '#bdd6e5')
body += text(532, 583, 'Player remains useful on its own.', 12, '#8caabc')
body += text(532, 608, 'No feeds. No required information dock.', 12, '#8caabc')
body += text(30, 790, 'Illustrative metadata and states · controls not yet wired · existing original blue-steel material', 10, '#7796ab')
for name, svg in [('ejamz-stereo-stack-v2', document(800, 810, body)),
                  ('ejamz-compact-v2', document(520, 264, player(20, 20)))]:
    path = out / (name + '.svg')
    path.write_text(svg)
    subprocess.run(['rsvg-convert', str(path), '-o', str(path.with_suffix('.png'))], check=True)
    print(path.relative_to(ROOT))
