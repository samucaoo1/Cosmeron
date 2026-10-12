#!/usr/bin/env sh
set -eu

# Diagnostic fixtures only. Production decoding never depends on ffmpeg.
# Two non-periodic signals make lag estimation less ambiguous than Tone.mp3.
command -v ffmpeg >/dev/null 2>&1 || {
  echo "ffmpeg is required for the host-side diagnostic job" >&2
  exit 1
}
dir="$(mktemp -d)"
trap 'rm -rf "$dir"' EXIT HUP INT TERM
cc -std=c11 -Wall -Wextra -Wpedantic -Werror \
  MP3ExperimentalCompare.c -o "$dir/mp3compare"

# Time-varying phase plus distinct partials. Mono and independent L/R
# stereo stress granule transitions without involving intensity stereo.
ffmpeg -nostdin -hide_banner -loglevel error -y \
  -f lavfi \
  -i 'aevalsrc=0.19*sin(2*PI*(173*t+210*t*t))+0.11*sin(2*PI*617*t)+0.06*sin(2*PI*947*t):s=44100:d=0.46' \
  -ac 1 -ar 44100 -c:a libmp3lame -b:a 128k "$dir/mono.mp3"
ffmpeg -nostdin -hide_banner -loglevel error -y \
  -f lavfi \
  -i 'aevalsrc=0.17*sin(2*PI*(181*t+240*t*t))+0.08*sin(2*PI*487*t)|0.16*sin(2*PI*(349*t+157*t*t))+0.07*sin(2*PI*863*t):s=44100:d=0.46' \
  -ac 2 -ar 44100 -c:a libmp3lame -joint_stereo 0 -b:a 192k "$dir/stereo.mp3"

# Exercise MPEG-1's other two standard sampling frequencies as well.
ffmpeg -nostdin -hide_banner -loglevel error -y \
  -f lavfi \
  -i 'aevalsrc=0.19*sin(2*PI*(163*t+190*t*t))+0.11*sin(2*PI*541*t)+0.06*sin(2*PI*877*t):s=32000:d=0.46' \
  -ac 1 -ar 32000 -c:a libmp3lame -b:a 96k "$dir/mono32.mp3"
ffmpeg -nostdin -hide_banner -loglevel error -y \
  -f lavfi \
  -i 'aevalsrc=0.16*sin(2*PI*(191*t+240*t*t))+0.09*sin(2*PI*457*t)|0.17*sin(2*PI*(317*t+140*t*t))+0.06*sin(2*PI*911*t):s=48000:d=0.46' \
  -ac 2 -ar 48000 -c:a libmp3lame -joint_stereo 0 -b:a 192k "$dir/stereo48.mp3"

# Correlated channels encourage MS frames in LAME.
ffmpeg -nostdin -hide_banner -loglevel error -y \
  -f lavfi \
  -i 'aevalsrc=0.18*sin(2*PI*(211*t+151*t*t))+0.08*sin(2*PI*673*t)|0.16*sin(2*PI*(211*t+151*t*t))+0.07*sin(2*PI*673*t):s=44100:d=0.46' \
  -ac 2 -ar 44100 -c:a libmp3lame -joint_stereo 1 -b:a 128k "$dir/jointMS.mp3"

# Exercise the standalone MPEG-2/2.5 scalefactor parser against actual
# bitstreams (this is not an LSF PCM equivalence test).
cc -std=c11 -Wall -Wextra -Wpedantic -Werror \
  MP3LSFFixtures.c -o "$dir/lsfcheck"
ffmpeg -nostdin -hide_banner -loglevel error -y \
  -f lavfi \
  -i 'aevalsrc=0.2*sin(2*PI*(193*t+100*t*t))+0.05*sin(2*PI*573*t):s=22050:d=0.48' \
  -ar 22050 -ac 1 -c:a libmp3lame -b:a 64k "$dir/mpeg2-lsf.mp3"
ffmpeg -nostdin -hide_banner -loglevel error -y \
  -f lavfi \
  -i 'aevalsrc=0.15*sin(2*PI*217*t)|0.12*sin(2*PI*(331*t+150*t*t)):s=11025:d=0.48' \
  -ar 11025 -ac 2 -c:a libmp3lame -b:a 32k "$dir/mpeg25-lsf.mp3"
"$dir/lsfcheck" "$dir/mpeg2-lsf.mp3" 2
"$dir/lsfcheck" "$dir/mpeg25-lsf.mp3" 25
# LSF PCM comparisons are diagnostic until numerical parity is demonstrated.
"$dir/mp3compare" "$dir/mpeg2-lsf.mp3"
"$dir/mp3compare" "$dir/mpeg25-lsf.mp3"

COSMERON_MP3_PARITY_STRICT=1 "$dir/mp3compare" "$dir/mono.mp3"
COSMERON_MP3_PARITY_STRICT=1 "$dir/mp3compare" "$dir/stereo.mp3"
COSMERON_MP3_PARITY_STRICT=1 "$dir/mp3compare" "$dir/mono32.mp3"
COSMERON_MP3_PARITY_STRICT=1 "$dir/mp3compare" "$dir/stereo48.mp3"
COSMERON_MP3_PARITY_STRICT=1 COSMERON_MP3_REQUIRE_MS=1 "$dir/mp3compare" "$dir/jointMS.mp3"
