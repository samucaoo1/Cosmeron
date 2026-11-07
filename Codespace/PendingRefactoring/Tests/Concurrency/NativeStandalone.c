#include "../../Cosmeron/Modules/Concurrency/Native.h"
static void Work(void *argument) {
  int *value = (int *)argument;
  *value = 42;
}
int main(void) {
  CONCURRENCY_TYPE(TThread) thread = {0};
  int value = 0;
  if (THREAD_FUNC(Create)(&thread, Work, &value) != STATUS_CONST(SUCCESS))
    return 1;
  if (THREAD_FUNC(Join)(&thread) != STATUS_CONST(SUCCESS))
    return 2;
  return value == 42 ? 0 : 3;
}
