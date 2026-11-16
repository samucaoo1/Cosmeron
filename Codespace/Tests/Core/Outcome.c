#include "../../../Cosmeron/Core/Outcome/Status.h"
#include "../../../Cosmeron/Core/Outcome/Result.h"
#include "../../../Cosmeron/Core/Outcome/Optional.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

TEMPLATE_INSTANTIATE(Result, int32, int32_t);
TEMPLATE_INSTANTIATE(Optional, int32, int32_t);

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

static OPSTATUS check_positive_opt(int32_t x, MODULE_TYPED(Outcome, Optional, int32) *out, void *ctx) {
  (void)ctx;
  if (x > 0) {
    return OBJECT_CALL(out, Some, x * 10);
  }
  return OBJECT_CALL_0(out, None);
}

static bool is_even(int32_t x, void *ctx) {
  (void)ctx;
  return (x % 2) == 0;
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
  MODULE_CLOSURE(Outcome, Result, int32, MapFunction) mapCb =
      NS_JOIN(MODULE_CLOSURE_FUNC(Outcome, Result, int32, MapFunction), Bind)(double_val, NULL);

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
  MODULE_CLOSURE(Outcome, Result, int32, BindFunction) bindCb =
      NS_JOIN(MODULE_CLOSURE_FUNC(Outcome, Result, int32, BindFunction), Bind)(check_positive, NULL);

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

  /* 7. OPTIONAL operations and queries */
  MODULE_TYPED(Outcome, Optional, int32) opt;
  assert(MODULE_FUNC(Outcome, Optional, int32, Init)(&opt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&opt, None) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&opt, HasValue) == false);
  assert(OBJECT_CALL_0(&opt, IsSome) == false);
  assert(OBJECT_CALL_0(&opt, IsNone) == true);
  assert(OBJECT_CALL(&opt, ValueOr, 999) == 999);
  assert(OBJECT_CALL_0(&opt, ValuePointer) == NULL);
  assert(OBJECT_CALL_0(&opt, MutablePointer) == NULL);

  int32_t optVal = 55;
  assert(OBJECT_CALL(&opt, Value, &optVal) == STATUS_CONST(NOT_FOUND));
  assert(optVal == 55); /* out unchanged on failure */

  /* Some */
  assert(OBJECT_CALL(&opt, Some, 42) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&opt, HasValue) == true);
  assert(OBJECT_CALL_0(&opt, IsSome) == true);
  assert(OBJECT_CALL_0(&opt, IsNone) == false);
  assert(OBJECT_CALL(&opt, ValueOr, 999) == 42);

  assert(OBJECT_CALL(&opt, Value, &optVal) == STATUS_CONST(SUCCESS));
  assert(optVal == 42);

  const int32_t *constPtr = OBJECT_CALL_0(&opt, ValuePointer);
  assert(constPtr != NULL && *constPtr == 42);

  int32_t *mutPtr = OBJECT_CALL_0(&opt, MutablePointer);
  assert(mutPtr != NULL && *mutPtr == 42);
  *mutPtr = 50;
  assert(OBJECT_CALL(&opt, ValueOr, 0) == 50);

  /* Clear */
  OBJECT_CALL_0(&opt, Clear);
  assert(OBJECT_CALL_0(&opt, HasValue) == false);
  assert(OBJECT_CALL_0(&opt, IsNone) == true);

  /* FromPointer */
  int32_t sample = 77;
  assert(OBJECT_CALL(&opt, FromPointer, &sample) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&opt, IsSome) == true);
  assert(OBJECT_CALL(&opt, ValueOr, 0) == 77);

  assert(OBJECT_CALL(&opt, FromPointer, NULL) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&opt, IsNone) == true);

  /* Map: on Some */
  assert(OBJECT_CALL(&opt, Some, 25) == STATUS_CONST(SUCCESS));
  MODULE_CLOSURE(Outcome, Optional, int32, MapFunction) optMapCb =
      NS_JOIN(MODULE_CLOSURE_FUNC(Outcome, Optional, int32, MapFunction), Bind)(double_val, NULL);
  MODULE_TYPED(Outcome, Optional, int32) mappedOpt;
  assert(MODULE_FUNC(Outcome, Optional, int32, Init)(&mappedOpt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&opt, Map, optMapCb, &mappedOpt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&mappedOpt, IsSome) == true);
  assert(OBJECT_CALL(&mappedOpt, ValueOr, 0) == 50);

  /* Map: on None */
  OBJECT_CALL_0(&opt, Clear);
  assert(OBJECT_CALL(&opt, Map, optMapCb, &mappedOpt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&mappedOpt, IsNone) == true);

  /* AndThen: on Some -> Some */
  assert(OBJECT_CALL(&opt, Some, 5) == STATUS_CONST(SUCCESS));
  MODULE_CLOSURE(Outcome, Optional, int32, BindFunction) optBindCb =
      NS_JOIN(MODULE_CLOSURE_FUNC(Outcome, Optional, int32, BindFunction), Bind)(check_positive_opt, NULL);
  MODULE_TYPED(Outcome, Optional, int32) boundOpt;
  assert(MODULE_FUNC(Outcome, Optional, int32, Init)(&boundOpt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&opt, AndThen, optBindCb, &boundOpt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundOpt, IsSome) == true);
  assert(OBJECT_CALL(&boundOpt, ValueOr, 0) == 50);

  /* AndThen: on Some -> None */
  assert(OBJECT_CALL(&opt, Some, -3) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&opt, AndThen, optBindCb, &boundOpt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundOpt, IsNone) == true);

  /* AndThen: on None -> None */
  OBJECT_CALL_0(&opt, Clear);
  assert(OBJECT_CALL(&opt, AndThen, optBindCb, &boundOpt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundOpt, IsNone) == true);

  /* Filter: with predicate */
  MODULE_CLOSURE(Outcome, Optional, int32, FilterPredicate) filterCb =
      NS_JOIN(MODULE_CLOSURE_FUNC(Outcome, Optional, int32, FilterPredicate), Bind)(is_even, NULL);
  MODULE_TYPED(Outcome, Optional, int32) filteredOpt;
  assert(MODULE_FUNC(Outcome, Optional, int32, Init)(&filteredOpt) == STATUS_CONST(SUCCESS));

  /* Filter: 10 is even -> Some(10) */
  assert(OBJECT_CALL(&opt, Some, 10) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&opt, Filter, filterCb, &filteredOpt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&filteredOpt, IsSome) == true);
  assert(OBJECT_CALL(&filteredOpt, ValueOr, 0) == 10);

  /* Filter: 11 is not even -> None */
  assert(OBJECT_CALL(&opt, Some, 11) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&opt, Filter, filterCb, &filteredOpt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&filteredOpt, IsNone) == true);

  /* Filter: on None -> None */
  OBJECT_CALL_0(&opt, Clear);
  assert(OBJECT_CALL(&opt, Filter, filterCb, &filteredOpt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&filteredOpt, IsNone) == true);

  return 0;
}
