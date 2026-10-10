# Planned component interfaces
UI calls application actions (open_files, save_playlist, load_playlist, play, pause, stop, next, previous, seek, set_volume). A fixed registry resolves JSON action names; unknown actions fail closed.
Core owns ordered playlist entries, stable entry IDs, current item, playback state and versioned persistence. Decoder selection reports capabilities; extensions alone do not establish format support.
Engine adapter: probe(local_path), open(local_path), play/pause/stop, seek(seconds), set_volume and events (ready, position, ended, error). libopenmpt outputs PCM; selecting a decoder does not select an audio sink. Define the PCM/sink integration before implementing two independent engines.
Provider adapter: identity, capabilities, authorization status, browse, resolve and optional provider-managed playback. Disabled placeholders return unavailable; they do not fall back to unofficial playback or silently contact providers.
Future eplet controls application state through a defined control API/MPRIS; it owns no decoder or provider credentials.
