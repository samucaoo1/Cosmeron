/*
 * Regression for FreeBSD compiler-rt complex division introducing an
 * unexpected dependency on libm (and for correctness of imaginary roots).
 * The code must compile and LINK with just CC -std=c11 -Werror.
 */
#include "../../Cosmeron/Modules/Math/Equation/Quadratic.h"

int main(void) {
#if !COMPILER_MSVC
  double _Complex first = 0;
  double _Complex second = 0;
  MATH_TYPE(Solution) solution = MATH_CONST(SOLUTION_NONE);
  if (Math_Equation_QuadraticComplex_F64(1.0, 0.0, 1.0,
                                        &first, &second, &solution) !=
      STATUS_CONST(SUCCESS))
    return 1;
  if (solution != MATH_CONST(SOLUTION_MULTIPLE))
    return 2;
  if (first != I || second != -I)
    return 3;

  if (Math_Equation_QuadraticComplex_F64(1.0, -3.0, 2.0,
                                        &first, &second, &solution) !=
      STATUS_CONST(SUCCESS))
    return 4;
  if (solution != MATH_CONST(SOLUTION_MULTIPLE))
    return 5;
  if (!((first == 2.0 && second == 1.0) ||
        (first == 1.0 && second == 2.0)))
    return 6;
#endif
  return 0;
}
