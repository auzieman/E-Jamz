# Remote-run readiness
Preparation is locally committed; no published repository or remote build exists yet.

| Requirement | Current evidence | Next action |
|---|---|---|
| Immutable source | Local feature commits | Select exact revision and stage an archive through BKC; no public hosting needed |
| Toolchain base | Pinned Alpine Dockerfile | Build remotely and record digest |
| EFL/dependency feed | Candidate packages only | Verify matching-release signed EFL feed/key, package versions and dependency closure |
| Scheduler target | Swarm/Kubernetes permitted, target not selected | Inspect existing capacity and choose node/namespace/storage through BKC |
| CLI-owned execution | Existing allowlisted workflows; EJ recipe unsupported | Trace generic staging/execute/collect primitives and add only missing core capability in a bounded BKC task |
| Compile/link proof | Shell/C assets, syntax checked | Run remotely and retain artifacts/log; do not claim runtime proof |
| Layout/artwork | Brief and visual skill | Produce player/playlist previews in selected visual task |
| Runtime impact/audio | Acceptance defined, no measurements | Test target laptop after working implementation |

The prepared pipeline remains draft until the core execution path and required inputs are established. Scheduler manifests should be derived from the chosen environment, not speculative resources. Neither a new executor branch nor hardcoded target credentials resolves readiness.
