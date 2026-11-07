#include "Test.h"
#include "../../Cosmeron/Modules/Container/Array/Ring.h"
#include "../../Cosmeron/Modules/Container/Linked/Ring.h"

#include <stdint.h>
#include <string.h>

static int Test_LifecycleAndArguments(void) {
  int storage[4];
  FLAT_RING_TYPE(int) ring;

  TEST_ASSERT(FLAT_RING_FUNC(int, Init)(NULL, storage, 4U) ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(FLAT_RING_FUNC(int, Init)(&ring, NULL, 4U) ==
              STATUS_CONST(INVALID_ARGUMENT));

  TEST_ASSERT(FLAT_RING_FUNC(int, Init)(&ring, storage, 4U) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Empty)(&ring));
  TEST_ASSERT(!FLAT_RING_FUNC(int, Full)(&ring));
  TEST_ASSERT(FLAT_RING_FUNC(int, Size)(&ring) == 0U);
  TEST_ASSERT(FLAT_RING_FUNC(int, Capacity)(&ring) == 4U);
  TEST_ASSERT(FLAT_RING_FUNC(int, Available)(&ring) == 4U);

  FLAT_RING_FUNC(int, Destroy)(&ring);
  TEST_ASSERT(FLAT_RING_FUNC(int, Capacity)(&ring) == 0U);
  return 0;
}

static int Test_PushPopPeek(void) {
  int storage[3];
  FLAT_RING_TYPE(int) ring;
  TEST_ASSERT(FLAT_RING_FUNC(int, Init)(&ring, storage, 3U) ==
              STATUS_CONST(SUCCESS));

  int out = -1;
  /* Empty pop & peek */
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &out) ==
              STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(out == -1); /* Untouched on error */
  TEST_ASSERT(FLAT_RING_FUNC(int, Peek)(&ring, &out) ==
              STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(out == -1);

  /* Null pointer checks */
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, NULL) ==
              STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(FLAT_RING_FUNC(int, Peek)(&ring, NULL) ==
              STATUS_CONST(INVALID_ARGUMENT));

  /* Push items */
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 10) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 20) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 30) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Full)(&ring));
  TEST_ASSERT(FLAT_RING_FUNC(int, Size)(&ring) == 3U);
  TEST_ASSERT(FLAT_RING_FUNC(int, Available)(&ring) == 0U);

  /* Overflow push */
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 40) ==
              STATUS_CONST(INSUFFICIENT_SPACE));

  /* Peek front */
  TEST_ASSERT(FLAT_RING_FUNC(int, Peek)(&ring, &out) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(out == 10);
  TEST_ASSERT(FLAT_RING_FUNC(int, Size)(&ring) == 3U);

  /* Pop items in FIFO order */
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &out) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(out == 10);
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &out) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(out == 20);
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &out) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(out == 30);
  TEST_ASSERT(FLAT_RING_FUNC(int, Empty)(&ring));
  return 0;
}

