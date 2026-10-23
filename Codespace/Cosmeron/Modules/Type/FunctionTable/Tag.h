#pragma once

#ifndef TYPE_DISABLE_FUNCTION_TABLE

#define TAG_FUNCTION_TABLE_STRUCT(SUFFIX, UINT_TYPE)                           \
  typedef struct TAG_FUNCTION_TABLE_TYPE(SUFFIX) {                             \
    OPSTATUS (*init)(TAG_TYPE(SUFFIX) *, const char *);                        \
    OPSTATUS (*fromUInt)(TAG_TYPE(SUFFIX) *, UINT_TYPE);                       \
    OPSTATUS (*fromBytes)(TAG_TYPE(SUFFIX) *, const void *, size_t);           \
    OPSTATUS (*readBE)(TAG_TYPE(SUFFIX) *, const void *);                      \
    OPSTATUS (*readLE)(TAG_TYPE(SUFFIX) *, const void *);                      \
    OPSTATUS (*writeBE)(const TAG_TYPE(SUFFIX) *, void *);                     \
    OPSTATUS (*writeLE)(const TAG_TYPE(SUFFIX) *, void *);                     \
    OPSTATUS (*toString)(const TAG_TYPE(SUFFIX) *, char *, size_t);            \
    const char *(*text)(const TAG_TYPE(SUFFIX) *);                             \
    UINT_TYPE (*toUInt)(const TAG_TYPE(SUFFIX) *);                             \
    bool (*isPrintable)(const TAG_TYPE(SUFFIX) *);                             \
    bool (*equal)(const TAG_TYPE(SUFFIX) *, const TAG_TYPE(SUFFIX) *);        \
    OPSTATUS (*compare)(const TAG_TYPE(SUFFIX) *, const TAG_TYPE(SUFFIX) *,    \
                        CMPOUT *);                                             \
  } TAG_FUNCTION_TABLE_TYPE(SUFFIX);

#define TAG_FUNCTION_TABLE_INSTANCE(SUFFIX, UINT_TYPE)                         \
  static const TAG_FUNCTION_TABLE_TYPE(SUFFIX) TAG_FUNC(SUFFIX, functions) = { \
      .init = &TAG_FUNC(SUFFIX, Init),                                         \
      .fromUInt = &TAG_FUNC(SUFFIX, FromUInt),                                 \
      .fromBytes = &TAG_FUNC(SUFFIX, FromBytes),                               \
      .readBE = &TAG_FUNC(SUFFIX, ReadBE),                                     \
      .readLE = &TAG_FUNC(SUFFIX, ReadLE),                                     \
      .writeBE = &TAG_FUNC(SUFFIX, WriteBE),                                   \
      .writeLE = &TAG_FUNC(SUFFIX, WriteLE),                                   \
      .toString = &TAG_FUNC(SUFFIX, ToString),                                 \
      .text = &TAG_FUNC(SUFFIX, Text),                                         \
      .toUInt = &TAG_FUNC(SUFFIX, ToUInt),                                     \
      .isPrintable = &TAG_FUNC(SUFFIX, IsPrintable),                           \
      .equal = &TAG_FUNC(SUFFIX, Equal),                                       \
      .compare = &TAG_FUNC(SUFFIX, Compare)};

#else
#define TAG_FUNCTION_TABLE_STRUCT(SUFFIX, UINT_TYPE)
#define TAG_FUNCTION_TABLE_INSTANCE(SUFFIX, UINT_TYPE)
#endif
