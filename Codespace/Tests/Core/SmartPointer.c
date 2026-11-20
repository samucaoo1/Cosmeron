#include "../../Cosmeron/Core/Memory/Pointer/Pointer.h"
#include <assert.h>
#include <stdint.h>

TEMPLATE_Pointer_Unique(int, int);
TEMPLATE_Pointer_Weak(int, int);
TEMPLATE_Pointer_Shared(int, int);
TEMPLATE_Pointer_AtomicWeak(int, int);
TEMPLATE_Pointer_AtomicShared(int, int);

struct TestIntrusiveNode {
  size_t refCount;
  int value;
};
TEMPLATE_Pointer_Intrusive(TestNode, struct TestIntrusiveNode);

static void Test_UniquePtr(void) {
  MODULE_TYPED(Pointer, Unique, int) u1 = {0};
  MODULE_FUNC(Pointer, Unique, int, Init)(&u1);
  assert(!OBJECT_CALL_0(&u1, IsValid));

  /* Alloc */
  assert(OBJECT_CALL(&u1, Alloc, 42) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&u1, IsValid));
  assert(OBJECT_CALL_0(&u1, Get) != NULL);
  assert(*OBJECT_CALL_0(&u1, Get) == 42);

  /* GetMutable */
  int *mut = OBJECT_CALL_0(&u1, GetMutable);
  assert(mut != NULL);
  *mut = 84;
  assert(*OBJECT_CALL_0(&u1, Get) == 84);

  /* AllocZero */
  assert(OBJECT_CALL_0(&u1, AllocZero) == STATUS_CONST(SUCCESS));
  assert(*OBJECT_CALL_0(&u1, Get) == 0);

  /* Move */
  MODULE_TYPED(Pointer, Unique, int) u2 = {0};
  MODULE_FUNC(Pointer, Unique, int, Init)(&u2);
  assert(OBJECT_CALL(&u2, Move, &u1) == STATUS_CONST(SUCCESS));
  assert(!OBJECT_CALL_0(&u1, IsValid));
  assert(OBJECT_CALL_0(&u2, IsValid));
  assert(*OBJECT_CALL_0(&u2, Get) == 0);

  /* Free */
  OBJECT_CALL_0(&u2, Free);
  assert(!OBJECT_CALL_0(&u2, IsValid));

  /* FromPointer & Reset */
  void *raw = NULL;
  assert(MODULE_FUNC(Memory, Alloc, Bytes)(&raw, sizeof(int)) == STATUS_CONST(SUCCESS));
  *(int *)raw = 123;
  assert(OBJECT_CALL(&u1, FromPointer, (int *)raw) == STATUS_CONST(SUCCESS));
  assert(*OBJECT_CALL_0(&u1, Get) == 123);

  OBJECT_CALL_0(&u1, Free);
}

static void Test_SharedPtrAndWeakPtr(void) {
  MODULE_TYPED(Pointer, Shared, int) s1 = {0};
  MODULE_FUNC(Pointer, Shared, int, Init)(&s1);

  /* Alloc */
  assert(OBJECT_CALL(&s1, Alloc, 100) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&s1, IsValid));
  assert(OBJECT_CALL_0(&s1, UseCount) == 1);
  assert(*OBJECT_CALL_0(&s1, Get) == 100);

  /* Clone */
  MODULE_TYPED(Pointer, Shared, int) s2 = {0};
  assert(OBJECT_CALL(&s1, Clone, &s2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&s1, UseCount) == 2);
  assert(OBJECT_CALL_0(&s2, UseCount) == 2);
  assert(*OBJECT_CALL_0(&s2, Get) == 100);

  /* WeakPtr Downgrade & Upgrade */
  MODULE_TYPED(Pointer, Weak, int) w1 = {0};
  assert(OBJECT_CALL(&s1, Downgrade, &w1) == STATUS_CONST(SUCCESS));
  assert(!OBJECT_CALL_0(&w1, IsExpired));
  assert(OBJECT_CALL_0(&w1, UseCount) == 2);

  /* Free s1 */
  OBJECT_CALL_0(&s1, Free);
  assert(!OBJECT_CALL_0(&s1, IsValid));
  assert(OBJECT_CALL_0(&s2, UseCount) == 1);
  assert(OBJECT_CALL_0(&w1, UseCount) == 1);

  /* Upgrade from weak */
  MODULE_TYPED(Pointer, Shared, int) s3 = {0};
  assert(OBJECT_CALL(&w1, Upgrade, &s3) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&s3, UseCount) == 2);

  OBJECT_CALL_0(&s2, Free);
  OBJECT_CALL_0(&s3, Free);

  /* Now weak pointer is expired */
  assert(OBJECT_CALL_0(&w1, IsExpired));
  OBJECT_CALL_0(&w1, Free);
}

static void Test_SharedPtrAtomic(void) {
  MODULE_TYPED(Pointer, AtomicShared, int) sa1 = {0};
  MODULE_FUNC(Pointer, AtomicShared, int, Init)(&sa1);

  assert(OBJECT_CALL(&sa1, Alloc, 999) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&sa1, IsValid));
  assert(OBJECT_CALL_0(&sa1, UseCount) == 1);
  assert(*OBJECT_CALL_0(&sa1, Get) == 999);

  MODULE_TYPED(Pointer, AtomicShared, int) sa2 = {0};
  assert(OBJECT_CALL(&sa1, Clone, &sa2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&sa1, UseCount) == 2);

  OBJECT_CALL_0(&sa1, Free);
  assert(OBJECT_CALL_0(&sa2, UseCount) == 1);
  OBJECT_CALL_0(&sa2, Free);
}

static void Test_IntrusivePtr(void) {
  void *raw = NULL;
  assert(MODULE_FUNC(Memory, Alloc, Bytes)(&raw, sizeof(struct TestIntrusiveNode)) == STATUS_CONST(SUCCESS));
  struct TestIntrusiveNode *node = (struct TestIntrusiveNode *)raw;
  node->refCount = 0;
  node->value = 555;

  MODULE_TYPED(Pointer, Intrusive, TestNode) ip1 = {0};
  MODULE_FUNC(Pointer, Intrusive, TestNode, Init)(&ip1);
  assert(OBJECT_CALL(&ip1, FromPointer, node) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&ip1, UseCount) == 1);

  MODULE_TYPED(Pointer, Intrusive, TestNode) ip2 = {0};
  assert(OBJECT_CALL(&ip1, Clone, &ip2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&ip1, UseCount) == 2);

  OBJECT_CALL_0(&ip1, Free);
  assert(OBJECT_CALL_0(&ip2, UseCount) == 1);
  OBJECT_CALL_0(&ip2, Free);
}

int main(void) {
  Test_UniquePtr();
  Test_SharedPtrAndWeakPtr();
  Test_SharedPtrAtomic();
  Test_IntrusivePtr();
  return 0;
}
