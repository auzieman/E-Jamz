# E-Jamz v0.1.0 release preparation

2026-10-12. The user accepted the current native application for release.
Publication destination and original-source/artwork license are being selected;
no GitHub release or tag has been published by this preparation record.

## Included behavior

- Compact native EFL/Edje player, attached playlist and ten-band EQ rack.
- GStreamer local playback; FFmpeg/OpenMPT tracker decoding into the same output.
- Native file/folder drops and background directory scans, wheel paging and
  filename/folder filtering; filtered M3U8 export and local M3U/M3U8 import.
- Five EQ presets, custom band controls, bypass and reset.
- Embedded dot-matrix LCD typography and Blue Steel/Midnight/Amber Steel skins.
- Animated attached settings drawer; saved theme, panel visibility and optional
  volume/EQ/bypass preferences. Theme changes preserve the engine and queue.

## Verification and build identity

BKC run `808d6c41-3abd-4d46-be0d-8501f3cdedd5` completed. Native controls, drawer
opening/closing, private settings-file save and restoration were exercised.
MP3/MOD/S3M/XM/MED position advance, pause/resume/stop, DSP continuity, all preset
curves/faders, bypass/restore and Flat passed on the exact released executable.
A prior six-format pass also covered M4A before the Seagate fixture mount became
unavailable. Source/artifact hashes and native screenshots are recorded in
[settings/drawer evidence](settings-drawer-evidence.json).

`build/out/releases/` holds the prepared Alpine 3.24 amd64 runtime archive and
checksums; generated binaries stay out of Git. The executable is dynamically
linked: matching EFL/GStreamer plugins, Pulse-compatible desktop audio and
FFmpeg/OpenMPT are required. The archive is not a universal Linux executable.
Doto's OFL notice accompanies the skin and derived font.

## Current limits

Channels, provider integrations, shuffle/repeat, queue/session restoration,
metadata facets, tracker seeking and arbitrary XMMS skin layouts remain future
work. Add Music's full browser retains legacy host diagnostics; drops and
playlist path dialogs are the proven alternatives. An earlier import recheck
timed out during a host interruption; it is not counted as a new pass. Existing
M3U tests are documented in ui-features-evidence.json. Sustained performance,
underrun and broader accessibility acceptance remain unclaimed.

## Reproduction

Regenerate native resources with `python3 tools/generate-player-edc.py`, then
use the documented BKC remote build recipe in build/prototype/build.sh.
Validate schema contracts in an isolated environment with
`python3 -m pip install -r requirements-dev.txt` and
`python3 tools/validate-contracts.py`. `EJAMZ_VERIFY=1` exercises user-supplied
local audio without reading or writing the user's preference file. GUI helpers
require X11/XTest; their fixture paths are examples from the development host.
