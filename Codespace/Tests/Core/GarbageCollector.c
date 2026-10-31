#include "../../Cosmeron/Core/Memory/GarbageCollector.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

typedef struct TestNode {
  int value;
  struct TestNode *next;
} TestNode;

GARBAGE_COLLECTOR_DESCRIPTOR_DEF(TestNode_Desc, TestNode,
    GARBAGE_COLLECTOR_FIELD_BIT(TestNode, next)
);

static int g_finalizerRanCount = 0;

static void OnNodeFinalize(void *context, void *ptr) {
  (void)context;
  (void)ptr;
  g_finalizerRanCount++;
}

static void ClearStackFrame(void) {
  volatile void *scratch[256];
  for (size_t i = 0U; i < 256U; ++i)
    scratch[i] = NULL;
  (void)scratch;
}

static void AllocateDeadNode(TGarbageCollector *collector) {
  TestNode *temp = NULL;
  assert(GARBAGE_COLLECTOR_FUNC(AllocTyped)(collector, &temp, &TestNode_Desc) == STATUS_CONST(SUCCESS));
  assert(temp != NULL);
  temp->value = 555;
  temp->next = NULL;

  TCALLBACK(GARBAGE_COLLECTOR_INS(FinalizerFunction)) finCb =
      GARBAGE_COLLECTOR_FINALIZER_BIND(OnNodeFinalize, NULL);
  assert(GARBAGE_COLLECTOR_FUNC(SetFinalizer)(collector, temp, finCb) == STATUS_CONST(SUCCESS));
}

int main(void) {
  volatile int stackAnchor = 0;
  TGarbageCollector collector = {0};

  /* Initialize collector */
  assert(GARBAGE_COLLECTOR_FUNC(Init)(&collector) == STATUS_CONST(SUCCESS));
  assert(GARBAGE_COLLECTOR_FUNC(Create)(&collector, 4U, (const void *)&stackAnchor) == STATUS_CONST(SUCCESS));

  /* Test 1: Conservative Allocation */
  int *conservativeInt = NULL;
  assert(GARBAGE_COLLECTOR_FUNC(Alloc)(&collector, &conservativeInt, sizeof(int),
                                       HEAP_CONST(POLICY_CONSERVATIVE), NULL) == STATUS_CONST(SUCCESS));
  assert(conservativeInt != NULL);
  *conservativeInt = 42;
  assert(GARBAGE_COLLECTOR_FUNC(Contains)(&collector, conservativeInt));

  /* Test 2: Atomic Allocation */
  char *atomicBuffer = NULL;
  assert(GARBAGE_COLLECTOR_FUNC(AllocAtomic)(&collector, &atomicBuffer, 128U) == STATUS_CONST(SUCCESS));
  assert(atomicBuffer != NULL);
  strcpy(atomicBuffer, "Hello Cosmeron GC!");
  assert(GARBAGE_COLLECTOR_FUNC(Contains)(&collector, atomicBuffer));

  /* Test 3: Typed Allocation with Descriptor and Pinning */
  TestNode *pinnedHead = NULL;
  TestNode *nodeB = NULL;
  assert(GARBAGE_COLLECTOR_FUNC(AllocTyped)(&collector, &pinnedHead, &TestNode_Desc) == STATUS_CONST(SUCCESS));
  assert(GARBAGE_COLLECTOR_FUNC(AllocTyped)(&collector, &nodeB, &TestNode_Desc) == STATUS_CONST(SUCCESS));
  assert(pinnedHead != NULL && nodeB != NULL);

  pinnedHead->value = 100;
  pinnedHead->next = nodeB;
  nodeB->value = 200;
  nodeB->next = NULL;

  assert(GARBAGE_COLLECTOR_FUNC(PinRoot)(&collector, &pinnedHead) == STATUS_CONST(SUCCESS));

  /* Run GC while root is pinned -> pinnedHead and nodeB must survive */
  assert(GARBAGE_COLLECTOR_FUNC(Collect)(&collector) == STATUS_CONST(SUCCESS));
  assert(pinnedHead->value == 100 && nodeB->value == 200);

  /* Test 4: Finalizer on unreferenced dead node */
  AllocateDeadNode(&collector);
  ClearStackFrame();

  assert(GARBAGE_COLLECTOR_FUNC(Collect)(&collector) == STATUS_CONST(SUCCESS));
  assert(g_finalizerRanCount == 1);

  /* Test 5: Unpin root and clean up */
  assert(GARBAGE_COLLECTOR_FUNC(UnpinRoot)(&collector, &pinnedHead) == STATUS_CONST(SUCCESS));
  assert(GARBAGE_COLLECTOR_FUNC(Destroy)(&collector) == STATUS_CONST(SUCCESS));

  /* Test 6: Package Default Collector Facade */
  assert(PACKAGE_CALL(LIB_PREFIX(GARBAGE_COLLECTOR_MOD), Startup, (const void *)&stackAnchor) == STATUS_CONST(SUCCESS));

  int *pkgNum = NULL;
  assert(PACKAGE_CALL(LIB_PREFIX(GARBAGE_COLLECTOR_MOD), Alloc, &pkgNum, sizeof(int)) == STATUS_CONST(SUCCESS));
  assert(pkgNum != NULL);
  *pkgNum = 999;
  assert(PACKAGE_CALL(LIB_PREFIX(GARBAGE_COLLECTOR_MOD), Contains, pkgNum));

  assert(PACKAGE_CALL(LIB_PREFIX(GARBAGE_COLLECTOR_MOD), Collect) == STATUS_CONST(SUCCESS));
  assert(PACKAGE_CALL(LIB_PREFIX(GARBAGE_COLLECTOR_MOD), Shutdown) == STATUS_CONST(SUCCESS));

  return 0;
}
