#include "../../../Cosmeron/Core/Condition/Error.h"
#include "../../../Cosmeron/Core/Condition/Panic.h"
#include "../../../Cosmeron/Core/Condition/Exception.h"
#include "../../../Cosmeron/Core/Diagnostic/Context.h"
#include "../../../Cosmeron/Core/Outcome/Status.h"

#include <assert.h>
#include <string.h>

typedef struct {
  bool triggered;
  MODULE_ENUM(Condition, PanicKind) lastKind;
  char lastMessage[256];
  char lastExpression[256];
} TestPanicState;

static void test_panic_handler(const MODULE_TYPE(Condition, PanicInfo) *info, void *context) {
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
  MODULE_TYPE(Condition, Error) err;
  assert(MODULE_FUNC(Condition, Error, Init)(&err) == STATUS_CONST(SUCCESS));
  assert(err.api != NULL);

  /* Create with custom message */
  assert(MODULE_FUNC(Condition, Error, Create)(&err, STATUS_CONST(INVALID_ARGUMENT), "Custom invalid arg") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, Code)(&err) == STATUS_CONST(INVALID_ARGUMENT));
  assert(strcmp(MODULE_FUNC(Condition, Error, Message)(&err), "Custom invalid arg") == 0);
  assert(MODULE_FUNC(Condition, Error, Is)(&err, STATUS_CONST(INVALID_ARGUMENT)) == true);
  assert(MODULE_FUNC(Condition, Error, Is)(&err, STATUS_CONST(OUT_OF_RANGE)) == false);
  assert(MODULE_FUNC(Condition, Error, IsSuccess)(&err) == false);
  assert(MODULE_FUNC(Condition, Error, IsFailure)(&err) == true);
  assert(MODULE_FUNC(Condition, Error, Category)(&err) == STATUS_CONST(CATEGORY_GENERIC));
  assert(MODULE_FUNC(Condition, Error, IsCategory)(&err, STATUS_CONST(CATEGORY_GENERIC)) == true);
  assert(MODULE_FUNC(Condition, Error, IsCategory)(&err, STATUS_CONST(CATEGORY_MEMORY)) == false);

  /* Calling via OBJECT_CALL */
  assert(OBJECT_CALL(&err, Is, STATUS_CONST(INVALID_ARGUMENT)) == true);
  assert(OBJECT_CALL_0(&err, IsFailure) == true);
  assert(strcmp(OBJECT_CALL_0(&err, Message), "Custom invalid arg") == 0);

  /* Calling via FunctionTable api pointer */
  assert(err.api->Is(&err, STATUS_CONST(INVALID_ARGUMENT)) == true);
  assert(strcmp(err.api->Message(&err), "Custom invalid arg") == 0);

  /* FromStatus: message defaults to Status_Message */
  assert(MODULE_FUNC(Condition, Error, FromStatus)(&err, STATUS_CONST(OUT_OF_MEMORY)) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, Code)(&err) == STATUS_CONST(OUT_OF_MEMORY));
  assert(strcmp(MODULE_FUNC(Condition, Error, Message)(&err), "Out of memory") == 0);
  assert(MODULE_FUNC(Condition, Error, Category)(&err) == STATUS_CONST(CATEGORY_MEMORY));
  assert(MODULE_FUNC(Condition, Error, IsCategory)(&err, STATUS_CONST(CATEGORY_MEMORY)) == true);

  /* Clear resets to SUCCESS */
  MODULE_FUNC(Condition, Error, Clear)(&err);
  assert(MODULE_FUNC(Condition, Error, Code)(&err) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, IsSuccess)(&err) == true);
  assert(MODULE_FUNC(Condition, Error, IsFailure)(&err) == false);
  assert(strcmp(MODULE_FUNC(Condition, Error, Message)(&err), "Success") == 0);

  /* NULL receiver safety */
  assert(MODULE_FUNC(Condition, Error, Code)(NULL) == STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Condition, Error, IsSuccess)(NULL) == false);
  assert(MODULE_FUNC(Condition, Error, IsFailure)(NULL) == true);
  assert(MODULE_FUNC(Condition, Error, Is)(NULL, STATUS_CONST(SUCCESS)) == false);
  assert(strstr(MODULE_FUNC(Condition, Error, Message)(NULL), "Unknown") != NULL);

  /* 2. Context binding on TError */
  MODULE_TYPE(Diagnostic, Context) ctx = {0};
  assert(MODULE_FUNC(Diagnostic, Context, Init)(&ctx) == STATUS_CONST(SUCCESS));
  int dummyData = 1234;
  assert(MODULE_FUNC(Diagnostic, Context, Create)(&ctx, "test_file.c", 42, "test_func",
                                                  "Sample description", &dummyData) ==
         STATUS_CONST(SUCCESS));

  assert(MODULE_FUNC(Condition, Error, HasContext)(&err) == false);
  assert(MODULE_FUNC(Condition, Error, SetContext)(&err, &ctx) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, HasContext)(&err) == true);
  assert(MODULE_FUNC(Condition, Error, Context)(&err)->line == 42);
  assert(strcmp(MODULE_FUNC(Diagnostic, Context, Function)(MODULE_FUNC(Condition, Error, Context)(&err)), "test_func") == 0);

  /* SetOrigin directly */
  assert(MODULE_FUNC(Condition, Error, SetOrigin)(&err, "main.c", 99, "main", "Direct context", NULL) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, HasContext)(&err) == true);
  assert(MODULE_FUNC(Condition, Error, Context)(&err)->line == 99);
  assert(strcmp(MODULE_FUNC(Condition, Error, Context)(&err)->description, "Direct context") == 0);

  /* Clear also clears context */
  MODULE_FUNC(Condition, Error, Clear)(&err);
  assert(MODULE_FUNC(Condition, Error, HasContext)(&err) == false);

  /* 3. Error Chain: cause chaining, depth, root cause, and cycle detection */
  MODULE_TYPE(Condition, Error) err1, err2, err3;
  assert(MODULE_FUNC(Condition, Error, Create)(&err1, STATUS_CONST(NOT_FOUND), "Resource missing") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, Create)(&err2, STATUS_CONST(GENERIC_ERROR), "Database lookup failed") ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, Create)(&err3, STATUS_CONST(TIMEOUT), "Request timed out") ==
         STATUS_CONST(SUCCESS));

  /* Initial depth of independent errors is 1 */
  assert(MODULE_FUNC(Condition, Error, Depth)(&err1) == 1);
  assert(MODULE_FUNC(Condition, Error, HasCause)(&err1) == false);
  assert(MODULE_FUNC(Condition, Error, Cause)(&err1) == NULL);
  assert(MODULE_FUNC(Condition, Error, RootCause)(&err1) == &err1);

  /* Chain: err3 -> err2 -> err1 */
  assert(MODULE_FUNC(Condition, Error, SetCause)(&err2, &err1) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, HasCause)(&err2) == true);
  assert(MODULE_FUNC(Condition, Error, Cause)(&err2) == &err1);
  assert(MODULE_FUNC(Condition, Error, Depth)(&err2) == 2);
  assert(MODULE_FUNC(Condition, Error, RootCause)(&err2) == &err1);

  assert(MODULE_FUNC(Condition, Error, SetCause)(&err3, &err2) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, HasCause)(&err3) == true);
  assert(MODULE_FUNC(Condition, Error, Cause)(&err3) == &err2);
  assert(MODULE_FUNC(Condition, Error, Depth)(&err3) == 3);
  assert(MODULE_FUNC(Condition, Error, RootCause)(&err3) == &err1);

  /* Polymorphic invocation of chain methods via OBJECT_CALL */
  assert(OBJECT_CALL_0(&err3, HasCause) == true);
  assert(OBJECT_CALL_0(&err3, Depth) == 3);
  assert(OBJECT_CALL_0(&err3, RootCause) == &err1);

  /* Direct self-cycle rejection */
  assert(MODULE_FUNC(Condition, Error, SetCause)(&err1, &err1) == STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Condition, Error, HasCause)(&err1) == false);

  /* Indirect cycle rejection: err1 -> err3 would create cycle err3 -> err2 -> err1 -> err3 */
  assert(MODULE_FUNC(Condition, Error, SetCause)(&err1, &err3) == STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Condition, Error, HasCause)(&err1) == false);

  /* Clearing cause by passing NULL */
  assert(MODULE_FUNC(Condition, Error, SetCause)(&err3, NULL) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, HasCause)(&err3) == false);
  assert(MODULE_FUNC(Condition, Error, Depth)(&err3) == 1);

  /* Max depth overflow test (MAX_CHAIN_DEPTH = 16) */
  MODULE_TYPE(Condition, Error) chainErrors[18];
  for (size_t i = 0; i < 17; ++i) {
    assert(MODULE_FUNC(Condition, Error, FromStatus)(&chainErrors[i], STATUS_CONST(GENERIC_ERROR)) ==
           STATUS_CONST(SUCCESS));
    if (i > 0) {
      assert(MODULE_FUNC(Condition, Error, SetCause)(&chainErrors[i], &chainErrors[i - 1]) ==
             STATUS_CONST(SUCCESS));
    }
  }
  assert(MODULE_FUNC(Condition, Error, Depth)(&chainErrors[16]) == 17);

  /* Exceeding MAX_CHAIN_DEPTH */
  assert(MODULE_FUNC(Condition, Error, FromStatus)(&chainErrors[17], STATUS_CONST(GENERIC_ERROR)) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Condition, Error, SetCause)(&chainErrors[17], &chainErrors[16]) ==
         STATUS_CONST(OUT_OF_RANGE));
  assert(MODULE_FUNC(Condition, Error, HasCause)(&chainErrors[17]) == false);

  /* 4. Panic and Contract Macros (ASSERT, VERIFY, ENSURE, UNREACHABLE, PANIC) */
  assert(strcmp(MODULE_FUNC(Condition, Panic, KindName)(MODULE_CONST(Condition, PANIC)), "PANIC") == 0);
  assert(strcmp(MODULE_FUNC(Condition, Panic, KindName)(MODULE_CONST(Condition, ASSERT)), "ASSERT") == 0);
  assert(strcmp(MODULE_FUNC(Condition, Panic, KindName)(MODULE_CONST(Condition, VERIFY)), "VERIFY") == 0);
  assert(strcmp(MODULE_FUNC(Condition, Panic, KindName)(MODULE_CONST(Condition, ENSURE)), "ENSURE") == 0);
  assert(strcmp(MODULE_FUNC(Condition, Panic, KindName)(MODULE_CONST(Condition, UNREACHABLE)), "UNREACHABLE") == 0);

  TestPanicState panicState = {false, MODULE_CONST(Condition, PANIC), {0}, {0}};
  TCALLBACK(MODULE_NS(Condition, Panic, Handler)) panicCb = {
      .function = test_panic_handler,
      .context = &panicState
  };
  MODULE_FUNC(Condition, Panic, SetHandler)(panicCb);

  /* Test PANIC macro */
  PANIC("Explicit fatal event");
  assert(panicState.triggered == true);
  assert(panicState.lastKind == MODULE_CONST(Condition, PANIC));
  assert(strcmp(panicState.lastMessage, "Explicit fatal event") == 0);

  /* Test ASSERT macro on failing condition */
  panicState.triggered = false;
  ASSERT(10 == 20);
  assert(panicState.triggered == true);
  assert(panicState.lastKind == MODULE_CONST(Condition, ASSERT));
  assert(strcmp(panicState.lastExpression, "10 == 20") == 0);

  /* Test ASSERT macro on true condition (must not trigger) */
  panicState.triggered = false;
  ASSERT(10 == 10);
  assert(panicState.triggered == false);

  /* Test VERIFY macro */
  panicState.triggered = false;
  VERIFY(5 > 100);
  assert(panicState.triggered == true);
  assert(panicState.lastKind == MODULE_CONST(Condition, VERIFY));
  assert(strcmp(panicState.lastExpression, "5 > 100") == 0);

  /* Test ENSURE macro */
  panicState.triggered = false;
  ENSURE(2 + 2 == 5);
  assert(panicState.triggered == true);
  assert(panicState.lastKind == MODULE_CONST(Condition, ENSURE));

  /* Test UNREACHABLE macro */
  panicState.triggered = false;
  UNREACHABLE();
  assert(panicState.triggered == true);
  assert(panicState.lastKind == MODULE_CONST(Condition, UNREACHABLE));

  /* Reset Panic handler */
  MODULE_FUNC(Condition, Panic, ResetHandler)();

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
  assert(MODULE_FUNC(Condition, Exception, HasActiveFrame)() == false);

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
    assert(MODULE_FUNC(Diagnostic, Context, HasOrigin)(&CURRENT_ERROR()->context) == true);
  } FINALLY {
    finallyRan = true;
  }
  assert(tryRan == true);
  assert(catchRan == true);
  assert(finallyRan == true);
  assert(MODULE_FUNC(Condition, Exception, HasActiveFrame)() == false);

  /* 5.3 Cleanup callback execution via RegisterCleanup */
  bool cleanupRan = false;
  tryRan = false;
  catchRan = false;

  TRY {
    tryRan = true;
    TCALLBACK(MODULE_NS(Condition, ExceptionFrame, Cleanup)) cleanupCb = {
        .function = test_cleanup_handler,
        .context = &cleanupRan
    };
    MODULE_FUNC(Condition, Exception, RegisterCleanup)(cleanupCb);
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
  assert(MODULE_FUNC(Condition, Exception, HasActiveFrame)() == false);

  return 0;
}
