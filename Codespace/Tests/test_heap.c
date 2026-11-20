#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "Codespace/Cosmeron/Core/Module/Module.h"
#include "Codespace/Cosmeron/Core/Primitive/Types.h"
#include "Codespace/Cosmeron/Core/Outcome/Status.h"
#include "Codespace/Cosmeron/Core/Memory/Heap.h"

int main(void) {
  printf("HeapPageHeader size: %zu\n", sizeof(MODULE_ITYPE(Memory, HeapPageHeader)));
  printf("HeapSpan size: %zu\n", sizeof(MODULE_ITYPE(Memory, HeapSpan)));
  printf("HeapRadixLevel2 size: %zu\n", sizeof(MODULE_ITYPE(Memory, HeapRadixLevel2)));
  printf("HeapSizeClass size: %zu\n", sizeof(MODULE_ITYPE(Memory, HeapSizeClass)));
  printf("TMemoryHeap size: %zu\n", sizeof(TMemoryHeap));

  assert(sizeof(MODULE_ITYPE(Memory, HeapPageHeader)) == 128);

  TMemoryHeap heap;
  OPSTATUS status = Memory_Heap_Init(&heap);
  assert(status == STATUS_CONST(SUCCESS));

  status = OBJECT_CALL(&heap, Create, 4);
  assert(status == STATUS_CONST(SUCCESS));

  void *ptr = NULL;
  status = OBJECT_CALL(&heap, Alloc, &ptr, 64, MODULE_CONST(MEMORY, HEAP, POLICY, MANUAL), NULL);
  assert(status == STATUS_CONST(SUCCESS));
  assert(ptr != NULL);
  assert(OBJECT_CALL(&heap, Contains, ptr));

  status = OBJECT_CALL(&heap, FreeCell, ptr);
  assert(status == STATUS_CONST(SUCCESS));

  status = OBJECT_CALL(&heap, Destroy);
  assert(status == STATUS_CONST(SUCCESS));

  printf("Heap test passed successfully!\n");
  return 0;
}
