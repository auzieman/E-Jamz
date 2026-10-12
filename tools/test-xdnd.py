"""Send a genuine Xdnd URI-list transaction; no application callbacks bypassed."""
import ctypes as C, time,sys
from x11_support import activate
U=C.c_ulong; P=C.c_void_p
class Client(C.Structure):
 _fields_=[('type',C.c_int),('serial',U),('send_event',C.c_int),('display',P),('window',U),('message_type',U),('format',C.c_int),('data',C.c_long*5)]
class Request(C.Structure):
 _fields_=[('type',C.c_int),('serial',U),('send_event',C.c_int),('display',P),('owner',U),('requestor',U),('selection',U),('target',U),('property',U),('time',U)]
class Notify(C.Structure):
 _fields_=[('type',C.c_int),('serial',U),('send_event',C.c_int),('display',P),('requestor',U),('selection',U),('target',U),('property',U),('time',U)]
class Event(C.Union):
 _fields_=[('type',C.c_int),('client',Client),('request',Request),('notify',Notify),('pad',C.c_long*24)]
x=C.CDLL('libX11.so.6');x.XOpenDisplay.restype=P;d=x.XOpenDisplay(None);assert d
for n,args,ret in [('XDefaultRootWindow',[P],U),('XCreateSimpleWindow',[P,U,C.c_int,C.c_int,C.c_uint,C.c_uint,C.c_uint,U,U],U),('XInternAtom',[P,C.c_char_p,C.c_int],U),('XSetSelectionOwner',[P,U,U,U],C.c_int),('XSendEvent',[P,U,C.c_int,C.c_long,C.POINTER(Event)],C.c_int),('XFlush',[P],C.c_int),('XPending',[P],C.c_int),('XNextEvent',[P,C.POINTER(Event)],C.c_int),('XChangeProperty',[P,U,U,U,C.c_int,C.c_int,P,C.c_int],C.c_int)]:
 f=getattr(x,n);f.argtypes=args;f.restype=ret
atom=lambda n:x.XInternAtom(d,n.encode(),0)
w=x.XCreateSimpleWindow(d,x.XDefaultRootWindow(d),0,0,1,1,0,0,0)
target,ox,oy=activate(sys.argv[1]);rx=ox+int(sys.argv[2]);ry=oy+int(sys.argv[3]);payload=sys.argv[4].encode()
xt=C.CDLL('libXtst.so.6');xt.XTestFakeMotionEvent.argtypes=[P,C.c_int,C.c_int,C.c_int,U]
xt.XTestFakeMotionEvent(d,-1,rx,ry,0);x.XFlush(d);time.sleep(.1)
x.XSetSelectionOwner(d,atom('XdndSelection'),w,0)
def send(name,values):
 e=Event();e.client.type=33;e.client.display=d;e.client.window=target;e.client.message_type=atom(name);e.client.format=32
 for i,v in enumerate(values):e.client.data[i]=v
 x.XSendEvent(d,target,0,0,C.byref(e));x.XFlush(d)
send('XdndEnter',[w,5<<24,atom('text/uri-list'),0,0]);time.sleep(.1)
send('XdndPosition',[w,0,(rx<<16)|ry,0,atom('XdndActionCopy')]);time.sleep(.2)
dropped=False;finished=False;deadline=time.monotonic()+8
while time.monotonic()<deadline:
 while x.XPending(d):
  e=Event();x.XNextEvent(d,C.byref(e))
  if e.type==33:
   print('CLIENT',e.client.message_type,list(e.client.data),flush=True)
   if e.client.message_type==atom('XdndStatus') and not dropped:
    assert e.client.data[1]&1,'target rejected drop'
    send('XdndDrop',[w,0,0,0,0]);dropped=True
   if e.client.message_type==atom('XdndFinished'):finished=bool(e.client.data[1]&1);break
  elif e.type==30:
   r=e.request;prop=r.property or r.target
   print('SELECTION REQUEST',flush=True)
   x.XChangeProperty(d,r.requestor,prop,r.target,8,0,C.cast(C.c_char_p(payload),P),len(payload))
   n=Event();n.notify=Notify(31,0,1,d,r.requestor,r.selection,r.target,prop,r.time)
   x.XSendEvent(d,r.requestor,0,0,C.byref(n));x.XFlush(d)
 if finished:break
 time.sleep(.02)
assert finished,'Xdnd did not complete successfully'
print('XDND PASS')