static int Test_FrontBackAtAndWrapAround(void) {
  int storage[4];
  FLAT_RING_TYPE(int) ring;
  TEST_ASSERT(FLAT_RING_FUNC(int, Init)(&ring, storage, 4U) ==
              STATUS_CONST(SUCCESS));

  int *ptr = NULL;
  TEST_ASSERT(FLAT_RING_FUNC(int, Front)(&ring, &ptr) ==
              STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(FLAT_RING_FUNC(int, Back)(&ring, &ptr) ==
              STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(FLAT_RING_FUNC(int, At)(&ring, 0U, &ptr) ==
              STATUS_CONST(OUT_OF_RANGE));

  /* Push 3 elements */
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 1) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 2) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 3) == STATUS_CONST(SUCCESS));

  /* Pop 2 elements */
  int out = 0;
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &out) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(out == 1);
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &out) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(out == 2);
  /* Ring currently contains [3] at index 2 */

  /* Push 3 elements to wrap around the boundary: capacity 4 */
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 4) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 5) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 6) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Full)(&ring));
  TEST_ASSERT(FLAT_RING_FUNC(int, Size)(&ring) == 4U);

  /* Front should be 3, Back should be 6 */
  TEST_ASSERT(FLAT_RING_FUNC(int, Front)(&ring, &ptr) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(*ptr == 3);
  TEST_ASSERT(FLAT_RING_FUNC(int, Back)(&ring, &ptr) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(*ptr == 6);

  /* Verify At indexing across wrap */
  TEST_ASSERT(FLAT_RING_FUNC(int, At)(&ring, 0U, &ptr) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(*ptr == 3);
  TEST_ASSERT(FLAT_RING_FUNC(int, At)(&ring, 1U, &ptr) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(*ptr == 4);
  TEST_ASSERT(FLAT_RING_FUNC(int, At)(&ring, 2U, &ptr) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(*ptr == 5);
  TEST_ASSERT(FLAT_RING_FUNC(int, At)(&ring, 3U, &ptr) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(*ptr == 6);
  TEST_ASSERT(FLAT_RING_FUNC(int, At)(&ring, 4U, &ptr) ==
              STATUS_CONST(OUT_OF_RANGE));

  /* Clear */
  FLAT_RING_FUNC(int, Clear)(&ring);
  TEST_ASSERT(FLAT_RING_FUNC(int, Empty)(&ring));
  TEST_ASSERT(FLAT_RING_FUNC(int, Size)(&ring) == 0U);
  return 0;
}

static int Test_Overwrite(void) {
  int storage[3];
  FLAT_RING_TYPE(int) ring;
  TEST_ASSERT(FLAT_RING_FUNC(int, Init)(&ring, storage, 3U) ==
              STATUS_CONST(SUCCESS));

  TEST_ASSERT(FLAT_RING_FUNC(int, Overwrite)(&ring, 100, NULL) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Overwrite)(&ring, 200, NULL) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Overwrite)(&ring, 300, NULL) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Full)(&ring));

  int dropped = 0;
  /* Ring is full. Overwrite should drop oldest (100) and enqueue 400 */
  TEST_ASSERT(FLAT_RING_FUNC(int, Overwrite)(&ring, 400, &dropped) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(dropped == 100);
  TEST_ASSERT(FLAT_RING_FUNC(int, Size)(&ring) == 3U);

  /* Next oldest is 200. Overwrite with 500 */
  TEST_ASSERT(FLAT_RING_FUNC(int, Overwrite)(&ring, 500, &dropped) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(dropped == 200);

  /* Drain remaining items: should be 300, 400, 500 */
  int val = 0;
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &val) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(val == 300);
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &val) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(val == 400);
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &val) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(val == 500);
  TEST_ASSERT(FLAT_RING_FUNC(int, Empty)(&ring));
  return 0;
}

static int Test_BulkWriteAndRead(void) {
  int storage[6];
  FLAT_RING_TYPE(int) ring;
  TEST_ASSERT(FLAT_RING_FUNC(int, Init)(&ring, storage, 6U) ==
              STATUS_CONST(SUCCESS));

  /* Advance head/tail to offset 4 so bulk ops must wrap around */
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 1) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 2) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 3) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Push)(&ring, 4) == STATUS_CONST(SUCCESS));
  int dummy = 0;
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &dummy) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &dummy) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &dummy) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Pop)(&ring, &dummy) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Empty)(&ring));

  /* tail is now at index 4, capacity is 6. Available = 6 */
  const int input[5] = {10, 20, 30, 40, 50};
  size_t written = 0U;
  /* Partial write of 5 items: 2 go to end [4,5], 3 wrap to [0,1,2] */
  TEST_ASSERT(FLAT_RING_FUNC(int, Write)(&ring, input, 5U, &written) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(written == 5U);
  TEST_ASSERT(FLAT_RING_FUNC(int, Size)(&ring) == 5U);

  /* Exact write: try to write 2 items when available is only 1 -> fails */
  const int extra[2] = {98, 99};
  TEST_ASSERT(FLAT_RING_FUNC(int, Write)(&ring, extra, 2U, NULL) ==
              STATUS_CONST(INSUFFICIENT_SPACE));

  /* Write 1 item with exact write -> succeeds */
  TEST_ASSERT(FLAT_RING_FUNC(int, Write)(&ring, extra, 1U, NULL) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Full)(&ring));

  /* Read 6 items out in bulk: should wrap across boundary */
  int output[6] = {0};
  size_t readCount = 0U;
  TEST_ASSERT(FLAT_RING_FUNC(int, Read)(&ring, output, 6U, &readCount) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(readCount == 6U);
  TEST_ASSERT(output[0] == 10);
  TEST_ASSERT(output[1] == 20);
  TEST_ASSERT(output[2] == 30);
  TEST_ASSERT(output[3] == 40);
  TEST_ASSERT(output[4] == 50);
  TEST_ASSERT(output[5] == 98);
  TEST_ASSERT(FLAT_RING_FUNC(int, Empty)(&ring));
  return 0;
}

