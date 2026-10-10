---
name: e-media-backend
description: Investigate or implement the E media player engine and provider adapters using Clementine as a feature/backend reference and verified provider authorization and playback support.
---
Read ../../../docs/project-contract.md, ../../../docs/brief.md and ../../../docs/task.md.
Select the requested investigation or playback slice. Inspect upstream components, dependencies and licenses before proposing reuse; distinguish Clementine application code from standalone media libraries.
Keep presentation, application state, engine and provider adapters separate. Validate authentication, catalog access and actual playback independently.
Spotify compliance is required: unofficial playback and successful OAuth are not evidence of provider approval. Establish supported Linux playback and distribution conditions; username/password belongs in supported provider-owned login, and QR mode requires supported pairing/authentication.
For investigation, produce a source-backed component/feasibility map and bounded next action. For implementation, verify actual output, state/error propagation and the selected slice's acceptance criteria. Do not register provider apps, alter accounts or launch external work without task authorization.
Record outcomes and remaining uncertainty in the active task; keep additional providers and UI redesigns as follow-ups.
