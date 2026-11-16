#define OBJECT_DISABLE_FUNCTION_TABLE 1
#define PACKAGE_DISABLE_FUNCTION_TABLE 1
#define MODULE_DISABLE_FUNCTION_TABLE 1

#include "../../../Cosmeron/Modules/Container/Container.h"
#include <assert.h>

int main(void) {
  /* Verify Array without FunctionTable */
  MODULE_TYPED(Array, Vector, Int32) vec;
  assert(MODULE_FUNC(Array, Vector, Int32, Create)(&vec, 4U) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Array, Vector, Int32, PushBack)(&vec, 42) ==
         STATUS_CONST(SUCCESS));

  MODULE_TYPED(Iterator, Contiguous, Int32) iter;
  assert(MODULE_FUNC(Array, Vector, Int32, Begin)(&vec, &iter) ==
         STATUS_CONST(SUCCESS));

  int32_t value = 0;
  assert(MODULE_FUNC(Iterator, Contiguous, Int32, StepValue)(&iter, &value) ==
         STATUS_CONST(SUCCESS));
  assert(value == 42);
  assert(MODULE_FUNC(Array, Vector, Int32, Dispose)(&vec) ==
         STATUS_CONST(SUCCESS));

  /* Verify Linked without FunctionTable */
  MODULE_TYPED(Linked, List, Int32) list;
  assert(MODULE_FUNC(Linked, List, Int32, Create)(&list) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Linked, List, Int32, PushBack)(&list, 99) ==
         STATUS_CONST(SUCCESS));

  MODULE_TYPED(Iterator, Bidirectional, Int32) bit;
  assert(MODULE_FUNC(Linked, List, Int32, Begin)(&list, &bit) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Iterator, Bidirectional, Int32, StepValue)(&bit, &value) ==
         STATUS_CONST(SUCCESS));
  assert(value == 99);
  assert(MODULE_FUNC(Linked, List, Int32, Dispose)(&list) ==
         STATUS_CONST(SUCCESS));

  return 0;
}
