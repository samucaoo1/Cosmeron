#include "../../Cosmeron/Core/Outcome/Status.h"
#include "../../Cosmeron/Core/Outcome/Result.h"
#include "../../Cosmeron/Core/Condition/Panic.h"
#include "../../Cosmeron/Core/Condition/Error.h"
#include "../../Cosmeron/Core/Condition/Exception.h"
#include "../../Cosmeron/Core/Diagnostic/Context.h"
#include "../../Cosmeron/Core/Diagnostic/Format.h"
#include "../../Cosmeron/Core/Diagnostic/Stacktrace.h"

#include <assert.h>
#include <string.h>

#define STATUS_TEST_COUNT(ROW) +1
enum { Error_Test_StatusCount = 0 STATUS_TABLE(STATUS_TEST_COUNT) };
#undef STATUS_TEST_COUNT

static const char *const status_messages[] = {
#define STATUS_MSG_ITEM(ROW)                                                   \
  [STATUS_CONST(STATUS_GET(NAME, ROW))] = STATUS_GET(MESSAGE, ROW),
    STATUS_TABLE(STATUS_MSG_ITEM)
#undef STATUS_MSG_ITEM
};

static OPSTATUS succeed(void) { return STATUS_CONST(SUCCESS); }
static OPSTATUS fail(void) { return STATUS_CONST(INVALID_ARGUMENT); }

static int32_t double_val(int32_t x, void *ctx) {
  (void)ctx;
  return x * 2;
}

static OPSTATUS check_positive(int32_t x, RESULT_TYPE(int32) * out, void *ctx) {
  (void)ctx;
  if (x > 0) {
    return OBJECT_CALL(out, Ok, x * 10);
  }
  ERROR_TYPE(Error) e;
  TError_Create(&e, STATUS_CONST(INVALID_ARGUMENT), "Must be positive");
  return OBJECT_CALL(out, Err, &e);
}

typedef struct {
  bool triggered;
  PANIC_TYPE(Kind) lastKind;
  char lastMessage[256];
  char lastExpression[256];
} TestPanicState;

static void test_panic_handler(const PANIC_TYPE(Info) * info, void *context) {
  TestPanicState *state = (TestPanicState *)context;
  state->triggered = true;
  state->lastKind = info->kind;
  if (info->message) {
    strncpy(state->lastMessage, info->message, sizeof(state->lastMessage) - 1);
    state->lastMessage[sizeof(state->lastMessage) - 1] = '\0';
  } else {
    state->lastMessage[0] = '\0';
  }
  if (info->expression) {
    strncpy(state->lastExpression, info->expression,
            sizeof(state->lastExpression) - 1);
    state->lastExpression[sizeof(state->lastExpression) - 1] = '\0';
  } else {
    state->lastExpression[0] = '\0';
  }
}

static void test_cleanup_handler(void *context) {
  bool *cleaned = (bool *)context;
  *cleaned = true;
}

