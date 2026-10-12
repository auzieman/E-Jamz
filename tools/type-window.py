"""Type ASCII filter text through XTest (no direct UI callback invocation)."""
import ctypes as c,sys,time
from x11_support import activate
lib=c.CDLL('libX11.so.6');xt=c.CDLL('libXtst.so.6')
lib.XOpenDisplay.restype=c.c_void_p;d=lib.XOpenDisplay(None);assert d
lib.XKeysymToKeycode.argtypes=[c.c_void_p,c.c_ulong];lib.XKeysymToKeycode.restype=c.c_uint
xt.XTestFakeKeyEvent.argtypes=[c.c_void_p,c.c_uint,c.c_int,c.c_ulong];lib.XFlush.argtypes=[c.c_void_p]
if '--replace' in sys.argv[3:]:
 control=lib.XKeysymToKeycode(d,0xffe3);letter=lib.XKeysymToKeycode(d,ord('a'))
 xt.XTestFakeKeyEvent(d,control,1,0);xt.XTestFakeKeyEvent(d,letter,1,0);xt.XTestFakeKeyEvent(d,letter,0,0);xt.XTestFakeKeyEvent(d,control,0,0)
 lib.XFlush(d);time.sleep(.15)
for char in sys.argv[2]:
 code=lib.XKeysymToKeycode(d,ord(char));assert code
 shift=char.isupper() or char in '_():+?"<>!@#$%^&*'
 if shift:xt.XTestFakeKeyEvent(d,lib.XKeysymToKeycode(d,0xffe1),1,0)
 xt.XTestFakeKeyEvent(d,code,1,0);xt.XTestFakeKeyEvent(d,code,0,0)
 if shift:xt.XTestFakeKeyEvent(d,lib.XKeysymToKeycode(d,0xffe1),0,0)
 lib.XFlush(d);time.sleep(.015)
