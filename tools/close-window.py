"""Ask a named X11 client to close normally, allowing preference saving."""
import ctypes as c,sys
from x11_support import locate
window,_,_=locate(sys.argv[1])
class Data(c.Union):
    _fields_=[('l',c.c_long*5)]
class Message(c.Structure):
    _fields_=[('type',c.c_int),('serial',c.c_ulong),('send_event',c.c_int),('display',c.c_void_p),('window',c.c_ulong),('message_type',c.c_ulong),('format',c.c_int),('data',Data)]
class Event(c.Union):
    _fields_=[('message',Message),('pad',c.c_long*24)]
lib=c.CDLL('libX11.so.6');lib.XOpenDisplay.restype=c.c_void_p
lib.XInternAtom.argtypes=[c.c_void_p,c.c_char_p,c.c_int];lib.XInternAtom.restype=c.c_ulong
lib.XSendEvent.argtypes=[c.c_void_p,c.c_ulong,c.c_int,c.c_long,c.POINTER(Event)]
lib.XFlush.argtypes=[c.c_void_p];lib.XCloseDisplay.argtypes=[c.c_void_p]
display=lib.XOpenDisplay(None);assert display
e=Event();e.message.type=33;e.message.display=display;e.message.window=window;e.message.message_type=lib.XInternAtom(display,b'WM_PROTOCOLS',0);e.message.format=32;e.message.data.l[0]=lib.XInternAtom(display,b'WM_DELETE_WINDOW',0)
assert lib.XSendEvent(display,window,0,0,c.byref(e));lib.XFlush(display);lib.XCloseDisplay(display)
print('Requested normal close:',sys.argv[1])