int main(void) {
  /* 1. Base status checks */
  if (succeed() != STATUS_CONST(SUCCESS))
    return 1;
  if (fail() != STATUS_CONST(INVALID_ARGUMENT))
    return 2;
  if (Error_Test_StatusCount != 18)
    return 3;
  if (strcmp(status_messages[STATUS_CONST(SUCCESS)], "Success") != 0)
    return 4;
  if (strcmp(status_messages[STATUS_CONST(INVALID_ARGUMENT)],
             "Invalid argument") != 0)
    return 5;
  if (strcmp(status_messages[STATUS_CONST(ARITHMETIC_OVERFLOW)],
             "Arithmetic overflow") != 0)
    return 6;

  /* 2. Predicates: IsSuccess / IsFailure / IsSpecial */
  assert(Status_IsSuccess(STATUS_CONST(SUCCESS)) == true);
  assert(Status_IsSuccess(STATUS_CONST(GENERIC_ERROR)) == false);
  assert(Status_IsFailure(STATUS_CONST(SUCCESS)) == false);
  assert(Status_IsFailure(STATUS_CONST(INVALID_ARGUMENT)) == true);

  assert(Status_IsSpecial(STATUS_CONST(WOULD_BLOCK)) == true);
  assert(Status_IsSpecial(STATUS_CONST(INCOMPLETE_SEQUENCE)) == true);
  assert(Status_IsSpecial(STATUS_CONST(SUCCESS)) == false);
  assert(Status_IsSpecial(STATUS_CONST(OUT_OF_MEMORY)) == false);

  /* 3. Categories and CategoryOf */
  assert(Status_CategoryOf(STATUS_CONST(SUCCESS)) ==
         STATUS_CONST(CATEGORY_SUCCESS));
  assert(Status_CategoryOf(STATUS_CONST(INVALID_ARGUMENT)) ==
         STATUS_CONST(CATEGORY_GENERIC));
  assert(Status_CategoryOf(STATUS_CONST(OUT_OF_MEMORY)) ==
         STATUS_CONST(CATEGORY_MEMORY));
  assert(Status_CategoryOf(STATUS_CONST(INSUFFICIENT_SPACE)) ==
         STATUS_CONST(CATEGORY_MEMORY));
  assert(Status_CategoryOf(STATUS_CONST(TIMEOUT)) ==
         STATUS_CONST(CATEGORY_CONCURRENCY));
  assert(Status_CategoryOf(STATUS_CONST(WOULD_BLOCK)) ==
         STATUS_CONST(CATEGORY_CONCURRENCY));
  assert(Status_CategoryOf(STATUS_CONST(ARITHMETIC_OVERFLOW)) ==
         STATUS_CONST(CATEGORY_ARITHMETIC));
  assert(Status_CategoryOf(STATUS_CONST(DIVISION_BY_ZERO)) ==
         STATUS_CONST(CATEGORY_ARITHMETIC));
  assert(Status_CategoryOf(STATUS_CONST(INVALID_SEQUENCE)) ==
         STATUS_CONST(CATEGORY_SEQUENCE));
  assert(Status_CategoryOf(STATUS_CONST(INCOMPLETE_SEQUENCE)) ==
         STATUS_CONST(CATEGORY_SEQUENCE));

  /* 4. IsCategory predicate */
  assert(Status_IsCategory(STATUS_CONST(OUT_OF_MEMORY),
                           STATUS_CONST(CATEGORY_MEMORY)) == true);
  assert(Status_IsCategory(STATUS_CONST(OUT_OF_MEMORY),
                           STATUS_CONST(CATEGORY_GENERIC)) == false);

  /* 5. Names and Messages */
  assert(strcmp(Status_Name(STATUS_CONST(SUCCESS)), "SUCCESS") == 0);
  assert(strcmp(Status_Name(STATUS_CONST(OUT_OF_RANGE)), "OUT_OF_RANGE") == 0);
  assert(strcmp(Status_Message(STATUS_CONST(SUCCESS)), "Success") == 0);
  assert(strcmp(Status_Message(STATUS_CONST(NOT_FOUND)), "Not found") == 0);
  assert(strcmp(Status_CategoryName(STATUS_CONST(CATEGORY_CONCURRENCY)),
                "Concurrency") == 0);

  /* 6. PACKAGE_CALL verification */
  assert(PACKAGE_CALL(Status, IsSuccess, STATUS_CONST(SUCCESS)) == true);
  assert(strcmp(PACKAGE_CALL(Status, Message, STATUS_CONST(TIMEOUT)),
                "Timeout") == 0);

  /* 7. ERROR_TYPE(Error) object operations and queries */
  ERROR_TYPE(Error) err;
  assert(TError_Init(&err) == STATUS_CONST(SUCCESS));
  assert(err.api != NULL);

  /* Create with custom message */
  assert(TError_Create(&err, STATUS_CONST(INVALID_ARGUMENT), "Custom invalid arg") ==
         STATUS_CONST(SUCCESS));
  assert(TError_Code(&err) == STATUS_CONST(INVALID_ARGUMENT));
  assert(strcmp(TError_Message(&err), "Custom invalid arg") == 0);
  assert(TError_Is(&err, STATUS_CONST(INVALID_ARGUMENT)) == true);
  assert(TError_Is(&err, STATUS_CONST(OUT_OF_RANGE)) == false);
  assert(TError_IsSuccess(&err) == false);
  assert(TError_IsFailure(&err) == true);
  assert(TError_Category(&err) == STATUS_CONST(CATEGORY_GENERIC));
  assert(TError_IsCategory(&err, STATUS_CONST(CATEGORY_GENERIC)) == true);
  assert(TError_IsCategory(&err, STATUS_CONST(CATEGORY_MEMORY)) == false);

  /* Calling via OBJECT_CALL */
  assert(OBJECT_CALL(&err, Is, STATUS_CONST(INVALID_ARGUMENT)) == true);
  assert(OBJECT_CALL_0(&err, IsFailure) == true);
  assert(strcmp(OBJECT_CALL_0(&err, Message), "Custom invalid arg") == 0);

  /* Calling via FunctionTable api pointer */
  assert(err.api->Is(&err, STATUS_CONST(INVALID_ARGUMENT)) == true);
  assert(strcmp(err.api->Message(&err), "Custom invalid arg") == 0);

  /* FromStatus: message defaults to Status_Message */
  assert(TError_FromStatus(&err, STATUS_CONST(OUT_OF_MEMORY)) ==
         STATUS_CONST(SUCCESS));
  assert(TError_Code(&err) == STATUS_CONST(OUT_OF_MEMORY));
  assert(strcmp(TError_Message(&err), "Out of memory") == 0);
  assert(TError_Category(&err) == STATUS_CONST(CATEGORY_MEMORY));
  assert(TError_IsCategory(&err, STATUS_CONST(CATEGORY_MEMORY)) == true);

  /* Clear resets to SUCCESS */
  TError_Clear(&err);
  assert(TError_Code(&err) == STATUS_CONST(SUCCESS));
  assert(TError_IsSuccess(&err) == true);
  assert(TError_IsFailure(&err) == false);
  assert(strcmp(TError_Message(&err), "Success") == 0);

  /* NULL receiver safety */
  assert(TError_Code(NULL) == STATUS_CONST(INVALID_ARGUMENT));
  assert(TError_IsSuccess(NULL) == false);
  assert(TError_IsFailure(NULL) == true);
  assert(TError_Is(NULL, STATUS_CONST(SUCCESS)) == false);
  assert(strstr(TError_Message(NULL), "Unknown") != NULL);

  /* 8. DIAGNOSTIC_TYPE(Context) operations and properties */
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

  /* SetContext / SetOrigin on TError */
  assert(TError_HasContext(&err) == false);
  assert(TError_SetContext(&err, &ctx) == STATUS_CONST(SUCCESS));
  assert(TError_HasContext(&err) == true);
  assert(TError_Context(&err)->line == 42);
  assert(strcmp(TDiagnosticContext_Function(TError_Context(&err)), "test_func") == 0);

  /* SetOrigin directly */
  assert(TError_SetOrigin(&err, "main.c", 99, "main", "Direct context", NULL) ==
         STATUS_CONST(SUCCESS));
  assert(TError_HasContext(&err) == true);
  assert(TError_Context(&err)->line == 99);
  assert(strcmp(TError_Context(&err)->description, "Direct context") == 0);

  /* TError_Clear also clears context */
  TError_Clear(&err);
  assert(TError_HasContext(&err) == false);

  /* 9. Error Chain: cause chaining, depth, root cause, and cycle detection */
  ERROR_TYPE(Error) err1, err2, err3;
  assert(TError_Create(&err1, STATUS_CONST(NOT_FOUND), "Resource missing") ==
         STATUS_CONST(SUCCESS));
  assert(TError_Create(&err2, STATUS_CONST(GENERIC_ERROR), "Database lookup failed") ==
         STATUS_CONST(SUCCESS));
  assert(TError_Create(&err3, STATUS_CONST(TIMEOUT), "Request timed out") ==
         STATUS_CONST(SUCCESS));

  /* Initial depth of independent errors is 1 */
  assert(TError_Depth(&err1) == 1);
  assert(TError_HasCause(&err1) == false);
  assert(TError_Cause(&err1) == NULL);
  assert(TError_RootCause(&err1) == &err1);

  /* Chain: err3 -> err2 -> err1 */
  assert(TError_SetCause(&err2, &err1) == STATUS_CONST(SUCCESS));
  assert(TError_HasCause(&err2) == true);
  assert(TError_Cause(&err2) == &err1);
  assert(TError_Depth(&err2) == 2);
  assert(TError_RootCause(&err2) == &err1);

  assert(TError_SetCause(&err3, &err2) == STATUS_CONST(SUCCESS));
  assert(TError_HasCause(&err3) == true);
  assert(TError_Cause(&err3) == &err2);
  assert(TError_Depth(&err3) == 3);
  assert(TError_RootCause(&err3) == &err1);

  /* Polymorphic invocation of chain methods via OBJECT_CALL */
  assert(OBJECT_CALL_0(&err3, HasCause) == true);
  assert(OBJECT_CALL_0(&err3, Depth) == 3);
  assert(OBJECT_CALL_0(&err3, RootCause) == &err1);

  /* Direct self-cycle rejection */
  assert(TError_SetCause(&err1, &err1) == STATUS_CONST(INVALID_ARGUMENT));
  assert(TError_HasCause(&err1) == false);

  /* Indirect cycle rejection: err1 -> err3 would create cycle err3 -> err2 ->
   * err1 -> err3 */
  assert(TError_SetCause(&err1, &err3) == STATUS_CONST(INVALID_ARGUMENT));
  assert(TError_HasCause(&err1) == false);

  /* Clearing cause by passing NULL */
  assert(TError_SetCause(&err3, NULL) == STATUS_CONST(SUCCESS));
  assert(TError_HasCause(&err3) == false);
  assert(TError_Depth(&err3) == 1);

  /* Max depth overflow test (MAX_CHAIN_DEPTH = 16) */
  ERROR_TYPE(Error) chainErrors[18];
  for (size_t i = 0; i < 17; ++i) {
    assert(TError_FromStatus(&chainErrors[i], STATUS_CONST(GENERIC_ERROR)) ==
           STATUS_CONST(SUCCESS));
    if (i > 0) {
      assert(TError_SetCause(&chainErrors[i], &chainErrors[i - 1]) ==
             STATUS_CONST(SUCCESS));
    }
  }
  assert(TError_Depth(&chainErrors[16]) == 17);

  /* Now, trying to set chainErrors[16] as the cause of chainErrors[17] exceeds MAX_CHAIN_DEPTH */
  assert(TError_FromStatus(&chainErrors[17], STATUS_CONST(GENERIC_ERROR)) ==
         STATUS_CONST(SUCCESS));
  assert(TError_SetCause(&chainErrors[17], &chainErrors[16]) ==
         STATUS_CONST(OUT_OF_RANGE));
  assert(TError_HasCause(&chainErrors[17]) == false);

  /* 10. RESULT_TYPE(SUFFIX) operations and queries */
  RESULT_TYPE(int32) res;
  assert(TResult_int32_Init(&res) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&res, Ok, 100) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&res, IsOk) == true);
  assert(OBJECT_CALL_0(&res, IsError) == false);
  assert(OBJECT_CALL(&res, ValueOr, 999) == 100);

  int32_t val = 0;
  assert(OBJECT_CALL(&res, Value, &val) == STATUS_CONST(SUCCESS));
  assert(val == 100);

  ERROR_TYPE(Error) extractedErr;
  assert(OBJECT_CALL(&res, Error, &extractedErr) ==
         STATUS_CONST(INVALID_ARGUMENT));

  /* Err result */
  ERROR_TYPE(Error) customErr;
  assert(TError_Create(&customErr, STATUS_CONST(OUT_OF_MEMORY), "Allocation failed") ==
         STATUS_CONST(SUCCESS));
  RESULT_TYPE(int32) errRes;
  assert(TResult_int32_Init(&errRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&errRes, Err, &customErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&errRes, IsOk) == false);
  assert(OBJECT_CALL_0(&errRes, IsError) == true);
  assert(OBJECT_CALL(&errRes, ValueOr, 999) == 999);

  val = 42;
  assert(OBJECT_CALL(&errRes, Value, &val) == STATUS_CONST(OUT_OF_MEMORY));
  assert(val == 42); /* out untouched */

  assert(OBJECT_CALL(&errRes, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(TError_Code(&extractedErr) == STATUS_CONST(OUT_OF_MEMORY));
  assert(strcmp(TError_Message(&extractedErr), "Allocation failed") == 0);

  /* FromStatus */
  RESULT_TYPE(int32) statusRes;
  assert(TResult_int32_Init(&statusRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&statusRes, FromStatus, STATUS_CONST(TIMEOUT)) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&statusRes, IsOk) == false);
  assert(OBJECT_CALL(&statusRes, Error, &extractedErr) ==
         STATUS_CONST(SUCCESS));
  assert(TError_Code(&extractedErr) == STATUS_CONST(TIMEOUT));
  assert(strcmp(TError_Message(&extractedErr), "Timeout") == 0);

  assert(OBJECT_CALL(&statusRes, FromStatus, STATUS_CONST(SUCCESS)) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&statusRes, IsOk) == true);

  /* Map: on Ok */
  RESULT_TYPE(int32) mappedRes;
  assert(TResult_int32_Init(&mappedRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&res, Map, RESULT_MAP_BIND(int32, double_val, NULL),
                     &mappedRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&mappedRes, IsOk) == true);
  assert(OBJECT_CALL(&mappedRes, ValueOr, 0) == 200);

  /* Map: on Err */
  RESULT_TYPE(int32) mappedErr;
  assert(TResult_int32_Init(&mappedErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&errRes, Map, RESULT_MAP_BIND(int32, double_val, NULL),
                     &mappedErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&mappedErr, IsError) == true);
  assert(OBJECT_CALL(&mappedErr, Error, &extractedErr) ==
         STATUS_CONST(SUCCESS));
  assert(TError_Code(&extractedErr) == STATUS_CONST(OUT_OF_MEMORY));

  /* AndThen: on Ok -> Ok */
  RESULT_TYPE(int32) boundRes;
  assert(TResult_int32_Init(&boundRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&res, AndThen,
                     RESULT_BIND_BIND(int32, check_positive, NULL),
                     &boundRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundRes, IsOk) == true);
  assert(OBJECT_CALL(&boundRes, ValueOr, 0) == 1000);

  /* AndThen: on Ok -> Err */
  RESULT_TYPE(int32) negRes;
  assert(TResult_int32_Init(&negRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&negRes, Ok, -5) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&negRes, AndThen,
                     RESULT_BIND_BIND(int32, check_positive, NULL),
                     &boundRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundRes, IsError) == true);
  assert(OBJECT_CALL(&boundRes, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(TError_Code(&extractedErr) == STATUS_CONST(INVALID_ARGUMENT));
  assert(strcmp(TError_Message(&extractedErr), "Must be positive") == 0);

  /* AndThen: on Err -> preserves original Err */
  assert(OBJECT_CALL(&errRes, AndThen,
                     RESULT_BIND_BIND(int32, check_positive, NULL),
                     &boundRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundRes, IsError) == true);
  assert(OBJECT_CALL(&boundRes, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(TError_Code(&extractedErr) == STATUS_CONST(OUT_OF_MEMORY));

  /* 11. Diagnostic_Format package functions */
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
  assert(strcmp(fmtBuf,
                "at test_file.c:42 in test_func() - Sample description") == 0);

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
                "[INVALID_ARGUMENT] Custom invalid arg (at test_file.c:42 "
                "in test_func() - Sample description)") == 0);

  /* Format_Chain */
  assert(TError_SetCause(&err3, &err2) == STATUS_CONST(SUCCESS));
  assert(Diagnostic_Format_Chain(&err3, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(SUCCESS));
  assert(strstr(fmtBuf, "[#0] [TIMEOUT] Request timed out") != NULL);
  assert(strstr(fmtBuf,
                "caused by: [#1] [GENERIC_ERROR] Database lookup failed") !=
         NULL);
  assert(strstr(fmtBuf, "caused by: [#2] [NOT_FOUND] Resource missing") !=
         NULL);

  /* Invalid arguments to Format */
  assert(Diagnostic_Format_Simple(NULL, fmtBuf, sizeof(fmtBuf), &written) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(Diagnostic_Format_Simple(&err1, NULL, sizeof(fmtBuf), &written) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(Diagnostic_Format_Simple(&err1, fmtBuf, 0, &written) ==
         STATUS_CONST(INVALID_ARGUMENT));

  /* 12. DIAGNOSTIC_TYPE(Stacktrace) object operations */
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
  assert(OBJECT_CALL(&trace, FrameAt, 999, &frameAddr) ==
         STATUS_CONST(OUT_OF_RANGE));

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

  /* 13. Panic and Contract Macros (ASSERT, VERIFY, ENSURE, UNREACHABLE, PANIC)
   */
  assert(strcmp(PANIC_FUNC(KindName)(PANIC_CONST(KIND_PANIC)), "PANIC") == 0);
  assert(strcmp(PANIC_FUNC(KindName)(PANIC_CONST(KIND_ASSERT)), "ASSERT") == 0);
  assert(strcmp(PANIC_FUNC(KindName)(PANIC_CONST(KIND_VERIFY)), "VERIFY") == 0);
  assert(strcmp(PANIC_FUNC(KindName)(PANIC_CONST(KIND_ENSURE)), "ENSURE") == 0);
  assert(strcmp(PANIC_FUNC(KindName)(PANIC_CONST(KIND_UNREACHABLE)),
                "UNREACHABLE") == 0);

  TestPanicState panicState = {false, PANIC_CONST(KIND_PANIC), {0}, {0}};
  PANIC_FUNC(SetHandler)(PANIC_HANDLER_BIND(test_panic_handler, &panicState));

  /* Test PANIC macro */
  PANIC("Explicit fatal event");
  assert(panicState.triggered == true);
  assert(panicState.lastKind == PANIC_CONST(KIND_PANIC));
  assert(strcmp(panicState.lastMessage, "Explicit fatal event") == 0);

  /* Test ASSERT macro on failing condition */
  panicState.triggered = false;
  ASSERT(10 == 20);
  assert(panicState.triggered == true);
  assert(panicState.lastKind == PANIC_CONST(KIND_ASSERT));
  assert(strcmp(panicState.lastExpression, "10 == 20") == 0);

  /* Test ASSERT macro on true condition (must not trigger) */
  panicState.triggered = false;
  ASSERT(10 == 10);
  assert(panicState.triggered == false);

  /* Test VERIFY macro */
  panicState.triggered = false;
  VERIFY(5 > 100);
  assert(panicState.triggered == true);
  assert(panicState.lastKind == PANIC_CONST(KIND_VERIFY));
  assert(strcmp(panicState.lastExpression, "5 > 100") == 0);

  /* Test ENSURE macro */
  panicState.triggered = false;
  ENSURE(2 + 2 == 5);
  assert(panicState.triggered == true);
  assert(panicState.lastKind == PANIC_CONST(KIND_ENSURE));

  /* Test UNREACHABLE macro */
  panicState.triggered = false;
  UNREACHABLE();
  assert(panicState.triggered == true);
  assert(panicState.lastKind == PANIC_CONST(KIND_UNREACHABLE));

  /* Reset Panic handler */
  PANIC_FUNC(ResetHandler)();

  /* 14. Exception Control Flow (TRY, CATCH, FINALLY, THROW, THROW_STATUS,
   * RETHROW) */
  bool tryRan = false;
  bool catchRan = false;
  bool finallyRan = false;

  /* 14.1 Normal execution without throwing */
  TRY { tryRan = true; }
  CATCH { catchRan = true; }
  FINALLY { finallyRan = true; }
  assert(tryRan == true);
  assert(catchRan == false);
  assert(finallyRan == true);
  assert(EXCEPTION_FUNC(HasActiveFrame)() == false);

  /* 14.2 Throw and Catch with THROW_STATUS */
  tryRan = false;
  catchRan = false;
  finallyRan = false;

  TRY {
    tryRan = true;
    THROW_STATUS(STATUS_CONST(INVALID_ARGUMENT), "Test exception message");
    tryRan = false; /* should not reach */
  }
  CATCH {
    catchRan = true;
    assert(CURRENT_ERROR()->code == STATUS_CONST(INVALID_ARGUMENT));
    assert(strcmp(CURRENT_ERROR()->message, "Test exception message") == 0);
    assert(TDiagnosticContext_HasOrigin(&CURRENT_ERROR()->context) == true);
  } FINALLY {
    finallyRan = true;
  }
  FINALLY { finallyRan = true; }
  assert(tryRan == true);
  assert(catchRan == true);
  assert(finallyRan == true);
  assert(EXCEPTION_FUNC(HasActiveFrame)() == false);

  /* 14.3 Cleanup callback execution via RegisterCleanup */
  bool cleanupRan = false;
  tryRan = false;
  catchRan = false;

  TRY {
    tryRan = true;
    EXCEPTION_FUNC(RegisterCleanup)(
        EXCEPTION_CLEANUP_BIND(test_cleanup_handler, &cleanupRan));
    THROW_STATUS(STATUS_CONST(OUT_OF_MEMORY), "Allocation exhausted");
  }
  CATCH {
    catchRan = true;
    assert(cleanupRan == true);
  }
  assert(tryRan == true);
  assert(catchRan == true);
  assert(cleanupRan == true);

  /* 14.4 Nested TRY blocks and RETHROW */
  bool outerTryRan = false;
  bool innerTryRan = false;
  bool innerCatchRan = false;
  bool innerFinallyRan = false;
  bool outerCatchRan = false;
  bool outerFinallyRan = false;

  TRY {
    outerTryRan = true;
    TRY {
      innerTryRan = true;
      THROW_STATUS(STATUS_CONST(TIMEOUT), "Nested timeout");
    }
    CATCH {
      innerCatchRan = true;
      assert(CURRENT_ERROR()->code == STATUS_CONST(TIMEOUT));
      RETHROW();
    }
    FINALLY { innerFinallyRan = true; }
  }
  CATCH {
    outerCatchRan = true;
    assert(CURRENT_ERROR()->code == STATUS_CONST(TIMEOUT));
    assert(strcmp(CURRENT_ERROR()->message, "Nested timeout") == 0);
  }
  FINALLY { outerFinallyRan = true; }

  assert(outerTryRan == true);
  assert(innerTryRan == true);
  assert(innerCatchRan == true);
  assert(innerFinallyRan == true);
  assert(outerCatchRan == true);
  assert(outerFinallyRan == true);
  assert(EXCEPTION_FUNC(HasActiveFrame)() == false);

  return 0;
}
