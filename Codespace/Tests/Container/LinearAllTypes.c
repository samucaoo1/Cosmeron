#include "Test.h"
#include "../../Cosmeron/Modules/Container/Array/Vector.h"
#include "../../Cosmeron/Modules/Container/Array/Queue.h"
#include "../../Cosmeron/Modules/Container/Array/Stack.h"
#include "../../Cosmeron/Modules/Container/Array/String.h"
#include "../../Cosmeron/Modules/Container/Linked/Forward.h"
#include "../../Cosmeron/Modules/Container/Linked/List.h"
#include "../../Cosmeron/Modules/Container/Linked/Queue.h"
#include "../../Cosmeron/Modules/Container/Linked/Stack.h"
#include "../../Cosmeron/Modules/Container/Linked/Deque.h"

#define TEST_VECTOR(TYPE, SUFFIX, A, B, C)                                    \
  do {                                                                         \
    FLAT_VECTOR_TYPE(SUFFIX) v;                                                \
    TYPE out = (TYPE)0;                                                        \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, Init)(&v) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, Empty)(&v));                          \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, Reserve)(&v, 8U) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, Capacity)(&v) >= 8U);                 \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, PushBack)(&v, (TYPE)(B)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, PushFront)(&v, (TYPE)(A)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, Insert)(&v, 2U, (TYPE)(C)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, Size)(&v) == 3U);                     \
    TEST_ASSERT(*FLAT_VECTOR_FUNC(SUFFIX, Front)(&v) == (TYPE)(A));            \
    TEST_ASSERT(*FLAT_VECTOR_FUNC(SUFFIX, Back)(&v) == (TYPE)(C));             \
    TEST_ASSERT(*FLAT_VECTOR_FUNC(SUFFIX, At)(&v, 1U) == (TYPE)(B));           \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, Data)(&v) != NULL);                   \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, Erase)(&v, 1U, 1U) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, PopFront)(&v, &out) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(out == (TYPE)(A));                                             \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, PopBack)(&v, &out) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(out == (TYPE)(C));                                             \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, ShrinkToFit)(&v) == STATUS_CONST(SUCCESS)); \
    FLAT_VECTOR_FUNC(SUFFIX, Clear)(&v);                                       \
    TEST_ASSERT(FLAT_VECTOR_FUNC(SUFFIX, Empty)(&v));                          \
    FLAT_VECTOR_FUNC(SUFFIX, Destroy)(&v);                                     \
  } while (0)

#define TEST_QUEUE(TYPE, SUFFIX, A, B, C)                                     \
  do {                                                                         \
    FLAT_QUEUE_TYPE(SUFFIX) q;                                                 \
    TYPE out = (TYPE)0;                                                        \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, Init)(&q) == STATUS_CONST(SUCCESS));  \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, Empty)(&q));                           \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, Reserve)(&q, 8U) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, Push)(&q, (TYPE)(A)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, Push)(&q, (TYPE)(B)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, Push)(&q, (TYPE)(C)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(*FLAT_QUEUE_FUNC(SUFFIX, Front)(&q) == (TYPE)(A));             \
    TEST_ASSERT(*FLAT_QUEUE_FUNC(SUFFIX, Back)(&q) == (TYPE)(C));              \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, Pop)(&q, &out) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(out == (TYPE)(A));                                             \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, Compact)(&q) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, Size)(&q) == 2U);                      \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, ShrinkToFit)(&q) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_QUEUE_FUNC(SUFFIX, Capacity)(&q) >= 2U);                  \
    FLAT_QUEUE_FUNC(SUFFIX, Destroy)(&q);                                      \
  } while (0)

