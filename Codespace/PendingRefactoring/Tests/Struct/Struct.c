#include "../../Cosmeron/Modules/Struct/Struct.h"

/* Custom template instantiation */
TEMPLATE_IMPLEMENT(TPAIR, custom16, uint16_t);

int main(void) {
  /* Legacy and macro-based initialization */
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

  /* Canonical package direct calls */
  Struct_TPair_int32 canonicalPair = Struct_TPair_int32_Create(100, 200);
  int32_t val1 = 0, val2 = 0;
  if (Struct_TPair_int32_Extract(canonicalPair, &val1, &val2) !=
      STATUS_CONST(SUCCESS))
    return 5;
  if (val1 != 100 || val2 != 200)
    return 6;

  /* Invalid argument checks */
  if (Struct_TPair_int32_Extract(canonicalPair, NULL, &val2) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 7;
  if (Struct_TPair_int32_Extract(canonicalPair, &val1, NULL) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 8;

  /* Infallible unpack */
  val1 = 0;
  val2 = 0;
  Struct_TPair_int32_Unpack(canonicalPair, &val1, NULL);
  if (val1 != 100)
    return 9;

  /* PACKAGE_CALL verification */
  Struct_TDual_int32 dual = PACKAGE_CALL(TDUAL_PKG(int32), Create, 300, 400);
  val1 = 0;
  val2 = 0;
  if (PACKAGE_CALL(TDUAL_PKG(int32), Extract, dual, &val1, &val2) !=
      STATUS_CONST(SUCCESS))
    return 10;
  if (val1 != 300 || val2 != 400)
    return 11;

  /* FunctionTable verification */
  const Struct_TPair_int32_FunctionTable *ft =
      Struct_TPair_int32_GetFunctionTable();
  if (ft == NULL || ft->Create == NULL || ft->Extract == NULL ||
      ft->Unpack == NULL)
    return 12;
  Struct_TPair_int32 ftPair = ft->Create(7, 8);
  if (ftPair.value1 != 7 || ftPair.value2 != 8)
    return 13;

  /* Custom template instantiation verification */
  Struct_TPair_custom16 customPair = Struct_TPair_custom16_Create(42U, 84U);
  uint16_t c1 = 0, c2 = 0;
  if (Struct_TPair_custom16_Extract(customPair, &c1, &c2) !=
      STATUS_CONST(SUCCESS))
    return 14;
  if (c1 != 42U || c2 != 84U)
    return 15;

  return 0;
}
