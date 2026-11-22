/* Compile with:
 * gcc -std=c11 -Wall -Wextra -Werror -pedantic -I Codespace
 * Codespace/Tests/Core/Callable.c -o /tmp/test_callable && /tmp/test_callable
 */
#include "Cosmeron/Core/Module/Callable/Callable.h"
#include "Cosmeron/Core/Algorithm/Comparison.h"
#include "Cosmeron/Core/Condition/Status.h"
#include "Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <stdbool.h>

/* ========================================================================== */
/* 1. Standalone Pure Callables                                               */
/* ========================================================================== */

CALLABLE_FUNCTION(int, Scale, (int value));
CALLABLE_FUNCTION(int, AddOffset, (int left, int right));
CALLABLE_PROCEDURE(Notify, (int code));
CALLABLE_FUNCTION(int, ReadValue, ());
CALLABLE_PROCEDURE(ResetValue, ());

CALLABLE_OPERATION(Commit, (int value));
CALLABLE_PREDICATE(IsPositive, (int value));
CALLABLE_OPERATION(Flush, ());
CALLABLE_PREDICATE(IsEnabled, ());
CALLABLE_COMPARISON(CompareInts, (int left, int right));

/* ========================================================================== */
/* 2. Declarative Module with Callables                                       */
/* ========================================================================== */

MODULE(DOMAIN(Core), TestCallableModule,
       CALLABLE(FUNCTION(int, DoubleVal, (int x)),
                PROCEDURE(LogCode, (int code))),
       PACKAGE(TestPkg, CALLABLE(OPERATION(OnEvent, (int code)))),
       OBJECT(TestObj, FIELDS(int id;),
              CALLABLE(PREDICATE(Filter, (int value)))));

/* ========================================================================== */
/* Stateless Target Functions                                                 */
/* ========================================================================== */

static int ScaleValue(int value) { return value * 3; }

static int AddOffsetValue(int left, int right) { return left + right + 5; }

static int g_notified = 0;
static void NotifyValue(int code) { g_notified = code; }

static int ReadDirect(void) { return 42; }

static void ResetDirect(void) { g_notified = 0; }

static OPSTATUS CommitValue(int value) {
  return value >= 0 ? STATUS_CONST(SUCCESS) : STATUS_CONST(INVALID_ARGUMENT);
}

static bool PositiveValue(int value) { return value > 0; }

static OPSTATUS FlushDirect(void) {
  g_notified = 0;
  return STATUS_CONST(SUCCESS);
}

static bool EnabledDirect(void) { return true; }

static CMPOUT CompareIntsValue(int left, int right) {
  if (left < right)
    return COMPARISON_CONST(LOWER);
  if (left > right)
    return COMPARISON_CONST(HIGHER);
  return COMPARISON_CONST(EQUAL);
}

static int DoubleValueFn(int x) { return x * 2; }

static void LogCodeFn(int code) { g_notified = code; }

static OPSTATUS EventValue(int code) {
  g_notified = code * 2;
  return STATUS_CONST(SUCCESS);
}

static bool FilterValue(int value) { return (value % 2) == 0; }

/* ========================================================================== */
/* Main Test Runner                                                           */
/* ========================================================================== */

int main(void) {
  /* 1. Verify sizeof pure callables is exactly sizeof(void *) = 8 bytes */
  assert(sizeof(TCALLABLE(Scale)) == sizeof(void *));
  assert(sizeof(TCALLABLE(AddOffset)) == sizeof(void *));
  assert(sizeof(TCALLABLE(Notify)) == sizeof(void *));
  assert(sizeof(TCALLABLE(ReadValue)) == sizeof(void *));
  assert(sizeof(TCALLABLE(ResetValue)) == sizeof(void *));
  assert(sizeof(TCALLABLE(Commit)) == sizeof(void *));
  assert(sizeof(TCALLABLE(IsPositive)) == sizeof(void *));
  assert(sizeof(TCALLABLE(Flush)) == sizeof(void *));
  assert(sizeof(TCALLABLE(IsEnabled)) == sizeof(void *));
  assert(sizeof(TCALLABLE(CompareInts)) == sizeof(void *));

  /* 2. Direct assignment and invocation */
  TCALLABLE(Scale) scale = ScaleValue;
  assert(CALLABLE_IS_VALID(scale));
  assert(CALLABLE_INVOKE(scale, 4) == 12);
  assert(CALLABLE_CALL(scale, 4) == 12);
  assert(scale(4) == 12);

  TCALLABLE(AddOffset) add = AddOffsetValue;
  assert(CALLABLE_IS_VALID(add));
  assert(CALLABLE_INVOKE(add, 2, 7) == 14);
  assert(CALLABLE_CALL(add, 2, 7) == 14);

  TCALLABLE(Notify) notify = NotifyValue;
  CALLABLE_CALL(notify, 42);
  assert(g_notified == 42);

  TCALLABLE(ReadValue) read = ReadDirect;
  assert(CALLABLE_INVOKE(read) == 42);
  assert(CALLABLE_CALL(read) == 42);

  TCALLABLE(ResetValue) reset = ResetDirect;
  CALLABLE_CALL(reset);
  assert(g_notified == 0);

  TCALLABLE(Scale) invalid = CALLABLE_EMPTY;
  assert(!CALLABLE_IS_VALID(invalid));

  TCALLABLE(Commit) commit = CommitValue;
  assert(CALLABLE_IS_VALID(commit));
  assert(CALLABLE_INVOKE(commit, 7) == STATUS_CONST(SUCCESS));
  assert(CALLABLE_INVOKE(commit, -1) == STATUS_CONST(INVALID_ARGUMENT));

  TCALLABLE(IsPositive) positive = PositiveValue;
  assert(CALLABLE_INVOKE(positive, 5));
  assert(!CALLABLE_INVOKE(positive, -2));

  TCALLABLE(Flush) flush = FlushDirect;
  g_notified = 99;
  assert(CALLABLE_INVOKE(flush) == STATUS_CONST(SUCCESS));
  assert(g_notified == 0);

  TCALLABLE(IsEnabled) enabled = EnabledDirect;
  assert(CALLABLE_INVOKE(enabled));

  TCALLABLE(CompareInts) cmp = CompareIntsValue;
  assert(CALLABLE_IS_VALID(cmp));
  assert(CALLABLE_INVOKE(cmp, 3, 5) == COMPARISON_CONST(LOWER));
  assert(CALLABLE_INVOKE(cmp, 5, 5) == COMPARISON_CONST(EQUAL));
  assert(CALLABLE_INVOKE(cmp, 8, 2) == COMPARISON_CONST(HIGHER));

  /* 3. Scoped Callables in MODULE, PACKAGE, OBJECT */
  MODULE_CALLABLE(TestCallableModule, DoubleVal) dbl = DoubleValueFn;
  assert(dbl(15) == 30);

  MODULE_CALLABLE(TestCallableModule, LogCode) logFn = LogCodeFn;
  logFn(88);
  assert(g_notified == 88);

  MODULE_CALLABLE(TestCallableModule, TestPkg, OnEvent) pkgCb = EventValue;
  assert(CALLABLE_IS_VALID(pkgCb));
  assert(CALLABLE_INVOKE(pkgCb, 21) == STATUS_CONST(SUCCESS));
  assert(g_notified == 42);

  MODULE_CALLABLE(TestCallableModule, TestObj, Filter) objCb = FilterValue;
  assert(CALLABLE_IS_VALID(objCb));
  assert(CALLABLE_INVOKE(objCb, 4) == true);
  assert(CALLABLE_INVOKE(objCb, 7) == false);

  return 0;
}
