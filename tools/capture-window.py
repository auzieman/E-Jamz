"""Read a named X11 client window and save RGB PNG without extra dependencies."""
import ctypes as c
import re
import struct
import subprocess
import sys
import zlib

title, destination = sys.argv[1:3]
from x11_support import activate
from x11_support import locate
window,_,_=locate(title) if '--no-activate' in sys.argv[3:] else activate(title)
lib = c.CDLL('libX11.so.6')
lib.XOpenDisplay.restype = c.c_void_p
display = lib.XOpenDisplay(None)
assert display, 'Cannot open X11 display'
lib.XGetGeometry.argtypes = [c.c_void_p, c.c_ulong, c.POINTER(c.c_ulong),
                            c.POINTER(c.c_int), c.POINTER(c.c_int),
                            c.POINTER(c.c_uint), c.POINTER(c.c_uint),
                            c.POINTER(c.c_uint), c.POINTER(c.c_uint)]
root, x, y = c.c_ulong(), c.c_int(), c.c_int()
width, height, border, depth = c.c_uint(), c.c_uint(), c.c_uint(), c.c_uint()
assert lib.XGetGeometry(display, window, c.byref(root), c.byref(x), c.byref(y),
                        c.byref(width), c.byref(height), c.byref(border), c.byref(depth))
lib.XGetImage.argtypes = [c.c_void_p, c.c_ulong, c.c_int, c.c_int, c.c_uint, c.c_uint, c.c_ulong, c.c_int]
lib.XGetImage.restype = c.c_void_p
image = lib.XGetImage(display, window, 0, 0, width.value, height.value, c.c_ulong(-1), 2)
assert image, 'Window must be visible'
lib.XGetPixel.argtypes = [c.c_void_p, c.c_int, c.c_int]
lib.XGetPixel.restype = c.c_ulong
rows = bytearray()
for yy in range(height.value):
    rows.append(0)
    for xx in range(width.value):
        pixel = lib.XGetPixel(image, xx, yy)
        rows.extend(((pixel >> 16) & 255, (pixel >> 8) & 255, pixel & 255))
def chunk(kind, data):
    return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data))
with open(destination, 'wb') as output:
    output.write(b'\x89PNG\r\n\x1a\n')
    output.write(chunk(b'IHDR', struct.pack('>IIBBBBB', width.value, height.value, 8, 2, 0, 0, 0)))
    output.write(chunk(b'IDAT', zlib.compress(rows)))
    output.write(chunk(b'IEND', b''))
lib.XDestroyImage.argtypes = [c.c_void_p]
lib.XDestroyImage(image)
lib.XCloseDisplay.argtypes = [c.c_void_p]
lib.XCloseDisplay(display)
print(destination, width.value, height.value)
