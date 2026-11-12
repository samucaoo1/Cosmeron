#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "Codespace/Cosmeron/Core/Module/Module.h"
#include "Codespace/Cosmeron/Core/Outcome/Status.h"
#include "Codespace/Cosmeron/Core/Memory/Memory.h"

/* Instanciações concretas para int32 */
TEMPLATE_INSTANTIATE(Memory, UniquePtr, int32, int32_t);
TEMPLATE_INSTANTIATE(Memory, WeakPtr, int32, int32_t);
TEMPLATE_INSTANTIATE(Memory, SharedPtr, int32, int32_t);
TEMPLATE_INSTANTIATE(Memory, WeakPtrAtomic, int32, int32_t);
TEMPLATE_INSTANTIATE(Memory, SharedPtrAtomic, int32, int32_t);

/* Objeto alvo para IntrusivePtr */
typedef struct TestNode {
  INTRUSIVE_REF_COUNT_FIELD
  int32_t value;
} TestNode;

TEMPLATE_INSTANTIATE(Memory, IntrusivePtr, Node, TestNode);

static void test_unique_ptr(void) {
  printf("[Test] UniquePtr...\n");
  MODULE_TYPED(Memory, UniquePtr, int32) u1;
  MODULE_TYPED(Memory, UniquePtr, int32) u2;

  OPSTATUS status = MODULE_FUNC(Memory, UniquePtr, int32, Make)(&u1, 42);
  assert(status == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Memory, UniquePtr, int32, IsValid)(&u1));
  assert(*MODULE_FUNC(Memory, UniquePtr, int32, Get)(&u1) == 42);

  /* Test Move */
  status = MODULE_FUNC(Memory, UniquePtr, int32, Move)(&u2, &u1);
  assert(status == STATUS_CONST(SUCCESS));
  assert(!MODULE_FUNC(Memory, UniquePtr, int32, IsValid)(&u1));
  assert(MODULE_FUNC(Memory, UniquePtr, int32, IsValid)(&u2));
  assert(*MODULE_FUNC(Memory, UniquePtr, int32, Get)(&u2) == 42);

  /* Test Release */
  int32_t *raw = NULL;
  status = MODULE_FUNC(Memory, UniquePtr, int32, Release)(&u2, &raw);
  assert(status == STATUS_CONST(SUCCESS));
  assert(raw != NULL);
  assert(*raw == 42);
  assert(!MODULE_FUNC(Memory, UniquePtr, int32, IsValid)(&u2));

  /* Re-assign via FromPointer */
  status = MODULE_FUNC(Memory, UniquePtr, int32, FromPointer)(&u1, raw);
  assert(status == STATUS_CONST(SUCCESS));
  assert(*MODULE_FUNC(Memory, UniquePtr, int32, Get)(&u1) == 42);

  /* Destroy */
  MODULE_FUNC(Memory, UniquePtr, int32, Destroy)(&u1);
  assert(!MODULE_FUNC(Memory, UniquePtr, int32, IsValid)(&u1));
  printf("  -> UniquePtr PASSED\n");
}

