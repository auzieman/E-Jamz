# XMMS reference for the next skin pass

Source study: https://github.com/sedwards/xmms/blob/master/xmms/skin.c
This is a mirror of original XMMS source, not proof of current maintenance.
The inspected skin loader separates main, transport, titlebar, shuffle/repeat,
volume, balance, play/pause, numbers, position, playlist and equalizer resources.
It also implements shaped/shaded main and EQ windows. The default main mask is
275×116, with doubled and narrow shaded variants. These provide concrete layout
references rather than a generic modern application shell.

E-Jamz follow-through: shorter stereo face, meaningful EQ/playlist switches,
consistent matching frames and compact/expanded states. Use live readable text
and keyboard controls rather than requiring a bitmap font. Keep our own original
blue-steel artwork and independent engine; no XMMS code/skin assets imported.
Source file declares GPL-2.0-or-later; any future source reuse needs a recorded
revision and compatible project licensing. Third-party skins need separate
asset provenance. Classic WSZ compatibility is a future feasibility task, not
implemented by this prototype.

EJ-011 control study: original README in the same mirror describes the PL
playlist switch, separate equalizer and Preset menu. E-Jamz uses full Playlist
and Equalizer labels, visibility lamps and a native anchored menu; its five
curves are original settings for GStreamer's current frequencies. Playlist
file interchange uses local M3U8 first. Reference:
https://github.com/sedwards/xmms/blob/master/README
