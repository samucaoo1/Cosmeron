# Internal desktop backend

`Miniaudio.inc` is derived from **miniaudio 0.11.23**, including its embedded
WAV, MP3 and FLAC decoders. Upstream source:
<https://github.com/mackron/miniaudio/blob/0.11.23/miniaudio.h>.

The upstream license text (public-domain/MIT alternatives) is retained at the
end of the imported file. Cosmeron's authored adapters follow the repository
license. Do not remove the upstream notices.

Reproduce the import from the repository root:

```sh
python3 .github/scripts/vendor-audio.py /path/to/miniaudio.h
```

The importer verifies the pinned SHA-256 and makes these changes:

1. Prefix miniaudio/decoder identifiers, including configuration macros and
   include guards, with `cosmeron_audio_vendor_`. Strings/comments are preserved,
   especially dynamically loaded OS symbol names.
2. Redirect `sin`, `exp`, `log`, `pow`, and `sqrt` to internal math adapters.
3. Coalesce the upstream global atomic fallback lock with GCC/Clang `weak` or
   MSVC `selectany`; making it TU-local would break synchronization across TUs.

The enclosing adapter selects static API linkage. Each translation unit contains
its own implementation; no implementation macro or separately linked `.c` file
is required. This trades compile time and unoptimized executable size for the
existing header-only contract. Upstream warnings are isolated locally; authored
Cosmeron code is compiled with normal strict diagnostics.

Linux desktop targets **glibc >= 2.34**, a normal dynamically linked executable,
and installed system audio libraries. Math symbols are loaded from `libm.so.6`
once per TU via `pthread_once`. Successful handles stay loaded for process
lifetime; failed initialization is reported before entering the backend. Math
adapters also prepare their own TU when an object crosses TU boundaries.
Windows/macOS use their normal CRT/system math. No extra link flags are supplied
in the test matrix. Unsupported platforms do not acquire a zero-link promise.

A null desktop backend is disabled: opening a nonexistent device must not claim
to play audible sound. Offline mixing is an explicit Player configuration.
`Audio/Offline.h` is now the explicit dependency-free entrypoint for PCM,
Oscillator, Envelope, Sequence and offline Beep rendering. It does not include
the Miniaudio implementation and passes strict C11 multi-translation-unit
link tests **without `-lm` or `-pthread`**. Legacy
`#define AUDIO_NO_DESKTOP` before including `Audio.h` is still supported.

Do not confuse the no-device `Player` engine (which still imports Miniaudio)
with the independent `Offline.h` synthesis path. `Codec`, `Player` and
`Device` remain desktop-backend features until separately certified.

Do not expose vendor types through public function signatures. Public objects
contain opaque state pointers; callers must not inspect the pointed-to state.

## Public entrypoints

- `Audio/Offline.h`: pure synthesis and PCM; no vendor backend.
- `Audio/CodecOnly.h`: offline synthesis and decoder/encoder public API,
  **without public Device/Player/Mixer headers**. The decoder still includes
  Miniaudio internally; its native math/thread dependencies may remain.
- `Audio/Desktop.h`: explicitly imports Codec, Device, Player, Mixer and
  Beep playback. Device access requires hardware/permissions at runtime.
- `Audio/Audio.h`: backwards-compatible aggregator; `AUDIO_NO_DESKTOP`
  preserves the offline-only behavior.

The CI probes Codec and Device **separately**, distinguishes functional
compatibility from zero-extra-link, and tests both standalone include paths.

## Optional decoder-only vendor build

`CodecMinimal.h` is an explicit profile for the same `Audio_Codec_*` API.
It configures miniaudio 0.11.23 with
`MA_NO_DEVICE_IO`, `MA_NO_THREADING`, `MA_NO_RESOURCE_MANAGER`,
`MA_NO_ENGINE`, `MA_NO_NODE_GRAPH`, and `MA_NO_GENERATION`.
This can reduce vendor footprint and removes its internally created device
threads. It does **not** automatically remove `-lm`, convert MP3/FLAC to
native Cosmeron codecs, or certify hardware. The generated code may still
depend on the host's math runtime.

`CodecMinimal.h` and `Desktop.h` must not be combined in **one TU**;
separate TUs may each select their own build profile, with only public
opaque decoder types crossing interfaces. The preexisting `CodecOnly.h`
remains full-profile compatible. The C11 regression `CodecMinimal.c`
executes a real WAV decode and the BSD CI records an independent
`AUDIO_CODEC_MINIMAL_ZERO_LINK` result.

### BSD codec-only math

For `AUDIO_MINIAUDIO_CODEC_MINIMAL` on the four BSD families, the five
vendor math functions are resolved once per translation unit from a system
`libm` shared object via `dlopen`/`dlsym`. A C11 atomic state machine
serializes initialization without introducing `pthread_once` or requiring
`-pthread` for the decoder-only profile. The library handle is deliberately
kept alive for the process lifetime. A missing or incompatible runtime math
library causes `Audio_Codec_Open*` to return `NOT_AVAILABLE` instead of
attempting a null function-pointer call. This feature removes the need to
pass `-lm` during **linking**, not the runtime dependency on `libm`.
The standard threaded desktop Miniaudio profile remains unchanged.

## BSD threaded backend and implicit linker support

The decoder-only `CodecMinimal.h` and explicit `Device/Manual.h` pump retain
their real zero-extra-linking API. They have different threading semantics
from the full desktop Miniaudio backend; they are **not** drop-in replacements
for background device callbacks or a native thread pool.

The full backend can embed `#pragma comment(lib, "pthread")` and `"m"`
on BSD **Clang + ELF-autolinking linkers**. This can remove manual
`-pthread`/`-lm` command-line arguments. The compiler may still record
`libpthread`/`libthr`/`libm` as dynamically linked dependencies. It is
not a claim of dependency-free linkage, and GCC or GNU ld may still need
normal linker options. Define `COSMERON_NO_IMPLICIT_LINK` to suppress the
pragma. No pthread calls are reimplemented via unsafe `dlopen` shims; on
some BSD variants the pthread runtime must be loaded before application
startup for reliable synchronization and errno interposition.

The BSD CI records both the strict no-library outcome and a distinct
`NO_MANUAL_FLAGS` result. The former does **not** become PASS merely because
the linker obtained pthread automatically.
