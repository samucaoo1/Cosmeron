#include "../../Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define CALCULATOR_TYPED(TYPE) GNS2(TCalculator, TYPE)

/* ============================================================
 * Generic Calculator Template using the new Core/Module architecture
 * ============================================================ */

#define TEMPLATE_TCALCULATOR(SUFFIX, TYPE)                                     \
  OBJECT(                                                                      \
      CALCULATOR_TYPED(SUFFIX),                                                \
      FIELDS(                                                                  \
          TYPE result;                                                         \
          bool hasResult;                                                      \
      ),                                                                       \
      OPERATION(Create, ()),                                                   \
      OPERATION(Add, (TYPE a, TYPE b, TYPE *out)),                             \
      OPERATION(Subtract, (TYPE a, TYPE b, TYPE *out)),                        \
      OPERATION(Multiply, (TYPE a, TYPE b, TYPE *out)),                        \
      OPERATION(Divide, (TYPE a, TYPE b, TYPE *out)),                          \
      CONST_FUNCTION(TYPE, Result, ()),                                        \
      CONST_PREDICATE(HasResult, ())                                           \
  )

#define TEMPLATE_TCALCULATOR_IMPLEMENT(SUFFIX, TYPE)                           \
  OBJECT_OPERATION_IMPLEMENT_EX(CALCULATOR_TYPED(SUFFIX), Create, ()) {        \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    self->result = (TYPE)0;                                                    \
    self->hasResult = false;                                                   \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  OBJECT_OPERATION_IMPLEMENT_EX(CALCULATOR_TYPED(SUFFIX), Add, (TYPE a, TYPE b, TYPE *out)) { \
    if (!self || !out)                                                         \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    *out = a + b;                                                              \
    self->result = *out;                                                       \
    self->hasResult = true;                                                    \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  OBJECT_OPERATION_IMPLEMENT_EX(CALCULATOR_TYPED(SUFFIX), Subtract, (TYPE a, TYPE b, TYPE *out)) { \
    if (!self || !out)                                                         \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    *out = a - b;                                                              \
    self->result = *out;                                                       \
    self->hasResult = true;                                                    \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  OBJECT_OPERATION_IMPLEMENT_EX(CALCULATOR_TYPED(SUFFIX), Multiply, (TYPE a, TYPE b, TYPE *out)) { \
    if (!self || !out)                                                         \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    *out = a * b;                                                              \
    self->result = *out;                                                       \
    self->hasResult = true;                                                    \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  OBJECT_OPERATION_IMPLEMENT_EX(CALCULATOR_TYPED(SUFFIX), Divide, (TYPE a, TYPE b, TYPE *out)) { \
    if (!self || !out)                                                         \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    if (b == (TYPE)0)                                                          \
      return STATUS_CONST(DIVISION_BY_ZERO);                                   \
    *out = a / b;                                                              \
    self->result = *out;                                                       \
    self->hasResult = true;                                                    \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  OBJECT_FUNCTION_IMPLEMENT_EX(CALCULATOR_TYPED(SUFFIX), TYPE, Result, ()) {   \
    return self ? self->result : (TYPE)0;                                      \
  }                                                                            \
  OBJECT_PREDICATE_IMPLEMENT_EX(CALCULATOR_TYPED(SUFFIX), HasResult, ()) {     \
    return self ? self->hasResult : false;                                     \
  }

/* Instantiate int and double specialization */
TEMPLATE_IMPLEMENT(TCALCULATOR, int, int);
TEMPLATE_IMPLEMENT(TCALCULATOR, double, double);

int main(void) {
  /* 1. Test Int Specialization */
  {
    TCalculator_int calc;
    assert(TCalculator_int_Init(&calc) == STATUS_CONST(SUCCESS));
    assert(OBJECT_CALL_0(&calc, Create) == STATUS_CONST(SUCCESS));
    assert(OBJECT_CALL_0(&calc, HasResult) == false);
    assert(OBJECT_CALL_0(&calc, Result) == 0);

    int out = 0;
    assert(OBJECT_CALL(&calc, Add, 20, 10, &out) == STATUS_CONST(SUCCESS));
    assert(out == 30);
    assert(OBJECT_CALL_0(&calc, Result) == 30);
    assert(OBJECT_CALL_0(&calc, HasResult) == true);

    assert(OBJECT_CALL(&calc, Subtract, 30, 8, &out) == STATUS_CONST(SUCCESS));
    assert(out == 22);
    assert(OBJECT_CALL_0(&calc, Result) == 22);

    assert(OBJECT_CALL(&calc, Multiply, 6, 7, &out) == STATUS_CONST(SUCCESS));
    assert(out == 42);
    assert(OBJECT_CALL_0(&calc, Result) == 42);

    assert(OBJECT_CALL(&calc, Divide, 42, 5, &out) == STATUS_CONST(SUCCESS));
    assert(out == 8);
    assert(OBJECT_CALL_0(&calc, Result) == 8);

    assert(OBJECT_CALL(&calc, Divide, 10, 0, &out) == STATUS_CONST(DIVISION_BY_ZERO));
    assert(OBJECT_CALL_0(&calc, Result) == 8);
  }

  /* 2. Test Double Specialization */
  {
    TCalculator_double calc;
    assert(TCalculator_double_Init(&calc) == STATUS_CONST(SUCCESS));
    assert(OBJECT_CALL_0(&calc, Create) == STATUS_CONST(SUCCESS));
    assert(OBJECT_CALL_0(&calc, HasResult) == false);
    assert(OBJECT_CALL_0(&calc, Result) == 0.0);

    double out = 0.0;
    assert(OBJECT_CALL(&calc, Add, 10.5, 4.5, &out) == STATUS_CONST(SUCCESS));
    assert(out == 15.0);
    assert(OBJECT_CALL_0(&calc, Result) == 15.0);
    assert(OBJECT_CALL_0(&calc, HasResult) == true);

    assert(OBJECT_CALL(&calc, Subtract, 15.0, 3.25, &out) == STATUS_CONST(SUCCESS));
    assert(out == 11.75);
    assert(OBJECT_CALL_0(&calc, Result) == 11.75);

    assert(OBJECT_CALL(&calc, Multiply, 2.5, 4.0, &out) == STATUS_CONST(SUCCESS));
    assert(out == 10.0);
    assert(OBJECT_CALL_0(&calc, Result) == 10.0);

    assert(OBJECT_CALL(&calc, Divide, 7.5, 2.0, &out) == STATUS_CONST(SUCCESS));
    assert(out == 3.75);
    assert(OBJECT_CALL_0(&calc, Result) == 3.75);

    assert(OBJECT_CALL(&calc, Divide, 10.0, 0.0, &out) == STATUS_CONST(DIVISION_BY_ZERO));
    assert(OBJECT_CALL_0(&calc, Result) == 3.75);
  }

  return 0;
}
