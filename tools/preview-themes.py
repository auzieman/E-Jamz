"""Code-native palette study; original texture remains unchanged."""
from pathlib import Path
import subprocess
r=Path(__file__).resolve().parents[1]
ns={'__file__':str(r/'tools/render-preview.py')}
exec((r/'tools/render-preview.py').read_text().split("out = ROOT / 'docs/images'")[0],ns)
body=''
for i,(name,lcd,ink,inner,mod) in enumerate([
 ('Blue Steel','#bdd6e4','#173c52','#10273b',(1,1,1)),
 ('Midnight','#aad3c6','#13312b','#262626',(.55,.63,.71)),
 ('Amber Steel','#eecf8c','#422b11','#262626',(.92,.80,.65))]):
 x=i*420
 filt=f'<filter id="t{i}" color-interpolation-filters="sRGB"><feColorMatrix type="matrix" values="{mod[0]} 0 0 0 0 0 {mod[1]} 0 0 0 0 0 {mod[2]} 0 0 0 0 0 1 0"/></filter>'
 body+=filt+f'<g transform="translate({x} 0)">'
 body+=f'<g filter="url(#t{i})">'+ns['material'](0,0,400,215)+'</g>'
 body+=ns['rect'](10,27,380,178,inner)+ns['text'](12,20,name,12)
 body+=ns['text'](280,20,'SETTINGS  _  X',10)
 body+=ns['rect'](20,40,360,64,lcd)+ns['text'](24,56,'LOCAL AUDIO',10,ink)
 body+=ns['text'](24,80,'DEREZZED',22,ink)+ns['text'](294,80,'02:34',22,ink)
 body+=ns['text'](24,99,'Playing',10,ink)
 for xx,txt in [(16,'<<'),(60,'>'),(110,'||'),(155,'[]'),(197,'>>')]:
  body+=f'<g filter="url(#t{i})">'+ns['material'](xx,138,38,28)+'</g>'+ns['text'](xx+10,157,txt,12)
 body+=ns['text'](255,156,'VOLUME  ━━━',11)
 for xx,txt in [(15,'ADD MUSIC'),(144,'PLAYLIST'),(270,'EQUALIZER')]:
  body+=f'<g filter="url(#t{i})">'+ns['material'](xx,180,115,24)+'</g>'+ns['text'](xx+8,196,txt,10)
 body+='</g>'
p=r/'docs/images/ej012-theme-study.svg';p.write_text(ns['document'](1240,215,body))
subprocess.run(['rsvg-convert',str(p),'-o',str(p.with_suffix('.png'))],check=True)
