# Random

The **Random** module provides one-shot random helpers, explicit seedable pseudo-random engines, optional output mixers, integer and real-valued distributions, array shuffling, and multiple methods of collecting entropy samples. It is implemented as C11 headers with inline functions and does not require a separate Random library link step.

```c
#include "Cosmeron/Modules/Random/Random.h"
```

The aggregate `Random.h` includes all packages. Individual package headers can be used when a smaller dependency surface is desired.

**Security distinction:** `RANDOM_ENTROPY_FUNC(System)` is the OS-backed byte generator. The convenience `RANDOM_FUNC(U64)` uses **heuristic** collected samples instead, while Engine and Source are deterministic PRNG mechanisms. Do **not** use the convenience functions, deterministic engines, shuffled arrays or mixers to create security tokens, passwords, cryptographic keys or secrets.

---

# Overview

| Package | Header | Public operations | Purpose |
| --- | --- | ---: | --- |
| [Random](#random-package) | `Random.h` | 6 | Convenience one-shot randomness and inclusive ranges. |
| [Source](#source-package) | `Source/Source.h` | 6 | Owned or borrowed generator state, engine descriptors, reseeding and raw draws. |
| [Distribution](#distribution-package) | `Distribution/Distribution.h` | 4 | Inclusive integer ranges, doubles in [0,1) and booleans driven by a Source. |
| [Shuffle](#shuffle-package) | `Shuffle/Shuffle.h` | 2 | In-place Fisher–Yates shuffling of fixed-size elements. |
| [Entropy](#entropy-package) | `Entropy/Entropy.h` | 7 | OS-backed random byte collection and non-cryptographic heuristic samples. |
| [Entropy Pool](#entropy-pool-package) | `Entropy/Pool/Pool.h` | 3 | Deterministic sample accumulation/mixing. |
| [Mixer](#mixer-package) | `Mixer/Mixer.h` | 8 | Stateless 64-bit integer output mixers. |
| [Engine](#engine-package) | `Engine/Engine.h` | 16 | Eight seedable deterministic PRNG algorithms. |

### Macro form

```c
uint64_t value = RANDOM_FUNC(U64_FromSeed)(42);
```

### Direct form

```c
uint64_t value = Random_U64_FromSeed(42);
```

Both forms use the same underlying function with the default Cosmeron namespace. With a custom `COSMERON_NAMESPACE`, use the macro form or update the direct symbol prefix accordingly.

### Choose the right interface

| Need | Use |
| --- | --- |
| Repeatable sequence | Source_Init with fixed seed, then Source_NextU64 or Distribution |
| No heap-allocated generator state | Source_InitWithState with correctly aligned caller storage |
| OS-backed random bytes | Entropy_System and check OPSTATUS |
| A quick non-secure value | Random_U64, RangeU64, RangeI64, F64 or Bool |
| Bounded values without range bias | Distribution_U64/I64 with a selected Source |
| Shuffle an array | Shuffle_Vector or Shuffle_VectorFromSource |
| Custom engine | Engine Descriptor with Seed/Next callbacks |
| Reversible-looking bit mixing, not entropy | Mixer functions |

---

# API reference

## Function summary

| Package | Function | Description |
| --- | --- | --- |
| Random | [`U64`](#random-u64) | One 64-bit value from heuristic entropy samples. |
| Random | [`U64_FromSeed`](#random-u64_fromseed) | One reproducible 64-bit value from a seed. |
| Random | [`RangeU64`](#random-rangeu64) | A random uint64_t in a closed interval. |
| Random | [`RangeI64`](#random-rangei64) | A random int64_t in a closed interval. |
| Random | [`F64`](#random-f64) | A double from 0 inclusive to 1 exclusive. |
| Random | [`Bool`](#random-bool) | A randomly selected Boolean. |
| Source | [`Init`](#source-init) | Allocates and seeds owned generator state. |
| Source | [`InitWithState`](#source-initwithstate) | Binds and seeds caller-managed engine storage. |
| Source | [`InitSystem`](#source-initsystem) | Initializes an engine using an OS-provided seed. |
| Source | [`Reseed`](#source-reseed) | Resets an initialized Source's PRNG state. |
| Source | [`NextU64`](#source-nextu64) | Advances and reads the next Source output. |
| Source | [`Destroy`](#source-destroy) | Destroys Source bookkeeping and any owned state. |
| Distribution | [`U64`](#distribution-u64) | Uniform-range unsigned 64-bit sampling from a Source. |
| Distribution | [`I64`](#distribution-i64) | Uniform-range signed 64-bit sampling from a Source. |
| Distribution | [`F64`](#distribution-f64) | Floating-point sample in [0,1) from a Source. |
| Distribution | [`Bool`](#distribution-bool) | Boolean sample from a Source. |
| Shuffle | [`VectorFromSource`](#shuffle-vectorfromsource) | In-place array shuffle using a chosen Source. |
| Shuffle | [`Vector`](#shuffle-vector) | In-place array shuffle using a temporary Source. |
| Entropy | [`System`](#entropy-system) | Fills a buffer using OS-provided random bytes. |
| Entropy | [`Address`](#entropy-address) | Sample derived from a memory address. |
| Entropy | [`Clock`](#entropy-clock) | Sample derived from timing readings. |
| Entropy | [`Jitter`](#entropy-jitter) | Heuristic timing jitter sample. |
| Entropy | [`Thread`](#entropy-thread) | Sample derived from a thread identity. |
| Entropy | [`Time`](#entropy-time) | Sample derived from calendar time. |
| Entropy | [`Collect`](#entropy-collect) | Combines heuristic samples into one 64-bit word. |
| Entropy Pool | [`Pool_Init`](#entropy-pool-pool_init) | Initializes an entropy accumulator. |
| Entropy Pool | [`Pool_Add`](#entropy-pool-pool_add) | Mixes a sample into a Pool. |
| Entropy Pool | [`Pool_Finalize`](#entropy-pool-pool_finalize) | Derives a final 64-bit Pool output. |
| Mixer | [`Jenkins`](#mixer-jenkins) | Deterministically mixes one 64-bit input with Jenkins. |
| Mixer | [`Knuth`](#mixer-knuth) | Deterministically mixes one 64-bit input with Knuth. |
| Mixer | [`Murmur3`](#mixer-murmur3) | Deterministically mixes one 64-bit input with Murmur3. |
| Mixer | [`Splitmix64`](#mixer-splitmix64) | Deterministically mixes one 64-bit input with Splitmix64. |
| Mixer | [`Stafford`](#mixer-stafford) | Deterministically mixes one 64-bit input with Stafford. |
| Mixer | [`Wang`](#mixer-wang) | Deterministically mixes one 64-bit input with Wang. |
| Mixer | [`WyHash`](#mixer-wyhash) | Deterministically mixes one 64-bit input with WyHash. |
| Mixer | [`Xorshift`](#mixer-xorshift) | Deterministically mixes one 64-bit input with Xorshift. |
| Engine | [`Lcg_Seed`](#engine-lcg_seed) | Seeds the Lcg engine. |
| Engine | [`Lcg_Next`](#engine-lcg_next) | Advances the Lcg engine. |
| Engine | [`Pcg_Seed`](#engine-pcg_seed) | Seeds the Pcg engine. |
| Engine | [`Pcg_Next`](#engine-pcg_next) | Advances the Pcg engine. |
| Engine | [`Romu_Seed`](#engine-romu_seed) | Seeds the Romu engine. |
| Engine | [`Romu_Next`](#engine-romu_next) | Advances the Romu engine. |
| Engine | [`Sfc_Seed`](#engine-sfc_seed) | Seeds the Sfc engine. |
| Engine | [`Sfc_Next`](#engine-sfc_next) | Advances the Sfc engine. |
| Engine | [`Splitmix_Seed`](#engine-splitmix_seed) | Seeds the Splitmix engine. |
| Engine | [`Splitmix_Next`](#engine-splitmix_next) | Advances the Splitmix engine. |
| Engine | [`WyRand_Seed`](#engine-wyrand_seed) | Seeds the WyRand engine. |
| Engine | [`WyRand_Next`](#engine-wyrand_next) | Advances the WyRand engine. |
| Engine | [`Xoroshiro_Seed`](#engine-xoroshiro_seed) | Seeds the Xoroshiro engine. |
| Engine | [`Xoroshiro_Next`](#engine-xoroshiro_next) | Advances the Xoroshiro engine. |
| Engine | [`Xoshiro_Seed`](#engine-xoshiro_seed) | Seeds the Xoshiro engine. |
| Engine | [`Xoshiro_Next`](#engine-xoshiro_next) | Advances the Xoshiro engine. |

---

# Public types and declarations

| Macro | Default direct type | Meaning |
| --- | --- | --- |
| `RANDOM_SOURCE_TYPE(Value)` | `Random_Source_Value` | Engine descriptor pointer, mixer pointer, state pointer, state size, ownsState flag |
| `RANDOM_ENGINE_TYPE(Descriptor)` | `Random_Engine_Descriptor` | `stateSize`, `stateAlignment`, `nextU64`, `seed` |
| `RANDOM_ENGINE_TYPE(Xoshiro)` and other engines | `Random_Engine_Xoshiro`, etc. | Concrete engine state structures |
| `RANDOM_MIXER_TYPE(Function)` | `Random_Mixer_Function` | `uint64_t (*)(uint64_t)` |
| `RANDOM_ENTROPY_TYPE(Pool)` | `Random_Entropy_Pool` | `uint64_t state`, `uint64_t count` |

The eight engine headers expose `RANDOM_ENGINE_FUNC(Name, Descriptor)`, e.g. `RANDOM_ENGINE_FUNC(Xoshiro, Descriptor)` (direct: `Random_Engine_Xoshiro_Descriptor`). These are **descriptor objects**, not callable functions. Descriptors include the size, alignment, and wrapper callbacks for a given engine state.

### Engine state types

| Engine | State fields | Note |
| --- | --- | --- |
| Lcg | `uint64_t value` | Linear congruential |
| Pcg | `uint64_t state, stream` | Effective output 32 bits |
| Romu | `uint64_t x, y, z` | Three-word state |
| Sfc | `uint64_t a, b, c, counter` | Three-word state plus counter |
| Splitmix | `uint64_t value` | One-word state |
| WyRand | `uint64_t value` | One-word state |
| Xoroshiro | `uint64_t state[2]` | Two-word state |
| Xoshiro | `uint64_t state[4]` | Four-word state |

---

# General error and ownership contracts

`Source`, `Distribution`, `Shuffle`, `Entropy_System`, and `Pool` operations that can fail use `OPSTATUS`, generally with `STATUS_CONST(SUCCESS)`, `STATUS_CONST(INVALID_ARGUMENT)`, `STATUS_CONST(NOT_AVAILABLE)`, `STATUS_CONST(OUT_OF_MEMORY)`, `STATUS_CONST(ARITHMETIC_OVERFLOW)` or `STATUS_CONST(OUT_OF_RANGE)` where appropriate. Direct-return functions (Random helpers, mixers, engine Next, heuristic entropy samplers) have no status output.

A Source created with `Source_Init` owns heap state and requires `Source_Destroy`. With `Source_InitWithState`, storage remains owned by the caller and must outlive Source use. Repeatedly initializing a live Source without Destroy can leak its previous owned state.

Random output quality depends on the engine and seed; a mixer can rearrange bits but does not create entropy or compensate for a poor seed. The module has no global `Random_Seed(42)` API in this branch; use `Source_Reseed` or directly `Engine_Seed`.

---

# Random package

Header: `Cosmeron/Modules/Random/Random.h`

Convenience one-shot randomness and inclusive ranges.

### Function summary

| Function | Description |
| --- | --- |
| [`U64`](#random-u64) | One 64-bit value from heuristic entropy samples. |
| [`U64_FromSeed`](#random-u64_fromseed) | One reproducible 64-bit value from a seed. |
| [`RangeU64`](#random-rangeu64) | A random uint64_t in a closed interval. |
| [`RangeI64`](#random-rangei64) | A random int64_t in a closed interval. |
| [`F64`](#random-f64) | A double from 0 inclusive to 1 exclusive. |
| [`Bool`](#random-bool) | A randomly selected Boolean. |

Entries below show each function's **declaration**, both naming forms, parameters, return and a focused usage example. Unless a snippet creates the Source or Pool, assume it has already been initialized.

---

# Random U64

One 64-bit value from heuristic entropy samples.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_FUNC(U64)(void);
```

#### Direct form

```c
static inline uint64_t Random_U64(void);
```

---

### Parameters

None.

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Calls Entropy_Collect, **not** Entropy_System. This output has no cryptographic unpredictability guarantee.

---

### Example

```c
uint64_t value = RANDOM_FUNC(U64)();
```

---

# Random U64_FromSeed

One reproducible 64-bit value from a seed.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_FUNC(U64_FromSeed)(uint64_t seed);
```

#### Direct form

```c
static inline uint64_t Random_U64_FromSeed(uint64_t seed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Seeds a temporary Xoshiro generator and returns its **first** output. Repeated calls with the same seed return the same value; no global PRNG is advanced.

---

### Example

```c
uint64_t first = RANDOM_FUNC(U64_FromSeed)(42);
uint64_t again = RANDOM_FUNC(U64_FromSeed)(42);
```

---

# Random RangeU64

A random uint64_t in a closed interval.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_FUNC(RangeU64)(uint64_t minimum, uint64_t maximum);
```

#### Direct form

```c
static inline uint64_t Random_RangeU64(uint64_t minimum, uint64_t maximum);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `minimum` | `uint64_t minimum` | Lower inclusive range endpoint; reverse-order bounds are swapped where applicable. |
| `maximum` | `uint64_t maximum` | Upper inclusive range endpoint; reverse-order bounds are swapped where applicable. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Uses a temporary Xoshiro engine seeded by heuristic Entropy_Collect. Swaps reversed bounds, includes both endpoints and uses rejection sampling to avoid ordinary modulo bias.

---

### Example

```c
uint64_t die = RANDOM_FUNC(RangeU64)(1, 6);
```

---

# Random RangeI64

A random int64_t in a closed interval.

### Syntax

#### Macro form

```c
static inline int64_t RANDOM_FUNC(RangeI64)(int64_t minimum, int64_t maximum);
```

#### Direct form

```c
static inline int64_t Random_RangeI64(int64_t minimum, int64_t maximum);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `minimum` | `int64_t minimum` | Lower inclusive range endpoint; reverse-order bounds are swapped where applicable. |
| `maximum` | `int64_t maximum` | Upper inclusive range endpoint; reverse-order bounds are swapped where applicable. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Uses the same temporary seeded Xoshiro approach; swaps reversed bounds and supports signed negative endpoints and the full signed range.

---

### Example

```c
int64_t value = RANDOM_FUNC(RangeI64)(-10, 10);
```

---

# Random F64

A double from 0 inclusive to 1 exclusive.

### Syntax

#### Macro form

```c
static inline double RANDOM_FUNC(F64)(void);
```

#### Direct form

```c
static inline double Random_F64(void);
```

---

### Parameters

None.

---

### Return value

`double`: value in [0,1).

---

### Remarks

Uses the top 53 bits of a one-shot U64 value multiplied by 2^-53; result is in [0,1).

---

### Example

```c
double unit = RANDOM_FUNC(F64)();
```

---

# Random Bool

A randomly selected Boolean.

### Syntax

#### Macro form

```c
static inline bool RANDOM_FUNC(Bool)(void);
```

#### Direct form

```c
static inline bool Random_Bool(void);
```

---

### Parameters

None.

---

### Return value

`bool`: true or false.

---

### Remarks

Uses the least significant bit of one `Random_U64()` output.

---

### Example

```c
bool coin = RANDOM_FUNC(Bool)();
```

---

# Source package

Header: `Cosmeron/Modules/Random/Source/Source.h`

Owned or borrowed generator state, engine descriptors, reseeding and raw draws.

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#source-init) | Allocates and seeds owned generator state. |
| [`InitWithState`](#source-initwithstate) | Binds and seeds caller-managed engine storage. |
| [`InitSystem`](#source-initsystem) | Initializes an engine using an OS-provided seed. |
| [`Reseed`](#source-reseed) | Resets an initialized Source's PRNG state. |
| [`NextU64`](#source-nextu64) | Advances and reads the next Source output. |
| [`Destroy`](#source-destroy) | Destroys Source bookkeeping and any owned state. |

Entries below show each function's **declaration**, both naming forms, parameters, return and a focused usage example. Unless a snippet creates the Source or Pool, assume it has already been initialized.

---

# Source Init

Allocates and seeds owned generator state.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_SOURCE_FUNC(Init)( RANDOM_SOURCE_TYPE(Value) *source, const RANDOM_ENGINE_TYPE(Descriptor) *engine, uint64_t seed, RANDOM_MIXER_TYPE(Function) mixer);
```

#### Direct form

```c
static inline OPSTATUS Random_Source_Init( Random_Source_Value *source, const Random_Engine_Descriptor *engine, uint64_t seed, Random_Mixer_Function mixer);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |
| `engine` | `const RANDOM_ENGINE_TYPE(Descriptor) *engine` | Pointer to an Engine Descriptor, including state size/alignment and Seed/Next callbacks. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |
| `mixer` | `RANDOM_MIXER_TYPE(Function) mixer` | Optional Mixer Function pointer; pass NULL for raw engine output. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Validates descriptor callbacks, positive size/alignment and allocator alignment limits; allocates `engine->stateSize` bytes and sets `ownsState=true`. Destroy after success. Do not Init an already live owning Source without first Destroying it.

---

### Example

```c
RANDOM_SOURCE_TYPE(Value) source = {0};
OPSTATUS status = RANDOM_SOURCE_FUNC(Init)(&source,
    &RANDOM_ENGINE_FUNC(Xoshiro, Descriptor), 42, NULL);
```

---

# Source InitWithState

Binds and seeds caller-managed engine storage.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_SOURCE_FUNC(InitWithState)( RANDOM_SOURCE_TYPE(Value) *source, const RANDOM_ENGINE_TYPE(Descriptor) *engine, void *state, size_t stateSize, uint64_t seed, RANDOM_MIXER_TYPE(Function) mixer);
```

#### Direct form

```c
static inline OPSTATUS Random_Source_InitWithState( Random_Source_Value *source, const Random_Engine_Descriptor *engine, void *state, size_t stateSize, uint64_t seed, Random_Mixer_Function mixer);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |
| `engine` | `const RANDOM_ENGINE_TYPE(Descriptor) *engine` | Pointer to an Engine Descriptor, including state size/alignment and Seed/Next callbacks. |
| `state` | `void *state` | Pointer to aligned state storage managed by caller or engine. |
| `stateSize` | `size_t stateSize` | Size in bytes of supplied state storage. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |
| `mixer` | `RANDOM_MIXER_TYPE(Function) mixer` | Optional Mixer Function pointer; pass NULL for raw engine output. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Requires state bytes at least engine->stateSize, correct address alignment and a valid engine. Sets ownsState=false; Destroy does not free the caller's storage.

---

### Example

```c
RANDOM_ENGINE_TYPE(Xoshiro) storage;
RANDOM_SOURCE_TYPE(Value) source = {0};
OPSTATUS status = RANDOM_SOURCE_FUNC(InitWithState)(&source,
    &RANDOM_ENGINE_FUNC(Xoshiro, Descriptor), &storage,
    sizeof(storage), 42, NULL);
```

---

# Source InitSystem

Initializes an engine using an OS-provided seed.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_SOURCE_FUNC(InitSystem)( RANDOM_SOURCE_TYPE(Value) *source, const RANDOM_ENGINE_TYPE(Descriptor) *engine, RANDOM_MIXER_TYPE(Function) mixer);
```

#### Direct form

```c
static inline OPSTATUS Random_Source_InitSystem( Random_Source_Value *source, const Random_Engine_Descriptor *engine, Random_Mixer_Function mixer);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |
| `engine` | `const RANDOM_ENGINE_TYPE(Descriptor) *engine` | Pointer to an Engine Descriptor, including state size/alignment and Seed/Next callbacks. |
| `mixer` | `RANDOM_MIXER_TYPE(Function) mixer` | Optional Mixer Function pointer; pass NULL for raw engine output. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Requests 8 system-random bytes, then delegates to Source_Init. May return NOT_AVAILABLE. A seed from the OS does **not** make Xoshiro or the other engines cryptographically secure.

---

### Example

```c
RANDOM_SOURCE_TYPE(Value) source = {0};
OPSTATUS status = RANDOM_SOURCE_FUNC(InitSystem)(&source,
    &RANDOM_ENGINE_FUNC(Xoshiro, Descriptor), NULL);
```

---

# Source Reseed

Resets an initialized Source's PRNG state.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_SOURCE_FUNC(Reseed)( RANDOM_SOURCE_TYPE(Value) *source, uint64_t seed);
```

#### Direct form

```c
static inline OPSTATUS Random_Source_Reseed( Random_Source_Value *source, uint64_t seed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Uses descriptor.seed on existing state; same engine/seed/mixer reproduces the same stream. Does not change storage ownership.

---

### Example

```c
OPSTATUS status = RANDOM_SOURCE_FUNC(Reseed)(&source, 42);
```

---

# Source NextU64

Advances and reads the next Source output.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_SOURCE_FUNC(NextU64)( RANDOM_SOURCE_TYPE(Value) *source, uint64_t *out);
```

#### Direct form

```c
static inline OPSTATUS Random_Source_NextU64( Random_Source_Value *source, uint64_t *out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |
| `out` | `uint64_t *out` | Writable 64-bit output destination. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Calls engine.nextU64, followed by optional mixer. Validates source state and output pointer; some engines (PCG) return fewer than 64 effective bits.

---

### Example

```c
uint64_t value = 0;
OPSTATUS status = RANDOM_SOURCE_FUNC(NextU64)(&source, &value);
```

---

# Source Destroy

Destroys Source bookkeeping and any owned state.

### Syntax

#### Macro form

```c
static inline void RANDOM_SOURCE_FUNC(Destroy)( RANDOM_SOURCE_TYPE(Value) *source);
```

#### Direct form

```c
static inline void Random_Source_Destroy( Random_Source_Value *source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |

---

### Return value

None.

---

### Remarks

Only frees state if ownsState=true; clears engine, mixer, state, stateSize and ownsState. Accepts NULL; borrowed state remains caller-owned.

---

### Example

```c
RANDOM_SOURCE_FUNC(Destroy)(&source);
```

---

# Distribution package

Header: `Cosmeron/Modules/Random/Distribution/Distribution.h`

Inclusive integer ranges, doubles in [0,1) and booleans driven by a Source.

### Function summary

| Function | Description |
| --- | --- |
| [`U64`](#distribution-u64) | Uniform-range unsigned 64-bit sampling from a Source. |
| [`I64`](#distribution-i64) | Uniform-range signed 64-bit sampling from a Source. |
| [`F64`](#distribution-f64) | Floating-point sample in [0,1) from a Source. |
| [`Bool`](#distribution-bool) | Boolean sample from a Source. |

Entries below show each function's **declaration**, both naming forms, parameters, return and a focused usage example. Unless a snippet creates the Source or Pool, assume it has already been initialized.

---

# Distribution U64

Uniform-range unsigned 64-bit sampling from a Source.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_DISTRIBUTION_FUNC(U64)( RANDOM_SOURCE_TYPE(Value) *source, uint64_t minimum, uint64_t maximum, uint64_t *outValue);
```

#### Direct form

```c
static inline OPSTATUS Random_Distribution_U64( Random_Source_Value *source, uint64_t minimum, uint64_t maximum, uint64_t *outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |
| `minimum` | `uint64_t minimum` | Lower inclusive range endpoint; reverse-order bounds are swapped where applicable. |
| `maximum` | `uint64_t maximum` | Upper inclusive range endpoint; reverse-order bounds are swapped where applicable. |
| `outValue` | `uint64_t *outValue` | Writable output destination. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Validates the source and output; both bounds inclusive and reversed bounds swapped. Rejection sampling handles full UINT64_MAX span without ordinary modulo bias.

---

### Example

```c
uint64_t value = 0;
OPSTATUS status = RANDOM_DISTRIBUTION_FUNC(U64)(
    &source, 1, 6, &value);
```

---

# Distribution I64

Uniform-range signed 64-bit sampling from a Source.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_DISTRIBUTION_FUNC(I64)( RANDOM_SOURCE_TYPE(Value) *source, int64_t minimum, int64_t maximum, int64_t *outValue);
```

#### Direct form

```c
static inline OPSTATUS Random_Distribution_I64( Random_Source_Value *source, int64_t minimum, int64_t maximum, int64_t *outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |
| `minimum` | `int64_t minimum` | Lower inclusive range endpoint; reverse-order bounds are swapped where applicable. |
| `maximum` | `int64_t maximum` | Upper inclusive range endpoint; reverse-order bounds are swapped where applicable. |
| `outValue` | `int64_t *outValue` | Writable output destination. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Validates source and output; normalizes bounds. Uses sign-bit biasing to handle negative endpoints and the complete int64_t interval.

---

### Example

```c
int64_t value = 0;
OPSTATUS status = RANDOM_DISTRIBUTION_FUNC(I64)(
    &source, -6, 6, &value);
```

---

# Distribution F64

Floating-point sample in [0,1) from a Source.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_DISTRIBUTION_FUNC(F64)( RANDOM_SOURCE_TYPE(Value) *source, double *outValue);
```

#### Direct form

```c
static inline OPSTATUS Random_Distribution_F64( Random_Source_Value *source, double *outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |
| `outValue` | `double *outValue` | Writable output destination. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Consumes a Source U64, then maps its top 53 bits to a double with an exclusive upper endpoint.

---

### Example

```c
double value = 0;
OPSTATUS status = RANDOM_DISTRIBUTION_FUNC(F64)(&source, &value);
```

---

# Distribution Bool

Boolean sample from a Source.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_DISTRIBUTION_FUNC(Bool)( RANDOM_SOURCE_TYPE(Value) *source, bool *outValue);
```

#### Direct form

```c
static inline OPSTATUS Random_Distribution_Bool( Random_Source_Value *source, bool *outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |
| `outValue` | `bool *outValue` | Writable output destination. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Consumes one Source U64 and extracts the low bit; result is returned via bool out parameter.

---

### Example

```c
bool value = false;
OPSTATUS status = RANDOM_DISTRIBUTION_FUNC(Bool)(&source, &value);
```

---

# Shuffle package

Header: `Cosmeron/Modules/Random/Shuffle/Shuffle.h`

In-place Fisher–Yates shuffling of fixed-size elements.

### Function summary

| Function | Description |
| --- | --- |
| [`VectorFromSource`](#shuffle-vectorfromsource) | In-place array shuffle using a chosen Source. |
| [`Vector`](#shuffle-vector) | In-place array shuffle using a temporary Source. |

Entries below show each function's **declaration**, both naming forms, parameters, return and a focused usage example. Unless a snippet creates the Source or Pool, assume it has already been initialized.

---

# Shuffle VectorFromSource

In-place array shuffle using a chosen Source.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_SHUFFLE_FUNC(VectorFromSource)( void *vector, size_t count, size_t elementSize, RANDOM_SOURCE_TYPE(Value) *source);
```

#### Direct form

```c
static inline OPSTATUS Random_Shuffle_VectorFromSource( void *vector, size_t count, size_t elementSize, Random_Source_Value *source);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vector` | `void *vector` | Writable array memory to shuffle in place. |
| `count` | `size_t count` | Number of elements in the array. |
| `elementSize` | `size_t elementSize` | Byte size of each array element; must be greater than zero. |
| `source` | `RANDOM_SOURCE_TYPE(Value) *source` | Pointer to an initialized and valid Random Source. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Fisher–Yates-style reverse traversal, using the unbiased integer range distribution. Requires nonzero elementSize and a valid Source even for count=0. Count*elementSize and pointer range are checked. Failure mid-shuffle may leave partial permutation.

---

### Example

```c
int values[] = {1, 2, 3, 4};
OPSTATUS status = RANDOM_SHUFFLE_FUNC(VectorFromSource)(
    values, 4, sizeof values[0], &source);
```

---

# Shuffle Vector

In-place array shuffle using a temporary Source.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_SHUFFLE_FUNC(Vector)( void *vector, size_t count, size_t elementSize);
```

#### Direct form

```c
static inline OPSTATUS Random_Shuffle_Vector( void *vector, size_t count, size_t elementSize);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vector` | `void *vector` | Writable array memory to shuffle in place. |
| `count` | `size_t count` | Number of elements in the array. |
| `elementSize` | `size_t elementSize` | Byte size of each array element; must be greater than zero. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Creates an owning Xoshiro Source seeded with Entropy_Collect, calls VectorFromSource and destroys Source. Does not use OS System entropy directly; count=0 returns success with nonzero elementSize.

---

### Example

```c
int values[] = {1, 2, 3, 4};
OPSTATUS status = RANDOM_SHUFFLE_FUNC(Vector)(
    values, 4, sizeof values[0]);
```

---

# Entropy package

Header: `Cosmeron/Modules/Random/Entropy/Entropy.h`

OS-backed random byte collection and non-cryptographic heuristic samples.

### Function summary

| Function | Description |
| --- | --- |
| [`System`](#entropy-system) | Fills a buffer using OS-provided random bytes. |
| [`Address`](#entropy-address) | Sample derived from a memory address. |
| [`Clock`](#entropy-clock) | Sample derived from timing readings. |
| [`Jitter`](#entropy-jitter) | Heuristic timing jitter sample. |
| [`Thread`](#entropy-thread) | Sample derived from a thread identity. |
| [`Time`](#entropy-time) | Sample derived from calendar time. |
| [`Collect`](#entropy-collect) | Combines heuristic samples into one 64-bit word. |

Entries below show each function's **declaration**, both naming forms, parameters, return and a focused usage example. Unless a snippet creates the Source or Pool, assume it has already been initialized.

---

# Entropy System

Fills a buffer using OS-provided random bytes.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_ENTROPY_FUNC(System)( void *destination, size_t size);
```

#### Direct form

```c
static inline OPSTATUS Random_Entropy_System( void *destination, size_t size);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `destination` | `void *destination` | Writable output buffer. |
| `size` | `size_t size` | Byte count of requested entropy or array element count, according to signature. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Windows uses dynamically loaded BCryptGenRandom, Linux getrandom, macOS/FreeBSD /dev/urandom. Other targets return NOT_AVAILABLE. NULL with nonzero size returns INVALID_ARGUMENT; partial bytes may have been written if an I/O error occurs.

---

### Example

```c
uint8_t randomBytes[32];
OPSTATUS status = RANDOM_ENTROPY_FUNC(System)(
    randomBytes, sizeof randomBytes);
```

---

# Entropy Address

Sample derived from a memory address.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENTROPY_FUNC(Address)(void);
```

#### Direct form

```c
static inline uint64_t Random_Entropy_Address(void);
```

---

### Parameters

None.

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Returns a converted local stack address. ASLR/address variability is not a measurable secure entropy guarantee.

---

### Example

```c
uint64_t sample = RANDOM_ENTROPY_FUNC(Address)();
```

---

# Entropy Clock

Sample derived from timing readings.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENTROPY_FUNC(Clock)(void);
```

#### Direct form

```c
static inline uint64_t Random_Entropy_Clock(void);
```

---

### Parameters

None.

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Combines clock() with timespec_get(TIME_UTC) when available. Environment timing is not a cryptographically secure source by itself.

---

### Example

```c
uint64_t sample = RANDOM_ENTROPY_FUNC(Clock)();
```

---

# Entropy Jitter

Heuristic timing jitter sample.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENTROPY_FUNC(Jitter)(void);
```

#### Direct form

```c
static inline uint64_t Random_Entropy_Jitter(void);
```

---

### Parameters

None.

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Performs 16 timing samples with a short local computation and mixes differences. This is not a certified entropy estimator and guarantees no minimum randomness.

---

### Example

```c
uint64_t sample = RANDOM_ENTROPY_FUNC(Jitter)();
```

---

# Entropy Thread

Sample derived from a thread identity.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENTROPY_FUNC(Thread)(void);
```

#### Direct form

```c
static inline uint64_t Random_Entropy_Thread(void);
```

---

### Parameters

None.

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Windows current thread ID, pthread_self on macOS/FreeBSD, and errno address on other targets. Identifiers can be predictable.

---

### Example

```c
uint64_t sample = RANDOM_ENTROPY_FUNC(Thread)();
```

---

# Entropy Time

Sample derived from calendar time.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENTROPY_FUNC(Time)(void);
```

#### Direct form

```c
static inline uint64_t Random_Entropy_Time(void);
```

---

### Parameters

None.

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Returns (uint64_t)time(NULL), often predictable to the second. Not appropriate by itself as a security seed.

---

### Example

```c
uint64_t sample = RANDOM_ENTROPY_FUNC(Time)();
```

---

# Entropy Collect

Combines heuristic samples into one 64-bit word.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENTROPY_FUNC(Collect)(void);
```

#### Direct form

```c
static inline uint64_t Random_Entropy_Collect(void);
```

---

### Parameters

None.

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Uses a deterministic Pool with addresses, clock, jitter, thread identity and time. **Does not invoke Entropy_System**. It neither measures entropy bits nor guarantees cryptographic unpredictability.

---

### Example

```c
uint64_t sample = RANDOM_ENTROPY_FUNC(Collect)();
```

---

# Entropy Pool package

Header: `Cosmeron/Modules/Random/Entropy/Pool/Pool.h`

Deterministic sample accumulation/mixing.

### Function summary

| Function | Description |
| --- | --- |
| [`Pool_Init`](#entropy-pool-pool_init) | Initializes an entropy accumulator. |
| [`Pool_Add`](#entropy-pool-pool_add) | Mixes a sample into a Pool. |
| [`Pool_Finalize`](#entropy-pool-pool_finalize) | Derives a final 64-bit Pool output. |

Entries below show each function's **declaration**, both naming forms, parameters, return and a focused usage example. Unless a snippet creates the Source or Pool, assume it has already been initialized.

---

# Entropy Pool Pool_Init

Initializes an entropy accumulator.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_ENTROPY_FUNC(Pool_Init)( RANDOM_ENTROPY_TYPE(Pool) *pool);
```

#### Direct form

```c
static inline OPSTATUS Random_Entropy_Pool_Init( Random_Entropy_Pool *pool);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `pool` | `RANDOM_ENTROPY_TYPE(Pool) *pool` | Entropy Pool pointer (required). |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Sets fixed initial state 0x6a09e667f3bcc909 and count=0. No OS randomness is obtained.

---

### Example

```c
RANDOM_ENTROPY_TYPE(Pool) pool;
OPSTATUS status = RANDOM_ENTROPY_FUNC(Pool_Init)(&pool);
```

---

# Entropy Pool Pool_Add

Mixes a sample into a Pool.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_ENTROPY_FUNC(Pool_Add)( RANDOM_ENTROPY_TYPE(Pool) *pool, uint64_t value);
```

#### Direct form

```c
static inline OPSTATUS Random_Entropy_Pool_Add( Random_Entropy_Pool *pool, uint64_t value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `pool` | `RANDOM_ENTROPY_TYPE(Pool) *pool` | Entropy Pool pointer (required). |
| `value` | `uint64_t value` | 64-bit input word or sample. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Mixes current state, sample and insertion count, then increments count. Predictable inputs do not become secure because they were mixed.

---

### Example

```c
OPSTATUS status = RANDOM_ENTROPY_FUNC(Pool_Add)(&pool, 1234);
```

---

# Entropy Pool Pool_Finalize

Derives a final 64-bit Pool output.

### Syntax

#### Macro form

```c
static inline OPSTATUS RANDOM_ENTROPY_FUNC(Pool_Finalize)( const RANDOM_ENTROPY_TYPE(Pool) *pool, uint64_t *outValue);
```

#### Direct form

```c
static inline OPSTATUS Random_Entropy_Pool_Finalize( const Random_Entropy_Pool *pool, uint64_t *outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `pool` | `const RANDOM_ENTROPY_TYPE(Pool) *pool` | Entropy Pool pointer (required). |
| `outValue` | `uint64_t *outValue` | Writable output destination. |

---

### Return value

`OPSTATUS`: `SUCCESS` or an operation-specific failure status; output pointers receive values on success unless otherwise noted.

---

### Remarks

Validates Pool and output pointer. Does not consume/reset the Pool, so successive Finalize calls without Add produce identical results.

---

### Example

```c
uint64_t mixed = 0;
OPSTATUS status = RANDOM_ENTROPY_FUNC(Pool_Finalize)(&pool, &mixed);
```

---

# Mixer package

Header: `Cosmeron/Modules/Random/Mixer/Mixer.h`

Stateless 64-bit integer output mixers.

### Function summary

| Function | Description |
| --- | --- |
| [`Jenkins`](#mixer-jenkins) | Deterministically mixes one 64-bit input with Jenkins. |
| [`Knuth`](#mixer-knuth) | Deterministically mixes one 64-bit input with Knuth. |
| [`Murmur3`](#mixer-murmur3) | Deterministically mixes one 64-bit input with Murmur3. |
| [`Splitmix64`](#mixer-splitmix64) | Deterministically mixes one 64-bit input with Splitmix64. |
| [`Stafford`](#mixer-stafford) | Deterministically mixes one 64-bit input with Stafford. |
| [`Wang`](#mixer-wang) | Deterministically mixes one 64-bit input with Wang. |
| [`WyHash`](#mixer-wyhash) | Deterministically mixes one 64-bit input with WyHash. |
| [`Xorshift`](#mixer-xorshift) | Deterministically mixes one 64-bit input with Xorshift. |

Entries below show each function's **declaration**, both naming forms, parameters, return and a focused usage example. Unless a snippet creates the Source or Pool, assume it has already been initialized.

---

# Mixer Jenkins

Deterministically mixes one 64-bit input with Jenkins.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_MIXER_FUNC(Jenkins)(uint64_t value);
```

#### Direct form

```c
static inline uint64_t Random_Mixer_Jenkins(uint64_t value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `uint64_t value` | 64-bit input word or sample. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Shift/XOR/add Jenkins-style mixing. This stateless mapping does not add entropy or turn an input into a cryptographic random value.

---

### Example

```c
uint64_t mixed = RANDOM_MIXER_FUNC(Jenkins)(UINT64_C(1234567));
```

---

# Mixer Knuth

Deterministically mixes one 64-bit input with Knuth.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_MIXER_FUNC(Knuth)(uint64_t value);
```

#### Direct form

```c
static inline uint64_t Random_Mixer_Knuth(uint64_t value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `uint64_t value` | 64-bit input word or sample. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Multiplication by a fixed 64-bit constant. This stateless mapping does not add entropy or turn an input into a cryptographic random value.

---

### Example

```c
uint64_t mixed = RANDOM_MIXER_FUNC(Knuth)(UINT64_C(1234567));
```

---

# Mixer Murmur3

Deterministically mixes one 64-bit input with Murmur3.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_MIXER_FUNC(Murmur3)(uint64_t value);
```

#### Direct form

```c
static inline uint64_t Random_Mixer_Murmur3(uint64_t value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `uint64_t value` | 64-bit input word or sample. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

A MurmurHash3-style avalanche finalizer. This stateless mapping does not add entropy or turn an input into a cryptographic random value.

---

### Example

```c
uint64_t mixed = RANDOM_MIXER_FUNC(Murmur3)(UINT64_C(1234567));
```

---

# Mixer Splitmix64

Deterministically mixes one 64-bit input with Splitmix64.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_MIXER_FUNC(Splitmix64)(uint64_t value);
```

#### Direct form

```c
static inline uint64_t Random_Mixer_Splitmix64(uint64_t value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `uint64_t value` | 64-bit input word or sample. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

**Currently delegates to Murmur3** rather than the Stafford/SplitMix64 finalizer. This stateless mapping does not add entropy or turn an input into a cryptographic random value.

---

### Example

```c
uint64_t mixed = RANDOM_MIXER_FUNC(Splitmix64)(UINT64_C(1234567));
```

---

# Mixer Stafford

Deterministically mixes one 64-bit input with Stafford.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_MIXER_FUNC(Stafford)(uint64_t value);
```

#### Direct form

```c
static inline uint64_t Random_Mixer_Stafford(uint64_t value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `uint64_t value` | 64-bit input word or sample. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

A Stafford/SplitMix64-style final mixing sequence. This stateless mapping does not add entropy or turn an input into a cryptographic random value.

---

### Example

```c
uint64_t mixed = RANDOM_MIXER_FUNC(Stafford)(UINT64_C(1234567));
```

---

# Mixer Wang

Deterministically mixes one 64-bit input with Wang.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_MIXER_FUNC(Wang)(uint64_t value);
```

#### Direct form

```c
static inline uint64_t Random_Mixer_Wang(uint64_t value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `uint64_t value` | 64-bit input word or sample. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Wang-inspired bit shifting and integer arithmetic. This stateless mapping does not add entropy or turn an input into a cryptographic random value.

---

### Example

```c
uint64_t mixed = RANDOM_MIXER_FUNC(Wang)(UINT64_C(1234567));
```

---

# Mixer WyHash

Deterministically mixes one 64-bit input with WyHash.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_MIXER_FUNC(WyHash)(uint64_t value);
```

#### Direct form

```c
static inline uint64_t Random_Mixer_WyHash(uint64_t value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `uint64_t value` | 64-bit input word or sample. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

128-bit product XOR where available; uses Stafford fallback without 128-bit arithmetic. This stateless mapping does not add entropy or turn an input into a cryptographic random value.

---

### Example

```c
uint64_t mixed = RANDOM_MIXER_FUNC(WyHash)(UINT64_C(1234567));
```

---

# Mixer Xorshift

Deterministically mixes one 64-bit input with Xorshift.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_MIXER_FUNC(Xorshift)(uint64_t value);
```

#### Direct form

```c
static inline uint64_t Random_Mixer_Xorshift(uint64_t value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `uint64_t value` | 64-bit input word or sample. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

A series of XOR and left/right shifts. This stateless mapping does not add entropy or turn an input into a cryptographic random value.

---

### Example

```c
uint64_t mixed = RANDOM_MIXER_FUNC(Xorshift)(UINT64_C(1234567));
```

---

# Engine package

Header: `Cosmeron/Modules/Random/Engine/Engine.h`

Eight seedable deterministic PRNG algorithms.

### Function summary

| Function | Description |
| --- | --- |
| [`Lcg_Seed`](#engine-lcg_seed) | Sets a reproducible initial Lcg state. |
| [`Lcg_Next`](#engine-lcg_next) | Advances Lcg and yields a pseudo-random word. |
| [`Pcg_Seed`](#engine-pcg_seed) | Sets a reproducible initial Pcg state. |
| [`Pcg_Next`](#engine-pcg_next) | Advances Pcg and yields a pseudo-random word. |
| [`Romu_Seed`](#engine-romu_seed) | Sets a reproducible initial Romu state. |
| [`Romu_Next`](#engine-romu_next) | Advances Romu and yields a pseudo-random word. |
| [`Sfc_Seed`](#engine-sfc_seed) | Sets a reproducible initial Sfc state. |
| [`Sfc_Next`](#engine-sfc_next) | Advances Sfc and yields a pseudo-random word. |
| [`Splitmix_Seed`](#engine-splitmix_seed) | Sets a reproducible initial Splitmix state. |
| [`Splitmix_Next`](#engine-splitmix_next) | Advances Splitmix and yields a pseudo-random word. |
| [`WyRand_Seed`](#engine-wyrand_seed) | Sets a reproducible initial WyRand state. |
| [`WyRand_Next`](#engine-wyrand_next) | Advances WyRand and yields a pseudo-random word. |
| [`Xoroshiro_Seed`](#engine-xoroshiro_seed) | Sets a reproducible initial Xoroshiro state. |
| [`Xoroshiro_Next`](#engine-xoroshiro_next) | Advances Xoroshiro and yields a pseudo-random word. |
| [`Xoshiro_Seed`](#engine-xoshiro_seed) | Sets a reproducible initial Xoshiro state. |
| [`Xoshiro_Next`](#engine-xoshiro_next) | Advances Xoshiro and yields a pseudo-random word. |

Entries below show each function's **declaration**, both naming forms, parameters, return and a focused usage example. Unless a snippet creates the Source or Pool, assume it has already been initialized.

---

# Engine Lcg_Seed

Seeds the Lcg engine state.

### Syntax

#### Macro form

```c
static inline void RANDOM_ENGINE_FUNC(Lcg, Seed)(RANDOM_ENGINE_TYPE(Lcg) *state, uint64_t seed);
```

#### Direct form

```c
static inline void Random_Engine_Lcg_Seed(Random_Engine_Lcg *state, uint64_t seed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Lcg) *state` | Pointer to aligned state storage managed by caller or engine. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |

---

### Return value

None.

---

### Remarks

64-bit linear congruential recurrence; state advances by multiplication and increment. Given the same seed, engine state is reproducible. Pass an appropriate engine state pointer; these raw operations do not validate NULL.

---

### Example

```c
RANDOM_ENGINE_TYPE(Lcg) state;
RANDOM_ENGINE_FUNC(Lcg, Seed)(&state, 42);
```

---

# Engine Lcg_Next

Returns the next Lcg pseudo-random output and advances its state.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENGINE_FUNC(Lcg, Next)(RANDOM_ENGINE_TYPE(Lcg) *state);
```

#### Direct form

```c
static inline uint64_t Random_Engine_Lcg_Next(Random_Engine_Lcg *state);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Lcg) *state` | Pointer to aligned state storage managed by caller or engine. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

64-bit linear congruential recurrence; state advances by multiplication and increment. Do not call Next on an unseeded state. An engine is deterministic, not a cryptographically secure generator.

---

### Example

```c
RANDOM_ENGINE_TYPE(Lcg) state;
RANDOM_ENGINE_FUNC(Lcg, Seed)(&state, 42);
uint64_t next = RANDOM_ENGINE_FUNC(Lcg, Next)(&state);
```

---

# Engine Pcg_Seed

Seeds the Pcg engine state.

### Syntax

#### Macro form

```c
static inline void RANDOM_ENGINE_FUNC(Pcg, Seed)(RANDOM_ENGINE_TYPE(Pcg) *state, uint64_t seed);
```

#### Direct form

```c
static inline void Random_Engine_Pcg_Seed(Random_Engine_Pcg *state, uint64_t seed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Pcg) *state` | Pointer to aligned state storage managed by caller or engine. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |

---

### Return value

None.

---

### Remarks

64-bit state/stream PCG-family generator. **Next returns only 32 effective bits** extended to uint64_t; upper 32 bits are zero. Given the same seed, engine state is reproducible. Pass an appropriate engine state pointer; these raw operations do not validate NULL.

---

### Example

```c
RANDOM_ENGINE_TYPE(Pcg) state;
RANDOM_ENGINE_FUNC(Pcg, Seed)(&state, 42);
```

---

# Engine Pcg_Next

Returns the next Pcg pseudo-random output and advances its state.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENGINE_FUNC(Pcg, Next)(RANDOM_ENGINE_TYPE(Pcg) *state);
```

#### Direct form

```c
static inline uint64_t Random_Engine_Pcg_Next(Random_Engine_Pcg *state);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Pcg) *state` | Pointer to aligned state storage managed by caller or engine. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

64-bit state/stream PCG-family generator. **Next returns only 32 effective bits** extended to uint64_t; upper 32 bits are zero. Do not call Next on an unseeded state. An engine is deterministic, not a cryptographically secure generator.

---

### Example

```c
RANDOM_ENGINE_TYPE(Pcg) state;
RANDOM_ENGINE_FUNC(Pcg, Seed)(&state, 42);
uint64_t next = RANDOM_ENGINE_FUNC(Pcg, Next)(&state);
```

---

# Engine Romu_Seed

Seeds the Romu engine state.

### Syntax

#### Macro form

```c
static inline void RANDOM_ENGINE_FUNC(Romu, Seed)(RANDOM_ENGINE_TYPE(Romu) *state, uint64_t seed);
```

#### Direct form

```c
static inline void Random_Engine_Romu_Seed(Random_Engine_Romu *state, uint64_t seed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Romu) *state` | Pointer to aligned state storage managed by caller or engine. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |

---

### Return value

None.

---

### Remarks

Three 64-bit state words with multiplication and rotation. Given the same seed, engine state is reproducible. Pass an appropriate engine state pointer; these raw operations do not validate NULL.

---

### Example

```c
RANDOM_ENGINE_TYPE(Romu) state;
RANDOM_ENGINE_FUNC(Romu, Seed)(&state, 42);
```

---

# Engine Romu_Next

Returns the next Romu pseudo-random output and advances its state.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENGINE_FUNC(Romu, Next)(RANDOM_ENGINE_TYPE(Romu) *state);
```

#### Direct form

```c
static inline uint64_t Random_Engine_Romu_Next(Random_Engine_Romu *state);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Romu) *state` | Pointer to aligned state storage managed by caller or engine. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Three 64-bit state words with multiplication and rotation. Do not call Next on an unseeded state. An engine is deterministic, not a cryptographically secure generator.

---

### Example

```c
RANDOM_ENGINE_TYPE(Romu) state;
RANDOM_ENGINE_FUNC(Romu, Seed)(&state, 42);
uint64_t next = RANDOM_ENGINE_FUNC(Romu, Next)(&state);
```

---

# Engine Sfc_Seed

Seeds the Sfc engine state.

### Syntax

#### Macro form

```c
static inline void RANDOM_ENGINE_FUNC(Sfc, Seed)(RANDOM_ENGINE_TYPE(Sfc) *state, uint64_t seed);
```

#### Direct form

```c
static inline void Random_Engine_Sfc_Seed(Random_Engine_Sfc *state, uint64_t seed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Sfc) *state` | Pointer to aligned state storage managed by caller or engine. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |

---

### Return value

None.

---

### Remarks

Three evolving 64-bit words plus a counter. Given the same seed, engine state is reproducible. Pass an appropriate engine state pointer; these raw operations do not validate NULL.

---

### Example

```c
RANDOM_ENGINE_TYPE(Sfc) state;
RANDOM_ENGINE_FUNC(Sfc, Seed)(&state, 42);
```

---

# Engine Sfc_Next

Returns the next Sfc pseudo-random output and advances its state.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENGINE_FUNC(Sfc, Next)(RANDOM_ENGINE_TYPE(Sfc) *state);
```

#### Direct form

```c
static inline uint64_t Random_Engine_Sfc_Next(Random_Engine_Sfc *state);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Sfc) *state` | Pointer to aligned state storage managed by caller or engine. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Three evolving 64-bit words plus a counter. Do not call Next on an unseeded state. An engine is deterministic, not a cryptographically secure generator.

---

### Example

```c
RANDOM_ENGINE_TYPE(Sfc) state;
RANDOM_ENGINE_FUNC(Sfc, Seed)(&state, 42);
uint64_t next = RANDOM_ENGINE_FUNC(Sfc, Next)(&state);
```

---

# Engine Splitmix_Seed

Seeds the Splitmix engine state.

### Syntax

#### Macro form

```c
static inline void RANDOM_ENGINE_FUNC(Splitmix, Seed)(RANDOM_ENGINE_TYPE(Splitmix) *state, uint64_t seed);
```

#### Direct form

```c
static inline void Random_Engine_Splitmix_Seed(Random_Engine_Splitmix *state, uint64_t seed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Splitmix) *state` | Pointer to aligned state storage managed by caller or engine. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |

---

### Return value

None.

---

### Remarks

One-word SplitMix stepping and output mixing. Given the same seed, engine state is reproducible. Pass an appropriate engine state pointer; these raw operations do not validate NULL.

---

### Example

```c
RANDOM_ENGINE_TYPE(Splitmix) state;
RANDOM_ENGINE_FUNC(Splitmix, Seed)(&state, 42);
```

---

# Engine Splitmix_Next

Returns the next Splitmix pseudo-random output and advances its state.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENGINE_FUNC(Splitmix, Next)(RANDOM_ENGINE_TYPE(Splitmix) *state);
```

#### Direct form

```c
static inline uint64_t Random_Engine_Splitmix_Next(Random_Engine_Splitmix *state);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Splitmix) *state` | Pointer to aligned state storage managed by caller or engine. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

One-word SplitMix stepping and output mixing. Do not call Next on an unseeded state. An engine is deterministic, not a cryptographically secure generator.

---

### Example

```c
RANDOM_ENGINE_TYPE(Splitmix) state;
RANDOM_ENGINE_FUNC(Splitmix, Seed)(&state, 42);
uint64_t next = RANDOM_ENGINE_FUNC(Splitmix, Next)(&state);
```

---

# Engine WyRand_Seed

Seeds the WyRand engine state.

### Syntax

#### Macro form

```c
static inline void RANDOM_ENGINE_FUNC(WyRand, Seed)(RANDOM_ENGINE_TYPE(WyRand) *state, uint64_t seed);
```

#### Direct form

```c
static inline void Random_Engine_WyRand_Seed(Random_Engine_WyRand *state, uint64_t seed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(WyRand) *state` | Pointer to aligned state storage managed by caller or engine. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |

---

### Return value

None.

---

### Remarks

One-word increment and multiply-XOR; MSVC x64 uses _umul128, other targets use int128 or portable fallback. Given the same seed, engine state is reproducible. Pass an appropriate engine state pointer; these raw operations do not validate NULL.

---

### Example

```c
RANDOM_ENGINE_TYPE(WyRand) state;
RANDOM_ENGINE_FUNC(WyRand, Seed)(&state, 42);
```

---

# Engine WyRand_Next

Returns the next WyRand pseudo-random output and advances its state.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENGINE_FUNC(WyRand, Next)(RANDOM_ENGINE_TYPE(WyRand) *state);
```

#### Direct form

```c
static inline uint64_t Random_Engine_WyRand_Next(Random_Engine_WyRand *state);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(WyRand) *state` | Pointer to aligned state storage managed by caller or engine. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

One-word increment and multiply-XOR; MSVC x64 uses _umul128, other targets use int128 or portable fallback. Do not call Next on an unseeded state. An engine is deterministic, not a cryptographically secure generator.

---

### Example

```c
RANDOM_ENGINE_TYPE(WyRand) state;
RANDOM_ENGINE_FUNC(WyRand, Seed)(&state, 42);
uint64_t next = RANDOM_ENGINE_FUNC(WyRand, Next)(&state);
```

---

# Engine Xoroshiro_Seed

Seeds the Xoroshiro engine state.

### Syntax

#### Macro form

```c
static inline void RANDOM_ENGINE_FUNC(Xoroshiro, Seed)(RANDOM_ENGINE_TYPE(Xoroshiro) *state, uint64_t seed);
```

#### Direct form

```c
static inline void Random_Engine_Xoroshiro_Seed(Random_Engine_Xoroshiro *state, uint64_t seed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Xoroshiro) *state` | Pointer to aligned state storage managed by caller or engine. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |

---

### Return value

None.

---

### Remarks

Two-word xoroshiro-family sequence with a scrambled output. Given the same seed, engine state is reproducible. Pass an appropriate engine state pointer; these raw operations do not validate NULL.

---

### Example

```c
RANDOM_ENGINE_TYPE(Xoroshiro) state;
RANDOM_ENGINE_FUNC(Xoroshiro, Seed)(&state, 42);
```

---

# Engine Xoroshiro_Next

Returns the next Xoroshiro pseudo-random output and advances its state.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENGINE_FUNC(Xoroshiro, Next)(RANDOM_ENGINE_TYPE(Xoroshiro) *state);
```

#### Direct form

```c
static inline uint64_t Random_Engine_Xoroshiro_Next(Random_Engine_Xoroshiro *state);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Xoroshiro) *state` | Pointer to aligned state storage managed by caller or engine. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Two-word xoroshiro-family sequence with a scrambled output. Do not call Next on an unseeded state. An engine is deterministic, not a cryptographically secure generator.

---

### Example

```c
RANDOM_ENGINE_TYPE(Xoroshiro) state;
RANDOM_ENGINE_FUNC(Xoroshiro, Seed)(&state, 42);
uint64_t next = RANDOM_ENGINE_FUNC(Xoroshiro, Next)(&state);
```

---

# Engine Xoshiro_Seed

Seeds the Xoshiro engine state.

### Syntax

#### Macro form

```c
static inline void RANDOM_ENGINE_FUNC(Xoshiro, Seed)(RANDOM_ENGINE_TYPE(Xoshiro) *state, uint64_t seed);
```

#### Direct form

```c
static inline void Random_Engine_Xoshiro_Seed(Random_Engine_Xoshiro *state, uint64_t seed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Xoshiro) *state` | Pointer to aligned state storage managed by caller or engine. |
| `seed` | `uint64_t seed` | Unsigned 64-bit integer used to initialize deterministic engine state. |

---

### Return value

None.

---

### Remarks

Four-word xoshiro-family sequence with scrambled output. Given the same seed, engine state is reproducible. Pass an appropriate engine state pointer; these raw operations do not validate NULL.

---

### Example

```c
RANDOM_ENGINE_TYPE(Xoshiro) state;
RANDOM_ENGINE_FUNC(Xoshiro, Seed)(&state, 42);
```

---

# Engine Xoshiro_Next

Returns the next Xoshiro pseudo-random output and advances its state.

### Syntax

#### Macro form

```c
static inline uint64_t RANDOM_ENGINE_FUNC(Xoshiro, Next)(RANDOM_ENGINE_TYPE(Xoshiro) *state);
```

#### Direct form

```c
static inline uint64_t Random_Engine_Xoshiro_Next(Random_Engine_Xoshiro *state);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `state` | `RANDOM_ENGINE_TYPE(Xoshiro) *state` | Pointer to aligned state storage managed by caller or engine. |

---

### Return value

`uint64_t` or `int64_t` as declared above.

---

### Remarks

Four-word xoshiro-family sequence with scrambled output. Do not call Next on an unseeded state. An engine is deterministic, not a cryptographically secure generator.

---

### Example

```c
RANDOM_ENGINE_TYPE(Xoshiro) state;
RANDOM_ENGINE_FUNC(Xoshiro, Seed)(&state, 42);
uint64_t next = RANDOM_ENGINE_FUNC(Xoshiro, Next)(&state);
```

---

# Shuffle convenience macros

The Shuffle header also provides two argument-count/element-size helpers. These are **macro conveniences**, not additional functions:

```c
#define RANDOM_SHUFFLE_VECTOR(VECTOR, COUNT) \
  RANDOM_SHUFFLE_FUNC(Vector)((VECTOR), (COUNT), sizeof(*(VECTOR)))

#define RANDOM_SHUFFLE_VECTOR_FROM_SOURCE(VECTOR, COUNT, SOURCE) \
  RANDOM_SHUFFLE_FUNC(VectorFromSource)((VECTOR), (COUNT), \
                                       sizeof(*(VECTOR)), (SOURCE))
```

Usage:

```c
int items[] = {10, 20, 30, 40};
OPSTATUS status = RANDOM_SHUFFLE_VECTOR(items, 4);
/* For reproducible order:
   status = RANDOM_SHUFFLE_VECTOR_FROM_SOURCE(items, 4, &source); */
```

Provide a pointer to the first element or a C array, **not a Container TVector object**. The count is the number of elements, not the total byte size.

---

# Complete examples

## Reproducible six-sided dice

```c
#include <stdint.h>
#include "Cosmeron/Modules/Random/Random.h"

int main(void) {
    RANDOM_SOURCE_TYPE(Value) source = {0};
    OPSTATUS status = RANDOM_SOURCE_FUNC(Init)(
        &source, &RANDOM_ENGINE_FUNC(Xoshiro, Descriptor),
        UINT64_C(42), NULL);
    if (status != STATUS_CONST(SUCCESS)) return 1;

    uint64_t results[3];
    for (size_t i = 0; i < 3; ++i) {
        status = RANDOM_DISTRIBUTION_FUNC(U64)(
            &source, 1, 6, &results[i]);
        if (status != STATUS_CONST(SUCCESS)) break;
    }

    RANDOM_SOURCE_FUNC(Destroy)(&source);
    return status == STATUS_CONST(SUCCESS) ? 0 : 2;
}
```

## Caller-owned state, mixer and deterministic shuffle

```c
#include "Cosmeron/Modules/Random/Random.h"

int main(void) {
    RANDOM_ENGINE_TYPE(Xoshiro) state;
    RANDOM_SOURCE_TYPE(Value) source = {0};
    int items[] = {1, 2, 3, 4, 5};

    OPSTATUS status = RANDOM_SOURCE_FUNC(InitWithState)(
        &source, &RANDOM_ENGINE_FUNC(Xoshiro, Descriptor),
        &state, sizeof(state), UINT64_C(1234),
        RANDOM_MIXER_FUNC(Stafford));

    if (status == STATUS_CONST(SUCCESS))
        status = RANDOM_SHUFFLE_VECTOR_FROM_SOURCE(items, 5, &source);

    RANDOM_SOURCE_FUNC(Destroy)(&source);
    /* The caller still owns state, which was allocated on the stack. */
    return status == STATUS_CONST(SUCCESS) ? 0 : 1;
}
```

## System random bytes with explicit failure handling

```c
#include <stdint.h>
#include "Cosmeron/Modules/Random/Entropy/Entropy.h"

int main(void) {
    uint8_t buffer[32] = {0};
    OPSTATUS status = RANDOM_ENTROPY_FUNC(System)(buffer, sizeof(buffer));
    if (status != STATUS_CONST(SUCCESS)) {
        /* Do not use the buffer after partial/failed acquisition. */
        return 1;
    }
    /* buffer contains operating-system random bytes. */
    return 0;
}
```

## Entropy Pool (deterministic mixing, not OS randomness)

```c
#include "Cosmeron/Modules/Random/Entropy/Pool/Pool.h"

int main(void) {
    RANDOM_ENTROPY_TYPE(Pool) pool;
    uint64_t result;
    if (RANDOM_ENTROPY_FUNC(Pool_Init)(&pool) != STATUS_CONST(SUCCESS))
        return 1;
    if (RANDOM_ENTROPY_FUNC(Pool_Add)(&pool, 123) != STATUS_CONST(SUCCESS))
        return 2;
    if (RANDOM_ENTROPY_FUNC(Pool_Add)(&pool, 456) != STATUS_CONST(SUCCESS))
        return 3;
    return RANDOM_ENTROPY_FUNC(Pool_Finalize)(&pool, &result)
        == STATUS_CONST(SUCCESS) ? 0 : 4;
}
```

---

# Design and portability notes

### Reproducibility vs entropy

An explicit Source seed produces a repeatable sequence for a given engine implementation and mixer. Reinitializing a Source with an identical engine, mixer and seed returns it to its initial state. By comparison, the no-argument `Random_U64`, `Random_F64` and `Random_Bool` convenience calls use heuristic entropy collection each time, and `Random_U64_FromSeed` is only a *single first sample* from a newly seeded Xoshiro instance.

`Random_Entropy_Collect` combines values that might vary between invocations, but it neither calls the OS secure generator nor provides a statistical measurement of available entropy bits. `Random_Entropy_System` is the API to use for bytes sourced from the OS RNG; callers must handle its failure.

### Engine details and limits

The PCG engine returns a 32-bit permuted result converted to `uint64_t`. A Distribution_U64 call driven by this engine may have limited reach for wide output spans and should not be expected to behave like a full 64-bit uniform source. The mixer called `Splitmix64` delegates to Murmur3 in the current code; the `Stafford` mixer implements a separate SplitMix-style avalanche function.

`Source_InitWithState` requires correct alignment and sufficient state size; use the matching `RANDOM_ENGINE_TYPE(Engine)` struct where possible. `Source_Init` uses the library's memory allocator and currently validates engine alignment against its maximum supported alignment. Descriptors are read-only metadata; raw engine Seed/Next functions expect non-NULL state pointers and do not provide OPSTATUS validation.

### Platform support

On Windows, `Random_Entropy_System` dynamically resolves `BCryptGenRandom` from `bcrypt.dll`, avoiding an explicit BCrypt import-library link. On Linux it calls `getrandom`; macOS and FreeBSD use `/dev/urandom`. Other OS targets return `STATUS_CONST(NOT_AVAILABLE)`. Some engine/mixer implementations use platform-specific 128-bit arithmetic or portable fallback paths; algorithm details can affect cross-compiler reproducibility.

The Random module's test Makefile targets C11 with strict warnings, `-D_POSIX_C_SOURCE=200809L`, and empty default `LDLIBS`. To build and run the existing tests:

```sh
make -C Codespace/Tests/Random run
```

Tests include seeded determinism, all eight engines, mixers, Source ownership, valid/invalid Distribution arguments, shuffling, OS entropy requests, and Pool finalization.

---

# Notes

- This reference covers **52 public function signatures**, including 16 explicit engine Seed/Next functions, plus two Shuffle convenience macros and eight engine descriptors.
- All 52 API operations have macro and default direct spellings. `*_PROTOTYPE` and `RANDOM_ENGINE_DESCRIPTOR_INSTANCE` help generate definitions, but are not independent runtime operations.
- **No global `Random_Seed` method exists** in the current API; use `Source_Reseed` or `Engine_Seed`.
- Entropy mixers and deterministic PRNG algorithms do **not** grant cryptographic security to their input, regardless of identifier names.
- No source files are changed by this documentation.
