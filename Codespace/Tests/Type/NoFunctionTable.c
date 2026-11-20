#define TYPE_DISABLE_FUNCTION_TABLE 1
#define MEMORY_BLOCK_DISABLE_FUNCTION_TABLE 1
#define BITSET_DISABLE_FUNCTION_TABLE 1
#define NUMERIC_DISABLE_FUNCTION_TABLE 1

#include "../../Cosmeron/Core/Bit/Bitset.h"
#include "../../Cosmeron/Core/Memory/Block.h"
#include "../../Cosmeron/Core/Types/ByteTag.h"
#include "../../Cosmeron/Modules/Numeric/Bigint.h"
#include "../../Cosmeron/Modules/Numeric/Decimal.h"

int main(void) {
  /* Verify exact POD-like sizes with function tables disabled */
  struct ExpectedByteTag32Layout {
    uint32_t value;
    char text[5];
  };
  if (sizeof(TType_ByteTag32) != sizeof(struct ExpectedByteTag32Layout))
    return 1;
  if (sizeof(TMemory_Block128) != 16U)
    return 2;
  if (sizeof(TBitset128) != 16U)
    return 3;
  if (sizeof(TNumeric_Bigint256) != 32U)
    return 4;
  if (sizeof(TNumeric_Decimal512) != 64U)
    return 5;

  TType_ByteTag32 tag = {0};
  if (Type_ByteTag32_FromString(&tag, "ABCD") != STATUS_CONST(SUCCESS))
    return 6;
  if (Type_ByteTag32_ToUInt(&tag) != TYPE_BYTETAG_MAKE_32('A', 'B', 'C', 'D'))
    return 7;

  TNumeric_Bigint128 a = {.limb = {10ULL, 0ULL}};
  TNumeric_Bigint128 b = {.limb = {32ULL, 0ULL}};
  if (Numeric_Bigint128_Add(&a, &b) != STATUS_CONST(SUCCESS))
    return 8;
  if (a.limb[0] != 42ULL)
    return 9;

  return 0;
}
