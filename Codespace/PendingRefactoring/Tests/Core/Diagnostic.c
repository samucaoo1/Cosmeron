#include "../../Cosmeron/Core/Diagnostic/Context.h"
#include "../../Cosmeron/Core/Diagnostic/Stacktrace.h"
#include "../../Cosmeron/Core/Diagnostic/Format.h"
#include "../../Cosmeron/Core/Condition/Error.h"
#include "../../Cosmeron/Core/Outcome/Status.h"

#include <assert.h>
#include <string.h>

int main(void) {
  /* 1. TDiagnosticContext operations and properties */
  DIAGNOSTIC_TYPE(Context) ctx = {0};
  assert(TDiagnosticContext_Init(&ctx) == STATUS_CONST(SUCCESS));
  assert(TDiagnosticContext_HasOrigin(&ctx) == false);

  int dummyData = 1234;
  assert(TDiagnosticContext_Create(&ctx, "test_file.c", 42, "test_func",
                                   "Sample description", &dummyData) ==
         STATUS_CONST(SUCCESS));
  assert(TDiagnosticContext_HasOrigin(&ctx) == true);
  assert(strcmp(TDiagnosticContext_File(&ctx), "test_file.c") == 0);
  assert(TDiagnosticContext_Line(&ctx) == 42);
  assert(strcmp(TDiagnosticContext_Function(&ctx), "test_func") == 0);
  assert(strcmp(TDiagnosticContext_Description(&ctx), "Sample description") == 0);
  assert(TDiagnosticContext_UserData(&ctx) == &dummyData);

  /* Calling via OBJECT_CALL on TDiagnosticContext */
  assert(OBJECT_CALL_0(&ctx, HasOrigin) == true);
  assert(strcmp(OBJECT_CALL_0(&ctx, File), "test_file.c") == 0);
  assert(OBJECT_CALL_0(&ctx, Line) == 42);

  /* Clear context */
  TDiagnosticContext_Clear(&ctx);
  assert(TDiagnosticContext_HasOrigin(&ctx) == false);
  assert(TDiagnosticContext_File(&ctx) == NULL);

  /* Re-initialize for subsequent tests */
  assert(TDiagnosticContext_Create(&ctx, "test_file.c", 42, "test_func",
                                   "Sample description", &dummyData) ==
         STATUS_CONST(SUCCESS));

  /* 2. Format package functions */
  ERROR_TYPE(Error) err1, err2, err3;
  assert(TError_Create(&err1, STATUS_CONST(NOT_FOUND), "Resource missing") ==
         STATUS_CONST(SUCCESS));
  assert(TError_Create(&err2, STATUS_CONST(GENERIC_ERROR), "Database lookup failed") ==
         STATUS_CONST(SUCCESS));
  assert(TError_Create(&err3, STATUS_CONST(TIMEOUT), "Request timed out") ==
         STATUS_CONST(SUCCESS));

  char fmtBuf[512];
  size_t written = 0;

  /* Format_Simple */
  assert(Diagnostic_Format_Simple(&err1, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf, "[NOT_FOUND] Resource missing") == 0);
  assert(written == strlen(fmtBuf));

  /* Format_Simple via PACKAGE_CALL */
  assert(PACKAGE_CALL(Diagnostic_Format, Simple, &err1, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf, "[NOT_FOUND] Resource missing") == 0);

  /* Format_Simple buffer overflow/truncation */
  char smallBuf[10];
  assert(Diagnostic_Format_Simple(&err1, smallBuf, sizeof(smallBuf), &written) ==
         STATUS_CONST(INSUFFICIENT_SPACE));
  assert(strlen(smallBuf) == 9);
  assert(written == 9);
  assert(smallBuf[9] == '\0');

  /* Format_Context */
  assert(Diagnostic_Format_Context(&ctx, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf, "at test_file.c:42 in test_func() - Sample description") == 0);

  DIAGNOSTIC_TYPE(Context) emptyCtx = {0};
  assert(Diagnostic_Format_Context(&emptyCtx, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf, "<no origin>") == 0);

  /* Format_Detailed: with and without context */
  assert(Diagnostic_Format_Detailed(&err1, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf, "[NOT_FOUND] Resource missing") == 0);

  ERROR_TYPE(Error) detailedErr;
  assert(TError_Create(&detailedErr, STATUS_CONST(INVALID_ARGUMENT), "Custom invalid arg") ==
         STATUS_CONST(SUCCESS));
  assert(TError_SetContext(&detailedErr, &ctx) == STATUS_CONST(SUCCESS));
  assert(Diagnostic_Format_Detailed(&detailedErr, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(fmtBuf,
                "[INVALID_ARGUMENT] Custom invalid arg (at test_file.c:42 in test_func() - Sample description)") == 0);

  /* Format_Chain */
  assert(TError_SetCause(&err2, &err1) == STATUS_CONST(SUCCESS));
  assert(TError_SetCause(&err3, &err2) == STATUS_CONST(SUCCESS));
  assert(Diagnostic_Format_Chain(&err3, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strstr(fmtBuf, "[#0] [TIMEOUT] Request timed out") != NULL);
  assert(strstr(fmtBuf, "caused by: [#1] [GENERIC_ERROR] Database lookup failed") != NULL);
  assert(strstr(fmtBuf, "caused by: [#2] [NOT_FOUND] Resource missing") != NULL);

  /* Invalid arguments to Format */
  assert(Diagnostic_Format_Simple(NULL, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(Diagnostic_Format_Simple(&err1, NULL, sizeof(fmtBuf), &written) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(Diagnostic_Format_Simple(&err1, fmtBuf, 0, &written) ==
         STATUS_CONST(INVALID_ARGUMENT));

  /* 3. TDiagnosticStacktrace object operations */
  DIAGNOSTIC_TYPE(Stacktrace) trace = {0};
  assert(TDiagnosticStacktrace_Init(&trace) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&trace, FrameCount) == 0);

  /* Capture stack frames */
  assert(OBJECT_CALL(&trace, Capture, 0, 16) == STATUS_CONST(SUCCESS));
  size_t frameCount = OBJECT_CALL_0(&trace, FrameCount);
  assert(frameCount > 0);
  assert(frameCount <= 16);

  /* FrameAt */
  void *frameAddr = NULL;
  assert(OBJECT_CALL(&trace, FrameAt, 0, &frameAddr) == STATUS_CONST(SUCCESS));
  assert(frameAddr != NULL);
  assert(OBJECT_CALL(&trace, FrameAt, 999, &frameAddr) == STATUS_CONST(OUT_OF_RANGE));

  /* Format stacktrace */
  char traceBuf[1024];
  assert(OBJECT_CALL(&trace, Format, traceBuf, sizeof(traceBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(written > 0);
  assert(strstr(traceBuf, "#00  0x") != NULL);

  /* Clear */
  OBJECT_CALL_0(&trace, Clear);
  assert(OBJECT_CALL_0(&trace, FrameCount) == 0);
  assert(OBJECT_CALL(&trace, Format, traceBuf, sizeof(traceBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(traceBuf, "<empty stacktrace>") == 0);

  return 0;
}

