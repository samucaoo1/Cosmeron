#!/usr/bin/env sh
set -eu
build="${BUILD:-.build}"
make "$build/MP3MemoryParity" "$build/NativeCodecProduction" "$build/FLACIntegrity" "$build/MP3ExperimentalCompare"
dir="$(mktemp -d)"
trap 'rm -rf "$dir"' EXIT HUP INT TERM
for layout in long short mixed; do
  for rate in 32000 44100 48000; do
    for ms in 0 1; do
      for coded in 0 1; do
        for bitrate in 64 128 320; do
          python3 MP3MPEG1IntensityFixture.py "$dir/intensity.mp3" "$ms" intensity 32 "$layout" "$rate" "$coded" "$bitrate"
          COSMERON_MP3_MPEG1_INTENSITY_STRICT=1 "$build/MP3ExperimentalCompare" "$dir/intensity.mp3"
        done
      done
    done
  done
done
sh MP3LSFInterop.sh
for rate in 8000 11025 12000 16000 22050 24000 32000 44100 48000; do
  for quality in 0 4 9; do
    ffmpeg -nostdin -v error -y -f lavfi \
      -i "aevalsrc=0.2*sin(2*PI*(173*t+111*t*t))|0.15*sin(2*PI*(233*t+77*t*t)):s=$rate:d=0.75" \
      -c:a libmp3lame -q:a "$quality" "$dir/lame.mp3"
    "$build/MP3MemoryParity" "$dir/lame.mp3"
    "$build/NativeCodecProduction" "$dir/lame.mp3"
  done
done
# Independent fixed-point encoder; Lavf writes zero delay/padding here.
ffmpeg -nostdin -v error -y -f lavfi \
  -i 'aevalsrc=0.2*sin(2*PI*(173*t+111*t*t)):s=44100:d=8' \
  -c:a libshine -b:a 128k "$dir/shine.mp3"
"$build/MP3MemoryParity" "$dir/shine.mp3"
"$build/NativeCodecProduction" "$dir/shine.mp3"
# Long VBR stream crosses several checkpoint intervals.
ffmpeg -nostdin -v error -y -f lavfi \
  -i 'aevalsrc=0.2*sin(2*PI*(173*t+111*t*t)):s=44100:d=8' \
  -c:a libmp3lame -q:a 4 "$dir/long-vbr.mp3"
"$build/MP3MemoryParity" "$dir/long-vbr.mp3"
"$build/NativeCodecProduction" "$dir/long-vbr.mp3"
for encoder in libFLAC ffmpeg; do
  for depth in 16 24 32; do
    if [ "$encoder" = ffmpeg ]; then
      [ "$depth" != 32 ] || continue
      format=s32; [ "$depth" != 16 ] || format=s16
      ffmpeg -nostdin -v error -y -f lavfi \
        -i 'aevalsrc=0.2*sin(2*PI*(173*t+111*t*t)):s=48000:d=1.25' \
        -sample_fmt "$format" -c:a flac "$dir/independent.flac"
    else
      python3 FLACIndependentFixture.py "$dir/independent.flac" "$depth"
    fi
    "$build/FLACIntegrity" "$dir/independent.flac"
  done
done
