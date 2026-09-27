#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"
#include <assert.h>
int main(void) {
  TSemaphore semaphore;
  bool acquired = false;
  assert(SEMAPHORE_FUNC(Init)(&semaphore, 1) == STATUS_CONST(SUCCESS));
  assert(SEMAPHORE_FUNC(TryWait)(&semaphore, &acquired) == STATUS_CONST(SUCCESS));
  assert(acquired);
  assert(SEMAPHORE_FUNC(TryWait)(&semaphore, &acquired) == STATUS_CONST(SUCCESS));
  assert(!acquired);
  assert(SEMAPHORE_FUNC(Post)(&semaphore) == STATUS_CONST(SUCCESS));
  assert(SEMAPHORE_FUNC(Wait)(&semaphore) == STATUS_CONST(SUCCESS));
  assert(SEMAPHORE_FUNC(Destroy)(&semaphore) == STATUS_CONST(SUCCESS));
  return 0;
}
