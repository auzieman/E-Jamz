# E-Jamz feature list

Working native baseline · October 12, 2026. Current implementation accepted for initial release; [release scope](release-v0.1.0.md) identifies implemented behavior and remaining acceptance.
Winamp/XMMS-style compactness and Clementine-like organization are references;
EFL/Edje and our independent blue-steel assets own the presentation.

| Feature | Delivery order / state | Acceptance |
|---|---|---|
| Compact player | Native implementation; broader accessibility pending | Track/artist, elapsed/remaining time, transport, seek and volume readable at laptop/external-display sizes; keyboard focus and inactive states |
| Playlist window/pane | Attached native panel implemented | Matching independently shown pane; closing it preserves playback and queue |
| Sound rack window | Native ten-band EQ/presets/bypass implemented | Matching EQ/tone interface; open/close independently; state remains core-owned |
| Local audio | MP3/M4A native tests; broader format matrix pending | Licensed WAV/MP3/FLAC/Ogg fixtures audibly play through the selected host output |
| Tracker audio | MOD/S3M/XM/MED tested; tracker seeking disabled | MOD/S3M/XM fixtures; accurate capability-aware duration/seek; IT optional; missing decoder explained |
| Transport and queue | First runtime slice | Play/pause/stop, previous/next, shuffle/repeat, reorder/remove, queue advance and recoverable missing/corrupt tracks |
| File opening | First runtime slice | Multi-file chooser and drag/drop; adding files does not silently move them |
| Playlist persistence | Manual M3U/M3U8 save/load; session restore pending | Versioned internal queue with stable item IDs; reopen retains order; M3U import/export evaluated separately |
| EQ and tone shaping | Next sound slice | Bass/treble, preamp, balance, mute, reset/flat, bypass and named presets; headroom/clipping feedback; predictable persisted settings |
| Settings and skins | Native drawer and three palettes implemented | Live switching preserves playback/queue/EQ; saved skin/panels/optional volume/EQ/bypass |
| Output selection | Host proof then runtime | Desktop default works first; device changes/errors surfaced; ALSA/JACK options depend on verified runtime support |
| Library discovery | After usable playback | User-selected roots; background/cancellable scan; metadata search; unavailable removable roots preserve entries |
| Locate missing media | Library slice | Relink file/folder and update every affected playlist reference without changing queue identity/order |
| Organize into Music | Explicit later operation | Preview copy/move destinations and collection subfolders; collision rules, byte verification, journal and interrupted-operation recovery |
| Seagate sample collection | Deferred collection task | Confirm drive/mount/root and licensed fixtures; user reviews a top-level Music plan before any copy/move |
| JSON dialogs/plugin metadata | Schemas prepared | Existing positive/negative validation passes; runtime renderer/action registry still pending |
| Remote toolchain | BKC native builds verified | BKC build with signed matching-release inputs, immutable source and retained logs; compile distinct from playback proof |
| Provider/stream adapters | Reserved, disabled | Separate supported auth/catalog/playback evidence before enabling; no provider contacts in local v0 |
| Multi-source mixer | Later scope decision | Independent sources and routing; EQ panel alone does not claim mixing |
| Eplet/MPRIS, visualizers, video | Deferred | Separate contracts and host-impact evidence before adding |

Details: [product design](product-design.md), [library relocation](library-relocation.md),
[host audio and fixtures](audio-and-fixtures.md), [visual/performance](visual-performance.md).
