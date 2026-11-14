#include "../../../Cosmeron/Core/Primitive/Types.h"
#include "../../../Cosmeron/Core/Primitive/Traits.h"
#include "../../../Cosmeron/Core/Primitive/TaggedUnion.h"
#include "../../../Cosmeron/Core/Primitive/Any.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define COUNT_ITEM(ROW) + 1

/* ========================================================================== */
/* Mock Extended Dynamic Type: TBigInt (Simulating arbitrary-precision int)   */
/* ========================================================================== */

MODULE(DOMAIN(Core), NumericMock,
    STRUCT(BigInt,
        FIELDS(
            int32_t sign;
            size_t limbCount;
            uint32_t *limbs;
        )
    )
);

static OPSTATUS NumericMock_BigInt_TraitInit(void *instance) {
  if (!instance)
    return STATUS_CONST(INVALID_ARGUMENT);
  MODULE_TYPE(NumericMock, BigInt) *big =
      (MODULE_TYPE(NumericMock, BigInt) *)instance;
  big->sign = 0;
  big->limbCount = 1U;
  big->limbs = (uint32_t *)calloc(1U, sizeof(uint32_t));
  if (!big->limbs)
    return STATUS_CONST(OUT_OF_MEMORY);
  return STATUS_CONST(SUCCESS);
}

static void NumericMock_BigInt_TraitDispose(void *instance) {
  if (!instance)
    return;
  MODULE_TYPE(NumericMock, BigInt) *big =
      (MODULE_TYPE(NumericMock, BigInt) *)instance;
  if (big->limbs) {
    free(big->limbs);
    big->limbs = NULL;
  }
  big->sign = 0;
  big->limbCount = 0U;
}

static OPSTATUS NumericMock_BigInt_TraitCopy(void *destination,
                                             const void *source) {
  if (!destination || !source)
    return STATUS_CONST(INVALID_ARGUMENT);
  MODULE_TYPE(NumericMock, BigInt) *dest =
      (MODULE_TYPE(NumericMock, BigInt) *)destination;
  const MODULE_TYPE(NumericMock, BigInt) *src =
      (const MODULE_TYPE(NumericMock, BigInt) *)source;

  dest->sign = src->sign;
  dest->limbCount = src->limbCount;
  if (src->limbCount > 0U && src->limbs != NULL) {
    dest->limbs = (uint32_t *)malloc(src->limbCount * sizeof(uint32_t));
    if (!dest->limbs)
      return STATUS_CONST(OUT_OF_MEMORY);
    memcpy(dest->limbs, src->limbs, src->limbCount * sizeof(uint32_t));
  } else {
    dest->limbs = NULL;
  }
  return STATUS_CONST(SUCCESS);
}

static void NumericMock_BigInt_TraitMove(void *destination, void *source) {
  if (!destination || !source || destination == source)
    return;
  MODULE_TYPE(NumericMock, BigInt) *dest =
      (MODULE_TYPE(NumericMock, BigInt) *)destination;
  MODULE_TYPE(NumericMock, BigInt) *src =
      (MODULE_TYPE(NumericMock, BigInt) *)source;

  *dest = *src;
  src->sign = 0;
  src->limbCount = 0U;
  src->limbs = NULL;
}

static CMPOUT NumericMock_BigInt_TraitCompare(const void *left,
                                              const void *right) {
  const MODULE_TYPE(NumericMock, BigInt) *a =
      (const MODULE_TYPE(NumericMock, BigInt) *)left;
  const MODULE_TYPE(NumericMock, BigInt) *b =
      (const MODULE_TYPE(NumericMock, BigInt) *)right;

  if (a->sign != b->sign) {
    return a->sign < b->sign ? COMPARISON_CONST(LOWER)
                             : COMPARISON_CONST(HIGHER);
  }
  if (a->sign == 0)
    return COMPARISON_CONST(EQUAL);

  CMPOUT magCmp = COMPARISON_CONST(EQUAL);
  if (a->limbCount != b->limbCount) {
    magCmp = a->limbCount < b->limbCount ? COMPARISON_CONST(LOWER)
                                         : COMPARISON_CONST(HIGHER);
  } else {
    for (size_t i = a->limbCount; i > 0U; --i) {
      uint32_t la = a->limbs[i - 1U];
      uint32_t lb = b->limbs[i - 1U];
      if (la != lb) {
        magCmp = la < lb ? COMPARISON_CONST(LOWER) : COMPARISON_CONST(HIGHER);
        break;
      }
    }
  }

  if (a->sign < 0) {
    if (magCmp == COMPARISON_CONST(LOWER))
      return COMPARISON_CONST(HIGHER);
    if (magCmp == COMPARISON_CONST(HIGHER))
      return COMPARISON_CONST(LOWER);
  }
  return magCmp;
}