#define TEST_STACK(TYPE, SUFFIX, A, B)                                        \
  do {                                                                         \
    FLAT_STACK_TYPE(SUFFIX) s;                                                 \
    TYPE out = (TYPE)0;                                                        \
    TEST_ASSERT(FLAT_STACK_FUNC(SUFFIX, Init)(&s) == STATUS_CONST(SUCCESS));  \
    TEST_ASSERT(FLAT_STACK_FUNC(SUFFIX, Empty)(&s));                           \
    TEST_ASSERT(FLAT_STACK_FUNC(SUFFIX, Reserve)(&s, 8U) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_STACK_FUNC(SUFFIX, Push)(&s, (TYPE)(A)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_STACK_FUNC(SUFFIX, Push)(&s, (TYPE)(B)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(*FLAT_STACK_FUNC(SUFFIX, Top)(&s) == (TYPE)(B));               \
    TEST_ASSERT(FLAT_STACK_FUNC(SUFFIX, Size)(&s) == 2U);                      \
    TEST_ASSERT(FLAT_STACK_FUNC(SUFFIX, Pop)(&s, &out) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(out == (TYPE)(B));                                             \
    TEST_ASSERT(FLAT_STACK_FUNC(SUFFIX, ShrinkToFit)(&s) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(FLAT_STACK_FUNC(SUFFIX, Capacity)(&s) >= 1U);                  \
    FLAT_STACK_FUNC(SUFFIX, Destroy)(&s);                                      \
  } while (0)

#define TEST_STRING(TYPE, SUFFIX)                                              \
  do {                                                                         \
    TSTRING_TYPE(SUFFIX) s, other, slice;                                      \
    TYPE base[] = {(TYPE)'a', (TYPE)'b', 0};                                   \
    TYPE tail[] = {(TYPE)'c', 0};                                              \
    TYPE otherText[] = {(TYPE)'d', (TYPE)'e', 0};                              \
    TYPE needle[] = {(TYPE)'b', (TYPE)'c', 0};                                 \
    TYPE out = (TYPE)0;                                                        \
    size_t index = 0U;                                                         \
    CMPOUT cmp;                                                     \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Init)(&s) == STATUS_CONST(SUCCESS));     \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Init)(&other) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Init)(&slice) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Empty)(&s));                              \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, FromCStr)(&s, base) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, CStr)(&s) != NULL);                       \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Length)(&s) == 2U);                       \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Reserve)(&s, 16U) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Capacity)(&s) >= 16U);                    \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, PushBack)(&s, (TYPE)'c') == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(*TSTRING_FUNC(SUFFIX, Back)(&s) == (TYPE)'c');                 \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, PopBack)(&s, &out) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(out == (TYPE)'c');                                             \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, InsertChar)(&s, 1U, (TYPE)'x') == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(*TSTRING_FUNC(SUFFIX, At)(&s, 1U) == (TYPE)'x');               \
    TEST_ASSERT(*TSTRING_FUNC(SUFFIX, Front)(&s) == (TYPE)'a');                \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Append)(&s, tail) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, FromCStr)(&other, otherText) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, AppendStr)(&s, &other) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Insert)(&s, 0U, tail) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Erase)(&s, 0U, 1U) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Substr)(&s, 0U, 2U, &slice) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Compare)(&s, &other, &cmp) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Find)(&s, (TYPE)'b', 0U, &index) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, FindStr)(&s, needle, 0U, &index) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Data)(&s) != NULL);                       \
    TSTRING_FUNC(SUFFIX, Clear)(&s);                                           \
    TEST_ASSERT(TSTRING_FUNC(SUFFIX, Empty)(&s));                              \
    TSTRING_FUNC(SUFFIX, Destroy)(&slice);                                     \
    TSTRING_FUNC(SUFFIX, Destroy)(&other);                                     \
    TSTRING_FUNC(SUFFIX, Destroy)(&s);                                         \
  } while (0)

