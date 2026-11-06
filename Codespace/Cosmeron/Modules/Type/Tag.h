#pragma once

#include "Type.space"

#define TAG_TYPE(SUFFIX) TYPE_TYPE(TTag, SUFFIX)
#define TAG_FUNC(SUFFIX, FUNCTION) GNS2(TAG_TYPE(SUFFIX), FUNCTION)

#define TEMPLATE_TTAG(SUFFIX, UINT_TYPE)                                       \
  OBJECT(TAG_TYPE(SUFFIX),                                                     \
      FIELDS(                                                                  \
          UINT_TYPE value;                                                     \
          char text[((SUFFIX) / 8U) + 1U];                                     \
      ),                                                                       \
      OPERATION(FromString, (const char *chars)),                              \
      OPERATION(FromUInt, (UINT_TYPE value)),                                  \
      OPERATION(FromBytes, (const void *bytes, size_t length)),                \
      OPERATION(ReadBE, (const void *src)),                                    \
      OPERATION(ReadLE, (const void *src)),                                    \
      CONST_OPERATION(WriteBE, (void *dest)),                                  \
      CONST_OPERATION(WriteLE, (void *dest)),                                  \
      CONST_OPERATION(ToString, (char *dest, size_t destCapacity)),             \
      CONST_FUNCTION(const char *, Text, ()),                                  \
      CONST_FUNCTION(UINT_TYPE, ToUInt, ()),                                    \
      CONST_PREDICATE(IsPrintable, ()),                                        \
      CONST_PREDICATE(Equal, (const TAG_TYPE(SUFFIX) *other)),                 \
      CONST_OPERATION(Compare, (const TAG_TYPE(SUFFIX) *other, CMPOUT *result)) \
  )

#include "Impl/Tag.impl"

TAG_INSTANTIATE_WIDTHS(TTAG)

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

typedef TAG_TYPE(32) TYPE_NS(TTag);

/* EOF */
