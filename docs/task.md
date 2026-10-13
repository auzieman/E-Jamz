# Active task — shared skin transparency

User reports transparency mismatch between player modules on October 12, 2026.
Inspect the current shared-window/palette implementation and native backdrop.
Keep skin-owned alpha consistent across player, playlist, EQ and settings;
preserve playback/queue/DSP and existing settings. Build natively through BKC,
then verify module visibility and alpha behavior in each palette. Retain the
previous binary/EDJ for rollback; do not relocate music or change desktop theme.
Implemented: basic shared ARGB window with Edje-declared window.alpha;
removed inherited standard-window backdrop, explicitly set rack/shell padding,
and gave settings controls a palette-matched opaque LCD surface. All realized
sections receive the same palette/alpha values. Desktop theme and user audio
settings are unchanged; the original live player was not interrupted.
Final BKC native build c6346d09-45b9-45dc-b764-0d5b8360f119 completed.
Native preset/theme checks passed across three palettes, preserving engine,
queue and DSP. Rendered pixel checks verified unused canvas alpha 0 and LCD/
settings alpha 255. Playlist/EQ/settings and Midnight/Amber rendering were
inspected; final Amber screenshot is docs/images/ej014-shared-alpha-amber.png.
Checks used an isolated settings file and no media tracks; no new sustained
playback/performance acceptance is claimed. Previous release artifacts remain
intact. Local ~/.local/bin/ejamz and desktop launcher use the updated build;
restart the existing player when convenient to use it.

# Previous task — initial release publication

Current objective: publish the user-accepted native E-Jamz slice through GitHub,
with source/build evidence, notices and reproducible runtime packages.
Public repository selected: auzieman/E-Jamz. Original software GPLv3; original
artwork CC BY-SA 4.0. Final publication verification follows below.

## Alignment preparation history
Date: 2026-10-10
Objective: establish media-player ownership, provider constraints and investigation boundaries.
Planned mutation: planning documents only. Rollback: remove new scaffold files.
Acceptance: backend selection remains evidence-led; compliance is not assumed; theme work remains separate.
Result: scaffold prepared. No application code, provider account/app registration or playback backend created.
Next action: bounded Clementine component/provider feasibility study when player work starts.
Follow-ups: choose engine; local playback slice; Spotify compliance feasibility; additional providers independently.

Task-start prompt: Read project-contract.md and this brief/task. Use e-media-backend for the selected slice. State the objective, source revision, acceptance and owning component; record plan/rollback before edits and actual evidence afterwards. Keep unrelated discoveries as follow-ups.

## Repository/toolchain setup — 2026-10-10
Plan: establish local main/development/feature branches, project folders, instructions, feature map and compiler-base recipe. Rollback: retain scaffold backup before removing repositories. Acceptance: tracked bootstrap, clean feature branches, independent skills and shared-theme ownership documented. Result: repository scaffolds prepared; compiler image and cluster deployment remain unvalidated/unexecuted.

Validation: project skill frontmatter passed quick_validate.py; Markdown links and skill contract references resolve. Compiler image has not been built; cluster target/storage and EFL package provenance remain pending. Local bootstrap committed; main/development/feature branches established.

## EJ-001 — E-Jamz preparation, 2026-10-10
Objective: define local-first playback, declarative dialogs/plugin metadata and a remote dependency-proof pipeline before implementation.
Plan: update feature contract; add schemas/examples and toolchain proof script; stage a draft BKC recipe using existing core ownership. No new executor dispatch, provider integration or cluster deployment.
Rollback: revert this task's local commits; original BKC checkout and theme repository are untouched.
Acceptance: schemas reject malformed metadata; shell proof syntax passes; dependency targets and artifact evidence are explicit; draft pipeline states unsupported wiring and required runtime inputs honestly.

EJ-001 result: local-first/UI/interface contract, versioned dialog/plugin schemas and examples, dependency manifest, remote compile/link probe and draft BKC recipe prepared. Schema validation and malformed/version/action negative cases passed; shell syntax and diff whitespace checks passed. No remote compilation, audio runtime or provider wiring. Matching-release dependencies and generic BKC core wiring remain pending. Next action: resolve remote target and signed EFL/dependency provenance, then implement the smallest missing CLI/core operation if necessary. Theme work remains parked.

