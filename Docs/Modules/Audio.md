# Audio

The **Audio** module provides portable sound synthesis and optional desktop audio decoding, playback and mixing. This reference is derived from the **implementation and public headers** on branch `audio-module`, replacing the earlier document.

```c
#include "Cosmeron/Modules/Audio/Audio.h"
```

---

# Overview

| Package | Purpose | `AUDIO_NO_DESKTOP` |
| --- | --- | --- |
| [PCM](#pcm-package) | Owning float PCM buffers and signed-16-bit conversion. | Yes |
| [Oscillator](#oscillator-package) | Sine, square, triangle, saw, noise and wavetable synthesis. | Yes |
| [Envelope](#envelope-package) | ADSR-style gain shaping of audio samples. | Yes |
| [Sequence](#sequence-package) | Musical events, MML parsing and streaming or whole-buffer rendering. | Yes |
| [Beep](#beep-package) | Square-wave beep and MML synthesis, plus desktop playback helpers. | Yes (render only) |
| [Codec](#codec-package) | WAV, MP3 and FLAC decoding, custom readers and WAV export. | No |
| [Device](#device-package) | Callback-driven hardware audio output. | No |
| [Mixer](#mixer-package) | Master gain and per-voice gain/pan. | No |
| [Player](#player-package) | Voice playback, file and buffer sources, and offline mixing. | No |

### Macro form

```c
AUDIO_OSCILLATOR_FUNC(SetAmplitude)(&oscillator, 0.5f);
```

### Direct form

```c
Audio_Oscillator_SetAmplitude(&oscillator, 0.5f);
```

Both forms call the same function with the default namespace. `COSMERON_NAMESPACE` changes the direct name but not the macro form; `COSMERON_NAMESPACE_CONST` optionally changes constant prefixes.

### Portable build

```c
#define AUDIO_NO_DESKTOP
#include "Cosmeron/Modules/Audio/Audio.h"
```

Exposes PCM, Oscillator, Envelope, Sequence and Beep offline rendering. Without that define, the aggregate header also exposes Codec, Device, Mixer, Player and Beep playback.

---

# General contracts

- C11 single-header-style implementation: `.impl` files are included by headers; callers need no separate audio implementation translation unit.
- PCM uses interleaved `float` samples. One frame has one sample per channel (stereo: 512 frames = 1024 floats).
- Public format validator permits sample rates from 1 to 384000 Hz and channel counts from 1 to 8.
- Recoverable failures return `OPSTATUS`, compared to `STATUS_CONST(SUCCESS)`. Other functions return `void`, `bool`, `size_t`, or a configuration value.
- Zero-initialize owning PCM, parsed sequence, decoder, player and device objects. Destroy or Close their owned resources; do not independently destroy copied owning objects.
- Borrowed references include oscillator wavetables, sequence cursor events and codec memory/reader state. `Player_PlayBuffer` copies PCM samples.
- Audio callbacks should not perform blocking waits, file I/O or unbounded allocations.

## Shared constants

| Constant | Value |
| --- | ---: |
| `AUDIO_CONST(DEFAULT_SAMPLE_RATE)` | 48000 |
| `AUDIO_CONST(MAX_SAMPLE_RATE)` | 384000 |
| `AUDIO_CONST(MAX_CHANNELS)` | 8 |
| `AUDIO_CONST(MONO)` / `AUDIO_CONST(STEREO)` | 1 / 2 |
| `AUDIO_CONST(DEFAULT_BPM)` | 120 |
| `AUDIO_CONST(DEFAULT_OCTAVE)` | 4 |
| `AUDIO_CONST(TICKS_PER_QUARTER)` | 960 |
| `AUDIO_CONST(MIDI_NOTE_COUNT)` | 128 |
| `AUDIO_CONST(RENDER_BLOCK_FRAMES)` | 512 |

---

# API reference

## Function summary

| Package | Function | Purpose |
| --- | --- | --- |
| PCM | [`Init`](#pcm-init) | Allocate a zero-initialized interleaved float PCM buffer. |
| PCM | [`Destroy`](#pcm-destroy) | Free samples and clear an owning buffer. |
| PCM | [`FromS16`](#pcm-froms16) | Convert int16_t samples to floats. |
| PCM | [`ToS16`](#pcm-tos16) | Convert floats to signed 16-bit PCM. |
| Oscillator | [`Init`](#oscillator-init) | Initialize an oscillator. |
| Oscillator | [`SetFrequency`](#oscillator-setfrequency) | Set the oscillator frequency. |
| Oscillator | [`SetAmplitude`](#oscillator-setamplitude) | Set output amplitude. |
| Oscillator | [`SetDutyCycle`](#oscillator-setdutycycle) | Set the square-wave duty cycle. |
| Oscillator | [`SetWaveform`](#oscillator-setwaveform) | Switch waveform type. |
| Oscillator | [`SetWavetable`](#oscillator-setwavetable) | Set a borrowed wavetable. |
| Oscillator | [`ResetPhase`](#oscillator-resetphase) | Reset cycle phase. |
| Oscillator | [`Seed`](#oscillator-seed) | Seed the noise PRNG. |
| Oscillator | [`Render`](#oscillator-render) | Render a mono float block. |
| Oscillator | [`SetQuality`](#oscillator-setquality) | Set BASIC or POLYBLEP quality. |
| Envelope | [`Init`](#envelope-init) | Initialize envelope durations and sustain level. |
| Envelope | [`Trigger`](#envelope-trigger) | Start the ATTACK stage. |
| Envelope | [`Release`](#envelope-release) | Start RELEASE from the current level. |
| Envelope | [`Apply`](#envelope-apply) | Multiply interleaved samples by the envelope. |
| Sequence | [`NoteFrequency`](#sequence-notefrequency) | Convert MIDI pitch to hertz. |
| Sequence | [`ParseMML`](#sequence-parsemml) | Parse MML into owned event data. |
| Sequence | [`Destroy`](#sequence-destroy) | Free a parsed sequence. |
| Sequence | [`Init`](#sequence-init) | Initialize a cursor over borrowed events. |
| Sequence | [`Finished`](#sequence-finished) | Test whether playback has ended. |
| Sequence | [`Render`](#sequence-render) | Render an event stream to caller memory. |
| Sequence | [`RenderBuffer`](#sequence-renderbuffer) | Render all events to an owning mono PCM buffer. |
| Beep | [`RenderTone`](#beep-rendertone) | Render a square-wave tone to PCM. |
| Beep | [`RenderMML`](#beep-rendermml) | Render MML music to PCM. |
| Beep | [`PlayTone`](#beep-playtone) | Start a generated beep on a player. |
| Beep | [`PlaySequence`](#beep-playsequence) | Play a supplied musical-event array. |
| Beep | [`PlayMML`](#beep-playmml) | Parse and start an MML voice. |
| Beep | [`PlayBlocking`](#beep-playblocking) | Play MML and block until completion. |
| Codec | [`OpenFile`](#codec-openfile) | Open a decoder from a file. |
| Codec | [`OpenMemory`](#codec-openmemory) | Open a decoder borrowing memory. |
| Codec | [`OpenReader`](#codec-openreader) | Open a decoder with custom I/O callbacks. |
| Codec | [`GetInfo`](#codec-getinfo) | Read stream metadata. |
| Codec | [`ReadFrames`](#codec-readframes) | Decode interleaved float frames. |
| Codec | [`SeekFrame`](#codec-seekframe) | Seek to an absolute PCM frame. |
| Codec | [`Close`](#codec-close) | Close decoder and free resources. |
| Codec | [`WriteWAV`](#codec-writewav) | Write signed-16-bit PCM WAV. |
| Device | [`Open`](#device-open) | Open a hardware output device. |
| Device | [`Start`](#device-start) | Start the device callback. |
| Device | [`Stop`](#device-stop) | Stop the device. |
| Device | [`IsStarted`](#device-isstarted) | Check whether playback is started. |
| Device | [`Close`](#device-close) | Close the device. |
| Mixer | [`SetMasterVolume`](#mixer-setmastervolume) | Set the player's master volume. |
| Mixer | [`SetVolume`](#mixer-setvolume) | Set an individual voice's volume. |
| Mixer | [`SetPan`](#mixer-setpan) | Set voice panning. |
| Player | [`Config`](#player-config) | Return default player configuration. |
| Player | [`Open`](#player-open) | Open desktop or offline player. |
| Player | [`PlayFile`](#player-playfile) | Start playback from a file. |
| Player | [`PlayBuffer`](#player-playbuffer) | Start playback from copied PCM. |
| Player | [`Pause`](#player-pause) | Pause a voice. |
| Player | [`Resume`](#player-resume) | Resume a voice. |
| Player | [`SeekFrame`](#player-seekframe) | Seek a voice by PCM frame. |
| Player | [`IsPlaying`](#player-isplaying) | Test whether a voice is playing. |
| Player | [`Stop`](#player-stop) | Stop and destroy a voice. |
| Player | [`Collect`](#player-collect) | Collect completed voices. |
| Player | [`Render`](#player-render) | Render an offline mix. |
| Player | [`Close`](#player-close) | Close player and destroy all voices. |

---

# PCM package

Header: `Cosmeron/Modules/Audio/PCM/PCM.h`

Owning float PCM buffers and signed-16-bit conversion.





### Types and constants

`AUDIO_PCM_TYPE(TBuffer)` owns `samples` and holds `frameCount`, `channels`, and `sampleRate`.

Examples below are usage excerpts; when a variable is not declared locally, it represents the previously initialized object named in the parameters table.

---

# PCM Init

Allocate a zero-initialized interleaved float PCM buffer.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PCM_FUNC(Init)(AUDIO_PCM_TYPE(TBuffer) * buffer, size_t frameCount, uint32_t channels, uint32_t sampleRate);
```

#### Direct form

```c
OPSTATUS Audio_PCM_Init(AUDIO_PCM_TYPE(TBuffer) * buffer, size_t frameCount, uint32_t channels, uint32_t sampleRate);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `buffer` | `AUDIO_PCM_TYPE(TBuffer) * buffer` | PCM buffer. |
| `frameCount` | `size_t frameCount` | Number of audio frames. |
| `channels` | `uint32_t channels` | Audio channel count. |
| `sampleRate` | `uint32_t sampleRate` | Sampling rate in Hz. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Allocates via calloc. Channels must be 1–8; rate 1–384000 Hz; size overflow is checked. An already-owned buffer returns BUSY; zero frames are allowed.

---

### Example

```c
AUDIO_PCM_TYPE(TBuffer) b = {0};
OPSTATUS status = AUDIO_PCM_FUNC(Init)(&b, 512, 2, 48000);
if (status == STATUS_CONST(SUCCESS)) AUDIO_PCM_FUNC(Destroy)(&b);
```

---

# PCM Destroy

Free samples and clear an owning buffer.

### Syntax

#### Macro form

```c
void AUDIO_PCM_FUNC(Destroy)(AUDIO_PCM_TYPE(TBuffer) * buffer);
```

#### Direct form

```c
void Audio_PCM_Destroy(AUDIO_PCM_TYPE(TBuffer) * buffer);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `buffer` | `AUDIO_PCM_TYPE(TBuffer) * buffer` | PCM buffer. |

---

### Return value

None.

---

### Remarks

Safe for NULL and repeated use on a destroyed object. Clears all fields.

---

### Example

```c
AUDIO_PCM_FUNC(Destroy)(buffer);
```

---

# PCM FromS16

Convert int16_t samples to floats.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PCM_FUNC(FromS16)(const int16_t *samples, size_t sampleCount, float *outSamples);
```

#### Direct form

```c
OPSTATUS Audio_PCM_FromS16(const int16_t *samples, size_t sampleCount, float *outSamples);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `samples` | `const int16_t *samples` | Input/output sample buffer. |
| `sampleCount` | `size_t sampleCount` | Count of individual samples. |
| `outSamples` | `float *outSamples` | Writable output sample array. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Maps int16 to float with division by 32768. sampleCount counts individual samples, not frames.

---

### Example

```c
int16_t in[] = {-32768, 0, 32767}; float out[3];
OPSTATUS status = AUDIO_PCM_FUNC(FromS16)(in, 3, out);
```

---

# PCM ToS16

Convert floats to signed 16-bit PCM.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PCM_FUNC(ToS16)(const float *samples, size_t sampleCount, int16_t *outSamples);
```

#### Direct form

```c
OPSTATUS Audio_PCM_ToS16(const float *samples, size_t sampleCount, int16_t *outSamples);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `samples` | `const float *samples` | Input/output sample buffer. |
| `sampleCount` | `size_t sampleCount` | Count of individual samples. |
| `outSamples` | `int16_t *outSamples` | Writable output sample array. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Validates finiteness, clamps to [-1,1], and scales positives by 32767. -1 maps to INT16_MIN.

---

### Example

```c
float in[] = {-1.f, 0.f, 1.f}; int16_t out[3];
OPSTATUS status = AUDIO_PCM_FUNC(ToS16)(in, 3, out);
```

---

# Oscillator package

Header: `Cosmeron/Modules/Audio/Oscillator/Oscillator.h`

Sine, square, triangle, saw, noise and wavetable synthesis.





### Types and constants

`TOscillator` contains phase, frequency, amplitude, sampleRate, waveform, quality and borrowed table. Waveforms: `SINE`, `SQUARE`, `TRIANGLE`, `SAW`, `NOISE`, `WAVETABLE`; quality: `BASIC`, `POLYBLEP`. Use `AUDIO_OSCILLATOR_CONST(...)`.

Examples below are usage excerpts; when a variable is not declared locally, it represents the previously initialized object named in the parameters table.

---

# Oscillator Init

Initialize an oscillator.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_OSCILLATOR_FUNC(Init)(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, uint32_t sampleRate, AUDIO_OSCILLATOR_TYPE(TWaveform) waveform, double frequency);
```

#### Direct form

```c
OPSTATUS Audio_Oscillator_Init(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, uint32_t sampleRate, AUDIO_OSCILLATOR_TYPE(TWaveform) waveform, double frequency);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `oscillator` | `AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator` | Object/value required by this operation. |
| `sampleRate` | `uint32_t sampleRate` | Sampling rate in Hz. |
| `waveform` | `AUDIO_OSCILLATOR_TYPE(TWaveform) waveform` | Waveform enum. |
| `frequency` | `double frequency` | Frequency in Hz. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Initializes phase 0, amplitude 0.25, duty 0.5, BASIC quality. Frequency must be finite, nonnegative and below sampleRate/2. WAVETABLE must be configured later.

---

### Example

```c
AUDIO_OSCILLATOR_TYPE(TOscillator) osc;
OPSTATUS status = AUDIO_OSCILLATOR_FUNC(Init)(
    &osc, 48000, AUDIO_OSCILLATOR_CONST(SINE), 440.0);
```

---

# Oscillator SetFrequency

Set the oscillator frequency.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_OSCILLATOR_FUNC(SetFrequency)(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, double frequency);
```

#### Direct form

```c
OPSTATUS Audio_Oscillator_SetFrequency(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, double frequency);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `oscillator` | `AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator` | Object/value required by this operation. |
| `frequency` | `double frequency` | Frequency in Hz. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Requires finite Hz in [0, sampleRate/2); phase is preserved.

---

### Example

```c
OPSTATUS status = AUDIO_OSCILLATOR_FUNC(SetFrequency)(oscillator, frequency);
```

---

# Oscillator SetAmplitude

Set output amplitude.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_OSCILLATOR_FUNC(SetAmplitude)(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, float amplitude);
```

#### Direct form

```c
OPSTATUS Audio_Oscillator_SetAmplitude(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, float amplitude);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `oscillator` | `AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator` | Object/value required by this operation. |
| `amplitude` | `float amplitude` | Amplitude gain. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Allowed range [0,1].

---

### Example

```c
OPSTATUS status = AUDIO_OSCILLATOR_FUNC(SetAmplitude)(oscillator, amplitude);
```

---

# Oscillator SetDutyCycle

Set the square-wave duty cycle.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_OSCILLATOR_FUNC(SetDutyCycle)(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, float dutyCycle);
```

#### Direct form

```c
OPSTATUS Audio_Oscillator_SetDutyCycle(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, float dutyCycle);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `oscillator` | `AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator` | Object/value required by this operation. |
| `dutyCycle` | `float dutyCycle` | Square-wave duty ratio. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Allowed range strictly between 0 and 1.

---

### Example

```c
OPSTATUS status = AUDIO_OSCILLATOR_FUNC(SetDutyCycle)(oscillator, dutyCycle);
```

---

# Oscillator SetWaveform

Switch waveform type.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_OSCILLATOR_FUNC(SetWaveform)(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, AUDIO_OSCILLATOR_TYPE(TWaveform) waveform);
```

#### Direct form

```c
OPSTATUS Audio_Oscillator_SetWaveform(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, AUDIO_OSCILLATOR_TYPE(TWaveform) waveform);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `oscillator` | `AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator` | Object/value required by this operation. |
| `waveform` | `AUDIO_OSCILLATOR_TYPE(TWaveform) waveform` | Waveform enum. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

WAVETABLE requires at least two previously supplied table samples.

---

### Example

```c
OPSTATUS status = AUDIO_OSCILLATOR_FUNC(SetWaveform)(oscillator, waveform);
```

---

# Oscillator SetWavetable

Set a borrowed wavetable.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_OSCILLATOR_FUNC(SetWavetable)(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, const float *table, size_t tableSize);
```

#### Direct form

```c
OPSTATUS Audio_Oscillator_SetWavetable(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, const float *table, size_t tableSize);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `oscillator` | `AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator` | Object/value required by this operation. |
| `table` | `const float *table` | Borrowed waveform sample table. |
| `tableSize` | `size_t tableSize` | Number of wavetable samples. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Borrows the supplied table. Size 2–UINT32_MAX; every sample must be finite and in [-1,1]. Caller keeps it alive during rendering.

---

### Example

```c
OPSTATUS status = AUDIO_OSCILLATOR_FUNC(SetWavetable)(oscillator, table, tableSize);
```

---

# Oscillator ResetPhase

Reset cycle phase.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_OSCILLATOR_FUNC(ResetPhase)(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, double phase);
```

#### Direct form

```c
OPSTATUS Audio_Oscillator_ResetPhase(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, double phase);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `oscillator` | `AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator` | Object/value required by this operation. |
| `phase` | `double phase` | Normalized cycle phase. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Valid normalized phase is [0,1).

---

### Example

```c
OPSTATUS status = AUDIO_OSCILLATOR_FUNC(ResetPhase)(oscillator, phase);
```

---

# Oscillator Seed

Seed the noise PRNG.

### Syntax

#### Macro form

```c
void AUDIO_OSCILLATOR_FUNC(Seed)(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, uint32_t seed);
```

#### Direct form

```c
void Audio_Oscillator_Seed(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, uint32_t seed);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `oscillator` | `AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator` | Object/value required by this operation. |
| `seed` | `uint32_t seed` | Noise seed. |

---

### Return value

None.

---

### Remarks

Returns void. Seed 0 selects the internal fallback nonzero seed. NULL is ignored.

---

### Example

```c
AUDIO_OSCILLATOR_FUNC(Seed)(oscillator, seed);
```

---

# Oscillator Render

Render a mono float block.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_OSCILLATOR_FUNC(Render)(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, float *outSamples, size_t frameCount);
```

#### Direct form

```c
OPSTATUS Audio_Oscillator_Render(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, float *outSamples, size_t frameCount);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `oscillator` | `AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator` | Object/value required by this operation. |
| `outSamples` | `float *outSamples` | Writable output sample array. |
| `frameCount` | `size_t frameCount` | Number of audio frames. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Generates mono output and updates oscillator phase/noise state. Output pointer is required even for zero frames.

---

### Example

```c
float block[256];
OPSTATUS status = AUDIO_OSCILLATOR_FUNC(Render)(&osc, block, 256);
```

---

# Oscillator SetQuality

Set BASIC or POLYBLEP quality.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_OSCILLATOR_FUNC(SetQuality)(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, AUDIO_OSCILLATOR_TYPE(TQuality) quality);
```

#### Direct form

```c
OPSTATUS Audio_Oscillator_SetQuality(AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator, AUDIO_OSCILLATOR_TYPE(TQuality) quality);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `oscillator` | `AUDIO_OSCILLATOR_TYPE(TOscillator) * oscillator` | Object/value required by this operation. |
| `quality` | `AUDIO_OSCILLATOR_TYPE(TQuality) quality` | Quality enum. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

BASIC/POLYBLEP. PolyBLEP adjusts SAW and SQUARE discontinuities only.

---

### Example

```c
OPSTATUS status = AUDIO_OSCILLATOR_FUNC(SetQuality)(oscillator, quality);
```

---

# Envelope package

Header: `Cosmeron/Modules/Audio/Envelope/Envelope.h`

ADSR-style gain shaping of audio samples.





### Types and constants

`TEnvelope` holds time/level/stage state; stages are `IDLE`, `ATTACK`, `DECAY`, `SUSTAIN`, `RELEASE` via `AUDIO_ENVELOPE_CONST(...)`.

Examples below are usage excerpts; when a variable is not declared locally, it represents the previously initialized object named in the parameters table.

---

# Envelope Init

Initialize envelope durations and sustain level.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_ENVELOPE_FUNC(Init)(AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope, uint64_t attackFrames, uint64_t decayFrames, float sustain, uint64_t releaseFrames);
```

#### Direct form

```c
OPSTATUS Audio_Envelope_Init(AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope, uint64_t attackFrames, uint64_t decayFrames, float sustain, uint64_t releaseFrames);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `envelope` | `AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope` | Object/value required by this operation. |
| `attackFrames` | `uint64_t attackFrames` | Attack length. |
| `decayFrames` | `uint64_t decayFrames` | Decay length. |
| `sustain` | `float sustain` | Sustain amplitude. |
| `releaseFrames` | `uint64_t releaseFrames` | Release length. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Durations measured in frames; sustain gain in [0,1]. Zero-length stages are supported.

---

### Example

```c
AUDIO_ENVELOPE_TYPE(TEnvelope) env;
OPSTATUS status = AUDIO_ENVELOPE_FUNC(Init)(&env, 100, 200, 0.6f, 300);
```

---

# Envelope Trigger

Start the ATTACK stage.

### Syntax

#### Macro form

```c
void AUDIO_ENVELOPE_FUNC(Trigger)(AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope);
```

#### Direct form

```c
void Audio_Envelope_Trigger(AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `envelope` | `AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope` | Object/value required by this operation. |

---

### Return value

None.

---

### Remarks

Starts ATTACK at level 0. Ignores NULL.

---

### Example

```c
AUDIO_ENVELOPE_FUNC(Trigger)(envelope);
```

---

# Envelope Release

Start RELEASE from the current level.

### Syntax

#### Macro form

```c
void AUDIO_ENVELOPE_FUNC(Release)(AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope);
```

#### Direct form

```c
void Audio_Envelope_Release(AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `envelope` | `AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope` | Object/value required by this operation. |

---

### Return value

None.

---

### Remarks

Captures current level and begins RELEASE. Does nothing for IDLE or NULL.

---

### Example

```c
AUDIO_ENVELOPE_FUNC(Release)(envelope);
```

---

# Envelope Apply

Multiply interleaved samples by the envelope.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_ENVELOPE_FUNC(Apply)(AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope, float *samples, size_t frameCount, uint32_t channels);
```

#### Direct form

```c
OPSTATUS Audio_Envelope_Apply(AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope, float *samples, size_t frameCount, uint32_t channels);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `envelope` | `AUDIO_ENVELOPE_TYPE(TEnvelope) * envelope` | Object/value required by this operation. |
| `samples` | `float *samples` | Input/output sample buffer. |
| `frameCount` | `size_t frameCount` | Number of audio frames. |
| `channels` | `uint32_t channels` | Audio channel count. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Updates envelope stage while multiplying each channel in a frame by the same gain; channels 1–8.

---

### Example

```c
float mono[4] = {1.f, 1.f, 1.f, 1.f};
OPSTATUS status = AUDIO_ENVELOPE_FUNC(Apply)(&env, mono, 4, 1);
```

---

# Sequence package

Header: `Cosmeron/Modules/Audio/Sequence/Sequence.h`

Musical events, MML parsing and streaming or whole-buffer rendering.





### Types and constants

`TEvent` has `frequency`, `ticks`, `bpm`, `volume`; owning `TSequence` contains an events pointer and count; `TCursor` borrows events and advances playback state.

Examples below are usage excerpts; when a variable is not declared locally, it represents the previously initialized object named in the parameters table.

---

# Sequence NoteFrequency

Convert MIDI pitch to hertz.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_SEQUENCE_FUNC(NoteFrequency)(uint32_t midiNote, double *outFrequency);
```

#### Direct form

```c
OPSTATUS Audio_Sequence_NoteFrequency(uint32_t midiNote, double *outFrequency);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `midiNote` | `uint32_t midiNote` | MIDI pitch number. |
| `outFrequency` | `double *outFrequency` | Frequency result output. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Accepts MIDI note 0–127; note 69 is ~440 Hz.

---

### Example

```c
double hz = 0;
OPSTATUS status = AUDIO_SEQUENCE_FUNC(NoteFrequency)(69, &hz);
```

---

# Sequence ParseMML

Parse MML into owned event data.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_SEQUENCE_FUNC(ParseMML)(const char *text, AUDIO_SEQUENCE_TYPE(TSequence) * outSequence, size_t *outErrorOffset);
```

#### Direct form

```c
OPSTATUS Audio_Sequence_ParseMML(const char *text, AUDIO_SEQUENCE_TYPE(TSequence) * outSequence, size_t *outErrorOffset);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `text` | `const char *text` | MML input string. |
| `outSequence` | `AUDIO_SEQUENCE_TYPE(TSequence) * outSequence` | Owning sequence result. |
| `outErrorOffset` | `size_t *outErrorOffset` | Optional parser error byte offset. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Two-pass parser; allocates events. INVALID_SEQUENCE sets optional error byte offset. BUSY if sequence already owns events; Destroy after use.

---

### Example

```c
AUDIO_SEQUENCE_TYPE(TSequence) seq = {0}; size_t errorAt = 0;
OPSTATUS status = AUDIO_SEQUENCE_FUNC(ParseMML)(
    "T120 O4 L4 C D E", &seq, &errorAt);
if (status == STATUS_CONST(SUCCESS)) AUDIO_SEQUENCE_FUNC(Destroy)(&seq);
```

---

# Sequence Destroy

Free a parsed sequence.

### Syntax

#### Macro form

```c
void AUDIO_SEQUENCE_FUNC(Destroy)(AUDIO_SEQUENCE_TYPE(TSequence) * sequence);
```

#### Direct form

```c
void Audio_Sequence_Destroy(AUDIO_SEQUENCE_TYPE(TSequence) * sequence);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `sequence` | `AUDIO_SEQUENCE_TYPE(TSequence) * sequence` | Owning parsed sequence. |

---

### Return value

None.

---

### Remarks

Frees owning parsed events and zeroes the struct. Do not call on static/borrowed events masquerading as owned.

---

### Example

```c
AUDIO_SEQUENCE_FUNC(Destroy)(sequence);
```

---

# Sequence Init

Initialize a cursor over borrowed events.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_SEQUENCE_FUNC(Init)(AUDIO_SEQUENCE_TYPE(TCursor) * cursor, const AUDIO_SEQUENCE_TYPE(TEvent) * events, size_t eventCount, uint32_t sampleRate, AUDIO_OSCILLATOR_TYPE(TWaveform) waveform);
```

#### Direct form

```c
OPSTATUS Audio_Sequence_Init(AUDIO_SEQUENCE_TYPE(TCursor) * cursor, const AUDIO_SEQUENCE_TYPE(TEvent) * events, size_t eventCount, uint32_t sampleRate, AUDIO_OSCILLATOR_TYPE(TWaveform) waveform);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `cursor` | `AUDIO_SEQUENCE_TYPE(TCursor) * cursor` | Sequence playback cursor. |
| `events` | `const AUDIO_SEQUENCE_TYPE(TEvent) * events` | Borrowed musical event array. |
| `eventCount` | `size_t eventCount` | Number of events. |
| `sampleRate` | `uint32_t sampleRate` | Sampling rate in Hz. |
| `waveform` | `AUDIO_OSCILLATOR_TYPE(TWaveform) waveform` | Waveform enum. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Borrows event array; events must stay alive and unchanged. Validates BPM 1–1000, nonzero ticks, finite frequency, gain [0,1].

---

### Example

```c
OPSTATUS status = AUDIO_SEQUENCE_FUNC(Init)(cursor, events, eventCount, sampleRate, waveform);
```

---

# Sequence Finished

Test whether playback has ended.

### Syntax

#### Macro form

```c
bool AUDIO_SEQUENCE_FUNC(Finished)(const AUDIO_SEQUENCE_TYPE(TCursor) * cursor);
```

#### Direct form

```c
bool Audio_Sequence_Finished(const AUDIO_SEQUENCE_TYPE(TCursor) * cursor);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `cursor` | `const AUDIO_SEQUENCE_TYPE(TCursor) * cursor` | Sequence playback cursor. |

---

### Return value

Boolean predicate result.

---

### Remarks

Returns true for NULL or completed cursor.

---

### Example

```c
bool value = AUDIO_SEQUENCE_FUNC(Finished)(cursor);
```

---

# Sequence Render

Render an event stream to caller memory.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_SEQUENCE_FUNC(Render)(AUDIO_SEQUENCE_TYPE(TCursor) * cursor, float *outSamples, size_t frameCount, size_t *outFrames);
```

#### Direct form

```c
OPSTATUS Audio_Sequence_Render(AUDIO_SEQUENCE_TYPE(TCursor) * cursor, float *outSamples, size_t frameCount, size_t *outFrames);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `cursor` | `AUDIO_SEQUENCE_TYPE(TCursor) * cursor` | Sequence playback cursor. |
| `outSamples` | `float *outSamples` | Writable output sample array. |
| `frameCount` | `size_t frameCount` | Number of audio frames. |
| `outFrames` | `size_t *outFrames` | Output number of generated frames. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Mono PCM. Fills unused requested frames with zero; outFrames counts real event frames. Applies short edge ramps and fractional-frame timing.

---

### Example

```c
float block[256]; size_t outFrames = 0;
OPSTATUS status = AUDIO_SEQUENCE_FUNC(Render)(&cursor, block, 256, &outFrames);
```

---

# Sequence RenderBuffer

Render all events to an owning mono PCM buffer.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_SEQUENCE_FUNC(RenderBuffer)(const AUDIO_SEQUENCE_TYPE(TEvent) * events, size_t eventCount, uint32_t sampleRate, AUDIO_OSCILLATOR_TYPE(TWaveform) waveform, AUDIO_PCM_TYPE(TBuffer) * outBuffer);
```

#### Direct form

```c
OPSTATUS Audio_Sequence_RenderBuffer(const AUDIO_SEQUENCE_TYPE(TEvent) * events, size_t eventCount, uint32_t sampleRate, AUDIO_OSCILLATOR_TYPE(TWaveform) waveform, AUDIO_PCM_TYPE(TBuffer) * outBuffer);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `events` | `const AUDIO_SEQUENCE_TYPE(TEvent) * events` | Borrowed musical event array. |
| `eventCount` | `size_t eventCount` | Number of events. |
| `sampleRate` | `uint32_t sampleRate` | Sampling rate in Hz. |
| `waveform` | `AUDIO_OSCILLATOR_TYPE(TWaveform) waveform` | Waveform enum. |
| `outBuffer` | `AUDIO_PCM_TYPE(TBuffer) * outBuffer` | Owning PCM buffer output. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Allocates an owning mono PCM TBuffer; output must be empty. Checks total-frame overflow.

---

### Example

```c
OPSTATUS status = AUDIO_SEQUENCE_FUNC(RenderBuffer)(events, eventCount, sampleRate, waveform, outBuffer);
```

---

# Beep package

Header: `Cosmeron/Modules/Audio/Beep/Beep.h`

Square-wave beep and MML synthesis, plus desktop playback helpers.



The four `Play*` operations are desktop-only, whereas `RenderTone` and `RenderMML` are portable.

### Types and constants

Beep rendering returns owning PCM buffers; playback methods return voice tokens.

Examples below are usage excerpts; when a variable is not declared locally, it represents the previously initialized object named in the parameters table.

---

# Beep RenderTone

Render a square-wave tone to PCM.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_BEEP_FUNC(RenderTone)(double frequency, uint32_t durationMs, float volume, uint32_t sampleRate, AUDIO_PCM_TYPE(TBuffer) * outBuffer);
```

#### Direct form

```c
OPSTATUS Audio_Beep_RenderTone(double frequency, uint32_t durationMs, float volume, uint32_t sampleRate, AUDIO_PCM_TYPE(TBuffer) * outBuffer);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `frequency` | `double frequency` | Frequency in Hz. |
| `durationMs` | `uint32_t durationMs` | Duration in milliseconds. |
| `volume` | `float volume` | Gain between 0 and 1. |
| `sampleRate` | `uint32_t sampleRate` | Sampling rate in Hz. |
| `outBuffer` | `AUDIO_PCM_TYPE(TBuffer) * outBuffer` | Owning PCM buffer output. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Portable. Duration >0 ms, gain [0,1]. Generates square tone with short fades; frequency 0 yields silence.

---

### Example

```c
AUDIO_PCM_TYPE(TBuffer) tone = {0};
OPSTATUS status = AUDIO_BEEP_FUNC(RenderTone)(440, 100, 0.2f, 48000, &tone);
if (status == STATUS_CONST(SUCCESS)) AUDIO_PCM_FUNC(Destroy)(&tone);
```

---

# Beep RenderMML

Render MML music to PCM.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_BEEP_FUNC(RenderMML)(const char *text, uint32_t sampleRate, AUDIO_PCM_TYPE(TBuffer) * outBuffer, size_t *outErrorOffset);
```

#### Direct form

```c
OPSTATUS Audio_Beep_RenderMML(const char *text, uint32_t sampleRate, AUDIO_PCM_TYPE(TBuffer) * outBuffer, size_t *outErrorOffset);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `text` | `const char *text` | MML input string. |
| `sampleRate` | `uint32_t sampleRate` | Sampling rate in Hz. |
| `outBuffer` | `AUDIO_PCM_TYPE(TBuffer) * outBuffer` | Owning PCM buffer output. |
| `outErrorOffset` | `size_t *outErrorOffset` | Optional parser error byte offset. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Portable. Parses events, renders with square wave, releases temporary events.

---

### Example

```c
AUDIO_PCM_TYPE(TBuffer) song = {0};
OPSTATUS status = AUDIO_BEEP_FUNC(RenderMML)(
    "T140 O5 L8 E D C", 48000, &song, NULL);
if (status == STATUS_CONST(SUCCESS)) AUDIO_PCM_FUNC(Destroy)(&song);
```

---

# Beep PlayTone

Start a generated beep on a player.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_BEEP_FUNC(PlayTone)(AUDIO_PLAYER_TYPE(TPlayer) * player, double frequency, uint32_t durationMs, float volume, AUDIO_PLAYER_TYPE(TVoice) * outVoice);
```

#### Direct form

```c
OPSTATUS Audio_Beep_PlayTone(AUDIO_PLAYER_TYPE(TPlayer) * player, double frequency, uint32_t durationMs, float volume, AUDIO_PLAYER_TYPE(TVoice) * outVoice);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `frequency` | `double frequency` | Frequency in Hz. |
| `durationMs` | `uint32_t durationMs` | Duration in milliseconds. |
| `volume` | `float volume` | Gain between 0 and 1. |
| `outVoice` | `AUDIO_PLAYER_TYPE(TVoice) * outVoice` | Voice token output. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Desktop-only. Renders temporary PCM then copies it into the player.

---

### Example

```c
OPSTATUS status = AUDIO_BEEP_FUNC(PlayTone)(player, frequency, durationMs, volume, outVoice);
```

---

# Beep PlaySequence

Play a supplied musical-event array.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_BEEP_FUNC(PlaySequence)(AUDIO_PLAYER_TYPE(TPlayer) * player, const AUDIO_SEQUENCE_TYPE(TEvent) * events, size_t eventCount, bool loop, AUDIO_PLAYER_TYPE(TVoice) * outVoice);
```

#### Direct form

```c
OPSTATUS Audio_Beep_PlaySequence(AUDIO_PLAYER_TYPE(TPlayer) * player, const AUDIO_SEQUENCE_TYPE(TEvent) * events, size_t eventCount, bool loop, AUDIO_PLAYER_TYPE(TVoice) * outVoice);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `events` | `const AUDIO_SEQUENCE_TYPE(TEvent) * events` | Borrowed musical event array. |
| `eventCount` | `size_t eventCount` | Number of events. |
| `loop` | `bool loop` | Whether to repeat playback. |
| `outVoice` | `AUDIO_PLAYER_TYPE(TVoice) * outVoice` | Voice token output. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Desktop-only. Renders sequence to temporary PCM, copies to player; loop optional.

---

### Example

```c
OPSTATUS status = AUDIO_BEEP_FUNC(PlaySequence)(player, events, eventCount, loop, outVoice);
```

---

# Beep PlayMML

Parse and start an MML voice.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_BEEP_FUNC(PlayMML)(AUDIO_PLAYER_TYPE(TPlayer) * player, const char *text, bool loop, AUDIO_PLAYER_TYPE(TVoice) * outVoice, size_t *outErrorOffset);
```

#### Direct form

```c
OPSTATUS Audio_Beep_PlayMML(AUDIO_PLAYER_TYPE(TPlayer) * player, const char *text, bool loop, AUDIO_PLAYER_TYPE(TVoice) * outVoice, size_t *outErrorOffset);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `text` | `const char *text` | MML input string. |
| `loop` | `bool loop` | Whether to repeat playback. |
| `outVoice` | `AUDIO_PLAYER_TYPE(TVoice) * outVoice` | Voice token output. |
| `outErrorOffset` | `size_t *outErrorOffset` | Optional parser error byte offset. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Desktop-only. Parses MML and starts a voice; destroys temporary parsed events.

---

### Example

```c
OPSTATUS status = AUDIO_BEEP_FUNC(PlayMML)(player, text, loop, outVoice, outErrorOffset);
```

---

# Beep PlayBlocking

Play MML and block until completion.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_BEEP_FUNC(PlayBlocking)(AUDIO_PLAYER_TYPE(TPlayer) * player, const char *text, size_t *outErrorOffset);
```

#### Direct form

```c
OPSTATUS Audio_Beep_PlayBlocking(AUDIO_PLAYER_TYPE(TPlayer) * player, const char *text, size_t *outErrorOffset);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `text` | `const char *text` | MML input string. |
| `outErrorOffset` | `size_t *outErrorOffset` | Optional parser error byte offset. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Desktop-only. NOT_SUPPORTED for offline player. Blocks caller, polls device in ~10ms intervals; avoid from audio callback.

---

### Example

```c
OPSTATUS status = AUDIO_BEEP_FUNC(PlayBlocking)(player, text, outErrorOffset);
```

---

# Codec package

Header: `Cosmeron/Modules/Audio/Codec/Codec.h`

WAV, MP3 and FLAC decoding, custom readers and WAV export.

**Desktop-only**: not included when `AUDIO_NO_DESKTOP` is enabled.



### Types and constants

`TDecoder` owns opaque state; `TInfo` has rate/channels/frameCount/lengthKnown. `TReader` has userData and read/seek callbacks. Seek origin constants: `SEEK_START`, `SEEK_CURRENT`, `SEEK_END`.

Examples below are usage excerpts; when a variable is not declared locally, it represents the previously initialized object named in the parameters table.

---

# Codec OpenFile

Open a decoder from a file.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_CODEC_FUNC(OpenFile)(AUDIO_CODEC_TYPE(TDecoder) * decoder, const char *path);
```

#### Direct form

```c
OPSTATUS Audio_Codec_OpenFile(AUDIO_CODEC_TYPE(TDecoder) * decoder, const char *path);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `decoder` | `AUDIO_CODEC_TYPE(TDecoder) * decoder` | Decoder handle. |
| `path` | `const char *path` | File path. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Decoder must be zero-initialized. BUSY if already open. Supports backend-recognized WAV/MP3/FLAC.

---

### Example

```c
OPSTATUS status = AUDIO_CODEC_FUNC(OpenFile)(decoder, path);
```

---

# Codec OpenMemory

Open a decoder borrowing memory.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_CODEC_FUNC(OpenMemory)(AUDIO_CODEC_TYPE(TDecoder) * decoder, const void *data, size_t size);
```

#### Direct form

```c
OPSTATUS Audio_Codec_OpenMemory(AUDIO_CODEC_TYPE(TDecoder) * decoder, const void *data, size_t size);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `decoder` | `AUDIO_CODEC_TYPE(TDecoder) * decoder` | Decoder handle. |
| `data` | `const void *data` | Encoded source memory. |
| `size` | `size_t size` | Source memory size. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Borrows compressed byte buffer until Close. Data must be non-null and size > 0.

---

### Example

```c
OPSTATUS status = AUDIO_CODEC_FUNC(OpenMemory)(decoder, data, size);
```

---

# Codec OpenReader

Open a decoder with custom I/O callbacks.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_CODEC_FUNC(OpenReader)(AUDIO_CODEC_TYPE(TDecoder) * decoder, const AUDIO_CODEC_TYPE(TReader) * reader);
```

#### Direct form

```c
OPSTATUS Audio_Codec_OpenReader(AUDIO_CODEC_TYPE(TDecoder) * decoder, const AUDIO_CODEC_TYPE(TReader) * reader);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `decoder` | `AUDIO_CODEC_TYPE(TDecoder) * decoder` | Decoder handle. |
| `reader` | `const AUDIO_CODEC_TYPE(TReader) * reader` | Custom reader configuration. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

TReader has userData, read and seek callbacks; both required. Reader struct copied but userData remains borrowed.

---

### Example

```c
OPSTATUS status = AUDIO_CODEC_FUNC(OpenReader)(decoder, reader);
```

---

# Codec GetInfo

Read stream metadata.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_CODEC_FUNC(GetInfo)(const AUDIO_CODEC_TYPE(TDecoder) * decoder, AUDIO_CODEC_TYPE(TInfo) * outInfo);
```

#### Direct form

```c
OPSTATUS Audio_Codec_GetInfo(const AUDIO_CODEC_TYPE(TDecoder) * decoder, AUDIO_CODEC_TYPE(TInfo) * outInfo);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `decoder` | `const AUDIO_CODEC_TYPE(TDecoder) * decoder` | Decoder handle. |
| `outInfo` | `AUDIO_CODEC_TYPE(TInfo) * outInfo` | Decoder information output. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

TInfo has sampleRate, channels, frameCount and lengthKnown. Check lengthKnown before trusting length.

---

### Example

```c
AUDIO_CODEC_TYPE(TInfo) info;
OPSTATUS status = AUDIO_CODEC_FUNC(GetInfo)(&decoder, &info);
```

---

# Codec ReadFrames

Decode interleaved float frames.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_CODEC_FUNC(ReadFrames)(AUDIO_CODEC_TYPE(TDecoder) * decoder, float *outSamples, size_t frameCapacity, size_t *outFrames);
```

#### Direct form

```c
OPSTATUS Audio_Codec_ReadFrames(AUDIO_CODEC_TYPE(TDecoder) * decoder, float *outSamples, size_t frameCapacity, size_t *outFrames);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `decoder` | `AUDIO_CODEC_TYPE(TDecoder) * decoder` | Decoder handle. |
| `outSamples` | `float *outSamples` | Writable output sample array. |
| `frameCapacity` | `size_t frameCapacity` | Maximum number of output frames. |
| `outFrames` | `size_t *outFrames` | Output number of generated frames. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Capacity counts frames; samples capacity = frames * stream channels. SUCCESS with zero frames at end. On error, partial PCM and outFrames may be written and decoder position may advance.

---

### Example

```c
float stereo[512 * 2]; size_t got = 0;
OPSTATUS status = AUDIO_CODEC_FUNC(ReadFrames)(&decoder, stereo, 512, &got);
```

---

# Codec SeekFrame

Seek to an absolute PCM frame.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_CODEC_FUNC(SeekFrame)(AUDIO_CODEC_TYPE(TDecoder) * decoder, uint64_t frameIndex);
```

#### Direct form

```c
OPSTATUS Audio_Codec_SeekFrame(AUDIO_CODEC_TYPE(TDecoder) * decoder, uint64_t frameIndex);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `decoder` | `AUDIO_CODEC_TYPE(TDecoder) * decoder` | Decoder handle. |
| `frameIndex` | `uint64_t frameIndex` | Absolute PCM frame position. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Absolute frame index; backend may reject unsupported seeking.

---

### Example

```c
OPSTATUS status = AUDIO_CODEC_FUNC(SeekFrame)(decoder, frameIndex);
```

---

# Codec Close

Close decoder and free resources.

### Syntax

#### Macro form

```c
void AUDIO_CODEC_FUNC(Close)(AUDIO_CODEC_TYPE(TDecoder) * decoder);
```

#### Direct form

```c
void Audio_Codec_Close(AUDIO_CODEC_TYPE(TDecoder) * decoder);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `decoder` | `AUDIO_CODEC_TYPE(TDecoder) * decoder` | Decoder handle. |

---

### Return value

None.

---

### Remarks

Accepts NULL and repeated close on an already-closed decoder.

---

### Example

```c
AUDIO_CODEC_FUNC(Close)(decoder);
```

---

# Codec WriteWAV

Write signed-16-bit PCM WAV.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_CODEC_FUNC(WriteWAV)(const char *path, const AUDIO_PCM_TYPE(TBuffer) * buffer);
```

#### Direct form

```c
OPSTATUS Audio_Codec_WriteWAV(const char *path, const AUDIO_PCM_TYPE(TBuffer) * buffer);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `path` | `const char *path` | File path. |
| `buffer` | `const AUDIO_PCM_TYPE(TBuffer) * buffer` | PCM buffer. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Writes 16-bit little-endian PCM WAV, not MP3/FLAC. Clamps float input, validates finite samples, enforces RIFF size limits. I/O errors may leave partial files.

---

### Example

```c
OPSTATUS status = AUDIO_CODEC_FUNC(WriteWAV)(path, buffer);
```

---

# Device package

Header: `Cosmeron/Modules/Audio/Device/Device.h`

Callback-driven hardware audio output.

**Desktop-only**: not included when `AUDIO_NO_DESKTOP` is enabled.



### Types and constants

`TDevice` owns an opaque backend; `TConfig` contains rate/channels/render callback/userData. Callback: `void (*)(void *userData, float *outSamples, size_t frameCount, uint32_t channels)`.

Examples below are usage excerpts; when a variable is not declared locally, it represents the previously initialized object named in the parameters table.

---

# Device Open

Open a hardware output device.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_DEVICE_FUNC(Open)(AUDIO_DEVICE_TYPE(TDevice) * device, const AUDIO_DEVICE_TYPE(TConfig) * config);
```

#### Direct form

```c
OPSTATUS Audio_Device_Open(AUDIO_DEVICE_TYPE(TDevice) * device, const AUDIO_DEVICE_TYPE(TConfig) * config);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `device` | `AUDIO_DEVICE_TYPE(TDevice) * device` | Audio device handle. |
| `config` | `const AUDIO_DEVICE_TYPE(TConfig) * config` | Configuration object. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

TConfig needs sampleRate, channels, callback and userData. Callback required. No fake device fallback.

---

### Example

```c
OPSTATUS status = AUDIO_DEVICE_FUNC(Open)(device, config);
```

---

# Device Start

Start the device callback.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_DEVICE_FUNC(Start)(AUDIO_DEVICE_TYPE(TDevice) * device);
```

#### Direct form

```c
OPSTATUS Audio_Device_Start(AUDIO_DEVICE_TYPE(TDevice) * device);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `device` | `AUDIO_DEVICE_TYPE(TDevice) * device` | Audio device handle. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Starts callback-driven playback. Avoid blocking, allocation and file I/O inside callback.

---

### Example

```c
OPSTATUS status = AUDIO_DEVICE_FUNC(Start)(device);
```

---

# Device Stop

Stop the device.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_DEVICE_FUNC(Stop)(AUDIO_DEVICE_TYPE(TDevice) * device);
```

#### Direct form

```c
OPSTATUS Audio_Device_Stop(AUDIO_DEVICE_TYPE(TDevice) * device);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `device` | `AUDIO_DEVICE_TYPE(TDevice) * device` | Audio device handle. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Stops playback without closing backend resources.

---

### Example

```c
OPSTATUS status = AUDIO_DEVICE_FUNC(Stop)(device);
```

---

# Device IsStarted

Check whether playback is started.

### Syntax

#### Macro form

```c
bool AUDIO_DEVICE_FUNC(IsStarted)(const AUDIO_DEVICE_TYPE(TDevice) * device);
```

#### Direct form

```c
bool Audio_Device_IsStarted(const AUDIO_DEVICE_TYPE(TDevice) * device);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `device` | `const AUDIO_DEVICE_TYPE(TDevice) * device` | Audio device handle. |

---

### Return value

Boolean predicate result.

---

### Remarks

False for NULL/closed device.

---

### Example

```c
bool value = AUDIO_DEVICE_FUNC(IsStarted)(device);
```

---

# Device Close

Close the device.

### Syntax

#### Macro form

```c
void AUDIO_DEVICE_FUNC(Close)(AUDIO_DEVICE_TYPE(TDevice) * device);
```

#### Direct form

```c
void Audio_Device_Close(AUDIO_DEVICE_TYPE(TDevice) * device);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `device` | `AUDIO_DEVICE_TYPE(TDevice) * device` | Audio device handle. |

---

### Return value

None.

---

### Remarks

Idempotent for initialized/cleared object; sets internal pointer NULL.

---

### Example

```c
AUDIO_DEVICE_FUNC(Close)(device);
```

---

# Mixer package

Header: `Cosmeron/Modules/Audio/Mixer/Mixer.h`

Master gain and per-voice gain/pan.

**Desktop-only**: not included when `AUDIO_NO_DESKTOP` is enabled.



### Types and constants

Mixer functions act on a Player and Voice; no separate owning mixer type.

Examples below are usage excerpts; when a variable is not declared locally, it represents the previously initialized object named in the parameters table.

---

# Mixer SetMasterVolume

Set the player's master volume.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_MIXER_FUNC(SetMasterVolume)(AUDIO_PLAYER_TYPE(TPlayer) * player, float volume);
```

#### Direct form

```c
OPSTATUS Audio_Mixer_SetMasterVolume(AUDIO_PLAYER_TYPE(TPlayer) * player, float volume);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `volume` | `float volume` | Gain between 0 and 1. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Volume must be within [0,1].

---

### Example

```c
OPSTATUS status = AUDIO_MIXER_FUNC(SetMasterVolume)(player, volume);
```

---

# Mixer SetVolume

Set an individual voice's volume.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_MIXER_FUNC(SetVolume)(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice, float volume);
```

#### Direct form

```c
OPSTATUS Audio_Mixer_SetVolume(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice, float volume);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `voice` | `AUDIO_PLAYER_TYPE(TVoice) voice` | Voice token. |
| `volume` | `float volume` | Gain between 0 and 1. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Voice gain [0,1]; invalid/stale voice returns NOT_FOUND.

---

### Example

```c
OPSTATUS status = AUDIO_MIXER_FUNC(SetVolume)(player, voice, volume);
```

---

# Mixer SetPan

Set voice panning.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_MIXER_FUNC(SetPan)(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice, float pan);
```

#### Direct form

```c
OPSTATUS Audio_Mixer_SetPan(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice, float pan);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `voice` | `AUDIO_PLAYER_TYPE(TVoice) voice` | Voice token. |
| `pan` | `float pan` | Stereo panning. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Pan [-1,1]; negative left, positive right; invalid/stale voice returns NOT_FOUND.

---

### Example

```c
OPSTATUS status = AUDIO_MIXER_FUNC(SetPan)(player, voice, pan);
```

---

# Player package

Header: `Cosmeron/Modules/Audio/Player/Player.h`

Voice playback, file and buffer sources, and offline mixing.

**Desktop-only**: not included when `AUDIO_NO_DESKTOP` is enabled.



### Types and constants

`TPlayer` owns opaque engine state and generation, `TConfig` holds rate/channels/offline; `TVoice` is a copyable token containing player identity, generation and voice ID.

Examples below are usage excerpts; when a variable is not declared locally, it represents the previously initialized object named in the parameters table.

---

# Player Config

Return default player configuration.

### Syntax

#### Macro form

```c
AUDIO_PLAYER_TYPE(TConfig) AUDIO_PLAYER_FUNC(Config)(void);
```

#### Direct form

```c
AUDIO_PLAYER_TYPE(TConfig) Audio_Player_Config(void);
```

---

### Parameters

None.

---

### Return value

Returns the default player configuration by value.

---

### Remarks

Returns sample rate 48000, stereo 2 channels and offline=false.

---

### Example

```c
AUDIO_PLAYER_TYPE(TConfig) config = AUDIO_PLAYER_FUNC(Config)();
config.offline = true;
```

---

# Player Open

Open desktop or offline player.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PLAYER_FUNC(Open)(AUDIO_PLAYER_TYPE(TPlayer) * player, const AUDIO_PLAYER_TYPE(TConfig) * config);
```

#### Direct form

```c
OPSTATUS Audio_Player_Open(AUDIO_PLAYER_TYPE(TPlayer) * player, const AUDIO_PLAYER_TYPE(TConfig) * config);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `config` | `const AUDIO_PLAYER_TYPE(TConfig) * config` | Configuration object. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Rate 1–384000, channels 1–8. Returns BUSY if already open. Reopening increments generation, invalidating old voice tokens.

---

### Example

```c
AUDIO_PLAYER_TYPE(TPlayer) player = {0};
AUDIO_PLAYER_TYPE(TConfig) config = AUDIO_PLAYER_FUNC(Config)();
OPSTATUS status = AUDIO_PLAYER_FUNC(Open)(&player, &config);
```

---

# Player PlayFile

Start playback from a file.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PLAYER_FUNC(PlayFile)(AUDIO_PLAYER_TYPE(TPlayer) * player, const char *path, bool loop, AUDIO_PLAYER_TYPE(TVoice) * outVoice);
```

#### Direct form

```c
OPSTATUS Audio_Player_PlayFile(AUDIO_PLAYER_TYPE(TPlayer) * player, const char *path, bool loop, AUDIO_PLAYER_TYPE(TVoice) * outVoice);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `path` | `const char *path` | File path. |
| `loop` | `bool loop` | Whether to repeat playback. |
| `outVoice` | `AUDIO_PLAYER_TYPE(TVoice) * outVoice` | Voice token output. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Streams on desktop; eagerly decodes in offline mode. Loop optional.

---

### Example

```c
OPSTATUS status = AUDIO_PLAYER_FUNC(PlayFile)(player, path, loop, outVoice);
```

---

# Player PlayBuffer

Start playback from copied PCM.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PLAYER_FUNC(PlayBuffer)(AUDIO_PLAYER_TYPE(TPlayer) * player, const AUDIO_PCM_TYPE(TBuffer) * buffer, bool loop, AUDIO_PLAYER_TYPE(TVoice) * outVoice);
```

#### Direct form

```c
OPSTATUS Audio_Player_PlayBuffer(AUDIO_PLAYER_TYPE(TPlayer) * player, const AUDIO_PCM_TYPE(TBuffer) * buffer, bool loop, AUDIO_PLAYER_TYPE(TVoice) * outVoice);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `buffer` | `const AUDIO_PCM_TYPE(TBuffer) * buffer` | PCM buffer. |
| `loop` | `bool loop` | Whether to repeat playback. |
| `outVoice` | `AUDIO_PLAYER_TYPE(TVoice) * outVoice` | Voice token output. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Copies validated nonempty finite interleaved PCM into player-owned storage; caller may destroy source after success.

---

### Example

```c
OPSTATUS status = AUDIO_PLAYER_FUNC(PlayBuffer)(player, buffer, loop, outVoice);
```

---

# Player Pause

Pause a voice.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PLAYER_FUNC(Pause)(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice);
```

#### Direct form

```c
OPSTATUS Audio_Player_Pause(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `voice` | `AUDIO_PLAYER_TYPE(TVoice) voice` | Voice token. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Preserves the voice for Resume; stale tokens yield NOT_FOUND.

---

### Example

```c
OPSTATUS status = AUDIO_PLAYER_FUNC(Pause)(player, voice);
```

---

# Player Resume

Resume a voice.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PLAYER_FUNC(Resume)(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice);
```

#### Direct form

```c
OPSTATUS Audio_Player_Resume(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `voice` | `AUDIO_PLAYER_TYPE(TVoice) voice` | Voice token. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Restarts paused voice; stale token yields NOT_FOUND.

---

### Example

```c
OPSTATUS status = AUDIO_PLAYER_FUNC(Resume)(player, voice);
```

---

# Player SeekFrame

Seek a voice by PCM frame.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PLAYER_FUNC(SeekFrame)(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice, uint64_t frameIndex);
```

#### Direct form

```c
OPSTATUS Audio_Player_SeekFrame(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice, uint64_t frameIndex);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `voice` | `AUDIO_PLAYER_TYPE(TVoice) voice` | Voice token. |
| `frameIndex` | `uint64_t frameIndex` | Absolute PCM frame position. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Seeks by PCM frame; backend may reject unsupported operations.

---

### Example

```c
OPSTATUS status = AUDIO_PLAYER_FUNC(SeekFrame)(player, voice, frameIndex);
```

---

# Player IsPlaying

Test whether a voice is playing.

### Syntax

#### Macro form

```c
bool AUDIO_PLAYER_FUNC(IsPlaying)(const AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice);
```

#### Direct form

```c
bool Audio_Player_IsPlaying(const AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `const AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `voice` | `AUDIO_PLAYER_TYPE(TVoice) voice` | Voice token. |

---

### Return value

Boolean predicate result.

---

### Remarks

False for missing/stale/paused/completed voices.

---

### Example

```c
bool value = AUDIO_PLAYER_FUNC(IsPlaying)(player, voice);
```

---

# Player Stop

Stop and destroy a voice.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PLAYER_FUNC(Stop)(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice);
```

#### Direct form

```c
OPSTATUS Audio_Player_Stop(AUDIO_PLAYER_TYPE(TPlayer) * player, AUDIO_PLAYER_TYPE(TVoice) voice);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `voice` | `AUDIO_PLAYER_TYPE(TVoice) voice` | Voice token. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Destroys voice and invalidates all copies of its token.

---

### Example

```c
OPSTATUS status = AUDIO_PLAYER_FUNC(Stop)(player, voice);
```

---

# Player Collect

Collect completed voices.

### Syntax

#### Macro form

```c
size_t AUDIO_PLAYER_FUNC(Collect)(AUDIO_PLAYER_TYPE(TPlayer) * player);
```

#### Direct form

```c
size_t Audio_Player_Collect(AUDIO_PLAYER_TYPE(TPlayer) * player);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |

---

### Return value

Number of resources collected.

---

### Remarks

Returns number of voices that reached end and were removed; invalidates collected tokens.

---

### Example

```c
size_t removed = AUDIO_PLAYER_FUNC(Collect)(player);
```

---

# Player Render

Render an offline mix.

### Syntax

#### Macro form

```c
OPSTATUS AUDIO_PLAYER_FUNC(Render)(AUDIO_PLAYER_TYPE(TPlayer) * player, float *outSamples, size_t frameCount, size_t *outFrames);
```

#### Direct form

```c
OPSTATUS Audio_Player_Render(AUDIO_PLAYER_TYPE(TPlayer) * player, float *outSamples, size_t frameCount, size_t *outFrames);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |
| `outSamples` | `float *outSamples` | Writable output sample array. |
| `frameCount` | `size_t frameCount` | Number of audio frames. |
| `outFrames` | `size_t *outFrames` | Output number of generated frames. |

---

### Return value

Returns `STATUS_CONST(SUCCESS)` or a failure `OPSTATUS` (usually `INVALID_ARGUMENT`, overflow, allocation or backend status as applicable).

---

### Remarks

Requires config.offline=true; otherwise NOT_SUPPORTED. Clamps output [-1,1]. On error may expose partial frames and their count.

---

### Example

```c
float stereo[512 * 2]; size_t got = 0;
OPSTATUS status = AUDIO_PLAYER_FUNC(Render)(&player, stereo, 512, &got);
```

---

# Player Close

Close player and destroy all voices.

### Syntax

#### Macro form

```c
void AUDIO_PLAYER_FUNC(Close)(AUDIO_PLAYER_TYPE(TPlayer) * player);
```

#### Direct form

```c
void Audio_Player_Close(AUDIO_PLAYER_TYPE(TPlayer) * player);
```

---

### Parameters

| Name | C type | Explanation |
| --- | --- | --- |
| `player` | `AUDIO_PLAYER_TYPE(TPlayer) * player` | Player handle. |

---

### Return value

None.

---

### Remarks

Destroys all voices and backend resources; repeated Close is safe; tokens remain invalid.

---

### Example

```c
AUDIO_PLAYER_FUNC(Close)(player);
```

---

# MML syntax

The Sequence parser recognizes the following case-insensitive Music Macro Language commands. Unsupported constructs are not silently accepted.

| Command | Meaning | Accepted values |
| --- | --- | --- |
| `T<number>` | BPM tempo | 1–1000, default 120 |
| `O<number>` | Octave | 0–9, default 4 |
| `L<number>` | Default note denominator | Power of two, 1–128, default 4 |
| `V<number>` | Volume | 0–127, default 32 |
| `C D E F G A B` | Note pitch | Optional `#`, `+`, `-` |
| `P` or `R` | Rest | Optional denominator and dots |
| `>` / `<` | Increase/decrease octave | Must remain 0–9 |
| `.` / `..` / `...` | Dotted duration | Up to three trailing dots |

A note or rest may have an explicit denominator such as `C8` or `R4`. The parser does not currently implement chords, instrument changes, or repeat loops. Invalid input returns `STATUS_CONST(INVALID_SEQUENCE)` and can set a byte offset via `outErrorOffset`.

---

# Complete examples

## Portable synthesis

```c
#define AUDIO_NO_DESKTOP
#include "Cosmeron/Modules/Audio/Audio.h"

int main(void) {
    AUDIO_OSCILLATOR_TYPE(TOscillator) oscillator;
    float samples[256];
    OPSTATUS status = AUDIO_OSCILLATOR_FUNC(Init)(
        &oscillator, 48000, AUDIO_OSCILLATOR_CONST(SINE), 440.0);
    if (status != STATUS_CONST(SUCCESS))
        return 1;
    status = AUDIO_OSCILLATOR_FUNC(Render)(&oscillator, samples, 256);
    return status == STATUS_CONST(SUCCESS) ? 0 : 2;
}
```

## Desktop WAV export

```c
#include "Cosmeron/Modules/Audio/Audio.h"

int main(void) {
    AUDIO_PCM_TYPE(TBuffer) buffer = {0};
    OPSTATUS status = AUDIO_BEEP_FUNC(RenderMML)(
        "T120 O4 L4 C D E F G", 48000, &buffer, NULL);
    if (status != STATUS_CONST(SUCCESS))
        return 1;
    status = AUDIO_CODEC_FUNC(WriteWAV)("melody.wav", &buffer);
    AUDIO_PCM_FUNC(Destroy)(&buffer);
    return status == STATUS_CONST(SUCCESS) ? 0 : 2;
}
```

## Desktop offline player

```c
#include "Cosmeron/Modules/Audio/Audio.h"

int main(void) {
    AUDIO_PLAYER_TYPE(TPlayer) player = {0};
    AUDIO_PLAYER_TYPE(TConfig) config = AUDIO_PLAYER_FUNC(Config)();
    AUDIO_PLAYER_TYPE(TVoice) voice;
    float samples[1024];
    size_t frames = 0;
    OPSTATUS status;

    config.offline = true;
    status = AUDIO_PLAYER_FUNC(Open)(&player, &config);
    if (status == STATUS_CONST(SUCCESS))
        status = AUDIO_BEEP_FUNC(PlayTone)(
            &player, 440.0, 100, 0.25f, &voice);
    if (status == STATUS_CONST(SUCCESS))
        status = AUDIO_PLAYER_FUNC(Render)(
            &player, samples, 512, &frames);
    AUDIO_PLAYER_FUNC(Close)(&player);
    return status == STATUS_CONST(SUCCESS) ? 0 : 1;
}
```

---

# Build and deployment

```sh
cc -std=c11 Codespace/Examples/Audio/BeepMusic.c -o beep-music
./beep-music
./beep-music --play
```

The desktop adapter includes a namespaced vendored miniaudio-derived implementation with WAV/MP3/FLAC decoders. Supported desktop profiles compile without separate audio, math or pthread **link flags**; they can still require runtime system libraries and installed audio services. On Linux desktop, the implementation targets glibc >= 2.34.

`AUDIO_NO_DESKTOP` excludes the whole Codec/Device/Mixer/Player API, not merely sound hardware use. The offline Player mode still belongs to the desktop-backed API. No implementation macro is necessary.

---

# Notes

- Public source signatures are extracted from the header prototype macros on `audio-module`. Internal vendored functions are deliberately excluded.
- PCM buffers, parsed sequences, decoders, devices and players own resources. Wavetables, cursor events, and Codec OpenMemory data are borrowed.
- Player PlayBuffer copies its source PCM. Voice tokens become invalid when stopped, collected or when a player is closed/reopened.
- Codec ReadFrames and Player Render may write partial PCM and a corresponding count even when returning an error.
- For realtime hardware rendering, callbacks should avoid file I/O, allocation and blocking waits.