static void test_shared_and_weak_ptr(void) {
  printf("[Test] SharedPtr & WeakPtr...\n");
  MODULE_TYPED(Memory, SharedPtr, int32) sp1;
  MODULE_TYPED(Memory, SharedPtr, int32) sp2;
  MODULE_TYPED(Memory, WeakPtr, int32) wp;

  OPSTATUS status = MODULE_FUNC(Memory, SharedPtr, int32, Make)(&sp1, 100);
  assert(status == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Memory, SharedPtr, int32, IsValid)(&sp1));
  assert(MODULE_FUNC(Memory, SharedPtr, int32, UseCount)(&sp1) == 1U);
  assert(MODULE_FUNC(Memory, SharedPtr, int32, Unique)(&sp1));
  assert(*MODULE_FUNC(Memory, SharedPtr, int32, Get)(&sp1) == 100);

  /* Clone */
  status = MODULE_FUNC(Memory, SharedPtr, int32, Clone)(&sp1, &sp2);
  assert(status == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Memory, SharedPtr, int32, UseCount)(&sp1) == 2U);
  assert(MODULE_FUNC(Memory, SharedPtr, int32, UseCount)(&sp2) == 2U);
  assert(!MODULE_FUNC(Memory, SharedPtr, int32, Unique)(&sp1));

  /* Downgrade to WeakPtr */
  status = MODULE_FUNC(Memory, SharedPtr, int32, Downgrade)(&sp1, &wp);
  assert(status == STATUS_CONST(SUCCESS));
  assert(!MODULE_FUNC(Memory, WeakPtr, int32, IsExpired)(&wp));
  assert(MODULE_FUNC(Memory, WeakPtr, int32, UseCount)(&wp) == 2U);

  /* Upgrade WeakPtr to new SharedPtr */
  MODULE_TYPED(Memory, SharedPtr, int32) sp3;
  status = MODULE_FUNC(Memory, WeakPtr, int32, Upgrade)(&wp, &sp3);
  assert(status == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Memory, SharedPtr, int32, UseCount)(&sp1) == 3U);
  assert(*MODULE_FUNC(Memory, SharedPtr, int32, Get)(&sp3) == 100);

  /* Destroy strong pointers */
  MODULE_FUNC(Memory, SharedPtr, int32, Destroy)(&sp1);
  MODULE_FUNC(Memory, SharedPtr, int32, Destroy)(&sp2);
  MODULE_FUNC(Memory, SharedPtr, int32, Destroy)(&sp3);

  /* WeakPtr should now be expired */
  assert(MODULE_FUNC(Memory, WeakPtr, int32, IsExpired)(&wp));
  assert(MODULE_FUNC(Memory, WeakPtr, int32, UseCount)(&wp) == 0U);

  /* Upgrade should now fail safely */
  status = MODULE_FUNC(Memory, WeakPtr, int32, Upgrade)(&wp, &sp1);
  assert(status == STATUS_CONST(NOT_FOUND));

  MODULE_FUNC(Memory, WeakPtr, int32, Destroy)(&wp);
  printf("  -> SharedPtr & WeakPtr PASSED\n");
}

static void test_shared_and_weak_ptr_atomic(void) {
  printf("[Test] SharedPtrAtomic & WeakPtrAtomic...\n");
  MODULE_TYPED(Memory, SharedPtrAtomic, int32) asp1;
  MODULE_TYPED(Memory, SharedPtrAtomic, int32) asp2;
  MODULE_TYPED(Memory, WeakPtrAtomic, int32) awp;

  OPSTATUS status = MODULE_FUNC(Memory, SharedPtrAtomic, int32, Make)(&asp1, 555);
  assert(status == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Memory, SharedPtrAtomic, int32, IsValid)(&asp1));
  assert(MODULE_FUNC(Memory, SharedPtrAtomic, int32, UseCount)(&asp1) == 1U);
  assert(MODULE_FUNC(Memory, SharedPtrAtomic, int32, Unique)(&asp1));
  assert(*MODULE_FUNC(Memory, SharedPtrAtomic, int32, Get)(&asp1) == 555);

  /* Clone atomic */
  status = MODULE_FUNC(Memory, SharedPtrAtomic, int32, Clone)(&asp1, &asp2);
  assert(status == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Memory, SharedPtrAtomic, int32, UseCount)(&asp1) == 2U);
  assert(MODULE_FUNC(Memory, SharedPtrAtomic, int32, UseCount)(&asp2) == 2U);

  /* Downgrade atomic */
  status = MODULE_FUNC(Memory, SharedPtrAtomic, int32, Downgrade)(&asp1, &awp);
  assert(status == STATUS_CONST(SUCCESS));
  assert(!MODULE_FUNC(Memory, WeakPtrAtomic, int32, IsExpired)(&awp));
  assert(MODULE_FUNC(Memory, WeakPtrAtomic, int32, UseCount)(&awp) == 2U);

  /* Upgrade atomic */
  MODULE_TYPED(Memory, SharedPtrAtomic, int32) asp3;
  status = MODULE_FUNC(Memory, WeakPtrAtomic, int32, Upgrade)(&awp, &asp3);
  assert(status == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Memory, SharedPtrAtomic, int32, UseCount)(&asp1) == 3U);
  assert(*MODULE_FUNC(Memory, SharedPtrAtomic, int32, Get)(&asp3) == 555);

  /* Destroy strong references */
  MODULE_FUNC(Memory, SharedPtrAtomic, int32, Destroy)(&asp1);
  MODULE_FUNC(Memory, SharedPtrAtomic, int32, Destroy)(&asp2);
  MODULE_FUNC(Memory, SharedPtrAtomic, int32, Destroy)(&asp3);

  /* WeakPtrAtomic is expired */
  assert(MODULE_FUNC(Memory, WeakPtrAtomic, int32, IsExpired)(&awp));
  assert(MODULE_FUNC(Memory, WeakPtrAtomic, int32, UseCount)(&awp) == 0U);

  status = MODULE_FUNC(Memory, WeakPtrAtomic, int32, Upgrade)(&awp, &asp1);
  assert(status == STATUS_CONST(NOT_FOUND));

  MODULE_FUNC(Memory, WeakPtrAtomic, int32, Destroy)(&awp);
  printf("  -> SharedPtrAtomic & WeakPtrAtomic PASSED\n");
}

