# Visual-led E-Jamz acceptance

## First visual slice
Produce a compact glossy blue-metal player preview before implementing the full layout. Include transport controls, track/time display, seek/volume and collapsed/expanded playlist. Test readable sizing and focus/disabled states on laptop and external displays. Minimized eplet is a later concept; no controller implementation in v0.
Use the project's e-visual-layout skill for native layouts. For new raster artwork use the available imagegen skill after reading it; use code-native EDC/vector assets for theme mechanics and reusable controls. Website creation skills apply only to an actual website task, not by analogy to native EFL.
Keep editable source and export metadata with assets. Put screenshots and annotated previews under docs/images; screenshots do not substitute for source. Record dimensions, intended scale, asset provenance and revision. Generated artwork previews are visual references, not proof of working EFL controls.

## Rendering responsibilities
EFL/Edje owns the interface; shared assets supply palette, metal texture and highlights. Cache static resources; avoid repeated decoding or per-frame rebuilding of static controls. Use state/event updates for text and transport changes.
Pause optional effects when hidden or minimized. Keep playback independent of UI visibility. Rate-limit later visualizers and make them optional; no visualizer in v0. Avoid introducing a browser engine merely to draw local-player controls.

## Runtime evidence
Measure on the actual target laptop after the first working slice, using the same source/image/package revisions and audio fixture:
- stopped, visible static window;
- ordinary audio playback, visible and minimized;
- tracker playback, visible and minimized;
- repeated playlist open/close and track transitions.
For each state retain process CPU, resident memory, observation duration, display/render backend and audio settings. Include process children and the relevant service contribution if playback is out of process. Separate startup peaks from steady state.
Acceptance: minimized UI stops optional visual updates while audio continues; static controls do not trigger continuous redraw work; sustained playback has no observed underruns; repeated operations show no unexplained progressive memory growth. Record measurements before assigning numerical budgets; no guessed universal CPU/RAM limit.
Visual acceptance and playback acceptance remain independent from the container compile/link probe. A release records both and explains unmeasured states.

## EJ-003 — actual native control resource
The first original raster/Edje resource snapshot is in assets/skins/blue-steel. It exports `bk/material/control`: textured PNG backing, live `label.activate` text, independent event area, hover/pressed/disabled descriptions and `bk,action,activate` output (source `activate`). Host sends `bk,control,disabled` or `bk,control,enabled`, source `activate`; the disabled state hides the pointer event part. Keyboard invocation must also respect disabled state in the host.

An E-Jamz UI adapter sets the label and maps activation to a transport request; the media core determines playback and reports current state back. An icon does not claim playback is working. No timers or periodic animation are used in this control. The EDC entrypoint is control.edc; compile with `edje_cc -id assets control.edc -o blue-steel.edj` through the approved remote toolchain.

This is a self-contained versioned resource export with origin revision and file hashes, not a runtime dependency on the desktop theme or theme-kit checkout. It is not yet compiled, rendered or wired into a player host. The high-resolution button texture must be checked for small-control readability and decoded-memory cost on target before it becomes a default.
