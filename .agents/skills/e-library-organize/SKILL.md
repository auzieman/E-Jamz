---
name: e-library-organize
description: Design or implement E-Jamz library discovery, missing-file relinking and explicitly requested music copy/move organization with playlist continuity.
---
Read ../../../docs/library-relocation.md and ../../../docs/task.md.
Determine whether the request is discovery, relinking or physical organization; scanning and playlist edits do not authorize moving media. Preserve stable item IDs, queue order and unavailable removable-volume entries.
For organization, prepare the source/destination/collection/copy-or-move plan and collision/space report before dependent file mutations. Reuse existing authorization for that concrete scope; a planning task leaves files untouched.
Use verified destination copies, non-overwriting finalization and a durable operation journal. Update all references only after the destination is verified; remove a move source only after reference commit. Handle interruption, cancellation and rollback without deleting the last verified copy.
Validate collisions, disconnected/read-only/full volumes and multi-playlist references with disposable fixtures first. Do not remount Seagate, follow external symlinks or consolidate unrelated collections incidentally.
