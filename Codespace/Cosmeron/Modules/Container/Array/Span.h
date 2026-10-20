#pragma once

#include "Flat.space"
#include "Vector.h"

/* ============================================================
 * Flat_Span (Non-owning contiguous buffer / slice)
 * ============================================================ */

#define FLAT_SPAN_TYPE(SUFFIX) GNS2(FLAT_NS(TSpan), SUFFIX)
#define FLAT_SPAN_NS(SUFFIX) GNS2(FLAT_NS(Span), SUFFIX)
#define FLAT_SPAN_FUNC(SUFFIX, FUNC) GNS2(FLAT_SPAN_NS(SUFFIX), FUNC)
#define FLAT_SPAN_STRUCT_TAG(SUFFIX) GNS2(FLAT_SPAN_NS(SUFFIX), str)
#define FLAT_SPAN_FUNCTION_TABLE_TYPE(SUFFIX)                                  \
  GNS2(FLAT_SPAN_NS(SUFFIX), FunctionTable)

/* ============================================================
 * Struct
 * ============================================================ */

#define FLAT_SPAN_STRUCT(TYPE, SUFFIX)                                         \
  CONTAINER_API_FORWARD(FLAT_SPAN_FUNCTION_TABLE_TYPE(SUFFIX))                 \
  typedef struct FLAT_SPAN_STRUCT_TAG(SUFFIX) {                                \
    CONTAINER_API_FIELD(FLAT_SPAN_FUNCTION_TABLE_TYPE(SUFFIX))                 \
    TYPE *data;                                                                \
    size_t size;                                                               \
  } FLAT_SPAN_TYPE(SUFFIX);

/* ============================================================
 * Prototypes — Lifecycle & Creation
 * ============================================================ */

#define FLAT_SPAN_INIT_PROTOTYPE(SUFFIX)                                       \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, Init)(                         \
      FLAT_SPAN_TYPE(SUFFIX) * span)

#define FLAT_SPAN_FROM_BUFFER_PROTOTYPE(TYPE, SUFFIX)                          \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, FromBuffer)(                    \
      FLAT_SPAN_TYPE(SUFFIX) * span, TYPE * buffer, size_t count)

#define FLAT_SPAN_CREATE_PROTOTYPE(TYPE, SUFFIX)                               \
  static inline FLAT_SPAN_TYPE(SUFFIX) FLAT_SPAN_FUNC(SUFFIX, Create)(         \
      TYPE * buffer, size_t count)

#define FLAT_SPAN_EMPTY_PROTOTYPE(SUFFIX)                                      \
  static inline FLAT_SPAN_TYPE(SUFFIX) FLAT_SPAN_FUNC(SUFFIX, Empty)(void)

/* ============================================================
 * Prototypes — Element Access
 * ============================================================ */

#define FLAT_SPAN_AT_PROTOTYPE(TYPE, SUFFIX)                                   \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, At)(                           \
      FLAT_SPAN_TYPE(SUFFIX) * span, size_t index, TYPE **out)

#define FLAT_SPAN_CONST_AT_PROTOTYPE(TYPE, SUFFIX)                             \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, ConstAt)(                      \
      const FLAT_SPAN_TYPE(SUFFIX) * span, size_t index, const TYPE **out)

#define FLAT_SPAN_FRONT_PROTOTYPE(TYPE, SUFFIX)                                \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, Front)(                        \
      FLAT_SPAN_TYPE(SUFFIX) * span, TYPE **out)

#define FLAT_SPAN_BACK_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, Back)(                         \
      FLAT_SPAN_TYPE(SUFFIX) * span, TYPE **out)

#define FLAT_SPAN_DATA_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, Data)(                         \
      FLAT_SPAN_TYPE(SUFFIX) * span, TYPE **outData)

#define FLAT_SPAN_CONST_DATA_PROTOTYPE(TYPE, SUFFIX)                           \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, ConstData)(                    \
      const FLAT_SPAN_TYPE(SUFFIX) * span, const TYPE **outData)

/* ============================================================
 * Prototypes — Observers
 * ============================================================ */

#define FLAT_SPAN_SIZE_PROTOTYPE(SUFFIX)                                       \
  static inline size_t FLAT_SPAN_FUNC(SUFFIX, Size)(                           \
      const FLAT_SPAN_TYPE(SUFFIX) * span)

#define FLAT_SPAN_SIZE_BYTES_PROTOTYPE(TYPE, SUFFIX)                           \
  static inline size_t FLAT_SPAN_FUNC(SUFFIX, SizeBytes)(                      \
      const FLAT_SPAN_TYPE(SUFFIX) * span)

#define FLAT_SPAN_IS_EMPTY_PROTOTYPE(SUFFIX)                                   \
  static inline bool FLAT_SPAN_FUNC(SUFFIX, IsEmpty)(                          \
      const FLAT_SPAN_TYPE(SUFFIX) * span)

/* ============================================================
 * Prototypes — Slicing
 * ============================================================ */

#define FLAT_SPAN_SUBSPAN_PROTOTYPE(SUFFIX)                                    \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, Subspan)(                      \
      const FLAT_SPAN_TYPE(SUFFIX) * span, size_t offset, size_t count,        \
      FLAT_SPAN_TYPE(SUFFIX) * outSpan)

