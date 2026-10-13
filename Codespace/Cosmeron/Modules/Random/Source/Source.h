#pragma once

#include "../../../Core/Error/Status.h"
#include "../../../Core/Memory/Alloc.h"
#include "../Engine/Descriptor.h"
#include "../Entropy/Entropy.h"
#include "../Mixer/Mixer.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct RANDOM_SOURCE_TYPE(Value) {
  const RANDOM_ENGINE_TYPE(Descriptor) *engine;
  RANDOM_MIXER_TYPE(Function) mixer;
  void *state;
  size_t stateSize;
  bool ownsState;
} RANDOM_SOURCE_TYPE(Value);


#define RANDOM_SOURCE_INIT_PROTOTYPE                                        \
  static inline OPSTATUS RANDOM_SOURCE_FUNC(Init)(                            \
      RANDOM_SOURCE_TYPE(Value) *source,                                      \
      const RANDOM_ENGINE_TYPE(Descriptor) *engine, uint64_t seed,         \
      RANDOM_MIXER_TYPE(Function) mixer)

#define RANDOM_SOURCE_INIT_WITH_STATE_PROTOTYPE                             \
  static inline OPSTATUS RANDOM_SOURCE_FUNC(InitWithState)(                  \
      RANDOM_SOURCE_TYPE(Value) *source,                                      \
      const RANDOM_ENGINE_TYPE(Descriptor) *engine, void *state,          \
      size_t stateSize, uint64_t seed, RANDOM_MIXER_TYPE(Function) mixer)

#define RANDOM_SOURCE_INIT_SYSTEM_PROTOTYPE                                 \
  static inline OPSTATUS RANDOM_SOURCE_FUNC(InitSystem)(                     \
      RANDOM_SOURCE_TYPE(Value) *source,                                      \
      const RANDOM_ENGINE_TYPE(Descriptor) *engine,                       \
      RANDOM_MIXER_TYPE(Function) mixer)

#define RANDOM_SOURCE_RESEED_PROTOTYPE                                      \
  static inline OPSTATUS RANDOM_SOURCE_FUNC(Reseed)(                         \
      RANDOM_SOURCE_TYPE(Value) *source, uint64_t seed)

#define RANDOM_SOURCE_NEXT_U64_PROTOTYPE                                    \
  static inline OPSTATUS RANDOM_SOURCE_FUNC(NextU64)(                         \
      RANDOM_SOURCE_TYPE(Value) *source, uint64_t *out)

#define RANDOM_SOURCE_DESTROY_PROTOTYPE                                     \
  static inline void RANDOM_SOURCE_FUNC(Destroy)(                            \
      RANDOM_SOURCE_TYPE(Value) *source)

RANDOM_SOURCE_INIT_PROTOTYPE;
RANDOM_SOURCE_INIT_WITH_STATE_PROTOTYPE;
RANDOM_SOURCE_INIT_SYSTEM_PROTOTYPE;
RANDOM_SOURCE_RESEED_PROTOTYPE;
RANDOM_SOURCE_NEXT_U64_PROTOTYPE;
RANDOM_SOURCE_DESTROY_PROTOTYPE;

#include "Impl/Source.impl"
/* EOF */
