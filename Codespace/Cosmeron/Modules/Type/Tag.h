#pragma once

#include "Common.inc"
#include "Tag.def"
#include "Impl/Tag.impl"

/* Standard Width Instantiations */
#define COSMERON_MACRO_INTERNAL_TYPE_TAG_INST(ROW)                             \
  TEMPLATE_IMPLEMENT(Type_Tag, TYPE_TAG_WIDTH_GET(SUFFIX, ROW),                \
                     TYPE_TAG_WIDTH_GET(TYPE, ROW));

TYPE_TAG_WIDTH_TABLE(COSMERON_MACRO_INTERNAL_TYPE_TAG_INST)
#undef COSMERON_MACRO_INTERNAL_TYPE_TAG_INST

/* Compile-Time Tag Packing Helpers */
#define TYPE_TAG_MAKE_16(a, b)                                                 \
  ((uint16_t)(((uint16_t)(uint8_t)(a) << 8U) | ((uint16_t)(uint8_t)(b))))

#define TYPE_TAG_MAKE_32(a, b, c, d)                                           \
  ((uint32_t)(((uint32_t)(uint8_t)(a) << 24U) |                                \
              ((uint32_t)(uint8_t)(b) << 16U) |                                \
              ((uint32_t)(uint8_t)(c) << 8U) | ((uint32_t)(uint8_t)(d))))

#define TYPE_TAG_MAKE_64(a, b, c, d, e, f, g, h)                               \
  ((uint64_t)(((uint64_t)(uint8_t)(a) << 56ULL) |                              \
              ((uint64_t)(uint8_t)(b) << 48ULL) |                              \
              ((uint64_t)(uint8_t)(c) << 40ULL) |                              \
              ((uint64_t)(uint8_t)(d) << 32ULL) |                              \
              ((uint64_t)(uint8_t)(e) << 24ULL) |                              \
              ((uint64_t)(uint8_t)(f) << 16ULL) |                              \
              ((uint64_t)(uint8_t)(g) << 8ULL) | ((uint64_t)(uint8_t)(h))))

/* EOF */
