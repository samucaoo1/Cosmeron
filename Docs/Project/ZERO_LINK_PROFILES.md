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