## EJ-002 — visual-led preparation, 2026-10-10
Objective: make artwork/layout and low host impact first-class acceptance requirements while completing the remote-run handoff.
Plan: add visual/runtime acceptance reference, tighten the visual skill and project guidance, and document exact readiness gaps. No product/theme implementation or cluster deployment.
Rollback: revert EJ-002 documentation commit. Acceptance: next slice starts with a visual preview; performance evidence distinguishes static, playing and minimized states; remote readiness does not claim unsupported pipeline execution.

EJ-002 result: visual-led skill/guidance, state-based performance acceptance and remote readiness map prepared. Skill validation, contract positive/negative cases, reference checks and whitespace checks passed. No artwork, runtime implementation, remote run or theme changes. Next bounded preparation task: trace the CLI-owned staging/remote-execute/artifact path and inspect existing build capacity before enabling the draft proof.

## EJ-003 — raster Edje control resource
Plan: import a versioned original button/control resource from theme-kit BK-010 for native E-Jamz presentation, and document host-owned labels, action requests and disabled state. No engine/provider implementation or desktop chrome dependency. Rollback: revert EJ-003 export/docs. Acceptance: self-contained EDC/raster snapshot with exact origin revision and hashes, signal contract and honest compiler/runtime limits.

EJ-003 result: imported independent raster button/control EDC snapshot from theme-kit 1d2ab61, with per-file hashes and host signal contract. Resource provenance and include/image references checked; no player host, audio backend, provider, compile or rendering PASS claimed.

## EJ-004 — feature and workflow preparation, 2026-10-11

Objective: consolidate the user-selected compact player, companion sound rack,
host audio integration and optional library relocation into an actionable plan.
Owner: media-player. Baseline: 63c043a. Planned mutations: documentation,
project-local skills and read-only host capability evidence only.
Rollback: revert these scoped files/task entries. No media relocation, package
installation, playback implementation, remote build or deployment is authorized
by this preparation task. Acceptance: feature priorities and observable criteria,
component boundaries, relocation recovery rules, fixture plan, discoverable skills
and consistent contracts. Seagate collection consolidation remains a later task.

EJ-004 result: expanded feature map and linked product/library/audio plans;
GStreamer primary direction, companion rack and independent panel state recorded.
Added e-audio-integration and e-library-organize skills; updated backend/visual
routing and README/contracts. Read-only host evidence confirms PulseAudio on
PipeWire and installed output/EQ elements; openmptdec absent. Rage upstream web
access was denied, so maintenance/reuse remains unverified. No music moved,
packages installed, host routing changed, player built or remote deployment run.
Validation: three new/updated backend/audio/library skills passed the bundled
quick validator using host Python; documentation/skill paths, JSON parsing and
Git whitespace checks passed. Existing schema validator could not run because
jsonschema is absent in both SDK and host Python; schemas/examples unchanged.
Next: player/playlist/rack visual preview, followed by signed toolchain and one
shared ordinary/tracker playback path proof. Later: read-only Seagate inventory
and an explicitly selected top-level Music organization plan.

## EJ-005 — stereo mockup and live prototype, 2026-10-11

User authorized mockup first, followed by wiring into a live player. Plan: editable native-layout reference using existing raster controls, silver-blue LCD, player/up-next/rack/info companion views; then an EFL/GStreamer local prototype with bounded queue/transport/DSP behavior. Preserve prior preparation edits. No music organization or provider work. Compile remotely through BKC; local rasterization/syntax checks are preview work. Rollback: remove EJ-005 source/assets/exports and prototype artifacts. Acceptance: rendered compact/full-stack previews, build evidence and explicit distinction between implemented, audible and unverified states.

EJ-005 result: compact/full-stack editable SVG/PNG mockups and stronger v2 frame following user feedback; native EDC/C player compiled twice through generic BKC folder execution on edge. Runs 5934803a-f598-4694-a61c-d50619caeaef and c8e4fc1b-e149-4529-8544-ae57be009e44 complete. Native v2 renders at 480×258; Play/Pause and skinned companion-panel buttons exercised; generated WAV stream connected to PipeWire default sink and position advanced. Current prototype left open paused. Native screenshots, artifact hashes and unverified acceptance are recorded in prototype-evidence.json and prototype.md. Existing texture unchanged; source-backed XMMS skin reference recorded in xmms-reference.md. Info dock and complete rack/queue skin, persistence, keyboard treatment, trackers and performance remain follow-ups. Earlier runtime produced EFL diagnostics and its window disappeared before recovery test; no recovery/listening PASS claimed. Preview XML, Python syntax, compile/EDJ and diff whitespace checks passed. No music relocated or provider contacted.

