#include "../Codespace/Cosmeron/Modules/Container/Container.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
  /* 1. Array Vector */
  MODULE_TYPED(Array, Vector, Int32) vec;
  assert(MODULE_FUNC(Array, Vector, Int32, Init)(&vec) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Array, Vector, Int32, Push)(&vec, 42) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Array, Vector, Int32, Create)(&vec, 0U) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Array, Vector, Int32, PushBack)(&vec, 42) == STATUS_CONST(SUCCESS));
  int32_t outVal = 0;
  assert(MODULE_FUNC(Array, Vector, Int32, Pop)(&vec, &outVal) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Array, Vector, Int32, PopBack)(&vec, &outVal) == STATUS_CONST(SUCCESS));
  assert(outVal == 42);
  MODULE_FUNC(Array, Vector, Int32, Dispose)(&vec);

  /* 2. HashMap */
  MODULE_TYPED(Hash, HashMap, Int32_Int32) map;
  assert(MODULE_FUNC(Hash, HashMap, Int32_Int32, Init)(&map) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Hash, HashMap, Int32_Int32, Create)(&map) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Hash, HashMap, Int32_Int32, Insert)(&map, 7, 99) == STATUS_CONST(SUCCESS));
  int32_t mapVal = 0;
  assert(MODULE_FUNC(Hash, HashMap, Int32_Int32, Get)(&map, 7, &mapVal) == STATUS_CONST(SUCCESS));
  assert(mapVal == 99);
  int32_t *mapVal = NULL;
  assert(MODULE_FUNC(Hash, HashMap, Int32_Int32, Find)(&map, 7, &mapVal) == STATUS_CONST(SUCCESS));
  assert(mapVal != NULL && *mapVal == 99);
  MODULE_FUNC(Hash, HashMap, Int32_Int32, Dispose)(&map);

  /* 3. DynamicBitset */
  MODULE_TYPE(Container, DynamicBitset) bs;
  assert(MODULE_FUNC(Container, DynamicBitset, Create)(&bs, 64U) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Container, DynamicBitset, BitSet)(&bs, 10U) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Container, DynamicBitset, BitTest)(&bs, 10U) == true);
  MODULE_FUNC(Container, DynamicBitset, Dispose)(&bs);

  printf("All Container gold-standard tests passed!\n");
  return 0;
}