#define TEST_FORWARD(TYPE, SUFFIX, A, B, C)                                   \
  do {                                                                         \
    LINKED_FORWARD_LIST_TYPE(SUFFIX) list;                                     \
    LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) *node = NULL;                        \
    TYPE out = (TYPE)0;                                                        \
    TEST_ASSERT(LINKED_FORWARD_LIST_FUNC(SUFFIX, Init)(&list) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_FORWARD_LIST_FUNC(SUFFIX, Empty)(&list));               \
    TEST_ASSERT(LINKED_FORWARD_LIST_FUNC(SUFFIX, PushFront)(&list, (TYPE)(B)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_FORWARD_LIST_FUNC(SUFFIX, PushFront)(&list, (TYPE)(A)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(*LINKED_FORWARD_LIST_FUNC(SUFFIX, Front)(&list) == (TYPE)(A)); \
    TEST_ASSERT(*LINKED_FORWARD_LIST_FUNC(SUFFIX, Back)(&list) == (TYPE)(B));  \
    node = LINKED_FORWARD_LIST_FUNC(SUFFIX, Begin)(&list);                     \
    TEST_ASSERT(node != NULL);                                                 \
    TEST_ASSERT(LINKED_FORWARD_LIST_FUNC(SUFFIX, InsertAfter)(&list, node, (TYPE)(C), NULL) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_FORWARD_LIST_FUNC(SUFFIX, EraseAfter)(&list, node) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_FORWARD_LIST_FUNC(SUFFIX, Size)(&list) == 2U);          \
    TEST_ASSERT(LINKED_FORWARD_LIST_FUNC(SUFFIX, PopFront)(&list, &out) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(out == (TYPE)(A));                                             \
    LINKED_FORWARD_LIST_FUNC(SUFFIX, Clear)(&list);                            \
    TEST_ASSERT(LINKED_FORWARD_LIST_FUNC(SUFFIX, Empty)(&list));               \
    LINKED_FORWARD_LIST_FUNC(SUFFIX, Destroy)(&list);                          \
  } while (0)

#define TEST_LIST(TYPE, SUFFIX, A, B, C)                                      \
  do {                                                                         \
    LINKED_LIST_TYPE(SUFFIX) list;                                             \
    LINKED_LIST_NODE_TYPE(SUFFIX) *node = NULL;                                \
    TYPE out = (TYPE)0;                                                        \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, Init)(&list) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, Empty)(&list));                       \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, PushFront)(&list, (TYPE)(A)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, PushBack)(&list, (TYPE)(B)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(*LINKED_LIST_FUNC(SUFFIX, Front)(&list) == (TYPE)(A));         \
    TEST_ASSERT(*LINKED_LIST_FUNC(SUFFIX, Back)(&list) == (TYPE)(B));          \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, Begin)(&list) == list.head);          \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, End)(&list) == NULL);                 \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, RBegin)(&list) == list.tail);         \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, Insert)(&list, list.tail, (TYPE)(C), &node) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(node != NULL);                                                 \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, Erase)(&list, node, NULL) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, Size)(&list) == 2U);                  \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, PopFront)(&list, &out) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(out == (TYPE)(A));                                             \
    TEST_ASSERT(LINKED_LIST_FUNC(SUFFIX, PopBack)(&list, &out) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(out == (TYPE)(B));                                             \
    LINKED_LIST_FUNC(SUFFIX, Clear)(&list);                                    \
    LINKED_LIST_FUNC(SUFFIX, Destroy)(&list);                                  \
  } while (0)

