#include "../../Cosmeron/Modules/Struct/TDual.h"
#include "../../Cosmeron/Modules/Struct/TPair.h"
#include "../../Cosmeron/Modules/Struct/TPenta.h"
#include "../../Cosmeron/Modules/Struct/TQuad.h"
#include "../../Cosmeron/Modules/Struct/TTriple.h"

int main(void) {
  Struct_TPair_int32 pair = Cast_int32_To_TPair(1, 2);
  Struct_TQuad_uint16 rect = {.left = 5, .right = 6, .top = 3, .bottom = 4};
  Struct_TDual_size range = {.start = 2, .end = 8};
  Struct_TTriple_int32 size = {.length = 10, .width = 20, .height = 30};
  TPENTA_TYPE(int32) penta = {.value1 = 1, .value2 = 2, .value3 = 3,
                              .value4 = 4, .value5 = 5};
  int32_t first = 0;
  int32_t second = 0;

  Cast_TPair_To_int32(pair, &first, &second);

  if (first != 1 || second != 2 || pair.values[0] != 1 || pair.y != 2)
    return 1;
  if (rect.values[0] != 5 || rect.values[2] != 3)
    return 2;
  if (range.value[0] != 2 || range.last != 8)
    return 3;
  if (size.values[2] != 30 || penta.values[4] != 5)
    return 4;

  return 0;
}
