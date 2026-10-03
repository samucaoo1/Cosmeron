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
`AUDIO_NO_DESKTOP` removes this integration from `Audio.h`; Oscillator, Envelope,
PCM, Sequence and offline Beep rendering then have no OS backend dependency.

Do not expose vendor types through public function signatures. Public objects
contain opaque state pointers; callers must not inspect the pointed-to state.
