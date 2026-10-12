# Library discovery, relinking and organization

Discovery adds references; it never relocates files. Relinking repairs missing
references. Organization explicitly copies or moves selected media into a chosen
Music root and collection subfolders. Present those as different user actions.

Library items need stable IDs, current URI/path, root identity, original path,
metadata and availability. Keep content hashes optional for discovery but required
for verifying copy/move. Identical bytes can be duplicate candidates; matching
names/tags are insufficient. Preserve playlist IDs/order and all references when
a location changes. A disconnected Seagate volume is unavailable, not deleted.
Do not guess another disk because its mount path happens to match.

## Organization plan

Show source, destination Music root, collection/folder layout, copy versus move,
file count/bytes, free space, conflicts and affected playlists before execution.
Keep collection folders by default; metadata-based artist/album organization is
optional and must preview missing tags, unsafe names and normalized collisions.
No automatic overwrite, source deletion, symlink traversal outside selected roots
or removal of companion artwork/cue files. Account for read-only mounts and
filesystem filename/case limitations; separate unsupported files for review.

Copy to a temporary file on the destination filesystem, verify size and hash,
then finalize without overwriting an existing target. Journal original and new
paths, verification and reference-update state. Commit reference changes only
after a verified destination exists. For a move, remove the source only after
that commit; cross-filesystem moves are verified copy-and-remove operations.
Interrupted work must resume or reconcile from the journal without losing the
last good file. Cancel stops at a safe item boundary and reports completed work.
Rollback restores references and files where feasible; detect new edits at either
path rather than promising unconditional undo. Never delete the last verified copy.

## Later Seagate collection task

Suggested root: a top-level `Music/` on the user-confirmed Seagate volume; exact
mount and destination are not selected. Begin with read-only inventory and a
small licensed sample subset. Preserve tracker collection folders and provenance.
The FS-UAE recovery mount may be read-only; do not remount it or reorganize other
collections as an incidental player task. No collection movement happened here.

Acceptance fixtures: same-name/different-content collision, duplicate bytes,
missing tags, disconnected source, read-only target, full destination, cancellation,
interruption after copy/before reference commit, and a track referenced by multiple
playlists. Confirm order/IDs, hashes and recoverability after each operation.
