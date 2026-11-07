#include "Test.h"
#include "../../Cosmeron/Modules/Container/Array/Span.h"
#include "../../Cosmeron/Modules/Container/Array/Vector.h"

static int TestSpanLifecycleAndEmpty(void) {
  TSpan(int, span);
  TEST_ASSERT(Container_Flat_Span_int_Init(&span) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Span_int_IsEmpty(&span) == true);
  TEST_ASSERT(Container_Flat_Span_int_Size(&span) == 0U);
  TEST_ASSERT(Container_Flat_Span_int_SizeBytes(&span) == 0U);

  int *item = NULL;
  TEST_ASSERT(Container_Flat_Span_int_Front(&span, &item) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(Container_Flat_Span_int_Back(&span, &item) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(Container_Flat_Span_int_At(&span, 0U, &item) == STATUS_CONST(OUT_OF_RANGE));

  /* Test Empty helper */
  TSpan_int emptySpan = Container_Flat_Span_int_Empty();
  TEST_ASSERT(Container_Flat_Span_int_IsEmpty(&emptySpan) == true);
  TEST_ASSERT(Container_Flat_Span_int_Size(&emptySpan) == 0U);
  return 0;
}

static int TestSpanFromBufferAndAccess(void) {
  int buffer[] = {10, 20, 30, 40, 50};
  TSpan(int, span);

  /* Invalid argument checks */
  TEST_ASSERT(Container_Flat_Span_int_FromBuffer(NULL, buffer, 5U) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(Container_Flat_Span_int_FromBuffer(&span, NULL, 5U) == STATUS_CONST(INVALID_ARGUMENT));

  /* Valid buffer binding */
  TEST_ASSERT(Container_Flat_Span_int_FromBuffer(&span, buffer, 5U) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(!Container_Flat_Span_int_IsEmpty(&span));
  TEST_ASSERT(Container_Flat_Span_int_Size(&span) == 5U);
  TEST_ASSERT(Container_Flat_Span_int_SizeBytes(&span) == 5U * sizeof(int));

  /* Data pointer access */
  int *data = NULL;
  TEST_ASSERT(Container_Flat_Span_int_Data(&span, &data) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(data == buffer);

  const int *cdata = NULL;
  TEST_ASSERT(Container_Flat_Span_int_ConstData(&span, &cdata) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(cdata == buffer);

  /* Front and Back */
  int *front = NULL;
  int *back = NULL;
  TEST_ASSERT(Container_Flat_Span_int_Front(&span, &front) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(front != NULL && *front == 10);
  TEST_ASSERT(Container_Flat_Span_int_Back(&span, &back) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(back != NULL && *back == 50);

  /* At and ConstAt */
  for (size_t i = 0U; i < 5U; ++i) {
    int *elem = NULL;
    const int *celem = NULL;
    TEST_ASSERT(Container_Flat_Span_int_At(&span, i, &elem) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(elem != NULL && *elem == buffer[i]);
    TEST_ASSERT(Container_Flat_Span_int_ConstAt(&span, i, &celem) == STATUS_CONST(SUCCESS));
    TEST_ASSERT(celem != NULL && *celem == buffer[i]);
  }

  /* Out of range */
  int *elem = NULL;
  TEST_ASSERT(Container_Flat_Span_int_At(&span, 5U, &elem) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(Container_Flat_Span_int_At(&span, 100U, &elem) == STATUS_CONST(OUT_OF_RANGE));

  /* Mutation through Span */
  *front = 99;
  TEST_ASSERT(buffer[0] == 99);
  buffer[0] = 10; /* Restore */
  return 0;
}

static int TestSpanCreateByValue(void) {
  int arr[] = {1, 2, 3};
  TSpan_int span = Container_Flat_Span_int_Create(arr, 3U);
  TEST_ASSERT(Container_Flat_Span_int_Size(&span) == 3U);
  TEST_ASSERT(!Container_Flat_Span_int_IsEmpty(&span));

  int *val = NULL;
  TEST_ASSERT(Container_Flat_Span_int_At(&span, 1U, &val) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(val != NULL && *val == 2);
  return 0;
}

static int TestSpanSlicing(void) {
  int numbers[] = {100, 200, 300, 400, 500, 600};
  TSpan_int full = Container_Flat_Span_int_Create(numbers, 6U);

  /* Subspan */
  TSpan_int mid;
  TEST_ASSERT(Container_Flat_Span_int_Subspan(&full, 2U, 3U, &mid) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Span_int_Size(&mid) == 3U);
  int *val = NULL;
  TEST_ASSERT(Container_Flat_Span_int_At(&mid, 0U, &val) == STATUS_CONST(SUCCESS) && *val == 300);
  TEST_ASSERT(Container_Flat_Span_int_At(&mid, 1U, &val) == STATUS_CONST(SUCCESS) && *val == 400);
  TEST_ASSERT(Container_Flat_Span_int_At(&mid, 2U, &val) == STATUS_CONST(SUCCESS) && *val == 500);

  /* First (prefix) */
  TSpan_int prefix;
  TEST_ASSERT(Container_Flat_Span_int_First(&full, 2U, &prefix) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Span_int_Size(&prefix) == 2U);
  TEST_ASSERT(Container_Flat_Span_int_At(&prefix, 0U, &val) == STATUS_CONST(SUCCESS) && *val == 100);
  TEST_ASSERT(Container_Flat_Span_int_At(&prefix, 1U, &val) == STATUS_CONST(SUCCESS) && *val == 200);

  /* Last (suffix) */
  TSpan_int suffix;
  TEST_ASSERT(Container_Flat_Span_int_Last(&full, 2U, &suffix) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Span_int_Size(&suffix) == 2U);
  TEST_ASSERT(Container_Flat_Span_int_At(&suffix, 0U, &val) == STATUS_CONST(SUCCESS) && *val == 500);
  TEST_ASSERT(Container_Flat_Span_int_At(&suffix, 1U, &val) == STATUS_CONST(SUCCESS) && *val == 600);

  /* Out of range slicing errors */
  TSpan_int errSpan;
  TEST_ASSERT(Container_Flat_Span_int_Subspan(&full, 7U, 1U, &errSpan) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(Container_Flat_Span_int_Subspan(&full, 4U, 3U, &errSpan) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(Container_Flat_Span_int_First(&full, 7U, &errSpan) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(Container_Flat_Span_int_Last(&full, 7U, &errSpan) == STATUS_CONST(OUT_OF_RANGE));
  return 0;
}

static int TestSpanEquality(void) {
  int arr1[] = {1, 2, 3, 4};
  int arr2[] = {1, 2, 3, 4};
  int arr3[] = {1, 2, 9, 4};
  int arr4[] = {1, 2, 3};

  TSpan_int s1 = Container_Flat_Span_int_Create(arr1, 4U);
  TSpan_int s2 = Container_Flat_Span_int_Create(arr2, 4U);
  TSpan_int s3 = Container_Flat_Span_int_Create(arr3, 4U);
  TSpan_int s4 = Container_Flat_Span_int_Create(arr4, 3U);

  TEST_ASSERT(Container_Flat_Span_int_Equal(&s1, &s2) == true);
  TEST_ASSERT(Container_Flat_Span_int_Equal(&s1, &s3) == false);
  TEST_ASSERT(Container_Flat_Span_int_Equal(&s1, &s4) == false);
  TEST_ASSERT(Container_Flat_Span_int_Equal(NULL, NULL) == true);
  TEST_ASSERT(Container_Flat_Span_int_Equal(&s1, NULL) == false);
  return 0;
}

static int TestSpanFromVector(void) {
  TVector(int, vec);
  TEST_ASSERT(Container_Flat_Vector_int_PushBack(&vec, 11) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Vector_int_PushBack(&vec, 22) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Vector_int_PushBack(&vec, 33) == STATUS_CONST(SUCCESS));

  TSpan(int, span);
  TEST_ASSERT(Container_Flat_Span_int_FromVector(&span, &vec) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Span_int_Size(&span) == 3U);

  int *elem = NULL;
  TEST_ASSERT(Container_Flat_Span_int_At(&span, 0U, &elem) == STATUS_CONST(SUCCESS) && *elem == 11);
  TEST_ASSERT(Container_Flat_Span_int_At(&span, 1U, &elem) == STATUS_CONST(SUCCESS) && *elem == 22);
  TEST_ASSERT(Container_Flat_Span_int_At(&span, 2U, &elem) == STATUS_CONST(SUCCESS) && *elem == 33);

  Container_Flat_Vector_int_Destroy(&vec);
  return 0;
}

static int TestSpanFunctionTable(void) {
  int arr[] = {10, 20, 30};
  TSpan(int, span);
  TEST_ASSERT(span.api != NULL);
  TEST_ASSERT(span.api->fromBuffer(&span, arr, 3U) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(span.api->size(&span) == 3U);
  TEST_ASSERT(!span.api->isEmpty(&span));

  int *elem = NULL;
  TEST_ASSERT(span.api->at(&span, 1U, &elem) == STATUS_CONST(SUCCESS) && *elem == 20);

  TSpan(int, sub);
  TEST_ASSERT(span.api->first(&span, 2U, &sub) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(sub.api->size(&sub) == 2U);

  TSpan(int, tail);
  TEST_ASSERT(span.api->last(&span, 1U, &tail) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(tail.api->size(&tail) == 1U);
  return 0;
}

static int TestSpanFloatAndDouble(void) {
  float fArr[] = {1.5f, 2.5f, 3.5f};
  TSpan(float, fSpan);
  TEST_ASSERT(Container_Flat_Span_float_FromBuffer(&fSpan, fArr, 3U) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Span_float_Size(&fSpan) == 3U);
  float *fVal = NULL;
  TEST_ASSERT(Container_Flat_Span_float_At(&fSpan, 1U, &fVal) == STATUS_CONST(SUCCESS) && *fVal == 2.5f);

  double dArr[] = {10.01, 20.02};
  TSpan(double, dSpan);
  TEST_ASSERT(Container_Flat_Span_double_FromBuffer(&dSpan, dArr, 2U) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Span_double_Size(&dSpan) == 2U);
  double *dVal = NULL;
  TEST_ASSERT(Container_Flat_Span_double_Back(&dSpan, &dVal) == STATUS_CONST(SUCCESS) && *dVal == dArr[1]);
  return 0;
}

int main(void) {
  if (TestSpanLifecycleAndEmpty() != 0) return 1;
  if (TestSpanFromBufferAndAccess() != 0) return 1;
  if (TestSpanCreateByValue() != 0) return 1;
  if (TestSpanSlicing() != 0) return 1;
  if (TestSpanEquality() != 0) return 1;
  if (TestSpanFromVector() != 0) return 1;
  if (TestSpanFunctionTable() != 0) return 1;
  if (TestSpanFloatAndDouble() != 0) return 1;
  return 0;
}
