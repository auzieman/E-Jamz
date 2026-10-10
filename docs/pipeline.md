# E-Jamz toolchain proof pipeline
Prepared BKC recipe: BKC-realignment/pipelines/ejamz-toolchain-proof. Status draft, not supported by the current workflow allowlist. No executor/worker/UI code was added.
Before enabling, map staging, signed dependency preparation, remote container launch and artifact collection onto the CLI-owned core. If a generic primitive is absent, add it there under a separate bounded task; never add an ejamz-only worker/executor branch.
Required runtime inputs: exact source commit, chosen swarm worker or Kubernetes target/namespace, immutable compiler image digest, signed matching-release dependency provenance and writable artifact storage. No production defaults or copied credentials.
Stages: resolve inputs -> stage immutable source -> prepare/reuse verified compiler image -> invoke build/prove-toolchain.sh -> collect package inventory, dependency versions, source/image identifiers, link probe and log.
The probe compiles/links only. It does not run Elementary against a display, play music or validate backend selection. A separate runtime task supplies licensed audio fixtures and observes actual output.
Container builds and probe execution occur remotely via BKC; local checks cover schema/shell syntax only. No pipeline run has occurred.

Preflight gaps are tracked in preflight.md; visual/audio/performance evidence follows visual-performance.md and belongs to later runtime acceptance, not this link probe.
