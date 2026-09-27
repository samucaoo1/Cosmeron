#pragma once

#include "../../Core/Preprocessor/Compiling.inc"

#include "Type.space"

/* -------------------------------------------------------------------------- */
/* Naming                                                                     */
/* -------------------------------------------------------------------------- */

#if defined(TYPE_ALIAS_SMALL) && defined(TYPE_ALIAS_COMPLETE)
    #error "TYPE_ALIAS_SMALL and TYPE_ALIAS_COMPLETE are mutually exclusive"
#endif

#if defined(TYPE_ALIAS_SAFE) && defined(TYPE_ALIAS_UNSAFE)
    #error "TYPE_ALIAS_SAFE and TYPE_ALIAS_UNSAFE are mutually exclusive"
#endif

#if !defined(TYPE_ALIAS_SMALL) && !defined(TYPE_ALIAS_COMPLETE)
    #define TYPE_ALIAS_COMPLETE
#endif

#if !defined(TYPE_ALIAS_SAFE) && !defined(TYPE_ALIAS_UNSAFE)
    #define TYPE_ALIAS_UNSAFE
#endif

#if defined(TYPE_ALIAS_SMALL)
    #define TYPE_ALIAS_SELECT(SMALL, COMPLETE) SMALL
#else
    #define TYPE_ALIAS_SELECT(SMALL, COMPLETE) COMPLETE
#endif

#if defined(TYPE_ALIAS_SAFE)
    #define TYPE_ALIAS_APPLY(NAME) TYPE_MOD(NAME)
#else
    #define TYPE_ALIAS_APPLY(NAME) NAME
#endif

#define TYPE_ALIAS(SMALL, COMPLETE) \
    TYPE_ALIAS_APPLY(TYPE_ALIAS_SELECT(SMALL, COMPLETE))

typedef int8_t   TYPE_ALIAS(I8, Int8);
typedef int16_t  TYPE_ALIAS(I16, Int16);
typedef int32_t  TYPE_ALIAS(I32, Int32);
typedef int64_t  TYPE_ALIAS(I64, Int64);

typedef uint8_t  TYPE_ALIAS(U8, UInt8);
typedef uint16_t TYPE_ALIAS(U16, UInt16);
typedef uint32_t TYPE_ALIAS(U32, UInt32);
typedef uint64_t TYPE_ALIAS(U64, UInt64);

typedef float       TYPE_ALIAS(F32, Float32);
typedef double      TYPE_ALIAS(F64, Float64);
typedef long double TYPE_ALIAS(LD, LongDouble);

typedef uint8_t  TYPE_ALIAS(Ch8, Char8);
typedef uint16_t TYPE_ALIAS(Ch16, Char16);
typedef uint32_t TYPE_ALIAS(Ch32, Char32);
/* EOF */
