#pragma once

#include "../Error/Status.h"
#include "../Preprocessor/Compiling.inc"
#include "Cast.space"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#define CAST_CONVERT_PROTOTYPE(NAME, FROM_TYPE, TO_TYPE)                       \
  static inline OPSTATUS CAST_FUNC(NAME)(TO_TYPE * outValue, FROM_TYPE inValue)

/* Unsigned downcasts (same signedness: Uint to width) */
CAST_CONVERT_PROTOTYPE(Uint64to32, uint64_t, uint32_t);
CAST_CONVERT_PROTOTYPE(Uint64to16, uint64_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Uint64to8, uint64_t, uint8_t);
CAST_CONVERT_PROTOTYPE(Uint32to16, uint32_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Uint32to8, uint32_t, uint8_t);
CAST_CONVERT_PROTOTYPE(Uint16to8, uint16_t, uint8_t);

/* Signed downcasts (same signedness: Int to width) */
CAST_CONVERT_PROTOTYPE(Int64to32, int64_t, int32_t);
CAST_CONVERT_PROTOTYPE(Int64to16, int64_t, int16_t);
CAST_CONVERT_PROTOTYPE(Int64to8, int64_t, int8_t);
CAST_CONVERT_PROTOTYPE(Int32to16, int32_t, int16_t);
CAST_CONVERT_PROTOTYPE(Int32to8, int32_t, int8_t);
CAST_CONVERT_PROTOTYPE(Int16to8, int16_t, int8_t);

/* Signed to unsigned conversions (Int to U+width) */
CAST_CONVERT_PROTOTYPE(Int64toU64, int64_t, uint64_t);
CAST_CONVERT_PROTOTYPE(Int64toU32, int64_t, uint32_t);
CAST_CONVERT_PROTOTYPE(Int64toU16, int64_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Int64toU8, int64_t, uint8_t);

CAST_CONVERT_PROTOTYPE(Int32toU64, int32_t, uint64_t);
CAST_CONVERT_PROTOTYPE(Int32toU32, int32_t, uint32_t);
CAST_CONVERT_PROTOTYPE(Int32toU16, int32_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Int32toU8, int32_t, uint8_t);

CAST_CONVERT_PROTOTYPE(Int16toU64, int16_t, uint64_t);
CAST_CONVERT_PROTOTYPE(Int16toU32, int16_t, uint32_t);
CAST_CONVERT_PROTOTYPE(Int16toU16, int16_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Int16toU8, int16_t, uint8_t);

CAST_CONVERT_PROTOTYPE(Int8toU64, int8_t, uint64_t);
CAST_CONVERT_PROTOTYPE(Int8toU32, int8_t, uint32_t);
CAST_CONVERT_PROTOTYPE(Int8toU16, int8_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Int8toU8, int8_t, uint8_t);

/* Unsigned to signed conversions (Uint to I+width) */
CAST_CONVERT_PROTOTYPE(Uint64toI64, uint64_t, int64_t);
CAST_CONVERT_PROTOTYPE(Uint64toI32, uint64_t, int32_t);
CAST_CONVERT_PROTOTYPE(Uint64toI16, uint64_t, int16_t);
CAST_CONVERT_PROTOTYPE(Uint64toI8, uint64_t, int8_t);

CAST_CONVERT_PROTOTYPE(Uint32toI64, uint32_t, int64_t);
CAST_CONVERT_PROTOTYPE(Uint32toI32, uint32_t, int32_t);
CAST_CONVERT_PROTOTYPE(Uint32toI16, uint32_t, int16_t);
CAST_CONVERT_PROTOTYPE(Uint32toI8, uint32_t, int8_t);

CAST_CONVERT_PROTOTYPE(Uint16toI64, uint16_t, int64_t);
CAST_CONVERT_PROTOTYPE(Uint16toI32, uint16_t, int32_t);
CAST_CONVERT_PROTOTYPE(Uint16toI16, uint16_t, int16_t);
CAST_CONVERT_PROTOTYPE(Uint16toI8, uint16_t, int8_t);

CAST_CONVERT_PROTOTYPE(Uint8toI64, uint8_t, int64_t);
CAST_CONVERT_PROTOTYPE(Uint8toI32, uint8_t, int32_t);
CAST_CONVERT_PROTOTYPE(Uint8toI16, uint8_t, int16_t);
CAST_CONVERT_PROTOTYPE(Uint8toI8, uint8_t, int8_t);

/* Size conversions */
CAST_CONVERT_PROTOTYPE(SizetoU64, size_t, uint64_t);
CAST_CONVERT_PROTOTYPE(SizetoU32, size_t, uint32_t);
CAST_CONVERT_PROTOTYPE(SizetoU16, size_t, uint16_t);
CAST_CONVERT_PROTOTYPE(SizetoU8, size_t, uint8_t);
CAST_CONVERT_PROTOTYPE(SizetoI64, size_t, int64_t);
CAST_CONVERT_PROTOTYPE(SizetoI32, size_t, int32_t);
CAST_CONVERT_PROTOTYPE(Uint64toSize, uint64_t, size_t);
CAST_CONVERT_PROTOTYPE(Int64toSize, int64_t, size_t);
CAST_CONVERT_PROTOTYPE(Int32toSize, int32_t, size_t);

#include "Impl/Integer.impl"
/* EOF */
