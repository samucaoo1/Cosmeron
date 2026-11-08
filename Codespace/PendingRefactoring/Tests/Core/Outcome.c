#include "../../../Cosmeron/Core/Outcome/Status.h"
#include "../../../Cosmeron/Core/Outcome/Result.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

TEMPLATE_INSTANTIATE(Result, int32, int32_t);

#define STATUS_TEST_COUNT(ROW) +1
enum { Outcome_Test_StatusCount = 0 STATUS_TABLE(STATUS_TEST_COUNT) };
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

static OPSTATUS check_positive(int32_t x, MODULE_TYPED(Outcome, Result, int32) *out, void *ctx) {
  (void)ctx;
  if (x > 0) {
    return OBJECT_CALL(out, Ok, x * 10);
  }
  MODULE_TYPE(Condition, Error) e;
  MODULE_FUNC(Condition, Error, Create)(&e, STATUS_CONST(INVALID_ARGUMENT), "Must be positive");
  return OBJECT_CALL(out, Err, &e);
}

int main(void) {
  /* 1. Base status checks */
  if (succeed() != STATUS_CONST(SUCCESS))
    return 1;
  if (fail() != STATUS_CONST(INVALID_ARGUMENT))
    return 2;
  if (Outcome_Test_StatusCount != 18)
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
  assert(MODULE_FUNC(Outcome, Status, IsSuccess)(STATUS_CONST(SUCCESS)) == true);
  assert(MODULE_FUNC(Outcome, Status, IsSuccess)(STATUS_CONST(GENERIC_ERROR)) == false);
  assert(MODULE_FUNC(Outcome, Status, IsFailure)(STATUS_CONST(SUCCESS)) == false);
  assert(MODULE_FUNC(Outcome, Status, IsFailure)(STATUS_CONST(INVALID_ARGUMENT)) == true);

  assert(MODULE_FUNC(Outcome, Status, IsSpecial)(STATUS_CONST(WOULD_BLOCK)) == true);
  assert(MODULE_FUNC(Outcome, Status, IsSpecial)(STATUS_CONST(INCOMPLETE_SEQUENCE)) == true);
  assert(MODULE_FUNC(Outcome, Status, IsSpecial)(STATUS_CONST(SUCCESS)) == false);
  assert(MODULE_FUNC(Outcome, Status, IsSpecial)(STATUS_CONST(OUT_OF_MEMORY)) == false);

  /* 3. Categories and CategoryOf */
  assert(MODULE_FUNC(Outcome, Status, CategoryOf)(STATUS_CONST(SUCCESS)) ==
         STATUS_CATEGORY_CONST(SUCCESS));
  assert(MODULE_FUNC(Outcome, Status, CategoryOf)(STATUS_CONST(INVALID_ARGUMENT)) ==
         STATUS_CATEGORY_CONST(GENERIC));
  assert(MODULE_FUNC(Outcome, Status, CategoryOf)(STATUS_CONST(OUT_OF_MEMORY)) ==
         STATUS_CATEGORY_CONST(MEMORY));
  assert(MODULE_FUNC(Outcome, Status, CategoryOf)(STATUS_CONST(INSUFFICIENT_SPACE)) ==
         STATUS_CATEGORY_CONST(MEMORY));
  assert(MODULE_FUNC(Outcome, Status, CategoryOf)(STATUS_CONST(TIMEOUT)) ==
         STATUS_CATEGORY_CONST(CONCURRENCY));
  assert(MODULE_FUNC(Outcome, Status, CategoryOf)(STATUS_CONST(WOULD_BLOCK)) ==
         STATUS_CATEGORY_CONST(CONCURRENCY));
  assert(MODULE_FUNC(Outcome, Status, CategoryOf)(STATUS_CONST(ARITHMETIC_OVERFLOW)) ==
         STATUS_CATEGORY_CONST(ARITHMETIC));
  assert(MODULE_FUNC(Outcome, Status, CategoryOf)(STATUS_CONST(DIVISION_BY_ZERO)) ==
         STATUS_CATEGORY_CONST(ARITHMETIC));
  assert(MODULE_FUNC(Outcome, Status, CategoryOf)(STATUS_CONST(INVALID_SEQUENCE)) ==
         STATUS_CATEGORY_CONST(SEQUENCE));
  assert(MODULE_FUNC(Outcome, Status, CategoryOf)(STATUS_CONST(INCOMPLETE_SEQUENCE)) ==
         STATUS_CATEGORY_CONST(SEQUENCE));

  /* 4. IsCategory predicate */
  assert(MODULE_FUNC(Outcome, Status, IsCategory)(STATUS_CONST(OUT_OF_MEMORY),
                                                  STATUS_CATEGORY_CONST(MEMORY)) == true);
  assert(MODULE_FUNC(Outcome, Status, IsCategory)(STATUS_CONST(OUT_OF_MEMORY),
                                                  STATUS_CATEGORY_CONST(GENERIC)) == false);

  /* 5. Names and Messages */
  assert(strcmp(MODULE_FUNC(Outcome, Status, Name)(STATUS_CONST(SUCCESS)), "SUCCESS") == 0);
  assert(strcmp(MODULE_FUNC(Outcome, Status, Name)(STATUS_CONST(OUT_OF_RANGE)), "OUT_OF_RANGE") == 0);
  assert(strcmp(MODULE_FUNC(Outcome, Status, Message)(STATUS_CONST(SUCCESS)), "Success") == 0);
  assert(strcmp(MODULE_FUNC(Outcome, Status, Message)(STATUS_CONST(NOT_FOUND)), "Not found") == 0);
  assert(strcmp(MODULE_FUNC(Outcome, Status, CategoryName)(STATUS_CATEGORY_CONST(CONCURRENCY)),
                "Concurrency") == 0);

  /* 6. RESULT operations and queries */
  MODULE_TYPED(Outcome, Result, int32) res;
  assert(MODULE_FUNC(Outcome, Result, int32, Init)(&res) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&res, Ok, 100) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&res, IsOk) == true);
  assert(OBJECT_CALL_0(&res, IsError) == false);
  assert(OBJECT_CALL(&res, ValueOr, 999) == 100);

  int32_t val = 0;
  assert(OBJECT_CALL(&res, Value, &val) == STATUS_CONST(SUCCESS));
  assert(val == 100);

  MODULE_TYPE(Condition, Error) extractedErr;
  assert(OBJECT_CALL(&res, Error, &extractedErr) == STATUS_CONST(INVALID_ARGUMENT));

  /* Err result */
  MODULE_TYPE(Condition, Error) customErr;
  assert(MODULE_FUNC(Condition, Error, Create)(&customErr, STATUS_CONST(OUT_OF_MEMORY), "Allocation failed") ==
         STATUS_CONST(SUCCESS));
  MODULE_TYPED(Outcome, Result, int32) errRes;
  assert(MODULE_FUNC(Outcome, Result, int32, Init)(&errRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&errRes, Err, &customErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&errRes, IsOk) == false);
  assert(OBJECT_CALL_0(&errRes, IsError) == true);
  assert(OBJECT_CALL(&errRes, ValueOr, 999) == 999);

  val = 42;
  assert(OBJECT_CALL(&errRes, Value, &val) == STATUS_CONST(OUT_OF_MEMORY));
  assert(val == 42); /* out untouched */

  assert(OBJECT_CALL(&errRes, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&extractedErr, Code) == STATUS_CONST(OUT_OF_MEMORY));
  assert(strcmp(OBJECT_CALL_0(&extractedErr, Message), "Allocation failed") == 0);

  /* FromStatus */
  MODULE_TYPED(Outcome, Result, int32) statusRes;
  assert(MODULE_FUNC(Outcome, Result, int32, Init)(&statusRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&statusRes, FromStatus, STATUS_CONST(TIMEOUT)) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&statusRes, IsOk) == false);
  assert(OBJECT_CALL(&statusRes, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&extractedErr, Code) == STATUS_CONST(TIMEOUT));
  assert(strcmp(OBJECT_CALL_0(&extractedErr, Message), "Timeout") == 0);

  assert(OBJECT_CALL(&statusRes, FromStatus, STATUS_CONST(SUCCESS)) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&statusRes, IsOk) == true);

  /* Map: on Ok */
  TCALLBACK(MODULE_CALLBACK(Outcome, Result, int32, MapFunction)) mapCb = {
      .function = double_val,
      .context = NULL
  };

  MODULE_TYPED(Outcome, Result, int32) mappedRes;
  assert(MODULE_FUNC(Outcome, Result, int32, Init)(&mappedRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&res, Map, mapCb, &mappedRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&mappedRes, IsOk) == true);
  assert(OBJECT_CALL(&mappedRes, ValueOr, 0) == 200);

  /* Map: on Err */
  MODULE_TYPED(Outcome, Result, int32) mappedErr;
  assert(MODULE_FUNC(Outcome, Result, int32, Init)(&mappedErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&errRes, Map, mapCb, &mappedErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&mappedErr, IsError) == true);
  assert(OBJECT_CALL(&mappedErr, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&extractedErr, Code) == STATUS_CONST(OUT_OF_MEMORY));

  /* AndThen: on Ok -> Ok */
  TCALLBACK(MODULE_CALLBACK(Outcome, Result, int32, BindFunction)) bindCb = {
      .function = check_positive,
      .context = NULL
  };

  MODULE_TYPED(Outcome, Result, int32) boundRes;
  assert(MODULE_FUNC(Outcome, Result, int32, Init)(&boundRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&res, AndThen, bindCb, &boundRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundRes, IsOk) == true);
  assert(OBJECT_CALL(&boundRes, ValueOr, 0) == 1000);

  /* AndThen: on Ok -> Err */
  MODULE_TYPED(Outcome, Result, int32) negRes;
  assert(MODULE_FUNC(Outcome, Result, int32, Init)(&negRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&negRes, Ok, -5) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&negRes, AndThen, bindCb, &boundRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundRes, IsError) == true);
  assert(OBJECT_CALL(&boundRes, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&extractedErr, Code) == STATUS_CONST(INVALID_ARGUMENT));
  assert(strcmp(OBJECT_CALL_0(&extractedErr, Message), "Must be positive") == 0);

  /* AndThen: on Err -> preserves original Err */
  assert(OBJECT_CALL(&errRes, AndThen, bindCb, &boundRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundRes, IsError) == true);
  assert(OBJECT_CALL(&boundRes, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&extractedErr, Code) == STATUS_CONST(OUT_OF_MEMORY));

  return 0;
}
