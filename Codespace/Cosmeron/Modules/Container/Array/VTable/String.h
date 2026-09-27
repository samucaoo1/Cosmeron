#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define TSTRING_FUNCTION_TABLE(CHAR_TYPE, SUFFIX)                            \
  struct TSTRING_FUNC(SUFFIX, FunctionTable) {                              \
    OPSTATUS (*init)(TSTRING_TYPE(SUFFIX) * str);                              \
    void (*destroy)(TSTRING_TYPE(SUFFIX) * str);                               \
    OPSTATUS (*fromCStr)(TSTRING_TYPE(SUFFIX) * str, const CHAR_TYPE *cstr);  \
    const CHAR_TYPE *(*cStr)(TSTRING_TYPE(SUFFIX) * str);                     \
    size_t (*length)(const TSTRING_TYPE(SUFFIX) *str);                              \
    size_t (*size)(const TSTRING_TYPE(SUFFIX) *str);                                \
    bool (*empty)(const TSTRING_TYPE(SUFFIX) *str);                                 \
    void (*clear)(TSTRING_TYPE(SUFFIX) * str);                                 \
    OPSTATUS (*reserve)(TSTRING_TYPE(SUFFIX) * str, size_t newCapacity);      \
    size_t (*capacity)(const TSTRING_TYPE(SUFFIX) *str);                             \
    OPSTATUS (*pushBack)(TSTRING_TYPE(SUFFIX) * str, CHAR_TYPE ch);           \
    OPSTATUS (*popBack)(TSTRING_TYPE(SUFFIX) * str, CHAR_TYPE *outValue);                          \
    CHAR_TYPE *(*at)(TSTRING_TYPE(SUFFIX) * str, size_t index);                \
    CHAR_TYPE *(*front)(TSTRING_TYPE(SUFFIX) * str);                           \
    CHAR_TYPE *(*back)(TSTRING_TYPE(SUFFIX) * str);                            \
    OPSTATUS (*append)(TSTRING_TYPE(SUFFIX) * str, const CHAR_TYPE *cstr);     \
    OPSTATUS (*appendStr)(TSTRING_TYPE(SUFFIX) * str,                         \
                           TSTRING_TYPE(SUFFIX) * other);                      \
    OPSTATUS (*insert)(TSTRING_TYPE(SUFFIX) * str, size_t pos,                \
                       CHAR_TYPE ch);                                          \
    OPSTATUS (*insertStr)(TSTRING_TYPE(SUFFIX) * str, size_t pos,             \
                           const CHAR_TYPE *cstr);                             \
    OPSTATUS (*erase)(TSTRING_TYPE(SUFFIX) * str, size_t pos, size_t count);   \
    OPSTATUS (*substr)(TSTRING_TYPE(SUFFIX) * str, size_t pos, size_t count,   \
                       TSTRING_TYPE(SUFFIX) * out);                            \
    OPSTATUS (*compare)(const TSTRING_TYPE(SUFFIX) *str,                      \
                        const TSTRING_TYPE(SUFFIX) *other,                     \
                        TComparisonResult *result);                            \
    OPSTATUS (*find)(const TSTRING_TYPE(SUFFIX) *str, CHAR_TYPE ch,            \
                     size_t start, size_t *index);                             \
    OPSTATUS (*findStr)(const TSTRING_TYPE(SUFFIX) *str,                       \
                        const CHAR_TYPE *needle, size_t start, size_t *index);                                          \
    CHAR_TYPE *(*data)(TSTRING_TYPE(SUFFIX) * str);                            \
  } TSTRING_FUNC(SUFFIX, FunctionTable);

#define TSTRING_FUNCTION_TABLE_INSTANCE(CHAR_TYPE, SUFFIX)                   \
  static const struct TSTRING_FUNC(SUFFIX, FunctionTable)                   \
      TSTRING_FUNC(SUFFIX, functions) = {                                      \
          .init = TSTRING_FUNC(SUFFIX, Init),                                  \
          .destroy = TSTRING_FUNC(SUFFIX, Destroy),                            \
          .fromCStr = TSTRING_FUNC(SUFFIX, FromCStr),                         \
          .cStr = TSTRING_FUNC(SUFFIX, CStr),                                 \
          .length = TSTRING_FUNC(SUFFIX, Length),                              \
          .size = TSTRING_FUNC(SUFFIX, Length),                                \
          .empty = TSTRING_FUNC(SUFFIX, Empty),                                \
          .clear = TSTRING_FUNC(SUFFIX, Clear),                                \
          .reserve = TSTRING_FUNC(SUFFIX, Reserve),                            \
          .capacity = TSTRING_FUNC(SUFFIX, Capacity),                          \
          .pushBack = TSTRING_FUNC(SUFFIX, PushBack),                         \
          .popBack = TSTRING_FUNC(SUFFIX, PopBack),                           \
          .at = TSTRING_FUNC(SUFFIX, At),                                      \
          .front = TSTRING_FUNC(SUFFIX, Front),                                \
          .back = TSTRING_FUNC(SUFFIX, Back),                                  \
          .append = TSTRING_FUNC(SUFFIX, Append),                              \
          .appendStr = TSTRING_FUNC(SUFFIX, AppendStr),                       \
          .insert = TSTRING_FUNC(SUFFIX, InsertChar),                          \
          .insertStr = TSTRING_FUNC(SUFFIX, Insert),                          \
          .erase = TSTRING_FUNC(SUFFIX, Erase),                                \
          .substr = TSTRING_FUNC(SUFFIX, Substr),                              \
          .compare = TSTRING_FUNC(SUFFIX, Compare),                            \
          .find = TSTRING_FUNC(SUFFIX, Find),                                  \
          .findStr = TSTRING_FUNC(SUFFIX, FindStr),                           \
          .data = TSTRING_FUNC(SUFFIX, Data),                                  \
  };

#else
#define TSTRING_FUNCTION_TABLE(...)
#define TSTRING_FUNCTION_TABLE_INSTANCE(...)
#endif
