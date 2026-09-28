#pragma once

#define TDECIMAL_FUNCTION_TABLE(SUFFIX)                                       \
  typedef struct TDECIMAL_FUNC(SUFFIX, FunctionTable) {                     \
    OPSTATUS (*clear)(TDECIMAL_TYPE(SUFFIX) *);                                    \
    OPSTATUS (*init)(TDECIMAL_TYPE(SUFFIX) *);                                     \
    /* Arithmetic */                                                           \
    OPSTATUS (*add)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*sub)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*mul)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*divMod)(const TDECIMAL_TYPE(SUFFIX) *,                              \
                       const TDECIMAL_TYPE(SUFFIX) *, TDECIMAL_TYPE(SUFFIX) *,       \
                       TDECIMAL_TYPE(SUFFIX) *);                                    \
    OPSTATUS (*div)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*mod)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*increment)(TDECIMAL_TYPE(SUFFIX) *);                                \
    OPSTATUS (*decrement)(TDECIMAL_TYPE(SUFFIX) *);                                \
    /* Bitwise */                                                              \
    OPSTATUS (*and)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*or)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);         \
    OPSTATUS (*xor)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*not)(TDECIMAL_TYPE(SUFFIX) *);                                      \
    OPSTATUS (*shiftLeft)(TDECIMAL_TYPE(SUFFIX) *, unsigned);                      \
    OPSTATUS (*shiftRight)(TDECIMAL_TYPE(SUFFIX) *, unsigned);                     \
    OPSTATUS (*rotateLeft)(TDECIMAL_TYPE(SUFFIX) *, unsigned);                     \
    OPSTATUS (*rotateRight)(TDECIMAL_TYPE(SUFFIX) *, unsigned);                    \
    OPSTATUS (*bitSet)(TDECIMAL_TYPE(SUFFIX) *, uint32_t);                         \
    OPSTATUS (*bitClear)(TDECIMAL_TYPE(SUFFIX) *, uint32_t);                       \
    OPSTATUS (*bitCheck)(const TDECIMAL_TYPE(SUFFIX) *, uint32_t, bool *);                 \
    /* Comparison */                                                           \
    OPSTATUS (*compare)(const TDECIMAL_TYPE(SUFFIX) *,                                \
                        const TDECIMAL_TYPE(SUFFIX) *, CMPOUT *);                             \
    bool (*equal)(const TDECIMAL_TYPE(SUFFIX) *,                               \
                  const TDECIMAL_TYPE(SUFFIX) *);                              \
    bool (*notEqual)(const TDECIMAL_TYPE(SUFFIX) *,                            \
                     const TDECIMAL_TYPE(SUFFIX) *);                           \
    bool (*lessThan)(const TDECIMAL_TYPE(SUFFIX) *,                            \
                     const TDECIMAL_TYPE(SUFFIX) *);                           \
    bool (*greaterThan)(const TDECIMAL_TYPE(SUFFIX) *,                         \
                        const TDECIMAL_TYPE(SUFFIX) *);                        \
    bool (*lessOrEqual)(const TDECIMAL_TYPE(SUFFIX) *,                         \
                        const TDECIMAL_TYPE(SUFFIX) *);                        \
    bool (*greaterOrEqual)(const TDECIMAL_TYPE(SUFFIX) *,                      \
                           const TDECIMAL_TYPE(SUFFIX) *);                     \
    bool (*isZero)(const TDECIMAL_TYPE(SUFFIX) *);                              \
    OPSTATUS (*toCString)(const TDECIMAL_TYPE(SUFFIX) *, char *, size_t);          \
    OPSTATUS (*toCStringBase)(const TDECIMAL_TYPE(SUFFIX) *, char *, size_t,       \
                              unsigned);                             \
  } TDECIMAL_FUNC(SUFFIX, FunctionTable);

#define TDECIMAL_FUNCTION_TABLE_INSTANCE(SUFFIX)                             \
  static const TDECIMAL_FUNC(SUFFIX, FunctionTable) TDECIMAL_FUNC(                 \
      SUFFIX, functions) = {                                                   \
      .clear = &TDECIMAL_FUNC(SUFFIX, Clear),                                  \
      .init = &TDECIMAL_FUNC(SUFFIX, Init),                                    \
      .add = &TDECIMAL_FUNC(SUFFIX, Add),                                      \
      .sub = &TDECIMAL_FUNC(SUFFIX, Sub),                                      \
      .mul = &TDECIMAL_FUNC(SUFFIX, Mul),                                      \
      .divMod = &TDECIMAL_FUNC(SUFFIX, DivMod),                                \
      .div = &TDECIMAL_FUNC(SUFFIX, Div),                                      \
      .mod = &TDECIMAL_FUNC(SUFFIX, Mod),                                      \
      .increment = &TDECIMAL_FUNC(SUFFIX, Increment),                          \
      .decrement = &TDECIMAL_FUNC(SUFFIX, Decrement),                          \
      .and = &TDECIMAL_FUNC(SUFFIX, And),                                      \
      .or = &TDECIMAL_FUNC(SUFFIX, Or),                                        \
      .xor = &TDECIMAL_FUNC(SUFFIX, Xor),                                      \
      .not = &TDECIMAL_FUNC(SUFFIX, Not),                                      \
      .shiftLeft = &TDECIMAL_FUNC(SUFFIX, ShiftLeft),                          \
      .shiftRight = &TDECIMAL_FUNC(SUFFIX, ShiftRight),                        \
      .rotateLeft = &TDECIMAL_FUNC(SUFFIX, RotateLeft),                        \
      .rotateRight = &TDECIMAL_FUNC(SUFFIX, RotateRight),                      \
      .bitSet = &TDECIMAL_FUNC(SUFFIX, BitSet),                                \
      .bitClear = &TDECIMAL_FUNC(SUFFIX, BitClear),                            \
      .bitCheck = &TDECIMAL_FUNC(SUFFIX, BitCheck),                            \
      .compare = &TDECIMAL_FUNC(SUFFIX, Compare),                              \
      .equal = &TDECIMAL_FUNC(SUFFIX, Equal),                                  \
      .notEqual = &TDECIMAL_FUNC(SUFFIX, NotEqual),                            \
      .lessThan = &TDECIMAL_FUNC(SUFFIX, LessThan),                            \
      .greaterThan = &TDECIMAL_FUNC(SUFFIX, GreaterThan),                      \
      .lessOrEqual = &TDECIMAL_FUNC(SUFFIX, LessOrEqual),                      \
      .greaterOrEqual = &TDECIMAL_FUNC(SUFFIX, GreaterOrEqual),                \
      .isZero = &TDECIMAL_FUNC(SUFFIX, IsZero),                                   \
      .toCString = &TDECIMAL_FUNC(SUFFIX, ToCString),                              \
      .toCStringBase = &TDECIMAL_FUNC(SUFFIX, ToCStringBase)};
/* EOF */
