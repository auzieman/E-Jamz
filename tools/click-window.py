"""Exercise a named visible X11 prototype control through XTest."""
import ctypes as c
import re
import subprocess
import sys

title, xx, yy = sys.argv[1:4]
from x11_support import activate,locate
_, ox, oy = locate(title) if "--no-activate" in sys.argv[4:] else activate(title)
x, y = ox + int(xx), oy + int(yy)
lib = c.CDLL('libX11.so.6'); xt = c.CDLL('libXtst.so.6')
lib.XOpenDisplay.restype = c.c_void_p
display = lib.XOpenDisplay(None); assert display
xt.XTestFakeMotionEvent.argtypes = [c.c_void_p, c.c_int, c.c_int, c.c_int, c.c_ulong]
xt.XTestFakeButtonEvent.argtypes = [c.c_void_p, c.c_uint, c.c_int, c.c_ulong]
lib.XFlush.argtypes = [c.c_void_p]
xt.XTestFakeMotionEvent(display, -1, x, y, 0)
xt.XTestFakeButtonEvent(display, 1, 1, 0)
xt.XTestFakeButtonEvent(display, 1, 0, 0)
lib.XFlush(display)
lib.XCloseDisplay.argtypes = [c.c_void_p]; lib.XCloseDisplay(display)
print('Clicked', title, xx, yy)
