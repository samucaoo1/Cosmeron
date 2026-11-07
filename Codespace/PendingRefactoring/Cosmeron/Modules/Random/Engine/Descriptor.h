#pragma once

#include "Engine.space"

#define RANDOM_ENGINE_DESCRIPTOR_STRUCT                                      \
  typedef struct RANDOM_ENGINE_TYPE(Descriptor) {                             \
    size_t stateSize;                                                         \
    size_t stateAlignment;                                                    \
    uint64_t (*nextU64)(void *state);                                         \
    void (*seed)(void *state, uint64_t seed);                                 \
  } RANDOM_ENGINE_TYPE(Descriptor)

RANDOM_ENGINE_DESCRIPTOR_STRUCT;

#if defined(__GNUC__) || defined(__clang__)
#define COSMERON_MACRO_INTERNAL_RANDOM_DESCRIPTOR_UNUSED __attribute__((unused))
#else
#define COSMERON_MACRO_INTERNAL_RANDOM_DESCRIPTOR_UNUSED
#endif

#define RANDOM_ENGINE_DESCRIPTOR_INSTANCE(ENGINE)                             \
  static inline uint64_t RANDOM_ENGINE_INS(ENGINE, Next)(void *state) {       \
    return RANDOM_ENGINE_FUNC(ENGINE, Next)(                                  \
        (RANDOM_ENGINE_TYPE(ENGINE) *)state);                                 \
  }                                                                           \
  static inline void RANDOM_ENGINE_INS(ENGINE, Seed)(                         \
      void *state, uint64_t seed) {                                           \
    RANDOM_ENGINE_FUNC(ENGINE, Seed)((RANDOM_ENGINE_TYPE(ENGINE) *)state,     \
                                     seed);                                   \
  }                                                                           \
  static const RANDOM_ENGINE_TYPE(Descriptor)                                 \
      RANDOM_ENGINE_FUNC(ENGINE, Descriptor)                                  \
          COSMERON_MACRO_INTERNAL_RANDOM_DESCRIPTOR_UNUSED = {                \
              sizeof(RANDOM_ENGINE_TYPE(ENGINE)),                             \
              _Alignof(RANDOM_ENGINE_TYPE(ENGINE)),                           \
              RANDOM_ENGINE_INS(ENGINE, Next),                                \
              RANDOM_ENGINE_INS(ENGINE, Seed)}