#define TEST_LINKED_QUEUE(TYPE, SUFFIX, A, B)                                 \
  do {                                                                         \
    LINKED_QUEUE_TYPE(SUFFIX) q;                                               \
    TYPE out = (TYPE)0;                                                        \
    TEST_ASSERT(LINKED_QUEUE_FUNC(SUFFIX, Init)(&q) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_QUEUE_FUNC(SUFFIX, Empty)(&q));                         \
    TEST_ASSERT(LINKED_QUEUE_FUNC(SUFFIX, Push)(&q, (TYPE)(A)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_QUEUE_FUNC(SUFFIX, Push)(&q, (TYPE)(B)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(*LINKED_QUEUE_FUNC(SUFFIX, Front)(&q) == (TYPE)(A));           \
    TEST_ASSERT(*LINKED_QUEUE_FUNC(SUFFIX, Back)(&q) == (TYPE)(B));            \
    TEST_ASSERT(LINKED_QUEUE_FUNC(SUFFIX, Size)(&q) == 2U);                    \
    TEST_ASSERT(LINKED_QUEUE_FUNC(SUFFIX, Pop)(&q, &out) == STATUS_CONST(SUCCESS)); \
    LINKED_QUEUE_FUNC(SUFFIX, Clear)(&q);                                      \
    LINKED_QUEUE_FUNC(SUFFIX, Destroy)(&q);                                    \
  } while (0)

#define TEST_LINKED_STACK(TYPE, SUFFIX, A, B)                                 \
  do {                                                                         \
    LINKED_STACK_TYPE(SUFFIX) s;                                               \
    TYPE out = (TYPE)0;                                                        \
    TEST_ASSERT(LINKED_STACK_FUNC(SUFFIX, Init)(&s) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_STACK_FUNC(SUFFIX, Empty)(&s));                         \
    TEST_ASSERT(LINKED_STACK_FUNC(SUFFIX, Push)(&s, (TYPE)(A)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_STACK_FUNC(SUFFIX, Push)(&s, (TYPE)(B)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(*LINKED_STACK_FUNC(SUFFIX, Top)(&s) == (TYPE)(B));             \
    TEST_ASSERT(LINKED_STACK_FUNC(SUFFIX, Size)(&s) == 2U);                    \
    TEST_ASSERT(LINKED_STACK_FUNC(SUFFIX, Pop)(&s, &out) == STATUS_CONST(SUCCESS)); \
    LINKED_STACK_FUNC(SUFFIX, Clear)(&s);                                      \
    LINKED_STACK_FUNC(SUFFIX, Destroy)(&s);                                    \
  } while (0)

#define TEST_LINKED_DEQUE(TYPE, SUFFIX, A, B)                                 \
  do {                                                                         \
    LINKED_DEQUE_TYPE(SUFFIX) d;                                               \
    TYPE out = (TYPE)0;                                                        \
    TEST_ASSERT(LINKED_DEQUE_FUNC(SUFFIX, Init)(&d) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_DEQUE_FUNC(SUFFIX, Empty)(&d));                         \
    TEST_ASSERT(LINKED_DEQUE_FUNC(SUFFIX, PushFront)(&d, (TYPE)(A)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_DEQUE_FUNC(SUFFIX, PushBack)(&d, (TYPE)(B)) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(*LINKED_DEQUE_FUNC(SUFFIX, Front)(&d) == (TYPE)(A));           \
    TEST_ASSERT(*LINKED_DEQUE_FUNC(SUFFIX, Back)(&d) == (TYPE)(B));            \
    TEST_ASSERT(LINKED_DEQUE_FUNC(SUFFIX, Size)(&d) == 2U);                    \
    TEST_ASSERT(LINKED_DEQUE_FUNC(SUFFIX, PopFront)(&d, &out) == STATUS_CONST(SUCCESS)); \
    TEST_ASSERT(LINKED_DEQUE_FUNC(SUFFIX, PopBack)(&d, &out) == STATUS_CONST(SUCCESS)); \
    LINKED_DEQUE_FUNC(SUFFIX, Clear)(&d);                                      \
    LINKED_DEQUE_FUNC(SUFFIX, Destroy)(&d);                                    \
  } while (0)

int main(void) {
  TEST_VECTOR(int, int, 1, 2, 3);
  TEST_VECTOR(float, float, 1, 2, 3);
  TEST_VECTOR(double, double, 1, 2, 3);
  TEST_QUEUE(int, int, 1, 2, 3);
  TEST_QUEUE(float, float, 1, 2, 3);
  TEST_QUEUE(double, double, 1, 2, 3);
  TEST_STACK(int, int, 1, 2);
  TEST_STACK(float, float, 1, 2);
  TEST_STACK(double, double, 1, 2);

  TEST_STRING(uint8_t, 8);
  TEST_STRING(uint16_t, 16);
  TEST_STRING(uint32_t, 32);

  TEST_FORWARD(int, int, 1, 2, 3);
  TEST_FORWARD(float, float, 1, 2, 3);
  TEST_FORWARD(double, double, 1, 2, 3);
  TEST_FORWARD(char, char, 'a', 'b', 'c');

  TEST_LIST(int, int, 1, 2, 3);
  TEST_LIST(float, float, 1, 2, 3);
  TEST_LIST(double, double, 1, 2, 3);
  TEST_LIST(char, char, 'a', 'b', 'c');
  {
    int a = 1, b = 2, c = 3;
    TEST_LIST(void_ptr, void_ptr, &a, &b, &c);
  }

  TEST_LINKED_QUEUE(int, int, 1, 2);
  TEST_LINKED_QUEUE(float, float, 1, 2);
  TEST_LINKED_QUEUE(double, double, 1, 2);
  TEST_LINKED_QUEUE(char, char, 'a', 'b');
  TEST_LINKED_STACK(int, int, 1, 2);
  TEST_LINKED_STACK(float, float, 1, 2);
  TEST_LINKED_STACK(double, double, 1, 2);
  TEST_LINKED_STACK(char, char, 'a', 'b');
  TEST_LINKED_DEQUE(int, int, 1, 2);
  TEST_LINKED_DEQUE(float, float, 1, 2);
  TEST_LINKED_DEQUE(double, double, 1, 2);
  TEST_LINKED_DEQUE(char, char, 'a', 'b');

  return 0;
}
