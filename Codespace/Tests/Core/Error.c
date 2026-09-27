#include "../../Cosmeron/Core/Error/Panic.h"
#include "../../Cosmeron/Core/Error/Status.h"

#define STATUS_TEST_COUNT(NAME, MESSAGE) +1
enum { Error_Test_StatusCount = 0 STATUS_TABLE(STATUS_TEST_COUNT) };
#undef STATUS_TEST_COUNT

static OPSTATUS succeed(void) { return STATUS_CONST(SUCCESS); }
static OPSTATUS fail(void) { return STATUS_CONST(INVALID_ARGUMENT); }

int main(void) {
  if (succeed() != STATUS_CONST(SUCCESS))
    return 1;
  if (fail() != STATUS_CONST(INVALID_ARGUMENT))
    return 2;
  if (STATUS_NS(SUCCESS) != STATUS_CONST(SUCCESS))
    return 3;
  if (Error_Test_StatusCount != 15)
    return 4;

  (void)&PANIC_INTERNAL_FUNC(Trigger);
  (void)&PANIC_INTERNAL_FUNC(Stacktrace);
  return 0;
}
