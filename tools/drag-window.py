"""Exercise native thumbs/titlebars in a named visible X11 prototype."""
import ctypes as c
import re
import subprocess
import sys
import time

title, *coords = sys.argv[1:]
x1, y1, x2, y2 = map(int, coords)
from x11_support import activate
_, ox, oy = activate(title)
lib = c.CDLL('libX11.so.6'); xt = c.CDLL('libXtst.so.6')
lib.XOpenDisplay.restype = c.c_void_p
display = lib.XOpenDisplay(None); assert display
xt.XTestFakeMotionEvent.argtypes = [c.c_void_p, c.c_int, c.c_int, c.c_int, c.c_ulong]
xt.XTestFakeButtonEvent.argtypes = [c.c_void_p, c.c_uint, c.c_int, c.c_ulong]
lib.XFlush.argtypes = [c.c_void_p]
xt.XTestFakeMotionEvent(display, -1, ox+x1, oy+y1, 0)
xt.XTestFakeButtonEvent(display, 1, 1, 0); lib.XFlush(display); time.sleep(.15)
for i in range(1, 11):
    xt.XTestFakeMotionEvent(display, -1, ox+x1+(x2-x1)*i//10, oy+y1+(y2-y1)*i//10, 0)
    lib.XFlush(display); time.sleep(.025)
xt.XTestFakeButtonEvent(display, 1, 0, 0); lib.XFlush(display)
lib.XCloseDisplay.argtypes = [c.c_void_p]; lib.XCloseDisplay(display)
print('Dragged', title, coords)
