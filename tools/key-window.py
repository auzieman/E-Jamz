"""Exercise focused native transport shortcuts with XTest."""
import ctypes as c
import sys
from x11_support import activate

title, key = sys.argv[1:3]
if "--no-activate" not in sys.argv[3:]: activate(title)
lib = c.CDLL('libX11.so.6'); xt = c.CDLL('libXtst.so.6')
lib.XOpenDisplay.restype = c.c_void_p
display = lib.XOpenDisplay(None); assert display
lib.XStringToKeysym.argtypes = [c.c_char_p]; lib.XStringToKeysym.restype = c.c_ulong
lib.XKeysymToKeycode.argtypes = [c.c_void_p, c.c_ulong]; lib.XKeysymToKeycode.restype = c.c_uint
code = lib.XKeysymToKeycode(display, lib.XStringToKeysym(key.encode())); assert code
xt.XTestFakeKeyEvent.argtypes = [c.c_void_p, c.c_uint, c.c_int, c.c_ulong]
xt.XTestFakeKeyEvent(display, code, 1, 0); xt.XTestFakeKeyEvent(display, code, 0, 0)
lib.XFlush.argtypes = [c.c_void_p]; lib.XFlush(display)
lib.XCloseDisplay.argtypes = [c.c_void_p]; lib.XCloseDisplay(display)
print('Key', title, key)
