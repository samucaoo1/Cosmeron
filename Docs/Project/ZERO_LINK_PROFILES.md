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

**Not included:** WAV/FLAC/MP3 decoding, audio device playback and the
Miniaudio-backed Player (including its own “offline” engine configuration).
These still need independent per-platform linker tests.

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
`Audio_Codec_WAV_DecodeMemory` reads RIFF/WAVE **PCM unsigned 8-bit** and
**PCM signed little-endian 16-bit** into an owning float PCM buffer.
`Audio_Codec_WAV_WriteFile` writes PCM16. Both use C11 only: no Miniaudio,
threads, `-lm` or extra link flags. Compressed/float/extensible WAVE and
container formats other than RIFF/WAVE return a documented status rather
than being decoded incorrectly. Fuzzing and more WAV variants remain future
work.

## User-supplied task executor

Include `Cosmeron/Modules/Concurrency/Task/Executor.h` or
`Concurrency/Cooperative.h`. An `Executor_Submit` callback can copy and
schedule a `TTask` on the application's existing worker pool. No native
thread is automatically created by the adapter. This does **not** turn the
native `Thread`, `Mutex`, or `ThreadPool` implementations into zero-link
backends. See [EXTERNAL_EXECUTOR.md](EXTERNAL_EXECUTOR.md).


## WAV decode variants

The pure C11 `Codec/WAV.h` decoder now supports RIFF/WAVE `WAVE_FORMAT_PCM`
for unsigned 8-bit and signed little-endian **16/24/32-bit** samples, and
`WAVE_FORMAT_IEEE_FLOAT` **32-bit** samples when the host uses IEEE binary32.
Output remains interleaved float PCM; the writer deliberately remains signed
PCM16 WAV. Non-finite float input is rejected, and any allocated output is
released on rejection. `WAVE_FORMAT_EXTENSIBLE`, floating-point 64-bit,
compressed ADPCM, RF64 and big-endian RIFX are still unsupported. Regression:
`Codespace/Tests/Audio/WAVFormats.c`.
