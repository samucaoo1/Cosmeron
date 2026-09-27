#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define LINKED_LIST_FUNCTION_TABLE(TYPE, SUFFIX)                                   \
  typedef struct LINKED_LIST_FUNC(SUFFIX, FunctionTable) {                   \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(LINKED_LIST_TYPE(SUFFIX) *);                                    \
    void (*destroy)(LINKED_LIST_TYPE(SUFFIX) *);                                     \
    /* Modifiers / Container aliases */                                        \
    OPSTATUS (*pushFront)(LINKED_LIST_TYPE(SUFFIX) *, TYPE);                        \
    OPSTATUS (*pushBack)(LINKED_LIST_TYPE(SUFFIX) *, TYPE);                         \
    OPSTATUS (*popFront)(LINKED_LIST_TYPE(SUFFIX) *, TYPE *);                       \
    OPSTATUS (*popBack)(LINKED_LIST_TYPE(SUFFIX) *, TYPE *);                        \
    OPSTATUS (*insert)(LINKED_LIST_TYPE(SUFFIX) *,                                   \
                       LINKED_LIST_NODE_TYPE(SUFFIX) *, TYPE,                        \
                       LINKED_LIST_NODE_TYPE(SUFFIX) **);                    \
    OPSTATUS (*erase)(LINKED_LIST_TYPE(SUFFIX) *,                                    \
                      LINKED_LIST_NODE_TYPE(SUFFIX) *,                       \
                      LINKED_LIST_NODE_TYPE(SUFFIX) **);                     \
    void (*clear)(LINKED_LIST_TYPE(SUFFIX) *);                                       \
    /* Element access */                                                       \
    TYPE *(*front)(LINKED_LIST_TYPE(SUFFIX) *);                                      \
    TYPE *(*back)(LINKED_LIST_TYPE(SUFFIX) *);                                       \
    LINKED_LIST_NODE_TYPE(SUFFIX) *(*begin)(LINKED_LIST_TYPE(SUFFIX) *);           \
    LINKED_LIST_NODE_TYPE(SUFFIX) *(*end)(LINKED_LIST_TYPE(SUFFIX) *);             \
    LINKED_LIST_NODE_TYPE(SUFFIX) *(*rBegin)(LINKED_LIST_TYPE(SUFFIX) *);          \
    /* Observers */                                                            \
    bool (*empty)(const LINKED_LIST_TYPE(SUFFIX) *);                                       \
    size_t (*size)(const LINKED_LIST_TYPE(SUFFIX) *);                                      \
  } LINKED_LIST_FUNC(SUFFIX, FunctionTable);

#define LINKED_LIST_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                          \
  static const LINKED_LIST_FUNC(SUFFIX, FunctionTable)                                \
      LINKED_LIST_FUNC(SUFFIX, functions) = {                                      \
          /* Lifecycle */                                                           \
          .init = LINKED_LIST_FUNC(SUFFIX, Init),                                  \
          .destroy = LINKED_LIST_FUNC(SUFFIX, Destroy),                            \
          /* Modifiers / Container aliases */                                       \
          .pushFront = LINKED_LIST_FUNC(SUFFIX, PushFront),                       \
          .pushBack = LINKED_LIST_FUNC(SUFFIX, PushBack),                         \
          .popFront = LINKED_LIST_FUNC(SUFFIX, PopFront),                         \
          .popBack = LINKED_LIST_FUNC(SUFFIX, PopBack),                           \
          .insert = LINKED_LIST_FUNC(SUFFIX, Insert),                              \
          .erase = LINKED_LIST_FUNC(SUFFIX, Erase),                                \
          .clear = LINKED_LIST_FUNC(SUFFIX, Clear),                                \
          /* Element access */                                                      \
          .front = LINKED_LIST_FUNC(SUFFIX, Front),                                \
          .back = LINKED_LIST_FUNC(SUFFIX, Back),                                  \
          .begin = LINKED_LIST_FUNC(SUFFIX, Begin),                                \
          .end = LINKED_LIST_FUNC(SUFFIX, End),                                    \
          .rBegin = LINKED_LIST_FUNC(SUFFIX, RBegin),                              \
          /* Observers */                                                           \
          .empty = LINKED_LIST_FUNC(SUFFIX, Empty),                                \
          .size = LINKED_LIST_FUNC(SUFFIX, Size)};

#else
#define LINKED_LIST_FUNCTION_TABLE(...)
#define LINKED_LIST_FUNCTION_TABLE_INSTANCE(...)
#endif
