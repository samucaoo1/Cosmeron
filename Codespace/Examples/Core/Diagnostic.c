#include "../../Cosmeron/Core/Condition/Error.h"
#include "../../Cosmeron/Core/Diagnostic/Context.h"
#include "../../Cosmeron/Core/Diagnostic/Format.h"

#include <stdio.h>

int main(void) {
  MODULE_TYPE(Diagnostic, Context) ctx = {0};
  MODULE_FUNC(Diagnostic, Context, Init)(&ctx);
  (void)MODULE_FUNC(Diagnostic, Context, Create)(
      &ctx, __FILE__, (uint32_t)__LINE__, __func__, "Loading user profile",
      NULL);

  MODULE_TYPE(Condition, Error) err = {0};
  MODULE_FUNC(Condition, Error, Init)(&err);
  (void)MODULE_FUNC(Condition, Error, Create)(&err, STATUS_CONST(NOT_FOUND),
                                              "Profile record missing");

  char buffer[256] = {0};
  size_t written = 0U;

  if (MODULE_FUNC(Diagnostic, Format, Simple)(&err, buffer, sizeof(buffer),
                                              &written) ==
      STATUS_CONST(SUCCESS)) {
    printf("Simple:  %s\n", buffer);
  }

  if (MODULE_FUNC(Diagnostic, Format, Context)(&ctx, buffer, sizeof(buffer),
                                               &written) ==
      STATUS_CONST(SUCCESS)) {
    printf("Context: %s\n", buffer);
  }

  return 0;
}
