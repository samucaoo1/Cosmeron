#pragma once

#include "Flat.space"

/* ============================================================
 * TFlat_Stack
 * ============================================================ */

#define FLAT_STACK_TYPE(SUFFIX) GNS2(FLAT_NS(TStack), SUFFIX)
#define FLAT_STACK_FUNC(SUFFIX, FUNC) GNS2(GNS2(FLAT_NS(Stack), SUFFIX), FUNC)

#define FLAT_STACK_STRUCT(TYPE, SUFFIX)                                        \
  CONTAINER_API_FORWARD(FLAT_STACK_FUNC(SUFFIX, FunctionTable))                            \
  typedef struct FLAT_STACK_FUNC(SUFFIX, str) {                                \
    CONTAINER_API_FIELD(FLAT_STACK_FUNC(SUFFIX, FunctionTable))              \
    TYPE *data;                                                                \
    size_t size;                                                               \
    size_t capacity;                                                           \
  } FLAT_STACK_TYPE(SUFFIX);

/* ============================================================
 * Prototypes
 * ============================================================ */

#define FLAT_STACK_INIT_PROTOTYPE(SUFFIX)                             \
  inline static OPSTATUS FLAT_STACK_FUNC(SUFFIX, Init)(                        \
      FLAT_STACK_TYPE(SUFFIX) * stack)

#define FLAT_STACK_DESTROY_PROTOTYPE(SUFFIX)                          \
  inline static void FLAT_STACK_FUNC(SUFFIX,                                   \
                                     Destroy)(FLAT_STACK_TYPE(SUFFIX) * stack)

#define FLAT_STACK_RESERVE_PROTOTYPE(SUFFIX)                          \
  inline static OPSTATUS FLAT_STACK_FUNC(SUFFIX, Reserve)(                     \
      FLAT_STACK_TYPE(SUFFIX) * stack, size_t newCapacity)

#define FLAT_STACK_SHRINK_TO_FIT_PROTOTYPE(SUFFIX)                    \
  inline static OPSTATUS FLAT_STACK_FUNC(SUFFIX, ShrinkToFit)(                 \
      FLAT_STACK_TYPE(SUFFIX) * stack)

#define FLAT_STACK_PUSH_PROTOTYPE(TYPE, SUFFIX)                       \
  inline static OPSTATUS FLAT_STACK_FUNC(SUFFIX, Push)(                        \
      FLAT_STACK_TYPE(SUFFIX) * stack, TYPE value)

#define FLAT_STACK_POP_PROTOTYPE(TYPE, SUFFIX)                        \
  inline static OPSTATUS FLAT_STACK_FUNC(SUFFIX, Pop)(                         \
      FLAT_STACK_TYPE(SUFFIX) * stack, TYPE * outValue)

#define FLAT_STACK_TOP_PROTOTYPE(TYPE, SUFFIX)                        \
  inline static TYPE *FLAT_STACK_FUNC(SUFFIX,                                  \
                                      Top)(FLAT_STACK_TYPE(SUFFIX) * stack)

#define FLAT_STACK_EMPTY_PROTOTYPE(SUFFIX)                            \
  inline static bool FLAT_STACK_FUNC(SUFFIX,                                   \
                                     Empty)(const FLAT_STACK_TYPE(SUFFIX) * stack)

#define FLAT_STACK_SIZE_PROTOTYPE(SUFFIX)                             \
  inline static size_t FLAT_STACK_FUNC(SUFFIX,                                 \
                                       Size)(const FLAT_STACK_TYPE(SUFFIX) * stack)

#define FLAT_STACK_CAPACITY_PROTOTYPE(SUFFIX)                         \
  inline static size_t FLAT_STACK_FUNC(SUFFIX, Capacity)(                      \
      const FLAT_STACK_TYPE(SUFFIX) * stack)

#include "VTable/Stack.h"

#define FLAT_STACK_DECLARE(TYPE, SUFFIX)                                       \
  FLAT_STACK_STRUCT(TYPE, SUFFIX)                                              \
  FLAT_STACK_INIT_PROTOTYPE(SUFFIX);                                  \
  FLAT_STACK_DESTROY_PROTOTYPE(SUFFIX);                               \
  FLAT_STACK_RESERVE_PROTOTYPE(SUFFIX);                               \
  FLAT_STACK_SHRINK_TO_FIT_PROTOTYPE(SUFFIX);                         \
  FLAT_STACK_PUSH_PROTOTYPE(TYPE, SUFFIX);                            \
  FLAT_STACK_POP_PROTOTYPE(TYPE, SUFFIX);                             \
  FLAT_STACK_TOP_PROTOTYPE(TYPE, SUFFIX);                             \
  FLAT_STACK_EMPTY_PROTOTYPE(SUFFIX);                                 \
  FLAT_STACK_SIZE_PROTOTYPE(SUFFIX);                                  \
  FLAT_STACK_CAPACITY_PROTOTYPE(SUFFIX);                              \
  FLAT_STACK_IMPLEMENT(TYPE, SUFFIX)                                           \
  FLAT_STACK_FUNCTION_TABLE(TYPE, SUFFIX)                                    \
  FLAT_STACK_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

#include "Impl/Stack.impl"

#define FLAT_STACK_INSTANCE_DECLARE(TYPE, NAME)                                                \
  FLAT_STACK_TYPE(TYPE) NAME;                                                  \
  CONTAINER_API_BIND(NAME, FLAT_STACK_FUNC(TYPE, functions));                                \
  FLAT_STACK_FUNC(TYPE, Init)(&NAME);
/* Compatibility alias: prefer FLAT_STACK_INSTANCE_DECLARE. */
#define TFlat_Stack(TYPE, NAME) FLAT_STACK_INSTANCE_DECLARE(TYPE, NAME)

FLAT_STACK_DECLARE(int, int)
FLAT_STACK_DECLARE(float, float)
FLAT_STACK_DECLARE(double, double)
