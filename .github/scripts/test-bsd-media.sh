#!/bin/sh
# Native Compression and Audio codec/device checks inside BSD guest VMs.
# Consolidates the former per-module BSD workflows; strict C11, no extra
# linker flags, default BSD header profile (as those workflows used).
set -eu
CC=${CC:-cc}
build=.build-bsd-media
mkdir -p "$build"
root=$(pwd)
echo "[bsd-media] $(uname -s) $(uname -m), compiler=$CC"

for test in NativeInflate NativeCodec Zip Allocation Robustness Codec; do
  echo "[bsd-media] Compression/$test"
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
    "Codespace/Tests/Compression/$test.c" -o "$build/Compression_$test"
  "$build/Compression_$test"
done

# FreeBSD carries the full native FLAC/MP3 decoder list; the other guests run
# the device lifecycle and production codec subset that their audio headers
# (sndio, audioio, OSS) were validated against.
case "$(uname -s)" in
  FreeBSD)
    audio_tests="MP3Recovery DeviceNative PCMProduction NativeCodecProduction \
      MP3CRC FLACIntegrity CodecRobustnessFuzz FLACMetadata FLACFrames \
      FLACArithmetic FLACStandalone MP3Headers MP3ID3Readers MP3MutationFuzz \
      MP3SideInfo MP3Scalefactors MP3ScalefactorsLSF MP3Huffman \
      MP3HuffmanRegions MP3Requant MP3Hybrid MP3IMDCT MP3Synthesis \
      MP3Intensity MP3JointStereo MP3JointStereoLSF MP3Xing MP3Experimental \
      MP3MemorySmoke CodecMP3OptIn CodecMP3StreamSmoke CodecMP3ReaderFaults"
    ;;
  *)
    audio_tests="MP3Recovery DeviceNative PCMProduction NativeCodecProduction \
      FLACIntegrity MP3CRC"
    ;;
esac

cd Codespace/Tests/Audio
for test in $audio_tests; do
  echo "[bsd-media] Audio/$test"
  "$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror "$test.c" \
    -o "$root/$build/Audio_$test"
  "$root/$build/Audio_$test"
done
echo "[bsd-media] Audio/DeviceGuardTU"
"$CC" -std=c11 -Wall -Wextra -Wpedantic -Werror \
  DeviceGuardTU_A.c DeviceGuardTU_B.c -o "$root/$build/Audio_DeviceGuardTU"
"$root/$build/Audio_DeviceGuardTU"
echo "[bsd-media] all Compression and Audio checks passed"
