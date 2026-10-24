#pragma once

#include "../Error/Status.h"
#include "../Module/Package.h"
#include "../Preprocessor/Compiling.inc"
#include "Cast.space"

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

PACKAGE(LIB_PREFIX(CAST_MOD),
        /* Unsigned downcasts (same signedness: Uint to width) */
        OPERATION(Uint64to32, (uint32_t * outValue, uint64_t inValue)),
        OPERATION(Uint64to16, (uint16_t * outValue, uint64_t inValue)),
        OPERATION(Uint64to8, (uint8_t * outValue, uint64_t inValue)),
        OPERATION(Uint32to16, (uint16_t * outValue, uint32_t inValue)),
        OPERATION(Uint32to8, (uint8_t * outValue, uint32_t inValue)),
        OPERATION(Uint16to8, (uint8_t * outValue, uint16_t inValue)),

        /* Signed downcasts (same signedness: Int to width) */
        OPERATION(Int64to32, (int32_t * outValue, int64_t inValue)),
        OPERATION(Int64to16, (int16_t * outValue, int64_t inValue)),
        OPERATION(Int64to8, (int8_t * outValue, int64_t inValue)),
        OPERATION(Int32to16, (int16_t * outValue, int32_t inValue)),
        OPERATION(Int32to8, (int8_t * outValue, int32_t inValue)),
        OPERATION(Int16to8, (int8_t * outValue, int16_t inValue)),

        /* Signed to unsigned conversions (Int to U+width) */
        OPERATION(Int64toU64, (uint64_t * outValue, int64_t inValue)),
        OPERATION(Int64toU32, (uint32_t * outValue, int64_t inValue)),
        OPERATION(Int64toU16, (uint16_t * outValue, int64_t inValue)),
        OPERATION(Int64toU8, (uint8_t * outValue, int64_t inValue)),

        OPERATION(Int32toU64, (uint64_t * outValue, int32_t inValue)),
        OPERATION(Int32toU32, (uint32_t * outValue, int32_t inValue)),
        OPERATION(Int32toU16, (uint16_t * outValue, int32_t inValue)),
        OPERATION(Int32toU8, (uint8_t * outValue, int32_t inValue)),

        OPERATION(Int16toU64, (uint64_t * outValue, int16_t inValue)),
        OPERATION(Int16toU32, (uint32_t * outValue, int16_t inValue)),
        OPERATION(Int16toU16, (uint16_t * outValue, int16_t inValue)),
        OPERATION(Int16toU8, (uint8_t * outValue, int16_t inValue)),

        OPERATION(Int8toU64, (uint64_t * outValue, int8_t inValue)),
        OPERATION(Int8toU32, (uint32_t * outValue, int8_t inValue)),
        OPERATION(Int8toU16, (uint16_t * outValue, int8_t inValue)),
        OPERATION(Int8toU8, (uint8_t * outValue, int8_t inValue)),

        /* Unsigned to signed conversions (Uint to I+width) */
        OPERATION(Uint64toI64, (int64_t * outValue, uint64_t inValue)),
        OPERATION(Uint64toI32, (int32_t * outValue, uint64_t inValue)),
        OPERATION(Uint64toI16, (int16_t * outValue, uint64_t inValue)),
        OPERATION(Uint64toI8, (int8_t * outValue, uint64_t inValue)),

        OPERATION(Uint32toI64, (int64_t * outValue, uint32_t inValue)),
        OPERATION(Uint32toI32, (int32_t * outValue, uint32_t inValue)),
        OPERATION(Uint32toI16, (int16_t * outValue, uint32_t inValue)),
        OPERATION(Uint32toI8, (int8_t * outValue, uint32_t inValue)),

        OPERATION(Uint16toI64, (int64_t * outValue, uint16_t inValue)),
        OPERATION(Uint16toI32, (int32_t * outValue, uint16_t inValue)),
        OPERATION(Uint16toI16, (int16_t * outValue, uint16_t inValue)),
        OPERATION(Uint16toI8, (int8_t * outValue, uint16_t inValue)),

        OPERATION(Uint8toI64, (int64_t * outValue, uint8_t inValue)),
        OPERATION(Uint8toI32, (int32_t * outValue, uint8_t inValue)),
        OPERATION(Uint8toI16, (int16_t * outValue, uint8_t inValue)),
        OPERATION(Uint8toI8, (int8_t * outValue, uint8_t inValue)),

        /* Size conversions */
        OPERATION(SizetoU64, (uint64_t * outValue, size_t inValue)),
        OPERATION(SizetoU32, (uint32_t * outValue, size_t inValue)),
        OPERATION(SizetoU16, (uint16_t * outValue, size_t inValue)),
        OPERATION(SizetoU8, (uint8_t * outValue, size_t inValue)),
        OPERATION(SizetoI64, (int64_t * outValue, size_t inValue)),
        OPERATION(SizetoI32, (int32_t * outValue, size_t inValue)),
        OPERATION(Uint64toSize, (size_t * outValue, uint64_t inValue)),
        OPERATION(Int64toSize, (size_t * outValue, int64_t inValue)),
        OPERATION(Int32toSize, (size_t * outValue, int32_t inValue)));

#include "Impl/Integer.impl"
/* EOF */
