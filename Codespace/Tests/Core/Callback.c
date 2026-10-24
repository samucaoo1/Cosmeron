/* Compile with: cc -std=c11 -Wall -Wextra -pedantic -I Codespace
 * Codespace/Tests/Core/Callback.c -o callback-test && ./callback-test */
#include "Cosmeron/Core/Module/Callback/Callback.h"
#include <assert.h>

CALLBACK_DECLARE(Scale, int, (int value), (value))
CALLBACK_DECLARE(AddOffset, int, (int left, int right), (left, right))
CALLBACK_DECLARE_PROCEDURE(Notify, (int code), (code))
CALLBACK_DECLARE_NULLARY(ReadValue, int)
CALLBACK_DECLARE_NULLARY_PROCEDURE(ResetValue)

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

int main(void) {
  int factor = 3;
  CALLBACK_TYPE(Scale) scale =
      CALLBACK_FUNC(Scale_Bind)(ScaleValue, &factor);
  assert(CALLBACK_FUNC(Scale_IsValid)(scale));
  assert(CALLBACK_FUNC(Scale_Invoke)(scale, 4) == 12);

  int offset = 5;
  CALLBACK_TYPE(AddOffset) add =
      CALLBACK_FUNC(AddOffset_Bind)(AddOffsetValue, &offset);
  assert(CALLBACK_FUNC(AddOffset_IsValid)(add));
  assert(CALLBACK_FUNC(AddOffset_Invoke)(add, 2, 7) == 14);

  int received = -1;
  CALLBACK_TYPE(Notify) notify =
      CALLBACK_FUNC(Notify_Bind)(NotifyValue, &received);
  CALLBACK_FUNC(Notify_Invoke)(notify, 42);
  assert(received == 42);

  CALLBACK_TYPE(ReadValue) read =
      CALLBACK_FUNC(ReadValue_Bind)(ReadContext, &factor);
  assert(CALLBACK_FUNC(ReadValue_Invoke)(read) == 3);

  int value = 9;
  CALLBACK_TYPE(ResetValue) reset =
      CALLBACK_FUNC(ResetValue_Bind)(ResetContext, &value);
  CALLBACK_FUNC(ResetValue_Invoke)(reset);
  assert(value == 0);

  CALLBACK_TYPE(Scale) invalid =
      CALLBACK_FUNC(Scale_Bind)(NULL, NULL);
  assert(!CALLBACK_FUNC(Scale_IsValid)(invalid));
  return 0;
}