static int Test_ZeroCopySpans(void) {
  int storage[5];
  FLAT_RING_TYPE(int) ring;
  TEST_ASSERT(FLAT_RING_FUNC(int, Init)(&ring, storage, 5U) ==
              STATUS_CONST(SUCCESS));

  /* Initially empty: write span should cover entire capacity */
  int *writePtr = NULL;
  size_t writeCount = 0U;
  TEST_ASSERT(FLAT_RING_FUNC(int, GetWriteSpan)(&ring, &writePtr, &writeCount) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(writePtr == &storage[0]);
  TEST_ASSERT(writeCount == 5U);

  /* Write 3 items directly into span without calling Push */
  writePtr[0] = 111;
  writePtr[1] = 222;
  writePtr[2] = 333;
  TEST_ASSERT(FLAT_RING_FUNC(int, AdvanceWrite)(&ring, 3U) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Size)(&ring) == 3U);

  /* Read span should now return pointer to 111 and count 3 */
  int *readPtr = NULL;
  size_t readCount = 0U;
  TEST_ASSERT(FLAT_RING_FUNC(int, GetReadSpan)(&ring, &readPtr, &readCount) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(readPtr == &storage[0]);
  TEST_ASSERT(readCount == 3U);
  TEST_ASSERT(readPtr[0] == 111);
  TEST_ASSERT(readPtr[1] == 222);
  TEST_ASSERT(readPtr[2] == 333);

  /* Consume 2 items directly from span */
  TEST_ASSERT(FLAT_RING_FUNC(int, AdvanceRead)(&ring, 2U) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Size)(&ring) == 1U);

  /* Now tail is at index 3. Write span to end of buffer is 2 (slots 3 and 4) */
  TEST_ASSERT(FLAT_RING_FUNC(int, GetWriteSpan)(&ring, &writePtr, &writeCount) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(writePtr == &storage[3]);
  TEST_ASSERT(writeCount == 2U);
  writePtr[0] = 444;
  writePtr[1] = 555;
  TEST_ASSERT(FLAT_RING_FUNC(int, AdvanceWrite)(&ring, 2U) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Size)(&ring) == 3U);

  /* Now tail wraps around to index 0! Available is 2 slots [0, 1] */
  TEST_ASSERT(FLAT_RING_FUNC(int, GetWriteSpan)(&ring, &writePtr, &writeCount) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(writePtr == &storage[0]);
  TEST_ASSERT(writeCount == 2U);
  writePtr[0] = 666;
  writePtr[1] = 777;
  TEST_ASSERT(FLAT_RING_FUNC(int, AdvanceWrite)(&ring, 2U) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Full)(&ring));

  /* Reading wrapped: read span from index 2 to end [2, 3, 4] gives count 3 */
  TEST_ASSERT(FLAT_RING_FUNC(int, GetReadSpan)(&ring, &readPtr, &readCount) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(readPtr == &storage[2]);
  TEST_ASSERT(readCount == 3U);
  TEST_ASSERT(readPtr[0] == 333);
  TEST_ASSERT(readPtr[1] == 444);
  TEST_ASSERT(readPtr[2] == 555);

  TEST_ASSERT(FLAT_RING_FUNC(int, AdvanceRead)(&ring, 3U) ==
              STATUS_CONST(SUCCESS));

  /* Second read span wraps to [0, 1] gives count 2 */
  TEST_ASSERT(FLAT_RING_FUNC(int, GetReadSpan)(&ring, &readPtr, &readCount) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(readPtr == &storage[0]);
  TEST_ASSERT(readCount == 2U);
  TEST_ASSERT(readPtr[0] == 666);
  TEST_ASSERT(readPtr[1] == 777);

  TEST_ASSERT(FLAT_RING_FUNC(int, AdvanceRead)(&ring, 2U) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(FLAT_RING_FUNC(int, Empty)(&ring));
  return 0;
}

static int Test_ByteBuffer(void) {
  uint8_t buffer[16];
  TBoundedBuffer ring;
  TEST_ASSERT(FLAT_RING_FUNC(byte, Init)(&ring, buffer, sizeof(buffer)) ==
              STATUS_CONST(SUCCESS));

  const char *msg = "Hello, Cosmeron!";
  size_t written = 0U;
  TEST_ASSERT(FLAT_RING_FUNC(byte, Write)(&ring, (const uint8_t *)msg, 16U,
                                          &written) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(written == 16U);
  TEST_ASSERT(FLAT_RING_FUNC(byte, Full)(&ring));

  uint8_t outBuf[17] = {0};
  size_t readBytes = 0U;
  TEST_ASSERT(FLAT_RING_FUNC(byte, Read)(&ring, outBuf, 16U, &readBytes) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(readBytes == 16U);
  TEST_ASSERT(memcmp(outBuf, msg, 16U) == 0);
  TEST_ASSERT(FLAT_RING_FUNC(byte, Empty)(&ring));
  return 0;
}

static int Test_FunctionTable(void) {
#if COSMERON_MACRO_INTERNAL_CONTAINER_FUNCTION_TABLE_ENABLED
  int buffer[4];
  TFlat_Ring(int, ring, buffer, 4U);

  TEST_ASSERT(ring.api != NULL);
  TEST_ASSERT(ring.api->empty(&ring));
  TEST_ASSERT(ring.api->push(&ring, 42) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(ring.api->size(&ring) == 1U);

  int val = 0;
  TEST_ASSERT(ring.api->peek(&ring, &val) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(val == 42);

  TEST_ASSERT(ring.api->pop(&ring, &val) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(val == 42);
  TEST_ASSERT(ring.api->empty(&ring));
#endif
  return 0;
}

static int Test_LinkedRing_Dynamic(void) {
  LINKED_RING_TYPE(int) ring;
  TEST_ASSERT(LINKED_RING_FUNC(int, Init)(NULL, 4U) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(LINKED_RING_FUNC(int, Init)(&ring, 0U) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(LINKED_RING_FUNC(int, Init)(&ring, 3U) == STATUS_CONST(SUCCESS));

  TEST_ASSERT(LINKED_RING_FUNC(int, Empty)(&ring));
  TEST_ASSERT(!LINKED_RING_FUNC(int, Full)(&ring));
  TEST_ASSERT(LINKED_RING_FUNC(int, Size)(&ring) == 0U);
  TEST_ASSERT(LINKED_RING_FUNC(int, Capacity)(&ring) == 3U);
  TEST_ASSERT(LINKED_RING_FUNC(int, Available)(&ring) == 3U);

  int val = 0;
  TEST_ASSERT(LINKED_RING_FUNC(int, Pop)(&ring, &val) == STATUS_CONST(OUT_OF_RANGE));
  TEST_ASSERT(LINKED_RING_FUNC(int, Peek)(&ring, &val) == STATUS_CONST(OUT_OF_RANGE));

  TEST_ASSERT(LINKED_RING_FUNC(int, Push)(&ring, 100) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(LINKED_RING_FUNC(int, Push)(&ring, 200) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(LINKED_RING_FUNC(int, Push)(&ring, 300) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(LINKED_RING_FUNC(int, Full)(&ring));
  TEST_ASSERT(LINKED_RING_FUNC(int, Push)(&ring, 400) == STATUS_CONST(INSUFFICIENT_SPACE));

  int *ptr = NULL;
  TEST_ASSERT(LINKED_RING_FUNC(int, Front)(&ring, &ptr) == STATUS_CONST(SUCCESS) && *ptr == 100);
  TEST_ASSERT(LINKED_RING_FUNC(int, Back)(&ring, &ptr) == STATUS_CONST(SUCCESS) && *ptr == 300);
  TEST_ASSERT(LINKED_RING_FUNC(int, At)(&ring, 0U, &ptr) == STATUS_CONST(SUCCESS) && *ptr == 100);
  TEST_ASSERT(LINKED_RING_FUNC(int, At)(&ring, 1U, &ptr) == STATUS_CONST(SUCCESS) && *ptr == 200);
  TEST_ASSERT(LINKED_RING_FUNC(int, At)(&ring, 2U, &ptr) == STATUS_CONST(SUCCESS) && *ptr == 300);
  TEST_ASSERT(LINKED_RING_FUNC(int, At)(&ring, 3U, &ptr) == STATUS_CONST(OUT_OF_RANGE));

  TEST_ASSERT(LINKED_RING_FUNC(int, Peek)(&ring, &val) == STATUS_CONST(SUCCESS) && val == 100);
  TEST_ASSERT(LINKED_RING_FUNC(int, Pop)(&ring, &val) == STATUS_CONST(SUCCESS) && val == 100);
  TEST_ASSERT(LINKED_RING_FUNC(int, Pop)(&ring, &val) == STATUS_CONST(SUCCESS) && val == 200);
  TEST_ASSERT(LINKED_RING_FUNC(int, Pop)(&ring, &val) == STATUS_CONST(SUCCESS) && val == 300);
  TEST_ASSERT(LINKED_RING_FUNC(int, Empty)(&ring));

  LINKED_RING_FUNC(int, Destroy)(&ring);
  TEST_ASSERT(LINKED_RING_FUNC(int, Capacity)(&ring) == 0U);
  return 0;
}

static int Test_LinkedRing_StaticAndOverwrite(void) {
  LINKED_RING_NODE_TYPE(int) nodes[3];
  LINKED_RING_TYPE(int) ring;
  TEST_ASSERT(LINKED_RING_FUNC(int, InitStatic)(&ring, nodes, 3U) == STATUS_CONST(SUCCESS));

  TEST_ASSERT(LINKED_RING_FUNC(int, Push)(&ring, 1) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(LINKED_RING_FUNC(int, Push)(&ring, 2) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(LINKED_RING_FUNC(int, Push)(&ring, 3) == STATUS_CONST(SUCCESS));

  int dropped = 0;
  TEST_ASSERT(LINKED_RING_FUNC(int, Overwrite)(&ring, 4, &dropped) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(dropped == 1);
  TEST_ASSERT(LINKED_RING_FUNC(int, Overwrite)(&ring, 5, &dropped) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(dropped == 2);

  int *backPtr = NULL;
  TEST_ASSERT(LINKED_RING_FUNC(int, Back)(&ring, &backPtr) == STATUS_CONST(SUCCESS) && *backPtr == 5);

  int val = 0;
  TEST_ASSERT(LINKED_RING_FUNC(int, Pop)(&ring, &val) == STATUS_CONST(SUCCESS) && val == 3);
  TEST_ASSERT(LINKED_RING_FUNC(int, Pop)(&ring, &val) == STATUS_CONST(SUCCESS) && val == 4);
  TEST_ASSERT(LINKED_RING_FUNC(int, Pop)(&ring, &val) == STATUS_CONST(SUCCESS) && val == 5);
  TEST_ASSERT(LINKED_RING_FUNC(int, Empty)(&ring));

  LINKED_RING_FUNC(int, Destroy)(&ring);
  return 0;
}

static int Test_LinkedRing_FunctionTable(void) {
#if COSMERON_MACRO_INTERNAL_CONTAINER_FUNCTION_TABLE_ENABLED
  TLinked_Ring(int, ring, 4U);
  TEST_ASSERT(ring.api != NULL);
  TEST_ASSERT(ring.api->empty(&ring));
  TEST_ASSERT(ring.api->push(&ring, 99) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(ring.api->size(&ring) == 1U);
  int val = 0;
  TEST_ASSERT(ring.api->pop(&ring, &val) == STATUS_CONST(SUCCESS) && val == 99);
  TEST_ASSERT(ring.api->empty(&ring));
  ring.api->destroy(&ring);
#endif
  return 0;
}

int main(void) {
  if (Test_LifecycleAndArguments() != 0) return 1;
  if (Test_PushPopPeek() != 0) return 1;
  if (Test_FrontBackAtAndWrapAround() != 0) return 1;
  if (Test_Overwrite() != 0) return 1;
  if (Test_BulkWriteAndRead() != 0) return 1;
  if (Test_ZeroCopySpans() != 0) return 1;
  if (Test_ByteBuffer() != 0) return 1;
  if (Test_FunctionTable() != 0) return 1;
  if (Test_LinkedRing_Dynamic() != 0) return 1;
  if (Test_LinkedRing_StaticAndOverwrite() != 0) return 1;
  if (Test_LinkedRing_FunctionTable() != 0) return 1;
  return 0;
}
