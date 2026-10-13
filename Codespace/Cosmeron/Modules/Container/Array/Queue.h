#pragma once

#include "Flat.space"

/* ============================================================
 * Flat_Queue
 * ============================================================ */

#define FLAT_QUEUE_TYPE(SUFFIX) GNS2(FLAT_NS(TQueue), SUFFIX)

#define FLAT_QUEUE_NS(SUFFIX) GNS2(FLAT_NS(Queue), SUFFIX)
#define FLAT_QUEUE_FUNC(SUFFIX, FUNC) GNS2(FLAT_QUEUE_NS(SUFFIX), FUNC)
#define FLAT_QUEUE_STRUCT_TAG(SUFFIX) GNS2(FLAT_QUEUE_NS(SUFFIX), str)
#define FLAT_QUEUE_FUNCTION_TABLE_TYPE(SUFFIX) \
  GNS2(FLAT_QUEUE_NS(SUFFIX), FunctionTable)

#define FLAT_QUEUE_STRUCT(TYPE, SUFFIX)                                        \
  CONTAINER_API_FORWARD(FLAT_QUEUE_FUNCTION_TABLE_TYPE(SUFFIX))                            \
  typedef struct FLAT_QUEUE_STRUCT_TAG(SUFFIX) {                                \
    CONTAINER_API_FIELD(FLAT_QUEUE_FUNCTION_TABLE_TYPE(SUFFIX))              \
    TYPE *data;                                                                \
    size_t size;                                                               \
    size_t capacity;                                                           \
    size_t head;                                                               \
  } FLAT_QUEUE_TYPE(SUFFIX);

/* ============================================================
 * Prototypes
 * ============================================================ */

#define FLAT_QUEUE_INIT_PROTOTYPE(SUFFIX)                             \
  static inline OPSTATUS FLAT_QUEUE_FUNC(SUFFIX, Init)(                        \
      FLAT_QUEUE_TYPE(SUFFIX) * queue)

#define FLAT_QUEUE_DESTROY_PROTOTYPE(SUFFIX)                          \
  static inline void FLAT_QUEUE_FUNC(SUFFIX,                                   \
                                     Destroy)(FLAT_QUEUE_TYPE(SUFFIX) * queue)

#define FLAT_QUEUE_RESERVE_PROTOTYPE(SUFFIX)                          \
  static inline OPSTATUS FLAT_QUEUE_FUNC(SUFFIX, Reserve)(                     \
      FLAT_QUEUE_TYPE(SUFFIX) * queue, size_t newCapacity)

#define FLAT_QUEUE_SHRINK_TO_FIT_PROTOTYPE(SUFFIX)                    \
  static inline OPSTATUS FLAT_QUEUE_FUNC(SUFFIX, ShrinkToFit)(                 \
      FLAT_QUEUE_TYPE(SUFFIX) * queue)

#define FLAT_QUEUE_COMPACT_PROTOTYPE(SUFFIX)                          \
  static inline OPSTATUS FLAT_QUEUE_FUNC(SUFFIX, Compact)(                     \
      FLAT_QUEUE_TYPE(SUFFIX) * queue)

#define FLAT_QUEUE_PUSH_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS FLAT_QUEUE_FUNC(SUFFIX, Push)(                        \
      FLAT_QUEUE_TYPE(SUFFIX) * queue, TYPE value)

#define FLAT_QUEUE_POP_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS FLAT_QUEUE_FUNC(SUFFIX, Pop)(                         \
      FLAT_QUEUE_TYPE(SUFFIX) * queue, TYPE * outValue)

#define FLAT_QUEUE_FRONT_PROTOTYPE(TYPE, SUFFIX)                      \
  static inline OPSTATUS FLAT_QUEUE_FUNC(SUFFIX, Front)(                       \
      FLAT_QUEUE_TYPE(SUFFIX) * queue, TYPE **out)

#define FLAT_QUEUE_BACK_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS FLAT_QUEUE_FUNC(SUFFIX, Back)(                        \
      FLAT_QUEUE_TYPE(SUFFIX) * queue, TYPE **out)

#define FLAT_QUEUE_EMPTY_PROTOTYPE(SUFFIX)                            \
  static inline bool FLAT_QUEUE_FUNC(SUFFIX,                                   \
                                     Empty)(const FLAT_QUEUE_TYPE(SUFFIX) * queue)
#define FLAT_QUEUE_SIZE_PROTOTYPE(SUFFIX)                             \
  static inline size_t FLAT_QUEUE_FUNC(SUFFIX,                                 \
                                       Size)(const FLAT_QUEUE_TYPE(SUFFIX) * queue)

#define FLAT_QUEUE_CAPACITY_PROTOTYPE(SUFFIX)                         \
  static inline size_t FLAT_QUEUE_FUNC(SUFFIX, Capacity)(                      \
      const FLAT_QUEUE_TYPE(SUFFIX) * queue)

#include "FunctionTable/Queue.h"

#define FLAT_QUEUE_IMPLEMENT_ALL(TYPE, SUFFIX)                                       \
  FLAT_QUEUE_STRUCT(TYPE, SUFFIX)                                              \
  FLAT_QUEUE_INIT_PROTOTYPE(SUFFIX);                                  \
  FLAT_QUEUE_DESTROY_PROTOTYPE(SUFFIX);                               \
  FLAT_QUEUE_RESERVE_PROTOTYPE(SUFFIX);                               \
  FLAT_QUEUE_SHRINK_TO_FIT_PROTOTYPE(SUFFIX);                         \
  FLAT_QUEUE_COMPACT_PROTOTYPE(SUFFIX);                               \
  FLAT_QUEUE_PUSH_PROTOTYPE(TYPE, SUFFIX);                            \
  FLAT_QUEUE_POP_PROTOTYPE(TYPE, SUFFIX);                             \
  FLAT_QUEUE_FRONT_PROTOTYPE(TYPE, SUFFIX);                           \
  FLAT_QUEUE_BACK_PROTOTYPE(TYPE, SUFFIX);                            \
  FLAT_QUEUE_EMPTY_PROTOTYPE(SUFFIX);                                 \
  FLAT_QUEUE_SIZE_PROTOTYPE(SUFFIX);                                  \
  FLAT_QUEUE_CAPACITY_PROTOTYPE(SUFFIX);                              \
  COSMERON_MACRO_INTERNAL_FLAT_QUEUE_IMPLEMENT(TYPE, SUFFIX)                                           \
  FLAT_QUEUE_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                                    \
  FLAT_QUEUE_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

#include "Impl/Queue.impl"

#define FLAT_QUEUE_INSTANCE_DECLARE(TYPE, NAME)                                                \
  FLAT_QUEUE_TYPE(TYPE) NAME;                                                  \
  CONTAINER_API_BIND(NAME, FLAT_QUEUE_FUNC(TYPE, functions));                                \
  FLAT_QUEUE_FUNC(TYPE, Init)(&NAME);
/* Compatibility alias: prefer FLAT_QUEUE_INSTANCE_DECLARE. */
#define TFlat_Queue(TYPE, NAME) FLAT_QUEUE_INSTANCE_DECLARE(TYPE, NAME)

FLAT_QUEUE_IMPLEMENT_ALL(int, int)
FLAT_QUEUE_IMPLEMENT_ALL(float, float)
FLAT_QUEUE_IMPLEMENT_ALL(double, double)
