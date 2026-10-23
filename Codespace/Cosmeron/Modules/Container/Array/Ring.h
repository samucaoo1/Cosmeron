#pragma once

#include "Flat.space"

/* ============================================================
 * Flat_Ring (Bounded Circular Ring FIFO Buffer)
 * ============================================================ */

#define FLAT_RING_TYPE(SUFFIX) GNS2(FLAT_NS(TRing), SUFFIX)
#define FLAT_RING_NS(SUFFIX) GNS2(FLAT_NS(Ring), SUFFIX)
#define FLAT_RING_FUNC(SUFFIX, FUNC) GNS2(FLAT_RING_NS(SUFFIX), FUNC)
#define FLAT_RING_STRUCT_TAG(SUFFIX) GNS2(FLAT_RING_NS(SUFFIX), str)
#define FLAT_RING_FUNCTION_TABLE_TYPE(SUFFIX)                                  \
  GNS2(FLAT_RING_NS(SUFFIX), FunctionTable)

#define FLAT_RING_STRUCT(TYPE, SUFFIX)                                         \
  CONTAINER_API_FORWARD(FLAT_RING_FUNCTION_TABLE_TYPE(SUFFIX))                 \
  typedef struct FLAT_RING_STRUCT_TAG(SUFFIX) {                                \
    CONTAINER_API_FIELD(FLAT_RING_FUNCTION_TABLE_TYPE(SUFFIX))                 \
    TYPE *data;                                                                \
    size_t capacity;                                                           \
    size_t head;                                                               \
    size_t tail;                                                               \
    size_t size;                                                               \
  } FLAT_RING_TYPE(SUFFIX);

/* ============================================================
 * Prototypes
 * ============================================================ */

#define FLAT_RING_INIT_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, Init)(                         \
      FLAT_RING_TYPE(SUFFIX) * ring, TYPE * buffer, size_t capacity)

#define FLAT_RING_DESTROY_PROTOTYPE(SUFFIX)                                    \
  static inline void FLAT_RING_FUNC(SUFFIX, Destroy)(                          \
      FLAT_RING_TYPE(SUFFIX) * ring)

#define FLAT_RING_CLEAR_PROTOTYPE(SUFFIX)                                      \
  static inline void FLAT_RING_FUNC(SUFFIX, Clear)(                            \
      FLAT_RING_TYPE(SUFFIX) * ring)

#define FLAT_RING_PUSH_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, Push)(                         \
      FLAT_RING_TYPE(SUFFIX) * ring, TYPE value)

#define FLAT_RING_POP_PROTOTYPE(TYPE, SUFFIX)                                  \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, Pop)(                          \
      FLAT_RING_TYPE(SUFFIX) * ring, TYPE * outValue)

#define FLAT_RING_PEEK_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, Peek)(                         \
      const FLAT_RING_TYPE(SUFFIX) * ring, TYPE * outValue)

#define FLAT_RING_OVERWRITE_PROTOTYPE(TYPE, SUFFIX)                            \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, Overwrite)(                     \
      FLAT_RING_TYPE(SUFFIX) * ring, TYPE value, TYPE * outDropped)

#define FLAT_RING_FRONT_PROTOTYPE(TYPE, SUFFIX)                                \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, Front)(                         \
      FLAT_RING_TYPE(SUFFIX) * ring, TYPE **out)

#define FLAT_RING_BACK_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, Back)(                          \
      FLAT_RING_TYPE(SUFFIX) * ring, TYPE **out)

#define FLAT_RING_AT_PROTOTYPE(TYPE, SUFFIX)                                   \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, At)(                           \
      FLAT_RING_TYPE(SUFFIX) * ring, size_t index, TYPE **out)

#define FLAT_RING_EMPTY_PROTOTYPE(SUFFIX)                                      \
  static inline bool FLAT_RING_FUNC(SUFFIX, Empty)(                            \
      const FLAT_RING_TYPE(SUFFIX) * ring)

#define FLAT_RING_FULL_PROTOTYPE(SUFFIX)                                       \
  static inline bool FLAT_RING_FUNC(SUFFIX, Full)(                             \
      const FLAT_RING_TYPE(SUFFIX) * ring)

#define FLAT_RING_SIZE_PROTOTYPE(SUFFIX)                                       \
  static inline size_t FLAT_RING_FUNC(SUFFIX, Size)(                           \
      const FLAT_RING_TYPE(SUFFIX) * ring)

#define FLAT_RING_CAPACITY_PROTOTYPE(SUFFIX)                                   \
  static inline size_t FLAT_RING_FUNC(SUFFIX, Capacity)(                       \
      const FLAT_RING_TYPE(SUFFIX) * ring)

#define FLAT_RING_AVAILABLE_PROTOTYPE(SUFFIX)                                  \
  static inline size_t FLAT_RING_FUNC(SUFFIX, Available)(                      \
      const FLAT_RING_TYPE(SUFFIX) * ring)