Additional observed limit: v2 also logs Ecore event-handler safety diagnostics while remaining responsive. Investigate before release/runtime acceptance; the prototype is not a clean-runtime PASS.

## EJ-006 — unified companion skins and window chrome

User reports partial companion UIs and panels jumping away from the player;
authorizes integrated window controls and removing outer borders. Plan: use one
Edje frame/control language for player/queue/rack, native EQ thumbs and playlist
rows, titlebar drag/minimize/close, anchored companion placement and grouped
movement. Preserve playback state when panels hide. Compile via the existing
BKC generic build lane; validate rendering, placement and controls on host.
Rollback: prior v2 binary/EDJ retained before replacement. No media changes.

EJ-006 result: three matching Edje skins; queue rows/paging and ten native EQ thumbs/bypass; skin drag/minimize/close controls; borderless windows; main buttons toggle and indicate section visibility; companions follow main via native X11 configure events. Runtime placement/drag, close/toggles and EQ parameter feedback observed. Final BKC f55b3d3a-4db8-4e44-ba43-81770f25b31a complete; earlier source-verification attempt bdb0e24a failed before build and was corrected. Thumb sizing and legacy move callback defects found by host checks and corrected. Short 3-second playback samples on the i3: player 3.67/4.33% one-core CPU and about 45 MiB RSS; paused 0%. Shared audio service activity was also present while paused, reported separately. No sustained playback/underrun or clean EFL runtime PASS claimed. Source hashes, screenshots, rollback and limits in rack-evidence.json. Final rack left open stopped/flat.

## EJ-007 — readable EQ faders

User reports EQ controls look missing. Live capture confirms dark 22×11 px caps blend into rails. Enlarge to 30×18, retain raster backing with bright silver-blue face/highlight/grip and scale marks. Skin-only behavior change; preserve EQ values and drag/bypass wiring. Verify compiled skin on host and drag feedback. Rollback: prior native EDJ in build/out/prototype.

EJ-007 result: BKC 530fdcb2-bde7-4109-9826-d096a72913ad compiled the enlarged faders; live capture confirms bright silver-blue caps, grip and scale marks. Previous screenshot was obscured by another application; raised EQ before visual verification. No additional DSP behavior selected.

## EJ-008 — attached rack and first XMMS interaction slice

User requests stepping through fixes, then easing into XMMS widgets/features.
Objective: remove inter-window rack drag lag and add bounded familiar controls.
Plan: player/EQ/queue are retained Edje panels inside one borderless window;
visibility toggles pack/unpack panels without resetting queue or DSP. Shared
minimize/drag/close operates on one window; panel close hides its section.
Add transport Z/X/C/V/B and Space, EQ Flat reset. Floating mode, skin imports,
shuffle/repeat and broader playlist features remain later slices. Desktop audio
settings reuse is a design follow-up, not host settings replacement.
Compile remotely through BKC; verify drag, visibility, keyboard, EQ reset and
short host impact sample. Rollback: retain EJ-007 binary/EDJ before replacement.

EJ-008 steering: user selects compact default. Playback failures take priority:
real S3M uses ModPlug with no push-chain support under playbin3; MOD/XM share
that failure, MED is misidentified by typefind. Silent host probes and forced
FFmpeg/libopenmpt probes retained under build/out/prototype. All four tracker
samples decode with installed FFmpeg/OpenMPT; MP3 reaches PLAYING through
explicit pulsesink. Plan: bounded tracker PCM decoder child feeding the same
GStreamer EQ/volume/output owner, explicit desktop Pulse output for ordinary
audio, precise error logs and disabled tracker seeking until proved. Recheck
actual selected formats before claiming playback. No host decoder installation.

EJ-008 follow-up: user confirms sample playback and requests playlist file/folder
drops. Add native URI-list target, background sorted recursive scan, local audio
extension filtering, no directory-symlink traversal or media movement. Preserve
current playback. Fix invalid tracker progress by resetting and disabling the
slider when duration/seeking is unavailable; elapsed display remains active.
Dot-LCD typography recorded for the next visual pass.

