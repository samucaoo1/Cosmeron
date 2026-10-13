#pragma once

#if COSMERON_MACRO_INTERNAL_TREE_FUNCTION_TABLE_ENABLED

/* =============================================================
 * FunctionTable definitions and instances (Section 4)
 * ============================================================= */

#define TREE_FUNCTION_TABLE_STRUCT_SET(T, KEY_TYPE)                           \
  typedef struct TREE_FUNCTION_TABLE_TYPE(T) {                                 \
    OPSTATUS (*init)(T *);                                                     \
    void (*destroy)(T *);                                                      \
    void (*clear)(T *);                                                        \
    OPSTATUS (*insert)(T *, KEY_TYPE);                                         \
    OPSTATUS (*remove)(T *, KEY_TYPE);                                         \
    TREE_NODE(T) * (*findNode)(T *, KEY_TYPE);                                 \
    OPSTATUS (*find)(T *, KEY_TYPE, KEY_TYPE **);                              \
    bool (*contains)(const T *, KEY_TYPE);                                     \
    TREE_NODE(T) * (*minNode)(TREE_NODE(T) *);                                 \
    TREE_NODE(T) * (*maxNode)(TREE_NODE(T) *);                                 \
    OPSTATUS (*min)(T *, KEY_TYPE **);                                         \
    OPSTATUS (*max)(T *, KEY_TYPE **);                                         \
    bool (*empty)(const T *);                                                  \
    size_t (*size)(const T *);                                                 \
    TREE_NODE(T) * (*begin)(T *);                                              \
    TREE_NODE(T) * (*end)(T *);                                                \
    TREE_NODE(T) * (*next)(TREE_NODE(T) *);                                    \
    TREE_NODE(T) * (*prev)(TREE_NODE(T) *);                                    \
    const TREE_NODE(T) *(*constBegin)(const T *);                              \
    const TREE_NODE(T) *(*constEnd)(const T *);                                \
    const TREE_NODE(T) *(*constNext)(const TREE_NODE(T) *);                     \
    const TREE_NODE(T) *(*constPrev)(const TREE_NODE(T) *);                     \
  } TREE_FUNCTION_TABLE_TYPE(T);

#define TREE_FUNCTION_TABLE_STRUCT_MAP(T, KEY_TYPE, VALUE_TYPE)                 \
  typedef struct TREE_FUNCTION_TABLE_TYPE(T) {                                 \
    OPSTATUS (*init)(T *);                                                     \
    void (*destroy)(T *);                                                      \
    void (*clear)(T *);                                                        \
    OPSTATUS (*insert)(T *, KEY_TYPE, VALUE_TYPE);                             \
    OPSTATUS (*remove)(T *, KEY_TYPE);                                         \
    TREE_NODE(T) * (*findNode)(T *, KEY_TYPE);                                 \
    OPSTATUS (*find)(T *, KEY_TYPE, VALUE_TYPE **);                            \
    bool (*contains)(const T *, KEY_TYPE);                                     \
    TREE_NODE(T) * (*minNode)(TREE_NODE(T) *);                                 \
    TREE_NODE(T) * (*maxNode)(TREE_NODE(T) *);                                 \
    OPSTATUS (*min)(T *, KEY_TYPE **);                                         \
    OPSTATUS (*max)(T *, KEY_TYPE **);                                         \
    bool (*empty)(const T *);                                                  \
    size_t (*size)(const T *);                                                 \
    TREE_NODE(T) * (*begin)(T *);                                              \
    TREE_NODE(T) * (*end)(T *);                                                \
    TREE_NODE(T) * (*next)(TREE_NODE(T) *);                                    \
    TREE_NODE(T) * (*prev)(TREE_NODE(T) *);                                    \
    const TREE_NODE(T) *(*constBegin)(const T *);                              \
    const TREE_NODE(T) *(*constEnd)(const T *);                                \
    const TREE_NODE(T) *(*constNext)(const TREE_NODE(T) *);                     \
    const TREE_NODE(T) *(*constPrev)(const TREE_NODE(T) *);                     \
  } TREE_FUNCTION_TABLE_TYPE(T);

