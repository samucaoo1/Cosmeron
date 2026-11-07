#include "../../Cosmeron/Core/Primitive/Types.h"

#include <assert.h>
#include <string.h>

#define COUNT_ITEM(ROW) + 1

int main(void) {
  /* 1. Verify Category Enum and ToString */
  PRIMITIVE_TYPE(Category) catBool = PRIMITIVE_CONST(BOOL);
  PRIMITIVE_TYPE(Category) catInt = PRIMITIVE_CONST(INT);
  PRIMITIVE_TYPE(Category) catUInt = PRIMITIVE_CONST(UINT);
  PRIMITIVE_TYPE(Category) catChar = PRIMITIVE_CONST(CHAR);
  PRIMITIVE_TYPE(Category) catFloat = PRIMITIVE_CONST(FLOAT);
  PRIMITIVE_TYPE(Category) catPtr = PRIMITIVE_CONST(PTR);

  if (strcmp(PRIMITIVE_FUNC(CategoryToString)(catBool), "Boolean") != 0) return 1;
  if (strcmp(PRIMITIVE_FUNC(CategoryToString)(catInt), "Signed integer") != 0) return 2;
  if (strcmp(PRIMITIVE_FUNC(CategoryToString)(catUInt), "Unsigned integer") != 0) return 3;
  if (strcmp(PRIMITIVE_FUNC(CategoryToString)(catChar), "Character") != 0) return 4;
  if (strcmp(PRIMITIVE_FUNC(CategoryToString)(catFloat), "Floating-point") != 0) return 5;
  if (strcmp(PRIMITIVE_FUNC(CategoryToString)(catPtr), "Architecture pointer or integer") != 0) return 6;
  if (strcmp(PRIMITIVE_FUNC(CategoryToString)((PRIMITIVE_TYPE(Category))999), "Unknown") != 0) return 7;

  /* 2. Verify Canonical Type Declarations */
  PRIMITIVE_TYPE(TBoolean) b = true;
  PRIMITIVE_TYPE(TInt8) i8 = -8;
  PRIMITIVE_TYPE(TInt16) i16 = -16;
  PRIMITIVE_TYPE(TInt32) i32 = -32;
  PRIMITIVE_TYPE(TInt64) i64 = -64;
  PRIMITIVE_TYPE(TUInt8) u8 = 8;
  PRIMITIVE_TYPE(TUInt16) u16 = 16;
  PRIMITIVE_TYPE(TUInt32) u32 = 32;
  PRIMITIVE_TYPE(TUInt64) u64 = 64;
  PRIMITIVE_TYPE(TChar) ch = 'A';
  PRIMITIVE_TYPE(TChar8) ch8 = 65;
  PRIMITIVE_TYPE(TChar16) ch16 = 65;
  PRIMITIVE_TYPE(TChar32) ch32 = 65;
  PRIMITIVE_TYPE(TFloat32) f32 = 1.0f;
  PRIMITIVE_TYPE(TFloat64) f64 = 2.0;
  PRIMITIVE_TYPE(TFloat) flt = 3.0f;
  PRIMITIVE_TYPE(TDouble) dbl = 4.0;
  PRIMITIVE_TYPE(TLongDouble) ld = 5.0L;
  PRIMITIVE_TYPE(TInt) nativeInt = -1;
  PRIMITIVE_TYPE(TUInt) nativeUInt = 1;
  PRIMITIVE_TYPE(TSize) sz = sizeof(size_t);
  PRIMITIVE_TYPE(TPtrDiff) diff = 10;
  PRIMITIVE_TYPE(TIntPtr) iptr = (intptr_t)&sz;
  PRIMITIVE_TYPE(TUIntPtr) uptr = (uintptr_t)&sz;

  (void)b; (void)i8; (void)i16; (void)i32; (void)i64;
  (void)u8; (void)u16; (void)u32; (void)u64;
  (void)ch; (void)ch8; (void)ch16; (void)ch32;
  (void)f32; (void)f64; (void)flt; (void)dbl; (void)ld;
  (void)nativeInt; (void)nativeUInt; (void)sz; (void)diff;
  (void)iptr; (void)uptr;

  /* 3. Verify Table Counts through Category Views */
  const int countBool = 0 PRIMITIVE_FOR_EACH_BOOLEAN(COUNT_ITEM);
  const int countSigned = 0 PRIMITIVE_FOR_EACH_SIGNED(COUNT_ITEM);
  const int countUnsigned = 0 PRIMITIVE_FOR_EACH_UNSIGNED(COUNT_ITEM);
  const int countExactInt = 0 PRIMITIVE_FOR_EACH_EXACT_INTEGER(COUNT_ITEM);
  const int countChar = 0 PRIMITIVE_FOR_EACH_CHARACTER(COUNT_ITEM);
  const int countFloat = 0 PRIMITIVE_FOR_EACH_FLOAT(COUNT_ITEM);
  const int countArch = 0 PRIMITIVE_FOR_EACH_ARCHITECTURE(COUNT_ITEM);
  const int countInt = 0 PRIMITIVE_FOR_EACH_INTEGER(COUNT_ITEM);
  const int countNumeric = 0 PRIMITIVE_FOR_EACH_NUMERIC(COUNT_ITEM);
  const int countTotal = 0 PRIMITIVE_FOR_EACH(COUNT_ITEM);

  if (countBool != 1) return 10;
  if (countSigned != 4) return 11;
  if (countUnsigned != 4) return 12;
  if (countExactInt != 8) return 13;
  if (countChar != 4) return 14;
  if (countFloat != 5) return 15;
  if (countArch != 6) return 16;
  if (countInt != 14) return 17;
  if (countNumeric != 19) return 18;
  if (countTotal != 24) return 19;

  return 0;
}

