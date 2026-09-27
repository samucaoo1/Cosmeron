#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define LINKED_QUEUE_FUNCTION_TABLE(TYPE, SUFFIX)                           \
  typedef struct LINKED_QUEUE_FUNC(SUFFIX, FunctionTable) {               \
    /* Lifecycle */                                                            \
    OPSTATUS (*init)(LINKED_QUEUE_TYPE(SUFFIX) *);                            \
    void (*destroy)(LINKED_QUEUE_TYPE(SUFFIX) *);                             \
    /* Modifiers / Container aliases */                                        \
    OPSTATUS (*push)(LINKED_QUEUE_TYPE(SUFFIX) *, TYPE);                      \
    OPSTATUS (*pop)(LINKED_QUEUE_TYPE(SUFFIX) *, TYPE *);                     \
    void (*clear)(LINKED_QUEUE_TYPE(SUFFIX) *);                               \
    /* Element access */                                                       \
    TYPE *(*front)(LINKED_QUEUE_TYPE(SUFFIX) *);                              \
    TYPE *(*back)(LINKED_QUEUE_TYPE(SUFFIX) *);                               \
    /* Observers */                                                            \
    bool (*empty)(const LINKED_QUEUE_TYPE(SUFFIX) *);                               \
    size_t (*size)(const LINKED_QUEUE_TYPE(SUFFIX) *);                              \
  } LINKED_QUEUE_FUNC(SUFFIX, FunctionTable);

#define LINKED_QUEUE_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)                  \
  static const LINKED_QUEUE_FUNC(SUFFIX, FunctionTable)                           \
      LINKED_QUEUE_FUNC(SUFFIX, functions) = {                                \
          /* Lifecycle */                                                       \
          .init = LINKED_QUEUE_FUNC(SUFFIX, Init),                    \
          .destroy = LINKED_QUEUE_FUNC(SUFFIX, Destroy),              \
          /* Modifiers / Container aliases */                                   \
          .push = LINKED_QUEUE_FUNC(SUFFIX, Push),                    \
          .pop = LINKED_QUEUE_FUNC(SUFFIX, Pop),                      \
          .clear = LINKED_QUEUE_FUNC(SUFFIX, Clear),                  \
          /* Element access */                                                  \
          .front = LINKED_QUEUE_FUNC(SUFFIX, Front),                  \
          .back = LINKED_QUEUE_FUNC(SUFFIX, Back),                    \
          /* Observers */                                                       \
          .empty = LINKED_QUEUE_FUNC(SUFFIX, Empty),                  \
          .size = LINKED_QUEUE_FUNC(SUFFIX, Size)};

#else
#define LINKED_QUEUE_FUNCTION_TABLE(...)
#define LINKED_QUEUE_FUNCTION_TABLE_INSTANCE(...)
#endif
