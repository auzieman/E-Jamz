"""Exercise native playlist wheel events through XTest."""
import ctypes as c,sys
from x11_support import activate
_,ox,oy=activate(sys.argv[1]);lib=c.CDLL('libX11.so.6');xt=c.CDLL('libXtst.so.6')
lib.XOpenDisplay.restype=c.c_void_p;d=lib.XOpenDisplay(None);assert d
xt.XTestFakeMotionEvent.argtypes=[c.c_void_p,c.c_int,c.c_int,c.c_int,c.c_ulong]
xt.XTestFakeButtonEvent.argtypes=[c.c_void_p,c.c_uint,c.c_int,c.c_ulong];lib.XFlush.argtypes=[c.c_void_p]
xt.XTestFakeMotionEvent(d,-1,ox+int(sys.argv[2]),oy+int(sys.argv[3]),0)
button=5 if int(sys.argv[4])>0 else 4
for i in range(abs(int(sys.argv[4]))):xt.XTestFakeButtonEvent(d,button,1,0);xt.XTestFakeButtonEvent(d,button,0,0)
lib.XFlush(d)
