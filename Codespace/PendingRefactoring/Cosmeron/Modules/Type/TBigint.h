#pragma once

#include "Type.space"
#include "../Bit/Bit.h"

#define TBIGINT_TYPE(SUFFIX) TYPE_TYPE(TBigint, SUFFIX)
#define TBIGINT_FUNC(SUFFIX, FUNCTION) NS(TBIGINT_TYPE(SUFFIX), FUNCTION)
#define TBIGINT_INS(SUFFIX, NAME) NS(TYPE_INS(PP_OP_CAT2(TBigint, SUFFIX)), NAME)

#define TEMPLATE_TBIGINT(SUFFIX, TYPE)                                         \
  OBJECT(TBIGINT_TYPE(SUFFIX),                                                 \
      FIELDS(                                                                  \
          union {                                                              \
            TYPE limb[((SUFFIX) / 64U)];                                       \
            uint8_t byte[((SUFFIX) / 8U)];                                     \
          };                                                                   \
      ),                                                                       \
      OPERATION(Clear, ()),                                                    \
      OPERATION(Init, ()),                                                     \
      OPERATION(Add, (const TBIGINT_TYPE(SUFFIX) *source)),                    \
      OPERATION(Sub, (const TBIGINT_TYPE(SUFFIX) *source)),                    \
      OPERATION(Mul, (const TBIGINT_TYPE(SUFFIX) *source)),                    \
      CONST_OPERATION(DivMod, (const TBIGINT_TYPE(SUFFIX) *divisor,           \
                               TBIGINT_TYPE(SUFFIX) *outQuotient,              \
                               TBIGINT_TYPE(SUFFIX) *outRemainder)),           \
      OPERATION(Div, (const TBIGINT_TYPE(SUFFIX) *divisor)),                   \
      OPERATION(Mod, (const TBIGINT_TYPE(SUFFIX) *divisor)),                   \
      OPERATION(Increment, ()),                                                \
      OPERATION(Decrement, ()),                                                \
      OPERATION(And, (const TBIGINT_TYPE(SUFFIX) *source)),                    \
      OPERATION(Or, (const TBIGINT_TYPE(SUFFIX) *source)),                     \
      OPERATION(Xor, (const TBIGINT_TYPE(SUFFIX) *source)),                    \
      OPERATION(Not, ()),                                                      \
      OPERATION(ShiftLeft, (unsigned bitCount)),                               \
      OPERATION(ShiftRight, (unsigned bitCount)),                              \
      OPERATION(RotateLeft, (unsigned shift)),                                 \
      OPERATION(RotateRight, (unsigned shift)),                                \
      OPERATION(BitSet, (uint32_t bit)),                                       \
      OPERATION(BitClear, (uint32_t bit)),                                     \
      CONST_OPERATION(BitCheck, (uint32_t bit, bool *outResult)),              \
      CONST_OPERATION(Compare, (const TBIGINT_TYPE(SUFFIX) *other,             \
                                CMPOUT *result)),                              \
      CONST_PREDICATE(Equal, (const TBIGINT_TYPE(SUFFIX) *other)),             \
      CONST_PREDICATE(NotEqual, (const TBIGINT_TYPE(SUFFIX) *other)),          \
      CONST_PREDICATE(LessThan, (const TBIGINT_TYPE(SUFFIX) *other)),          \
      CONST_PREDICATE(GreaterThan, (const TBIGINT_TYPE(SUFFIX) *other)),       \
      CONST_PREDICATE(LessOrEqual, (const TBIGINT_TYPE(SUFFIX) *other)),       \
      CONST_PREDICATE(GreaterOrEqual, (const TBIGINT_TYPE(SUFFIX) *other)),    \
      CONST_PREDICATE(IsZero, ()),                                             \
      CONST_OPERATION(ToCStringBase, (char *buffer, size_t bufferSize,          \
                                      unsigned base)),                         \
      CONST_OPERATION(ToCString, (char *buffer, size_t bufferSize))             \
  )

#include "Impl/TBigint.impl"

TYPE_INSTANTIATE_WIDTHS(TBIGINT)

/* EOF */