#define TREE_FUNCTION_TABLE_INSTANCE_SET(T)                                    \
  static const TREE_FUNCTION_TABLE_TYPE(T) TREE_FUNC(T, functions) = {        \
      .init = TREE_FUNC(T, Init),                                              \
      .destroy = TREE_FUNC(T, Destroy),                                        \
      .clear = TREE_FUNC(T, Clear),                                            \
      .insert = TREE_FUNC(T, Insert),                                          \
      .remove = TREE_FUNC(T, Remove),                                          \
      .findNode = TREE_FUNC(T, FindNode),                                      \
      .find = TREE_FUNC(T, Find),                                              \
      .contains = TREE_FUNC(T, Contains),                                      \
      .minNode = TREE_FUNC(T, MinNode),                                        \
      .maxNode = TREE_FUNC(T, MaxNode),                                        \
      .min = TREE_FUNC(T, Min),                                                \
      .max = TREE_FUNC(T, Max),                                                \
      .empty = TREE_FUNC(T, Empty),                                            \
      .size = TREE_FUNC(T, Size),                                              \
      .begin = TREE_FUNC(T, Begin),                                            \
      .end = TREE_FUNC(T, End),                                                \
      .next = TREE_FUNC(T, Next),                                              \
      .prev = TREE_FUNC(T, Prev),                                              \
      .constBegin = TREE_FUNC(T, ConstBegin),                                  \
      .constEnd = TREE_FUNC(T, ConstEnd),                                      \
      .constNext = TREE_FUNC(T, ConstNext),                                    \
      .constPrev = TREE_FUNC(T, ConstPrev),                                    \
  };

#define TREE_FUNCTION_TABLE_INSTANCE_MAP(T)                                    \
  static const TREE_FUNCTION_TABLE_TYPE(T) TREE_FUNC(T, functions) = {        \
      .init = TREE_FUNC(T, Init),                                              \
      .destroy = TREE_FUNC(T, Destroy),                                        \
      .clear = TREE_FUNC(T, Clear),                                            \
      .insert = TREE_FUNC(T, Insert),                                          \
      .remove = TREE_FUNC(T, Remove),                                          \
      .findNode = TREE_FUNC(T, FindNode),                                      \
      .find = TREE_FUNC(T, Find),                                              \
      .contains = TREE_FUNC(T, Contains),                                      \
      .minNode = TREE_FUNC(T, MinNode),                                        \
      .maxNode = TREE_FUNC(T, MaxNode),                                        \
      .min = TREE_FUNC(T, Min),                                                \
      .max = TREE_FUNC(T, Max),                                                \
      .empty = TREE_FUNC(T, Empty),                                            \
      .size = TREE_FUNC(T, Size),                                              \
      .begin = TREE_FUNC(T, Begin),                                            \
      .end = TREE_FUNC(T, End),                                                \
      .next = TREE_FUNC(T, Next),                                              \
      .prev = TREE_FUNC(T, Prev),                                              \
      .constBegin = TREE_FUNC(T, ConstBegin),                                  \
      .constEnd = TREE_FUNC(T, ConstEnd),                                      \
      .constNext = TREE_FUNC(T, ConstNext),                                    \
      .constPrev = TREE_FUNC(T, ConstPrev),                                    \
  };

#else

#define TREE_FUNCTION_TABLE_STRUCT_SET(...)
#define TREE_FUNCTION_TABLE_STRUCT_MAP(...)
#define TREE_FUNCTION_TABLE_INSTANCE_SET(...)
#define TREE_FUNCTION_TABLE_INSTANCE_MAP(...)

#endif

/* Compatibility aliases */
#define TTREE_FUNCTION_TABLE_STRUCT_SET TREE_FUNCTION_TABLE_STRUCT_SET
#define TTREE_FUNCTION_TABLE_STRUCT_MAP TREE_FUNCTION_TABLE_STRUCT_MAP
#define TTREE_FUNCTION_TABLE_INSTANCE_SET TREE_FUNCTION_TABLE_INSTANCE_SET
#define TTREE_FUNCTION_TABLE_INSTANCE_MAP TREE_FUNCTION_TABLE_INSTANCE_MAP
