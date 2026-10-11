# Zero-link profiles — C11

Cosmeron distinguishes link flags needed by the application from runtime
capabilities. Using a dynamic library loader does not eliminate that
library's **runtime** dependency; successful C11 compilation is also not
proof of a device backend working.

## Audio (offline synthesis)

```c
#include "Cosmeron/Modules/Audio/Offline.h"

int main(void) {
  AUDIO_PCM_TYPE(TBuffer) tone = {0};
  if (AUDIO_BEEP_FUNC(RenderTone)(440.0, 100, 0.2f, 48000, &tone)
      != STATUS_CONST(SUCCESS)) return 1;
  AUDIO_PCM_FUNC(Destroy)(&tone);
  return 0;
}
```

Works without importing the vendor Miniaudio backend. The namespace and
function signatures are unchanged. `Audio.h` remains the full desktop
aggregator; defining `AUDIO_NO_DESKTOP` still selects the former reduced
behavior. Tests: `Synthesis`, `OfflineStandalone`, `OfflineCompat` and
`OfflineMultiTU` under `Codespace/Tests/Audio/`.

The pure C11 WAV decoder is available separately through `Offline.h` /
`Codec/WAV.h`; this does **not** enable Miniaudio's FLAC or MP3 decoder.
Desktop device playback and the Miniaudio-backed Player still need native
OS runtime integration.

## Concurrency (cooperative only)

```c
#include "Cosmeron/Modules/Concurrency/Cooperative.h"
```

Provides lock-free Atomics (where supported), stackless Coroutine and
directly executed Task operations. No real parallelism, Mutex, ThreadPool,
Join or worker threads are provided by this entrypoint. This is a separate
API profile, **not** a substitute for the native threading backend.
`Codespace/Tests/Concurrency/Cooperative.c` verifies the strict C11
no-extra-link contract.

## Embedded SQLite

On FreeBSD, OpenBSD, NetBSD and DragonFly, the default implementation uses
`DATABASE_SQLITE_THREADSAFE=0` (no SQLite internal mutexes). It passed the
zero-extra-link test in the four guest VMs. Concurrent SQLite calls are
**unsupported** in this configuration, even through distinct connections.
Define `DATABASE_SQLITE_THREADSAFE=1` in the single implementation TU to
opt into mutexes; a native `-pthread` link dependency may return.

See [SQLITE_LINKING.md](SQLITE_LINKING.md) for the exact configuration.

## Audio manual playback on BSD

See [AUDIO_MANUAL_BSD.md](AUDIO_MANUAL_BSD.md). Include
`Cosmeron/Modules/Audio/Device/Manual.h` to use the callback-driven
`Manual_Open`, `Manual_Start`, `Manual_Pump`, `Manual_Stop`, and
`Manual_Close` API. Each pump call renders interleaved float PCM, converts
to S16LE and writes using the host calling thread. There is no automatic
thread or Miniaudio vendor dependency in this header.

FreeBSD / DragonFly: native OSS; NetBSD: audio(4); OpenBSD: sndio resolved
through `dlopen`. OpenBSD has **no explicit `-lsndio` flag**, but *does*
require the sndio shared library and working sndiod/audio device at
runtime. These backends are compile/link tested on real BSD VMs; physical
playback is **not** yet certified.

## WAV codec without vendor

Include `Cosmeron/Modules/Audio/Codec/WAV.h` or `Audio/Offline.h`.
`Audio_Codec_WAV_DecodeMemory` supports RIFF/WAVE unsigned PCM8,
signed PCM16/24/32 and IEEE float32 (on IEEE binary32 hosts) into an owning
interleaved float PCM buffer. `Audio_Codec_WAV_WriteFile` writes PCM16.
Neither imports Miniaudio, threads or libm. Compressed/Extensible WAVE,
RF64 and big-endian RIFX are unsupported; inputs are bounded by RIFF and
data chunk lengths. The `NativeWAV`, `WAVRobustness` and `WAVFormats`
test programs verify supported formats and reject malformed files.

## User-supplied task executor

Include `Cosmeron/Modules/Concurrency/Task/Executor.h` or
`Concurrency/Cooperative.h`. An `Executor_Submit` callback can copy and
schedule a `TTask` on the application's existing worker pool. No native
thread is automatically created by the adapter. This does **not** turn the
native `Thread`, `Mutex`, or `ThreadPool` implementations into zero-link
backends. See [EXTERNAL_EXECUTOR.md](EXTERNAL_EXECUTOR.md).


## Miniaudio decoder-only profile (without device threads)

Include `Cosmeron/Modules/Audio/CodecMinimal.h` to retain the public
`Audio_Codec_*` decoder API, including Miniaudio MP3/FLAC support, but
compile the pinned Miniaudio vendor with Device I/O, native threading,
resource manager, engine, node graph and generation APIs disabled.

```c
#include "Cosmeron/Modules/Audio/CodecMinimal.h"

int main(void) {
  AUDIO_CODEC_TYPE(TDecoder) decoder = {0};
  /* Audio_Codec_OpenFile(&decoder, "sample.flac");
     ... Audio_Codec_Close(&decoder); */
  return 0;
}
```

This is **not** the same as the no-vendor WAV decoder above. On the
four BSD VMs, actual memory decoding in this profile passed with `-lm`
and **without `-pthread`**, but failed to link with no extra flags.
The result is recorded as `AUDIO_CODEC_MINIMAL_FUNCTIONAL=PASS` and
`AUDIO_CODEC_MINIMAL_ZERO_LINK=NOT_SUPPORTED`.

A translation unit must select either `CodecMinimal.h` or `Desktop.h`
because the embedded Miniaudio configuration is fixed by its first include.
The unchanged `CodecOnly.h` preserves the old full-vendor profile.
Miniaudio's decoder-specific features (including MP3/FLAC) remain covered
by their respective full and minimal vendor tests.

The Linux math adapter currently uses `pthread_once` to initialize
runtime math symbols, even though the minimal Miniaudio build itself
creates no worker threads. This configuration does not claim to avoid
every possible runtime dependency; it only avoids new user `-pthread`
link flags where tested.

**Remaining gap:** the complete threaded device/Player backend still
needs its platform-specific libraries. Hardware tests remain opt-in.
