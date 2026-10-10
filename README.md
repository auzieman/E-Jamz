# E-Jamz

Local-first EFL audio player preparation; no playback implementation or release yet.

See [v0 contract](docs/v0-contract.md), [interfaces](docs/interfaces.md) and [pipeline preparation](docs/pipeline.md).

Read [brief](docs/brief.md), [feature map](docs/feature-map.md), [active task](docs/task.md), [shared design/ownership contract](docs/project-contract.md), [branching](docs/branching.md) and [remote toolchain](docs/toolchain.md).

```text
src/ui/ — EFL presentation
src/core/ — application state/library/queue
src/engine/ — local media engine adapters
src/providers/ — authentication/catalog/playback adapters
assets/ — application assets
build/ — toolchain/build preparation
docs/images/ — screenshots and visual evidence
.agents/skills/ — project workflows
```

Work from development into feature/<topic>; merge accepted features back to development. Validated releases merge to main and receive annotated version tags. Source licensing and reused-asset notices must be established before distribution.

Preparation references: [visual/performance acceptance](docs/visual-performance.md) and [remote readiness](docs/preflight.md).
