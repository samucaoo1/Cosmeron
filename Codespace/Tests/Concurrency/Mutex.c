#include <assert.h>
#include "../../Cosmeron/Modules/Concurrency/Synchronization/Mutex.h"
int main(void) {
  TMutex mutex;
  bool acquired = false;
  assert(MUTEX_FUNC(Init)(&mutex) == STATUS_CONST(SUCCESS));
  assert(MUTEX_FUNC(Lock)(&mutex) == STATUS_CONST(SUCCESS));
  assert(MUTEX_FUNC(Unlock)(&mutex) == STATUS_CONST(SUCCESS));
  assert(MUTEX_FUNC(TryLock)(&mutex, &acquired) == STATUS_CONST(SUCCESS));
  assert(acquired);
  assert(MUTEX_FUNC(Unlock)(&mutex) == STATUS_CONST(SUCCESS));
  assert(MUTEX_FUNC(Destroy)(&mutex) == STATUS_CONST(SUCCESS));
  return 0;
}
