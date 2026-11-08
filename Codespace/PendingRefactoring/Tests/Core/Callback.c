/* Compile with: cc -std=c11 -Wall -Wextra -pedantic -I Codespace
 * Codespace/Tests/Core/Callback.c -o callback-test && ./callback-test */
#include "../../../Cosmeron/Core/Module/Callback/Callback.h"
#include "../../../Cosmeron/Core/Module/Package.h"
#include "../../../Cosmeron/Core/Module/Object.h"
#include <assert.h>

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

PACKAGE(TestPkg,
  CALLBACK(
    OPERATION(OnEvent, (int code))
  )
);

OBJECT(TestObj,
  FIELDS(
    int id;
  ),
  CALLBACK(
    PREDICATE(Filter, (int value))
  )
);

static OPSTATUS CommitValue(int value, void *context) {
  *(int *)context += value;
  return value >= 0 ? STATUS_CONST(SUCCESS) : STATUS_CONST(INVALID_ARGUMENT);
}

static bool PositiveValue(int value, void *context) {
  return value > *(int *)context;
}

static OPSTATUS FlushContext(void *context) {
  *(int *)context = 0;
  return STATUS_CONST(SUCCESS);
}

static bool EnabledContext(void *context) {
  return *(bool *)context;
}

static int ScaleValue(int value, void *context) {
  return value * *(int *)context;
}

static int AddOffsetValue(int left, int right, void *context) {
  return left + right + *(int *)context;
}

static void NotifyValue(int code, void *context) {
  *(int *)context = code;
}

static int ReadContext(void *context) {
  return *(int *)context;
}

static void ResetContext(void *context) {
  *(int *)context = 0;
}

static CMPOUT CompareIntsValue(int left, int right, void *context) {
  (void)context;
  if (left < right) return COMPARISON_CONST(LOWER);
  if (left > right) return COMPARISON_CONST(HIGHER);
  return COMPARISON_CONST(EQUAL);
}

static OPSTATUS EventValue(int code, void *context) {
  *(int *)context = code * 2;
  return STATUS_CONST(SUCCESS);
}

static bool FilterValue(int value, void *context) {
  return value % 2 == *(int *)context;
}

int main(void) {
  int factor = 3;
  TCALLBACK(Scale) scale =
      CALLBACK_FUNC(Scale_Bind)(ScaleValue, &factor);
  assert(CALLBACK_FUNC(Scale_IsValid)(scale));
  assert(scale.IsValid(&scale));
  assert(CALLBACK_IS_VALID(scale));
  assert(CALLBACK_INVOKE(scale, 4) == 12);
  assert(CALLBACK_CALL(scale, 4) == 12);

  int offset = 5;
  TCALLBACK(AddOffset) add =
      CALLBACK_FUNC(AddOffset_Bind)(AddOffsetValue, &offset);
  assert(add.IsValid(&add));
  assert(CALLBACK_INVOKE(add, 2, 7) == 14);
  assert(CALLBACK_CALL(add, 2, 7) == 14);

  int received = -1;
  TCALLBACK(Notify) notify =
      CALLBACK_FUNC(Notify_Bind)(NotifyValue, &received);
  CALLBACK_CALL(notify, 42);
  assert(received == 42);

  int readFactor = 3;
  TCALLBACK(ReadValue) read =
      CALLBACK_FUNC(ReadValue_Bind)(ReadContext, &readFactor);
  assert(CALLBACK_FUNC(ReadValue_Invoke)(read) == 3);
  assert(CALLBACK_INVOKE(read) == 3);
  assert(CALLBACK_CALL(read) == 3);

  int value = 9;
  TCALLBACK(ResetValue) reset =
      CALLBACK_FUNC(ResetValue_Bind)(ResetContext, &value);
  CALLBACK_FUNC(ResetValue_Invoke)(reset);
  assert(value == 0);
  value = 5;
  CALLBACK_INVOKE(reset);
  assert(value == 0);

  TCALLBACK(Scale) invalid =
      CALLBACK_FUNC(Scale_Bind)(NULL, NULL);
  assert(!invalid.IsValid(&invalid));
  assert(!CALLBACK_IS_VALID(invalid));

  int committed = 10;
  TCALLBACK(Commit) commit = CALLBACK_FUNC(Commit_Bind)(CommitValue, &committed);
  assert(commit.IsValid(&commit));
  assert(CALLBACK_INVOKE(commit, 7) == STATUS_CONST(SUCCESS));
  assert(committed == 17);
  assert(CALLBACK_INVOKE(commit, -1) == STATUS_CONST(INVALID_ARGUMENT));

  int threshold = 4;
  TCALLBACK(IsPositive) positive =
      CALLBACK_FUNC(IsPositive_Bind)(PositiveValue, &threshold);
  assert(CALLBACK_INVOKE(positive, 5));
  assert(!CALLBACK_INVOKE(positive, 4));

  TCALLBACK(Flush) flush = CALLBACK_FUNC(Flush_Bind)(FlushContext, &committed);
  assert(CALLBACK_INVOKE(flush) == STATUS_CONST(SUCCESS));
  assert(committed == 0);

  bool enabledValue = true;
  TCALLBACK(IsEnabled) enabled =
      CALLBACK_FUNC(IsEnabled_Bind)(EnabledContext, &enabledValue);
  assert(CALLBACK_INVOKE(enabled));
  enabledValue = false;
  assert(!CALLBACK_INVOKE(enabled));

  /* Test COMPARISON callback */
  TCALLBACK(CompareInts) cmp = CALLBACK_FUNC(CompareInts_Bind)(CompareIntsValue, NULL);
  assert(cmp.IsValid(&cmp));
  assert(CALLBACK_IS_VALID(cmp));
  assert(CALLBACK_INVOKE(cmp, 3, 5) == COMPARISON_CONST(LOWER));
  assert(CALLBACK_INVOKE(cmp, 5, 5) == COMPARISON_CONST(EQUAL));
  assert(CALLBACK_INVOKE(cmp, 8, 2) == COMPARISON_CONST(HIGHER));

  /* Test Bind method on struct */
  TCALLBACK(Scale) scaleMethod = {0};
  scaleMethod.Bind(&scaleMethod, ScaleValue, &factor);
  assert(scaleMethod.IsValid(&scaleMethod));
  assert(CALLBACK_INVOKE(scaleMethod, 10) == 30);

  /* Test CALLBACK in PACKAGE */
  int eventData = 0;
  TCALLBACK(TestPkg_OnEvent) pkgCb = CALLBACK_FUNC(TestPkg_OnEvent_Bind)(EventValue, &eventData);
  assert(pkgCb.IsValid(&pkgCb));
  assert(CALLBACK_IS_VALID(pkgCb));
  assert(CALLBACK_INVOKE(pkgCb, 21) == STATUS_CONST(SUCCESS));
  assert(eventData == 42);

  /* Test CALLBACK in OBJECT */
  int parity = 0; /* even */
  TCALLBACK(TestObj_Filter) objCb = CALLBACK_FUNC(TestObj_Filter_Bind)(FilterValue, &parity);
  assert(objCb.IsValid(&objCb));
  assert(CALLBACK_IS_VALID(objCb));
  assert(CALLBACK_INVOKE(objCb, 4) == true);
  assert(CALLBACK_INVOKE(objCb, 7) == false);

  return 0;
}
