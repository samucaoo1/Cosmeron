#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"
#include <assert.h>
int main(void) {
  TRWLock lock;
  bool acquired = false;
  assert(RWLOCK_FUNC(Init)(&lock) == STATUS_CONST(SUCCESS));
  assert(RWLOCK_FUNC(ReadLock)(&lock) == STATUS_CONST(SUCCESS));
  assert(RWLOCK_FUNC(ReadUnlock)(&lock) == STATUS_CONST(SUCCESS));
  assert(RWLOCK_FUNC(WriteLock)(&lock) == STATUS_CONST(SUCCESS));
  assert(RWLOCK_FUNC(WriteUnlock)(&lock) == STATUS_CONST(SUCCESS));
  assert(RWLOCK_FUNC(TryWriteLock)(&lock, &acquired) == STATUS_CONST(SUCCESS));
  assert(acquired);
  assert(RWLOCK_FUNC(WriteUnlock)(&lock) == STATUS_CONST(SUCCESS));
  assert(RWLOCK_FUNC(TryReadLock)(&lock, &acquired) == STATUS_CONST(SUCCESS));
  assert(acquired);
  assert(RWLOCK_FUNC(ReadUnlock)(&lock) == STATUS_CONST(SUCCESS));
  assert(RWLOCK_FUNC(Destroy)(&lock) == STATUS_CONST(SUCCESS));
  return 0;
}
