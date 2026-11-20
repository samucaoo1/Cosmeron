#include "../../Cosmeron/Modules/Container/Container.h"

#include <stdio.h>

int main(void) {
  /* 1. Dynamic Vector (Array_Vector_Int32) */
  MODULE_TYPED(Array, Vector, Int32) vec;
  (void)MODULE_FUNC(Array, Vector, Int32, Create)(&vec, 4U);
  (void)OBJECT_CALL(&vec, PushBack, 10);
  (void)OBJECT_CALL(&vec, PushBack, 20);
  (void)OBJECT_CALL(&vec, PushBack, 30);

  printf("Vector size=%zu:", OBJECT_CALL_0(&vec, Size));
  MODULE_TYPED(Iterator, Contiguous, Int32) it;
  (void)OBJECT_CALL(&vec, Begin, &it);
  while (OBJECT_CALL_0(&it, HasNext)) {
    int32_t item = 0;
    (void)OBJECT_CALL(&it, StepValue, &item);
    printf(" %d", (int)item);
  }
  printf("\n");
  OBJECT_CALL_0(&vec, Destroy);

  /* 2. Hash Map (Hash_Map_Int32_Int32) */
  MODULE_TYPED(Hash, Map, Int32_Int32) map;
  (void)MODULE_FUNC(Hash, Map, Int32_Int32, Create)(&map);
  (void)OBJECT_CALL(&map, Upsert, 1, 100);
  (void)OBJECT_CALL(&map, Upsert, 2, 200);

  int32_t *found = NULL;
  if (OBJECT_CALL(&map, Find, 2, &found) == STATUS_CONST(SUCCESS) && found) {
    printf("HashMap[2] = %d\n", (int)*found);
  }
  OBJECT_CALL_0(&map, Destroy);

  return 0;
}
