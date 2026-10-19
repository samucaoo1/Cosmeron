#pragma once

#include "../Error/Status.h"
#include "../Preprocessor/Compiling.inc"
#include "Cast.space"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#define CAST_CONVERT_PROTOTYPE(FROM_NAME, TO_NAME, FROM_TYPE, TO_TYPE)         \
  static inline OPSTATUS CAST_FUNC(PP_OP_CAT3(FROM_NAME, To, TO_NAME))(        \
      TO_TYPE * outValue, FROM_TYPE inValue)

/* Unsigned downcasts */
CAST_CONVERT_PROTOTYPE(UInt64, UInt32, uint64_t, uint32_t);
CAST_CONVERT_PROTOTYPE(UInt64, UInt16, uint64_t, uint16_t);
CAST_CONVERT_PROTOTYPE(UInt64, UInt8, uint64_t, uint8_t);
CAST_CONVERT_PROTOTYPE(UInt32, UInt16, uint32_t, uint16_t);
CAST_CONVERT_PROTOTYPE(UInt32, UInt8, uint32_t, uint8_t);
CAST_CONVERT_PROTOTYPE(UInt16, UInt8, uint16_t, uint8_t);

/* Signed downcasts */
CAST_CONVERT_PROTOTYPE(Int64, Int32, int64_t, int32_t);
CAST_CONVERT_PROTOTYPE(Int64, Int16, int64_t, int16_t);
CAST_CONVERT_PROTOTYPE(Int64, Int8, int64_t, int8_t);
CAST_CONVERT_PROTOTYPE(Int32, Int16, int32_t, int16_t);
CAST_CONVERT_PROTOTYPE(Int32, Int8, int32_t, int8_t);
CAST_CONVERT_PROTOTYPE(Int16, Int8, int16_t, int8_t);

/* Signed to unsigned conversions */
CAST_CONVERT_PROTOTYPE(Int64, UInt64, int64_t, uint64_t);
CAST_CONVERT_PROTOTYPE(Int64, UInt32, int64_t, uint32_t);
CAST_CONVERT_PROTOTYPE(Int64, UInt16, int64_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Int64, UInt8, int64_t, uint8_t);

CAST_CONVERT_PROTOTYPE(Int32, UInt64, int32_t, uint64_t);
CAST_CONVERT_PROTOTYPE(Int32, UInt32, int32_t, uint32_t);
CAST_CONVERT_PROTOTYPE(Int32, UInt16, int32_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Int32, UInt8, int32_t, uint8_t);

CAST_CONVERT_PROTOTYPE(Int16, UInt64, int16_t, uint64_t);
CAST_CONVERT_PROTOTYPE(Int16, UInt32, int16_t, uint32_t);
CAST_CONVERT_PROTOTYPE(Int16, UInt16, int16_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Int16, UInt8, int16_t, uint8_t);

CAST_CONVERT_PROTOTYPE(Int8, UInt64, int8_t, uint64_t);
CAST_CONVERT_PROTOTYPE(Int8, UInt32, int8_t, uint32_t);
CAST_CONVERT_PROTOTYPE(Int8, UInt16, int8_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Int8, UInt8, int8_t, uint8_t);

/* Unsigned to signed conversions */
CAST_CONVERT_PROTOTYPE(UInt64, Int64, uint64_t, int64_t);
CAST_CONVERT_PROTOTYPE(UInt64, Int32, uint64_t, int32_t);
CAST_CONVERT_PROTOTYPE(UInt64, Int16, uint64_t, int16_t);
CAST_CONVERT_PROTOTYPE(UInt64, Int8, uint64_t, int8_t);

CAST_CONVERT_PROTOTYPE(UInt32, Int64, uint32_t, int64_t);
CAST_CONVERT_PROTOTYPE(UInt32, Int32, uint32_t, int32_t);
CAST_CONVERT_PROTOTYPE(UInt32, Int16, uint32_t, int16_t);
CAST_CONVERT_PROTOTYPE(UInt32, Int8, uint32_t, int8_t);

CAST_CONVERT_PROTOTYPE(UInt16, Int64, uint16_t, int64_t);
CAST_CONVERT_PROTOTYPE(UInt16, Int32, uint16_t, int32_t);
CAST_CONVERT_PROTOTYPE(UInt16, Int16, uint16_t, int16_t);
CAST_CONVERT_PROTOTYPE(UInt16, Int8, uint16_t, int8_t);

CAST_CONVERT_PROTOTYPE(UInt8, Int64, uint8_t, int64_t);
CAST_CONVERT_PROTOTYPE(UInt8, Int32, uint8_t, int32_t);
CAST_CONVERT_PROTOTYPE(UInt8, Int16, uint8_t, int16_t);
CAST_CONVERT_PROTOTYPE(UInt8, Int8, uint8_t, int8_t);

/* Size conversions */
CAST_CONVERT_PROTOTYPE(Size, UInt64, size_t, uint64_t);
CAST_CONVERT_PROTOTYPE(Size, UInt32, size_t, uint32_t);
CAST_CONVERT_PROTOTYPE(Size, UInt16, size_t, uint16_t);
CAST_CONVERT_PROTOTYPE(Size, UInt8, size_t, uint8_t);
CAST_CONVERT_PROTOTYPE(Size, Int64, size_t, int64_t);
CAST_CONVERT_PROTOTYPE(Size, Int32, size_t, int32_t);
CAST_CONVERT_PROTOTYPE(UInt64, Size, uint64_t, size_t);
CAST_CONVERT_PROTOTYPE(Int64, Size, int64_t, size_t);
CAST_CONVERT_PROTOTYPE(Int32, Size, int32_t, size_t);

#include "Impl/Integral.impl"
/* EOF */
