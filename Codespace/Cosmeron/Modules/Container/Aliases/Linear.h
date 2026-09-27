#pragma once

#include "../Array/Vector.h"
#include "../Array/Queue.h"
#include "../Array/Stack.h"
#include "../Array/String.h"
#include "../Linked/List.h"
#include "../Linked/Forward.h"
#include "../Linked/Deque.h"
#include "../Linked/Queue.h"
#include "../Linked/Stack.h"

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define CONTAINER_ALIAS_SELECT_2(_1, _2, NAME, ...) NAME
#define CONTAINER_ALIAS_SELECT_4(_1, _2, _3, _4, NAME, ...) NAME

#define CONTAINER_VECTOR_ASSOC(OP)                                             \
  FLAT_VECTOR_TYPE(int) *: FLAT_VECTOR_FUNC(int, OP),                         \
  FLAT_VECTOR_TYPE(float) *: FLAT_VECTOR_FUNC(float, OP),                     \
  FLAT_VECTOR_TYPE(double) *: FLAT_VECTOR_FUNC(double, OP)

#define CONTAINER_FLAT_QUEUE_ASSOC(OP)                                         \
  FLAT_QUEUE_TYPE(int) *: FLAT_QUEUE_FUNC(int, OP),                           \
  FLAT_QUEUE_TYPE(float) *: FLAT_QUEUE_FUNC(float, OP),                       \
  FLAT_QUEUE_TYPE(double) *: FLAT_QUEUE_FUNC(double, OP)

#define CONTAINER_FLAT_STACK_ASSOC(OP)                                         \
  FLAT_STACK_TYPE(int) *: FLAT_STACK_FUNC(int, OP),                           \
  FLAT_STACK_TYPE(float) *: FLAT_STACK_FUNC(float, OP),                       \
  FLAT_STACK_TYPE(double) *: FLAT_STACK_FUNC(double, OP)

#define CONTAINER_STRING_ASSOC(OP)                                             \
  TSTRING_TYPE(8) *: TSTRING_FUNC(8, OP),                                     \
  TSTRING_TYPE(16) *: TSTRING_FUNC(16, OP),                                   \
  TSTRING_TYPE(32) *: TSTRING_FUNC(32, OP)

#define CONTAINER_LIST_ASSOC(OP)                                               \
  LINKED_LIST_TYPE(int) *: LINKED_LIST_FUNC(int, OP),                         \
  LINKED_LIST_TYPE(float) *: LINKED_LIST_FUNC(float, OP),                     \
  LINKED_LIST_TYPE(double) *: LINKED_LIST_FUNC(double, OP)

#define CONTAINER_FORWARD_ASSOC(OP)                                            \
  LINKED_FORWARD_LIST_TYPE(int) *: LINKED_FORWARD_LIST_FUNC(int, OP),         \
  LINKED_FORWARD_LIST_TYPE(float) *: LINKED_FORWARD_LIST_FUNC(float, OP),     \
  LINKED_FORWARD_LIST_TYPE(double) *: LINKED_FORWARD_LIST_FUNC(double, OP)

#define CONTAINER_DEQUE_ASSOC(OP)                                              \
  LINKED_DEQUE_TYPE(int) *: LINKED_DEQUE_FUNC(int, OP),                       \
  LINKED_DEQUE_TYPE(float) *: LINKED_DEQUE_FUNC(float, OP),                   \
  LINKED_DEQUE_TYPE(double) *: LINKED_DEQUE_FUNC(double, OP)

#define CONTAINER_LINKED_QUEUE_ASSOC(OP)                                       \
  LINKED_QUEUE_TYPE(int) *: LINKED_QUEUE_FUNC(int, OP),                       \
  LINKED_QUEUE_TYPE(float) *: LINKED_QUEUE_FUNC(float, OP),                   \
  LINKED_QUEUE_TYPE(double) *: LINKED_QUEUE_FUNC(double, OP)

#define CONTAINER_LINKED_STACK_ASSOC(OP)                                       \
  LINKED_STACK_TYPE(int) *: LINKED_STACK_FUNC(int, OP),                       \
  LINKED_STACK_TYPE(float) *: LINKED_STACK_FUNC(float, OP),                   \
  LINKED_STACK_TYPE(double) *: LINKED_STACK_FUNC(double, OP)

