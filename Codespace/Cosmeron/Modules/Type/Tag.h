#pragma once

#include "../../Core/Algorithm/Comparison.h"
#include "../../Core/Error/Status.h"
#include "../../Core/Preprocessor/Compiling.inc"

#include "Type.space"

#define TAG_TYPE(SUFFIX) TYPE_TYPE(TTag, SUFFIX)
#define TAG_FUNC(SUFFIX, FUNCTION) TYPE_FUNC(Tag, SUFFIX, FUNCTION)
#define TAG_FUNCTION_TABLE_TYPE(SUFFIX) GNS2(TAG_TYPE(SUFFIX), FunctionTable)

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TAG_FUNCTION_TABLE_FORWARD(SUFFIX)                                     \
  struct TAG_FUNCTION_TABLE_TYPE(SUFFIX);
#define TAG_API_MEMBER(SUFFIX)                                                 \
  const struct TAG_FUNCTION_TABLE_TYPE(SUFFIX) *api;
#else
#define TAG_FUNCTION_TABLE_FORWARD(SUFFIX)
#define TAG_API_MEMBER(SUFFIX)
#endif

#define TAG_STRUCT(SUFFIX, UINT_TYPE, BYTE_COUNT)                              \
  TAG_FUNCTION_TABLE_FORWARD(SUFFIX)                                           \
  typedef struct {                                                             \
    TAG_API_MEMBER(SUFFIX)                                                     \
    UINT_TYPE value;                                                           \
    char text[(BYTE_COUNT) + 1U];                                              \
  } TAG_TYPE(SUFFIX);

#define TAG_INIT_PROTOTYPE(SUFFIX)                                             \
  static inline OPSTATUS TAG_FUNC(SUFFIX, Init)(                               \
      TAG_TYPE(SUFFIX) *tag, const char *chars)

#define TAG_FROM_UINT_PROTOTYPE(SUFFIX, UINT_TYPE)                            \
  static inline OPSTATUS TAG_FUNC(SUFFIX, FromUInt)(                           \
      TAG_TYPE(SUFFIX) *tag, UINT_TYPE value)

#define TAG_FROM_BYTES_PROTOTYPE(SUFFIX)                                       \
  static inline OPSTATUS TAG_FUNC(SUFFIX, FromBytes)(                          \
      TAG_TYPE(SUFFIX) *tag, const void *bytes, size_t length)

#define TAG_READ_BE_PROTOTYPE(SUFFIX)                                          \
  static inline OPSTATUS TAG_FUNC(SUFFIX, ReadBE)(                             \
      TAG_TYPE(SUFFIX) *tag, const void *src)

#define TAG_READ_LE_PROTOTYPE(SUFFIX)                                          \
  static inline OPSTATUS TAG_FUNC(SUFFIX, ReadLE)(                             \
      TAG_TYPE(SUFFIX) *tag, const void *src)

#define TAG_WRITE_BE_PROTOTYPE(SUFFIX)                                         \
  static inline OPSTATUS TAG_FUNC(SUFFIX, WriteBE)(                            \
      const TAG_TYPE(SUFFIX) *tag, void *dest)

#define TAG_WRITE_LE_PROTOTYPE(SUFFIX)                                         \
  static inline OPSTATUS TAG_FUNC(SUFFIX, WriteLE)(                            \
      const TAG_TYPE(SUFFIX) *tag, void *dest)

#define TAG_TO_STRING_PROTOTYPE(SUFFIX)                                        \
  static inline OPSTATUS TAG_FUNC(SUFFIX, ToString)(                           \
      const TAG_TYPE(SUFFIX) *tag, char *dest, size_t destCapacity)

#define TAG_TEXT_PROTOTYPE(SUFFIX)                                             \
  static inline const char *TAG_FUNC(SUFFIX, Text)(                            \
      const TAG_TYPE(SUFFIX) *tag)

#define TAG_TO_UINT_PROTOTYPE(SUFFIX, UINT_TYPE)                               \
  static inline UINT_TYPE TAG_FUNC(SUFFIX, ToUInt)(                            \
      const TAG_TYPE(SUFFIX) *tag)

#define TAG_IS_PRINTABLE_PROTOTYPE(SUFFIX)                                     \
  static inline bool TAG_FUNC(SUFFIX, IsPrintable)(                            \
      const TAG_TYPE(SUFFIX) *tag)

#define TAG_EQUAL_PROTOTYPE(SUFFIX)                                            \
  static inline bool TAG_FUNC(SUFFIX, Equal)(                                  \
      const TAG_TYPE(SUFFIX) *tagA, const TAG_TYPE(SUFFIX) *tagB)

