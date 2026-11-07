#include "../../Cosmeron/Core/Memory/Pool.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

typedef struct TestNode {
  int id;
  char name[28];
} TestNode;

int main(void) {
  /* ------------------------------------------------------------
   * 1. Test TPool via OBJECT API
   * ------------------------------------------------------------ */
  {
    TMemoryPool pool = {0};
    assert(POOL_FUNC(Init)(&pool) == STATUS_CONST(SUCCESS));
    assert(pool.api != NULL);

    /* Invalid parameters */
    assert(pool.api->Create(&pool, 0, 10) == STATUS_CONST(INVALID_ARGUMENT));
    assert(pool.api->Create(&pool, sizeof(TestNode), 0) == STATUS_CONST(INVALID_ARGUMENT));

    /* Valid creation: 3 elements of TestNode */
    assert(pool.api->Create(&pool, sizeof(TestNode), 3) == STATUS_CONST(SUCCESS));
    assert(pool.api->Capacity(&pool) == 3);
    assert(pool.api->Available(&pool) == 3);
    assert(pool.api->Allocated(&pool) == 0);
    assert(pool.api->ElementSize(&pool) >= sizeof(TestNode));

    void *p1 = NULL;
    void *p2 = NULL;
    void *p3 = NULL;
    void *p4 = NULL;

    assert(pool.api->Alloc(&pool, &p1) == STATUS_CONST(SUCCESS));
    assert(p1 != NULL);
    assert(pool.api->Contains(&pool, p1) == true);
    assert(pool.api->Allocated(&pool) == 1);
    assert(pool.api->Available(&pool) == 2);

    assert(pool.api->Alloc(&pool, &p2) == STATUS_CONST(SUCCESS));
    assert(p2 != NULL);
    assert(p2 != p1);
    assert(pool.api->Allocated(&pool) == 2);

    assert(pool.api->Alloc(&pool, &p3) == STATUS_CONST(SUCCESS));
    assert(p3 != NULL);
    assert(pool.api->Allocated(&pool) == 3);
    assert(pool.api->Available(&pool) == 0);

    /* Pool exhausted */
    assert(pool.api->Alloc(&pool, &p4) == STATUS_CONST(OUT_OF_MEMORY));
    assert(p4 == NULL);

    /* Write data into allocated nodes to verify memory safety */
    TestNode *node1 = (TestNode *)p1;
    node1->id = 101;
    strcpy(node1->name, "Node 1");

    TestNode *node2 = (TestNode *)p2;
    node2->id = 202;
    strcpy(node2->name, "Node 2");

    assert(node1->id == 101);
    assert(node2->id == 202);

    /* Free middle node p2 */
    assert(pool.api->Free(&pool, p2) == STATUS_CONST(SUCCESS));
    assert(pool.api->Allocated(&pool) == 2);
    assert(pool.api->Available(&pool) == 1);

    /* Attempt to free an invalid pointer outside pool */
    int externalVar = 42;
    assert(pool.api->Free(&pool, &externalVar) == STATUS_CONST(INVALID_ARGUMENT));
    assert(pool.api->Contains(&pool, &externalVar) == false);

    /* Reallocate: should reuse p2 */
    void *reallocated = NULL;
    assert(pool.api->Alloc(&pool, &reallocated) == STATUS_CONST(SUCCESS));
    assert(reallocated == p2);
    assert(pool.api->Allocated(&pool) == 3);

    /* Reset pool without freeing buffer */
    assert(pool.api->Reset(&pool) == STATUS_CONST(SUCCESS));
    assert(pool.api->Allocated(&pool) == 0);
    assert(pool.api->Available(&pool) == 3);

    /* Destroy pool */
    assert(pool.api->Destroy(&pool) == STATUS_CONST(SUCCESS));
    assert(pool.buffer == NULL);
    assert(pool.api->Capacity(&pool) == 0);
  }

  /* ------------------------------------------------------------
   * 2. Test TPool via Direct Method Calls & OBJECT_CALL
   * ------------------------------------------------------------ */
  {
    TMemoryPool pool = {0};
    assert(POOL_FUNC(Init)(&pool) == STATUS_CONST(SUCCESS));
    assert(POOL_FUNC(Create)(&pool, sizeof(uint64_t), 4) == STATUS_CONST(SUCCESS));

    void *item = NULL;
    assert(POOL_FUNC(Alloc)(&pool, &item) == STATUS_CONST(SUCCESS));
    assert(item != NULL);
    assert(POOL_FUNC(Allocated)(&pool) == 1);

    assert(OBJECT_CALL_0(&pool, Available) == 3);
    assert(OBJECT_CALL(&pool, Free, item) == STATUS_CONST(SUCCESS));
    assert(OBJECT_CALL_0(&pool, Available) == 4);

    assert(POOL_FUNC(Destroy)(&pool) == STATUS_CONST(SUCCESS));
  }

  /* ------------------------------------------------------------
   * 3. Test Static Memory / Stack Buffer (CreateBuffer)
   * ------------------------------------------------------------ */
  {
    uint8_t rawBuffer[256];
    TMemoryPool pool = {0};
    assert(POOL_FUNC(Init)(&pool) == STATUS_CONST(SUCCESS));
    assert(POOL_FUNC(CreateBuffer)(&pool, rawBuffer, sizeof(rawBuffer), 32) == STATUS_CONST(SUCCESS));

    size_t cap = POOL_FUNC(Capacity)(&pool);
    assert(cap > 0);

    void *p = NULL;
    assert(POOL_FUNC(Alloc)(&pool, &p) == STATUS_CONST(SUCCESS));
    assert(p != NULL);
    assert(POOL_FUNC(Contains)(&pool, p) == true);

    /* Destroy should just clear metadata without freeing rawBuffer */
    assert(POOL_FUNC(Destroy)(&pool) == STATUS_CONST(SUCCESS));
  }

  /* ------------------------------------------------------------
   * 4. Test Namespace Macro Invocations via POOL_FUNC
   * ------------------------------------------------------------ */
  {
    TMemoryPool pool = {0};
    assert(POOL_FUNC(Init)(&pool) == STATUS_CONST(SUCCESS));
    assert(POOL_FUNC(Create)(&pool, sizeof(int), 8) == STATUS_CONST(SUCCESS));
    assert(POOL_FUNC(Capacity)(&pool) == 8);

    void *ptr = NULL;
    assert(POOL_FUNC(Alloc)(&pool, &ptr) == STATUS_CONST(SUCCESS));
    assert(ptr != NULL);
    assert(POOL_FUNC(Allocated)(&pool) == 1);
    assert(POOL_FUNC(Available)(&pool) == 7);
    assert(POOL_FUNC(Contains)(&pool, ptr) == true);

    assert(POOL_FUNC(Free)(&pool, ptr) == STATUS_CONST(SUCCESS));
    assert(POOL_FUNC(Allocated)(&pool) == 0);

    assert(POOL_FUNC(Destroy)(&pool) == STATUS_CONST(SUCCESS));
  }

  return 0;
}
