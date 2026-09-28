#pragma once

#define TBLOCK_FUNCTION_TABLE(SUFFIX)                                        \
  typedef struct TBLOCK_FUNC(SUFFIX, FunctionTable) {                       \
    OPSTATUS (*clear)(TBLOCK_TYPE(SUFFIX) *);                                      \
    OPSTATUS (*and)(TBLOCK_TYPE(SUFFIX) *, const TBLOCK_TYPE(SUFFIX) *);           \
    OPSTATUS (*or)(TBLOCK_TYPE(SUFFIX) *, const TBLOCK_TYPE(SUFFIX) *);            \
    OPSTATUS (*xor)(TBLOCK_TYPE(SUFFIX) *, const TBLOCK_TYPE(SUFFIX) *);           \
    OPSTATUS (*not)(TBLOCK_TYPE(SUFFIX) *);                                        \
    OPSTATUS (*shiftLeft)(TBLOCK_TYPE(SUFFIX) *, unsigned);                        \
    OPSTATUS (*shiftRight)(TBLOCK_TYPE(SUFFIX) *, unsigned);                       \
    OPSTATUS (*rotateLeft)(TBLOCK_TYPE(SUFFIX) *, unsigned);                       \
    OPSTATUS (*rotateRight)(TBLOCK_TYPE(SUFFIX) *, unsigned);                      \
    OPSTATUS (*bitSet)(TBLOCK_TYPE(SUFFIX) *, uint32_t);                           \
    OPSTATUS (*bitClear)(TBLOCK_TYPE(SUFFIX) *, uint32_t);                         \
    OPSTATUS (*bitCheck)(const TBLOCK_TYPE(SUFFIX) *, uint32_t, bool *);                   \
  } TBLOCK_FUNC(SUFFIX, FunctionTable);

#define TBLOCK_FUNCTION_TABLE_INSTANCE(SUFFIX)                               \
  static const TBLOCK_FUNC(SUFFIX, FunctionTable) TBLOCK_FUNC(                     \
      SUFFIX, functions) = {                                                   \
      .clear = &TBLOCK_FUNC(SUFFIX, Clear),                                    \
      .and = &TBLOCK_FUNC(SUFFIX, And),                                        \
      .or = &TBLOCK_FUNC(SUFFIX, Or),                                          \
      .xor = &TBLOCK_FUNC(SUFFIX, Xor),                                        \
      .not = &TBLOCK_FUNC(SUFFIX, Not),                                        \
      .shiftLeft = &TBLOCK_FUNC(SUFFIX, ShiftLeft),                            \
      .shiftRight = &TBLOCK_FUNC(SUFFIX, ShiftRight),                          \
      .rotateLeft = &TBLOCK_FUNC(SUFFIX, RotateLeft),                          \
      .rotateRight = &TBLOCK_FUNC(SUFFIX, RotateRight),                        \
      .bitSet = &TBLOCK_FUNC(SUFFIX, BitSet),                                  \
      .bitClear = &TBLOCK_FUNC(SUFFIX, BitClear),                              \
      .bitCheck = &TBLOCK_FUNC(SUFFIX, BitCheck)};
/* EOF */
