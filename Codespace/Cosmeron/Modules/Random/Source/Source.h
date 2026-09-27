#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../Engine/FunctionTable.h"
#include "../Entropy/Entropy.h"
#include "../Mixer/Mixer.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct RANDOM_SOURCE_TYPE(Value) {
  const RANDOM_ENGINE_TYPE(FunctionTable) *engine;
  RANDOM_MIXER_TYPE(Function) mixer;
  void *state;
  size_t stateSize;
  bool ownsState;
} RANDOM_SOURCE_TYPE(Value);

typedef RANDOM_SOURCE_TYPE(Value) RANDOM_SOURCE_TYPE(Value);

static inline OPSTATUS RANDOM_SOURCE_FUNC(Init)(
    RANDOM_SOURCE_TYPE(Value) *source, const RANDOM_ENGINE_TYPE(FunctionTable) *engine,
    uint64_t seed, RANDOM_MIXER_TYPE(Function) mixer);
static inline OPSTATUS RANDOM_SOURCE_FUNC(InitWithState)(
    RANDOM_SOURCE_TYPE(Value) *source, const RANDOM_ENGINE_TYPE(FunctionTable) *engine,
    void *state, size_t stateSize, uint64_t seed, RANDOM_MIXER_TYPE(Function) mixer);
static inline OPSTATUS RANDOM_SOURCE_FUNC(InitSystem)(
    RANDOM_SOURCE_TYPE(Value) *source, const RANDOM_ENGINE_TYPE(FunctionTable) *engine,
    RANDOM_MIXER_TYPE(Function) mixer);
static inline OPSTATUS RANDOM_SOURCE_FUNC(Reseed)(RANDOM_SOURCE_TYPE(Value) *source,
                                                  uint64_t seed);
static inline uint64_t RANDOM_SOURCE_FUNC(NextU64)(RANDOM_SOURCE_TYPE(Value) *source);
static inline void RANDOM_SOURCE_FUNC(Destroy)(RANDOM_SOURCE_TYPE(Value) *source);

#include "Impl/Source.impl"
/* EOF */
