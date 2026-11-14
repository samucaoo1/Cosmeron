#include "../../../Cosmeron/Modules/Container/DynamicBitset.h"

int main(void) {
  MODULE_TYPE(Container, DynamicBitset) bs = {0};
  if (Container_DynamicBitset_Create(&bs, 100U) != STATUS_CONST(SUCCESS))
    return 1;
  if (OBJECT_CALL_0(&bs, Size) != 100U || !OBJECT_CALL_0(&bs, None))
    return 2;

  /* Tail-masked Not and SetAll */
  if (OBJECT_CALL_0(&bs, SetAll) != STATUS_CONST(SUCCESS))
    return 3;
  if (!OBJECT_CALL_0(&bs, All) || OBJECT_CALL_0(&bs, Count) != 100U)
    return 4;

  /* ClearRange, FlipRange, BitTest */
  if (OBJECT_CALL(&bs, ClearRange, 50U, 100U) != STATUS_CONST(SUCCESS))
    return 5;
  if (OBJECT_CALL_0(&bs, Count) != 50U)
    return 6;
  if (OBJECT_CALL(&bs, FlipRange, 40U, 60U) != STATUS_CONST(SUCCESS))
    return 7;
  if (OBJECT_CALL_0(&bs, Count) != 50U || OBJECT_CALL(&bs, BitTest, 45U) ||
      !OBJECT_CALL(&bs, BitTest, 55U))
    return 8;

  /* Resize smaller then larger (verifies zeroing + tail mask) */
  if (OBJECT_CALL(&bs, Resize, 40U) != STATUS_CONST(SUCCESS))
    return 9;
  if (OBJECT_CALL_0(&bs, Size) != 40U || OBJECT_CALL_0(&bs, Count) != 40U ||
      !OBJECT_CALL_0(&bs, All))
    return 10;
  if (OBJECT_CALL(&bs, Resize, 150U) != STATUS_CONST(SUCCESS))
    return 11;
  if (OBJECT_CALL_0(&bs, Size) != 150U || OBJECT_CALL_0(&bs, Count) != 40U)
    return 12;

  /* Clone and Set relations */
  MODULE_TYPE(Container, DynamicBitset) clone = {0};
  if (OBJECT_CALL(&bs, Clone, &clone) != STATUS_CONST(SUCCESS))
    return 13;
  if (!OBJECT_CALL(&bs, Equal, &clone) ||
      !OBJECT_CALL(&bs, IsSubset, &clone))
    return 14;

  /* ShiftLeft + FindFirstSet / FindFirstZero */
  if (OBJECT_CALL(&clone, ShiftLeft, 100U) != STATUS_CONST(SUCCESS))
    return 15;
  size_t bit = 0U;
  if (OBJECT_CALL(&clone, FindFirstSet, &bit) != STATUS_CONST(SUCCESS) ||
      bit != 100U)
    return 16;
  if (OBJECT_CALL(&clone, FindFirstZero, &bit) != STATUS_CONST(SUCCESS) ||
      bit != 0U)
    return 17;
  if (!OBJECT_CALL(&bs, IsDisjoint, &clone) ||
      OBJECT_CALL(&bs, Intersects, &clone))
    return 18;

  /* Out-of-range check */
  if (OBJECT_CALL(&bs, BitSet, 150U) != STATUS_CONST(OUT_OF_RANGE))
    return 19;

  if (OBJECT_CALL_0(&clone, Dispose) != STATUS_CONST(SUCCESS))
    return 20;
  if (OBJECT_CALL_0(&bs, Dispose) != STATUS_CONST(SUCCESS))
    return 21;

  return 0;
}
