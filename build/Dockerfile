FROM alpine:3.24.2@sha256:d56c381f961d307a21b3ca004cf1e3910f106644aefb1f43e654c8a56c4fd395
RUN apk add --no-cache alpine-sdk python3 meson ninja cmake pkgconf git &&     adduser -D -u 1000 builder && addgroup builder abuild &&     mkdir -p /work /outputs && chown builder:builder /work /outputs
USER builder
WORKDIR /work
# EFL packages are installed in a verified derived image; see docs/toolchain.md.
CMD ["sh"]
