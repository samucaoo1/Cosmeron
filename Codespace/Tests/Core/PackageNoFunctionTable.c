#define PACKAGE_DISABLE_FUNCTION_TABLE
#include "../../Cosmeron/Core/Module/Package.h"
#include <assert.h>

PACKAGE(Calc,
    FUNCTION(int, DoubleVal, (int x)),
    FUNCTION_INLINE(int, TripleVal, (int x), {
        return x * 3;
    }),
    FUNCTION(int, ConstZero, ())
);

PACKAGE_PROTOTYPE(Calc, DoubleVal, x) {
  return x * 2;
}

PACKAGE_PROTOTYPE(Calc, ConstZero) {
  return 0;
}

PACKAGE_IMPLEMENT(Calc);

int main(void) {
  assert(Calc_DoubleVal(21) == 42);
  assert(Calc_TripleVal(10) == 30);
  assert(Calc_ConstZero() == 0);
  return 0;
}
