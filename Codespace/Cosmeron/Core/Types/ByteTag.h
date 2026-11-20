#pragma once

#include "Impl/Common.inc"


#include "ByteTag.def"

/* Standard Width Instantiations */
TEMPLATE_INSTANTIATE_TABLE(Type, ByteTag, TYPE_BYTETAG_TYPE_TABLE);

/* Compile-Time Tag Packing Helpers */
#define TYPE_BYTETAG_MAKE_16(a, b)                                             \
  ((uint16_t)(((uint16_t)(uint8_t)(a) << CNS(Bit, Internal, SHIFT_8)) |        \
              ((uint16_t)(uint8_t)(b))))

#define TYPE_BYTETAG_MAKE_32(a, b, c, d)                                       \
  ((uint32_t)(((uint32_t)(uint8_t)(a) << CNS(Bit, Internal, SHIFT_24)) |       \
              ((uint32_t)(uint8_t)(b) << CNS(Bit, Internal, SHIFT_16)) |       \
              ((uint32_t)(uint8_t)(c) << CNS(Bit, Internal, SHIFT_8)) |        \
              ((uint32_t)(uint8_t)(d))))

#define TYPE_BYTETAG_MAKE_64(a, b, c, d, e, f, g, h)                           \
  ((uint64_t)(((uint64_t)(uint8_t)(a) << CNS(Bit, Internal, SHIFT_56)) |       \
              ((uint64_t)(uint8_t)(b) << 48ULL) |                              \
              ((uint64_t)(uint8_t)(c) << CNS(Bit, Internal, SHIFT_40)) |       \
              ((uint64_t)(uint8_t)(d) << CNS(Bit, Internal, SHIFT_32)) |       \
              ((uint64_t)(uint8_t)(e) << CNS(Bit, Internal, SHIFT_24)) |       \
              ((uint64_t)(uint8_t)(f) << CNS(Bit, Internal, SHIFT_16)) |       \
              ((uint64_t)(uint8_t)(g) << CNS(Bit, Internal, SHIFT_8)) |        \
              ((uint64_t)(uint8_t)(h))))

/* EOF */
