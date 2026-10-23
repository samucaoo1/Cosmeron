#define OBJECT_DISABLE_FUNCTION_TABLE
#include "../../Cosmeron/Core/Memory/Pool.h"
#include <assert.h>
#include <stdint.h>

int main(void) {
  TMemoryPool pool = {0};
  assert(POOL_FUNC(Init)(&pool) == STATUS_CONST(SUCCESS));
  assert(POOL_FUNC(Create)(&pool, sizeof(uint64_t), 4) == STATUS_CONST(SUCCESS));
  assert(POOL_FUNC(Capacity)(&pool) == 4);

  void *p = NULL;
  assert(POOL_FUNC(Alloc)(&pool, &p) == STATUS_CONST(SUCCESS));
  assert(p != NULL);
  assert(POOL_FUNC(Allocated)(&pool) == 1);
  assert(POOL_FUNC(Contains)(&pool, p) == true);

  assert(POOL_FUNC(Free)(&pool, p) == STATUS_CONST(SUCCESS));
  assert(POOL_FUNC(Allocated)(&pool) == 0);

  assert(POOL_FUNC(Destroy)(&pool) == STATUS_CONST(SUCCESS));
  return 0;
}
