# Channels — next streaming slice

A channel is a named playable source, such as a direct radio stream or a
service-backed station. Collections group channels; a channel may link to a
playlist or another collection. Keep this adjacent to local playlists rather
than replacing the compact player with a catalog browser.

First slice: named direct HTTP(S) audio streams, collection selection, clear
connection/reconnect/error state and the same GStreamer/EQ/output owner. Distinguish
live elapsed time from a seekable recording; station switching releases the old
stream. Store a stable ID, label, source URI/type and optional artwork.

Service adapters add catalog/authentication/playback capabilities independently.
Credentials use established supported flows. A listed station or linked playlist
is not evidence of provider playback. Network requests begin only when selected.

Local M3U8 export/import is implemented first. Remote entries are currently
skipped and reported; they become channel candidates once direct-stream handling
is proven. Next acceptance: one public test station, switch/stop/reconnect, clear
errors and output/resource evidence. No provider integration is claimed here.
