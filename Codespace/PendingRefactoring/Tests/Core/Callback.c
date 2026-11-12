/* Compile with:
 * gcc -std=c11 -Wall -Wextra -Werror -pedantic -I Codespace Codespace/PendingRefactoring/Tests/Core/Callback.c -o /tmp/test_callback && /tmp/test_callback
 */
#include "Cosmeron/Core/Algorithm/Comparison.h"
#include "Cosmeron/Core/Outcome/Status.h"
#include "Cosmeron/Core/Module/Callback/Callback.h"
#include "Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <stdbool.h>

/* ========================================================================== */
/* 1. Standalone Pure Callbacks                                               */
/* ========================================================================== */

CALLBACK_FUNCTION(int, Scale, (int value));
CALLBACK_FUNCTION(int, AddOffset, (int left, int right));
CALLBACK_PROCEDURE(Notify, (int code));
CALLBACK_FUNCTION(int, ReadValue, ());
CALLBACK_PROCEDURE(ResetValue, ());

CALLBACK_OPERATION(Commit, (int value));
CALLBACK_PREDICATE(IsPositive, (int value));
CALLBACK_OPERATION(Flush, ());
CALLBACK_PREDICATE(IsEnabled, ());
CALLBACK_COMPARISON(CompareInts, (int left, int right));

/* ========================================================================== */
/* 2. Declarative Module with Callbacks                                       */
/* ========================================================================== */

MODULE(DOMAIN(Core), TestCallbackModule,
  CALLBACK(
    FUNCTION(int, DoubleVal, (int x)),
    PROCEDURE(LogCode, (int code))
  ),
  PACKAGE(TestPkg,
    CALLBACK(
      OPERATION(OnEvent, (int code))
    )
  ),
  OBJECT(TestObj,
    FIELDS(
      int id;
    ),
    CALLBACK(
      PREDICATE(Filter, (int value))
    )
  )
);

/* ========================================================================== */
/* Stateless Target Functions                                                 */
/* ========================================================================== */

static int ScaleValue(int value) {
  return value * 3;
}

static int AddOffsetValue(int left, int right) {
  return left + right + 5;
}

static int g_notified = 0;
static void NotifyValue(int code) {
  g_notified = code;
}

static int ReadDirect(void) {
  return 42;
}

static void ResetDirect(void) {
  g_notified = 0;
}

static OPSTATUS CommitValue(int value) {
  return value >= 0 ? STATUS_CONST(SUCCESS) : STATUS_CONST(INVALID_ARGUMENT);
}

static bool PositiveValue(int value) {
  return value > 0;
}

static OPSTATUS FlushDirect(void) {
  g_notified = 0;
  return STATUS_CONST(SUCCESS);
}

static bool EnabledDirect(void) {
  return true;
}

static CMPOUT CompareIntsValue(int left, int right) {
  if (left < right) return COMPARISON_CONST(LOWER);
  if (left > right) return COMPARISON_CONST(HIGHER);
  return COMPARISON_CONST(EQUAL);
}

static int DoubleValueFn(int x) {
  return x * 2;
}

static void LogCodeFn(int code) {
  g_notified = code;
}

static OPSTATUS EventValue(int code) {
  g_notified = code * 2;
  return STATUS_CONST(SUCCESS);
}

static bool FilterValue(int value) {
  return (value % 2) == 0;
}

/* ========================================================================== */
/* Main Test Runner                                                           */
/* ========================================================================== */

