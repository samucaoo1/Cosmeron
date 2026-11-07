#include "../../Cosmeron/Modules/Math/Arithmetic/Basic.h"
#include "../../Cosmeron/Modules/Math/Arithmetic/Clamp.h"
#include "../../Cosmeron/Modules/Math/Equation/Linear.h"
#include "../../Cosmeron/Modules/Math/Equation/Quadratic.h"
#include "../../Cosmeron/Modules/Math/Value/Between.h"
#include "../../Cosmeron/Modules/Math/Value/Clamp.h"
#include "../../Cosmeron/Modules/Math/Value/MaxAndMin.h"

#define TEST_BASIC(TYPE, SUFFIX)                                                \
  do {                                                                         \
    TYPE result = (TYPE)0;                                                     \
    if (ARITHMETIC_TYPED_FUNC(Add, SUFFIX)((TYPE)8, (TYPE)3, &result) !=       \
            STATUS_CONST(SUCCESS) ||                                           \
        result != (TYPE)11)                                                    \
      return __LINE__;                                                         \
    if (ARITHMETIC_TYPED_FUNC(Sub, SUFFIX)((TYPE)8, (TYPE)3, &result) !=       \
            STATUS_CONST(SUCCESS) ||                                           \
        result != (TYPE)5)                                                     \
      return __LINE__;                                                         \
    if (ARITHMETIC_TYPED_FUNC(Mul, SUFFIX)((TYPE)8, (TYPE)3, &result) !=       \
            STATUS_CONST(SUCCESS) ||                                           \
        result != (TYPE)24)                                                    \
      return __LINE__;                                                         \
    if (ARITHMETIC_TYPED_FUNC(Div, SUFFIX)((TYPE)8, (TYPE)2, &result) !=       \
            STATUS_CONST(SUCCESS) ||                                           \
        result != (TYPE)4)                                                     \
      return __LINE__;                                                         \
    if (ARITHMETIC_TYPED_FUNC(Mod, SUFFIX)((TYPE)8, (TYPE)3, &result) !=       \
            STATUS_CONST(SUCCESS) ||                                           \
        result != (TYPE)2)                                                     \
      return __LINE__;                                                         \
  } while (0)

#define TEST_CLAMP_ARITHMETIC(TYPE, SUFFIX)                                    \
  do {                                                                         \
    if (ARITHMETIC_TYPED_FUNC(ClampAdd, SUFFIX)(                               \
            (TYPE)8, (TYPE)5, (TYPE)0, (TYPE)10) != (TYPE)10)                 \
      return __LINE__;                                                         \
    if (ARITHMETIC_TYPED_FUNC(ClampSub, SUFFIX)(                               \
            (TYPE)2, (TYPE)5, (TYPE)0, (TYPE)10) != (TYPE)0)                  \
      return __LINE__;                                                         \
    if (ARITHMETIC_TYPED_FUNC(ClampMul, SUFFIX)(                               \
            (TYPE)4, (TYPE)4, (TYPE)0, (TYPE)10) != (TYPE)10)                 \
      return __LINE__;                                                         \
    if (ARITHMETIC_TYPED_FUNC(SaturatingAdd, SUFFIX)((TYPE)1, (TYPE)2) !=      \
        (TYPE)3)                                                               \
      return __LINE__;                                                         \
    if (ARITHMETIC_TYPED_FUNC(SaturatingSub, SUFFIX)((TYPE)5, (TYPE)2) !=      \
        (TYPE)3)                                                               \
      return __LINE__;                                                         \
    if (ARITHMETIC_TYPED_FUNC(SaturatingMul, SUFFIX)((TYPE)2, (TYPE)3) !=      \
        (TYPE)6)                                                               \
      return __LINE__;                                                         \
  } while (0)

#define TEST_VALUE(TYPE, SUFFIX)                                               \
  do {                                                                         \
    TYPE result = (TYPE)0;                                                     \
    if (!VALUE_TYPED_FUNC(IsBetween, SUFFIX)((TYPE)2, (TYPE)1, (TYPE)3))       \
      return __LINE__;                                                         \
    if (VALUE_TYPED_FUNC(Between, SUFFIX)((TYPE)2, (TYPE)1, (TYPE)3) !=        \
        MATH_RANGE_CONST(BETWEEN))                                             \
      return __LINE__;                                                         \
    if (VALUE_TYPED_FUNC(Clamp, SUFFIX)((TYPE)9, (TYPE)1, (TYPE)3) !=          \
        (TYPE)3)                                                               \
      return __LINE__;                                                         \
    if (VALUE_TYPED_FUNC(Max, SUFFIX)((TYPE)1, (TYPE)3) != (TYPE)3)            \
      return __LINE__;                                                         \
    if (VALUE_TYPED_FUNC(Min, SUFFIX)((TYPE)1, (TYPE)3) != (TYPE)1)            \
      return __LINE__;                                                         \
    if (VALUE_TYPED_FUNC(Smallest, SUFFIX)(                                    \
            &result, 3U, (TYPE)3, (TYPE)1, (TYPE)2) != STATUS_CONST(SUCCESS) ||\
        result != (TYPE)1)                                                     \
      return __LINE__;                                                         \
    if (VALUE_TYPED_FUNC(Biggest, SUFFIX)(                                     \
            &result, 3U, (TYPE)3, (TYPE)1, (TYPE)2) != STATUS_CONST(SUCCESS) ||\
        result != (TYPE)3)                                                     \
      return __LINE__;                                                         \
  } while (0)

