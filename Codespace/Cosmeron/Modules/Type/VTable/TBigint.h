#pragma once

#define TBIGINT_FUNCTION_TABLE(SUFFIX)                                        \
  typedef struct TBIGINT_FUNC(SUFFIX, FunctionTable) {                      \
    OPSTATUS (*Clear)(TBIGINT_TYPE(SUFFIX) *);                                     \
    OPSTATUS (*Init)(TBIGINT_TYPE(SUFFIX) *);                                      \
    /* Arithmetic */                                                           \
    OPSTATUS (*Add)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*Sub)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*Mul)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*DivMod)(const TBIGINT_TYPE(SUFFIX) *,                              \
                       const TBIGINT_TYPE(SUFFIX) *, TBIGINT_TYPE(SUFFIX) *,        \
                       TBIGINT_TYPE(SUFFIX) *);                                     \
    OPSTATUS (*Div)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*Mod)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*Increment)(TBIGINT_TYPE(SUFFIX) *);                                 \
    OPSTATUS (*Decrement)(TBIGINT_TYPE(SUFFIX) *);                                 \
    /* Bitwise */                                                              \
    OPSTATUS (*And)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*Or)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);          \
    OPSTATUS (*Xor)(TBIGINT_TYPE(SUFFIX) *, const TBIGINT_TYPE(SUFFIX) *);         \
    OPSTATUS (*Not)(TBIGINT_TYPE(SUFFIX) *);                                       \
    OPSTATUS (*ShiftLeft)(TBIGINT_TYPE(SUFFIX) *, unsigned);                       \
    OPSTATUS (*ShiftRight)(TBIGINT_TYPE(SUFFIX) *, unsigned);                      \
    OPSTATUS (*RotateLeft)(TBIGINT_TYPE(SUFFIX) *, unsigned);                      \
    OPSTATUS (*RotateRight)(TBIGINT_TYPE(SUFFIX) *, unsigned);                     \
    OPSTATUS (*BitSet)(TBIGINT_TYPE(SUFFIX) *, uint32_t);                          \
    OPSTATUS (*BitClear)(TBIGINT_TYPE(SUFFIX) *, uint32_t);                        \
    OPSTATUS (*BitCheck)(const TBIGINT_TYPE(SUFFIX) *, uint32_t, bool *);                  \
    /* Comparison */                                                           \
    OPSTATUS (*Compare)(const TBIGINT_TYPE(SUFFIX) *,                                \
                        const TBIGINT_TYPE(SUFFIX) *, TComparisonResult *);                              \
    bool (*Equal)(const TBIGINT_TYPE(SUFFIX) *,                                \
                  const TBIGINT_TYPE(SUFFIX) *);                               \
    bool (*NotEqual)(const TBIGINT_TYPE(SUFFIX) *,                             \
                     const TBIGINT_TYPE(SUFFIX) *);                            \
    bool (*LessThan)(const TBIGINT_TYPE(SUFFIX) *,                             \
                     const TBIGINT_TYPE(SUFFIX) *);                            \
    bool (*GreaterThan)(const TBIGINT_TYPE(SUFFIX) *,                          \
                        const TBIGINT_TYPE(SUFFIX) *);                         \
    bool (*LessOrEqual)(const TBIGINT_TYPE(SUFFIX) *,                          \
                        const TBIGINT_TYPE(SUFFIX) *);                         \
    bool (*GreaterOrEqual)(const TBIGINT_TYPE(SUFFIX) *,                       \
                           const TBIGINT_TYPE(SUFFIX) *);                      \
    bool (*IsZero)(const TBIGINT_TYPE(SUFFIX) *);                              \
    OPSTATUS (*ToCString)(const TBIGINT_TYPE(SUFFIX) *, char *, size_t);          \
    OPSTATUS (*ToCStringBase)(const TBIGINT_TYPE(SUFFIX) *, char *, size_t,       \
                              unsigned);                              \
  } TBIGINT_FUNC(SUFFIX, FunctionTable);

#define TBIGINT_FUNCTION_TABLE_INSTANCE(SUFFIX)                              \
  static const TBIGINT_FUNC(SUFFIX, FunctionTable) TBIGINT_FUNC(                   \
      SUFFIX, functions) = {                                                   \
      .Clear = &TBIGINT_FUNC(SUFFIX, Clear),                                   \
      .Init = &TBIGINT_FUNC(SUFFIX, Init),                                     \
      .Add = &TBIGINT_FUNC(SUFFIX, Add),                                       \
      .Sub = &TBIGINT_FUNC(SUFFIX, Sub),                                       \
      .Mul = &TBIGINT_FUNC(SUFFIX, Mul),                                       \
      .DivMod = &TBIGINT_FUNC(SUFFIX, DivMod),                                 \
      .Div = &TBIGINT_FUNC(SUFFIX, Div),                                       \
      .Mod = &TBIGINT_FUNC(SUFFIX, Mod),                                       \
      .Increment = &TBIGINT_FUNC(SUFFIX, Increment),                           \
      .Decrement = &TBIGINT_FUNC(SUFFIX, Decrement),                           \
      .And = &TBIGINT_FUNC(SUFFIX, And),                                       \
      .Or = &TBIGINT_FUNC(SUFFIX, Or),                                         \
      .Xor = &TBIGINT_FUNC(SUFFIX, Xor),                                       \
      .Not = &TBIGINT_FUNC(SUFFIX, Not),                                       \
      .ShiftLeft = &TBIGINT_FUNC(SUFFIX, ShiftLeft),                           \
      .ShiftRight = &TBIGINT_FUNC(SUFFIX, ShiftRight),                         \
      .RotateLeft = &TBIGINT_FUNC(SUFFIX, RotateLeft),                         \
      .RotateRight = &TBIGINT_FUNC(SUFFIX, RotateRight),                       \
      .BitSet = &TBIGINT_FUNC(SUFFIX, BitSet),                                 \
      .BitClear = &TBIGINT_FUNC(SUFFIX, BitClear),                             \
      .BitCheck = &TBIGINT_FUNC(SUFFIX, BitCheck),                             \
      .Compare = &TBIGINT_FUNC(SUFFIX, Compare),                               \
      .Equal = &TBIGINT_FUNC(SUFFIX, Equal),                                   \
      .NotEqual = &TBIGINT_FUNC(SUFFIX, NotEqual),                             \
      .LessThan = &TBIGINT_FUNC(SUFFIX, LessThan),                             \
      .GreaterThan = &TBIGINT_FUNC(SUFFIX, GreaterThan),                       \
      .LessOrEqual = &TBIGINT_FUNC(SUFFIX, LessOrEqual),                       \
      .GreaterOrEqual = &TBIGINT_FUNC(SUFFIX, GreaterOrEqual),                 \
      .IsZero = &TBIGINT_FUNC(SUFFIX, IsZero),                                   \
      .ToCString = &TBIGINT_FUNC(SUFFIX, ToCString),                              \
      .ToCStringBase = &TBIGINT_FUNC(SUFFIX, ToCStringBase)};
/* EOF */
