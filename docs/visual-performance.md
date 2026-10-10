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
