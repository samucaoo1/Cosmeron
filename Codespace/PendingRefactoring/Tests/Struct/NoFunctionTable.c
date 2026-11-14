#include "../../../Cosmeron/Modules/Struct/Struct.h"
#include <assert.h>

int main(void) {
  /* Verify exact POD sizes (zero vtable or api pointer overhead) */
  static_assert(sizeof(Struct_TPair_Int32) == sizeof(int32_t) * 2,
                "TPair size mismatch");
  static_assert(sizeof(Struct_TDual_Int32) == sizeof(int32_t) * 2,
                "TDual size mismatch");
  static_assert(sizeof(Struct_TTriple_Int32) == sizeof(int32_t) * 3,
                "TTriple size mismatch");
  static_assert(sizeof(Struct_TQuad_Int32) == sizeof(int32_t) * 4,
                "TQuad size mismatch");
  static_assert(sizeof(Struct_TPenta_Int32) == sizeof(int32_t) * 5,
                "TPenta size mismatch");

  Struct_TPair_Int32 pair = {.value1 = 10, .value2 = 20};
  if (pair.value1 != 10 || pair.value2 != 20)
    return 1;

  Struct_TDual_Int32 dual = {.start = 30, .end = 40};
  if (dual.start != 30 || dual.end != 40)
    return 2;

  Struct_TTriple_Int32 triple = {.length = 1, .width = 2, .height = 3};
  if (triple.length != 1 || triple.width != 2 || triple.height != 3)
    return 3;

  Struct_TQuad_Int32 quad = {.left = 1, .right = 2, .top = 3, .bottom = 4};
  if (quad.left != 1 || quad.right != 2 || quad.top != 3 || quad.bottom != 4)
    return 4;

  Struct_TPenta_Int32 penta = {.value1 = 1, .value2 = 2, .value3 = 3,
                               .value4 = 4, .value5 = 5};
  if (penta.value1 != 1 || penta.value5 != 5)
    return 5;

  return 0;
}
