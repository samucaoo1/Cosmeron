#pragma once

#include "Engine.space"

typedef struct RANDOM_ENGINE_VTABLE {
  size_t stateSize;
  size_t stateAlignment;
  uint64_t (*nextU64)(void *state);
  void (*seed)(void *state, uint64_t seed);
} RANDOM_ENGINE_VTABLE;

#if defined(__GNUC__) || defined(__clang__)
#define RANDOM_ENGINE_VTABLE_UNUSED __attribute__((unused))
#else
#define RANDOM_ENGINE_VTABLE_UNUSED
#endif

#define RANDOM_ENGINE_VTABLE_DEFINE(ENGINE)                                    \
  inline static uint64_t RANDOM_ENGINE_FUNC(ENGINE, VTableNext)(void *state) { \
    return RANDOM_ENGINE_FUNC(ENGINE, Next)(                                   \
        (RANDOM_ENGINE_TYPE(ENGINE) *)state);                                  \
  }                                                                            \
  inline static void RANDOM_ENGINE_FUNC(ENGINE, VTableSeed)(                   \
      void *state, uint64_t seed) {                                            \
    RANDOM_ENGINE_FUNC(ENGINE, Seed)((RANDOM_ENGINE_TYPE(ENGINE) *)state, seed);\
  }                                                                            \
  static const RANDOM_ENGINE_VTABLE RANDOM_ENGINE_FUNC(ENGINE, VTable)         \
      RANDOM_ENGINE_VTABLE_UNUSED = {                                          \
          sizeof(RANDOM_ENGINE_TYPE(ENGINE)),                                  \
          _Alignof(RANDOM_ENGINE_TYPE(ENGINE)),                                \
          RANDOM_ENGINE_FUNC(ENGINE, VTableNext),                              \
          RANDOM_ENGINE_FUNC(ENGINE, VTableSeed)}
