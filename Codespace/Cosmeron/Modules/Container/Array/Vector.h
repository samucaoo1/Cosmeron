#pragma once

#include "Flat.space"

#define FLAT_VECTOR_TYPE(SUFFIX) GNS2(FLAT_NS(TVector), SUFFIX)
#define FLAT_VECTOR_FUNC(SUFFIX, FUNC) GNS2(GNS2(FLAT_NS(Vector), SUFFIX), FUNC)

/* ============================================================
 * Struct
 * ============================================================ */

#define FLAT_VECTOR_STRUCT(TYPE, SUFFIX)                                       \
  CONTAINER_API_FORWARD(FLAT_VECTOR_FUNC(SUFFIX, FunctionTable))                           \
  typedef struct FLAT_VECTOR_FUNC(SUFFIX, str) {                               \
    CONTAINER_API_FIELD(FLAT_VECTOR_FUNC(SUFFIX, FunctionTable))             \
    TYPE *data;                                                                \
    size_t size;                                                               \
    size_t capacity;                                                           \
  } FLAT_VECTOR_TYPE(SUFFIX);

/* ============================================================
 * Prototypes — Lifecycle
 * ============================================================ */

#define FLAT_VECTOR_INIT_PROTOTYPE(SUFFIX)                            \
  inline static OPSTATUS FLAT_VECTOR_FUNC(SUFFIX, Init)(                       \
      FLAT_VECTOR_TYPE(SUFFIX) * vec)

#define FLAT_VECTOR_DESTROY_PROTOTYPE(SUFFIX)                         \
  inline static void FLAT_VECTOR_FUNC(SUFFIX,                                  \
                                      Destroy)(FLAT_VECTOR_TYPE(SUFFIX) * vec)

/* ============================================================
 * Prototypes — Capacity
 * ============================================================ */

#define FLAT_VECTOR_RESERVE_PROTOTYPE(SUFFIX)                         \
  inline static OPSTATUS FLAT_VECTOR_FUNC(SUFFIX, Reserve)(                    \
      FLAT_VECTOR_TYPE(SUFFIX) * vec, size_t newCapacity)

#define FLAT_VECTOR_SHRINK_TO_FIT_PROTOTYPE(SUFFIX)                   \
  inline static OPSTATUS FLAT_VECTOR_FUNC(SUFFIX, ShrinkToFit)(                \
      FLAT_VECTOR_TYPE(SUFFIX) * vec)

/* ============================================================
 * Prototypes — Element Access
 * ============================================================ */

#define FLAT_VECTOR_AT_PROTOTYPE(TYPE, SUFFIX)                        \
  inline static TYPE *FLAT_VECTOR_FUNC(SUFFIX, At)(                            \
      FLAT_VECTOR_TYPE(SUFFIX) * vec, size_t index)

#define FLAT_VECTOR_FRONT_PROTOTYPE(TYPE, SUFFIX)                     \
  inline static TYPE *FLAT_VECTOR_FUNC(SUFFIX,                                 \
                                       Front)(FLAT_VECTOR_TYPE(SUFFIX) * vec)

#define FLAT_VECTOR_BACK_PROTOTYPE(TYPE, SUFFIX)                      \
  inline static TYPE *FLAT_VECTOR_FUNC(SUFFIX,                                 \
                                       Back)(FLAT_VECTOR_TYPE(SUFFIX) * vec)

#define FLAT_VECTOR_DATA_PROTOTYPE(TYPE, SUFFIX)                      \
  inline static TYPE *FLAT_VECTOR_FUNC(SUFFIX,                                 \
                                       Data)(FLAT_VECTOR_TYPE(SUFFIX) * vec)

/* ============================================================
 * Prototypes — Modifiers
 * ============================================================ */

#define FLAT_VECTOR_PUSH_BACK_PROTOTYPE(TYPE, SUFFIX)                 \
  inline static OPSTATUS FLAT_VECTOR_FUNC(SUFFIX, PushBack)(                   \
      FLAT_VECTOR_TYPE(SUFFIX) * vec, TYPE value)

#define FLAT_VECTOR_POP_BACK_PROTOTYPE(TYPE, SUFFIX)                  \
  inline static OPSTATUS FLAT_VECTOR_FUNC(SUFFIX, PopBack)(                    \
      FLAT_VECTOR_TYPE(SUFFIX) * vec, TYPE * outValue)

#define FLAT_VECTOR_INSERT_PROTOTYPE(TYPE, SUFFIX)                    \
  inline static OPSTATUS FLAT_VECTOR_FUNC(SUFFIX, Insert)(                     \
      FLAT_VECTOR_TYPE(SUFFIX) * vec, size_t position, TYPE value)

#define FLAT_VECTOR_ERASE_PROTOTYPE(SUFFIX)                           \
  inline static OPSTATUS FLAT_VECTOR_FUNC(SUFFIX, Erase)(                      \
      FLAT_VECTOR_TYPE(SUFFIX) * vec, size_t position, size_t count)

#define FLAT_VECTOR_CLEAR_PROTOTYPE(SUFFIX)                           \
  inline static void FLAT_VECTOR_FUNC(SUFFIX,                                  \
                                      Clear)(FLAT_VECTOR_TYPE(SUFFIX) * vec)

