#include "../../Cosmeron/Core/Cast/Integral.h"

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

static int Test_MacrosAndLegacy(void) {
  CAST_TYPE(Value) value = CAST_TYPE_TO_STRUCT(int, Value)(7);
  CHECK_TRUE(CAST_FUNC(Identity)(value.value) == 7);
  CHECK_TRUE(CAST_STRUCT_TO_TYPE(Value, int)(value) == 7);
  return 0;
}

static int Test_UnsignedDowncast(void) {
  uint32_t u32 = 0;
  uint16_t u16 = 0;
  uint8_t u8 = 0;

  /* Null pointer check */
  CHECK_STATUS(CAST_FUNC(UInt64ToUInt32)(NULL, 100),
               STATUS_CONST(INVALID_ARGUMENT));

  /* UInt64 to UInt32 */
  CHECK_STATUS(CAST_FUNC(UInt64ToUInt32)(&u32, 0x12345678ULL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32 == 0x12345678U);

  u32 = 0xAA;
  CHECK_STATUS(CAST_FUNC(UInt64ToUInt32)(&u32, 0x100000000ULL),
               STATUS_CONST(ARITHMETIC_OVERFLOW));
  CHECK_TRUE(u32 == 0xAA); /* outValue untouched on failure */

  /* UInt32 to UInt16 */
  CHECK_STATUS(CAST_FUNC(UInt32ToUInt16)(&u16, 0xFFFFU),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16 == 0xFFFFU);

  CHECK_STATUS(CAST_FUNC(UInt32ToUInt16)(&u16, 0x10000U),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* UInt16 to UInt8 */
  CHECK_STATUS(CAST_FUNC(UInt16ToUInt8)(&u8, 0xFFU),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8 == 0xFFU);

  CHECK_STATUS(CAST_FUNC(UInt16ToUInt8)(&u8, 0x100U),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* UInt64 to UInt8 */
  CHECK_STATUS(CAST_FUNC(UInt64ToUInt8)(&u8, 42ULL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8 == 42U);

  CHECK_STATUS(CAST_FUNC(UInt64ToUInt8)(&u8, 256ULL),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  return 0;
}

static int Test_SignedDowncast(void) {
  int32_t i32 = 0;
  int16_t i16 = 0;
  int8_t i8 = 0;

  /* Int64 to Int32 */
  CHECK_STATUS(CAST_FUNC(Int64ToInt32)(&i32, -1234567LL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == -1234567);

  CHECK_STATUS(CAST_FUNC(Int64ToInt32)(&i32, 2147483647LL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 2147483647);

  CHECK_STATUS(CAST_FUNC(Int64ToInt32)(&i32, 2147483648LL),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int64ToInt32)(&i32, -2147483648LL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == (-2147483647 - 1));

  CHECK_STATUS(CAST_FUNC(Int64ToInt32)(&i32, -2147483649LL),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Int32 to Int16 */
  CHECK_STATUS(CAST_FUNC(Int32ToInt16)(&i16, 32767),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i16 == 32767);

  CHECK_STATUS(CAST_FUNC(Int32ToInt16)(&i16, 32768),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int32ToInt16)(&i16, -32768),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i16 == -32768);

  CHECK_STATUS(CAST_FUNC(Int32ToInt16)(&i16, -32769),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Int16 to Int8 */
  CHECK_STATUS(CAST_FUNC(Int16ToInt8)(&i8, 127),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == 127);

  CHECK_STATUS(CAST_FUNC(Int16ToInt8)(&i8, 128),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int16ToInt8)(&i8, -128),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == -128);

  CHECK_STATUS(CAST_FUNC(Int16ToInt8)(&i8, -129),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  return 0;
}

static int Test_SignedToUnsigned(void) {
  uint64_t u64 = 0;
  uint32_t u32 = 0;
  uint16_t u16 = 0;
  uint8_t u8 = 0;

  /* Int64 to UInt64 */
  CHECK_STATUS(CAST_FUNC(Int64ToUInt64)(&u64, 100LL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64 == 100ULL);

  CHECK_STATUS(CAST_FUNC(Int64ToUInt64)(&u64, -1LL),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Int64 to UInt32 */
  CHECK_STATUS(CAST_FUNC(Int64ToUInt32)(&u32, 0x7FFFFFFFLL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32 == 0x7FFFFFFFU);

  CHECK_STATUS(CAST_FUNC(Int64ToUInt32)(&u32, -5LL),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int64ToUInt32)(&u32, 0x100000000LL),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Int32 to UInt16 */
  CHECK_STATUS(CAST_FUNC(Int32ToUInt16)(&u16, 65535),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u16 == 65535U);

  CHECK_STATUS(CAST_FUNC(Int32ToUInt16)(&u16, -1),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int32ToUInt16)(&u16, 65536),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Int16 to UInt8 */
  CHECK_STATUS(CAST_FUNC(Int16ToUInt8)(&u8, 255),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u8 == 255U);

  CHECK_STATUS(CAST_FUNC(Int16ToUInt8)(&u8, -1),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int16ToUInt8)(&u8, 256),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Small signed to large unsigned (Int8 to UInt64) */
  CHECK_STATUS(CAST_FUNC(Int8ToUInt64)(&u64, 42),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u64 == 42ULL);

  CHECK_STATUS(CAST_FUNC(Int8ToUInt64)(&u64, -42),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  return 0;
}

static int Test_UnsignedToSigned(void) {
  int64_t i64 = 0;
  int32_t i32 = 0;
  int16_t i16 = 0;
  int8_t i8 = 0;

  /* UInt64 to Int64 */
  CHECK_STATUS(CAST_FUNC(UInt64ToInt64)(&i64, 0x7FFFFFFFFFFFFFFFULL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i64 == 0x7FFFFFFFFFFFFFFFLL);

  CHECK_STATUS(CAST_FUNC(UInt64ToInt64)(&i64, 0x8000000000000000ULL),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* UInt32 to Int32 */
  CHECK_STATUS(CAST_FUNC(UInt32ToInt32)(&i32, 0x7FFFFFFFU),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 0x7FFFFFFF);

  CHECK_STATUS(CAST_FUNC(UInt32ToInt32)(&i32, 0x80000000U),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* UInt16 to Int16 */
  CHECK_STATUS(CAST_FUNC(UInt16ToInt16)(&i16, 32767U),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i16 == 32767);

  CHECK_STATUS(CAST_FUNC(UInt16ToInt16)(&i16, 32768U),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* UInt8 to Int8 */
  CHECK_STATUS(CAST_FUNC(UInt8ToInt8)(&i8, 127U),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i8 == 127);

  CHECK_STATUS(CAST_FUNC(UInt8ToInt8)(&i8, 128U),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  /* Large unsigned to small signed (UInt64 to Int32) */
  CHECK_STATUS(CAST_FUNC(UInt64ToInt32)(&i32, 500ULL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 500);

  CHECK_STATUS(CAST_FUNC(UInt64ToInt32)(&i32, 0x80000000ULL),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  return 0;
}

static int Test_SizeConversions(void) {
  size_t sz = 0;
  uint32_t u32 = 0;
  uint8_t u8 = 0;
  int32_t i32 = 0;

  CHECK_STATUS(CAST_FUNC(SizeToUInt32)(&u32, (size_t)1000),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(u32 == 1000U);

  CHECK_STATUS(CAST_FUNC(SizeToUInt8)(&u8, (size_t)300),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(SizeToInt32)(&i32, (size_t)2147483647),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(i32 == 2147483647);

  CHECK_STATUS(CAST_FUNC(UInt64ToSize)(&sz, 500ULL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(sz == 500U);

  CHECK_STATUS(CAST_FUNC(Int64ToSize)(&sz, 100LL),
               STATUS_CONST(SUCCESS));
  CHECK_TRUE(sz == 100U);

  CHECK_STATUS(CAST_FUNC(Int64ToSize)(&sz, -1LL),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

  CHECK_STATUS(CAST_FUNC(Int32ToSize)(&sz, -1),
               STATUS_CONST(ARITHMETIC_OVERFLOW));

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

  return 0;
}
