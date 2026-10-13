from pathlib import Path
r=Path(__file__).resolve().parents[1]
height=258
p=['fonts { font: "EJamzLCD.ttf" "EJamzLCD"; } images { image: "button.png" COMP; } collections { group { name: "ejamz/player"; min: 480 258; data { item: "window.alpha" "1"; } parts {']
def box(name,x,y,w,h,color,kind='RECT',more=''):
 p.append(f'part {{ name: "{name}"; type: {kind}; mouse_events: 0; description {{ state: "default" 0.0; rel1 {{ relative: {x/480} {y/height}; }} rel2 {{ relative: {(x+w)/480} {(y+h)/height}; }} color: {color}; {more} }} }}')
def label(name,x,y,w,h,value,size,color='200 222 236 255'):
 box(name,x,y,w,h,color,'TEXT',f'text {{ text: "{value}"; font: "DejaVu Sans"; size: {size}; align: 0.0 0.5; }}')

def event(name,x,y,w,h):
 p.append(f'part {{ name: "event.{name}"; type: RECT; description {{ state: "default" 0.0; rel1 {{ relative: {x/480} {y/height}; }} rel2 {{ relative: {(x+w)/480} {(y+h)/height}; }} color: 0 0 0 0; }} }}')
def button(name,x,y,w,h,value):
 box('button.'+name,x,y,w,h,'255 255 255 255','IMAGE','image { normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.035; }')
 label('label.'+name,x+9,y+2,w-9,h-4,value,11)
 event(name,x,y,w,h)
def chrome():
 event('drag',8,3,307,27)
 button('settings',320,4,76,23,'SETTINGS')
 button('minimize',403,4,29,23,'_')
 button('close',439,4,29,23,'X')

box('body',0,0,480,258,'255 255 255 255','IMAGE','image { normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.08; }')
box('inner',12,32,456,214,'16 39 59 245')
box('header',2,2,476,29,'255 255 255 255','IMAGE','image { normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.035; }')
label('brand',14,4,300,25,'E-JAMZ  /  STEREO PLAYER',13)
box('lcd.frame',16,40,448,94,'255 255 255 255','IMAGE','image { normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.065; }')
box('lcd',25,49,430,76,'189 214 228 255')
label('format',28,47,420,20,'LOCAL AUDIO',10,'50 90 110 255')
label('title',28,69,290,28,'ADD MUSIC',28,'23 60 82 255')
label('status',28,100,420,24,'Stopped',12,'50 90 110 255')
label('time',355,69,100,28,'00:00',28,'23 60 82 255')
for name,x,w,value in [('previous',18,45,'<<'),('play',70,51,'>'),('pause',128,45,'||'),('stop',180,45,'[]'),('next',232,45,'>>')]:
 box('button.'+name,x,164,w,35,'255 255 255 255','IMAGE','image { normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.035; }')
 label('label.'+name,x+10,169,w-10,25,value,16)
 p.append(f'part {{ name: "event.{name}"; type: RECT; description {{ state: "default" 0.0; rel1 {{ relative: {x/480} {164/258}; }} rel2 {{ relative: {(x+w)/480} {199/258}; }} color: 0 0 0 0; }} }}')
for name,x,y,w,h in [('seek',16,133,448,25),('volume',300,160,150,39)]:
 box('swallow.'+name,x,y,w,h,'255 255 255 255','SWALLOW')
for name,x,w,value in [('open',18,140,'ADD MUSIC'),('queue',170,140,'PLAYLIST'),('rack',322,140,'EQUALIZER')]:
 box('button.'+name,x,212,w,29,'255 255 255 255','IMAGE','image { normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.035; }')
 label('label.'+name,x+12,215,w-12,23,value,10)
 p.append(f'part {{ name: "event.{name}"; type: RECT; description {{ state: "default" 0.0; rel1 {{ relative: {x/480} {212/258}; }} rel2 {{ relative: {(x+w)/480} {241/258}; }} color: 0 0 0 0; }} }}')
