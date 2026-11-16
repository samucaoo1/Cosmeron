#define TYPE_DISABLE_FUNCTION_TABLE 1
#include "../../../Cosmeron/Modules/Type/Type.h"

int main(void) {
  /* Verify exact POD-like sizes with function tables disabled */
  if (sizeof(Type_TTag_32) < sizeof(uint32_t) + 5U)
    return 1;
  if (sizeof(Type_TBlock_128) != 16U)
    return 2;
  if (sizeof(Type_TBigint_256) != 32U)
    return 3;
  if (sizeof(Type_TDecimal_512) != 64U)
    return 4;

  Type_TTag_32 tag = {0};
  if (Type_Tag_32_FromString(&tag, "ABCD") != STATUS_CONST(SUCCESS))
    return 5;
  if (Type_Tag_32_ToUInt(&tag) != TYPE_TAG_MAKE_32('A', 'B', 'C', 'D'))
    return 6;

  Type_TBigint_128 a = {.limb = {10ULL, 0ULL}};
  Type_TBigint_128 b = {.limb = {32ULL, 0ULL}};
  if (Type_Bigint_128_Add(&a, &b) != STATUS_CONST(SUCCESS))
    return 7;
  if (a.limb[0] != 42ULL)
    return 8;

  return 0;
}