int main(void) {
  /* 1. Verify sizeof pure callbacks is exactly sizeof(void *) = 8 bytes */
  assert(sizeof(TCALLBACK(Scale)) == sizeof(void *));
  assert(sizeof(TCALLBACK(AddOffset)) == sizeof(void *));
  assert(sizeof(TCALLBACK(Notify)) == sizeof(void *));
  assert(sizeof(TCALLBACK(ReadValue)) == sizeof(void *));
  assert(sizeof(TCALLBACK(ResetValue)) == sizeof(void *));
  assert(sizeof(TCALLBACK(Commit)) == sizeof(void *));
  assert(sizeof(TCALLBACK(IsPositive)) == sizeof(void *));
  assert(sizeof(TCALLBACK(Flush)) == sizeof(void *));
  assert(sizeof(TCALLBACK(IsEnabled)) == sizeof(void *));
  assert(sizeof(TCALLBACK(CompareInts)) == sizeof(void *));

  /* 2. Direct assignment and invocation */
  TCALLBACK(Scale) scale = ScaleValue;
  assert(CALLBACK_IS_VALID(scale));
  assert(CALLBACK_INVOKE(scale, 4) == 12);
  assert(CALLBACK_CALL(scale, 4) == 12);
  assert(scale(4) == 12);

  TCALLBACK(AddOffset) add = AddOffsetValue;
  assert(CALLBACK_IS_VALID(add));
  assert(CALLBACK_INVOKE(add, 2, 7) == 14);
  assert(CALLBACK_CALL(add, 2, 7) == 14);

  TCALLBACK(Notify) notify = NotifyValue;
  CALLBACK_CALL(notify, 42);
  assert(g_notified == 42);

  TCALLBACK(ReadValue) read = ReadDirect;
  assert(CALLBACK_INVOKE(read) == 42);
  assert(CALLBACK_CALL(read) == 42);

  TCALLBACK(ResetValue) reset = ResetDirect;
  CALLBACK_CALL(reset);
  assert(g_notified == 0);

  TCALLBACK(Scale) invalid = CALLBACK_EMPTY;
  assert(!CALLBACK_IS_VALID(invalid));

  TCALLBACK(Commit) commit = CommitValue;
  assert(CALLBACK_IS_VALID(commit));
  assert(CALLBACK_INVOKE(commit, 7) == STATUS_CONST(SUCCESS));
  assert(CALLBACK_INVOKE(commit, -1) == STATUS_CONST(INVALID_ARGUMENT));

  TCALLBACK(IsPositive) positive = PositiveValue;
  assert(CALLBACK_INVOKE(positive, 5));
  assert(!CALLBACK_INVOKE(positive, -2));

  TCALLBACK(Flush) flush = FlushDirect;
  g_notified = 99;
  assert(CALLBACK_INVOKE(flush) == STATUS_CONST(SUCCESS));
  assert(g_notified == 0);

  TCALLBACK(IsEnabled) enabled = EnabledDirect;
  assert(CALLBACK_INVOKE(enabled));

  TCALLBACK(CompareInts) cmp = CompareIntsValue;
  assert(CALLBACK_IS_VALID(cmp));
  assert(CALLBACK_INVOKE(cmp, 3, 5) == COMPARISON_CONST(LOWER));
  assert(CALLBACK_INVOKE(cmp, 5, 5) == COMPARISON_CONST(EQUAL));
  assert(CALLBACK_INVOKE(cmp, 8, 2) == COMPARISON_CONST(HIGHER));

  /* 3. Scoped Callbacks in MODULE, PACKAGE, OBJECT */
  MODULE_CALLBACK(TestCallbackModule, DoubleVal) dbl = DoubleValueFn;
  assert(dbl(15) == 30);

  MODULE_CALLBACK(TestCallbackModule, LogCode) logFn = LogCodeFn;
  logFn(88);
  assert(g_notified == 88);

  MODULE_CALLBACK(TestCallbackModule, TestPkg, OnEvent) pkgCb = EventValue;
  assert(CALLBACK_IS_VALID(pkgCb));
  assert(CALLBACK_INVOKE(pkgCb, 21) == STATUS_CONST(SUCCESS));
  assert(g_notified == 42);

  MODULE_CALLBACK(TestCallbackModule, TestObj, Filter) objCb = FilterValue;
  assert(CALLBACK_IS_VALID(objCb));
  assert(CALLBACK_INVOKE(objCb, 4) == true);
  assert(CALLBACK_INVOKE(objCb, 7) == false);

  return 0;
}
