#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../Engine/VTable.h"
#include "../Entropy/Entropy.h"
#include "../Mixer/Mixer.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct RANDOM_SOURCE_TYPE(Value) {
  const RANDOM_ENGINE_VTABLE *engine;
  RANDOM_MIXER_FUNCTION mixer;
  void *state;
  size_t stateSize;
  bool ownsState;
} RANDOM_SOURCE_TYPE(Value);

typedef RANDOM_SOURCE_TYPE(Value) TRandomSource;

inline static OPSTATUS RANDOM_SOURCE_FUNC(Init)(
    TRandomSource *source, const RANDOM_ENGINE_VTABLE *engine,
    uint64_t seed, RANDOM_MIXER_FUNCTION mixer);
inline static OPSTATUS RANDOM_SOURCE_FUNC(InitWithState)(
    TRandomSource *source, const RANDOM_ENGINE_VTABLE *engine,
    void *state, size_t stateSize, uint64_t seed, RANDOM_MIXER_FUNCTION mixer);
inline static OPSTATUS RANDOM_SOURCE_FUNC(InitSystem)(
    TRandomSource *source, const RANDOM_ENGINE_VTABLE *engine,
    RANDOM_MIXER_FUNCTION mixer);
inline static OPSTATUS RANDOM_SOURCE_FUNC(Reseed)(TRandomSource *source,
                                                  uint64_t seed);
inline static uint64_t RANDOM_SOURCE_FUNC(NextU64)(TRandomSource *source);
inline static void RANDOM_SOURCE_FUNC(Destroy)(TRandomSource *source);

#include "Impl/Source.impl"
/* EOF */
