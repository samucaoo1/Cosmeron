#include "../../../Cosmeron/Modules/Struct/TDual.h"
#include "../../../Cosmeron/Modules/Struct/TPair.h"
#include "../../../Cosmeron/Modules/Struct/TPenta.h"
#include "../../../Cosmeron/Modules/Struct/TQuad.h"
#include "../../../Cosmeron/Modules/Struct/TTriple.h"

#define TEST_PAIR(TYPE, SUFFIX)                                                \
  do {                                                                         \
    Struct_TPair_##SUFFIX val = {.value1 = (TYPE)1, .value2 = (TYPE)2};        \
    if (val.value1 != (TYPE)1 || val.value2 != (TYPE)2)                        \
      return __LINE__;                                                         \
    if (val.values[0] != (TYPE)1 || val.values[1] != (TYPE)2)                  \
      return __LINE__;                                                         \
  } while (0)

#define TEST_DUAL(TYPE, SUFFIX)                                                \
  do {                                                                         \
    Struct_TDual_##SUFFIX val = {.start = (TYPE)1, .last = (TYPE)2};           \
    if (val.first != (TYPE)1 || val.end != (TYPE)2)                            \
      return __LINE__;                                                         \
    if (val.values[0] != (TYPE)1 || val.values[1] != (TYPE)2)                  \
      return __LINE__;                                                         \
  } while (0)

#define TEST_TRIPLE(TYPE, SUFFIX)                                              \
  do {                                                                         \
    Struct_TTriple_##SUFFIX val = {                                            \
        .value1 = (TYPE)1, .value2 = (TYPE)2, .value3 = (TYPE)3};              \
    if (val.length != (TYPE)1 || val.width != (TYPE)2 ||                       \
        val.height != (TYPE)3)                                                 \
      return __LINE__;                                                         \
    if (val.values[0] != (TYPE)1 || val.values[1] != (TYPE)2 ||                \
        val.values[2] != (TYPE)3)                                              \
      return __LINE__;                                                         \
  } while (0)

#define TEST_QUAD(TYPE, SUFFIX)                                                \
  do {                                                                         \
    Struct_TQuad_##SUFFIX val = {.left = (TYPE)1,                              \
                                 .right = (TYPE)2,                             \
                                 .top = (TYPE)3,                               \
                                 .bottom = (TYPE)4};                           \
    if (val.value1 != (TYPE)1 || val.value2 != (TYPE)2 ||                      \
        val.value3 != (TYPE)3 || val.value4 != (TYPE)4)                        \
      return __LINE__;                                                         \
    if (val.values[0] != (TYPE)1 || val.values[1] != (TYPE)2 ||                \
        val.values[2] != (TYPE)3 || val.values[3] != (TYPE)4)                  \
      return __LINE__;                                                         \
  } while (0)

#define TEST_PENTA(TYPE, SUFFIX)                                               \
  do {                                                                         \
    Struct_TPenta_##SUFFIX val = {.value1 = (TYPE)1,                           \
                                  .value2 = (TYPE)2,                           \
                                  .value3 = (TYPE)3,                           \
                                  .value4 = (TYPE)4,                           \
                                  .value5 = (TYPE)5};                          \
    if (val.a != (TYPE)1 || val.b != (TYPE)2 || val.c != (TYPE)3 ||            \
        val.d != (TYPE)4 || val.e != (TYPE)5)                                  \
      return __LINE__;                                                         \
    if (val.values[0] != (TYPE)1 || val.values[4] != (TYPE)5)                  \
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
  TEST_STANDARD(bool, Boolean);
  TEST_STANDARD(uint8_t, UInt8);
  TEST_STANDARD(uint16_t, UInt16);
  TEST_STANDARD(uint32_t, UInt32);
  TEST_STANDARD(uint64_t, UInt64);
  TEST_STANDARD(int8_t, Int8);
  TEST_STANDARD(int16_t, Int16);
  TEST_STANDARD(int32_t, Int32);
  TEST_STANDARD(int64_t, Int64);
  TEST_STANDARD(char, Char);
  TEST_STANDARD(uint8_t, Char8);
  TEST_STANDARD(uint16_t, Char16);
  TEST_STANDARD(uint32_t, Char32);
  TEST_STANDARD(float, Float32);
  TEST_STANDARD(double, Float64);
  TEST_STANDARD(float, Float);
  TEST_STANDARD(double, Double);
  TEST_STANDARD(long double, LongDouble);
  TEST_STANDARD(int, Int);
  TEST_STANDARD(unsigned int, UInt);
  TEST_STANDARD(size_t, Size);
  TEST_STANDARD(ptrdiff_t, PtrDiff);
  TEST_STANDARD(intptr_t, IntPtr);
  TEST_STANDARD(uintptr_t, UIntPtr);

#if !COMPILER_MSVC
  TEST_STANDARD(float _Complex, FloatComplex);
  TEST_STANDARD(double _Complex, DoubleComplex);
  TEST_STANDARD(long double _Complex, LongDoubleComplex);
#endif

  return 0;
}
