# Native Audio Architecture

The Cosmeron Audio module is 100% native C11 code with zero external library dependencies.
All previously vendored third-party backends (including Miniaudio) have been completely removed.

## Architecture

- **Synthesis**: Offline PCM synthesis, oscillators, envelopes, sequences, and MML playback in `Audio/Offline.h`.
- **Codecs**: 100% native C11 streaming decoders for WAV, FLAC, and MPEG Layer III (MP3) in `Audio/Codec/`.
- **Playback & Devices**: Native multiplatform device drivers in `Audio/Device/` (ALSA on Linux, waveOut on Windows, AudioQueue on macOS, sndio on OpenBSD, OSS on FreeBSD/DragonFly, audioio on NetBSD) using zero-link runtime dynamic resolution.
- **Player & Mixer**: Native voice mixing, volume, pan, and playback management in `Audio/Player/` and `Audio/Mixer/`.

## Public Entrypoints

- `Audio/Offline.h`: Offline synthesis, PCM buffers, oscillators, envelopes, and MML render.
- `Audio/CodecOnly.h`: Complete WAV/FLAC/MP3 codec decoding and encoding API without device/playback.
- `Audio/Desktop.h`: Full audio suite including playback device, player, and mixer.
- `Audio/Audio.h`: Comprehensive audio umbrella header.