/*
 * The controlling expression of _Generic is not evaluated. Therefore the
 * self expression below is evaluated exactly once: in the actual call.
 */
#define Push(self, value)                                                      \
  _Generic(&(self), CONTAINER_FLAT_QUEUE_ASSOC(Push),                         \
                    CONTAINER_FLAT_STACK_ASSOC(Push),                         \
                    CONTAINER_LINKED_QUEUE_ASSOC(Push),                       \
                    CONTAINER_LINKED_STACK_ASSOC(Push))(&(self), (value))

#define PushBack(self, value)                                                  \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(PushBack),                         \
                    CONTAINER_STRING_ASSOC(PushBack),                         \
                    CONTAINER_LIST_ASSOC(PushBack),                           \
                    CONTAINER_DEQUE_ASSOC(PushBack))(&(self), (value))

#define PushFront(self, value)                                                 \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(PushFront),                        \
                    CONTAINER_LIST_ASSOC(PushFront),                          \
                    CONTAINER_FORWARD_ASSOC(PushFront),                       \
                    CONTAINER_DEQUE_ASSOC(PushFront))(&(self), (value))

#define CONTAINER_POP_1(self) CONTAINER_POP_2(self, NULL)
#define CONTAINER_POP_2(self, out)                                             \
  _Generic(&(self), CONTAINER_FLAT_QUEUE_ASSOC(Pop),                          \
                    CONTAINER_FLAT_STACK_ASSOC(Pop),                          \
                    CONTAINER_LINKED_QUEUE_ASSOC(Pop),                        \
                    CONTAINER_LINKED_STACK_ASSOC(Pop))(&(self), (out))
#define Pop(...)                                                               \
  CONTAINER_ALIAS_SELECT_2(__VA_ARGS__, CONTAINER_POP_2, CONTAINER_POP_1,     \
                           CONTAINER_ALIAS_UNUSED)(__VA_ARGS__)

#define CONTAINER_POP_BACK_1(self) CONTAINER_POP_BACK_2(self, NULL)
#define CONTAINER_POP_BACK_2(self, out)                                        \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(PopBack),                          \
                    CONTAINER_STRING_ASSOC(PopBack),                          \
                    CONTAINER_LIST_ASSOC(PopBack),                            \
                    CONTAINER_DEQUE_ASSOC(PopBack))(&(self), (out))
#define PopBack(...)                                                           \
  CONTAINER_ALIAS_SELECT_2(__VA_ARGS__, CONTAINER_POP_BACK_2,                 \
                           CONTAINER_POP_BACK_1,                               \
                           CONTAINER_ALIAS_UNUSED)(__VA_ARGS__)

#define CONTAINER_POP_FRONT_1(self) CONTAINER_POP_FRONT_2(self, NULL)
#define CONTAINER_POP_FRONT_2(self, out)                                       \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(PopFront),                         \
                    CONTAINER_LIST_ASSOC(PopFront),                           \
                    CONTAINER_FORWARD_ASSOC(PopFront),                        \
                    CONTAINER_DEQUE_ASSOC(PopFront))(&(self), (out))
#define PopFront(...)                                                          \
  CONTAINER_ALIAS_SELECT_2(__VA_ARGS__, CONTAINER_POP_FRONT_2,                \
                           CONTAINER_POP_FRONT_1,                              \
                           CONTAINER_ALIAS_UNUSED)(__VA_ARGS__)

#define At(self, index)                                                        \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(At), CONTAINER_STRING_ASSOC(At))   \
      (&(self), (index))

#define Front(self)                                                            \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Front),                            \
                    CONTAINER_FLAT_QUEUE_ASSOC(Front),                        \
                    CONTAINER_LIST_ASSOC(Front),                              \
                    CONTAINER_FORWARD_ASSOC(Front),                           \
                    CONTAINER_DEQUE_ASSOC(Front))(&(self))

#define Back(self)                                                             \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Back),                             \
                    CONTAINER_FLAT_QUEUE_ASSOC(Back),                         \
                    CONTAINER_LIST_ASSOC(Back),                               \
                    CONTAINER_FORWARD_ASSOC(Back),                            \
                    CONTAINER_DEQUE_ASSOC(Back))(&(self))

