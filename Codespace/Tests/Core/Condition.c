#include "../../Cosmeron/Core/Condition/Error.h"
#include "../../Cosmeron/Core/Condition/Panic.h"
#include "../../Cosmeron/Core/Condition/Exception.h"
#include "../../Cosmeron/Core/Diagnostic/Context.h"
#include "../../Cosmeron/Core/Outcome/Status.h"

#include <assert.h>
#include <string.h>

typedef struct {
  bool triggered;
  PANIC_TYPE(Kind) lastKind;
  char lastMessage[256];
  char lastExpression[256];
} TestPanicState;

static void test_panic_handler(const PANIC_TYPE(Info) *info, void *context) {
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
    strncpy(state->lastExpression, info->expression, sizeof(state->lastExpression) - 1);
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
  /* 1. TError object operations and queries */
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
  assert(TError_Category(&err) == STATUS_CATEGORY_CONST(GENERIC));
  assert(TError_IsCategory(&err, STATUS_CATEGORY_CONST(GENERIC)) == true);
  assert(TError_IsCategory(&err, STATUS_CATEGORY_CONST(MEMORY)) == false);

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
  assert(TError_Category(&err) == STATUS_CATEGORY_CONST(MEMORY));
  assert(TError_IsCategory(&err, STATUS_CATEGORY_CONST(MEMORY)) == true);

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

  /* 2. Context binding on TError */
  DIAGNOSTIC_TYPE(Context) ctx = {0};
  assert(TDiagnosticContext_Init(&ctx) == STATUS_CONST(SUCCESS));
  int dummyData = 1234;
  assert(TDiagnosticContext_Create(&ctx, "test_file.c", 42, "test_func",
                                   "Sample description", &dummyData) ==
         STATUS_CONST(SUCCESS));

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

  /* 3. Error Chain: cause chaining, depth, root cause, and cycle detection */
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

  /* Indirect cycle rejection: err1 -> err3 would create cycle err3 -> err2 -> err1 -> err3 */
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

  /* Exceeding MAX_CHAIN_DEPTH */
  assert(TError_FromStatus(&chainErrors[17], STATUS_CONST(GENERIC_ERROR)) ==
         STATUS_CONST(SUCCESS));
  assert(TError_SetCause(&chainErrors[17], &chainErrors[16]) ==
         STATUS_CONST(OUT_OF_RANGE));
  assert(TError_HasCause(&chainErrors[17]) == false);

  /* 4. Panic and Contract Macros (ASSERT, VERIFY, ENSURE, UNREACHABLE, PANIC) */
  assert(strcmp(PANIC_FUNC(KindName)(PANIC_CONST(KIND_PANIC)), "PANIC") == 0);
  assert(strcmp(PANIC_FUNC(KindName)(PANIC_CONST(KIND_ASSERT)), "ASSERT") == 0);
  assert(strcmp(PANIC_FUNC(KindName)(PANIC_CONST(KIND_VERIFY)), "VERIFY") == 0);
  assert(strcmp(PANIC_FUNC(KindName)(PANIC_CONST(KIND_ENSURE)), "ENSURE") == 0);
  assert(strcmp(PANIC_FUNC(KindName)(PANIC_CONST(KIND_UNREACHABLE)), "UNREACHABLE") == 0);

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

  /* 5. Exception Control Flow (TRY, CATCH, FINALLY, THROW, THROW_STATUS, RETHROW) */
  bool tryRan = false;
  bool catchRan = false;
  bool finallyRan = false;

  /* 5.1 Normal execution without throwing */
  TRY {
    tryRan = true;
  } CATCH {
    catchRan = true;
  } FINALLY {
    finallyRan = true;
  }
  assert(tryRan == true);
  assert(catchRan == false);
  assert(finallyRan == true);
  assert(EXCEPTION_FUNC(HasActiveFrame)() == false);

  /* 5.2 Throw and Catch with THROW_STATUS */
  tryRan = false;
  catchRan = false;
  finallyRan = false;

  TRY {
    tryRan = true;
    THROW_STATUS(STATUS_CONST(INVALID_ARGUMENT), "Test exception message");
    tryRan = false; /* should not reach */
  } CATCH {
    catchRan = true;
    assert(CURRENT_ERROR()->code == STATUS_CONST(INVALID_ARGUMENT));
    assert(strcmp(CURRENT_ERROR()->message, "Test exception message") == 0);
    assert(TDiagnosticContext_HasOrigin(&CURRENT_ERROR()->context) == true);
  } FINALLY {
    finallyRan = true;
  }
  assert(tryRan == true);
  assert(catchRan == true);
  assert(finallyRan == true);
  assert(EXCEPTION_FUNC(HasActiveFrame)() == false);

  /* 5.3 Cleanup callback execution via RegisterCleanup */
  bool cleanupRan = false;
  tryRan = false;
  catchRan = false;

  TRY {
    tryRan = true;
    EXCEPTION_FUNC(RegisterCleanup)(EXCEPTION_CLEANUP_BIND(test_cleanup_handler, &cleanupRan));
    THROW_STATUS(STATUS_CONST(OUT_OF_MEMORY), "Allocation exhausted");
  } CATCH {
    catchRan = true;
    assert(cleanupRan == true);
  }
  assert(tryRan == true);
  assert(catchRan == true);
  assert(cleanupRan == true);

  /* 5.4 Nested TRY blocks and RETHROW */
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
    } CATCH {
      innerCatchRan = true;
      assert(CURRENT_ERROR()->code == STATUS_CONST(TIMEOUT));
      RETHROW();
    } FINALLY {
      innerFinallyRan = true;
    }
  } CATCH {
    outerCatchRan = true;
    assert(CURRENT_ERROR()->code == STATUS_CONST(TIMEOUT));
    assert(strcmp(CURRENT_ERROR()->message, "Nested timeout") == 0);
  } FINALLY {
    outerFinallyRan = true;
  }

  assert(outerTryRan == true);
  assert(innerTryRan == true);
  assert(innerCatchRan == true);
  assert(innerFinallyRan == true);
  assert(outerCatchRan == true);
  assert(outerFinallyRan == true);
  assert(EXCEPTION_FUNC(HasActiveFrame)() == false);

  return 0;
}

