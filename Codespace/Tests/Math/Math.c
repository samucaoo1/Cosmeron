#include "../../Cosmeron/Modules/Math/Equation/Linear.h"
#include "../../Cosmeron/Modules/Math/Equation/Quadratic.h"

static float MathTest_AbsoluteF32(float value) {
  return value < 0.0f ? -value : value;
}

int main(void) {
  float r1 = 0.0f, r2 = 0.0f;
  TMathSolution solution = MATH_SOLUTION_NONE;

  if (Math_Equation_Linear_F32(2.0f, -4.0f, &r1, &solution) != STATUS_CONST(SUCCESS) ||
      solution != MATH_SOLUTION_ONE || MathTest_AbsoluteF32(r1 - 2.0f) > 0.0001f)
    return 1;

  if (Math_Equation_Linear_F32(0.0f, 0.0f, &r1, &solution) != STATUS_CONST(SUCCESS) ||
      solution != MATH_SOLUTION_INFINITE)
    return 2;

  if (Math_Equation_Quadratic_F32(1.0f, 0.0f, -1.0f, &r1, &r2, &solution) != STATUS_CONST(SUCCESS) ||
      solution != MATH_SOLUTION_MULTIPLE)
    return 3;

  if (Math_Equation_Quadratic_F32(1.0f, 0.0f, 1.0f, &r1, &r2, &solution) != STATUS_CONST(SUCCESS) ||
      solution != MATH_SOLUTION_NONE)
    return 4;

  return 0;
}