EJ-008 result: compact 400px single-window panels and mixed-format audio
verified through BKC 8b8d2b42-c32a-4dc3-b11f-1c78aaa626c9. Drop/progress
follow-up compiled in BKC 20b0852a-30a0-452b-a2be-8fc2e0434c3d (complete).
Native callback/background scan test imported three expected tracks, including
escaped spaces, excluding unsupported/hidden files, directory cycles and remote
URIs. Native desktop-output regression passed MP3, S3M, MOD, XM, MED Playing,
pause/resume/stop, EQ continuity and Flat. Logs in build/out/prototype.
Actual file-manager drag gesture remains user/runtime acceptance; keyboard and
collapse assertions remain incomplete. No sustained performance acceptance.

## EJ-009 — real import paths and tracker progress

User reports folder chooser and actual drops unavailable; tracker progress static.
Plan: chooser selections and CLI directories use the background import scanner,
explicit Add-folder button, deferred chooser destruction, whole-player URI/text
drop target, local path compatibility. Read tracker duration with installed
FFprobe/OpenMPT off-thread; show progress while keeping tracker seeking disabled.
Generation token prevents stale duration applying to another track. Confirm real
Xdnd exchange and user-provided TRON folder (15 M4A files), desktop playback and
tracker bar. Rollback: retain EJ-008 binary/skin. No media movement.

EJ-009 extension: user's 700+ track collection calls for wheel paging and a
compact case-insensitive filename/folder filter. Filtered rows retain original
queue indexes; browsing must not change playback order. Tag-backed artist/album
facets remain a background metadata-index slice. EFL layered drop routing did
not complete in actual protocol tests, so the X11 prototype owns URI transfers
using Ecore_X events independent of Edje hit-test ancestry.

## EJ-010 — LCD dot typography

User selects XMMS-like dot LCD styling. Plan: preview with original blue-steel
material, embed an OFL Doto 800-weight square-dot instance for title/time only,
uppercase the display title, keep small labels/playlist/buttons readable. Font
source blobs/hashes and license retained. Generate the font instance remotely
with FontTools and embed with Edje. Rollback: retain EJ-009 font-free skin.
Acceptance: actual native readout, 400px bounds and licensing/provenance. No
desktop font installation or animated text.

EJ-009 result: real Xdnd URI transfers now pass through window-level Ecore_X;
chooser Add-folder imports the 15 TRON M4A tracks and deferred Cancel completes
without destruction diagnostics in this check. Wheel moves page 1 to page 2.
Typing derezzed yields 2/16 rows without Z transport activation after embedded
widget pointer/focus repair. Tracker duration/progress advances; Ecore owns
FFprobe children to remove the observed GLib/Ecore reaper race. Final combined
BKC d658c626-5aed-4689-8c35-b3c6f86416a7 complete. Prior folder/import iteration
74031f37 and focus iteration 0ed78219 also retained. Artist/album tag facets,
Wayland, persistence, transient status and sustained performance remain later.

EJ-010 result: 400px SVG/PNG typography study rendered before wiring. User
approves the look. Doto 800/ROND=0 instanced and embedded remotely with license
and blob/hash provenance. Native uppercase title/clock readable at 400px;
small labels/playlist remain sans-serif. Combined final build/playback checks
passed six formats, pause/resume/stop, EQ continuity and Flat with no GLib
reaper warning in retained logs. Live player refreshed; media files untouched.
Evidence, artifact hashes and remaining limits: interaction-lcd-evidence.json.

## EJ-011 — XMMS control clarity and EQ presets

User requests UI-feature refinement, clearer labels and EQ presets. Plan: Add
Music / Playlist / Equalizer labels, separate panel visibility lamps from DSP
On/Off state, matching panel titles, anchored native preset menu, Flat/Bass
Focus/Vocal/Bright/Warm original cut-only curves, Reset and Custom indication.
Keep current band frequencies and processing owner; preserve presets across
track changes and bypass. No XMMS code/curves imported. Preview controls, compile
through BKC and verify real parameter/bypass/thumb changes and native menu.
Rollback: retain EJ-010 binary/skin. Saved/user presets remain later.

EJ-011 steering: Save List snapshots the visible filter results; Load List
appends local M3U/M3U8 entries in file order without moving media or interrupting
playback. Relative paths/file URIs supported; missing/remote/unsupported entries
counted as skipped. Save As creates a new file and preserves existing files.
Channels is the next planned streaming/service collection model, not an enabled
provider or streaming claim. Verify filtered export, ordered import, relative
paths and preservation of existing output files.

