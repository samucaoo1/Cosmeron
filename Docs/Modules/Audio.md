# Audio

C11 header-only synthesis, beep music, WAV/MP3/FLAC decoding and desktop playback.
Include `Cosmeron/Modules/Audio/Audio.h`; no implementation macro is necessary.

## Contents

- [Build and platforms](#build-and-platforms)
- [Ownership and execution](#ownership-and-execution)
- [Oscillator](#oscillator)
- [PCM and envelope](#pcm-and-envelope)
- [Sequences and MML](#sequences-and-mml)
- [Beep music](#beep-music)
- [Codecs](#codecs)
- [Player and mixer](#player-and-mixer)
- [Device callbacks](#device-callbacks)
- [Validation and limits](#validation-and-limits)

## Build and platforms

```sh
cc -std=c11 Codespace/Examples/Audio/BeepMusic.c -o beep-music
./beep-music
./beep-music --play
```

The first invocation writes `melody.wav` without opening a device. The second
also plays it. No `-lm`, `-ldl`, `-pthread`, audio library, or framework flags
are supplied. This is **no additional link flags**, not an absence of runtime
system libraries.

| Profile | Contract |
|---|---|
| Linux desktop | glibc >= 2.34; normal dynamic executable; `libm.so.6` and an available system audio backend |
| Windows desktop | MinGW/MSVC; backend DLLs resolved at runtime |
| macOS desktop | Clang; system audio frameworks resolved at runtime |
| Portable synthesis | Define `AUDIO_NO_DESKTOP` before the first Audio include |

The portable profile includes PCM, Oscillator, Envelope, Sequence and offline
Beep rendering. It does not include the third-party desktop backend. The core
oscillator and cursor render into caller-provided buffers without allocating or
calling the OS. Parsing MML and creating a `TBuffer` allocate memory; embedded
applications can instead use constant event arrays and stack/static buffers.
No fixed-point profile is implemented yet.

Codecs/backends are pinned internally; see
[backend provenance](../../Codespace/Cosmeron/Modules/Audio/Internal/README.md).
Including desktop Audio increases compile time and unoptimized binary size.
An optimized build can discard unused static code.

## Ownership and execution

Zero initialize owning objects (`TBuffer`, `TSequence`, `TDecoder`, `TDevice`,
`TPlayer`). Do not copy or move an initialized owning object. Close/destroy it
before reuse. Oscillators, envelopes and sequence cursors are copyable states;
copying them also copies their current position. `TVoice` is a copyable token.

Pointers are borrowed unless stated otherwise. `PlayBuffer` **copies** PCM, so
the input buffer can be destroyed immediately after success. `OpenMemory`
borrows compressed bytes until `Close`. A cursor borrows its event array;
a wavetable oscillator borrows its table. Keep those inputs alive and unchanged.

Control a Player, Device or Decoder from one application thread at a time.
Internal playback/streaming workers are managed by the backend. Do not call
control functions, allocate memory, or do file I/O from a Device callback.
Closing a player releases all voices. Voice tokens from a closed/reopened player
are rejected; `Stop` and `Collect` invalidate the affected tokens.

All PCM is interleaved float. A **frame** contains one sample for each channel;
allocate `frames * channels` samples. Supported public channel counts are 1–8,
with rates from 1 to 384000 Hz. Oscillator frequencies must be finite,
nonnegative and strictly below half the sample rate. Amplitudes are 0–1.

Recoverable errors return `OPSTATUS`. Normal outputs are unchanged on failure.
Exceptions: `ReadFrames` and Player `Render` may expose partial PCM and its
count after a backend error. Decoder position may advance on such an error.
An optional MML error offset is written on syntax failure. WAV writing may
leave a partial file after an I/O error. `Close`/`Destroy` accept null and are
idempotent for properly initialized objects.

Names below use the macro form. With no custom namespace, the direct equivalent
is `Audio_<Package>_<Function>`. For example:

```c
AUDIO_OSCILLATOR_FUNC(SetFrequency)(&oscillator, 440.0);
Audio_Oscillator_SetFrequency(&oscillator, 440.0);
```

Defining `COSMERON_NAMESPACE Lab` makes the latter
`Lab_Audio_Oscillator_SetFrequency`. Constant prefixes can be selected separately
with `COSMERON_NAMESPACE_CONST`.

## Oscillator

Type: `AUDIO_OSCILLATOR_TYPE(TOscillator)`.

```c
AUDIO_OSCILLATOR_TYPE(TOscillator) oscillator;
float samples[512];
OPSTATUS status = AUDIO_OSCILLATOR_FUNC(Init)(
    &oscillator, 48000, AUDIO_OSCILLATOR_CONST(SINE), 440.0);
if (status == STATUS_CONST(SUCCESS))
  status = AUDIO_OSCILLATOR_FUNC(Render)(&oscillator, samples, 512);
```

| Function | Contract |
|---|---|
| `Init(state, rate, waveform, hz)` | Initializes phase zero, amplitude 0.25 and duty 0.5; use `SetWavetable` for a custom table |
| `SetFrequency(state, hz)` | Changes frequency without resetting phase |
| `SetAmplitude(state, gain)` | Sets 0–1 gain |
| `SetDutyCycle(state, duty)` | Pulse duty strictly between 0 and 1 |
| `SetWaveform(state, waveform)` | Selects SINE, SQUARE, TRIANGLE, SAW, NOISE or configured WAVETABLE |
| `SetWavetable(state, table, count)` | Validates and borrows at least two samples in [-1,1]; enables WAVETABLE |
| `SetQuality(state, quality)` | BASIC or POLYBLEP; correction applies to square/saw discontinuities |
| `ResetPhase(state, phase)` | Phase in [0,1) cycles |
| `Seed(state, seed)` | Deterministic noise; zero maps to a nonzero seed |
| `Render(state, output, frames)` | Writes mono PCM, preserving phase and noise state between calls |

Sine uses a folded polynomial, without libm. Wavetables use linear interpolation.
BASIC square/saw/triangle and arbitrary wavetables can alias; POLYBLEP reduces
square/saw aliasing but is not a general oversampling filter. Raw changes to
oscillator parameters are immediate; use an envelope for smooth transitions.

## PCM and envelope

| Function | Contract |
|---|---|
| `PCM Init(buffer, frames, channels, rate)` | Allocates zeroed float PCM with overflow checks |
| `PCM Destroy(buffer)` | Releases owned samples and clears the object |
| `PCM FromS16(input, samples, output)` | Signed 16-bit to float, using 32768 as the denominator |
| `PCM ToS16(input, samples, output)` | Rejects nonfinite input; saturates to signed 16-bit |
| `Envelope Init(state, attackFrames, decayFrames, sustain, releaseFrames)` | Configures a linear ADSR; durations are frames |
| `Envelope Trigger(state)` | Restarts attack from zero |
| `Envelope Release(state)` | Releases from the current gain |
| `Envelope Apply(state, samples, frames, channels)` | Multiplies interleaved PCM in place |

Conversion buffers must not overlap. Zero-duration envelope stages are supported.
Use `AUDIO_PCM_FUNC(...)` and `AUDIO_ENVELOPE_FUNC(...)`; their direct forms are
`Audio_PCM_...` and `Audio_Envelope_...`.

## Sequences and MML

`TEvent` has `frequency` (Hz; zero means rest), `ticks`, `bpm` and `volume`.
There are 960 ticks per quarter note. Each sequence is monophonic; simultaneous
voices or tracks can be mixed by Player. Events carry their own tempo and volume.

```c
const AUDIO_SEQUENCE_TYPE(TEvent) notes[] = {
  {440.0, 960, 120, 0.2f},
  {0.0,   480, 120, 0.0f},
  {523.2511306, 960, 120, 0.2f}
};
```

| Function | Contract |
|---|---|
| `NoteFrequency(midiNote, outHz)` | Equal temperament, MIDI 0–127, A4=440 Hz |
| `ParseMML(text, outSequence, outErrorOffset)` | Two-pass parser; owned events; optional byte offset on syntax error |
| `Destroy(sequence)` | Frees the parser-produced event array |
| `Init(cursor, events, count, rate, waveform)` | Validates all events and creates a borrowing render cursor |
| `Finished(cursor)` | True at end; a null cursor is treated as finished |
| `Render(cursor, output, capacity, outFrames)` | Writes mono PCM and zero-fills the unused tail; EOF is SUCCESS/0 |
| `RenderBuffer(events, count, rate, waveform, outBuffer)` | Renders the complete sequence into owned mono PCM |

Scheduling uses sample frames with fractional remainder carried between notes,
including tempo changes. Notes have short edge ramps, capped to half their
length. Rendering the same cursor in different block sizes produces the same
samples. Frame counts use integer durations after accumulated rounding.

Supported **MML subset**, case insensitive:

| Syntax | Meaning |
|---|---|
| `T120` | Tempo 1–1000 BPM; default 120 |
| `O4` | Octave 0–9; resulting MIDI pitch must remain 0–127 |
| `L8` | Default denominator 1,2,4,8,16,32,64,128; default 4 |
| `V32` | Volume 0–127; default 32 |
| `C D E F G A B` | Notes, with optional `#`, `+`, or `-` |
| `C8`, `C4.` | Explicit duration and up to three dots |
| `P` or `R` | Rest, with optional duration/dots |
| `>` / `<` | Raise/lower octave |

Whitespace is allowed between commands. Loops, ties, chords and comments are not
part of this initial text dialect. Very short dotted durations quantize to ticks.
Out-of-range pitches and unknown commands are errors, not silently ignored.

## Beep music

| Function | Contract |
|---|---|
| `RenderTone(hz, milliseconds, volume, rate, outBuffer)` | Owned mono square-wave PCM with short edge ramps; zero Hz is silence |
| `RenderMML(text, rate, outBuffer, outErrorOffset)` | Parses and renders a complete melody |
| `PlayTone(player, hz, milliseconds, volume, outVoice)` | Starts a finite tone asynchronously |
| `PlaySequence(player, events, count, loop, outVoice)` | Renders/copies a sequence, then starts playback |
| `PlayMML(player, text, loop, outVoice, outErrorOffset)` | Parses and plays MML asynchronously |
| `PlayBlocking(player, text, outErrorOffset)` | Waits for a nonlooping melody on a desktop player; rejects offline mode |

Stop a beep with `AUDIO_PLAYER_FUNC(Stop)(player, voice)`. Convenience playback
pre-renders the melody; use a cursor plus Device callback for bounded-memory
streaming or feed its PCM into a DAC/I2S driver. A hardware PWM/buzzer event
adapter is not included in this release. Beep music uses the normal audio output,
not the terminal bell or Windows `Beep` API.

## Codecs

Type: `AUDIO_CODEC_TYPE(TDecoder)`, initialized to `{0}`.

| Function | Contract |
|---|---|
| `OpenFile(decoder, path)` | Opens WAV, MP3 or FLAC, detected from content |
| `OpenMemory(decoder, bytes, size)` | Borrows encoded bytes until Close |
| `OpenReader(decoder, reader)` | Copies callback descriptors; borrows user data; read and seek callbacks required |
| `GetInfo(decoder, outInfo)` | Output sample rate/channels and optional known frame count |
| `ReadFrames(decoder, output, capacity, outFrames)` | Interleaved float in native rate/channels; EOF is SUCCESS/0 |
| `SeekFrame(decoder, frame)` | Seeks in decoded PCM frames; capability depends on codec/source |
| `Close(decoder)` | Releases decoding state; does not close a callback-owned reader |
| `WriteWAV(path, buffer)` | Writes little-endian 16-bit PCM WAV; saturates samples and checks RIFF size limits |

WAV reading includes PCM integers and IEEE float supported by the pinned decoder.
Writing is currently PCM16. A WAV extension does not guarantee a supported codec
inside it. MP3/FLAC encoding, Vorbis, Opus and AAC/M4A are not included.
Reader callbacks return SUCCESS with zero bytes at EOF; seeking uses the library's
SEEK_START, SEEK_CURRENT and SEEK_END constants. The reader must report actual
byte counts even when its I/O operation fails.

## Player and mixer

```c
AUDIO_PLAYER_TYPE(TPlayer) player = {0};
AUDIO_PLAYER_TYPE(TConfig) config = AUDIO_PLAYER_FUNC(Config)();
AUDIO_PLAYER_TYPE(TVoice) voice;
OPSTATUS status = AUDIO_PLAYER_FUNC(Open)(&player, &config);
if (status == STATUS_CONST(SUCCESS))
  status = AUDIO_PLAYER_FUNC(PlayFile)(&player, "music.mp3", false, &voice);
/* Keep player alive while playing; control from the application thread. */
AUDIO_PLAYER_FUNC(Close)(&player);
```

| Player function | Contract |
|---|---|
| `Config()` | Default stereo, 48000 Hz, physical device |
| `Open(player, config)` | Starts output, or prepares explicit offline mixing |
| `PlayFile(player, path, loop, outVoice)` | Desktop streaming; synchronous decoding for deterministic offline use |
| `PlayBuffer(player, buffer, loop, outVoice)` | Copies PCM; resamples and maps channels to the player format |
| `Pause` / `Resume(player, voice)` | Suspend/resume without destroying the voice |
| `SeekFrame(player, voice, frame)` | Source-frame seek; may be applied asynchronously by playback |
| `IsPlaying(player, voice)` | False for paused, ended or invalid tokens |
| `Stop(player, voice)` | Stops, releases and invalidates the voice |
| `Collect(player)` | Releases ended voices, returning a count; paused voices remain |
| `Render(player, output, frames, outFrames)` | Explicit offline mixing only; clamps final PCM to [-1,1] |
| `Close(player)` | Stops output and releases every remaining voice |

Use `config.offline = true` for manual rendering without hardware. The desktop
backend supplies worker threads for output and file streaming; applications do
not need to pump an Update function. Call `Collect` periodically to release
completed voices, or use Stop/Close explicitly. Control calls can allocate/block.

| Mixer function | Contract |
|---|---|
| `SetMasterVolume(player, volume)` | Master gain 0–1 |
| `SetVolume(player, voice, volume)` | Voice gain 0–1 |
| `SetPan(player, voice, pan)` | -1 left, 0 center, +1 right |

These use `AUDIO_MIXER_FUNC(...)` / `Audio_Mixer_...` and operate on Player voices.
There is no separate owning mixer object or user-configurable graph in this version.

## Device callbacks

`TConfig` selects rate, channels, a `TRenderCallback` and borrowed `userData`.
`Open(device, config)` creates a stopped output; `Start`, `Stop`, `IsStarted`
and `Close` manage it. The callback receives float PCM, frame count and channels.
The output is cleared before invocation; unfilled samples remain silent.
Nonfinite callback output becomes silence and final samples are clamped.

The callback and its data must outlive the device. Do not call Stop or Close
from inside the callback. Device and Player are alternative levels of access;
a Player already manages its own device. Device enumeration, explicit backend
selection, capture, underrun counters and device-change recovery are future work.

## Validation and limits

```sh
make -C Codespace/Tests/Audio run
make -C Codespace/Tests/Audio run BUILD=.build-sanitize \
  CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror -O1 -g -fsanitize=address,undefined'
```

Tests cover sample conversion, oscillator bounds/phase/block continuity, seeded
noise, wavetable interpolation, ADSR, tempo rounding, MML errors, real committed
WAV/MP3/FLAC fixtures, custom readers, memory decoding, WAV round-trip, offline
resampling/mixing, voice lifetime, namespace customization and multi-TU linkage.
The CI adds GCC, Clang, macOS, MinGW, MSVC, sanitizers and Linux 32-bit builds.
Hardware audibility/latency is a separate manual check using the examples; offline
tests cannot establish it. Static libc builds and arbitrary older Linux/BSD
systems are outside the initial no-extra-link-flags contract.