#define Top(self)                                                              \
  _Generic(&(self), CONTAINER_FLAT_STACK_ASSOC(Top),                          \
                    CONTAINER_LINKED_STACK_ASSOC(Top))(&(self))

#define Reserve(self, count)                                                   \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Reserve),                          \
                    CONTAINER_FLAT_QUEUE_ASSOC(Reserve),                      \
                    CONTAINER_FLAT_STACK_ASSOC(Reserve),                      \
                    CONTAINER_STRING_ASSOC(Reserve))(&(self), (count))

#define Capacity(self)                                                         \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Capacity),                         \
                    CONTAINER_FLAT_QUEUE_ASSOC(Capacity),                     \
                    CONTAINER_FLAT_STACK_ASSOC(Capacity),                     \
                    CONTAINER_STRING_ASSOC(Capacity))(&(self))

#define Data(self)                                                             \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Data), CONTAINER_STRING_ASSOC(Data))\
      (&(self))

#define CONTAINER_INSERT_3(self, position, value)                              \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Insert))(&(self), (position),      \
                                                     (value))
#define CONTAINER_INSERT_4(self, position, value, outNode)                     \
  _Generic(&(self), CONTAINER_LIST_ASSOC(Insert))(&(self), (position),        \
                                                   (value), (outNode))
#define Insert(...)                                                            \
  CONTAINER_ALIAS_SELECT_4(__VA_ARGS__, CONTAINER_INSERT_4,                   \
                           CONTAINER_INSERT_3, CONTAINER_ALIAS_UNUSED)         \
      (__VA_ARGS__)

#define Erase(self, position, countOrOut)                                      \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Erase),                            \
                    CONTAINER_STRING_ASSOC(Erase),                            \
                    CONTAINER_LIST_ASSOC(Erase))(&(self), (position),         \
                                                  (countOrOut))

#define Clear(self)                                                            \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Clear),                            \
                    CONTAINER_STRING_ASSOC(Clear),                            \
                    CONTAINER_LIST_ASSOC(Clear),                              \
                    CONTAINER_FORWARD_ASSOC(Clear),                           \
                    CONTAINER_DEQUE_ASSOC(Clear),                             \
                    CONTAINER_LINKED_QUEUE_ASSOC(Clear),                      \
                    CONTAINER_LINKED_STACK_ASSOC(Clear))(&(self))

#define Empty(self)                                                            \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Empty),                            \
                    CONTAINER_FLAT_QUEUE_ASSOC(Empty),                        \
                    CONTAINER_FLAT_STACK_ASSOC(Empty),                        \
                    CONTAINER_STRING_ASSOC(Empty),                            \
                    CONTAINER_LIST_ASSOC(Empty),                              \
                    CONTAINER_FORWARD_ASSOC(Empty),                           \
                    CONTAINER_DEQUE_ASSOC(Empty),                             \
                    CONTAINER_LINKED_QUEUE_ASSOC(Empty),                      \
                    CONTAINER_LINKED_STACK_ASSOC(Empty))(&(self))

#define Size(self)                                                             \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Size),                             \
                    CONTAINER_FLAT_QUEUE_ASSOC(Size),                         \
                    CONTAINER_FLAT_STACK_ASSOC(Size),                         \
                    CONTAINER_LIST_ASSOC(Size),                               \
                    CONTAINER_FORWARD_ASSOC(Size),                            \
                    CONTAINER_DEQUE_ASSOC(Size),                              \
                    CONTAINER_LINKED_QUEUE_ASSOC(Size),                       \
                    CONTAINER_LINKED_STACK_ASSOC(Size))(&(self))

#define Destroy(self)                                                          \
  _Generic(&(self), CONTAINER_VECTOR_ASSOC(Destroy),                          \
                    CONTAINER_FLAT_QUEUE_ASSOC(Destroy),                      \
                    CONTAINER_FLAT_STACK_ASSOC(Destroy),                      \
                    CONTAINER_STRING_ASSOC(Destroy),                          \
                    CONTAINER_LIST_ASSOC(Destroy),                            \
                    CONTAINER_FORWARD_ASSOC(Destroy),                         \
                    CONTAINER_DEQUE_ASSOC(Destroy),                           \
                    CONTAINER_LINKED_QUEUE_ASSOC(Destroy),                    \
                    CONTAINER_LINKED_STACK_ASSOC(Destroy))(&(self))

#endif
