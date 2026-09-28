#pragma once

#include "../../Core/Preprocessor/Compiling.inc"

#include "Type.space"

/* -------------------------------------------------------------------------- */
/* Naming                                                                     */
/* -------------------------------------------------------------------------- */

#if defined(TYPE_ALIAS_SMALL) && defined(TYPE_ALIAS_COMPLETE)
#error "TYPE_ALIAS_SMALL and TYPE_ALIAS_COMPLETE are mutually exclusive"
#endif

#if !defined(TYPE_ALIAS_SMALL) && !defined(TYPE_ALIAS_COMPLETE)
#define TYPE_ALIAS_COMPLETE
#endif

#if defined(TYPE_ALIAS_SMALL)
#define TYPE_ALIAS_SELECT(SMALL, COMPLETE) SMALL
#else
#define TYPE_ALIAS_SELECT(SMALL, COMPLETE) COMPLETE
#endif

#define TYPE_ALIAS(SMALL, COMPLETE)                                           \
  TYPE_NS(TYPE_ALIAS_SELECT(SMALL, COMPLETE))

typedef int8_t TYPE_ALIAS(TI8, TInt8);
typedef int16_t TYPE_ALIAS(TI16, TInt16);
typedef int32_t TYPE_ALIAS(TI32, TInt32);
typedef int64_t TYPE_ALIAS(TI64, TInt64);

typedef uint8_t TYPE_ALIAS(TU8, TUInt8);
typedef uint16_t TYPE_ALIAS(TU16, TUInt16);
typedef uint32_t TYPE_ALIAS(TU32, TUInt32);
typedef uint64_t TYPE_ALIAS(TU64, TUInt64);

typedef float TYPE_ALIAS(TF32, TFloat32);
typedef double TYPE_ALIAS(TF64, TFloat64);
typedef long double TYPE_ALIAS(TLD, TLongDouble);

typedef uint8_t TYPE_ALIAS(TCh8, TChar8);
typedef uint16_t TYPE_ALIAS(TCh16, TChar16);
typedef uint32_t TYPE_ALIAS(TCh32, TChar32);
/* EOF */