#define FLAT_RING_WRITE_PROTOTYPE(TYPE, SUFFIX)                                \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, Write)(                        \
      FLAT_RING_TYPE(SUFFIX) * ring, const TYPE * items, size_t count,         \
      size_t * outWritten)

#define FLAT_RING_READ_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, Read)(                         \
      FLAT_RING_TYPE(SUFFIX) * ring, TYPE * dest, size_t count,                \
      size_t * outRead)

#define FLAT_RING_GET_READ_SPAN_PROTOTYPE(TYPE, SUFFIX)                        \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, GetReadSpan)(                  \
      FLAT_RING_TYPE(SUFFIX) * ring, TYPE **outPtr, size_t * outCount)

#define FLAT_RING_ADVANCE_READ_PROTOTYPE(SUFFIX)                               \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, AdvanceRead)(                  \
      FLAT_RING_TYPE(SUFFIX) * ring, size_t count)

#define FLAT_RING_GET_WRITE_SPAN_PROTOTYPE(TYPE, SUFFIX)                       \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, GetWriteSpan)(                 \
      FLAT_RING_TYPE(SUFFIX) * ring, TYPE **outPtr, size_t * outCount)

#define FLAT_RING_ADVANCE_WRITE_PROTOTYPE(SUFFIX)                              \
  static inline OPSTATUS FLAT_RING_FUNC(SUFFIX, AdvanceWrite)(                 \
      FLAT_RING_TYPE(SUFFIX) * ring, size_t count)

#include "FunctionTable/Ring.h"

#define FLAT_RING_IMPLEMENT_ALL(TYPE, SUFFIX)                                  \
  FLAT_RING_STRUCT(TYPE, SUFFIX)                                               \
  FLAT_RING_INIT_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_RING_DESTROY_PROTOTYPE(SUFFIX);                                         \
  FLAT_RING_CLEAR_PROTOTYPE(SUFFIX);                                           \
  FLAT_RING_PUSH_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_RING_POP_PROTOTYPE(TYPE, SUFFIX);                                       \
  FLAT_RING_PEEK_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_RING_OVERWRITE_PROTOTYPE(TYPE, SUFFIX);                                 \
  FLAT_RING_FRONT_PROTOTYPE(TYPE, SUFFIX);                                     \
  FLAT_RING_BACK_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_RING_AT_PROTOTYPE(TYPE, SUFFIX);                                        \
  FLAT_RING_EMPTY_PROTOTYPE(SUFFIX);                                           \
  FLAT_RING_FULL_PROTOTYPE(SUFFIX);                                            \
  FLAT_RING_SIZE_PROTOTYPE(SUFFIX);                                            \
  FLAT_RING_CAPACITY_PROTOTYPE(SUFFIX);                                        \
  FLAT_RING_AVAILABLE_PROTOTYPE(SUFFIX);                                       \
  FLAT_RING_WRITE_PROTOTYPE(TYPE, SUFFIX);                                     \
  FLAT_RING_READ_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_RING_GET_READ_SPAN_PROTOTYPE(TYPE, SUFFIX);                             \
  FLAT_RING_ADVANCE_READ_PROTOTYPE(SUFFIX);                                    \
  FLAT_RING_GET_WRITE_SPAN_PROTOTYPE(TYPE, SUFFIX);                            \
  FLAT_RING_ADVANCE_WRITE_PROTOTYPE(SUFFIX);                                   \
  COSMERON_MACRO_INTERNAL_FLAT_RING_IMPLEMENT(TYPE, SUFFIX)                    \
  FLAT_RING_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                                \
  FLAT_RING_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

#include "Impl/Ring.impl"

#define FLAT_RING_INSTANCE_DECLARE(TYPE, NAME, BUFFER, CAPACITY)               \
  FLAT_RING_TYPE(TYPE) NAME;                                                   \
  CONTAINER_API_BIND(NAME, FLAT_RING_FUNC(TYPE, functions));                   \
  FLAT_RING_FUNC(TYPE, Init)(&NAME, (BUFFER), (CAPACITY));

/* Compatibility alias: prefer FLAT_RING_INSTANCE_DECLARE. */
#define TFlat_Ring(TYPE, NAME, BUFFER, CAPACITY)                               \
  FLAT_RING_INSTANCE_DECLARE(TYPE, NAME, BUFFER, CAPACITY)

FLAT_RING_IMPLEMENT_ALL(int, int)
FLAT_RING_IMPLEMENT_ALL(float, float)
FLAT_RING_IMPLEMENT_ALL(double, double)
FLAT_RING_IMPLEMENT_ALL(uint8_t, byte)
FLAT_RING_IMPLEMENT_ALL(uint8_t, uint8)

typedef FLAT_RING_TYPE(byte) TBoundedBuffer;
typedef FLAT_RING_TYPE(byte) TRingBuffer;
