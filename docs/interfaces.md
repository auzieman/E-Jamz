# Planned component interfaces
UI calls application actions (open_files, save_playlist, load_playlist, play, pause, stop, next, previous, seek, set_volume). A fixed registry resolves JSON action names; unknown actions fail closed.
Core owns ordered playlist entries, stable entry IDs, current item, playback state and versioned persistence. Decoder selection reports capabilities; extensions alone do not establish format support.
Engine adapter: probe(local_path), open(local_path), play/pause/stop, seek(seconds), set_volume and events (ready, position, ended, error). libopenmpt outputs PCM; selecting a decoder does not select an audio sink. Define the PCM/sink integration before implementing two independent engines.
Provider adapter: identity, capabilities, authorization status, browse, resolve and optional provider-managed playback. Disabled placeholders return unavailable; they do not fall back to unofficial playback or silently contact providers.
Future eplet controls application state through a defined control API/MPRIS; it owns no decoder or provider credentials.

Sound rack actions: set_tone, set_eq_band, set_preamp, set_balance, set_processing_bypass, load/save/reset_preset. Core persists settings; engine reports applied state/errors; closing panels does not discard state. Exact parameter ranges remain a DSP proof decision.
Library actions: scan_roots, locate_missing, plan_organization, execute/cancel/recover_organization. Discovery is separate from file mutation. Stable media IDs and a journal preserve queue and playlist references; see library-relocation.md.

Current tracker prototype: FFmpeg’s installed libopenmpt demuxer supplies bounded
stereo 48 kHz S16 PCM through a pipe to GStreamer’s clock/EQ/volume/Pulse output.
The producer is a managed child, stopped/reaped on track changes and exit.
Pause backpressure limits decoding ahead. Ordinary files use audio-only playbin3
and explicit pulsesink. Tracker seek is disabled pending a verified implementation;
no conversion file or second audio sink is created.
