#pragma once

#include "Impl/ByteTag.impl"
#include "Impl/Common.inc"
#include "ByteTag.def"

#define TYPE_BYTETAG_WIDTH_COL_SUFFIX(SUFFIX, TYPE, BYTES) SUFFIX
#define TYPE_BYTETAG_WIDTH_COL_TYPE(SUFFIX, TYPE, BYTES) TYPE
#define TYPE_BYTETAG_WIDTH_COL_BYTES(SUFFIX, TYPE, BYTES) BYTES
#define TYPE_BYTETAG_WIDTH_GET(FIELD, ROW)                                         \
  PP_OP_CAT2(TYPE_BYTETAG_WIDTH_COL_, FIELD) ROW

#define TYPE_BYTETAG_WIDTH_TABLE(X)                                                \
  X((16, uint16_t, 2))                                                         \
  X((32, uint32_t, 4))                                                         \
  X((64, uint64_t, 8))

/* Standard Width Instantiations */
#define COSMERON_MACRO_INTERNAL_TYPE_BYTETAG_INST(ROW)                             \
  TEMPLATE_IMPLEMENT(Type_ByteTag, TYPE_BYTETAG_WIDTH_GET(SUFFIX, ROW),                \
                     TYPE_BYTETAG_WIDTH_GET(TYPE, ROW));

TYPE_BYTETAG_WIDTH_TABLE(COSMERON_MACRO_INTERNAL_TYPE_BYTETAG_INST)
#undef COSMERON_MACRO_INTERNAL_TYPE_BYTETAG_INST

/* Compile-Time Tag Packing Helpers */
#define TYPE_BYTETAG_MAKE_16(a, b)                                                 \
  ((uint16_t)(((uint16_t)(uint8_t)(a) << 8U) | ((uint16_t)(uint8_t)(b))))

#define TYPE_BYTETAG_MAKE_32(a, b, c, d)                                           \
  ((uint32_t)(((uint32_t)(uint8_t)(a) << 24U) |                                \
              ((uint32_t)(uint8_t)(b) << 16U) |                                \
              ((uint32_t)(uint8_t)(c) << 8U) | ((uint32_t)(uint8_t)(d))))

#define TYPE_BYTETAG_MAKE_64(a, b, c, d, e, f, g, h)                               \
  ((uint64_t)(((uint64_t)(uint8_t)(a) << 56ULL) |                              \
              ((uint64_t)(uint8_t)(b) << 48ULL) |                              \
              ((uint64_t)(uint8_t)(c) << 40ULL) |                              \
              ((uint64_t)(uint8_t)(d) << 32ULL) |                              \
              ((uint64_t)(uint8_t)(e) << 24ULL) |                              \
              ((uint64_t)(uint8_t)(f) << 16ULL) |                              \
              ((uint64_t)(uint8_t)(g) << 8ULL) | ((uint64_t)(uint8_t)(h))))

/* EOF */
