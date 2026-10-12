# E-Jamz stereo prototype

[Full stereo mockup](images/ejamz-stereo-stack-v2.png) ·
[Compact mockup](images/ejamz-compact-v2.png).
Editable SVG exports live alongside PNG previews. Regenerate with
`python3 tools/render-preview.py`; original raster backing is embedded unchanged
and nine-sliced, while labels/layout remain editable. Native player EDC is
regenerated with `python3 tools/generate-player-edc.py`.

Prototype owner: `src/ui/prototype.c`, an intentionally bounded integration spike
before splitting proven behavior into core/engine/UI modules. Native skin source:
`src/ui/player.edc`. One active GStreamer output pipeline; ordinary files use audio-only playbin3,
trackers use installed FFmpeg/OpenMPT PCM feeding GStreamer EQ/volume/Pulse.
Ecore polls bus messages and visible position state. Player, queue and rack are
attached panels in one window, eliminating inter-window drag lag. Section close
hides that panel; main close exits and releases pipeline/decoder child.

Implemented first slice: local file chooser/CLI files and playlist URI-list drops, queue selection/advance,
play/pause/stop/previous/next, seek, application volume and ten EQ bands/bypass.
Player, up-next and rack share native Edje frames and borderless chrome inside
one attached window. Rack buttons toggle panel packing; the whole stack moves
and minimizes as one object. Queue rows include eight-item paging. The information/lyrics dock, bass/treble/preamp/balance,
user-defined EQ presets, session persistence, shuffle/repeat, block chaining, tracker seeking,
full focus/accessibility treatment and relocation remain follow-ups. Transport
keys Z/X/C/V/B and Space plus EQ Flat are the first familiar interaction slice. This is an initial
working slice, not completed feature-map acceptance or a release.

Build remotely through BKC `e-native-prototype-build`, which uses the existing
generic folder executor. Snapshot hash pins the source archive; the builder uses
the pinned Alpine 3.24.2 image and signed official/AlpinE packages with the
existing public key. Private key material is not shipped. Build outputs include
binary, EDJ, dependency inventory and SHA256 checksums.

Run workspace artifacts on the laptop:

```sh
ELM_ENGINE=software_x11 ELM_ACCEL=none \
EJAMZ_THEME="$PWD/build/out/prototype/player.edj" \
  build/out/prototype/ejamz /path/to/local/audio.wav
```

No files are organized or moved. Generated low-amplitude WAV fixtures are test
inputs only; observing the desktop stream is distinct from audible listening
acceptance and sustained performance/underrun evidence.

Runtime follow-up: Ecore event-handler safety diagnostics occur in the v2 log despite successful rendering/Play/Pause. Investigate event-loop and plugin initialization before calling runtime acceptance complete. See [evidence](prototype-evidence.json).

See [rack/runtime evidence](rack-evidence.json) and native screenshots:
[player](images/ejamz-player-borderless.png), [rack](images/ejamz-rack-skinned.png),
[queue](images/ejamz-queue-skinned.png). Captioned resource samples include separate shared-service activity and are not sustained-performance acceptance.

Integrated playback verification: set `EJAMZ_VERIFY=1` and pass local fixtures to
the binary. It checks actual Playing/position, pause/resume/stop, EQ continuity
across files and Flat reset against the desktop output at reduced volume, then
exits with failure status if state transitions fail. Logs identify selected paths
and detailed GStreamer errors. Verification helpers activate their target window
before sending input or capturing so another application cannot obscure the test.

Drops anywhere on the player accept local file-manager URI lists (including escaped spaces) and local absolute paths in URI transfers.
Files append without interrupting playback. Folders expand recursively on an
Ecore worker in filename order, skipping hidden entries, directory symlinks and
unsupported extensions. No copying or relocation occurs. Chooser selections and its explicit Add-folder button use the same scanner.
Tracker duration is read off-thread using FFprobe/OpenMPT: elapsed time and
progress advance, while tracker seeking remains disabled.

The playlist supports mouse-wheel paging and a case-insensitive filter over
filenames and folder paths. Filtering only changes the visible rows; selection
uses the original queue index and playback order is retained. Artist/album tag
facets still require a background metadata index. Embedded native controls
receive pointer events, and text-field focus releases transport key grabs while editing.

