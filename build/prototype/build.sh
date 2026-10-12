#!/bin/sh
set -eu
cp /work/build/prototype/alpine-public.rsa.pub /etc/apk/keys/-6aaf02a2.rsa.pub
printf '%s\n' 'https://dl-cdn.alpinelinux.org/alpine/v3.24/main' 'https://dl-cdn.alpinelinux.org/alpine/v3.24/community' 'https://auzix.auzietek.com/alpine/repo/v3.24' > /etc/apk/repositories
apk add --no-cache build-base pkgconf efl-dev gstreamer-dev py3-fonttools
mkdir -p /work/build/out
pkg-config --modversion elementary gstreamer-1.0 > /work/build/out/dependencies.txt
apk info -v >> /work/build/out/dependencies.txt
cc -std=gnu11 -Wall -Wextra -O2 /work/src/ui/prototype.c -o /work/build/out/ejamz $(pkg-config --cflags --libs elementary gstreamer-1.0 ecore-x)
python3 -m fontTools.varLib.instancer /work/assets/fonts/Doto-variable.ttf wght=800 ROND=0 -o /work/build/out/EJamzLCD.ttf
cp /work/assets/fonts/OFL.txt /work/build/out/Doto-OFL.txt
edje_cc -fd /work/build/out -id /work/assets/skins/blue-steel/assets /work/src/ui/player.edc -o /work/build/out/player.edj
sha256sum /work/build/out/ejamz /work/build/out/player.edj > /work/build/out/SHA256SUMS
printf '%s\n' 'COMPLETE: native player binary and Edje skin compiled; playback acceptance separate.'