EJ-011 demonstrated prerequisite: rack expansion at y=643 placed the playlist
below the 900px laptop screen. Keep expansion within the current monitor's
geometry with minimal position adjustment. Initial LCD says Add Music or
Select A Track to match queue availability. Popup interaction verified after
moving the test window on-screen; capture/input helpers support preserving
popup focus instead of raising the parent during menu checks.

EJ-011 runtime prerequisite: async software-X11 rendering delayed Save window
creation with Evas wait timeouts. A separate app-scoped synchronous-render
check opened Save normally; select this mode for the static prototype. Existing
fileselector/Ecore diagnostics remain and are not a clean-runtime PASS.

EJ-011 file-dialog fallback: the host's legacy fileselector still reports
model/focus errors; Save/Load use a native modal path entry with Save/Load/Cancel
and Enter, preserving exact export snapshots and safe new-file creation. Plain
filenames resolve under home; full paths and ~/ are accepted. Keep keyboard
transport inactive while editing the dialog. Broader file-browser repair stays
separate; Add Music retains its existing browser/drop routes.

EJ-011 filename-entry check: native Save path entry failed before file creation,
while direct writer/export checks pass. Release global transport key grabs while
search/path fields edit, rather than only ignoring their callbacks; this also
ensures grabbed letters reach native input. Save failures log the actual path
and errno. Native GUI round-trip remains required before completion.

Native Save GUI writes exactly the two Derezzed filter matches. Initial path
failures came from the synthetic typing helper dropping the leading slash after
Ctrl+A; its input stabilization now preserves the path. Long search text also
exposed unscrollable-entry minimum-width growth; enable horizontal scrolling
with compact readable styling so filtering does not widen the rack.

EJ-011 result: final BKC 0e17ff8f-ff39-4227-a2f3-925965e86785 complete.
Clear stable labels, visibility lamps, five EQ presets/Reset/Custom/bypass and
filtered M3U8 Save/Load implemented. Preset backend/fader and bypass checks,
filtered export/existing-file preservation, independent BOM/CRLF/relative/URI/
skip/order fixture and six-format playback regression passed. Actual final GUI
saved two Derezzed matches, loaded both, displayed 4/17 matches and played the
correct loaded M4A row. Final window stayed 400px wide; expanded 400x719 at y149
fits the 900px monitor with 32px clearance. Source/proof in ui-features-evidence.json.

3-second resource snapshot: playing loaded M4A 22% one-core CPU / about 75 MiB
RSS; paused 0%. Shared audio services remained active while paused and are
reported separately. This is not sustained performance/underrun acceptance.
Native final dialog test logs contain no legacy fileselector/async-render
diagnostics; Add Music's existing full browser remains a separate follow-up.
Updated UI pass left available, original player queue preserved. Channels next
slice documented in channels.md; saved custom EQ presets, tag facets, overwrite
workflow, session persistence and broader focus/accessibility remain later.


EJ-012 — 2026-10-12: native settings and multiple coordinated skins.
Plan: preview Blue Steel/Midnight/Amber Steel, use per-object Edje color classes
for live changes with no layout or engine replacement; persist theme, panel
visibility and optional volume/EQ in an atomic user settings file. Verify live
playback/queue/EQ continuity and restart restoration with isolated config.
Rollback: previous EJ-011 artifacts remain available; remove settings.ini to
restore defaults. No automatic playback, collection movement or channels work.

EJ-013 — 2026-10-12: replace modal settings with an attached right drawer.
Keep 400px rack geometry, grow window horizontally, clip/reveal the matching
skinned drawer over 180ms and stop animation when settled. Native scroller
provides room for future settings/channels sections. Playback remains accessible.
Verify opening/closing, theme/panel changes and settings save with isolated files.
Rollback: EJ-012 c458090e artifacts preserve the modal settings implementation.

EJ-012/EJ-013 result: final BKC 808d6c41-3abd-4d46-be0d-8501f3cdedd5
complete. Three original-material palettes, per-object live recoloring and
private saved theme/panels/optional volume/EQ/bypass preferences implemented.
Modal settings replaced by a matching attached 280px right drawer, opening
over 180ms with a vertically scrolling content area. Timeline ends when
settled; playback controls stay available. Fixed pixel header/control anchors
avoid stretching as the playlist increases height.

Native checks: 680px open / 400px closed, expanded 680x719, live theme and
panel toggles during one MP3 selection, Save & Close, header X and private
0600 saved preferences passed. Saved view/audio state restores on restart;
current interactive Amber choice preserved. GUI helpers use managed UTF-8
client names and raise the intended frame to avoid occluded captures.

