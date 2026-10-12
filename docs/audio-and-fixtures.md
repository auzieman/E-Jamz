# Host audio and playback preparation

Read-only observation on October 11, 2026: the host's `pactl info` reports
PulseAudio on PipeWire 1.6.8, with a local session endpoint and analog-stereo
output. [Recorded capability evidence](audio-host-20261011.json) includes tool
versions. No services, packages, audio routes or user volume were changed.

Installed GStreamer elements: playbin3, pulsesink, pipewiresink, alsasink,
jackaudiosink, equalizer-10bands, audiopanorama and avdec_mp3. openmptdec was not
found. Element presence does not prove usable JACK service, negotiated output,
tracker support, audible playback or a complete codec set. Verify packages and
signed Alpine 3.24 development inputs before remote compilation.

Primary backend direction: GStreamer. Start with the existing desktop endpoint;
FFmpeg/libav is supporting codec/tooling where needed. Tracker integration remains
a proof task: available decoder plugin or libopenmpt PCM into the shared processing
and output path. No package installation or engine implementation in this pass.

[GStreamer playbin3 documentation](https://gstreamer.freedesktop.org/documentation/playback/playbin3.html)
describes bus-driven state/error/tag reporting and selectable output sinks. Keep
asynchronous work off the EFL UI thread; seeking acceptance follows actual backend
capabilities. The exact processing chain and event-loop integration remain to prove.

## Fixture manifest to prepare next

For each sample record format, relative location, hash, author/source URL,
license/permission, duration and expected seek support. Use generated WAV tones
for repeatable measurements and licensed music for listening; retain conversion
commands when creating codec variants. Cover WAV/MP3/FLAC/Ogg and MOD/S3M/XM;
IT is optional. Include missing/corrupt fixtures and unsupported-format handling.
Never assume recovered or bundled songs are redistributable.

Playback acceptance includes audible output, transitions, pause/seek/stop, queue
advance and error recovery. Rack acceptance includes flat/bypass comparison,
headroom, presets and settings surviving panel closure. Measure stopped, visible,
playing and minimized CPU/RSS per visual-performance.md. Remote compilation is
separate evidence; Seagate sample consolidation is a later explicitly scoped task.
