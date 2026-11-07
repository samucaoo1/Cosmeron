#include "../../Cosmeron/Modules/Struct/TDual.h"
#include "../../Cosmeron/Modules/Struct/TPair.h"
#include "../../Cosmeron/Modules/Struct/TPenta.h"
#include "../../Cosmeron/Modules/Struct/TQuad.h"
#include "../../Cosmeron/Modules/Struct/TTriple.h"

#define TEST_PAIR(TYPE, SUFFIX)                                                \
  do {                                                                         \
    TYPE a = (TYPE)0, b = (TYPE)0;                                             \
    TPAIR_TYPE(SUFFIX) value = CAST_TYPE_TO_STRUCT(SUFFIX, TPair)(             \
        (TYPE)1, (TYPE)2);                                                     \
    CAST_STRUCT_TO_TYPE(TPair, SUFFIX)(value, &a, &b);                         \
    if (a != (TYPE)1 || b != (TYPE)2)                                          \
      return __LINE__;                                                         \
  } while (0)

#define TEST_DUAL(TYPE, SUFFIX)                                                \
  do {                                                                         \
    TYPE a = (TYPE)0, b = (TYPE)0;                                             \
    TDUAL_TYPE(SUFFIX) value = CAST_TYPE_TO_STRUCT(SUFFIX, TDual)(             \
        (TYPE)1, (TYPE)2);                                                     \
    CAST_STRUCT_TO_TYPE(TDual, SUFFIX)(value, &a, &b);                         \
    if (a != (TYPE)1 || b != (TYPE)2)                                          \
      return __LINE__;                                                         \
  } while (0)

#define TEST_TRIPLE(TYPE, SUFFIX)                                              \
  do {                                                                         \
    TYPE a = (TYPE)0, b = (TYPE)0, c = (TYPE)0;                                \
    TTRIPLE_TYPE(SUFFIX) value = CAST_TYPE_TO_STRUCT(SUFFIX, TTriple)(         \
        (TYPE)1, (TYPE)2, (TYPE)3);                                            \
    CAST_STRUCT_TO_TYPE(TTriple, SUFFIX)(value, &a, &b, &c);                  \
    if (a != (TYPE)1 || b != (TYPE)2 || c != (TYPE)3)                          \
      return __LINE__;                                                         \
  } while (0)

#define TEST_QUAD(TYPE, SUFFIX)                                                \
  do {                                                                         \
    TYPE a = (TYPE)0, b = (TYPE)0, c = (TYPE)0, d = (TYPE)0;                   \
    TQUAD_TYPE(SUFFIX) value = CAST_TYPE_TO_STRUCT(SUFFIX, TQuad)(             \
        (TYPE)1, (TYPE)2, (TYPE)3, (TYPE)4);                                   \
    CAST_STRUCT_TO_TYPE(TQuad, SUFFIX)(value, &a, &b, &c, &d);                \
    if (a != (TYPE)1 || b != (TYPE)2 || c != (TYPE)3 || d != (TYPE)4)          \
      return __LINE__;                                                         \
  } while (0)

#define TEST_PENTA(TYPE, SUFFIX)                                               \
  do {                                                                         \
    TYPE a = (TYPE)0, b = (TYPE)0, c = (TYPE)0, d = (TYPE)0, e = (TYPE)0;      \
    TPENTA_TYPE(SUFFIX) value = CAST_TYPE_TO_STRUCT(SUFFIX, TPenta)(           \
        (TYPE)1, (TYPE)2, (TYPE)3, (TYPE)4, (TYPE)5);                          \
    CAST_STRUCT_TO_TYPE(TPenta, SUFFIX)(value, &a, &b, &c, &d, &e);           \
    if (a != (TYPE)1 || b != (TYPE)2 || c != (TYPE)3 || d != (TYPE)4 ||        \
        e != (TYPE)5)                                                          \
      return __LINE__;                                                         \
  } while (0)

#define TEST_STANDARD(TYPE, SUFFIX)                                            \
  TEST_PAIR(TYPE, SUFFIX);                                                     \
  TEST_DUAL(TYPE, SUFFIX);                                                     \
  TEST_TRIPLE(TYPE, SUFFIX);                                                   \
  TEST_QUAD(TYPE, SUFFIX);                                                     \
  TEST_PENTA(TYPE, SUFFIX)

#define TEST_NONPAIR(TYPE, SUFFIX)                                             \
  TEST_DUAL(TYPE, SUFFIX);                                                     \
  TEST_TRIPLE(TYPE, SUFFIX);                                                   \
  TEST_QUAD(TYPE, SUFFIX);                                                     \
  TEST_PENTA(TYPE, SUFFIX)

int main(void) {
  TEST_STANDARD(uint8_t, uint8);
  TEST_STANDARD(uint16_t, uint16);
  TEST_STANDARD(uint32_t, uint32);
  TEST_STANDARD(uint64_t, uint64);
  TEST_STANDARD(int8_t, int8);
  TEST_STANDARD(int16_t, int16);
  TEST_STANDARD(int32_t, int32);
  TEST_STANDARD(int64_t, int64);
  TEST_STANDARD(float, float);
  TEST_STANDARD(double, double);
  TEST_STANDARD(long double, longdouble);

  TEST_DUAL(size_t, size);

#if !COMPILER_MSVC
  TEST_NONPAIR(float _Complex, float_complex);
  TEST_NONPAIR(double _Complex, double_complex);
  TEST_NONPAIR(long double _Complex, longdouble_complex);
#endif

  return 0;
}
