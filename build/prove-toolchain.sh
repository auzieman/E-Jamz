#!/bin/sh
set -eu
: "${EJAMZ_OUTPUT_DIR:?Set a writable artifact directory}"
: "${EJAMZ_SOURCE_REVISION:?Set the immutable source revision}"
: "${EJAMZ_TOOLCHAIN_DIGEST:?Set the compiler image digest}"
mkdir -p "$EJAMZ_OUTPUT_DIR"
project_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
printf '%s\n' "$EJAMZ_SOURCE_REVISION" > "$EJAMZ_OUTPUT_DIR/source-revision.txt"
printf '%s\n' "$EJAMZ_TOOLCHAIN_DIGEST" > "$EJAMZ_OUTPUT_DIR/toolchain-digest.txt"
apk info -v > "$EJAMZ_OUTPUT_DIR/packages.txt"
for dependency in elementary gstreamer-1.0 libopenmpt; do
    pkg-config --exists "$dependency"
    printf '%s ' "$dependency" >> "$EJAMZ_OUTPUT_DIR/dependencies.txt"
    pkg-config --modversion "$dependency" >> "$EJAMZ_OUTPUT_DIR/dependencies.txt"
done
# pkg-config emits compiler flags; intentional field splitting here.
cc "$project_root/build/probe.c" $(pkg-config --cflags --libs elementary gstreamer-1.0 libopenmpt) -o "$EJAMZ_OUTPUT_DIR/ejamz-link-probe"
printf '%s\n' 'compile-link PASS; graphical/audio runtime not tested' > "$EJAMZ_OUTPUT_DIR/result.txt"
