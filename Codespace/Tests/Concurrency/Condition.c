#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"
#include <assert.h>

typedef struct {
  TMutex mutex;
  TCondition condition;
  bool ready;
} TState;

static void worker(void *argument) {
  TState *state = (TState *)argument;
  MUTEX_FUNC(Lock)(&state->mutex);
  state->ready = true;
  CONDITION_FUNC(NotifyOne)(&state->condition);
  MUTEX_FUNC(Unlock)(&state->mutex);
}

int main(void) {
  TState state;
  TThread thread;
  state.ready = false;

  assert(MUTEX_FUNC(Init)(&state.mutex) == STATUS_CONST(SUCCESS));
  assert(CONDITION_FUNC(Init)(&state.condition) == STATUS_CONST(SUCCESS));
  assert(CONDITION_FUNC(NotifyAll)(&state.condition) == STATUS_CONST(SUCCESS));
  assert(THREAD_FUNC(Create)(&thread, worker, &state) == STATUS_CONST(SUCCESS));
  MUTEX_FUNC(Lock)(&state.mutex);
  while (!state.ready)
    assert(CONDITION_FUNC(Wait)(&state.condition, &state.mutex) ==
           STATUS_CONST(SUCCESS));
  MUTEX_FUNC(Unlock)(&state.mutex);
  assert(THREAD_FUNC(Join)(&thread) == STATUS_CONST(SUCCESS));
  assert(CONDITION_FUNC(Destroy)(&state.condition) == STATUS_CONST(SUCCESS));
  assert(MUTEX_FUNC(Destroy)(&state.mutex) == STATUS_CONST(SUCCESS));
  return 0;
}
