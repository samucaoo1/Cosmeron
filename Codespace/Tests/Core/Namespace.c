#define COSMERON_NAMESPACE CosmeronTest
#define COSMERON_NAMESPACE_CONST COSMERON_TEST

#include "../../Cosmeron/Core/Namespace.h"

#define TEST_TYPE(NAME) GNS3(LIB_PREFIX(Module), Type, NAME)
#define TEST_FUNC(NAME) GNS3(LIB_PREFIX(Module), Func, NAME)
#define TEST_NESTED_FUNC(NAME) GNS4(LIB_PREFIX(Module), Func, Nested, NAME)
#define TEST_CONST(NAME) CNS3(LIB_PREFIX_CONST(MODULE), CONST, NAME)

typedef struct TEST_TYPE(Value) {
  int value;
} TEST_TYPE(Value);

static int TEST_FUNC(Identity)(int value) { return value; }
static int TEST_NESTED_FUNC(Double)(int value) { return value * 2; }

enum { TEST_CONST(SUCCESS) = 0 };

int main(void) {
  TEST_TYPE(Value) value = {TEST_FUNC(Identity)(7)};
  return value.value == 7 && TEST_NESTED_FUNC(Double)(3) == 6 &&
                 TEST_CONST(SUCCESS) == 0
             ? 0
             : 1;
}
