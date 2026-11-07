#define PACKAGE_DISABLE_FUNCTION_TABLE 1
#include "../../Cosmeron/Modules/Struct/Struct.h"

int main(void) {
  Struct_TPair_int32 pair = Struct_TPair_int32_Create(10, 20);
  int32_t a = 0, b = 0;
  if (Struct_TPair_int32_Extract(pair, &a, &b) != STATUS_CONST(SUCCESS))
    return 1;
  if (a != 10 || b != 20)
    return 2;

  Struct_TDual_int32 dual = Struct_TDual_int32_Create(30, 40);
  int32_t c = 0, d = 0;
  Struct_TDual_int32_Unpack(dual, &c, &d);
  if (c != 30 || d != 40)
    return 3;

  Struct_TTriple_int32 triple = Struct_TTriple_int32_Create(1, 2, 3);
  int32_t e = 0, f = 0, g = 0;
  if (Struct_TTriple_int32_Extract(triple, &e, &f, &g) != STATUS_CONST(SUCCESS))
    return 4;
  if (e != 1 || f != 2 || g != 3)
    return 5;

  Struct_TQuad_int32 quad = Struct_TQuad_int32_Create(1, 2, 3, 4);
  int32_t h = 0, i = 0, j = 0, k = 0;
  if (Struct_TQuad_int32_Extract(quad, &h, &i, &j, &k) != STATUS_CONST(SUCCESS))
    return 6;
  if (h != 1 || i != 2 || j != 3 || k != 4)
    return 7;

  Struct_TPenta_int32 penta = Struct_TPenta_int32_Create(1, 2, 3, 4, 5);
  int32_t l = 0, m = 0, n = 0, o = 0, p = 0;
  if (Struct_TPenta_int32_Extract(penta, &l, &m, &n, &o, &p) !=
      STATUS_CONST(SUCCESS))
    return 8;
  if (l != 1 || m != 2 || n != 3 || o != 4 || p != 5)
    return 9;

  return 0;
}

