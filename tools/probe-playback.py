"""Short decode-to-fakesink probes; no audible output or full-file fast decoding."""
from pathlib import Path
import gi,json,time

gi.require_version('Gst','1.0')
from gi.repository import Gst
Gst.init(None)
root=Path('/home/auzieman/Projects/E-projects/media-player')
fixtures={
 'mp3':Path('/mnt/seagate-readonly/Music/(various)/Bull Durham - Original Motion Picture Soundtrack/07 Centerfield - John Fogerty.mp3'),
 'mod':Path('/home/auzieman/Music/mods/MotionOvertaki.mod'),
 'xm':Path('/home/auzieman/Music/mods/unvguest.xm'),
 'med':Path('/home/auzieman/Music/aminet-moods/unpacked/hum_m087/Funky_Sadeness.med'),
}
fixtures['s3m']=next(Path('/home/auzieman/Music').rglob('12ftgrve.s3m'))
results=[]
for factory in ('playbin3','playbin'):
 for kind,path in fixtures.items():
  pipeline=Gst.ElementFactory.make(factory)
  sink=Gst.ElementFactory.make('fakesink');sink.set_property('sync',True);sink.set_property('signal-handoffs',True)
  buffers=[0];sink.connect('handoff',lambda *args:buffers.__setitem__(0,buffers[0]+1))
  pipeline.set_property('audio-sink',sink)
  pipeline.set_property('video-sink',Gst.ElementFactory.make('fakesink'))
  pipeline.set_property('audio-filter',Gst.ElementFactory.make('equalizer-10bands'))
  pipeline.set_property('uri',path.as_uri())
  result={'factory':factory,'format':kind,'path':str(path),'buffers':0}
  pipeline.set_state(Gst.State.PLAYING);bus=pipeline.get_bus();start=time.monotonic()
  while time.monotonic()-start<4 and buffers[0]<3:
   message=bus.timed_pop_filtered(100*Gst.MSECOND,Gst.MessageType.ERROR|Gst.MessageType.EOS)
   if message and message.type==Gst.MessageType.ERROR:
    error,debug=message.parse_error();result['error']=str(error);result['debug']=debug;break
   if message and message.type==Gst.MessageType.EOS:break
  result['buffers']=buffers[0]
  pipeline.set_state(Gst.State.NULL);pipeline.get_state(2*Gst.SECOND)
  results.append(result);print(json.dumps(result),flush=True)
(root/'build/out/prototype/decoder-probe.json').write_text(json.dumps(results,indent=2)+'\n')
