#include "../../../Cosmeron/Core/Diagnostic/Context.h"
#include "../../../Cosmeron/Core/Diagnostic/Stacktrace.h"
#include "../../../Cosmeron/Core/Diagnostic/Format.h"
#include "../../../Cosmeron/Core/Condition/Error.h"
#include "../../../Cosmeron/Core/Outcome/Status.h"

#include <assert.h>
#include <string.h>

int main(void) {
  /* 1. TDiagnosticContext operations and properties */
  MODULE_TYPE(Diagnostic, Context) ctx = {0};
  assert(MODULE_FUNC(Diagnostic, Context, Init)(&ctx) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&ctx, HasOrigin) == false);

  int dummyData = 1234;
  assert(MODULE_FUNC(Diagnostic, Context, Create)(&ctx, "test_file.c", 42, "test_func",
                                                  "Sample description", &dummyData) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&ctx, HasOrigin) == true);
  assert(strcmp(OBJECT_CALL_0(&ctx, File), "test_file.c") == 0);
  assert(OBJECT_CALL_0(&ctx, Line) == 42);
  assert(strcmp(OBJECT_CALL_0(&ctx, Function), "test_func") == 0);
  assert(strcmp(OBJECT_CALL_0(&ctx, Description), "Sample description") == 0);
  assert(OBJECT_CALL_0(&ctx, UserData) == &dummyData);

  /* Clear context */
  MODULE_FUNC(Diagnostic, Context, Clear)(&ctx);
  assert(OBJECT_CALL_0(&ctx, HasOrigin) == false);
  assert(OBJECT_CALL_0(&ctx, File) == NULL);

  /* Re-initialize for subsequent tests */
  assert(MODULE_FUNC(Diagnostic, Context, Create)(&ctx, "test_file.c", 42, "test_func",
                                                  "Sample description", &dummyData) ==
         STATUS_CONST(SUCCESS));

  /* 2. Format package functions */
  MODULE_TYPE(Condition, Error) err1, err2, err3;
  MODULE_FUNC(Condition, Error, Init)(&err1);
  MODULE_FUNC(Condition, Error, Init)(&err2);
  MODULE_FUNC(Condition, Error, Init)(&err3);

  assert(MODULE_FUNC(Condition, Error, Create)(&err1, STATUS_CONST(NOT_FOUND), "Resource missing") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, Create)(&err2, STATUS_CONST(GENERIC_ERROR), "Database lookup failed") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, Create)(&err3, STATUS_CONST(TIMEOUT), "Request timed out") ==
         STATUS_CONST(SUCCESS));

  char fmtBuf[512];
  size_t written = 0;

  /* Format_Simple */
  assert(MODULE_FUNC(Diagnostic, Format, Simple)(&err1, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf, "[NOT_FOUND] Resource missing") == 0);
  assert(written == strlen(fmtBuf));

  /* Format_Simple buffer overflow/truncation */
  char smallBuf[10];
  assert(MODULE_FUNC(Diagnostic, Format, Simple)(&err1, smallBuf, sizeof(smallBuf), &written) ==
         STATUS_CONST(INSUFFICIENT_SPACE));
  assert(strlen(smallBuf) == 9);
  assert(written == 9);
  assert(smallBuf[9] == '\0');

  /* Format_Context */
  assert(MODULE_FUNC(Diagnostic, Format, Context)(&ctx, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf, "at test_file.c:42 in test_func() - Sample description") == 0);

  MODULE_TYPE(Diagnostic, Context) emptyCtx = {0};
  MODULE_FUNC(Diagnostic, Context, Init)(&emptyCtx);
  assert(MODULE_FUNC(Diagnostic, Format, Context)(&emptyCtx, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf, "<no origin>") == 0);

  /* Format_Detailed: with and without context */
  assert(MODULE_FUNC(Diagnostic, Format, Detailed)(&err1, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf, "[NOT_FOUND] Resource missing") == 0);

  MODULE_TYPE(Condition, Error) detailedErr;
  MODULE_FUNC(Condition, Error, Init)(&detailedErr);
  assert(MODULE_FUNC(Condition, Error, Create)(&detailedErr, STATUS_CONST(INVALID_ARGUMENT), "Custom invalid arg") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, SetContext)(&detailedErr, &ctx) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Diagnostic, Format, Detailed)(&detailedErr, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf,
                "[INVALID_ARGUMENT] Custom invalid arg (at test_file.c:42 in test_func() - Sample description)") == 0);

  /* Format_Chain */
  assert(MODULE_FUNC(Condition, Error, SetCause)(&err2, &err1) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, SetCause)(&err3, &err2) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Diagnostic, Format, Chain)(&err3, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strstr(fmtBuf, "[#0] [TIMEOUT] Request timed out") != NULL);
  assert(strstr(fmtBuf, "[#1] [GENERIC_ERROR] Database lookup failed") != NULL);
  assert(strstr(fmtBuf, "[#2] [NOT_FOUND] Resource missing") != NULL);

  /* 3. Stacktrace object operations */
  MODULE_TYPE(Diagnostic, Stacktrace) st = {0};
  assert(MODULE_FUNC(Diagnostic, Stacktrace, Init)(&st) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&st, FrameCount) == 0);

  assert(OBJECT_CALL(&st, Capture, 0, 10) == STATUS_CONST(SUCCESS));
  size_t capturedCount = OBJECT_CALL_0(&st, FrameCount);
  assert(capturedCount > 0);
  assert(capturedCount <= 10);

  void *framePtr = NULL;
  assert(OBJECT_CALL(&st, FrameAt, 0, &framePtr) == STATUS_CONST(SUCCESS));
  assert(framePtr != NULL);

  assert(OBJECT_CALL(&st, FrameAt, 999, &framePtr) == STATUS_CONST(OUT_OF_RANGE));

  char stBuf[1024];
  assert(OBJECT_CALL(&st, Format, stBuf, sizeof(stBuf), &written) == STATUS_CONST(SUCCESS));
  assert(written > 0);
  assert(stBuf[0] == '#');

  OBJECT_CALL_0(&st, Clear);
  assert(OBJECT_CALL_0(&st, FrameCount) == 0);
  assert(OBJECT_CALL(&st, Format, stBuf, sizeof(stBuf), &written) == STATUS_CONST(SUCCESS));
  assert(strcmp(stBuf, "<empty stacktrace>") == 0);

  return 0;
}
