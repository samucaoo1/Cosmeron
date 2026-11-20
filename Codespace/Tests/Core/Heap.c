#include "../../Cosmeron/Core/Memory/Heap.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

int main(void) {
  TMemoryHeap heap = {0};
  assert(HEAP_FUNC(Init)(&heap) == STATUS_CONST(SUCCESS));
  assert(HEAP_FUNC(Create)(&heap, 4) == STATUS_CONST(SUCCESS));

  /* Test 1: Small Allocation in size class 32 */
  void *p1 = NULL;
  assert(HEAP_FUNC(Alloc)(&heap, &p1, 24, HEAP_CONST(POLICY_CONSERVATIVE), NULL) == STATUS_CONST(SUCCESS));
  assert(p1 != NULL);
  assert(HEAP_FUNC(Contains)(&heap, p1) == true);

  /* Test 2: Interior pointer resolution */
  void *interior = (void *)((uintptr_t)p1 + 8U);
  void *resolvedBase = NULL;
  size_t resolvedSize = 0;
  uint8_t resolvedPolicy = 0;
  assert(HEAP_FUNC(ResolvePointer)(&heap, interior, &resolvedBase, &resolvedSize, &resolvedPolicy) == true);
  assert(resolvedBase == p1);
  assert(resolvedSize == 32U);
  assert(resolvedPolicy == HEAP_CONST(POLICY_CONSERVATIVE));

  /* Test 3: Large Allocation (> 2048 bytes) */
  void *pLarge = NULL;
  assert(HEAP_FUNC(Alloc)(&heap, &pLarge, 5000, HEAP_CONST(POLICY_ATOMIC), NULL) == STATUS_CONST(SUCCESS));
  assert(pLarge != NULL);
  assert(HEAP_FUNC(Contains)(&heap, pLarge) == true);

  void *largeInterior = (void *)((uintptr_t)pLarge + 1024U);
  assert(HEAP_FUNC(ResolvePointer)(&heap, largeInterior, &resolvedBase, &resolvedSize, &resolvedPolicy) == true);
  assert(resolvedBase == pLarge);
  assert(resolvedSize == 5000U);
  assert(resolvedPolicy == HEAP_CONST(POLICY_ATOMIC));

  /* Test 4: FreeCell */
  assert(HEAP_FUNC(FreeCell)(&heap, p1) == STATUS_CONST(SUCCESS));
  assert(HEAP_FUNC(Contains)(&heap, p1) == false);

  assert(HEAP_FUNC(FreeCell)(&heap, pLarge) == STATUS_CONST(SUCCESS));
  assert(HEAP_FUNC(Contains)(&heap, pLarge) == false);

  /* Test 5: Blacklist */
  uintptr_t dummyPageIndex = 0x12345U;
  assert(HEAP_FUNC(IsBlacklisted)(&heap, dummyPageIndex) == false);
  assert(HEAP_FUNC(BlacklistPage)(&heap, dummyPageIndex) == STATUS_CONST(SUCCESS));
  assert(HEAP_FUNC(IsBlacklisted)(&heap, dummyPageIndex) == true);

  /* Test 6: Destroy */
  assert(HEAP_FUNC(Destroy)(&heap) == STATUS_CONST(SUCCESS));

  return 0;
}

