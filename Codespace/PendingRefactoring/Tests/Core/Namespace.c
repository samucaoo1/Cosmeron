#define COSMERON_NAMESPACE CosmeronTest
#define COSMERON_NAMESPACE_CONST COSMERON_TEST

#include "../../Cosmeron/Core/Module/Namespace.h"

LIBRARY_NAMESPACE_CONFIG(COSMERON,
    NAME(CosmeronTest),
    SHORT(CT),
    MODE(STANDARD),
    CASE(HYBRID),
    TABLE(ENABLED)
);

#define TEST_TYPE(NAME) NS(LIB_PREFIX(Module), Type, NAME)
#define TEST_FUNC(NAME) NS(LIB_PREFIX(Module), Func, NAME)
#define TEST_NESTED_FUNC(NAME) NS(LIB_PREFIX(Module), Func, Nested, NAME)
#define TEST_CONST(NAME) CNS(LIB_PREFIX_CONST(MODULE), CONST, NAME)

typedef struct TEST_TYPE(Value) {
  int value;
} TEST_TYPE(Value);

static int TEST_FUNC(Identity)(int value) { return value; }
static int TEST_NESTED_FUNC(Double)(int value) { return value * 2; }

enum { TEST_CONST(SUCCESS) = 0 };

/* Forward declarations */
FORWARD_OBJECT(Text, String);

/* Actual types for tests */
struct MODULE_TYPE(Text, String) {
  int length;
};

enum MODULE_ENUM(IO, SeekOrigin) {
  MODULE_CONST(File, SEEK_BEGIN) = 1,
  MODULE_CONST(File, SEEK_END) = 2
};

/* Test functions for MODULE_CALL */
static int CosmeronTest_Math_Abs(int x) { return x < 0 ? -x : x; }
static int CosmeronTest_Memory_Alloc_Bytes(int size, int tag) { return size + tag; }
static int CosmeronTest_StreamBuffer_Utils_Transform_Compress(int a, int b, int c) {
  return a + b + c;
}

int main(void) {
  TEST_TYPE(Value) value = {TEST_FUNC(Identity)(7)};

  MODULE_TYPE(Text, String) str = {42};
  enum MODULE_ENUM(IO, SeekOrigin) origin = MODULE_CONST(File, SEEK_BEGIN);

  int r1 = MODULE_CALL(Math, Abs, (-10));
  int r2 = MODULE_CALL(Memory, Alloc, Bytes, (100, 20));
  int r3 = MODULE_CALL(StreamBuffer, Utils, Transform, Compress, (1, 2, 3));

  /* Test universal NS and CNS powered by Tree Engine */
  int NS(test, direct, two) = 2;
  int NS(test, direct, three, parts) = 3;
  int NS(test, direct, four, parts, path) = 4;
  int NS(a, b, c, d, e, f) = 6;
  int CNS(CONST, DOMAIN, CODE) = 42;

  if (test_direct_two != 2 || test_direct_three_parts != 3 ||
      test_direct_four_parts_path != 4 || a_b_c_d_e_f != 6 ||
      CONST_DOMAIN_CODE != 42)
    return 4;

  if (value.value != 7 || TEST_NESTED_FUNC(Double)(3) != 6 || TEST_CONST(SUCCESS) != 0)
    return 1;

  if (str.length != 42 || origin != 1)
    return 2;

  if (r1 != 10 || r2 != 120 || r3 != 6)
    return 3;

  return 0;
}
