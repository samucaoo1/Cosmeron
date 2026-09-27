#include "../../Cosmeron/Modules/Concurrency/Concurrency.h"
#include <assert.h>

#define ITEM_COUNT 50000

typedef struct TQueue_State {
  TMutex mutex;
  TCondition condition;
  unsigned produced;
  unsigned consumed;
  bool done;
} TQueue_State;

static void producer(void *argument) {
  TQueue_State *state = (TQueue_State *)argument;
  for (unsigned i = 0; i < ITEM_COUNT; ++i) {
    MUTEX_FUNC(Lock)(&state->mutex);
    ++state->produced;
    CONDITION_FUNC(NotifyOne)(&state->condition);
    MUTEX_FUNC(Unlock)(&state->mutex);
  }
  MUTEX_FUNC(Lock)(&state->mutex);
  state->done = true;
  CONDITION_FUNC(NotifyAll)(&state->condition);
  MUTEX_FUNC(Unlock)(&state->mutex);
  return;
}

static void consumer(void *argument) {
  TQueue_State *state = (TQueue_State *)argument;
  MUTEX_FUNC(Lock)(&state->mutex);
  for (;;) {
    while (state->consumed == state->produced && !state->done)
      assert(CONDITION_FUNC(Wait)(&state->condition, &state->mutex) == STATUS_CONST(SUCCESS));
    if (state->consumed < state->produced) {
      ++state->consumed;
      continue;
    }
    if (state->done) break;
  }
  MUTEX_FUNC(Unlock)(&state->mutex);
  return;
}

int main(void) {
  TQueue_State state = {0};
  TThread producer_thread;
  TThread consumer_thread;
  assert(MUTEX_FUNC(Init)(&state.mutex) == STATUS_CONST(SUCCESS));
  assert(CONDITION_FUNC(Init)(&state.condition) == STATUS_CONST(SUCCESS));
  assert(THREAD_FUNC(Create)(&consumer_thread, consumer, &state) == STATUS_CONST(SUCCESS));
  assert(THREAD_FUNC(Create)(&producer_thread, producer, &state) == STATUS_CONST(SUCCESS));
  assert(THREAD_FUNC(Join)(&producer_thread) == STATUS_CONST(SUCCESS));
  assert(THREAD_FUNC(Join)(&consumer_thread) == STATUS_CONST(SUCCESS));
  assert(state.produced == ITEM_COUNT);
  assert(state.consumed == ITEM_COUNT);
  assert(CONDITION_FUNC(Destroy)(&state.condition) == STATUS_CONST(SUCCESS));
  assert(MUTEX_FUNC(Destroy)(&state.mutex) == STATUS_CONST(SUCCESS));
  return 0;
}