#define FLAT_VECTOR_PUSH_FRONT_PROTOTYPE(TYPE, SUFFIX)                \
  inline static OPSTATUS FLAT_VECTOR_FUNC(SUFFIX, PushFront)(                  \
      FLAT_VECTOR_TYPE(SUFFIX) * vec, TYPE value)

#define FLAT_VECTOR_POP_FRONT_PROTOTYPE(TYPE, SUFFIX)                 \
  inline static OPSTATUS FLAT_VECTOR_FUNC(SUFFIX, PopFront)(                   \
      FLAT_VECTOR_TYPE(SUFFIX) * vec, TYPE * outValue)

/* ============================================================
 * Prototypes — Observers
 * ============================================================ */

#define FLAT_VECTOR_EMPTY_PROTOTYPE(SUFFIX)                           \
  inline static bool FLAT_VECTOR_FUNC(SUFFIX,                                  \
                                      Empty)(const FLAT_VECTOR_TYPE(SUFFIX) * vec)

#define FLAT_VECTOR_SIZE_PROTOTYPE(SUFFIX)                            \
  inline static size_t FLAT_VECTOR_FUNC(SUFFIX,                                \
                                        Size)(const FLAT_VECTOR_TYPE(SUFFIX) * vec)

#define FLAT_VECTOR_CAPACITY_PROTOTYPE(SUFFIX)                        \
  inline static size_t FLAT_VECTOR_FUNC(SUFFIX, Capacity)(                     \
      const FLAT_VECTOR_TYPE(SUFFIX) * vec)

/* ============================================================
 * Complete declaration
 * ============================================================ */

#define FLAT_VECTOR_DECLARE(TYPE, SUFFIX)                                      \
  FLAT_VECTOR_STRUCT(TYPE, SUFFIX)                                             \
  FLAT_VECTOR_INIT_PROTOTYPE(SUFFIX);                                 \
  FLAT_VECTOR_DESTROY_PROTOTYPE(SUFFIX);                              \
  FLAT_VECTOR_RESERVE_PROTOTYPE(SUFFIX);                              \
  FLAT_VECTOR_SHRINK_TO_FIT_PROTOTYPE(SUFFIX);                        \
  FLAT_VECTOR_AT_PROTOTYPE(TYPE, SUFFIX);                             \
  FLAT_VECTOR_FRONT_PROTOTYPE(TYPE, SUFFIX);                          \
  FLAT_VECTOR_BACK_PROTOTYPE(TYPE, SUFFIX);                           \
  FLAT_VECTOR_DATA_PROTOTYPE(TYPE, SUFFIX);                           \
  FLAT_VECTOR_PUSH_BACK_PROTOTYPE(TYPE, SUFFIX);                      \
  FLAT_VECTOR_POP_BACK_PROTOTYPE(TYPE, SUFFIX);                       \
  FLAT_VECTOR_PUSH_FRONT_PROTOTYPE(TYPE, SUFFIX);                     \
  FLAT_VECTOR_POP_FRONT_PROTOTYPE(TYPE, SUFFIX);                      \
  FLAT_VECTOR_INSERT_PROTOTYPE(TYPE, SUFFIX);                         \
  FLAT_VECTOR_ERASE_PROTOTYPE(SUFFIX);                                \
  FLAT_VECTOR_CLEAR_PROTOTYPE(SUFFIX);                                \
  FLAT_VECTOR_EMPTY_PROTOTYPE(SUFFIX);                                \
  FLAT_VECTOR_SIZE_PROTOTYPE(SUFFIX);                                 \
  FLAT_VECTOR_CAPACITY_PROTOTYPE(SUFFIX);                             \
  FLAT_VECTOR_IMPLEMENT(TYPE, SUFFIX)                                          \
  FLAT_VECTOR_FUNCTION_TABLE(TYPE, SUFFIX)                                   \
  FLAT_VECTOR_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

#include "VTable/Vector.h"

#define TVECTOR_TYPE(SUFFIX) FLAT_VECTOR_TYPE(SUFFIX)
#define TVECTOR_FUNC(SUFFIX, FUNC) FLAT_VECTOR_FUNC(SUFFIX, FUNC)

#define FLAT_VECTOR_INSTANCE_DECLARE(TYPE, NAME)                                                    \
  FLAT_VECTOR_TYPE(TYPE) NAME;                                                 \
  CONTAINER_API_BIND(NAME, FLAT_VECTOR_FUNC(TYPE, functions));                               \
  FLAT_VECTOR_FUNC(TYPE, Init)(&NAME);
/* Compatibility alias: prefer FLAT_VECTOR_INSTANCE_DECLARE. */
#define TVector(TYPE, NAME) FLAT_VECTOR_INSTANCE_DECLARE(TYPE, NAME)

/* ============================================================
 * Implementation
 * ============================================================ */
#include "Impl/Vector.impl"

/* ============================================================
 * Instantiations
 * ============================================================ */
FLAT_VECTOR_DECLARE(int, int)
FLAT_VECTOR_DECLARE(float, float)
FLAT_VECTOR_DECLARE(double, double)
