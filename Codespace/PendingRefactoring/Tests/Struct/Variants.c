#include "../../../Cosmeron/Modules/Struct/TDual.h"
#include "../../../Cosmeron/Modules/Struct/TPair.h"
#include "../../../Cosmeron/Modules/Struct/TPenta.h"
#include "../../../Cosmeron/Modules/Struct/TQuad.h"
#include "../../../Cosmeron/Modules/Struct/TTriple.h"

int main(void) {
  Struct_TPair_Int32 p = {.value1 = 1, .value2 = 2};
  if (p.a != 1 || p.b != 2 || p.x != 1 || p.y != 2 || p.first != 1 ||
      p.second != 2)
    return 1;

  Struct_TDual_Int32 q = {.start = 3, .last = 4};
  if (q.first != 3 || q.end != 4 || q.begin != 3 || q.last != 4 ||
      q.real != 3 || q.imaginary != 4)
    return 2;

  Struct_TTriple_Int32 t = {.length = 5, .width = 6, .height = 7};
  if (t.a != 5 || t.b != 6 || t.c != 7 || t.red != 5 || t.green != 6 ||
      t.blue != 7)
    return 3;

  Struct_TQuad_Int32 u = {.left = 8, .right = 9, .top = 10, .bottom = 11};
  if (u.a != 8 || u.b != 9 || u.c != 10 || u.d != 11 || u.red != 8 ||
      u.alpha != 11)
    return 4;

  Struct_TPenta_Int32 v = {.value1 = 12, .value2 = 13, .value3 = 14,
                           .value4 = 15, .value5 = 16};
  if (v.a != 12 || v.b != 13 || v.c != 14 || v.d != 15 || v.e != 16 ||
      v.values[4] != 16)
    return 5;

  if (sizeof(Struct_TPair_UInt64) != sizeof(uint64_t) * 2U)
    return 6;
  if (sizeof(Struct_TTriple_UInt16) != sizeof(uint16_t) * 3U)
    return 7;
  if (sizeof(Struct_TQuad_UInt8) != sizeof(uint8_t) * 4U)
    return 8;
  if (sizeof(Struct_TPenta_UInt32) != sizeof(uint32_t) * 5U)
    return 9;
  return 0;
}