static size_t NumericMock_BigInt_TraitHash(const void *instance) {
  const MODULE_TYPE(NumericMock, BigInt) *big =
      (const MODULE_TYPE(NumericMock, BigInt) *)instance;
  if (!big || big->sign == 0 || !big->limbs || big->limbCount == 0U)
    return 0U;
  size_t h = MODULE_FUNC(Primitive, Traits, HashBytes)(
      &big->sign, sizeof(big->sign));
  h ^= MODULE_FUNC(Primitive, Traits, HashBytes)(
      big->limbs, big->limbCount * sizeof(uint32_t));
  return h;
}

static const MODULE_TYPE(Primitive, Traits) *NumericMock_BigInt_Traits(void) {
  static const MODULE_TYPE(Primitive, Traits) descriptor = {
      .name = "NumericMock_TBigInt",
      .size = sizeof(MODULE_TYPE(NumericMock, BigInt)),
      .alignment = _Alignof(MODULE_TYPE(NumericMock, BigInt)),
      .category = MODULE_CONST(Primitive, EXTENDED_NUMERIC),
      .flags = MODULE_CONST(Primitive, TraitFlag, NEEDS_DISPOSE) |
               MODULE_CONST(Primitive, TraitFlag, IS_NUMERIC) |
               MODULE_CONST(Primitive, TraitFlag, IS_SIGNED) |
               MODULE_CONST(Primitive, TraitFlag, IS_INTEGER) |
               MODULE_CONST(Primitive, TraitFlag, IS_DYNAMIC),
      .init = NumericMock_BigInt_TraitInit,
      .dispose = NumericMock_BigInt_TraitDispose,
      .copy = NumericMock_BigInt_TraitCopy,
      .move = NumericMock_BigInt_TraitMove,
      .compare = NumericMock_BigInt_TraitCompare,
      .hash = NumericMock_BigInt_TraitHash};
  return &descriptor;
}

int main(void) {
  /* 1. Verify Category Enum and ToString */
  MODULE_ENUM(Primitive, Category) catBool = MODULE_CONST(Primitive, BOOL);
  MODULE_ENUM(Primitive, Category) catInt = MODULE_CONST(Primitive, INT);
  MODULE_ENUM(Primitive, Category) catUInt = MODULE_CONST(Primitive, UINT);
  MODULE_ENUM(Primitive, Category) catChar = MODULE_CONST(Primitive, CHAR);
  MODULE_ENUM(Primitive, Category) catFloat = MODULE_CONST(Primitive, FLOAT);
  MODULE_ENUM(Primitive, Category) catPtr = MODULE_CONST(Primitive, PTR);
  MODULE_ENUM(Primitive, Category) catExtNum = MODULE_CONST(Primitive, EXTENDED_NUMERIC);
  MODULE_ENUM(Primitive, Category) catComp = MODULE_CONST(Primitive, COMPOSITE);
  MODULE_ENUM(Primitive, Category) catObj = MODULE_CONST(Primitive, OBJECT);

  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catBool), "Boolean") != 0) return 1;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catInt), "Signed integer") != 0) return 2;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catUInt), "Unsigned integer") != 0) return 3;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catChar), "Character") != 0) return 4;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catFloat), "Floating-point") != 0) return 5;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catPtr), "Architecture pointer or integer") != 0) return 6;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catExtNum), "Extended or arbitrary-precision numeric") != 0) return 6;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catComp), "Composite structure or value type") != 0) return 6;
  if (strcmp(MODULE_FUNC(Primitive, Category, ToString)(catObj), "Managed or stateful object") != 0) return 6;
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
  const int countComplex = 0 PRIMITIVE_FOR_EACH_COMPLEX(COUNT_ITEM);
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
#if !COMPILER_MSVC
  if (countComplex != 3) return 20;
  if (countArch != 6) return 16;
  if (countInt != 14) return 17;
  if (countNumeric != 22) return 18;
  if (countTotal != 27) return 19;
