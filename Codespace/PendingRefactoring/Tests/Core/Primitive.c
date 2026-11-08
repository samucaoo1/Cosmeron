#include "../../../Cosmeron/Core/Primitive/Types.h"

#include <assert.h>
#include <string.h>

#define COUNT_ITEM(ROW) + 1

int main(void) {
  /* 1. Verify Category Enum and ToString */
  MODULE_ENUM(Primitive, Category) catBool = MODULE_CONST(Primitive, BOOL);
  MODULE_ENUM(Primitive, Category) catInt = MODULE_CONST(Primitive, INT);
  MODULE_ENUM(Primitive, Category) catUInt = MODULE_CONST(Primitive, UINT);
  MODULE_ENUM(Primitive, Category) catChar = MODULE_CONST(Primitive, CHAR);
  MODULE_ENUM(Primitive, Category) catFloat = MODULE_CONST(Primitive, FLOAT);
  MODULE_ENUM(Primitive, Category) catPtr = MODULE_CONST(Primitive, PTR);

  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catBool), "Boolean") != 0) return 1;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catInt), "Signed integer") != 0) return 2;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catUInt), "Unsigned integer") != 0) return 3;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catChar), "Character") != 0) return 4;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catFloat), "Floating-point") != 0) return 5;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catPtr), "Architecture pointer or integer") != 0) return 6;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)((MODULE_ENUM(Primitive, Category))999), "Unknown") != 0) return 7;

  /* 2. Verify Canonical Type Declarations */
  MODULE_TYPE(Primitive, Boolean) b = true;
  MODULE_TYPE(Primitive, Int8) i8 = -8;
  MODULE_TYPE(Primitive, Int16) i16 = -16;
  MODULE_TYPE(Primitive, Int32) i32 = -32;
  MODULE_TYPE(Primitive, Int64) i64 = -64;
  MODULE_TYPE(Primitive, UInt8) u8 = 8;
  MODULE_TYPE(Primitive, UInt16) u16 = 16;
  MODULE_TYPE(Primitive, UInt32) u32 = 32;
  MODULE_TYPE(Primitive, UInt64) u64 = 64;
  MODULE_TYPE(Primitive, Char) ch = 'A';
  MODULE_TYPE(Primitive, Char8) ch8 = 65;
  MODULE_TYPE(Primitive, Char16) ch16 = 65;
  MODULE_TYPE(Primitive, Char32) ch32 = 65;
  MODULE_TYPE(Primitive, Float32) f32 = 1.0f;
  MODULE_TYPE(Primitive, Float64) f64 = 2.0;
  MODULE_TYPE(Primitive, Float) flt = 3.0f;
  MODULE_TYPE(Primitive, Double) dbl = 4.0;
  MODULE_TYPE(Primitive, LongDouble) ld = 5.0L;
  MODULE_TYPE(Primitive, Int) nativeInt = -1;
  MODULE_TYPE(Primitive, UInt) nativeUInt = 1;
  MODULE_TYPE(Primitive, Size) sz = sizeof(size_t);
  MODULE_TYPE(Primitive, PtrDiff) diff = 10;
  MODULE_TYPE(Primitive, IntPtr) iptr = (intptr_t)&sz;
  MODULE_TYPE(Primitive, UIntPtr) uptr = (uintptr_t)&sz;

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