For this software-X11 prototype, Ecore_X owns the window-level URI transfer,
independent of Edje object hit-test ancestry. Wayland drops remain unverified.
Duration probes are asynchronous Ecore child processes, avoiding competing
GLib/Ecore child reapers. No media organization or provider work occurs.

LCD title/time use an embedded Doto dot-matrix font (800 weight, square dots).
The title display is uppercase; source filenames and playlist labels are retained.
Small labels and playlist controls keep their readable sans-serif font. Source
blobs, license and reproduction are in assets/fonts. Typography preview:
[dot-LCD study](images/ejamz-dot-lcd-preview.png).

Control labels: Add Music appends inputs, Playlist and Equalizer toggle their
panels with independent visibility lights. The EQ On/Off control indicates DSP
state. Presets: Flat, Bass Focus, Vocal, Bright and Warm use original cut-only
curves matched to the current band frequencies. Reset returns Flat; moving a
fader marks Custom. Bypass retains the selected curve. User preset files remain
a later slice.

Save List snapshots all visible tracks; with a filter it becomes Save Matches.
Save/Load use a native filename dialog; full paths and ~/ are accepted, and bare
filenames resolve under home. New M3U8 files preserve order and reference existing
media. Existing files are preserved; choose a fresh name. Load appends local
M3U/M3U8 references in order, resolving relative paths and file URIs. Missing,
remote and unsupported references are counted as skipped. Folder scans do not
automatically expand playlists found inside collections. No media is moved.

The static prototype selects app-scoped synchronous Evas rendering to avoid
observed software-X11 async wait stalls. Rack expansion accounts for native minimum sizes and keeps controls above
the screen edge with 32px shelf clearance. Legacy Add Music file-browser diagnostics remain a
follow-up; Save/Load avoid that browser. [Channels](channels.md) records the next
streaming/service collection slice; channel playback is not implemented.

[UI features and current evidence](ui-features-evidence.json).


## Settings and skins (EJ-012)

SETTINGS in any rack header toggles an attached right-side preferences drawer.
The rack stays 400px wide; the drawer adds 280px and slides over 180ms.
Playback controls remain available while settings are open. The drawer shares
the selected skin and scrolls vertically as controls grow. Blue Steel is
the default; Midnight uses a mint LCD and darker metal, Amber Steel a warm LCD
and metal tint. All three use the existing original texture and embedded dot
font. Per-object Edje color classes change all three panels in place without
recreating layouts, queue widgets or the GStreamer output.

Preferences remember skin and playlist/equalizer visibility. “Remember volume
and EQ” also restores application volume, all ten EQ gains and bypass. Turning
it off keeps current playback unchanged and uses default audio settings on
the next launch. Save & Close writes preferences; orderly exit saves later
panel/audio changes. Tracks are never automatically played on startup.

Storage: `$XDG_CONFIG_HOME/ejamz/settings.ini` (normally
`~/.config/ejamz/settings.ini`), atomically replaced with mode 0600. Invalid
theme/volume/curve values fall back to defaults. `EJAMZ_CONFIG_FILE` overrides
the path for isolated tests. Playback verification does not read or write the
user’s settings. Delete the file to reset preferences. Queue/session
restoration and output-device selection remain follow-ups.

Palette source is generated alongside `player.edc` in `theme-palette.h`;
`python3 tools/generate-player-edc.py` regenerates both. Skins are color
variations of one shared rack geometry, rather than arbitrary imported XMMS
layouts. [Palette study](images/ej012-theme-study.png) is a source preview;
[drawer study](images/ej013-drawer-study.png) and
[restored Midnight rack](images/ej012-restored-midnight.png) show native views.

The drawer is a view within the same native window, with no independent window
position to track. Opening it is temporary and is not restored at startup.
Save & Close or the drawer X saves preferences and retracts it; the SETTINGS
button retracts it too. Future channels sections will use this space; streaming
and provider linkage are still planned in channels.md.

Native drawer acceptance and source/artifact hashes are recorded in
[settings/drawer evidence](settings-drawer-evidence.json).
[Expanded native drawer](images/ej013-drawer-expanded.png) and
[collapsed rack](images/ej013-drawer-collapsed.png) show the final layout.
