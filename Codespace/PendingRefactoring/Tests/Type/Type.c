#include "../../../Cosmeron/Modules/Type/Type.h"

/* Custom width template instantiations via Dual-Mode Engine */
TEMPLATE_IMPLEMENT(Type_Bigint, 192, uint64_t);
TEMPLATE_IMPLEMENT(Type_Bitset, 100, 100U);

int main(void) {
  /* 1. Primitive Types from Core/Primitive/Types.h */
  MODULE_TYPE(Primitive, Int32) pInt = -42;
  MODULE_TYPE(Primitive, UInt64) pUInt = 100ULL;
  if (pInt != -42 || pUInt != 100ULL)
    return 1;

  /* 2. Tag (16, 32, 64) */
  MODULE_TYPED(Type, Tag, 32) tag = {0};
  if (Type_Tag_32_Init(&tag) != STATUS_CONST(SUCCESS))
    return 2;
  if (OBJECT_CALL(&tag, FromString, "RIFF") != STATUS_CONST(SUCCESS))
    return 3;
  if (OBJECT_CALL_0(&tag, ToUInt) != TYPE_TAG_MAKE_32('R', 'I', 'F', 'F'))
    return 4;
  if (!OBJECT_CALL_0(&tag, IsPrintable))
    return 5;
  char tagBuf[8] = {0};
  if (OBJECT_CALL(&tag, ToString, tagBuf, sizeof(tagBuf)) !=
      STATUS_CONST(SUCCESS))
    return 6;
  if (strcmp(tagBuf, "RIFF") != 0)
    return 7;

  uint8_t beBytes[4] = {0};
  if (OBJECT_CALL(&tag, WriteBE, beBytes) != STATUS_CONST(SUCCESS))
    return 8;
  MODULE_TYPED(Type, Tag, 32) tag2 = {0};
  Type_Tag_32_Init(&tag2);
  if (OBJECT_CALL(&tag2, ReadBE, beBytes) != STATUS_CONST(SUCCESS))
    return 9;
  if (!OBJECT_CALL(&tag, Equal, &tag2))
    return 10;

  /* 3. TBlock (128, 256, 512, 1024) */
  MODULE_TYPED(Type, Block, 128) blk = {0};
  Type_Block_128_Init(&blk);
  if (OBJECT_CALL(&blk, BitSet, 127U) != STATUS_CONST(SUCCESS))
    return 11;
  bool bitSet = false;
  if (OBJECT_CALL(&blk, BitCheck, 127U, &bitSet) != STATUS_CONST(SUCCESS) ||
      !bitSet)
    return 12;
  if (OBJECT_CALL(&blk, RotateLeft, 1U) != STATUS_CONST(SUCCESS))
    return 13;
  if (OBJECT_CALL(&blk, BitCheck, 0U, &bitSet) != STATUS_CONST(SUCCESS) ||
      !bitSet)
    return 14;
  if (OBJECT_CALL_0(&blk, Popcount) != 1U || !OBJECT_CALL_0(&blk, Any) ||
      OBJECT_CALL_0(&blk, All) || OBJECT_CALL_0(&blk, None))
    return 21;
  if (OBJECT_CALL(&blk, BitFlip, 0U) != STATUS_CONST(SUCCESS) ||
      !OBJECT_CALL_0(&blk, None))
    return 22;
  if (OBJECT_CALL_0(&blk, SetAll) != STATUS_CONST(SUCCESS) ||
      !OBJECT_CALL_0(&blk, All) || OBJECT_CALL_0(&blk, Popcount) != 128U)
    return 23;

  /* 3b. TBitset (standard 128 + non-multiple-of-64 custom 100) */
  MODULE_TYPED(Type, Bitset, 100) bsA = {0};
  MODULE_TYPED(Type, Bitset, 100) bsB = {0};
  Type_Bitset_100_Init(&bsA);
  Type_Bitset_100_Init(&bsB);
  if (OBJECT_CALL_0(&bsA, Capacity) != 100U || !OBJECT_CALL_0(&bsA, None))
    return 24;
  if (OBJECT_CALL_0(&bsA, Not) != STATUS_CONST(SUCCESS))
    return 25;
  if (!OBJECT_CALL_0(&bsA, All) || OBJECT_CALL_0(&bsA, Count) != 100U)
    return 26;
  size_t foundBit = 0U;
  if (OBJECT_CALL(&bsA, FindFirstZero, &foundBit) != STATUS_CONST(NOT_FOUND))
    return 27;
  if (OBJECT_CALL_0(&bsA, Clear) != STATUS_CONST(SUCCESS) ||
      OBJECT_CALL(&bsA, SetRange, 10U, 70U) != STATUS_CONST(SUCCESS))
    return 28;
  if (OBJECT_CALL_0(&bsA, Count) != 60U)
    return 29;
  if (OBJECT_CALL(&bsA, FindFirstSet, &foundBit) != STATUS_CONST(SUCCESS) ||
      foundBit != 10U)
    return 30;
  if (OBJECT_CALL(&bsA, FindNextSet, 65U, &foundBit) !=
          STATUS_CONST(SUCCESS) ||
      foundBit != 65U)
    return 31;
  if (OBJECT_CALL(&bsB, SetRange, 20U, 30U) != STATUS_CONST(SUCCESS))
    return 32;
  if (!OBJECT_CALL(&bsB, IsSubset, &bsA) ||
      OBJECT_CALL(&bsB, IsDisjoint, &bsA) ||
      !OBJECT_CALL(&bsB, Intersects, &bsA))
    return 33;

  /* 4. TBigint (128, 256, 512, 1024) */
  MODULE_TYPED(Type, Bigint, 128) bigA = {0};
  MODULE_TYPED(Type, Bigint, 128) bigB = {0};
  Type_Bigint_128_Init(&bigA);
  Type_Bigint_128_Init(&bigB);
  bigA.limb[0] = 100ULL;
  bigB.limb[0] = 23ULL;
  if (OBJECT_CALL(&bigA, Add, &bigB) != STATUS_CONST(SUCCESS) ||
      bigA.limb[0] != 123ULL)
    return 15;
  if (OBJECT_CALL(&bigA, Mul, &bigB) != STATUS_CONST(SUCCESS) ||
      bigA.limb[0] != 2829ULL)
    return 16;
  char numBuf[64] = {0};
  if (OBJECT_CALL(&bigA, ToCString, numBuf, sizeof(numBuf)) !=
      STATUS_CONST(SUCCESS))
    return 17;
  if (strcmp(numBuf, "2829") != 0)
    return 18;

  /* 5. TDecimal (128, 256, 512, 1024) */
  MODULE_TYPED(Type, Decimal, 256) decA = {0};
  MODULE_TYPED(Type, Decimal, 256) decB = {0};
  Type_Decimal_256_Init(&decA);
  Type_Decimal_256_Init(&decB);
  decA.limb[0] = 50ULL;
  decB.limb[0] = 8ULL;
  if (OBJECT_CALL(&decA, Sub, &decB) != STATUS_CONST(SUCCESS) ||
      decA.limb[0] != 42ULL)
    return 19;

  /* 6. Custom 192-bit Bigint instantiation */
  MODULE_TYPED(Type, Bigint, 192) custom = {0};
  Type_Bigint_192_Init(&custom);
  custom.limb[2] = 1ULL;
  if (OBJECT_CALL_0(&custom, IsZero))
    return 20;

  return 0;
}
