from pathlib import Path
import os,subprocess,time,sys
r=Path(__file__).resolve().parents[2];o=r/'build/out/prototype';a=o/'ej013-final';title='E-Jamz drawer check';config=o/'ej013-final-settings.ini'
config.write_text('[view]\ntheme=0\nplaylist=false\nequalizer=false\n[audio]\nremember=true\nvolume=0.08\nbands=0;0;0;0;0;0;0;0;0;0;\nbypass=false\n')
log=(o/'ej013-final-gui.log').open('w')
env=dict(os.environ,ELM_ENGINE='software_x11',ELM_ACCEL='none',EJAMZ_THEME=str(a/'player.edj'),EJAMZ_TITLE=title,EJAMZ_CONFIG_FILE=str(config))
p=subprocess.Popen([str(a/'ejamz'),'/home/auzieman/Music/aminet-openshot/mp3/hum_m089_City_Lights_DB.mp3'],env=env,stdout=log,stderr=log,start_new_session=True)
(o/'ej013-gui.pid').write_text(str(p.pid));print(p.pid,flush=True)
sys.path.insert(0,str(r/'tools'));from x11_support import locate
for i in range(60):
 try:locate(title);break
 except RuntimeError:time.sleep(.25)
else:raise RuntimeError('player did not appear')
def tool(name,*args):subprocess.run(['python3',str(r/'tools'/name),title,*map(str,args)],check=True)
time.sleep(.3);tool('click-window.py',78,150);time.sleep(.7)
tool('click-window.py',293,13);time.sleep(.6)
tool('capture-window.py',r/'docs/images/ej013-drawer-native.png')
print('DRAWER OPEN / PLAYING CAPTURED',flush=True)