#define TAG_COMPARE_PROTOTYPE(SUFFIX)                                          \
  static inline OPSTATUS TAG_FUNC(SUFFIX, Compare)(                            \
      const TAG_TYPE(SUFFIX) *tagA, const TAG_TYPE(SUFFIX) *tagB,              \
      CMPOUT *result)

#define TAG_DECLARE_PROTOTYPES(SUFFIX, UINT_TYPE)                              \
  TAG_INIT_PROTOTYPE(SUFFIX);                                                  \
  TAG_FROM_UINT_PROTOTYPE(SUFFIX, UINT_TYPE);                                  \
  TAG_FROM_BYTES_PROTOTYPE(SUFFIX);                                            \
  TAG_READ_BE_PROTOTYPE(SUFFIX);                                               \
  TAG_READ_LE_PROTOTYPE(SUFFIX);                                               \
  TAG_WRITE_BE_PROTOTYPE(SUFFIX);                                              \
  TAG_WRITE_LE_PROTOTYPE(SUFFIX);                                              \
  TAG_TO_STRING_PROTOTYPE(SUFFIX);                                             \
  TAG_TEXT_PROTOTYPE(SUFFIX);                                                  \
  TAG_TO_UINT_PROTOTYPE(SUFFIX, UINT_TYPE);                                    \
  TAG_IS_PRINTABLE_PROTOTYPE(SUFFIX);                                          \
  TAG_EQUAL_PROTOTYPE(SUFFIX);                                                 \
  TAG_COMPARE_PROTOTYPE(SUFFIX);

#include "Impl/Tag.impl"
#ifndef TYPE_DISABLE_FUNCTION_TABLE
#include "FunctionTable/Tag.h"
#endif

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TYPE_FUNCTION_TABLE_DECLARE_TAG(SUFFIX, UINT_TYPE)                     \
  TAG_FUNCTION_TABLE_STRUCT(SUFFIX, UINT_TYPE)                                 \
  TAG_FUNCTION_TABLE_INSTANCE(SUFFIX, UINT_TYPE)
#else
#define TYPE_FUNCTION_TABLE_DECLARE_TAG(SUFFIX, UINT_TYPE)
#endif

#define TAG_DECLARE(SUFFIX, UINT_TYPE, BYTE_COUNT)                             \
  TAG_STRUCT(SUFFIX, UINT_TYPE, BYTE_COUNT)                                    \
  TAG_DECLARE_PROTOTYPES(SUFFIX, UINT_TYPE)                                    \
  TAG_IMPLEMENT(SUFFIX, UINT_TYPE, BYTE_COUNT)                                 \
  TYPE_FUNCTION_TABLE_DECLARE_TAG(SUFFIX, UINT_TYPE)

#ifndef TYPE_DISABLE_FUNCTION_TABLE
#define TTag(SUFFIX, NAME)                                                     \
  TAG_TYPE(SUFFIX) NAME = {0};                                                 \
  NAME.api = &TAG_FUNC(SUFFIX, functions);
#else
#define TTag(SUFFIX, NAME) TAG_TYPE(SUFFIX) NAME = {0};
#endif

#define TTag16(NAME) TTag(16, NAME)
#define TTag32(NAME) TTag(32, NAME)
#define TTag64(NAME) TTag(64, NAME)

#define TAG16_MAKE(a, b)                                                       \
  (((uint16_t)(uint8_t)(a) << 8U) |                                            \
   ((uint16_t)(uint8_t)(b)))

#define TAG32_MAKE(a, b, c, d)                                                 \
  (((uint32_t)(uint8_t)(a) << 24U) |                                           \
   ((uint32_t)(uint8_t)(b) << 16U) |                                           \
   ((uint32_t)(uint8_t)(c) << 8U) |                                            \
   ((uint32_t)(uint8_t)(d)))

#define TAG64_MAKE(a, b, c, d, e, f, g, h)                                     \
  (((uint64_t)(uint8_t)(a) << 56ULL) |                                         \
   ((uint64_t)(uint8_t)(b) << 48ULL) |                                         \
   ((uint64_t)(uint8_t)(c) << 40ULL) |                                         \
   ((uint64_t)(uint8_t)(d) << 32ULL) |                                         \
   ((uint64_t)(uint8_t)(e) << 24ULL) |                                         \
   ((uint64_t)(uint8_t)(f) << 16ULL) |                                         \
   ((uint64_t)(uint8_t)(g) << 8ULL) |                                          \
   ((uint64_t)(uint8_t)(h)))

#define TAG_MAKE(a, b, c, d) TAG32_MAKE(a, b, c, d)

TAG_DECLARE(16, uint16_t, 2U)
TAG_DECLARE(32, uint32_t, 4U)
TAG_DECLARE(64, uint64_t, 8U)

typedef TAG_TYPE(32) TYPE_NS(TTag);
/* EOF */
