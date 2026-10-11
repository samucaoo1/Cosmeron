#!/bin/sh
# Exercise real WAV, MP3 and FLAC decoding under the native Linux libc.
# In particular, musl exposes math functions from libc, not libm.so.6.
# No additional -lm, -ldl or -pthread linker switches are allowed here.
set -eu
CC=${CC:-cc}
mkdir -p .build-linux-codec
for test in CodecMinimal CodecMath CodecCompressed; do
  echo "[linux-codec] $test, strict C11, zero extra linker flags"
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    "Codespace/Tests/Audio/$test.c" \
    -o ".build-linux-codec/$test"
  ".build-linux-codec/$test"
done
echo "[linux-codec] PASS (libc/libm runtime dependencies still apply)"
