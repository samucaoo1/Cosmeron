#include "../../../Cosmeron/Core/Algorithm/Comparison.h"
#include "../../../Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <math.h>
#include <string.h>

OBJECT(Person,
  FIELDS(
    int age;
    double score;
    const char *name;
    unsigned char data[4];
  ),
  COMPARISON_NUMERIC(CompareAge, age),
  COMPARISON_NUMERIC(CompareScore, score),
  COMPARISON_STRING(CompareName, name),
  COMPARISON_BYTES(CompareData, data),
  COMPARISON_INLINE(CompareReverseAge, (const Person *other), {
    if (self->age < other->age)
      return COMPARISON_CONST(HIGHER);
    if (self->age > other->age)
      return COMPARISON_CONST(LOWER);
    return COMPARISON_CONST(EQUAL);
  }),
  COMPARISON(CompareManual, (const Person *other))
);

OBJECT_COMPARISON_IMPLEMENT_EX(Person, CompareManual, (const Person *other)) {
  if (self->age != other->age) {
    return self->age < other->age ? COMPARISON_CONST(LOWER) : COMPARISON_CONST(HIGHER);
  }
  return NS(Person, CompareName)(self, other);
}

OBJECT_IMPLEMENT(Person);

PACKAGE(MathCmp,
  COMPARISON_INLINE(CompareDouble, (double a, double b), {
    if (a < b)
      return COMPARISON_CONST(LOWER);
    if (a > b)
      return COMPARISON_CONST(HIGHER);
    return COMPARISON_CONST(EQUAL);
  }),
  COMPARISON(CompareInt, (int a, int b))
);

PACKAGE_COMPARISON_IMPLEMENT_EX(MathCmp, CompareInt, (int a, int b)) {
  if (a < b)
    return COMPARISON_CONST(LOWER);
  if (a > b)
    return COMPARISON_CONST(HIGHER);
  return COMPARISON_CONST(EQUAL);
}

PACKAGE_IMPLEMENT(MathCmp);

