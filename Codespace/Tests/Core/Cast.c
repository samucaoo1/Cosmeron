#include "../../Cosmeron/Core/Cast/Cast.h"
#include "../../Cosmeron/Core/Module/Object.h"
#include "../../Cosmeron/Core/Module/Package.h"

#include <float.h>
#include <math.h>
#include <stdint.h>

#define CHECK_TRUE(COND)                                                       \
  do {                                                                         \
    if (!(COND))                                                               \
      return __LINE__;                                                         \
  } while (0)

#define CHECK_STATUS(EXPR, EXPECTED)                                           \
  do {                                                                         \
    if ((EXPR) != (EXPECTED))                                                  \
      return __LINE__;                                                         \
  } while (0)

typedef struct CAST_TYPE(Value) {
  int value;
} CAST_TYPE(Value);

static int CAST_FUNC(Identity)(int value) { return value; }

static CAST_TYPE(Value) CAST_TYPE_TO_STRUCT(int, Value)(int value) {
  CAST_TYPE(Value) result = {value};
  return result;
}

static int CAST_STRUCT_TO_TYPE(Value, int)(CAST_TYPE(Value) value) {
  return value.value;
}

/* -------------------------------------------------------------------------- */
/* Custom Package with CONVERTING_OPERATIONS & CONVERTING_TARGET              */
/* -------------------------------------------------------------------------- */

PACKAGE(
    CustomConvertPkg,
    CONVERTING_OPERATIONS(
        Uint64,
        TO_EX(U16, uint16_t, UNSIGNED, UINT16_MAX)
    ),
    CONVERTING_OPERATIONS(
        (CustomInt, int32_t, SIGNED),
        TO_EX(I8, int8_t, SIGNED, INT8_MIN, INT8_MAX)
    )
);

CONVERTING_TARGET(
    CustomConvertPkg,
    Uint64,
    TO_EX(U16, uint16_t, UNSIGNED, UINT16_MAX)
)

CONVERTING_TARGET(
    CustomConvertPkg,
    (CustomInt, int32_t, SIGNED),
    TO_EX(I8, int8_t, SIGNED, INT8_MIN, INT8_MAX)
)

PACKAGE_IMPLEMENT(CustomConvertPkg);

/* -------------------------------------------------------------------------- */
/* Custom Package with CONVERTING_INLINE                                      */
/* -------------------------------------------------------------------------- */

PACKAGE(
    CustomInlinePkg,
    CONVERTING_INLINE(
        Uint32,
        TO_EX(U8, uint8_t, UNSIGNED, UINT8_MAX)
    )
);

PACKAGE_IMPLEMENT(CustomInlinePkg);

/* -------------------------------------------------------------------------- */
/* Custom Object with OBJECT_CONVERTING_INLINE                                */
/* -------------------------------------------------------------------------- */

OBJECT(
    TConvertObject,
    FIELDS(int dummy;),
    OBJECT_CONVERTING_INLINE(
        Int32,
        TO_EX(I8, int8_t, SIGNED, INT8_MIN, INT8_MAX)
    )
);

OBJECT_IMPLEMENT(TConvertObject);

/* -------------------------------------------------------------------------- */
/* Custom Object with CONVERTING_OPERATIONS and CONVERTING_TARGET             */
/* -------------------------------------------------------------------------- */

OBJECT(
    TConvertOutLineObject,
    FIELDS(int dummy;),
    CONVERTING_OPERATIONS(
        Int16,
        TO_EX(I8, int8_t, SIGNED, INT8_MIN, INT8_MAX)
    )
);

#define TConvertOutLineObject_IS_OBJECT 1
CONVERTING_TARGET(
    TConvertOutLineObject,
    Int16,
    TO_EX(I8, int8_t, SIGNED, INT8_MIN, INT8_MAX)
)

OBJECT_IMPLEMENT(TConvertOutLineObject);

/* -------------------------------------------------------------------------- */
/* Standalone CONVERTING test with custom type                                */
/* -------------------------------------------------------------------------- */

CONVERTING(
    (StandaloneInt, int32_t, SIGNED),
    TO_EX(I8, int8_t, SIGNED, INT8_MIN, INT8_MAX)
)

/* -------------------------------------------------------------------------- */
/* Test Cases                                                                 */
/* -------------------------------------------------------------------------- */

