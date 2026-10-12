"""Preview embedded-font styling without installing a desktop font."""
import os,subprocess
from pathlib import Path
r=Path(__file__).resolve().parents[1]
ns={'__file__':str(r/'tools/render-preview.py')}
exec((r/'tools/render-preview.py').read_text().split("out = ROOT / 'docs/images'")[0],ns)
config=r/'build/prototype/font-preview.conf'
config.write_text(f'<fontconfig><include>/etc/fonts/fonts.conf</include><dir>{r}/assets/fonts</dir><cachedir>/tmp/ejamz-font-preview-cache</cachedir></fontconfig>')
body=ns['material'](0,0,400,140)+ns['rect'](16,35,368,72,'#bdd6e4','#4a7086',2)
body+=ns['text'](24,49,'LOCAL AUDIO / DESKTOP OUTPUT',10,'#325a6e')
body+=ns['text'](24,79,'DEREZZED',24,'#173c52','font-family="Doto" font-weight="800"')
body+=ns['text'](375,79,'02:34',24,'#173c52','font-family="Doto" font-weight="800" text-anchor="end"')
body+=ns['text'](24,99,'Playing',10,'#325a6e')
body+=ns['text'](16,126,'LCD typography study · existing blue-steel material',10,'#c8deec')
svg=ns['document'](400,140,body);p=r/'docs/images/ejamz-dot-lcd-preview.svg';p.write_text(svg)
subprocess.run(['rsvg-convert',str(p),'-o',str(p.with_suffix('.png'))],env=dict(os.environ,FONTCONFIG_FILE=str(config)),check=True)