#define TEST_EQUATION(TYPE, SUFFIX)                                            \
  do {                                                                         \
    TYPE first = (TYPE)0, second = (TYPE)0;                                    \
    MATH_TYPE(Solution) solution = MATH_CONST(SOLUTION_NONE);                        \
    if (EQUATION_TYPED_FUNC(Linear, SUFFIX)(                                   \
            (TYPE)2, (TYPE)-8, &first, &solution) != STATUS_CONST(SUCCESS) ||  \
        solution != MATH_CONST(SOLUTION_ONE) || first != (TYPE)4)              \
      return __LINE__;                                                         \
    if (EQUATION_TYPED_FUNC(QuadraticDiscriminant, SUFFIX)(                    \
            (TYPE)1, (TYPE)0, (TYPE)-4) != (TYPE)16)                          \
      return __LINE__;                                                         \
    if (EQUATION_TYPED_FUNC(Quadratic, SUFFIX)(                                \
            (TYPE)1, (TYPE)-3, (TYPE)2, &first, &second, &solution) !=         \
            STATUS_CONST(SUCCESS) ||                                           \
        solution != MATH_CONST(SOLUTION_MULTIPLE))                             \
      return __LINE__;                                                         \
  } while (0)

int main(void) {
  TEST_BASIC(int8_t, I8);
  TEST_BASIC(int16_t, I16);
  TEST_BASIC(int32_t, I32);
  TEST_BASIC(int64_t, I64);
  TEST_BASIC(uint8_t, U8);
  TEST_BASIC(uint16_t, U16);
  TEST_BASIC(uint32_t, U32);
  TEST_BASIC(uint64_t, U64);

  TEST_CLAMP_ARITHMETIC(int8_t, I8);
  TEST_CLAMP_ARITHMETIC(int16_t, I16);
  TEST_CLAMP_ARITHMETIC(int32_t, I32);
  TEST_CLAMP_ARITHMETIC(int64_t, I64);
  TEST_CLAMP_ARITHMETIC(uint8_t, U8);
  TEST_CLAMP_ARITHMETIC(uint16_t, U16);
  TEST_CLAMP_ARITHMETIC(uint32_t, U32);
  TEST_CLAMP_ARITHMETIC(uint64_t, U64);

  TEST_VALUE(int8_t, I8);
  TEST_VALUE(int16_t, I16);
  TEST_VALUE(int32_t, I32);
  TEST_VALUE(int64_t, I64);
  TEST_VALUE(uint8_t, U8);
  TEST_VALUE(uint16_t, U16);
  TEST_VALUE(uint32_t, U32);
  TEST_VALUE(uint64_t, U64);
  TEST_VALUE(float, F32);
  TEST_VALUE(double, F64);
  TEST_VALUE(long double, F128);

  TEST_EQUATION(float, F32);
  TEST_EQUATION(double, F64);
  TEST_EQUATION(long double, F128);

#if !COMPILER_MSVC
  {
    float _Complex a, b;
    MATH_TYPE(Solution) solution;
    if (Math_Equation_QuadraticComplex_F32(
            1.0f, 0.0f, 1.0f, &a, &b, &solution) != STATUS_CONST(SUCCESS))
      return __LINE__;
  }
  {
    double _Complex a, b;
    MATH_TYPE(Solution) solution;
    if (Math_Equation_QuadraticComplex_F64(
            1.0, 0.0, 1.0, &a, &b, &solution) != STATUS_CONST(SUCCESS))
      return __LINE__;
  }
  {
    long double _Complex a, b;
    MATH_TYPE(Solution) solution;
    if (Math_Equation_QuadraticComplex_F128(
            1.0L, 0.0L, 1.0L, &a, &b, &solution) != STATUS_CONST(SUCCESS))
      return __LINE__;
  }
#endif

  return 0;
}
