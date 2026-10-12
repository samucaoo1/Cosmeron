# Native audio production paths

## Decoder selection

`Codec.h` now prefers original C11 WAV, FLAC and MP3 decoders for
`OpenMemory`, `OpenFile` and `OpenReader`. Define `AUDIO_CODEC_NATIVE_MP3=0`
before inclusion to select the miniaudio MP3 compatibility backend.
`AUDIO_CODEC_EXPERIMENTAL_NATIVE_MP3` remains a legacy opt-in alias.

For a translation unit that must contain no miniaudio implementation:

```c
#include "Cosmeron/Modules/Audio/CodecNative.h"
```

Select the profile before other desktop Audio headers in that translation
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

Normal playback populates indices lazily. `BuildSeekIndex` performs one full
native decoding pass and restores the playback position, preparing a file
for repeated random seeks. It is optional and rejects forward-only sources.

MP3 checkpoints retain exact reservoir, IMDCT overlap and polyphase history,
plus compressed offsets and presentation positions. Seeking chooses the
closest usable saved state; it does not approximate from bitrate. Intervals
are at least 64 MPEG frames, widened for large files, with at most 1,024
checkpoints. Allocation failure falls back to replay. Thus an unvisited span
still needs decoding and index construction is not free. FLAC retains up to
65,536 decoded frame positions and uses binary search. No whole PCM recording
is retained. FLAC SEEKTABLE ingestion is not yet implemented.

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
Native output is attempted first; miniaudio remains a fallback by default.
For standalone output without imported code, define
`AUDIO_DEVICE_MINIAUDIO_FALLBACK=0` before `Device/Device.h`.

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
Without it, Open returns `NOT_AVAILABLE`; default configuration can use the
compatibility fallback. A standalone native-only BSD callback build therefore
is not certified as an independent zero-link thread runtime.

No manual ALSA, WinMM, Apple framework or pthread link flags are introduced
for these native backends. ALSA keeps a shared runtime-loader reference per translation unit for process
lifetime: unloading its global configuration cache on each device Close orphaned
allocations. Publication is atomic; individual PCM devices and workers still
close normally. Native system runtimes/drivers remain dependencies;
this is not a freestanding build. Unsupported native API/format initialization
returns `NOT_AVAILABLE`, and default Open can try miniaudio. Named output does
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
complete destruction. The callback never frees itself. The miniaudio fallback
also rejects reentrant lifecycle calls, but requires owner-thread Stop.

## Validation and measured limits

`NativeCodecInterop.sh` exercises 108 authored MPEG-1 intensity combinations:
long/short/mixed, 32/44.1/48 kHz, MS off/on, coded right-channel data off/on,
and 64/128/320 kbps. It compares identical encoded bytes against miniaudio.
`MP3LSFInterop.sh` additionally verifies 408 MPEG-2/2.5 intensity combinations
across six rates, three bitrates, layouts, MS/intensity_scale and coded right
patterns. It also verifies LAME VBR at all nine sample rates and three quality settings,
Shine, long VBR indexed replay, forward-only callbacks, and independent
libFLAC/FFmpeg streams. Thresholds require normalized residual <1e-5 and gain
within 0.1%, with presentation frame counts equal for encoder fixtures.

On this host, optimized five-pass decoding of an 8-second mono Shine file
produced 1,759,915 PCM frames in 0.346 s native versus 0.0243 s miniaudio,
roughly 14.3 times the CPU time. Peak RSS was approximately 1,152/1,128 KiB;
these process measurements depend on the allocator and host and are not
portable per-decoder memory guarantees. SIMD/fast transform optimization
remains worthwhile. Native-first selection prioritizes independent code and
validated PCM, not a demonstrated performance win.

The `07-audio-fuzz.yml` workflow maintains a persistent corpus and runs
ASan/UBSan/libFuzzer over native memory, seekable and forward-only callback
paths on relevant PRs. Its weekly schedule activates after this workflow reaches
the repository default branch. Crashing inputs are uploaded as reproducers.
CRC-protected MP3 headers/side information are checked before decoder state
is published; this CRC does not cover all main data. Corrupt frames and lost
sync currently produce explicit errors. Automatic resynchronization and audio
concealment are not yet implemented, and strict preflight rejects interrupted
seekable MP3 streams.

MPEG-2.5 8-kHz mixed blocks remain a reference-disagreement case. In a baseline
fixture with a silent encoded right channel, native output preserves silence;
miniaudio emits right-channel energy (~23.24), although left PCM matches within
~1.51e-8 residual. FFmpeg's mixed output also differs. `MP3Mixed8kReference.c`
checks native silence and left parity without treating incorrect reference
right samples as expected output. Broader independent validation is still needed
before claiming complete 8-kHz mixed/intensity conformance.

Headless CI and ALSA `null` validate compilation and lifecycle only. Physical
playback, unplug/underrun behavior on actual drivers, native BSD real-driver validation, broader LSF intensity encoder coverage, APE/iTunSMPB/VBRI gapless
variants and arbitrary large-file stress remain open validation/work items.
Do not remove miniaudio or close the umbrella issues based on headless results.
