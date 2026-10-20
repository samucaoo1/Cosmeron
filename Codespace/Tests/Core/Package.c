#include "../../Cosmeron/Core/Package/Package.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* ============================================================
 * Package 1: Math
 * Demonstrates:
 * - 0-argument functions
 * - 1-argument and multi-argument functions
 * - Inline functions
 * - High argument count (20 parameters)
 * - PACKAGE_PROTOTYPE with type inference via __typeof__
 * ============================================================ */

PACKAGE(Math,
    FUNCTION(int, GetVersion, ()),
    FUNCTION(int, Negate, (int x)),
    FUNCTION(int, Add, (int a, int b)),
    FUNCTION(int, Sub, (int a, int b)),
    FUNCTION_INLINE(int, Square, (int x), {
        return x * x;
    }),
    FUNCTION(int, Sum20, (int a1, int a2, int a3, int a4, int a5,
                          int a6, int a7, int a8, int a9, int a10,
                          int a11, int a12, int a13, int a14, int a15,
                          int a16, int a17, int a18, int a19, int a20))
);

PACKAGE_PROTOTYPE(Math, GetVersion) {
  return 100;
}

PACKAGE_PROTOTYPE(Math, Negate, x) {
  return -x;
}

PACKAGE_PROTOTYPE(Math, Add, a, b) {
  return a + b;
}

PACKAGE_PROTOTYPE(Math, Sub, a, b) {
  return a - b;
}

PACKAGE_PROTOTYPE(Math, Sum20, a1, a2, a3, a4, a5,
                               a6, a7, a8, a9, a10,
                               a11, a12, a13, a14, a15,
                               a16, a17, a18, a19, a20) {
  return a1 + a2 + a3 + a4 + a5 +
         a6 + a7 + a8 + a9 + a10 +
         a11 + a12 + a13 + a14 + a15 +
         a16 + a17 + a18 + a19 + a20;
}

PACKAGE_IMPLEMENT(Math);

/* ============================================================
 * Package 2: StringUtil
 * Demonstrates:
 * - PACKAGE_PROTOTYPEEX (Pure C11 without __typeof__)
 * ============================================================ */

PACKAGE(StringUtil,
    FUNCTION(size_t, Length, (const char *str)),
    FUNCTION(bool, IsEmpty, (const char *str))
);

PACKAGE_PROTOTYPEEX(StringUtil, Length, (const char *str)) {
  return str ? strlen(str) : 0;
}

PACKAGE_PROTOTYPEEX(StringUtil, IsEmpty, (const char *str)) {
  return (str == NULL || str[0] == '\0');
}

PACKAGE_IMPLEMENT(StringUtil);

/* ============================================================
 * Main Test Runner
 * ============================================================ */

int main(void) {
  /* 1. Direct function calls */
  assert(Math_GetVersion() == 100);
  assert(Math_Negate(42) == -42);
  assert(Math_Add(10, 20) == 30);
  assert(Math_Sub(50, 15) == 35);
  assert(Math_Square(6) == 36);

  int sum = Math_Sum20(1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
                       11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
  assert(sum == 210);

  /* 2. Pure C11 PACKAGE_PROTOTYPEEX functions */
  assert(StringUtil_Length("Cosmeron") == 8);
  assert(StringUtil_Length(NULL) == 0);
  assert(StringUtil_IsEmpty(""));
  assert(StringUtil_IsEmpty(NULL));
  assert(!StringUtil_IsEmpty("Cosmeron"));

  /* 3. FunctionTable struct access */
  const Math_FunctionTable *mathTable = Math_GetFunctionTable();
  assert(mathTable != NULL);
  assert(mathTable->Add(7, 8) == 15);
  assert(mathTable->Square(5) == 25);
  assert(mathTable->GetVersion() == 100);

  const StringUtil_FunctionTable *strTable = StringUtil_GetFunctionTable();
  assert(strTable != NULL);
  assert(strTable->Length("Test") == 4);
  assert(strTable->IsEmpty(""));

  /* 4. PACKAGE_CALL macro helpers */
  assert(PACKAGE_CALL(Math, Add, 100, 200) == 300);
  assert(PACKAGE_CALL(Math, Square, 7) == 49);
  assert(PACKAGE_CALL(Math, GetVersion) == 100);
  assert(PACKAGE_CALL_0(Math, GetVersion) == 100);
  assert(PACKAGE_CALL(StringUtil, Length, "Hello World") == 11);

  return 0;
}
