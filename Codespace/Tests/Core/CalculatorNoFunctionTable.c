#define OBJECT_DISABLE_FUNCTION_TABLE
#include "../../Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <stdbool.h>

#define CALCULATOR_TYPED(TYPE) GNS2(TCalculator, TYPE)

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

TEMPLATE_IMPLEMENT(TCALCULATOR, int, int);
TEMPLATE_IMPLEMENT(TCALCULATOR, double, double);

int main(void) {
  {
    TCalculator_int calc;
    assert(TCalculator_int_Init(&calc) == STATUS_CONST(SUCCESS));
    assert(TCalculator_int_Create(&calc) == STATUS_CONST(SUCCESS));
    assert(TCalculator_int_HasResult(&calc) == false);
    assert(TCalculator_int_Result(&calc) == 0);

    int out = 0;
    assert(TCalculator_int_Add(&calc, 20, 10, &out) == STATUS_CONST(SUCCESS));
    assert(out == 30);
    assert(TCalculator_int_Result(&calc) == 30);
    assert(TCalculator_int_HasResult(&calc) == true);

    assert(TCalculator_int_Subtract(&calc, 30, 8, &out) == STATUS_CONST(SUCCESS));
    assert(out == 22);
    assert(TCalculator_int_Result(&calc) == 22);

    assert(TCalculator_int_Multiply(&calc, 6, 7, &out) == STATUS_CONST(SUCCESS));
    assert(out == 42);
    assert(TCalculator_int_Result(&calc) == 42);

    assert(TCalculator_int_Divide(&calc, 42, 5, &out) == STATUS_CONST(SUCCESS));
    assert(out == 8);
    assert(TCalculator_int_Result(&calc) == 8);

    assert(TCalculator_int_Divide(&calc, 10, 0, &out) == STATUS_CONST(DIVISION_BY_ZERO));
    assert(TCalculator_int_Result(&calc) == 8);
  }

  {
    TCalculator_double calc;
    assert(TCalculator_double_Init(&calc) == STATUS_CONST(SUCCESS));
    assert(TCalculator_double_Create(&calc) == STATUS_CONST(SUCCESS));
    assert(TCalculator_double_HasResult(&calc) == false);
    assert(TCalculator_double_Result(&calc) == 0.0);

    double out = 0.0;
    assert(TCalculator_double_Add(&calc, 10.5, 4.5, &out) == STATUS_CONST(SUCCESS));
    assert(out == 15.0);
    assert(TCalculator_double_Result(&calc) == 15.0);
    assert(TCalculator_double_HasResult(&calc) == true);

    assert(TCalculator_double_Subtract(&calc, 15.0, 3.25, &out) == STATUS_CONST(SUCCESS));
    assert(out == 11.75);
    assert(TCalculator_double_Result(&calc) == 11.75);

    assert(TCalculator_double_Multiply(&calc, 2.5, 4.0, &out) == STATUS_CONST(SUCCESS));
    assert(out == 10.0);
    assert(TCalculator_double_Result(&calc) == 10.0);

    assert(TCalculator_double_Divide(&calc, 7.5, 2.0, &out) == STATUS_CONST(SUCCESS));
    assert(out == 3.75);
    assert(TCalculator_double_Result(&calc) == 3.75);

    assert(TCalculator_double_Divide(&calc, 10.0, 0.0, &out) == STATUS_CONST(DIVISION_BY_ZERO));
    assert(TCalculator_double_Result(&calc) == 3.75);
  }

  return 0;
}
