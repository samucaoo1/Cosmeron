#include <assert.h>
#include "../../Cosmeron/Modules/Concurrency/Synchronization/Once.h"
static int calls = 0;
static void initialize(void) { ++calls; }
int main(void) {
  CONCURRENCY_TYPE(TOnce) once = ONCE_INIT;
  assert(ONCE_FUNC(Call)(&once, initialize) == STATUS_CONST(SUCCESS));
  assert(ONCE_FUNC(Call)(&once, initialize) == STATUS_CONST(SUCCESS));
  assert(calls == 1);
  return 0;
}
