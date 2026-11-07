/*
 * Probe whether C11 threads are provided without an additional link library.
 * A successful run verifies actual thrd_create / thrd_join semantics, not
 * merely the presence of <threads.h> in the compiler sysroot.
 *
 * This does NOT change Cosmeron's public pthread-backed API.
 */
#include <threads.h>
static int worker(void *arg) {
  int *output = (int *)arg;
  *output = 7;
  return 11;
}
int main(void) {
  thrd_t thread;
  int output = 0, result = 0;
  if (thrd_create(&thread, worker, &output) != thrd_success)
    return 1;
  if (thrd_join(thread, &result) != thrd_success)
    return 2;
  return output == 7 && result == 11 ? 0 : 3;
}