static int Test_MacrosAndLegacy(void) {
  CAST_TYPE(Value) value = CAST_TYPE_TO_STRUCT(int, Value)(7);
  CHECK_TRUE(CAST_FUNC(Identity)(value.value) == 7);
  CHECK_TRUE(CAST_STRUCT_TO_TYPE(Value, int)(value) == 7);

  /* FunctionTable call test: (inValue, outValue) */
  uint32_t outVal = 0;
  CHECK_STATUS(PACKAGE_CALL(LIB_PREFIX(CAST_MOD), Uint64to32, 0x1234U, &outVal),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(outVal == 0x1234U);
  return 0;
}

static int Test_UnsignedDowncast(void) {
  uint32_t u32 = 0;
  uint16_t u16 = 0;
  uint8_t u8 = 0;

  /* Null pointer check: (inValue, outValue) */
  CHECK_STATUS(CAST_FUNC(Uint64to32)(100, NULL),
               STATUS_CONST(INVALID_ARGUMENT));

  /* Uint64 to 32 */
  CHECK_STATUS(CAST_FUNC(Uint64to32)(0x12345678ULL, &u32),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32 == 0x12345678U);

  u32 = 0xAA;
  CHECK_STATUS(CAST_FUNC(Uint64to32)(0x100000000ULL, &u32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_TRUE(u32 == 0xAA); /* outValue untouched on failure */

  /* Uint32 to 16 */
  CHECK_STATUS(CAST_FUNC(Uint32to16)(0xFFFFU, &u16),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16 == 0xFFFFU);

  CHECK_STATUS(CAST_FUNC(Uint32to16)(0x10000U, &u16),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Uint16 to 8 */
  CHECK_STATUS(CAST_FUNC(Uint16to8)(0xFFU, &u8),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8 == 0xFFU);

  CHECK_STATUS(CAST_FUNC(Uint16to8)(0x100U, &u8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Uint64 to 8 */
  CHECK_STATUS(CAST_FUNC(Uint64to8)(42ULL, &u8),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8 == 42U);

  CHECK_STATUS(CAST_FUNC(Uint64to8)(256ULL, &u8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  return 0;
}

static int Test_SignedDowncast(void) {
  int32_t i32 = 0;
  int16_t i16 = 0;
  int8_t i8 = 0;

  /* Int64 to 32 */
  CHECK_STATUS(CAST_FUNC(Int64to32)(-1234567LL, &i32),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == -1234567);

  CHECK_STATUS(CAST_FUNC(Int64to32)(2147483647LL, &i32),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 2147483647);

  CHECK_STATUS(CAST_FUNC(Int64to32)(2147483648LL, &i32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int64to32)(-2147483648LL, &i32),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == (-2147483647 - 1));

  CHECK_STATUS(CAST_FUNC(Int64to32)(-2147483649LL, &i32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Int32 to 16 */
  CHECK_STATUS(CAST_FUNC(Int32to16)(32767, &i16),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i16 == 32767);

  CHECK_STATUS(CAST_FUNC(Int32to16)(32768, &i16),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int32to16)(-32768, &i16),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i16 == -32768);

  CHECK_STATUS(CAST_FUNC(Int32to16)(-32769, &i16),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Int16 to 8 */
  CHECK_STATUS(CAST_FUNC(Int16to8)(127, &i8),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == 127);

  CHECK_STATUS(CAST_FUNC(Int16to8)(128, &i8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int16to8)(-128, &i8),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == -128);

  CHECK_STATUS(CAST_FUNC(Int16to8)(-129, &i8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  return 0;
}

static int Test_SignedToUnsigned(void) {
  uint64_t u64 = 0;
  uint32_t u32 = 0;
  uint16_t u16 = 0;
  uint8_t u8 = 0;

  /* Int64 to U64 */
  CHECK_STATUS(CAST_FUNC(Int64toU64)(100LL, &u64),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64 == 100ULL);

  CHECK_STATUS(CAST_FUNC(Int64toU64)(-1LL, &u64),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Int64 to U32 */
  CHECK_STATUS(CAST_FUNC(Int64toU32)(0x7FFFFFFFLL, &u32),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32 == 0x7FFFFFFFU);

  CHECK_STATUS(CAST_FUNC(Int64toU32)(-5LL, &u32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int64toU32)(0x100000000LL, &u32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Int32 to U16 */
  CHECK_STATUS(CAST_FUNC(Int32toU16)(65535, &u16),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16 == 65535U);

  CHECK_STATUS(CAST_FUNC(Int32toU16)(-1, &u16),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int32toU16)(65536, &u16),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Int16 to U8 */
  CHECK_STATUS(CAST_FUNC(Int16toU8)(255, &u8),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8 == 255U);

  CHECK_STATUS(CAST_FUNC(Int16toU8)(-1, &u8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int16toU8)(256, &u8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Small signed to large unsigned (Int8 to U64) */
  CHECK_STATUS(CAST_FUNC(Int8toU64)(42, &u64),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64 == 42ULL);

  CHECK_STATUS(CAST_FUNC(Int8toU64)(-42, &u64),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  return 0;
}

static int Test_UnsignedToSigned(void) {
  int64_t i64 = 0;
  int32_t i32 = 0;
  int16_t i16 = 0;
  int8_t i8 = 0;

  /* Uint64 to I64 */
  CHECK_STATUS(CAST_FUNC(Uint64toI64)(0x7FFFFFFFFFFFFFFFULL, &i64),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i64 == 0x7FFFFFFFFFFFFFFFLL);

  CHECK_STATUS(CAST_FUNC(Uint64toI64)(0x8000000000000000ULL, &i64),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Uint32 to I32 */
  CHECK_STATUS(CAST_FUNC(Uint32toI32)(0x7FFFFFFFU, &i32),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 0x7FFFFFFF);

  CHECK_STATUS(CAST_FUNC(Uint32toI32)(0x80000000U, &i32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Uint16 to I16 */
  CHECK_STATUS(CAST_FUNC(Uint16toI16)(32767U, &i16),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i16 == 32767);

  CHECK_STATUS(CAST_FUNC(Uint16toI16)(32768U, &i16),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Uint8 to I8 */
  CHECK_STATUS(CAST_FUNC(Uint8toI8)(127U, &i8),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == 127);

  CHECK_STATUS(CAST_FUNC(Uint8toI8)(128U, &i8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Large unsigned to small signed (Uint64 to I32) */
  CHECK_STATUS(CAST_FUNC(Uint64toI32)(500ULL, &i32),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 500);

  CHECK_STATUS(CAST_FUNC(Uint64toI32)(0x80000000ULL, &i32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  return 0;
}

static int Test_SizeConversions(void) {
  size_t sz = 0;
  uint32_t u32 = 0;
  uint8_t u8 = 0;
  int32_t i32 = 0;

  CHECK_STATUS(CAST_FUNC(SizetoU32)((size_t)1000, &u32),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32 == 1000U);

  CHECK_STATUS(CAST_FUNC(SizetoU8)((size_t)300, &u8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(SizetoI32)((size_t)2147483647, &i32),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 2147483647);

  CHECK_STATUS(CAST_FUNC(Uint64toSize)(500ULL, &sz),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(sz == 500U);

  CHECK_STATUS(CAST_FUNC(Int64toSize)(100LL, &sz),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(sz == 100U);

  CHECK_STATUS(CAST_FUNC(Int64toSize)(-1LL, &sz),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int32toSize)(-1, &sz),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  return 0;
}

static int Test_CustomAndIntegration(void) {
  uint16_t u16 = 0;
  uint8_t u8 = 0;
  int8_t i8 = 0;

  /* Custom Package calls */
  CHECK_STATUS(CustomConvertPkg_Uint64toU16(500ULL, &u16),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16 == 500U);

  CHECK_STATUS(PACKAGE_CALL(CustomConvertPkg, CustomInttoI8, 100, &i8),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == 100);

  CHECK_STATUS(PACKAGE_CALL(CustomConvertPkg, CustomInttoI8, 200, &i8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Null argument checks */
  CHECK_STATUS(CustomConvertPkg_Uint64toU16(500ULL, NULL),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(CAST_FUNC(Uint64to32)(10ULL, NULL),
               STATUS_CONST(INVALID_ARGUMENT));

  /* Custom Package with CONVERTING_INLINE */
  CHECK_STATUS(CustomInlinePkg_Uint32toU8(42U, &u8),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8 == 42U);

  CHECK_STATUS(PACKAGE_CALL(CustomInlinePkg, Uint32toU8, 300U, &u8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Custom Object with OBJECT_CONVERTING_INLINE */
  TConvertObject obj;
  CHECK_STATUS(TConvertObject_Init(&obj), STATUS_CONST(SUCCESS));

  CHECK_STATUS(OBJECT_CALL(&obj, Int32toI8, 55, &i8), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == 55);

  CHECK_STATUS(OBJECT_CALL(&obj, Int32toI8, 300, &i8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(TConvertObject_Int32toI8(NULL, 55, &i8),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(OBJECT_CALL(&obj, Int32toI8, 55, NULL),
               STATUS_CONST(INVALID_ARGUMENT));

  /* Custom Object with CONVERTING_OPERATIONS + CONVERTING_TARGET (out-of-line) */
  TConvertOutLineObject outObj;
  CHECK_STATUS(TConvertOutLineObject_Init(&outObj), STATUS_CONST(SUCCESS));

  CHECK_STATUS(OBJECT_CALL(&outObj, Int16toI8, 12, &i8), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == 12);

  CHECK_STATUS(OBJECT_CALL(&outObj, Int16toI8, 500, &i8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(TConvertOutLineObject_Int16toI8(NULL, 12, &i8),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(OBJECT_CALL(&outObj, Int16toI8, 12, NULL),
               STATUS_CONST(INVALID_ARGUMENT));

  /* Standalone CONVERTING call */
  CHECK_STATUS(Cast_StandaloneInttoI8(42, &i8), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == 42);
  CHECK_STATUS(Cast_StandaloneInttoI8(42, NULL), STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(Cast_StandaloneInttoI8(500, &i8), STATUS_CONST(ARITHMETIC_OVERFLOW));

  return 0;
}

static int Test_FloatConversions(void) {
  float f = 0.0f;
  double d = 0.0;
  long double ld = 0.0L;
  int64_t i64 = 0;
  int32_t i32 = 0;
  int16_t i16 = 0;
  int8_t i8 = 0;
  uint64_t u64 = 0;
  uint32_t u32 = 0;
  uint16_t u16 = 0;
  uint8_t u8 = 0;
  size_t sz = 0;

  /* 1. Float to Double / LongDouble (infallible widening) */
  CHECK_STATUS(CAST_FUNC(FloattoDouble)(3.25f, &d), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabs(d - 3.25) < 1e-6);

  CHECK_STATUS(CAST_FUNC(FloattoLongDouble)(3.25f, &ld), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabsl(ld - 3.25L) < 1e-6L);

  /* 2. Double to Float (range checked downcast) & to LongDouble */
  CHECK_STATUS(CAST_FUNC(DoubletoFloat)(3.25, &f), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabsf(f - 3.25f) < 1e-6f);

  CHECK_STATUS(CAST_FUNC(DoubletoLongDouble)(99.5, &ld), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabsl(ld - 99.5L) < 1e-9L);

  CHECK_STATUS(CAST_FUNC(DoubletoFloat)(1e39, &f),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(DoubletoFloat)(-1e39, &f),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(DoubletoFloat)(NAN, &f),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(DoubletoFloat)(INFINITY, &f),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(DoubletoFloat)(-INFINITY, &f),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* 3. LongDouble downcasts and integer conversions */
  CHECK_STATUS(CAST_FUNC(LongDoubletoFloat)(42.5L, &f), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabsf(f - 42.5f) < 1e-6f);

  CHECK_STATUS(CAST_FUNC(LongDoubletoDouble)(12345.678L, &d), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabs(d - 12345.678) < 1e-9);

  CHECK_STATUS(CAST_FUNC(LongDoubletoI32)(999.0L, &i32), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 999);

  CHECK_STATUS(CAST_FUNC(LongDoubletoU32)(1234.0L, &u32), STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32 == 1234U);

  CHECK_STATUS(CAST_FUNC(LongDoubletoI32)(NAN, &i32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(LongDoubletoU32)(-1.0L, &u32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* 4. Float to Signed Integers */
  CHECK_STATUS(CAST_FUNC(FloattoI32)(123.0f, &i32), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 123);

  CHECK_STATUS(CAST_FUNC(FloattoI32)(-50.0f, &i32), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == -50);

  CHECK_STATUS(CAST_FUNC(FloattoI32)(3e9f, &i32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(FloattoI32)(-3e9f, &i32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(FloattoI32)(NAN, &i32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(FloattoI8)(120.0f, &i8), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == 120);
  CHECK_STATUS(CAST_FUNC(FloattoI8)(200.0f, &i8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(FloattoI8)(-150.0f, &i8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(FloattoI16)(30000.0f, &i16), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i16 == 30000);
  CHECK_STATUS(CAST_FUNC(FloattoI16)(40000.0f, &i16),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(FloattoI64)(1000000000.0f, &i64), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i64 == 1000000000LL);

  /* 4. Float to Unsigned Integers */
  CHECK_STATUS(CAST_FUNC(FloattoU32)(500.0f, &u32), STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32 == 500U);

  CHECK_STATUS(CAST_FUNC(FloattoU32)(-1.0f, &u32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(FloattoU32)(5e9f, &u32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(FloattoU32)(NAN, &u32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(FloattoU8)(250.0f, &u8), STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8 == 250U);
  CHECK_STATUS(CAST_FUNC(FloattoU8)(300.0f, &u8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(FloattoU8)(-0.5f, &u8),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(FloattoU16)(60000.0f, &u16), STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16 == 60000U);
  CHECK_STATUS(CAST_FUNC(FloattoU16)(70000.0f, &u16),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(FloattoU64)(12345.0f, &u64), STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64 == 12345ULL);
  CHECK_STATUS(CAST_FUNC(FloattoU64)(-5.0f, &u64),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(FloattoSize)(1024.0f, &sz), STATUS_CONST(SUCCESS));
  CHECK_TRUE(sz == 1024U);
  CHECK_STATUS(CAST_FUNC(FloattoSize)(-1.0f, &sz),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* 5. Double to Integers */
  CHECK_STATUS(CAST_FUNC(DoubletoI64)(5000000000.0, &i64), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i64 == 5000000000LL);
  CHECK_STATUS(CAST_FUNC(DoubletoI64)(1e20, &i64),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(DoubletoI64)(-1e20, &i64),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(DoubletoI64)(NAN, &i64),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(DoubletoI32)(42.0, &i32), STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 42);
  CHECK_STATUS(CAST_FUNC(DoubletoI32)(3e9, &i32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(DoubletoU64)(10000000000.0, &u64), STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64 == 10000000000ULL);
  CHECK_STATUS(CAST_FUNC(DoubletoU64)(-1.0, &u64),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_STATUS(CAST_FUNC(DoubletoU64)(1e20, &u64),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(DoubletoU32)(9999.0, &u32), STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32 == 9999U);
  CHECK_STATUS(CAST_FUNC(DoubletoU32)(-0.1, &u32),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(DoubletoSize)(8192.0, &sz), STATUS_CONST(SUCCESS));
  CHECK_TRUE(sz == 8192U);
  CHECK_STATUS(CAST_FUNC(DoubletoSize)(-2.0, &sz),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* 6. Integers to Float / Double */
  CHECK_STATUS(CAST_FUNC(Int32toFloat)(12345, &f), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabsf(f - 12345.0f) < 1e-6f);

  CHECK_STATUS(CAST_FUNC(Int32toDouble)(-67890, &d), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabs(d - (-67890.0)) < 1e-9);

  CHECK_STATUS(CAST_FUNC(Uint32toFloat)(99999U, &f), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabsf(f - 99999.0f) < 1e-6f);

  CHECK_STATUS(CAST_FUNC(Uint64toDouble)(1000000ULL, &d), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabs(d - 1000000.0) < 1e-9);

  CHECK_STATUS(CAST_FUNC(SizetoFloat)(500U, &f), STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabsf(f - 500.0f) < 1e-6f);

  /* 7. Package Call on Float conversions */
  CHECK_STATUS(PACKAGE_CALL(GNS2(LIB_PREFIX(CAST_MOD), Float), FloattoI32, 77.0f, &i32),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 77);

  CHECK_STATUS(PACKAGE_CALL(GNS2(LIB_PREFIX(CAST_MOD), Float), DoubletoFloat, 1.25, &f),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(fabsf(f - 1.25f) < 1e-6f);

  /* 8. NULL destination pointer validation */
  CHECK_STATUS(CAST_FUNC(FloattoI32)(10.0f, NULL),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(CAST_FUNC(DoubletoFloat)(10.0, NULL),
               STATUS_CONST(INVALID_ARGUMENT));
  CHECK_STATUS(CAST_FUNC(Int32toFloat)(10, NULL),
               STATUS_CONST(INVALID_ARGUMENT));

  return 0;
}

int main(void) {
  int result;

  result = Test_MacrosAndLegacy();
  if (result != 0)
    return result;

  result = Test_UnsignedDowncast();
  if (result != 0)
    return result;

  result = Test_SignedDowncast();
  if (result != 0)
    return result;

  result = Test_SignedToUnsigned();
  if (result != 0)
    return result;

  result = Test_UnsignedToSigned();
  if (result != 0)
    return result;

  result = Test_SizeConversions();
  if (result != 0)
    return result;

  result = Test_CustomAndIntegration();
  if (result != 0)
    return result;

  result = Test_FloatConversions();
  if (result != 0)
    return result;

  return 0;
}