for name,x in [('queue',298),('rack',450)]:
 p.append(f'part {{ name: "led.{name}"; type: RECT; mouse_events: 0; description {{ state: "default" 0.0; rel1 {{ relative: {x/480} {222/258}; }} rel2 {{ relative: {(x+4)/480} {231/258}; }} color: 43 73 93 255; }} description {{ state: "active" 0.0; inherit: "default" 0.0; color: 165 232 255 255; }} }}')
chrome()
p.append('} programs {')
for name in ['queue','rack']:
 for state,description in [('on','active'),('off','default')]:
  p.append(f'program {{ name: "{name}.{state}"; signal: "panel,{name},{state}"; source: "app"; action: STATE_SET "{description}" 0.0; target: "led.{name}"; }}')
p.append('} }')
for group,title,group_height in [('queue','E-JAMZ  /  PLAYLIST',344),('rack','E-JAMZ  /  EQUALIZER',220)]:
 height=group_height
 p.append(f'group {{ name: "ejamz/{group}"; min: 480 {height}; parts {{')
 box('body',0,0,480,height,'255 255 255 255','IMAGE','image { normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.08; }')
 box('inner',12,32,456,height-44,'16 39 59 245')
 box('header',2,2,476,29,'255 255 255 255','IMAGE','image { normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.035; }')
 label('brand',14,4,300,25,title,13)
 if group=='queue':
  box('summary.lcd',18,40,444,27,'189 214 228 255')
  label('summary',28,41,158,25,'QUEUE EMPTY',11,'23 60 82 255')
  box('filter.lcd',190,40,272,27,'189 214 228 255')
  box('swallow.filter',190,40,272,27,'0 0 0 0','SWALLOW')
  for i in range(8):
   yy=74+i*28
   box(f'row.background.{i}',18,yy,444,26,'13 32 49 255')
   label(f'row.{i}',27,yy,430,26,'',13)
   event(f'row.{i}',18,yy,444,26)
  button('back',18,307,48,26,'<')
  button('forward',78,307,48,26,'>')
  button('open',138,307,96,26,'+ MUSIC')
  button('save',246,307,104,26,'SAVE LIST')
  button('load',362,307,100,26,'LOAD LIST')
 else:
  label('eq.status',23,37,105,23,'10 BAND EQ',11)
  for i,value in enumerate(['29','59','119','237','474','947','1.9k','3.8k','7.5k','15k']):
   xx=28+i*46
   box(f'track.{i}',xx-11,66,30,114,'0 0 0 0')
   box(f'rail.{i}',xx,66,8,114,'6 20 32 255')
   p.append(f'part {{ name: "eq.{i}"; type: IMAGE; dragable {{ x: 0 0 0; y: 1 1 0; confine: "track.{i}"; }} description {{ state: "default" 0.0; min: 30 18; max: 30 18; rel1 {{ relative: {xx/480} {66/height}; offset: -10 0; }} rel2 {{ relative: {xx/480} {66/height}; offset: 19 17; }} image {{ normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.02; }} }} }}')
   for suffix,color,r1,r2,offsets in [('face','181 216 235 220','0.0 0.0','1.0 1.0','2 2 -3 -3'),('shine','238 250 255 255','0.0 0.0','1.0 0.0','3 2 -4 3'),('grip','37 79 106 255','0.0 0.5','1.0 0.5','5 0 -6 1')]:
    a,b,c,d=offsets.split()
    p.append(f'part {{ name: "eq.{suffix}.{i}"; type: RECT; mouse_events: 0; description {{ state: "default" 0.0; rel1 {{ to: "eq.{i}"; relative: {r1}; offset: {a} {b}; }} rel2 {{ to: "eq.{i}"; relative: {r2}; offset: {c} {d}; }} color: {color}; }} }}')
   for ty in (67,104,141,178):
    box(f'tick.{i}.{ty}',xx+10,ty,4,1,'112 152 177 255')
   label(f'frequency.{i}',xx-10,183,41,17,value,10)
  button('flat',140,35,68,26,'RESET')
  button('presets',220,35,146,26,'FLAT v')
  button('bypass',378,35,84,26,'EQ: ON')
 chrome()
 p.append('} }')
