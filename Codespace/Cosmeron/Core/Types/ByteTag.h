#pragma once

#include "Impl/Common.inc"
#include "ByteTag.def"

/* Standard Width Instantiations */
TEMPLATE_INSTANTIATE_TABLE(Type, ByteTag, TYPE_BYTETAG_TYPE_TABLE);

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
