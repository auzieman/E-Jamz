"""Activate the target before native UI checks, avoiding occluded-window input."""
import ctypes as c
import ctypes.util
import re
import subprocess
import time
import signal

def activate(title):
    window,_,_=locate(title)
    previous_sigchld=signal.getsignal(signal.SIGCHLD)
    lib = c.CDLL(ctypes.util.find_library('ecore_x'))
    lib.ecore_x_init.argtypes = [c.c_char_p]
    assert lib.ecore_x_init(None), 'Cannot open X11 display'
    lib.ecore_x_window_root_first_get.restype = c.c_uint
    lib.ecore_x_netwm_client_active_request.argtypes = [c.c_uint, c.c_uint, c.c_int, c.c_uint]
    lib.ecore_x_netwm_client_active_request(lib.ecore_x_window_root_first_get(), window, 2, 0)
    lib.ecore_x_sync(); time.sleep(.3); lib.ecore_x_shutdown()
    # Raise the reparenting frame as well as requesting activation. E can defer
    # activation while another client owns focus; otherwise XGetImage sees it.
    xlib=c.CDLL('libX11.so.6');xlib.XOpenDisplay.restype=c.c_void_p
    display=xlib.XOpenDisplay(None)
    xlib.XQueryTree.argtypes=[c.c_void_p,c.c_ulong,c.POINTER(c.c_ulong),c.POINTER(c.c_ulong),c.POINTER(c.POINTER(c.c_ulong)),c.POINTER(c.c_uint)]
    root,parent=c.c_ulong(),c.c_ulong();children=c.POINTER(c.c_ulong)();count=c.c_uint()
    xlib.XRaiseWindow.argtypes=[c.c_void_p,c.c_ulong];xlib.XFlush.argtypes=[c.c_void_p]
    xlib.XFree.argtypes=[c.c_void_p];xlib.XCloseDisplay.argtypes=[c.c_void_p]
    if(xlib.XQueryTree(display,window,c.byref(root),c.byref(parent),c.byref(children),c.byref(count))):
        xlib.XRaiseWindow(display,parent.value if parent.value!=root.value else window)
        if children:xlib.XFree(children)
    xlib.XFlush(display);time.sleep(.3);xlib.XCloseDisplay(display)
    signal.signal(signal.SIGCHLD,previous_sigchld)
    return locate(title)

def locate(title):
    # Query managed clients, avoiding a full tree walk through transient browser windows.
    lib=c.CDLL('libX11.so.6');lib.XOpenDisplay.restype=c.c_void_p
    lib.XDefaultRootWindow.argtypes=[c.c_void_p];lib.XDefaultRootWindow.restype=c.c_ulong
    lib.XInternAtom.argtypes=[c.c_void_p,c.c_char_p,c.c_int];lib.XInternAtom.restype=c.c_ulong
    lib.XGetWindowProperty.argtypes=[c.c_void_p,c.c_ulong,c.c_ulong,c.c_long,c.c_long,c.c_int,c.c_ulong,c.POINTER(c.c_ulong),c.POINTER(c.c_int),c.POINTER(c.c_ulong),c.POINTER(c.c_ulong),c.POINTER(c.c_void_p)]
    lib.XFetchName.argtypes=[c.c_void_p,c.c_ulong,c.POINTER(c.c_void_p)]
    lib.XTranslateCoordinates.argtypes=[c.c_void_p,c.c_ulong,c.c_ulong,c.c_int,c.c_int,c.POINTER(c.c_int),c.POINTER(c.c_int),c.POINTER(c.c_ulong)]
    lib.XFree.argtypes=[c.c_void_p];lib.XCloseDisplay.argtypes=[c.c_void_p]
    display=lib.XOpenDisplay(None);assert display
    root=lib.XDefaultRootWindow(display);atom=lib.XInternAtom(display,b'_NET_CLIENT_LIST',0)
    actual=c.c_ulong();fmt=c.c_int();count=c.c_ulong();after=c.c_ulong();value=c.c_void_p()
    result=None
    try:
        assert lib.XGetWindowProperty(display,root,atom,0,4096,0,0,c.byref(actual),c.byref(fmt),c.byref(count),c.byref(after),c.byref(value))==0
        if value and fmt.value==32:
            clients=c.cast(value,c.POINTER(c.c_ulong))
            for i in range(count.value):
                name=c.c_void_p();na=c.c_ulong();nf=c.c_int();nc=c.c_ulong();nr=c.c_ulong()
                name_atom=lib.XInternAtom(display,b'_NET_WM_NAME',0)
                lib.XGetWindowProperty(display,clients[i],name_atom,0,1024,0,0,c.byref(na),c.byref(nf),c.byref(nc),c.byref(nr),c.byref(name))
                if name and nf.value==8:
                    match=c.string_at(name,nc.value).decode('utf-8',errors='replace')==title;lib.XFree(name)
                    if match:
                        x,y=c.c_int(),c.c_int();child=c.c_ulong()
                        lib.XTranslateCoordinates(display,clients[i],root,0,0,c.byref(x),c.byref(y),c.byref(child))
                        result=clients[i],x.value,y.value;break
    finally:
        if value:lib.XFree(value)
        lib.XCloseDisplay(display)
    if result is None:raise RuntimeError('Window not found: '+title)
    return result
