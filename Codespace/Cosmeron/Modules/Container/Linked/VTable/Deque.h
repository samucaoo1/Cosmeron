#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define LINKED_DEQUE_FUNCTION_TABLE(TYPE, SUFFIX)                                  \
  typedef struct LINKED_DEQUE_FUNC(SUFFIX, FunctionTable) {                       \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(LINKED_DEQUE_TYPE(SUFFIX) *);                                   \
    void (*destroy)(LINKED_DEQUE_TYPE(SUFFIX) *);                                    \
    /* Modifiers / Container aliases */                                        \
    OPSTATUS (*pushFront)(LINKED_DEQUE_TYPE(SUFFIX) *, TYPE);                       \
    OPSTATUS (*pushBack)(LINKED_DEQUE_TYPE(SUFFIX) *, TYPE);                        \
    OPSTATUS (*popFront)(LINKED_DEQUE_TYPE(SUFFIX) *, TYPE *);                      \
    OPSTATUS (*popBack)(LINKED_DEQUE_TYPE(SUFFIX) *, TYPE *);                       \
    void (*clear)(LINKED_DEQUE_TYPE(SUFFIX) *);                                      \
    /* Element access */                                                       \
    TYPE *(*front)(LINKED_DEQUE_TYPE(SUFFIX) *);                                     \
    TYPE *(*back)(LINKED_DEQUE_TYPE(SUFFIX) *);                                      \
    /* Observers */                                                            \
    bool (*empty)(const LINKED_DEQUE_TYPE(SUFFIX) *);                                      \
    size_t (*size)(const LINKED_DEQUE_TYPE(SUFFIX) *);                                     \
  } LINKED_DEQUE_FUNC(SUFFIX, FunctionTable);

#define LINKED_DEQUE_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                         \
  static const LINKED_DEQUE_FUNC(SUFFIX, FunctionTable)                                  \
      LINKED_DEQUE_FUNC(SUFFIX, functions) = {                                       \
          /* Lifecycle */                                                       \
          .init = LINKED_DEQUE_FUNC(SUFFIX, Init),                           \
          .destroy = LINKED_DEQUE_FUNC(SUFFIX, Destroy),                     \
          /* Modifiers / Container aliases */                                   \
          .pushFront = LINKED_DEQUE_FUNC(SUFFIX, PushFront),               \
          .pushBack = LINKED_DEQUE_FUNC(SUFFIX, PushBack),                 \
          .popFront = LINKED_DEQUE_FUNC(SUFFIX, PopFront),                 \
          .popBack = LINKED_DEQUE_FUNC(SUFFIX, PopBack),                   \
          .clear = LINKED_DEQUE_FUNC(SUFFIX, Clear),                         \
          /* Element access */                                                  \
          .front = LINKED_DEQUE_FUNC(SUFFIX, Front),                         \
          .back = LINKED_DEQUE_FUNC(SUFFIX, Back),                           \
          /* Observers */                                                       \
          .empty = LINKED_DEQUE_FUNC(SUFFIX, Empty),                         \
          .size = LINKED_DEQUE_FUNC(SUFFIX, Size)};

#else
#define LINKED_DEQUE_FUNCTION_TABLE(...)
#define LINKED_DEQUE_FUNCTION_TABLE_INSTANCE(...)
#endif
