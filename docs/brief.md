# E media player — working name
Outcome: native EFL media interface with Clementine-like library/playlist capabilities and provider adapters where supported.
Initial investigation: map Clementine engine/library/provider components, dependency and license boundaries, and available native Linux provider playback routes. Compare backend reuse with an adapter over existing media libraries; do not assume a Qt frontend can simply be replaced.
Potential first playback slice: local MP3/MP4 and direct streams with play/pause/seek/queue. Implementation choice follows investigation.
Spotify milestone: establish compliant authorization and playback route, provider requirements and distribution limits before claiming supported integration. Password and QR options are conditional on supported flows. Unofficial playback is not proof of compliance.
Acceptance for backend study: source-backed component map, reuse costs, provider feasibility and one bounded prototype recommendation.
Future implementation acceptance: native preview, actual audio/video output, state/error handling and provider-specific validation. Metadata listing alone is not playback proof.

October 11 direction: distinctive compact EFL player plus matching playlist and sound rack, backed primarily by GStreamer and supporting media libraries. Host audio and library discovery/relink/optional organization are explicit features. See product-design.md and feature-map.md for delivery order; Seagate collection consolidation is deferred.