Five-format local regression (MP3/MOD/S3M/XM/MED), all preset curves/faders,
bypass/restore, Flat and engine/queue/DSP-preserving three-theme checks passed.
Final binary hash equals the playback-tested executable; final change only
fixes drawer EDC anchors. Seagate is currently unavailable, so this run does
not recheck M4A; previous EJ-012 six-format pass covered it. Earlier EJ-012
import retest timed out during host interruption and is not claimed as a new
import PASS. No importer changes in the drawer slice.

Default workspace artifacts updated atomically. Current native drawer window
left available. Sources/hashes/checks in settings-drawer-evidence.json.
Channels/provider integrations, queue/session persistence, independent arbitrary
XMMS skin geometry and sustained resource acceptance remain follow-ups.


Release preparation — 2026-10-12: user accepts E-Jamz for publication.
Plan: preserve separate repositories, commit current source/evidence, prepare
v0.1.0 notes and verified runtime bundles, then integrate through development
and main before annotated tags/push when destination and licensing are settled.
No GitHub destination exists in local remotes; connected account is auzieman.
GitHub SSH authentication succeeds. New-repository creation is not exposed by
the connected GitHub tools; destination input remains pending. Rollback: retain
current feature branch and all prior artifacts; packaging changes no live state.

Release preparation result: current source/evidence committed; feature,
development and main integrated by fast-forward in isolated worktrees.
Runtime/source archives, checksums and offline Git bundles prepared under
build/out/releases/. Generated player resources reproduce recorded hashes;
retained native artifacts and theme package checksums pass. Python/JSON/shell
source checks pass; E-Jamz dialog/plugin positive and negative schema checks
pass in a workspace-only jsonschema environment. GitHub SSH authenticates as
auzieman. Remotes/publication/tags wait for destination and license selection.
The connected GitHub tools do not provide new-repository creation.

Publication authorization — 2026-10-12: user explicitly selected public
auzieman/E-Jamz and auzieman/BlackKnight-E-Theme repositories, GPLv3 original
code and CC BY-SA 4.0 original artwork. License texts and contribution boundaries
added; Doto OFL retained and Sci-Fi donor portions not relicensed. Browser
authenticated as auzieman provides the new-repository creation route; GitHub
connector metadata and Git SSH refs verify the result. Publish accepted main
and development, with annotated v0.1.0 tags identifying retained native proof.
Runtime archives include licenses/notices and the corresponding source URL.

GitHub publication: public E-Jamz repository created through the signed-in
browser and verified by connected GitHub metadata. Main/development/feature
branches and annotated v0.1.0 tag pushed through authenticated SSH; remote
main and peeled tag exactly match accepted local refs. User-supplied desktop
shot added to README without edits; original file/hash retained in provenance.
Release download archives contain selected licenses/notices and tagged source
URL. Full Sci-Fi donor EDJ remains outside public binary assets.

## 2026-10-12 — Native APK publication

User authorized APK packaging, repository updates and articles. Build ejamz and blackknight-wallpapers from the accepted immutable v0.1.0 source commits using pinned Alpine 3.24.2 and matching signed EFL dependencies. Keep original licenses and font notices; exclude full donor theme binary. Use BKC staged build/publication and retain logs, package/signature/integration proof. Existing signing-host availability is under investigation; use a separately signed companion feed if its key cannot be recovered without touching existing feed trust. Rollback retains the preceding feed snapshot and backed-up article/page bodies. No host installation or image rebuild is implied.

Publication result: signed ejamz 0.1.0-r0 and blackknight-wallpapers 0.1.0-r0 are published at https://auzix.auzietek.com/alpine/repo/apps/v3.24/. BKC validation run 490909fd-80e2-4c31-aa0a-c38dfbdac8b8 and publication run cc54ba45-4cdb-4c97-9c05-4400a4e34984 completed. Native APK signatures, a clean-root installation, installed launcher/skin/wallpaper checks, musl ELF linkage, HTTPS byte hashes and fresh-client signed-index/package resolution passed. Both wallpaper EDJs match the accepted release bytes. The additional public key and fingerprint are documented in AlpinEifyd/docs/companion-apks.md; the private signer is retained on edge outside Git and public directories. Existing base-feed trust remains unchanged.

The native companion apps article and two related discussion articles are published; both AUZiX/AlpinE homepages include installation links. No runtime restart, laptop installation or new boot-image acceptance is claimed.
