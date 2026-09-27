#pragma once

#define TDECIMAL_FUNCTION_TABLE(SUFFIX)                                       \
  typedef struct TDECIMAL_FUNC(SUFFIX, FunctionTable) {                     \
    OPSTATUS (*Clear)(TDECIMAL_TYPE(SUFFIX) *);                                    \
    OPSTATUS (*Init)(TDECIMAL_TYPE(SUFFIX) *);                                     \
    /* Arithmetic */                                                           \
    OPSTATUS (*Add)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*Sub)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*Mul)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*DivMod)(const TDECIMAL_TYPE(SUFFIX) *,                              \
                       const TDECIMAL_TYPE(SUFFIX) *, TDECIMAL_TYPE(SUFFIX) *,       \
                       TDECIMAL_TYPE(SUFFIX) *);                                    \
    OPSTATUS (*Div)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*Mod)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*Increment)(TDECIMAL_TYPE(SUFFIX) *);                                \
    OPSTATUS (*Decrement)(TDECIMAL_TYPE(SUFFIX) *);                                \
    /* Bitwise */                                                              \
    OPSTATUS (*And)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*Or)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);         \
    OPSTATUS (*Xor)(TDECIMAL_TYPE(SUFFIX) *, const TDECIMAL_TYPE(SUFFIX) *);        \
    OPSTATUS (*Not)(TDECIMAL_TYPE(SUFFIX) *);                                      \
    OPSTATUS (*ShiftLeft)(TDECIMAL_TYPE(SUFFIX) *, unsigned);                      \
    OPSTATUS (*ShiftRight)(TDECIMAL_TYPE(SUFFIX) *, unsigned);                     \
    OPSTATUS (*RotateLeft)(TDECIMAL_TYPE(SUFFIX) *, unsigned);                     \
    OPSTATUS (*RotateRight)(TDECIMAL_TYPE(SUFFIX) *, unsigned);                    \
    OPSTATUS (*BitSet)(TDECIMAL_TYPE(SUFFIX) *, uint32_t);                         \
    OPSTATUS (*BitClear)(TDECIMAL_TYPE(SUFFIX) *, uint32_t);                       \
    OPSTATUS (*BitCheck)(const TDECIMAL_TYPE(SUFFIX) *, uint32_t, bool *);                 \
    /* Comparison */                                                           \
    OPSTATUS (*Compare)(const TDECIMAL_TYPE(SUFFIX) *,                                \
                        const TDECIMAL_TYPE(SUFFIX) *, TComparisonResult *);                             \
    bool (*Equal)(const TDECIMAL_TYPE(SUFFIX) *,                               \
                  const TDECIMAL_TYPE(SUFFIX) *);                              \
    bool (*NotEqual)(const TDECIMAL_TYPE(SUFFIX) *,                            \
                     const TDECIMAL_TYPE(SUFFIX) *);                           \
    bool (*LessThan)(const TDECIMAL_TYPE(SUFFIX) *,                            \
                     const TDECIMAL_TYPE(SUFFIX) *);                           \
    bool (*GreaterThan)(const TDECIMAL_TYPE(SUFFIX) *,                         \
                        const TDECIMAL_TYPE(SUFFIX) *);                        \
    bool (*LessOrEqual)(const TDECIMAL_TYPE(SUFFIX) *,                         \
                        const TDECIMAL_TYPE(SUFFIX) *);                        \
    bool (*GreaterOrEqual)(const TDECIMAL_TYPE(SUFFIX) *,                      \
                           const TDECIMAL_TYPE(SUFFIX) *);                     \
    bool (*IsZero)(const TDECIMAL_TYPE(SUFFIX) *);                              \
    OPSTATUS (*ToCString)(const TDECIMAL_TYPE(SUFFIX) *, char *, size_t);          \
    OPSTATUS (*ToCStringBase)(const TDECIMAL_TYPE(SUFFIX) *, char *, size_t,       \
                              unsigned);                             \
  } TDECIMAL_FUNC(SUFFIX, FunctionTable);

#define TDECIMAL_FUNCTION_TABLE_INSTANCE(SUFFIX)                             \
  static const TDECIMAL_FUNC(SUFFIX, FunctionTable) TDECIMAL_FUNC(                 \
      SUFFIX, functions) = {                                                   \
      .Clear = &TDECIMAL_FUNC(SUFFIX, Clear),                                  \
      .Init = &TDECIMAL_FUNC(SUFFIX, Init),                                    \
      .Add = &TDECIMAL_FUNC(SUFFIX, Add),                                      \
      .Sub = &TDECIMAL_FUNC(SUFFIX, Sub),                                      \
      .Mul = &TDECIMAL_FUNC(SUFFIX, Mul),                                      \
      .DivMod = &TDECIMAL_FUNC(SUFFIX, DivMod),                                \
      .Div = &TDECIMAL_FUNC(SUFFIX, Div),                                      \
      .Mod = &TDECIMAL_FUNC(SUFFIX, Mod),                                      \
      .Increment = &TDECIMAL_FUNC(SUFFIX, Increment),                          \
      .Decrement = &TDECIMAL_FUNC(SUFFIX, Decrement),                          \
      .And = &TDECIMAL_FUNC(SUFFIX, And),                                      \
      .Or = &TDECIMAL_FUNC(SUFFIX, Or),                                        \
      .Xor = &TDECIMAL_FUNC(SUFFIX, Xor),                                      \
      .Not = &TDECIMAL_FUNC(SUFFIX, Not),                                      \
      .ShiftLeft = &TDECIMAL_FUNC(SUFFIX, ShiftLeft),                          \
      .ShiftRight = &TDECIMAL_FUNC(SUFFIX, ShiftRight),                        \
      .RotateLeft = &TDECIMAL_FUNC(SUFFIX, RotateLeft),                        \
      .RotateRight = &TDECIMAL_FUNC(SUFFIX, RotateRight),                      \
      .BitSet = &TDECIMAL_FUNC(SUFFIX, BitSet),                                \
      .BitClear = &TDECIMAL_FUNC(SUFFIX, BitClear),                            \
      .BitCheck = &TDECIMAL_FUNC(SUFFIX, BitCheck),                            \
      .Compare = &TDECIMAL_FUNC(SUFFIX, Compare),                              \
      .Equal = &TDECIMAL_FUNC(SUFFIX, Equal),                                  \
      .NotEqual = &TDECIMAL_FUNC(SUFFIX, NotEqual),                            \
      .LessThan = &TDECIMAL_FUNC(SUFFIX, LessThan),                            \
      .GreaterThan = &TDECIMAL_FUNC(SUFFIX, GreaterThan),                      \
      .LessOrEqual = &TDECIMAL_FUNC(SUFFIX, LessOrEqual),                      \
      .GreaterOrEqual = &TDECIMAL_FUNC(SUFFIX, GreaterOrEqual),                \
      .IsZero = &TDECIMAL_FUNC(SUFFIX, IsZero),                                   \
      .ToCString = &TDECIMAL_FUNC(SUFFIX, ToCString),                              \
      .ToCStringBase = &TDECIMAL_FUNC(SUFFIX, ToCStringBase)};
/* EOF */