static void test_intrusive_ptr(void) {
  printf("[Test] IntrusivePtr...\n");
  void *rawMem = NULL;
  OPSTATUS status = MODULE_FUNC(Memory, Alloc, Bytes)(&rawMem, sizeof(TestNode));
  assert(status == STATUS_CONST(SUCCESS));
  TestNode *node = (TestNode *)rawMem;
  node->value = 999;

  MODULE_TYPED(Memory, IntrusivePtr, Node) ip1;
  MODULE_TYPED(Memory, IntrusivePtr, Node) ip2;

  status = MODULE_FUNC(Memory, IntrusivePtr, Node, FromPointer)(&ip1, node);
  assert(status == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Memory, IntrusivePtr, Node, UseCount)(&ip1) == 1U);
  assert(MODULE_FUNC(Memory, IntrusivePtr, Node, Unique)(&ip1));
  assert(MODULE_FUNC(Memory, IntrusivePtr, Node, Get)(&ip1)->value == 999);

  /* Clone intrusive */
  status = MODULE_FUNC(Memory, IntrusivePtr, Node, Clone)(&ip1, &ip2);
  assert(status == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Memory, IntrusivePtr, Node, UseCount)(&ip1) == 2U);
  assert(MODULE_FUNC(Memory, IntrusivePtr, Node, UseCount)(&ip2) == 2U);

  /* Destroy ip1: refCount decrements to 1 */
  MODULE_FUNC(Memory, IntrusivePtr, Node, Destroy)(&ip1);
  assert(!MODULE_FUNC(Memory, IntrusivePtr, Node, IsValid)(&ip1));
  assert(MODULE_FUNC(Memory, IntrusivePtr, Node, UseCount)(&ip2) == 1U);

  /* Destroy ip2: refCount decrements to 0 and frees node */
  MODULE_FUNC(Memory, IntrusivePtr, Node, Destroy)(&ip2);
  assert(!MODULE_FUNC(Memory, IntrusivePtr, Node, IsValid)(&ip2));
  printf("  -> IntrusivePtr PASSED\n");
}

int main(void) {
  test_unique_ptr();
  test_shared_and_weak_ptr();
  test_shared_and_weak_ptr_atomic();
  test_intrusive_ptr();
  printf("\nAll Smart Pointer tests executed successfully!\n");
  return 0;
}

