# Native audio production paths

## Decoder selection

`Codec.h` provides pure original C11 WAV, FLAC and MP3 decoders for
`OpenMemory`, `OpenFile` and `OpenReader`. All external dependencies have been removed.

For a translation unit including the decoder API:

```c
#include "Cosmeron/Modules/Audio/CodecNative.h"
```

Use consistent compilation profiles in translation units sharing live Audio
handles. Select the profile before other desktop Audio headers in that translation
unit. `CodecNative.h` provides the shared decoder API without imported
code, math, threads or device libraries. WAV and FLAC callback readers
still require seeking. MP3 readers accept `TReader.seek = NULL`.

## Forward-only MP3

The reader owns its cursor; Cosmeron never rewinds a forward-only source.
Opening reads the first complete frame and skips ID3 metadata in bounded
chunks. Reads retain partial compressed-frame delivery across callback
errors. Storage is one compressed frame and one PCM frame, independent of
stream length. Xing/LAME/Lavf/Lavc trim includes synthesis priming, including
encoders with short or zero trailing padding.

`GetInfo.lengthKnown` is false until EOF when metadata does not provide a
length; `frameCount` is zero while unknown. For trusted Xing gapless metadata
it is known immediately. Early EOF inside a promised duration is incomplete,
not successful truncation. ID3v1 trailers are accepted.

Seeking inside the current PCM frame needs no I/O. Forward seeks consume
input and cannot roll back a failed nonseekable read. Older backward seeks
return `NOT_SUPPORTED`. Reader state and input bytes are borrowed until Close.
A failed OpenReader may have consumed bytes from a forward-only source.

## Indices

```c
OPSTATUS Audio_Codec_BuildSeekIndex(Audio_Codec_TDecoder *decoder);
```

Normal playback populates exact-state checkpoints and FLAC indices lazily. `BuildSeekIndex` performs one full
native decoding pass and restores the playback position, preparing a file
for repeated random seeks. It is optional and rejects forward-only sources.

Seekable MP3 opens collect byte offsets in the existing header scan. The
thin index initially stores every 64th frame, compacts by doubling its stride
at 65,536 entries, and uses at most 1 MiB. A cold distant seek loads a nearby
offset, feeds at least 512 predecessor payload bytes into the reservoir,
then decodes two predecessor frames to restore hybrid and synthesis history.
It does not decode the entire prefix. Nonzero VBR and long/short/mixed
MPEG-1/2/2.5 tests compare seek output byte-for-byte with continuous decode.

Playback also retains sparse exact-state checkpoints (up to 1,024, intervals
at least 64 MPEG frames). Nearby seeks reuse those states. Allocation failure
falls back to available states or replay. Recovery mode disables the thin
index because damaged boundaries need sequential interpretation.

FLAC retains up to 65,536 decoded frame positions and uses binary search. No
whole PCM recording is retained. FLAC SEEKTABLE ingestion is not yet implemented.

## FLAC integrity

```c
OPSTATUS Audio_Codec_FLAC_VerifyMemory(const void *data, size_t size);
OPSTATUS Audio_Codec_VerifyIntegrity(Audio_Codec_TDecoder *decoder);
```

MD5 verification is explicit and optional. It hashes exact signed integer
samples in channel-interleaved little-endian order at the original precision,
before float conversion. It supports 16/24/32-bit files from libFLAC, including
32-bit depth in the frame header. `VerifyIntegrity` handles native memory,
file and seekable callback FLAC and restores the playback position. Missing
all-zero STREAMINFO MD5 returns `NOT_AVAILABLE`; mismatches return
`INVALID_SEQUENCE`. MD5 here detects corruption; it is not authentication.

## PCM

`FromS16`/`ToS16`, `FromS24LE`/`ToS24LE` and `FromS32`/`ToS32` use signed PCM
normalization. Integer conversions saturate and round to nearest, and reject
nonfinite input before publishing output. `Sanitize` maps nonfinite float
samples to silence and clips to [-1,1]. `Normalize` explicitly scales a finite
array to a requested peak in (0,1]. Decoders retain float headroom rather than
silently clipping valid decoded overshoots. Float32 cannot preserve every
low bit of a 32-bit integer sample; FLAC MD5 does not suffer this limitation.

## Output devices

Existing `Open`, `Start`, `Stop`, `IsStarted` and `Close` are preserved.
Device output uses pure original C11 OS backends with dynamic runtime zero-link loading.

| OS | Native backend | PCM | Device ID |
| --- | --- | --- | --- |
| Linux | Runtime-loaded ALSA | float32 interleaved | ALSA PCM name |
| Windows | Runtime-loaded waveOut | signed16 interleaved, double-buffered | Numeric output ID |
| macOS | Runtime-loaded AudioQueue/CoreAudio | float32 interleaved | CoreAudio device UID |
| FreeBSD/DragonFly | OSS | native-endian signed16 | `/dev/dsp*` path |
| OpenBSD | Runtime-loaded sndio | native-endian signed16 | sndio name (`rsnd/N`) |
| NetBSD | audioio | native-endian signed16 | `/dev/audio*` path |