height=504
p.append('group { name: "ejamz/settings"; min: 480 504; parts {')
box('body',0,0,480,height,'255 255 255 255','IMAGE','image { normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.08; }')
# Native settings text uses dark ink on this theme-owned LCD surface.
box('inner',12,32,456,height-44,'189 214 228 255')
box('header',2,2,476,29,'255 255 255 255','IMAGE','image { normal: "button.png"; border: 160 160 160 160; border_scale: 1; border_scale_by: 0.035; }')
label('brand',18,4,370,25,'E-JAMZ / SETTINGS',13)
event('drag',8,3,410,27)
button('close',439,4,29,23,'X')
box('swallow.settings',18,40,444,450,'255 255 255 255','SWALLOW')
p.append('} }')
p.append('}')
# Compact default preserves relative layout and readable minimum text sizes.
import re
edc='\n'.join(p)+'\n'
edc=re.sub(r'(name: "(?:title|time)";.*?font:) "DejaVu Sans"',r'\1 "EJamzLCD"',edc)
edc=edc.replace('type: SWALLOW; mouse_events: 0;','type: SWALLOW; mouse_events: 1;')
edc=re.sub(r'(name: "ejamz/[^"]+"; min:) 480 (\d+);',lambda m:m[1]+' 400 '+str(round(int(m[2])*5/6))+';',edc)
edc=re.sub(r'(?<![\w-])size: (\d+);',lambda m:'size: '+str(max(10,round(int(m[1])*.86)))+';',edc)
edc=edc.replace('min: 30 18; max: 30 18;','min: 25 15; max: 25 15;')
edc=re.sub(r'border_scale_by: ([0-9.]+);',lambda m:'border_scale_by: '+str(float(m[1])*5/6)+';',edc)
edc=edc.replace('name: "ejamz/settings"; min: 400 420;', 'name: "ejamz/settings"; min: 280 420;')
# Keep drawer chrome fixed in pixels when the playlist changes window height.
start=edc.index('group { name: "ejamz/settings"')
head,drawer=edc[:start],edc[start:]
parts=drawer.split('part { name:')
bounds={'inner':(27,-7,1),'header':(2,25,0),'brand':(3,23,0),
        'event.drag':(3,24,0),'button.close':(3,23,0),
        'label.close':(5,21,0),'event.close':(3,23,0),
        'swallow.settings':(33,-10,1)}
for i in range(1,len(parts)):
 name=parts[i].split('"')[1]
 if name not in bounds:continue
 top,bottom,anchor=bounds[name]
 parts[i]=re.sub(r'rel1 { relative: ([0-9.]+) [0-9.]+; }',lambda m:'rel1 { relative: '+m[1]+' 0; offset: 0 '+str(top)+'; }',parts[i],count=1)
 parts[i]=re.sub(r'rel2 { relative: ([0-9.]+) [0-9.]+; }',lambda m:'rel2 { relative: '+m[1]+' '+str(anchor)+'; offset: -1 '+str(bottom)+'; }',parts[i],count=1)
edc=head+'part { name:'.join(parts)
# Color classes permit instantaneous per-object palette changes without replacing layouts.
colors=sorted(set(re.findall(r'color: (\d+ \d+ \d+ \d+);',edc)))
def color_class(m):
 c=m[1]; return 'color: 255 255 255 255; color_class: "ejamz.'+c.replace(' ','_')+'";'
edc=re.sub(r'color: (\d+ \d+ \d+ \d+);',color_class,edc)
classes='color_classes { '+''.join('color_class { name: "ejamz.'+c.replace(' ','_')+'"; color: '+c+'; }' for c in colors)+' }\n'
(r/'src/ui/player.edc').write_text(classes+edc)
(r/'src/ui/theme-palette.h').write_text('/* Generated by tools/generate-player-edc.py */\nstatic const struct { const char *name; int r,g,b,a; } palette[] = {\n'+''.join('    {"ejamz.'+c.replace(' ','_')+'",'+c.replace(' ', ',')+'},\n' for c in colors)+'};\n')