int main(void) {
  Person p1;
  Person p2;
  NS(Person, Init)(&p1);
  NS(Person, Init)(&p2);

  p1.age = 20;
  p1.score = 85.5;
  p1.name = "Alice";
  p1.data[0] = 1; p1.data[1] = 2; p1.data[2] = 3; p1.data[3] = 4;

  p2.age = 25;
  p2.score = 92.0;
  p2.name = "Bob";
  p2.data[0] = 1; p2.data[1] = 2; p2.data[2] = 3; p2.data[3] = 5;

  /* 1. Direct function calls */
  assert(NS(Person, CompareAge)(&p1, &p2) == COMPARISON_CONST(LOWER));
  assert(NS(Person, CompareAge)(&p2, &p1) == COMPARISON_CONST(HIGHER));
  assert(NS(Person, CompareAge)(&p1, &p1) == COMPARISON_CONST(EQUAL));

  /* 2. OBJECT_CALL dispatch */
  assert(OBJECT_CALL(&p1, CompareAge, &p2) == COMPARISON_CONST(LOWER));
  assert(OBJECT_CALL(&p2, CompareAge, &p1) == COMPARISON_CONST(HIGHER));
  assert(OBJECT_CALL(&p1, CompareAge, &p1) == COMPARISON_CONST(EQUAL));

  /* 3. COMPARISON_NUMERIC with floating point and NaN */
  assert(OBJECT_CALL(&p1, CompareScore, &p2) == COMPARISON_CONST(LOWER));
  assert(OBJECT_CALL(&p2, CompareScore, &p1) == COMPARISON_CONST(HIGHER));

  p1.score = -0.0;
  p2.score = 0.0;
  assert(OBJECT_CALL(&p1, CompareScore, &p2) == COMPARISON_CONST(EQUAL));

#if defined(NAN)
  p1.score = NAN;
  p2.score = 100.0;
  assert(OBJECT_CALL(&p1, CompareScore, &p2) == COMPARISON_CONST(HIGHER)); /* NaN > number */
  assert(OBJECT_CALL(&p2, CompareScore, &p1) == COMPARISON_CONST(LOWER));

  p2.score = NAN;
  assert(OBJECT_CALL(&p1, CompareScore, &p2) == COMPARISON_CONST(EQUAL)); /* NaN == NaN */
#endif

  /* 4. COMPARISON_STRING with lexicographical ordering and NULL safety */
  p1.name = "Alice";
  p2.name = "Bob";
  assert(OBJECT_CALL(&p1, CompareName, &p2) == COMPARISON_CONST(LOWER));
  assert(OBJECT_CALL(&p2, CompareName, &p1) == COMPARISON_CONST(HIGHER));

  p1.name = "Alice";
  p2.name = "Alice";
  assert(OBJECT_CALL(&p1, CompareName, &p2) == COMPARISON_CONST(EQUAL));

  p1.name = NULL;
  p2.name = "Alice";
  assert(OBJECT_CALL(&p1, CompareName, &p2) == COMPARISON_CONST(LOWER)); /* NULL < non-NULL */
  assert(OBJECT_CALL(&p2, CompareName, &p1) == COMPARISON_CONST(HIGHER));

  p2.name = NULL;
  assert(OBJECT_CALL(&p1, CompareName, &p2) == COMPARISON_CONST(EQUAL)); /* NULL == NULL */

  /* 5. COMPARISON_BYTES */
  p1.data[3] = 4;
  p2.data[3] = 5;
  assert(OBJECT_CALL(&p1, CompareData, &p2) == COMPARISON_CONST(LOWER));
  assert(OBJECT_CALL(&p2, CompareData, &p1) == COMPARISON_CONST(HIGHER));
  p2.data[3] = 4;
  assert(OBJECT_CALL(&p1, CompareData, &p2) == COMPARISON_CONST(EQUAL));

  /* 6. COMPARISON_INLINE (reverse age) */
  p1.age = 20;
  p2.age = 25;
  assert(OBJECT_CALL(&p1, CompareReverseAge, &p2) == COMPARISON_CONST(HIGHER));
  assert(OBJECT_CALL(&p2, CompareReverseAge, &p1) == COMPARISON_CONST(LOWER));

  /* 7. Out-of-line COMPARISON (CompareManual) */
  p1.name = "Alice";
  p2.name = "Bob";
  assert(OBJECT_CALL(&p1, CompareManual, &p2) == COMPARISON_CONST(LOWER));
  p1.age = 30;
  assert(OBJECT_CALL(&p1, CompareManual, &p2) == COMPARISON_CONST(HIGHER));
  p2.age = 30;
  assert(OBJECT_CALL(&p1, CompareManual, &p2) == COMPARISON_CONST(LOWER)); /* tied age, compared by name */

  /* 8. Package comparison functions */
  assert(PACKAGE_CALL(MathCmp, CompareDouble, 1.5, 2.5) == COMPARISON_CONST(LOWER));
  assert(PACKAGE_CALL(MathCmp, CompareDouble, 3.5, 2.5) == COMPARISON_CONST(HIGHER));
  assert(PACKAGE_CALL(MathCmp, CompareDouble, 2.0, 2.0) == COMPARISON_CONST(EQUAL));

  assert(PACKAGE_CALL(MathCmp, CompareInt, 10, 20) == COMPARISON_CONST(LOWER));
  assert(PACKAGE_CALL(MathCmp, CompareInt, 30, 20) == COMPARISON_CONST(HIGHER));
  assert(PACKAGE_CALL(MathCmp, CompareInt, 15, 15) == COMPARISON_CONST(EQUAL));

  /* 9. Automatic ComparatorAdapter and AsComparator integration with TCALLBACK(Comparison) */
  p1.age = 20;
  p2.age = 25;
  TCALLBACK(Comparison) cbAge = Person_CompareAge_AsComparator();
  assert(CALLBACK_IS_VALID(cbAge));
  assert(CALLBACK_INVOKE(cbAge, &p1, &p2) == COMPARISON_CONST(LOWER));
  assert(CALLBACK_INVOKE(cbAge, &p2, &p1) == COMPARISON_CONST(HIGHER));
  assert(CALLBACK_INVOKE(cbAge, &p1, &p1) == COMPARISON_CONST(EQUAL));

  p1.name = "Alice";
  p2.name = "Bob";
  TCALLBACK(Comparison) cbName = Person_CompareName_AsComparator();
  assert(CALLBACK_IS_VALID(cbName));
  assert(CALLBACK_INVOKE(cbName, &p1, &p2) == COMPARISON_CONST(LOWER));
  assert(CALLBACK_INVOKE(cbName, &p2, &p1) == COMPARISON_CONST(HIGHER));

  return 0;
}