#else
  if (countComplex != 0) return 20;
  if (countArch != 6) return 16;
  if (countInt != 14) return 17;
  if (countNumeric != 19) return 18;
  if (countTotal != 24) return 19;
#endif

  /* 4. Verify Static Trait Descriptors for Built-in Primitives */
  const MODULE_TYPE(Primitive, Traits) *tInt32 =
      MODULE_FUNC(Primitive, Traits, Int32)();
  const MODULE_TYPE(Primitive, Traits) *tUInt64 =
      MODULE_FUNC(Primitive, Traits, UInt64)();
  const MODULE_TYPE(Primitive, Traits) *tFloat64 =
      MODULE_FUNC(Primitive, Traits, Float64)();
  const MODULE_TYPE(Primitive, Traits) *tBool =
      MODULE_FUNC(Primitive, Traits, Boolean)();
  const MODULE_TYPE(Primitive, Traits) *tCString =
      MODULE_FUNC(Primitive, Traits, CString)();

  if (!MODULE_FUNC(Primitive, Traits, IsValid)(tInt32)) return 20;
  if (!MODULE_FUNC(Primitive, Traits, IsEqual)(
          tInt32, MODULE_FUNC(Primitive, Traits, Int32)())) return 21;
  if (MODULE_FUNC(Primitive, Traits, IsEqual)(tInt32, tUInt64)) return 22;

  /* Verify Int32 traits introspection */
  if (strcmp(tInt32->name, "TInt32") != 0) return 23;
  if (tInt32->size != sizeof(int32_t)) return 24;
  if (!MODULE_FUNC(Primitive, Traits, IsTrivial)(tInt32)) return 25;
  if (!MODULE_FUNC(Primitive, Traits, IsTrivialCopy)(tInt32)) return 26;
  if (!MODULE_FUNC(Primitive, Traits, IsTrivialCompare)(tInt32)) return 27;
  if (MODULE_FUNC(Primitive, Traits, NeedsDispose)(tInt32)) return 28;
  if (!MODULE_FUNC(Primitive, Traits, IsNumeric)(tInt32)) return 29;
  if (!MODULE_FUNC(Primitive, Traits, IsSigned)(tInt32)) return 30;
  if (!MODULE_FUNC(Primitive, Traits, IsInteger)(tInt32)) return 31;
  if (MODULE_FUNC(Primitive, Traits, IsFloat)(tInt32)) return 32;
  if (MODULE_FUNC(Primitive, Traits, IsDynamic)(tInt32)) return 33;

  /* Verify UInt64 & Float64 & Boolean introspection */
  if (MODULE_FUNC(Primitive, Traits, IsSigned)(tUInt64)) return 34;
  if (!MODULE_FUNC(Primitive, Traits, IsInteger)(tUInt64)) return 35;
  if (!MODULE_FUNC(Primitive, Traits, IsFloat)(tFloat64)) return 36;
  if (!MODULE_FUNC(Primitive, Traits, IsSigned)(tFloat64)) return 37;
  if (MODULE_FUNC(Primitive, Traits, IsTrivialCompare)(tFloat64)) return 38;
  if (MODULE_FUNC(Primitive, Traits, IsNumeric)(tBool)) return 39;

  /* Verify Primitive value operations through Traits */
  int32_t valA = 42;
  int32_t valB = 0;
  if (MODULE_FUNC(Primitive, Traits, Init)(tInt32, &valB) !=
      STATUS_CONST(SUCCESS)) return 40;
  if (valB != 0) return 41;
  if (MODULE_FUNC(Primitive, Traits, Copy)(tInt32, &valB, &valA) !=
      STATUS_CONST(SUCCESS)) return 42;
  if (valB != 42) return 43;
  if (!MODULE_FUNC(Primitive, Traits, Equals)(tInt32, &valA, &valB)) return 44;
  if (MODULE_FUNC(Primitive, Traits, Compare)(tInt32, &valA, &valB) !=
      COMPARISON_CONST(EQUAL)) return 45;

  int32_t valC = -10;
  if (MODULE_FUNC(Primitive, Traits, Compare)(tInt32, &valC, &valA) !=
      COMPARISON_CONST(LOWER)) return 46;
  if (MODULE_FUNC(Primitive, Traits, Swap)(tInt32, &valA, &valC) !=
      STATUS_CONST(SUCCESS)) return 47;
  if (valA != -10 || valC != 42) return 48;
  if (MODULE_FUNC(Primitive, Traits, Hash)(tInt32, &valC) == 0U) return 49;

  /* Verify CString Traits */
  const char *s1 = "alpha";
  const char *s2 = "beta";
  if (MODULE_FUNC(Primitive, Traits, Compare)(tCString, &s1, &s2) !=
      COMPARISON_CONST(LOWER)) return 50;
  if (!MODULE_FUNC(Primitive, Traits, Equals)(tCString, &s1, &s1)) return 51;
  if (MODULE_FUNC(Primitive, Traits, Hash)(tCString, &s1) == 0U) return 52;

  /* 5. Verify Custom Dynamic Type Traits (NumericMock_TBigInt) */
  const MODULE_TYPE(Primitive, Traits) *tBigInt = NumericMock_BigInt_Traits();
  if (!MODULE_FUNC(Primitive, Traits, IsValid)(tBigInt)) return 60;
  if (MODULE_FUNC(Primitive, Traits, IsTrivial)(tBigInt)) return 61;
  if (MODULE_FUNC(Primitive, Traits, IsTrivialCopy)(tBigInt)) return 62;
  if (!MODULE_FUNC(Primitive, Traits, NeedsDispose)(tBigInt)) return 63;
  if (!MODULE_FUNC(Primitive, Traits, IsNumeric)(tBigInt)) return 64;
  if (!MODULE_FUNC(Primitive, Traits, IsSigned)(tBigInt)) return 65;
  if (!MODULE_FUNC(Primitive, Traits, IsInteger)(tBigInt)) return 66;
  if (!MODULE_FUNC(Primitive, Traits, IsDynamic)(tBigInt)) return 67;

  MODULE_TYPE(NumericMock, BigInt) big1;
  MODULE_TYPE(NumericMock, BigInt) big2;
  if (MODULE_FUNC(Primitive, Traits, Init)(tBigInt, &big1) !=
      STATUS_CONST(SUCCESS)) return 68;
  if (MODULE_FUNC(Primitive, Traits, Init)(tBigInt, &big2) !=
      STATUS_CONST(SUCCESS)) return 69;

  big1.sign = 1;
  big1.limbs[0] = 100000U;

  /* Deep Copy via Traits */
  MODULE_FUNC(Primitive, Traits, Dispose)(tBigInt, &big2);
  if (MODULE_FUNC(Primitive, Traits, Copy)(tBigInt, &big2, &big1) !=
      STATUS_CONST(SUCCESS)) return 70;
  if (big2.limbs == big1.limbs || big2.limbs == NULL) return 71;
  if (!MODULE_FUNC(Primitive, Traits, Equals)(tBigInt, &big1, &big2)) return 72;
  if (MODULE_FUNC(Primitive, Traits, Hash)(tBigInt, &big1) !=
      MODULE_FUNC(Primitive, Traits, Hash)(tBigInt, &big2)) return 73;

  big2.limbs[0] = 200000U;
  if (MODULE_FUNC(Primitive, Traits, Compare)(tBigInt, &big1, &big2) !=
      COMPARISON_CONST(LOWER)) return 74;

  /* Swap dynamic objects via Move hook in Traits */
  if (MODULE_FUNC(Primitive, Traits, Swap)(tBigInt, &big1, &big2) !=
      STATUS_CONST(SUCCESS)) return 75;
  if (big1.limbs[0] != 200000U || big2.limbs[0] != 100000U) return 76;

  /* Array lifecycle via Traits */
  MODULE_TYPE(NumericMock, BigInt) bigArray[3];
  if (MODULE_FUNC(Primitive, Traits, InitArray)(tBigInt, bigArray, 3U) !=
      STATUS_CONST(SUCCESS)) return 77;
  bigArray[0].sign = 1;
  bigArray[0].limbs[0] = 11U;
  bigArray[1].sign = -1;
  bigArray[1].limbs[0] = 22U;
  bigArray[2].sign = 1;
  bigArray[2].limbs[0] = 33U;

  MODULE_TYPE(NumericMock, BigInt) bigArrayCopy[3];
  if (MODULE_FUNC(Primitive, Traits, CopyArray)(tBigInt, bigArrayCopy, bigArray,
                                                3U) != STATUS_CONST(SUCCESS))
    return 78;
  if (!MODULE_FUNC(Primitive, Traits, Equals)(tBigInt, &bigArray[1],
                                              &bigArrayCopy[1]))
    return 79;

  MODULE_FUNC(Primitive, Traits, DisposeArray)(tBigInt, bigArray, 3U);
  MODULE_FUNC(Primitive, Traits, DisposeArray)(tBigInt, bigArrayCopy, 3U);
  MODULE_FUNC(Primitive, Traits, Dispose)(tBigInt, &big1);
  MODULE_FUNC(Primitive, Traits, Dispose)(tBigInt, &big2);

  /* ======================================================================== */
  /* 6. Verify TaggedUnion (Closed Discriminated Union)                       */
  /* ======================================================================== */
  MODULE_TYPE(Primitive, TaggedUnion) tuA;
  MODULE_TYPE(Primitive, TaggedUnion) tuB;
  MODULE_FUNC(Primitive, TaggedUnion, Init)(&tuA);
  if (!MODULE_FUNC(Primitive, TaggedUnion, IsEmpty)(&tuA)) return 80;
  if (!MODULE_FUNC(Primitive, TaggedUnion, IsTag)(
          &tuA, MODULE_CONST(Primitive, Tag, NONE))) return 81;

  tuA = MODULE_FUNC(Primitive, TaggedUnion, From_Int32)(-42);
  if (MODULE_FUNC(Primitive, TaggedUnion, IsEmpty)(&tuA)) return 82;
  if (!MODULE_FUNC(Primitive, TaggedUnion, IsNumeric)(&tuA)) return 83;
  if (!MODULE_FUNC(Primitive, TaggedUnion, IsSigned)(&tuA)) return 84;
  if (!MODULE_FUNC(Primitive, TaggedUnion, IsInteger)(&tuA)) return 85;
  if (MODULE_FUNC(Primitive, TaggedUnion, IsFloat)(&tuA)) return 86;

  int32_t outI32 = 0;
  if (MODULE_FUNC(Primitive, TaggedUnion, Get_Int32)(&tuA, &outI32) !=
      STATUS_CONST(SUCCESS) || outI32 != -42) return 87;
  uint64_t outU64 = 0U;
  if (MODULE_FUNC(Primitive, TaggedUnion, Get_UInt64)(&tuA, &outU64) !=
      STATUS_CONST(INVALID_ARGUMENT)) return 88;
  if (MODULE_FUNC(Primitive, TaggedUnion, CoerceUInt64)(&tuA, &outU64) !=
      STATUS_CONST(ARITHMETIC_OVERFLOW)) return 89;

  int64_t outI64 = 0;
  if (MODULE_FUNC(Primitive, TaggedUnion, CoerceInt64)(&tuA, &outI64) !=
      STATUS_CONST(SUCCESS) || outI64 != -42) return 90;
  double outF64 = 0.0;
  if (MODULE_FUNC(Primitive, TaggedUnion, CoerceFloat64)(&tuA, &outF64) !=
      STATUS_CONST(SUCCESS) || outF64 != -42.0) return 91;

  /* Overflow check on UInt64 -> Int64 coercion */
  tuB = MODULE_FUNC(Primitive, TaggedUnion, From_UInt64)(UINT64_MAX);
  if (MODULE_FUNC(Primitive, TaggedUnion, CoerceInt64)(&tuB, &outI64) !=
      STATUS_CONST(ARITHMETIC_OVERFLOW)) return 92;

  /* CString and Bytes in TaggedUnion */
  tuA = MODULE_FUNC(Primitive, TaggedUnion, From_CString)("cosmeron");
  const char *outCStr = NULL;
  if (MODULE_FUNC(Primitive, TaggedUnion, Get_CString)(&tuA, &outCStr) !=
      STATUS_CONST(SUCCESS) || strcmp(outCStr, "cosmeron") != 0) return 93;

  const uint8_t rawBytes[4] = {1U, 2U, 3U, 4U};
  tuB = MODULE_FUNC(Primitive, TaggedUnion, From_Bytes)(rawBytes, sizeof(rawBytes));
  MODULE_TYPE(Primitive, ByteSlice) outSlice;
  if (MODULE_FUNC(Primitive, TaggedUnion, Get_Bytes)(&tuB, &outSlice) !=
      STATUS_CONST(SUCCESS) || outSlice.size != 4U) return 94;

  MODULE_TYPE(Primitive, TaggedUnion) tuC =
      MODULE_FUNC(Primitive, TaggedUnion, From_Int32)(10);
  MODULE_TYPE(Primitive, TaggedUnion) tuD =
      MODULE_FUNC(Primitive, TaggedUnion, From_Int32)(20);
  if (MODULE_FUNC(Primitive, TaggedUnion, Compare)(&tuC, &tuD) !=
      COMPARISON_CONST(LOWER)) return 95;
  if (MODULE_FUNC(Primitive, TaggedUnion, Equals)(&tuC, &tuD)) return 96;
  if (MODULE_FUNC(Primitive, TaggedUnion, Hash)(&tuC) == 0U) return 97;

  /* ======================================================================== */
  /* 7. Verify Any (Type-Erased Box with SBO + Heap + Traits)                 */
  /* ======================================================================== */
  MODULE_TYPE(Primitive, Any) anyA;
  MODULE_TYPE(Primitive, Any) anyB;
  MODULE_FUNC(Primitive, Any, Init)(&anyA);
  MODULE_FUNC(Primitive, Any, Init)(&anyB);

  if (!MODULE_FUNC(Primitive, Any, IsEmpty)(&anyA)) return 100;
  if (MODULE_FUNC(Primitive, Any, HasValue)(&anyA)) return 101;

  /* 7.1 SBO Inline storage for primitives */
  if (MODULE_FUNC(Primitive, Any, Set_Int32)(&anyA, 1337) !=
      STATUS_CONST(SUCCESS)) return 102;
  if (!MODULE_FUNC(Primitive, Any, IsInline)(&anyA)) return 103;
  if (!MODULE_FUNC(Primitive, Any, IsType)(&anyA, tInt32)) return 104;

  int32_t anyI32 = 0;
  if (MODULE_FUNC(Primitive, Any, Get_Int32)(&anyA, &anyI32) !=
      STATUS_CONST(SUCCESS) || anyI32 != 1337) return 105;
  if (MODULE_FUNC(Primitive, Any, Get_UInt64)(&anyA, &outU64) !=
      STATUS_CONST(INVALID_ARGUMENT)) return 106;

  /* Borrow without copy */
  const void *borrowedPtr = NULL;
  if (MODULE_FUNC(Primitive, Any, Borrow)(&anyA, tInt32, &borrowedPtr) !=
      STATUS_CONST(SUCCESS) || *(const int32_t *)borrowedPtr != 1337) return 107;

  /* 7.2 Interop: Any <-> TaggedUnion */
  MODULE_TYPE(Primitive, TaggedUnion) roundTripTu;
  if (MODULE_FUNC(Primitive, Any, ToTaggedUnion)(&anyA, &roundTripTu) !=
      STATUS_CONST(SUCCESS)) return 108;
  if (MODULE_FUNC(Primitive, TaggedUnion, Get_Int32)(&roundTripTu, &anyI32) !=
      STATUS_CONST(SUCCESS) || anyI32 != 1337) return 109;

  if (MODULE_FUNC(Primitive, Any, FromTaggedUnion)(&anyB, &tuC) !=
      STATUS_CONST(SUCCESS)) return 110;
  if (MODULE_FUNC(Primitive, Any, Get_Int32)(&anyB, &anyI32) !=
      STATUS_CONST(SUCCESS) || anyI32 != 10) return 111;

  /* 7.3 Heap-allocated dynamic type inside Any (TBigInt: sizeof > 16B) */
  MODULE_TYPE(NumericMock, BigInt) heapBig;
  if (MODULE_FUNC(Primitive, Traits, Init)(tBigInt, &heapBig) !=
      STATUS_CONST(SUCCESS)) return 112;
  heapBig.sign = 1;
  heapBig.limbs[0] = 999999U;

  if (MODULE_FUNC(Primitive, Any, Emplace)(&anyA, tBigInt, &heapBig) !=
      STATUS_CONST(SUCCESS)) return 113;
  if (MODULE_FUNC(Primitive, Any, IsInline)(&anyA)) return 114;
  if (!MODULE_FUNC(Primitive, Any, IsType)(&anyA, tBigInt)) return 115;

  /* Deep Clone of Any holding heap TBigInt */
  if (MODULE_FUNC(Primitive, Any, Clone)(&anyB, &anyA) !=
      STATUS_CONST(SUCCESS)) return 116;
  if (!MODULE_FUNC(Primitive, Any, Equals)(&anyA, &anyB)) return 117;
  if (MODULE_FUNC(Primitive, Any, RawPointer)(&anyA) ==
      MODULE_FUNC(Primitive, Any, RawPointer)(&anyB)) return 118;

  /* Swap inline vs heap */
  if (MODULE_FUNC(Primitive, Any, Set_Float64)(&anyB, 2.5) !=
      STATUS_CONST(SUCCESS)) return 119;
  if (!MODULE_FUNC(Primitive, Any, IsInline)(&anyB)) return 120;
  if (MODULE_FUNC(Primitive, Any, Swap)(&anyA, &anyB) !=
      STATUS_CONST(SUCCESS)) return 121;
  if (!MODULE_FUNC(Primitive, Any, IsInline)(&anyA)) return 122;
  if (MODULE_FUNC(Primitive, Any, IsInline)(&anyB)) return 123;
  if (!MODULE_FUNC(Primitive, Any, IsType)(&anyA, tFloat64)) return 124;
  if (!MODULE_FUNC(Primitive, Any, IsType)(&anyB, tBigInt)) return 125;

  /* Nested Any inside Any (via Any_Traits) */
  MODULE_TYPE(Primitive, Any) outerAny;
  MODULE_FUNC(Primitive, Any, Init)(&outerAny);
  if (MODULE_FUNC(Primitive, Any, Emplace)(
          &outerAny, MODULE_FUNC(Primitive, Any, Traits)(), &anyB) !=
      STATUS_CONST(SUCCESS)) return 126;
  if (MODULE_FUNC(Primitive, Any, ToTaggedUnion)(&outerAny, &roundTripTu) !=
      STATUS_CONST(NOT_SUPPORTED)) return 127;

  MODULE_FUNC(Primitive, Any, Dispose)(&outerAny);
  MODULE_FUNC(Primitive, Any, Dispose)(&anyA);
  MODULE_FUNC(Primitive, Any, Dispose)(&anyB);
  MODULE_FUNC(Primitive, Traits, Dispose)(tBigInt, &heapBig);

  /* ======================================================================== */
  /* 8. Verify SBO-Sized Any32 / Any64 / Any128 / Any256 & AnyConversion      */
  /* ======================================================================== */
  if (MODULE_FUNC(Primitive, Any, Capacity)() != 16U) return 130;
  if (MODULE_FUNC(Primitive, Any32, Capacity)() != 32U) return 131;
  if (MODULE_FUNC(Primitive, Any64, Capacity)() != 64U) return 132;
  if (MODULE_FUNC(Primitive, Any128, Capacity)() != 128U) return 133;
  if (MODULE_FUNC(Primitive, Any256, Capacity)() != 256U) return 134;

  MODULE_TYPE(Primitive, Any32) any32;
  MODULE_TYPE(Primitive, Any64) any64;
  MODULE_TYPE(Primitive, Any128) any128;
  MODULE_TYPE(Primitive, Any256) any256;
  MODULE_FUNC(Primitive, Any32, Init)(&any32);
  MODULE_FUNC(Primitive, Any64, Init)(&any64);
  MODULE_FUNC(Primitive, Any128, Init)(&any128);
  MODULE_FUNC(Primitive, Any256, Init)(&any256);

  /* TBigInt (24B) fits inline in Any32, but requires heap in default Any (16B) */
  MODULE_TYPE(NumericMock, BigInt) bigForSbo;
  if (MODULE_FUNC(Primitive, Traits, Init)(tBigInt, &bigForSbo) !=
      STATUS_CONST(SUCCESS)) return 135;
  bigForSbo.sign = 1;
  bigForSbo.limbs[0] = 777777U;

  if (MODULE_FUNC(Primitive, Any32, Emplace)(&any32, tBigInt, &bigForSbo) !=
      STATUS_CONST(SUCCESS)) return 136;
  if (!MODULE_FUNC(Primitive, Any32, IsInline)(&any32)) return 137;

  /* Convert Any32 (inline) -> Any (16B, auto-promoted to heap) */
  MODULE_TYPE(Primitive, Any) any16;
  MODULE_FUNC(Primitive, Any, Init)(&any16);
  if (Primitive_Any_Conversion_Clone(&any16, &any32) !=
      STATUS_CONST(SUCCESS)) return 138;
  if (MODULE_FUNC(Primitive, Any, IsInline)(&any16)) return 139;
  if (!MODULE_FUNC(Primitive, Any, IsType)(&any16, tBigInt)) return 140;

  /* Move Any (heap) -> Any64 (auto-demoted to inline, source emptied) */
  if (Primitive_Any_Conversion_Move(&any64, &any16) !=
      STATUS_CONST(SUCCESS)) return 141;
  if (!MODULE_FUNC(Primitive, Any, IsEmpty)(&any16)) return 142;
  if (!MODULE_FUNC(Primitive, Any64, IsInline)(&any64)) return 143;

  /* Move Any64 -> Any128 -> Any256 */
  if (Primitive_Any_Conversion_Move(&any128, &any64) !=
      STATUS_CONST(SUCCESS)) return 144;
  if (!MODULE_FUNC(Primitive, Any64, IsEmpty)(&any64)) return 145;
  if (!MODULE_FUNC(Primitive, Any128, IsInline)(&any128)) return 146;

  if (Primitive_Any_Conversion_Clone(&any256, &any128) !=
      STATUS_CONST(SUCCESS)) return 147;
  if (!MODULE_FUNC(Primitive, Any256, IsInline)(&any256)) return 148;

  const void *p32 = MODULE_FUNC(Primitive, Any32, RawPointer)(&any32);
  const void *p256 = MODULE_FUNC(Primitive, Any256, RawPointer)(&any256);
  if (!MODULE_FUNC(Primitive, Traits, Equals)(tBigInt, p32, p256)) return 149;

  MODULE_FUNC(Primitive, Any, Dispose)(&any16);
  MODULE_FUNC(Primitive, Any32, Dispose)(&any32);
  MODULE_FUNC(Primitive, Any64, Dispose)(&any64);
  MODULE_FUNC(Primitive, Any128, Dispose)(&any128);
  MODULE_FUNC(Primitive, Any256, Dispose)(&any256);
  MODULE_FUNC(Primitive, Traits, Dispose)(tBigInt, &bigForSbo);

  return 0;
}
