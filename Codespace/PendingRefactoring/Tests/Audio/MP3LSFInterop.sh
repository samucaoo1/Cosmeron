#!/usr/bin/env sh
set -eu
cd "$(dirname "$0")"
build="${BUILD:-.build}"
make "$build/MP3MemoryParity" "$build/MP3Mixed8kReference"
dir="$(mktemp -d)"
trap 'rm -rf "$dir"' EXIT HUP INT TERM
for rate in 8000 11025 12000 16000 22050 24000; do
  for layout in long short mixed; do
    if [ "$rate" = 8000 ] && [ "$layout" = mixed ]; then
      python3 MP3LSFIntensityFixture.py "$dir/baseline8.mp3" 0 0 baseline-mixed 8000 64 0
      "$build/MP3Mixed8kReference" "$dir/baseline8.mp3"
      continue # Independent references disagree on this MPEG-2.5 layout.
    fi
    for ms in 0 1; do
      for scale in 0 1; do
        for coded in 0 1; do
          for bitrate in 16 32 64; do
            python3 MP3LSFIntensityFixture.py "$dir/lsf.mp3" "$ms" "$scale" "$layout" "$rate" "$bitrate" "$coded"
            "$build/MP3MemoryParity" "$dir/lsf.mp3"
          done
        done
      done
    done
  done
done
