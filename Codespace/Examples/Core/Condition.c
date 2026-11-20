#include "../../Cosmeron/Core/Condition/Defer.h"
#include "../../Cosmeron/Core/Condition/Error.h"
#include "../../Cosmeron/Core/Condition/Status.h"

#include <stdio.h>

static void Example_Cleanup(void *context) {
  int *resource = (int *)context;
  if (resource) {
    *resource = 0;
    printf("Deferred cleanup executed.\n");
  }
}

static OPSTATUS Example_ValidatePositive(int value,
                                         MODULE_TYPE(Condition, Error) *
                                             outError) {
  if (value <= 0) {
    if (outError) {
      (void)MODULE_FUNC(Condition, Error, Create)(
          outError, STATUS_CONST(INVALID_ARGUMENT), "Value must be positive");
    }
    return STATUS_CONST(INVALID_ARGUMENT);
  }
  return STATUS_CONST(SUCCESS);
}

int main(void) {
  /* 1. Rich Error Object */
  MODULE_TYPE(Condition, Error) err = {0};
  MODULE_FUNC(Condition, Error, Init)(&err);

  if (Example_ValidatePositive(-5, &err) != STATUS_CONST(SUCCESS)) {
    printf("Error [%s]: %s\n",
           MODULE_FUNC(Condition, Status, Name)(OBJECT_CALL_0(&err, Code)),
           OBJECT_CALL_0(&err, Message));
  }

  /* 2. Scoped Defer */
  int handle = 123;
  {
    DEFER(Example_Cleanup, &handle);
    printf("Using handle=%d inside defer scope...\n", handle);
  }
  printf("After defer scope, handle=%d\n", handle);

  return 0;
}
