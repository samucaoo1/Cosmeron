#include "../../../Cosmeron/Modules/Struct/Struct.h"

/* Custom template instantiation of a pure POD struct */
TEMPLATE_INSTANTIATE(Pair, custom16, uint16_t);

int main(void) {
  /* Native designated initialization */
  Struct_TPair_int32 pair = {.x = 1, .y = 2};
  Struct_TQuad_uint16 rect = {.left = 5, .right = 6, .top = 3, .bottom = 4};
  Struct_TDual_size range = {.start = 2, .end = 8};
  Struct_TTriple_int32 size = {.length = 10, .width = 20, .height = 30};
  Struct_TPenta_int32 penta = {.value1 = 1, .value2 = 2, .value3 = 3,
                               .value4 = 4, .value5 = 5};

  if (pair.first != 1 || pair.second != 2 || pair.values[0] != 1 ||
      pair.y != 2)
    return 1;
  if (rect.values[0] != 5 || rect.values[2] != 3)
    return 2;
  if (range.value[0] != 2 || range.last != 8)
    return 3;
  if (size.values[2] != 30 || penta.values[4] != 5)
    return 4;

  /* Value semantics: pass-by-value, direct copying */
  Struct_TPair_int32 copy = pair;
  if (copy.x != 1 || copy.y != 2)
    return 5;

  /* Mutation and array access */
  copy.x = 100;
  copy.values[1] = 200;
  if (copy.value1 != 100 || copy.second != 200)
    return 6;

  /* Custom template instantiation verification */
  Struct_TPair_custom16 customPair = {.first = 42U, .second = 84U};
  if (customPair.x != 42U || customPair.y != 84U ||
      customPair.values[0] != 42U || customPair.values[1] != 84U)
    return 7;

  return 0;
}
