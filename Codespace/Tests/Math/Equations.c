#include "../../Cosmeron/Modules/Math/Equation/Linear.h"
#include "../../Cosmeron/Modules/Math/Equation/Quadratic.h"
static double MathTest_AbsoluteF64(double value) {
  return value < 0.0 ? -value : value;
}

int main(void) {
  double a=0,b=0;
  TMathSolution sol=MATH_CONST(SOLUTION_NONE);
  if (Math_Equation_Linear_F64(2.0,-8.0,&a,&sol)!=STATUS_CONST(SUCCESS) || sol!=MATH_CONST(SOLUTION_ONE) || MathTest_AbsoluteF64(a-4.0)>1e-12) return 1;
  if (Math_Equation_Linear_F64(0.0,0.0,&a,&sol)!=STATUS_CONST(SUCCESS) || sol!=MATH_CONST(SOLUTION_INFINITE)) return 2;
  if (Math_Equation_QuadraticDiscriminant_F64(1.0,0.0,-4.0)!=16.0) return 3;
  if (Math_Equation_Quadratic_F64(1.0,-3.0,2.0,&a,&b,&sol)!=STATUS_CONST(SUCCESS) || sol!=MATH_CONST(SOLUTION_MULTIPLE)) return 4;
  if (!((MathTest_AbsoluteF64(a-1.0)<1e-12 && MathTest_AbsoluteF64(b-2.0)<1e-12) || (MathTest_AbsoluteF64(a-2.0)<1e-12 && MathTest_AbsoluteF64(b-1.0)<1e-12))) return 5;
  return 0;
}
