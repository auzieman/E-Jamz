"""Attached drawer geometry preview; native behavior is verified separately."""
from pathlib import Path
import subprocess
r=Path(__file__).resolve().parents[1];ns={'__file__':str(r/'tools/render-preview.py')}
exec((r/'tools/render-preview.py').read_text().split("out = ROOT / 'docs/images'")[0],ns)
b=ns['material'](0,0,400,215)+ns['rect'](10,27,380,178,'#10273b')
b+=ns['text'](12,20,'E-JAMZ / STEREO PLAYER',10)+ns['text'](280,20,'SETTINGS',10)
b+=ns['rect'](20,40,360,64,'#bdd6e4')+ns['text'](25,78,'DEREZZED                  02:34',20,'#173c52')
b+=ns['text'](25,98,'Playing',10,'#325a6e')
for x,t in [(16,'<<'),(65,'>'),(115,'||'),(165,'[]'),(215,'>>')]:b+=ns['material'](x,138,40,28)+ns['text'](x+10,157,t,12)
b+=ns['text'](18,195,'ADD MUSIC        PLAYLIST        EQUALIZER',10)
b+=ns['material'](400,0,280,420)+ns['rect'](407,27,266,385,'#10273b')
b+=ns['text'](411,20,'E-JAMZ / SETTINGS',10)+ns['text'](658,20,'X',10)
for y,t in [(66,'● Blue Steel'),(101,'○ Midnight'),(136,'○ Amber Steel'),(185,'✓ Show playlist'),(220,'✓ Show equalizer'),(255,'✓ Remember volume and EQ'),(305,'Theme changes apply live.'),(325,'Playback stays manual.')]:b+=ns['text'](422,y,t,12)
b+=ns['material'](420,355,240,30)+ns['text'](482,375,'SAVE & CLOSE',11)
p=r/'docs/images/ej013-drawer-study.svg';p.write_text(ns['document'](680,420,b));subprocess.run(['rsvg-convert',str(p),'-o',str(p.with_suffix('.png'))],check=True)
