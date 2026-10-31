#pragma once

#include "../Error/Status.h"
#include "../Module/Common/Dispatch.impl"
#include "../Module/Common/Tuple.impl"
#include "../Preprocessor/Arguments.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Foreach.h"
#include "../Preprocessor/Operations.h"
#include "Cast.space"

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Declarative Type Conversion Engine (CONVERTING / TO / TO_EX)               */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/* 1. TO and TO_EX Specification Tuples                                       */
/* Tuple format: (IS_EX, LIMIT_COUNT, TO_NAME, TO_TYPE, ESPECIAL, LIMITS)     */
/* -------------------------------------------------------------------------- */

#define TO(NAME, TYPE, ESPECIAL)                                               \
  (0, 0, NAME, TYPE, ESPECIAL, ())

#define TO_EX(NAME, TYPE, ESPECIAL, ...)                                       \
  (1, PP_ARG_COUNT(__VA_ARGS__), NAME, TYPE, ESPECIAL, (__VA_ARGS__))

/* -------------------------------------------------------------------------- */
/* 2. Primitive Type Information Lookup                                       */
/* Format: (NAME, C_TYPE, SIGN_CATEGORY)                                      */
/* -------------------------------------------------------------------------- */

#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Uint64                          \
  (Uint64, uint64_t, UNSIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_UInt64                          \
  (UInt64, uint64_t, UNSIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Uint32                          \
  (Uint32, uint32_t, UNSIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_UInt32                          \
  (UInt32, uint32_t, UNSIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Uint16                          \
  (Uint16, uint16_t, UNSIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_UInt16                          \
  (UInt16, uint16_t, UNSIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Uint8                           \
  (Uint8, uint8_t, UNSIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_UInt8                           \
  (UInt8, uint8_t, UNSIGNED)

#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Int64                           \
  (Int64, int64_t, SIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Int32                           \
  (Int32, int32_t, SIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Int16                           \
  (Int16, int16_t, SIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Int8                            \
  (Int8, int8_t, SIGNED)

#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Size                            \
  (Size, size_t, UNSIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_PtrDiff                         \
  (PtrDiff, ptrdiff_t, SIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_IntPtr                          \
  (IntPtr, intptr_t, SIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_UIntPtr                         \
  (UIntPtr, uintptr_t, UNSIGNED)

#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Float                           \
  (Float, float, FLOAT)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Float32                         \
  (Float32, float, FLOAT)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_F32                             \
  (F32, float, FLOAT)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Double                          \
  (Double, double, FLOAT)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_Float64                         \
  (Float64, double, FLOAT)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_F64                             \
  (F64, double, FLOAT)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_LongDouble                      \
  (LongDouble, long double, FLOAT)
#define COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_LD                              \
  (LD, long double, FLOAT)

/* Unwrapping helper for parenthesized type specifications */
#define COSMERON_MACRO_INTERNAL_CAST_FROM_PAREN_2(NAME, C_TYPE)                \
  (NAME, C_TYPE, SIGNED)
#define COSMERON_MACRO_INTERNAL_CAST_FROM_PAREN_3(NAME, C_TYPE, SIGN)         \
  (NAME, C_TYPE, SIGN)
#define COSMERON_MACRO_INTERNAL_CAST_FROM_PAREN_DISPATCH(N, ...)               \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_CAST_FROM_PAREN_, N)(__VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_CAST_FROM_PAREN(...)                           \
  COSMERON_MACRO_INTERNAL_CAST_FROM_PAREN_DISPATCH(                            \
      PP_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_RESOLVE_TOKEN(NAME)                      \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_CAST_TYPE_INFO_, NAME)

#define COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM_1(SPEC)                      \
  COSMERON_MACRO_INTERNAL_CAST_FROM_PAREN SPEC

#define COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM_0(SPEC)                      \
  COSMERON_MACRO_INTERNAL_CAST_RESOLVE_TOKEN(SPEC)

#define COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM(SPEC)                        \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM_,                       \
             PP_IS_PAREN(SPEC))(SPEC)

/* -------------------------------------------------------------------------- */
/* 3. Overflow Check Generators and ID Dispatch                               */
/* -------------------------------------------------------------------------- */

/* Standard Rule Generator IDs */
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_UNSIGNED_UNSIGNED_1 1
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_UNSIGNED_UNSIGNED_0 0
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_UNSIGNED_SIGNED_0   0
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_UNSIGNED_SIGNED_1   1
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_SIGNED_SIGNED_0     0
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_SIGNED_SIGNED_2     4
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_SIGNED_UNSIGNED_0   2
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_SIGNED_UNSIGNED_1   3

/* Float Rule Generator IDs */
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_FLOAT_FLOAT_0       0
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_FLOAT_FLOAT_1       5
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_FLOAT_SIGNED_2      6
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_FLOAT_UNSIGNED_1    7
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_SIGNED_FLOAT_0      0
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_UNSIGNED_FLOAT_0    0

/* Descriptive Named Categories */
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_UNSIGNED_UNSIGNED_DOWNCAST_1 1
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_SIGNED_SIGNED_DOWNCAST_2     4
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_SIGNED_SIGNED_TO_UNSIGNED_0  2
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_SIGNED_SIGNED_TO_UNSIGNED_1  3
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_UNSIGNED_UNSIGNED_TO_SIGNED_0 0
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_UNSIGNED_UNSIGNED_TO_SIGNED_1 1
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_UNSIGNED_WIDER_0             0
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_SIGNED_WIDER_0               0
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_FLOAT_WIDER_0                0
#define COSMERON_MACRO_INTERNAL_CAST_GEN_ID_FLOAT_FLOAT_DOWNCAST_1       5

/* Generator 0: Infallible / No check */
#define COSMERON_MACRO_INTERNAL_CAST_CHECK_0(FROM_TYPE)                        \
  (void)inValue;

/* Generator 1: Upper limit check (Unsigned downcast / Unsigned to Signed) */
#define COSMERON_MACRO_INTERNAL_CAST_CHECK_1(FROM_TYPE, MAX)                   \
  if ((uint64_t)inValue > (uint64_t)(MAX))                                     \
    return STATUS_CONST(ARITHMETIC_OVERFLOW);

/* Generator 2: Negative check (Signed to same or wider Unsigned) */
#define COSMERON_MACRO_INTERNAL_CAST_CHECK_2(FROM_TYPE)                        \
  if (inValue < 0)                                                             \
    return STATUS_CONST(ARITHMETIC_OVERFLOW);

/* Generator 3: Negative and Upper limit check (Signed to narrower Unsigned) */
#define COSMERON_MACRO_INTERNAL_CAST_CHECK_3(FROM_TYPE, MAX)                   \
  if (inValue < 0 || (uint64_t)inValue > (uint64_t)(MAX))                      \
    return STATUS_CONST(ARITHMETIC_OVERFLOW);

/* Generator 4: Range check (Signed downcast) */
#define COSMERON_MACRO_INTERNAL_CAST_CHECK_4(FROM_TYPE, MIN, MAX)              \
  if (inValue < (FROM_TYPE)(MIN) || inValue > (FROM_TYPE)(MAX))                \
    return STATUS_CONST(ARITHMETIC_OVERFLOW);

/* Generator 5: Float downcast (Double/LongDouble downcast with MAX check) */
#define COSMERON_MACRO_INTERNAL_CAST_CHECK_5(FROM_TYPE, MAX)                   \
  if (isnan(inValue) || inValue < -(FROM_TYPE)(MAX) ||                         \
      inValue > (FROM_TYPE)(MAX))                                              \
    return STATUS_CONST(ARITHMETIC_OVERFLOW);

/* Generator 6: Float to signed integer (Float/Double/LongDouble to Signed int with MIN, MAX check) */
#define COSMERON_MACRO_INTERNAL_CAST_CHECK_6(FROM_TYPE, MIN, MAX)              \
  if (isnan(inValue) || inValue < (FROM_TYPE)(MIN) ||                          \
      inValue > (FROM_TYPE)(MAX) ||                                            \
      inValue >= (FROM_TYPE)9223372036854775808.0L)                           \
    return STATUS_CONST(ARITHMETIC_OVERFLOW);

/* Generator 7: Float to unsigned integer (Float/Double/LongDouble to Unsigned int with MAX check) */
#define COSMERON_MACRO_INTERNAL_CAST_CHECK_7(FROM_TYPE, MAX)                   \
  if (isnan(inValue) || inValue < (FROM_TYPE)0.0L ||                           \
      inValue > (FROM_TYPE)(MAX) ||                                            \
      inValue >= (FROM_TYPE)18446744073709551616.0L)                          \
    return STATUS_CONST(ARITHMETIC_OVERFLOW);

/* Exact Non-Variadic Dispatchers to satisfy ISO C99 pedantic rules */
#define COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_0(ID, FROM_TYPE, LIMITS)      \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_CAST_CHECK_, ID)(FROM_TYPE)

#define COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_1_DISPATCH(ID, FROM_TYPE, MAX) \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_CAST_CHECK_, ID)(FROM_TYPE, MAX)
#define COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_1_I(...)                       \
  COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_1_DISPATCH(__VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_1(ID, FROM_TYPE, LIMITS)      \
  COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_1_I(ID, FROM_TYPE, PP_UNWRAP LIMITS)

#define COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_2_DISPATCH(ID, FROM_TYPE, MIN, MAX) \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_CAST_CHECK_, ID)(FROM_TYPE, MIN, MAX)
#define COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_2_I(...)                       \
  COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_2_DISPATCH(__VA_ARGS__)
#define COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_2(ID, FROM_TYPE, LIMITS)      \
  COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_2_I(ID, FROM_TYPE, PP_UNWRAP LIMITS)

#define COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK(COUNT, ID, FROM_TYPE, LIMITS) \
  PP_OP_CAT2(COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK_, COUNT)(ID, FROM_TYPE, LIMITS)

/* Receiver 'self' validation helper for OBJECT targets */
#define COSMERON_MACRO_INTERNAL_CAST_CHECK_SELF(TARGET)                        \
  PP_IIF(COSMERON_IS_TARGET_OBJECT(TARGET),                                    \
         if (!self) return STATUS_CONST(INVALID_ARGUMENT);,                    \
         (void)0;)

/* -------------------------------------------------------------------------- */
/* 4. Tuple Unwrapping and Item Generators                                    */
/* -------------------------------------------------------------------------- */

/* Implementation Generator (via OPERATION_IMPLEMENT_EX) */
#define COSMERON_MACRO_INTERNAL_CAST_IMPL_UNWRAP(CTX, ITEM)                   \
  COSMERON_MACRO_INTERNAL_CAST_IMPL_UNWRAP_E(PP_UNWRAP CTX, PP_UNWRAP ITEM)

#define COSMERON_MACRO_INTERNAL_CAST_IMPL_UNWRAP_E(...)                        \
  COSMERON_MACRO_INTERNAL_CAST_IMPL_UNWRAP_I(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_IMPL_UNWRAP_I(                           \
    TARGET, FROM_NAME, FROM_TYPE, FROM_SIGN, IS_EX, COUNT, TO_NAME, TO_TYPE,  \
    ESPECIAL, LIMITS)                                                          \
  OPERATION_IMPLEMENT_EX(                                                      \
      TARGET, PP_OP_CAT3(FROM_NAME, to, TO_NAME),                             \
      (FROM_TYPE inValue, TO_TYPE * outValue)) {                               \
    COSMERON_MACRO_INTERNAL_CAST_CHECK_SELF(TARGET)                            \
    if (!outValue)                                                             \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK(                                   \
        COUNT,                                                                 \
        PP_OP_CAT6(COSMERON_MACRO_INTERNAL_CAST_GEN_ID_, FROM_SIGN, _,         \
                   ESPECIAL, _, COUNT),                                        \
        FROM_TYPE, LIMITS)                                                     \
    *outValue = (TO_TYPE)inValue;                                              \
    return STATUS_CONST(SUCCESS);                                              \
  }

/* Member Specification Generator (OPERATION) */
#define COSMERON_MACRO_INTERNAL_CAST_OP_UNWRAP(CTX, ITEM)                     \
  COSMERON_MACRO_INTERNAL_CAST_OP_UNWRAP_E(PP_UNWRAP CTX, PP_UNWRAP ITEM)

#define COSMERON_MACRO_INTERNAL_CAST_OP_UNWRAP_E(...)                         \
  COSMERON_MACRO_INTERNAL_CAST_OP_UNWRAP_I(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_OP_UNWRAP_I(                             \
    FROM_NAME, FROM_TYPE, FROM_SIGN, IS_EX, COUNT, TO_NAME, TO_TYPE,          \
    ESPECIAL, LIMITS)                                                          \
  OPERATION(PP_OP_CAT3(FROM_NAME, to, TO_NAME),                                \
            (FROM_TYPE inValue, TO_TYPE * outValue))

/* Member Specification Generator (CONST_OPERATION) */
#define COSMERON_MACRO_INTERNAL_CAST_CONST_OP_UNWRAP(CTX, ITEM)               \
  COSMERON_MACRO_INTERNAL_CAST_CONST_OP_UNWRAP_E(PP_UNWRAP CTX, PP_UNWRAP ITEM)

#define COSMERON_MACRO_INTERNAL_CAST_CONST_OP_UNWRAP_E(...)                   \
  COSMERON_MACRO_INTERNAL_CAST_CONST_OP_UNWRAP_I(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_CONST_OP_UNWRAP_I(                       \
    FROM_NAME, FROM_TYPE, FROM_SIGN, IS_EX, COUNT, TO_NAME, TO_TYPE,          \
    ESPECIAL, LIMITS)                                                          \
  CONST_OPERATION(PP_OP_CAT3(FROM_NAME, to, TO_NAME),                          \
                  (FROM_TYPE inValue, TO_TYPE * outValue))

/* Package Member Specification Generator (OPERATION_INLINE) */
#define COSMERON_MACRO_INTERNAL_CAST_INLINE_UNWRAP(CTX, ITEM)                 \
  COSMERON_MACRO_INTERNAL_CAST_INLINE_UNWRAP_E(PP_UNWRAP CTX, PP_UNWRAP ITEM)

#define COSMERON_MACRO_INTERNAL_CAST_INLINE_UNWRAP_E(...)                     \
  COSMERON_MACRO_INTERNAL_CAST_INLINE_UNWRAP_I(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_INLINE_UNWRAP_I(                         \
    FROM_NAME, FROM_TYPE, FROM_SIGN, IS_EX, COUNT, TO_NAME, TO_TYPE,          \
    ESPECIAL, LIMITS)                                                          \
  OPERATION_INLINE(                                                            \
      PP_OP_CAT3(FROM_NAME, to, TO_NAME),                                     \
      (FROM_TYPE inValue, TO_TYPE * outValue), {                               \
        if (!outValue)                                                         \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK(                               \
            COUNT,                                                             \
            PP_OP_CAT6(COSMERON_MACRO_INTERNAL_CAST_GEN_ID_, FROM_SIGN, _,     \
                       ESPECIAL, _, COUNT),                                    \
            FROM_TYPE, LIMITS)                                                 \
        *outValue = (TO_TYPE)inValue;                                          \
        return STATUS_CONST(SUCCESS);                                          \
      })

/* Package Member Specification Generator (CONST_OPERATION_INLINE) */
#define COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_UNWRAP(CTX, ITEM)           \
  COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_UNWRAP_E(PP_UNWRAP CTX,             \
                                                     PP_UNWRAP ITEM)

#define COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_UNWRAP_E(...)               \
  COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_UNWRAP_I(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_UNWRAP_I(                   \
    FROM_NAME, FROM_TYPE, FROM_SIGN, IS_EX, COUNT, TO_NAME, TO_TYPE,          \
    ESPECIAL, LIMITS)                                                          \
  CONST_OPERATION_INLINE(                                                      \
      PP_OP_CAT3(FROM_NAME, to, TO_NAME),                                     \
      (FROM_TYPE inValue, TO_TYPE * outValue), {                               \
        if (!outValue)                                                         \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK(                               \
            COUNT,                                                             \
            PP_OP_CAT6(COSMERON_MACRO_INTERNAL_CAST_GEN_ID_, FROM_SIGN, _,     \
                       ESPECIAL, _, COUNT),                                    \
            FROM_TYPE, LIMITS)                                                 \
        *outValue = (TO_TYPE)inValue;                                          \
        return STATUS_CONST(SUCCESS);                                          \
      })

/* Object Member Specification Generator (OPERATION_INLINE with self receiver check) */
#define COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_UNWRAP(CTX, ITEM)             \
  COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_UNWRAP_E(PP_UNWRAP CTX,             \
                                                   PP_UNWRAP ITEM)

#define COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_UNWRAP_E(...)                 \
  COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_UNWRAP_I(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_UNWRAP_I(                     \
    FROM_NAME, FROM_TYPE, FROM_SIGN, IS_EX, COUNT, TO_NAME, TO_TYPE,          \
    ESPECIAL, LIMITS)                                                          \
  OPERATION_INLINE(                                                            \
      PP_OP_CAT3(FROM_NAME, to, TO_NAME),                                     \
      (FROM_TYPE inValue, TO_TYPE * outValue), {                               \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!outValue)                                                         \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK(                               \
            COUNT,                                                             \
            PP_OP_CAT6(COSMERON_MACRO_INTERNAL_CAST_GEN_ID_, FROM_SIGN, _,     \
                       ESPECIAL, _, COUNT),                                    \
            FROM_TYPE, LIMITS)                                                 \
        *outValue = (TO_TYPE)inValue;                                          \
        return STATUS_CONST(SUCCESS);                                          \
      })

/* Object Member Specification Generator (CONST_OPERATION_INLINE with self receiver check) */
#define COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_UNWRAP(CTX, ITEM)       \
  COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_UNWRAP_E(PP_UNWRAP CTX,         \
                                                         PP_UNWRAP ITEM)

#define COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_UNWRAP_E(...)           \
  COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_UNWRAP_I(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_UNWRAP_I(               \
    FROM_NAME, FROM_TYPE, FROM_SIGN, IS_EX, COUNT, TO_NAME, TO_TYPE,          \
    ESPECIAL, LIMITS)                                                          \
  CONST_OPERATION_INLINE(                                                      \
      PP_OP_CAT3(FROM_NAME, to, TO_NAME),                                     \
      (FROM_TYPE inValue, TO_TYPE * outValue), {                               \
        if (!self)                                                             \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (!outValue)                                                         \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        COSMERON_MACRO_INTERNAL_CAST_CALL_CHECK(                               \
            COUNT,                                                             \
            PP_OP_CAT6(COSMERON_MACRO_INTERNAL_CAST_GEN_ID_, FROM_SIGN, _,     \
                       ESPECIAL, _, COUNT),                                    \
            FROM_TYPE, LIMITS)                                                 \
        *outValue = (TO_TYPE)inValue;                                          \
        return STATUS_CONST(SUCCESS);                                          \
      })

/* -------------------------------------------------------------------------- */
/* 5. Public Generation Macros                                                */
/* -------------------------------------------------------------------------- */

/* Target-Specific Implementation: CONVERTING_TARGET(TARGET, FROM_SPEC, ...) */
#define COSMERON_MACRO_INTERNAL_CAST_TARGET_APPLY_E(                          \
    TARGET, FROM_NAME, FROM_TYPE, FROM_SIGN, ...)                              \
  PP_FOREACH_CTX(COSMERON_MACRO_INTERNAL_CAST_IMPL_UNWRAP,                     \
                 (TARGET, FROM_NAME, FROM_TYPE, FROM_SIGN), __VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_TARGET_DISPATCH_I(...)                   \
  COSMERON_MACRO_INTERNAL_CAST_TARGET_APPLY_E(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_TARGET_DISPATCH(TARGET, INFO, ITEMS)      \
  COSMERON_MACRO_INTERNAL_CAST_TARGET_DISPATCH_I(                              \
      TARGET, PP_UNWRAP INFO, PP_UNWRAP ITEMS)

#define CONVERTING_TARGET(TARGET, FROM_SPEC, ...)                              \
  COSMERON_MACRO_INTERNAL_CAST_TARGET_DISPATCH(                                \
      TARGET,                                                                  \
      COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM(FROM_SPEC),                    \
      (__VA_ARGS__))

/* Standalone Implementation for Cast namespace: CONVERTING(FROM_SPEC, ...) */
#define CONVERTING(FROM_SPEC, ...)                                             \
  CONVERTING_TARGET(LIB_PREFIX(CAST_MOD), FROM_SPEC, __VA_ARGS__)

/* Member Specification Block: CONVERTING_OPERATIONS(FROM_SPEC, ...) */
#define COSMERON_MACRO_INTERNAL_CAST_OP_APPLY_E(                               \
    FROM_NAME, FROM_TYPE, FROM_SIGN, ...)                                      \
  PP_FOREACH_COMMA_CTX(COSMERON_MACRO_INTERNAL_CAST_OP_UNWRAP,                 \
                       (FROM_NAME, FROM_TYPE, FROM_SIGN), __VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_OP_DISPATCH_I(...)                       \
  COSMERON_MACRO_INTERNAL_CAST_OP_APPLY_E(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_OP_DISPATCH(INFO, ITEMS)                  \
  COSMERON_MACRO_INTERNAL_CAST_OP_DISPATCH_I(PP_UNWRAP INFO, PP_UNWRAP ITEMS)

#define CONVERTING_OPERATIONS(FROM_SPEC, ...)                                  \
  COSMERON_MACRO_INTERNAL_CAST_OP_DISPATCH(                                    \
      COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM(FROM_SPEC),                    \
      (__VA_ARGS__))

/* Const Member Specification Block: CONST_CONVERTING_OPERATIONS(FROM_SPEC, ...) */
#define COSMERON_MACRO_INTERNAL_CAST_CONST_OP_APPLY_E(                         \
    FROM_NAME, FROM_TYPE, FROM_SIGN, ...)                                      \
  PP_FOREACH_COMMA_CTX(COSMERON_MACRO_INTERNAL_CAST_CONST_OP_UNWRAP,           \
                       (FROM_NAME, FROM_TYPE, FROM_SIGN), __VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_CONST_OP_DISPATCH_I(...)                 \
  COSMERON_MACRO_INTERNAL_CAST_CONST_OP_APPLY_E(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_CONST_OP_DISPATCH(INFO, ITEMS)            \
  COSMERON_MACRO_INTERNAL_CAST_CONST_OP_DISPATCH_I(PP_UNWRAP INFO,             \
                                                   PP_UNWRAP ITEMS)

#define CONST_CONVERTING_OPERATIONS(FROM_SPEC, ...)                            \
  COSMERON_MACRO_INTERNAL_CAST_CONST_OP_DISPATCH(                              \
      COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM(FROM_SPEC),                    \
      (__VA_ARGS__))

/* Package Inline Member Specification Block: CONVERTING_INLINE(FROM_SPEC, ...) */
#define COSMERON_MACRO_INTERNAL_CAST_INLINE_APPLY_E(                          \
    FROM_NAME, FROM_TYPE, FROM_SIGN, ...)                                      \
  PP_FOREACH_COMMA_CTX(COSMERON_MACRO_INTERNAL_CAST_INLINE_UNWRAP,             \
                       (FROM_NAME, FROM_TYPE, FROM_SIGN), __VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_INLINE_DISPATCH_I(...)                   \
  COSMERON_MACRO_INTERNAL_CAST_INLINE_APPLY_E(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_INLINE_DISPATCH(INFO, ITEMS)              \
  COSMERON_MACRO_INTERNAL_CAST_INLINE_DISPATCH_I(PP_UNWRAP INFO,              \
                                                  PP_UNWRAP ITEMS)

#define CONVERTING_INLINE(FROM_SPEC, ...)                                      \
  COSMERON_MACRO_INTERNAL_CAST_INLINE_DISPATCH(                                \
      COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM(FROM_SPEC),                    \
      (__VA_ARGS__))

/* Package Const Inline Member Specification: CONST_CONVERTING_INLINE(FROM_SPEC, ...) */
#define COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_APPLY_E(                    \
    FROM_NAME, FROM_TYPE, FROM_SIGN, ...)                                      \
  PP_FOREACH_COMMA_CTX(COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_UNWRAP,       \
                       (FROM_NAME, FROM_TYPE, FROM_SIGN), __VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_DISPATCH_I(...)             \
  COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_APPLY_E(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_DISPATCH(INFO, ITEMS)        \
  COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_DISPATCH_I(PP_UNWRAP INFO,         \
                                                        PP_UNWRAP ITEMS)

#define CONST_CONVERTING_INLINE(FROM_SPEC, ...)                                \
  COSMERON_MACRO_INTERNAL_CAST_CONST_INLINE_DISPATCH(                          \
      COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM(FROM_SPEC),                    \
      (__VA_ARGS__))

/* Object Inline Member Specification Block: OBJECT_CONVERTING_INLINE(FROM_SPEC, ...) */
#define COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_APPLY_E(                      \
    FROM_NAME, FROM_TYPE, FROM_SIGN, ...)                                      \
  PP_FOREACH_COMMA_CTX(COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_UNWRAP,         \
                       (FROM_NAME, FROM_TYPE, FROM_SIGN), __VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_DISPATCH_I(...)               \
  COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_APPLY_E(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_DISPATCH(INFO, ITEMS)          \
  COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_DISPATCH_I(PP_UNWRAP INFO,           \
                                                      PP_UNWRAP ITEMS)

#define OBJECT_CONVERTING_INLINE(FROM_SPEC, ...)                               \
  COSMERON_MACRO_INTERNAL_CAST_OBJ_INLINE_DISPATCH(                            \
      COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM(FROM_SPEC),                    \
      (__VA_ARGS__))

/* Object Const Inline Member Specification: OBJECT_CONST_CONVERTING_INLINE(FROM_SPEC, ...) */
#define COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_APPLY_E(                \
    FROM_NAME, FROM_TYPE, FROM_SIGN, ...)                                      \
  PP_FOREACH_COMMA_CTX(COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_UNWRAP,   \
                       (FROM_NAME, FROM_TYPE, FROM_SIGN), __VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_DISPATCH_I(...)         \
  COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_DISPATCH_I(__VA_ARGS__)

#define COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_DISPATCH(INFO, ITEMS)    \
  COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_DISPATCH_I(                    \
      PP_UNWRAP INFO, PP_UNWRAP ITEMS)

#define OBJECT_CONST_CONVERTING_INLINE(FROM_SPEC, ...)                         \
  COSMERON_MACRO_INTERNAL_CAST_OBJ_CONST_INLINE_DISPATCH(                      \
      COSMERON_MACRO_INTERNAL_CAST_RESOLVE_FROM(FROM_SPEC),                    \
      (__VA_ARGS__))

/* EOF */
