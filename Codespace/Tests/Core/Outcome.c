#include "../../Cosmeron/Core/Outcome/Status.h"
#include "../../Cosmeron/Core/Outcome/Result.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

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

static OPSTATUS check_positive(int32_t x, RESULT_TYPE(int32) *out, void *ctx) {
  (void)ctx;
  if (x > 0) {
    return OBJECT_CALL(out, Ok, x * 10);
  }
  ERROR_TYPE(Error) e;
  TError_Create(&e, STATUS_CONST(INVALID_ARGUMENT), "Must be positive");
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
         STATUS_CATEGORY_CONST(SUCCESS));
  assert(Status_CategoryOf(STATUS_CONST(INVALID_ARGUMENT)) ==
         STATUS_CATEGORY_CONST(GENERIC));
  assert(Status_CategoryOf(STATUS_CONST(OUT_OF_MEMORY)) ==
         STATUS_CATEGORY_CONST(MEMORY));
  assert(Status_CategoryOf(STATUS_CONST(INSUFFICIENT_SPACE)) ==
         STATUS_CATEGORY_CONST(MEMORY));
  assert(Status_CategoryOf(STATUS_CONST(TIMEOUT)) ==
         STATUS_CATEGORY_CONST(CONCURRENCY));
  assert(Status_CategoryOf(STATUS_CONST(WOULD_BLOCK)) ==
         STATUS_CATEGORY_CONST(CONCURRENCY));
  assert(Status_CategoryOf(STATUS_CONST(ARITHMETIC_OVERFLOW)) ==
         STATUS_CATEGORY_CONST(ARITHMETIC));
  assert(Status_CategoryOf(STATUS_CONST(DIVISION_BY_ZERO)) ==
         STATUS_CATEGORY_CONST(ARITHMETIC));
  assert(Status_CategoryOf(STATUS_CONST(INVALID_SEQUENCE)) ==
         STATUS_CATEGORY_CONST(SEQUENCE));
  assert(Status_CategoryOf(STATUS_CONST(INCOMPLETE_SEQUENCE)) ==
         STATUS_CATEGORY_CONST(SEQUENCE));

  /* 4. IsCategory predicate */
  assert(Status_IsCategory(STATUS_CONST(OUT_OF_MEMORY),
                           STATUS_CATEGORY_CONST(MEMORY)) == true);
  assert(Status_IsCategory(STATUS_CONST(OUT_OF_MEMORY),
                           STATUS_CATEGORY_CONST(GENERIC)) == false);

  /* 5. Names and Messages */
  assert(strcmp(Status_Name(STATUS_CONST(SUCCESS)), "SUCCESS") == 0);
  assert(strcmp(Status_Name(STATUS_CONST(OUT_OF_RANGE)), "OUT_OF_RANGE") == 0);
  assert(strcmp(Status_Message(STATUS_CONST(SUCCESS)), "Success") == 0);
  assert(strcmp(Status_Message(STATUS_CONST(NOT_FOUND)), "Not found") == 0);
  assert(strcmp(Status_CategoryName(STATUS_CATEGORY_CONST(CONCURRENCY)),
                "Concurrency") == 0);

  /* 6. PACKAGE_CALL verification */
  assert(PACKAGE_CALL(Status, IsSuccess, STATUS_CONST(SUCCESS)) == true);
  assert(strcmp(PACKAGE_CALL(Status, Message, STATUS_CONST(TIMEOUT)),
                "Timeout") == 0);

  /* 7. RESULT_TYPE(SUFFIX) operations and queries */
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
  assert(OBJECT_CALL(&res, Error, &extractedErr) == STATUS_CONST(INVALID_ARGUMENT));

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
  assert(OBJECT_CALL(&statusRes, FromStatus, STATUS_CONST(TIMEOUT)) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&statusRes, IsOk) == false);
  assert(OBJECT_CALL(&statusRes, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(TError_Code(&extractedErr) == STATUS_CONST(TIMEOUT));
  assert(strcmp(TError_Message(&extractedErr), "Timeout") == 0);

  assert(OBJECT_CALL(&statusRes, FromStatus, STATUS_CONST(SUCCESS)) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&statusRes, IsOk) == true);

  /* Map: on Ok */
  RESULT_TYPE(int32) mappedRes;
  assert(TResult_int32_Init(&mappedRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&res, Map, RESULT_MAP_BIND(int32, double_val, NULL), &mappedRes) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&mappedRes, IsOk) == true);
  assert(OBJECT_CALL(&mappedRes, ValueOr, 0) == 200);

  /* Map: on Err */
  RESULT_TYPE(int32) mappedErr;
  assert(TResult_int32_Init(&mappedErr) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&errRes, Map, RESULT_MAP_BIND(int32, double_val, NULL), &mappedErr) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&mappedErr, IsError) == true);
  assert(OBJECT_CALL(&mappedErr, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(TError_Code(&extractedErr) == STATUS_CONST(OUT_OF_MEMORY));

  /* AndThen: on Ok -> Ok */
  RESULT_TYPE(int32) boundRes;
  assert(TResult_int32_Init(&boundRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&res, AndThen, RESULT_BIND_BIND(int32, check_positive, NULL), &boundRes) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundRes, IsOk) == true);
  assert(OBJECT_CALL(&boundRes, ValueOr, 0) == 1000);

  /* AndThen: on Ok -> Err */
  RESULT_TYPE(int32) negRes;
  assert(TResult_int32_Init(&negRes) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&negRes, Ok, -5) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&negRes, AndThen, RESULT_BIND_BIND(int32, check_positive, NULL), &boundRes) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundRes, IsError) == true);
  assert(OBJECT_CALL(&boundRes, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(TError_Code(&extractedErr) == STATUS_CONST(INVALID_ARGUMENT));
  assert(strcmp(TError_Message(&extractedErr), "Must be positive") == 0);

  /* AndThen: on Err -> preserves original Err */
  assert(OBJECT_CALL(&errRes, AndThen, RESULT_BIND_BIND(int32, check_positive, NULL), &boundRes) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&boundRes, IsError) == true);
  assert(OBJECT_CALL(&boundRes, Error, &extractedErr) == STATUS_CONST(SUCCESS));
  assert(TError_Code(&extractedErr) == STATUS_CONST(OUT_OF_MEMORY));

  return 0;
}