BSD workers require a thread runtime already loaded by the host at startup.
`RTLD_NOLOAD` explicitly prevents unsafe late loading of libthr/libpthread.
Without it, Open returns `NOT_AVAILABLE`. A standalone native-only BSD callback build therefore
is not certified as an independent zero-link thread runtime.

No manual ALSA, WinMM, Apple framework or pthread link flags are introduced
for these native backends. ALSA keeps a shared runtime-loader reference per translation unit for process
lifetime: unloading its global configuration cache on each device Close orphaned
allocations. Publication is atomic; individual PCM devices and workers still
close normally. Native system runtimes/drivers remain dependencies;
this is not a freestanding build. Unsupported native API/format initialization
returns `NOT_AVAILABLE`. Named output does
not silently choose a different device. ALSA allows its PCM plugin layer to
resample; other backends accept the requested application PCM format or fail.

```c
OPSTATUS Audio_Device_OpenNamed(Audio_Device_TDevice *device,
    const Audio_Device_TConfig *config, const char *id);
OPSTATUS Audio_Device_Enumerate(Audio_Device_TEnumerateCallback callback, void *userData);
OPSTATUS Audio_Device_GetFormat(const Audio_Device_TDevice *device,
    uint32_t *outRate, uint32_t *outChannels);
OPSTATUS Audio_Device_GetStatus(const Audio_Device_TDevice *device);
```

Enumerate borrows ID/name strings for the callback duration. GetFormat reports
the callback format, not the physical mixer format. Lifecycle calls belong
to one serialized owner thread. Render runs on the native worker/queue and
must not block. ALSA underruns are recovered; fatal write/enqueue errors set
observable status and stop output. Stop joins/quiesces the worker before
resources are released. A native render callback may request Stop/Close:
Stop returns `BUSY`, output stops, and a subsequent owner-thread Close must
complete destruction. The callback never frees itself.

## Validation and measured limits

`NativeCodecInterop.sh` exercises 108 authored MPEG-1 intensity combinations:
long/short/mixed, 32/44.1/48 kHz, MS off/on, coded right-channel data off/on,
and 64/128/320 kbps. It verifies identical encoded bitstreams against reference decoder output.
`MP3LSFInterop.sh` additionally verifies 408 MPEG-2/2.5 intensity combinations
across six rates, three bitrates, layouts, MS/intensity_scale and coded right
patterns. It also verifies LAME VBR at all nine sample rates and three quality settings,
Shine, long VBR indexed replay, forward-only callbacks, and independent
libFLAC/FFmpeg streams. Thresholds require normalized residual <1e-5 and gain
within 0.1%, with presentation frame counts equal for encoder fixtures.

On this host, optimized five-pass decoding of an 8-second mono Shine file
produced 1,759,915 PCM frames in 0.346 s native. Peak RSS was approximately 1,152 KiB;
these process measurements depend on the allocator and host and are not
portable per-decoder memory guarantees. SIMD/fast transform optimization
remains worthwhile. Native-first selection prioritizes independent code and
validated PCM, not a demonstrated performance win.

The `07-audio-fuzz.yml` workflow maintains a persistent corpus and runs
ASan/UBSan/libFuzzer over native memory, seekable and forward-only callback
paths on relevant PRs in both strict and recovery modes. The default-branch
`08-audio-native-weekly.yml` wrapper checks out `audio-module` and runs those
profiles every Monday at 04:23 UTC, with manual dispatch also available. This
keeps the schedule active before the full Audio branch reaches `main` (PR #164).
Crashing inputs are uploaded as reproducers and the corpus persists between runs.
CRC-protected MP3 headers/side information are checked before decoder state
is published; this CRC does not cover all main data. Strict mode reports corrupt frames and lost synchronization as errors.
Define `AUDIO_CODEC_MP3_RECOVER_ERRORS=1` to enable bounded resynchronization
(up to `AUDIO_CODEC_MP3_MAX_RESYNC_BYTES`, default 65,536) and silence for
complete damaged frames. I/O failures, allocation failures and truncated
frames remain errors. `GetRecoveryCounts` reports discarded compressed bytes
and concealed PCM frames in the current seek-restorable state. Recovery is
lossy and gapless trimming is disabled after preflight resynchronization;
forward-only streams discard duration/trim assumptions after lost sync.

MPEG-2.5 8-kHz mixed blocks: in a baseline
fixture with a silent encoded right channel, native output preserves silence;
external reference decoders emit spurious right-channel energy (~23.24), although left PCM matches within
~1.51e-8 residual. FFmpeg's mixed output also differs. `MP3Mixed8kReference.c`
checks native silence and left parity without treating incorrect reference
right samples as expected output. Broader independent validation is still ongoing.

Headless CI and ALSA `null` validate compilation and lifecycle. Physical
playback, unplug/underrun behavior on actual drivers, native BSD real-driver validation, broader LSF intensity encoder coverage, APE/iTunSMPB/VBRI gapless
variants and physical large-file stress remain open continuous validation items.
A virtual 4.33-GB source tests 64-bit offsets/counters and a bounded cold seek;
it does not establish physical disk performance.
All legacy third-party audio backends (including Miniaudio) have been completely removed.

