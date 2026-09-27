#define TYPE_DISABLE_FUNCTION_TABLE

#include "../../Cosmeron/Modules/Type/TBigint.h"
#include "../../Cosmeron/Modules/Type/TBlock.h"
#include "../../Cosmeron/Modules/Type/TDecimal.h"

int main(void) {
  TBigint(128, bigint);
  TBlock(128, block);
  TDecimal(128, decimal);

  if (TBIGINT_FUNC(128, Clear)(&bigint) != STATUS_CONST(SUCCESS))
    return 1;
  if (TBLOCK_FUNC(128, Clear)(&block) != STATUS_CONST(SUCCESS))
    return 2;
  if (TDECIMAL_FUNC(128, Clear)(&decimal) != STATUS_CONST(SUCCESS))
    return 3;

  if (sizeof(TBIGINT_TYPE(128)) != 16U || sizeof(TBIGINT_TYPE(1024)) != 128U)
    return 4;
  if (sizeof(TBLOCK_TYPE(128)) != 16U || sizeof(TBLOCK_TYPE(1024)) != 128U)
    return 5;
  if (sizeof(TDECIMAL_TYPE(128)) != 16U || sizeof(TDECIMAL_TYPE(1024)) != 128U)
    return 6;

  {
    TBIGINT_TYPE(256) value = {0};
    value.limb[0] = 1U;
    if (TBIGINT_FUNC(256, ShiftLeft)(&value, 255U) != STATUS_CONST(SUCCESS))
      return 7;
    if (value.limb[3] != UINT64_C(0x8000000000000000))
      return 8;
  }

  return 0;
}
