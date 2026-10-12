---
name: e-audio-integration
description: Prepare or verify E-Jamz host audio output, GStreamer processing and ordinary/tracker playback without replacing the desktop audio stack.
---
Read ../../../docs/audio-and-fixtures.md, ../../../docs/interfaces.md and ../../../docs/task.md.
Identify the selected output/decoder/processing slice. Inspect actual session audio server, installed elements and matching-release package provenance; plugin presence is not runtime proof. Preserve the host's default routing/services unless the task selects a change.
Keep one playback/output owner. Route ordinary and tracker decoding through the same processing chain; do not confuse libopenmpt PCM decoding with an audio sink. FFmpeg is supporting tooling/codecs when justified.
Use licensed/hash-identified fixtures. Report audible output, bus state/errors, seek capability, EQ bypass/headroom and visible/minimized impact separately from compile success. JACK availability requires runtime evidence, not just jackaudiosink discovery.
Record findings in the active task. Preparation does not imply package installation, routing changes or remote deployment.
