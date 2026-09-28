#pragma once

#define TBIGINT_FUNCTION_TABLE_STRUCT(SUFFIX)                                        \
  typedef struct TBIGINT_FUNCTION_TABLE_TYPE(SUFFIX) {                      \
    OPSTATUS (*clear)(TBIGINT_TYPE(SUFFIX) *);                                     \
    OPSTATUS (*init)(TBIGINT_TYPE(SUFFIX) *);                                      \
    /* Arithmetic */                                                           \
    OPSTATUS (*add)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*sub)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*mul)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*divMod)(const TBIGINT_TYPE(SUFFIX) *,                              \
                       const TBIGINT_TYPE(SUFFIX) *, TBIGINT_TYPE(SUFFIX) *,        \
                       TBIGINT_TYPE(SUFFIX) *);                                     \
    OPSTATUS (*div)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*mod)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*increment)(TBIGINT_TYPE(SUFFIX) *);                                 \
    OPSTATUS (*decrement)(TBIGINT_TYPE(SUFFIX) *);                                 \
    /* Bitwise */                                                              \
    OPSTATUS (*and)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*or)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);          \
    OPSTATUS (*xor)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*not)(TBIGINT_TYPE(SUFFIX) *);                                       \
    OPSTATUS (*shiftLeft)(TBIGINT_TYPE(SUFFIX) *, unsigned);                       \
    OPSTATUS (*shiftRight)(TBIGINT_TYPE(SUFFIX) *, unsigned);                      \
    OPSTATUS (*rotateLeft)(TBIGINT_TYPE(SUFFIX) *, unsigned);                      \
    OPSTATUS (*rotateRight)(TBIGINT_TYPE(SUFFIX) *, unsigned);                     \
    OPSTATUS (*bitSet)(TBIGINT_TYPE(SUFFIX) *, uint32_t);                          \
    OPSTATUS (*bitClear)(TBIGINT_TYPE(SUFFIX) *, uint32_t);                        \
    OPSTATUS (*bitCheck)(const TBIGINT_TYPE(SUFFIX) *, uint32_t, bool *);                  \
    /* Comparison */                                                           \
    OPSTATUS (*compare)(const TBIGINT_TYPE(SUFFIX) *,                                \
                        const TBIGINT_TYPE(SUFFIX) *, CMPOUT *);                              \
    bool (*equal)(const TBIGINT_TYPE(SUFFIX) *,                                \
                  const TBIGINT_TYPE(SUFFIX) *);                               \
    bool (*notEqual)(const TBIGINT_TYPE(SUFFIX) *,                             \
                     const TBIGINT_TYPE(SUFFIX) *);                            \
    bool (*lessThan)(const TBIGINT_TYPE(SUFFIX) *,                             \
                     const TBIGINT_TYPE(SUFFIX) *);                            \
    bool (*greaterThan)(const TBIGINT_TYPE(SUFFIX) *,                          \
                        const TBIGINT_TYPE(SUFFIX) *);                         \
    bool (*lessOrEqual)(const TBIGINT_TYPE(SUFFIX) *,                          \
                        const TBIGINT_TYPE(SUFFIX) *);                         \
    bool (*greaterOrEqual)(const TBIGINT_TYPE(SUFFIX) *,                       \
                           const TBIGINT_TYPE(SUFFIX) *);                      \
    bool (*isZero)(const TBIGINT_TYPE(SUFFIX) *);                              \
    OPSTATUS (*toCString)(const TBIGINT_TYPE(SUFFIX) *, char *, size_t);          \
    OPSTATUS (*toCStringBase)(const TBIGINT_TYPE(SUFFIX) *, char *, size_t,       \
                              unsigned);                              \
  } TBIGINT_FUNCTION_TABLE_TYPE(SUFFIX);

#define TBIGINT_FUNCTION_TABLE_INSTANCE(SUFFIX)                              \
  static const TBIGINT_FUNCTION_TABLE_TYPE(SUFFIX) TBIGINT_FUNC(                   \
      SUFFIX, functions) = {                                                   \
      .clear = &TBIGINT_FUNC(SUFFIX, Clear),                                   \
      .init = &TBIGINT_FUNC(SUFFIX, Init),                                     \
      .add = &TBIGINT_FUNC(SUFFIX, Add),                                       \
      .sub = &TBIGINT_FUNC(SUFFIX, Sub),                                       \
      .mul = &TBIGINT_FUNC(SUFFIX, Mul),                                       \
      .divMod = &TBIGINT_FUNC(SUFFIX, DivMod),                                 \
      .div = &TBIGINT_FUNC(SUFFIX, Div),                                       \
      .mod = &TBIGINT_FUNC(SUFFIX, Mod),                                       \
      .increment = &TBIGINT_FUNC(SUFFIX, Increment),                           \
      .decrement = &TBIGINT_FUNC(SUFFIX, Decrement),                           \
      .and = &TBIGINT_FUNC(SUFFIX, And),                                       \
      .or = &TBIGINT_FUNC(SUFFIX, Or),                                         \
      .xor = &TBIGINT_FUNC(SUFFIX, Xor),                                       \
      .not = &TBIGINT_FUNC(SUFFIX, Not),                                       \
      .shiftLeft = &TBIGINT_FUNC(SUFFIX, ShiftLeft),                           \
      .shiftRight = &TBIGINT_FUNC(SUFFIX, ShiftRight),                         \
      .rotateLeft = &TBIGINT_FUNC(SUFFIX, RotateLeft),                         \
      .rotateRight = &TBIGINT_FUNC(SUFFIX, RotateRight),                       \
      .bitSet = &TBIGINT_FUNC(SUFFIX, BitSet),                                 \
      .bitClear = &TBIGINT_FUNC(SUFFIX, BitClear),                             \
      .bitCheck = &TBIGINT_FUNC(SUFFIX, BitCheck),                             \
      .compare = &TBIGINT_FUNC(SUFFIX, Compare),                               \
      .equal = &TBIGINT_FUNC(SUFFIX, Equal),                                   \
      .notEqual = &TBIGINT_FUNC(SUFFIX, NotEqual),                             \
      .lessThan = &TBIGINT_FUNC(SUFFIX, LessThan),                             \
      .greaterThan = &TBIGINT_FUNC(SUFFIX, GreaterThan),                       \
      .lessOrEqual = &TBIGINT_FUNC(SUFFIX, LessOrEqual),                       \
      .greaterOrEqual = &TBIGINT_FUNC(SUFFIX, GreaterOrEqual),                 \
      .isZero = &TBIGINT_FUNC(SUFFIX, IsZero),                                   \
      .toCString = &TBIGINT_FUNC(SUFFIX, ToCString),                              \
      .toCStringBase = &TBIGINT_FUNC(SUFFIX, ToCStringBase)};
/* EOF */
