# E-Jamz product design

## One audio core, distinct faces

The compact main player, playlist and sound rack are native Edje panels in one
attached EFL window. Section visibility is independent; they share application state,
queue and one playback/output owner. This prepares the user's two-app concept
without assuming two executables, duplicate decoders or IPC in v0. Floating panels or a separately launched rack can be evaluated later against
the same control interface. The attached default avoids asynchronous follower
window movement and minimizes as one unit.

Main player: title/artist, time, transport, seek, volume, shuffle/repeat and buttons
for playlist and rack. Playlist: ordered queue, current item, add/remove/reorder,
save/load and search/filter when useful. Library browsing is a later larger view.
Rack: bass/treble, preamp, balance, graphic EQ, preset selection, reset and bypass.
The exact EQ band count/ranges and tone/EQ interaction need a listening prototype;
do not silently stack two boosts or promise a clipping meter without measurement.
A multi-source mixer is a separate capability, not a synonym for the rack.

## Ownership and audio path

EFL/Edje owns layout, textured controls and state feedback. The application core
owns track IDs, queue, persistence, processing settings and requested actions.
GStreamer is the selected primary backend direction; FFmpeg/libav and libopenmpt
serve demonstrated decoding/conversion gaps, not independent competing players.
Prove the PCM-to-processing-to-single-output path for tracker playback. UI events
request changes; backend state/error events report what actually happened.

Output uses the host desktop audio setup first. The current host is PipeWire with
a PulseAudio-compatible endpoint. JACK/ALSA are optional integration paths, not
reasons to replace services or change routing during preparation.

## Reuse and visual identity

Keep the Winamp/XMMS compact form and independent companion panels. Use existing
blue-steel raster control assets, quiet text backgrounds and restrained highlights;
check scale, focus, disabled states and readability before full widget work.
Clementine is a library/playlist reference. Rage is an EFL source-study candidate:
inspect exact revision, component boundaries and license before borrowing. Its
maintenance status has not been established; upstream web access was blocked
on October 11. No Qt application code or Rage runtime dependency is selected.

## Implementation sequence

1. Preview compact player, playlist and rack, including constrained/inactive states.
2. Prove matching-release toolchain and host output using licensed local fixtures.
3. Implement local queue/transport/persistence, ordinary and tracker playback.
4. Add rack processing and listening/headroom/preset acceptance.
5. Add discovery/relinking, then separately authorized organization operations.

Streaming providers, visualizers, eplet and actual multi-source mixing stay later.

The user-selected compact default is 400 px wide: player 215 px, rack 183 px and
queue 287 px nominal heights, with text kept at least 10 px. Actual window sizing
includes Elementary layout spacing. Silver-blue faders retain high contrast.

## Shared surface and transparency

Player, playlist, EQ and settings share one native window and palette owner.
The main Edje group declares `window.alpha` (1 enables the ARGB surface); the
skin's part/image/color-class alpha determines coverage. The host uses a basic
window without an unrelated Elementary standard background underneath it.
Palette changes preserve per-part alpha and update every realized section.
Transparent corners and unused drawer space remain transparent; LCD ink and
readable surfaces retain their authored opacity. External skins should declare
their window-alpha contract explicitly. No global desktop theme is modified.