#define FLAT_SPAN_FIRST_PROTOTYPE(SUFFIX)                                      \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, First)(                        \
      const FLAT_SPAN_TYPE(SUFFIX) * span, size_t count,                       \
      FLAT_SPAN_TYPE(SUFFIX) * outSpan)

#define FLAT_SPAN_LAST_PROTOTYPE(SUFFIX)                                       \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, Last)(                         \
      const FLAT_SPAN_TYPE(SUFFIX) * span, size_t count,                       \
      FLAT_SPAN_TYPE(SUFFIX) * outSpan)

/* ============================================================
 * Prototypes — Comparison & Interoperability
 * ============================================================ */

#define FLAT_SPAN_EQUAL_PROTOTYPE(TYPE, SUFFIX)                                \
  static inline bool FLAT_SPAN_FUNC(SUFFIX, Equal)(                            \
      const FLAT_SPAN_TYPE(SUFFIX) * left,                                     \
      const FLAT_SPAN_TYPE(SUFFIX) * right)

#define FLAT_SPAN_FROM_VECTOR_PROTOTYPE(SUFFIX)                                \
  static inline OPSTATUS FLAT_SPAN_FUNC(SUFFIX, FromVector)(                    \
      FLAT_SPAN_TYPE(SUFFIX) * span,                                           \
      const FLAT_VECTOR_TYPE(SUFFIX) * vector)

/* ============================================================
 * Function Table & Implementation Includes
 * ============================================================ */

#include "FunctionTable/Span.h"
#include "Impl/Span.impl"

/* ============================================================
 * Complete Generator
 * ============================================================ */

#define FLAT_SPAN_IMPLEMENT_ALL(TYPE, SUFFIX)                                  \
  FLAT_SPAN_STRUCT(TYPE, SUFFIX)                                               \
  FLAT_SPAN_INIT_PROTOTYPE(SUFFIX);                                            \
  FLAT_SPAN_FROM_BUFFER_PROTOTYPE(TYPE, SUFFIX);                               \
  FLAT_SPAN_CREATE_PROTOTYPE(TYPE, SUFFIX);                                    \
  FLAT_SPAN_EMPTY_PROTOTYPE(SUFFIX);                                           \
  FLAT_SPAN_AT_PROTOTYPE(TYPE, SUFFIX);                                        \
  FLAT_SPAN_CONST_AT_PROTOTYPE(TYPE, SUFFIX);                                  \
  FLAT_SPAN_FRONT_PROTOTYPE(TYPE, SUFFIX);                                     \
  FLAT_SPAN_BACK_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_SPAN_DATA_PROTOTYPE(TYPE, SUFFIX);                                      \
  FLAT_SPAN_CONST_DATA_PROTOTYPE(TYPE, SUFFIX);                                \
  FLAT_SPAN_SIZE_PROTOTYPE(SUFFIX);                                            \
  FLAT_SPAN_SIZE_BYTES_PROTOTYPE(TYPE, SUFFIX);                                \
  FLAT_SPAN_IS_EMPTY_PROTOTYPE(SUFFIX);                                        \
  FLAT_SPAN_SUBSPAN_PROTOTYPE(SUFFIX);                                         \
  FLAT_SPAN_FIRST_PROTOTYPE(SUFFIX);                                           \
  FLAT_SPAN_LAST_PROTOTYPE(SUFFIX);                                            \
  FLAT_SPAN_EQUAL_PROTOTYPE(TYPE, SUFFIX);                                     \
  FLAT_SPAN_FROM_VECTOR_PROTOTYPE(SUFFIX);                                     \
  COSMERON_MACRO_INTERNAL_FLAT_SPAN_IMPLEMENT(TYPE, SUFFIX)                     \
  typedef FLAT_SPAN_TYPE(SUFFIX) GNS2(TSpan, SUFFIX);                          \
  FLAT_SPAN_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                                \
  FLAT_SPAN_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

#define TSPAN_TYPE(SUFFIX) FLAT_SPAN_TYPE(SUFFIX)
#define TSPAN_FUNC(SUFFIX, FUNC) FLAT_SPAN_FUNC(SUFFIX, FUNC)

#define FLAT_SPAN_INSTANCE_DECLARE(TYPE, NAME)                                 \
  FLAT_SPAN_TYPE(TYPE) NAME;                                                   \
  CONTAINER_API_BIND(NAME, FLAT_SPAN_FUNC(TYPE, functions));                   \
  FLAT_SPAN_FUNC(TYPE, Init)(&NAME);

/* Compatibility alias: prefer FLAT_SPAN_INSTANCE_DECLARE. */
#define TFlat_Span(TYPE, NAME) FLAT_SPAN_INSTANCE_DECLARE(TYPE, NAME)
#define TSpan(TYPE, NAME) FLAT_SPAN_INSTANCE_DECLARE(TYPE, NAME)

/* ============================================================
 * Instantiations
 * ============================================================ */

FLAT_SPAN_IMPLEMENT_ALL(int, int)
FLAT_SPAN_IMPLEMENT_ALL(float, float)
FLAT_SPAN_IMPLEMENT_ALL(double, double)
