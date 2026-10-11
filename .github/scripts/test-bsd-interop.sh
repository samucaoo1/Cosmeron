#!/bin/sh
# Independent BSD interoperability checks: the actual OS gzip/unzip/bsdtar
# tools verify the Cosmeron C11 codec's output and vice versa.
set -eu
CC=${CC:-cc}
if ! command -v gzip >/dev/null 2>&1; then
  echo "[bsd-interop] ERROR: native gzip is unavailable" >&2
  exit 2
fi
if ! command -v cmp >/dev/null 2>&1; then
  echo "[bsd-interop] ERROR: native cmp is unavailable" >&2
  exit 2
fi
base=.build-bsd-extended
work="$base/interop"
mkdir -p "$work"
testbin="$base/Compression_Interop"
"$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
  Codespace/Tests/Compression/Interop.c -o "$testbin"
# The ZIP interoperability test uses this same payload.
src="$work/payload.bin"
i=0
while [ "$i" -lt 250 ]; do
  printf 'Cosmeron-BSD-compression-%s\n' "$i"
  i=$((i + 1))
done > "$src"
# gzip created by Cosmeron must be independently decompressed by native gzip.
"$testbin" encode 2 "$src" "$work/cosmeron.gz"
gzip -dc "$work/cosmeron.gz" > "$work/native-decompressed"
cmp "$src" "$work/native-decompressed"
# And a real gzip stream created by the OS must decode through Cosmeron.
gzip -c "$src" > "$work/native.gz"
"$testbin" decode 2 "$work/native.gz" "$work/cosmeron-decompressed"
cmp "$src" "$work/cosmeron-decompressed"
echo "[bsd-interop] GZIP_EXTERNAL=PASS"

# Validate ZIP archives written by Cosmeron with an independent OS decoder,
# without making optional utilities mandatory for otherwise valid BSD jobs.
"$testbin" zipwrite 0 "$src" "$work/cosmeron.zip"
zip_status=NOT_AVAILABLE
if command -v unzip >/dev/null 2>&1; then
  unzip -p "$work/cosmeron.zip" payload.bin > "$work/external-zip-content"
  cmp "$src" "$work/external-zip-content"
  zip_status=PASS
elif command -v bsdtar >/dev/null 2>&1; then
  bsdtar -xOf "$work/cosmeron.zip" payload.bin > "$work/external-zip-content"
  cmp "$src" "$work/external-zip-content"
  zip_status=PASS
fi

# In the other direction, exercise an archive made by the external zip tool
# when the command is installed; do not mislabel absence as a successful test.
zip_import=NOT_AVAILABLE
if command -v zip >/dev/null 2>&1; then
  (cd "$work" && zip -q native.zip payload.bin)
  "$testbin" zipread 0 "$work/native.zip" "$work/cosmeron-unzipped"
  cmp "$src" "$work/cosmeron-unzipped"
  zip_import=PASS
fi
printf '%s\n' "ZIP_EXTERNAL=$zip_status" "ZIP_IMPORT=$zip_import" | tee "$base/interop-status.txt"
