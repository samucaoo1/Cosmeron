#pragma once

#define TBLOCK_FUNCTION_TABLE(SUFFIX)                                        \
  typedef struct TBLOCK_FUNC(SUFFIX, FunctionTable) {                       \
    OPSTATUS (*Clear)(TBLOCK_TYPE(SUFFIX) *);                                      \
    OPSTATUS (*And)(TBLOCK_TYPE(SUFFIX) *, const TBLOCK_TYPE(SUFFIX) *);           \
    OPSTATUS (*Or)(TBLOCK_TYPE(SUFFIX) *, const TBLOCK_TYPE(SUFFIX) *);            \
    OPSTATUS (*Xor)(TBLOCK_TYPE(SUFFIX) *, const TBLOCK_TYPE(SUFFIX) *);           \
    OPSTATUS (*Not)(TBLOCK_TYPE(SUFFIX) *);                                        \
    OPSTATUS (*ShiftLeft)(TBLOCK_TYPE(SUFFIX) *, unsigned);                        \
    OPSTATUS (*ShiftRight)(TBLOCK_TYPE(SUFFIX) *, unsigned);                       \
    OPSTATUS (*RotateLeft)(TBLOCK_TYPE(SUFFIX) *, unsigned);                       \
    OPSTATUS (*RotateRight)(TBLOCK_TYPE(SUFFIX) *, unsigned);                      \
    OPSTATUS (*BitSet)(TBLOCK_TYPE(SUFFIX) *, uint32_t);                           \
    OPSTATUS (*BitClear)(TBLOCK_TYPE(SUFFIX) *, uint32_t);                         \
    OPSTATUS (*BitCheck)(const TBLOCK_TYPE(SUFFIX) *, uint32_t, bool *);                   \
  } TBLOCK_FUNC(SUFFIX, FunctionTable);

#define TBLOCK_FUNCTION_TABLE_INSTANCE(SUFFIX)                               \
  static const TBLOCK_FUNC(SUFFIX, FunctionTable) TBLOCK_FUNC(                     \
      SUFFIX, functions) = {                                                   \
      .Clear = &TBLOCK_FUNC(SUFFIX, Clear),                                    \
      .And = &TBLOCK_FUNC(SUFFIX, And),                                        \
      .Or = &TBLOCK_FUNC(SUFFIX, Or),                                          \
      .Xor = &TBLOCK_FUNC(SUFFIX, Xor),                                        \
      .Not = &TBLOCK_FUNC(SUFFIX, Not),                                        \
      .ShiftLeft = &TBLOCK_FUNC(SUFFIX, ShiftLeft),                            \
      .ShiftRight = &TBLOCK_FUNC(SUFFIX, ShiftRight),                          \
      .RotateLeft = &TBLOCK_FUNC(SUFFIX, RotateLeft),                          \
      .RotateRight = &TBLOCK_FUNC(SUFFIX, RotateRight),                        \
      .BitSet = &TBLOCK_FUNC(SUFFIX, BitSet),                                  \
      .BitClear = &TBLOCK_FUNC(SUFFIX, BitClear),                              \
      .BitCheck = &TBLOCK_FUNC(SUFFIX, BitCheck)};
/* EOF */
