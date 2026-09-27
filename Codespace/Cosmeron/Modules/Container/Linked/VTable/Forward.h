#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define LINKED_FORWARD_LIST_FUNCTION_TABLE(TYPE, SUFFIX)                           \
  typedef struct LINKED_FORWARD_LIST_FUNC(SUFFIX, FunctionTable) {               \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(LINKED_FORWARD_LIST_TYPE(SUFFIX) *);                            \
    void (*destroy)(LINKED_FORWARD_LIST_TYPE(SUFFIX) *);                             \
    /* Modifiers / Container aliases */                                        \
    OPSTATUS (*pushFront)(LINKED_FORWARD_LIST_TYPE(SUFFIX) *, TYPE);                \
    OPSTATUS (*popFront)(LINKED_FORWARD_LIST_TYPE(SUFFIX) *, TYPE *);               \
    OPSTATUS (*insertAfter)(LINKED_FORWARD_LIST_TYPE(SUFFIX) *,                     \
                             LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) *, TYPE,          \
                             LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) **);              \
    OPSTATUS (*eraseAfter)(LINKED_FORWARD_LIST_TYPE(SUFFIX) *,                      \
                            LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) *);                \
    void (*clear)(LINKED_FORWARD_LIST_TYPE(SUFFIX) *);                               \
    /* Element access */                                                       \
    TYPE *(*front)(LINKED_FORWARD_LIST_TYPE(SUFFIX) *);                              \
    LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) *(*begin)(LINKED_FORWARD_LIST_TYPE(SUFFIX) *);   \
    /* Observers */                                                            \
    bool (*empty)(const LINKED_FORWARD_LIST_TYPE(SUFFIX) *);                               \
    size_t (*size)(const LINKED_FORWARD_LIST_TYPE(SUFFIX) *);                              \
  } LINKED_FORWARD_LIST_FUNC(SUFFIX, FunctionTable);

#define LINKED_FORWARD_LIST_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                  \
  static const LINKED_FORWARD_LIST_FUNC(SUFFIX, FunctionTable)                           \
      LINKED_FORWARD_LIST_FUNC(SUFFIX, functions) = {                                \
          /* Lifecycle */                                                       \
          .init = LINKED_FORWARD_LIST_FUNC(SUFFIX, Init),                    \
          .destroy = LINKED_FORWARD_LIST_FUNC(SUFFIX, Destroy),              \
          /* Modifiers / Container aliases */                                   \
          .pushFront = LINKED_FORWARD_LIST_FUNC(SUFFIX, PushFront),        \
          .popFront = LINKED_FORWARD_LIST_FUNC(SUFFIX, PopFront),          \
          .insertAfter = LINKED_FORWARD_LIST_FUNC(SUFFIX, InsertAfter),    \
          .eraseAfter = LINKED_FORWARD_LIST_FUNC(SUFFIX, EraseAfter),      \
          .clear = LINKED_FORWARD_LIST_FUNC(SUFFIX, Clear),                  \
          /* Element access */                                                  \
          .front = LINKED_FORWARD_LIST_FUNC(SUFFIX, Front),                  \
          .begin = LINKED_FORWARD_LIST_FUNC(SUFFIX, Begin),                  \
          /* Observers */                                                       \
          .empty = LINKED_FORWARD_LIST_FUNC(SUFFIX, Empty),                  \
          .size = LINKED_FORWARD_LIST_FUNC(SUFFIX, Size)};

#else
#define LINKED_FORWARD_LIST_FUNCTION_TABLE(...)
#define LINKED_FORWARD_LIST_FUNCTION_TABLE_INSTANCE(...)
#endif
