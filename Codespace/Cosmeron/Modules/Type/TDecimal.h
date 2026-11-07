#pragma once

#include "Type.space"
#include "../Bit/Bit.h"

#define TDECIMAL_TYPE(SUFFIX) TYPE_TYPE(TDecimal, SUFFIX)
#define TDECIMAL_FUNC(SUFFIX, FUNCTION) NS(TDECIMAL_TYPE(SUFFIX), FUNCTION)
#define TDECIMAL_INS(SUFFIX, NAME) NS(TYPE_INS(PP_OP_CAT2(TDecimal, SUFFIX)), NAME)

#define TEMPLATE_TDECIMAL(SUFFIX, TYPE)                                        \
  OBJECT(TDECIMAL_TYPE(SUFFIX),                                                \
      FIELDS(                                                                  \
          union {                                                              \
            TYPE limb[((SUFFIX) / 64U)];                                       \
            uint8_t byte[((SUFFIX) / 8U)];                                     \
          };                                                                   \
      ),                                                                       \
      OPERATION(Clear, ()),                                                    \
      OPERATION(Init, ()),                                                     \
      OPERATION(Add, (const TDECIMAL_TYPE(SUFFIX) *source)),                   \
      OPERATION(Sub, (const TDECIMAL_TYPE(SUFFIX) *source)),                   \
      OPERATION(Mul, (const TDECIMAL_TYPE(SUFFIX) *source)),                   \
      CONST_OPERATION(DivMod, (const TDECIMAL_TYPE(SUFFIX) *divisor,          \
                               TDECIMAL_TYPE(SUFFIX) *outQuotient,             \
                               TDECIMAL_TYPE(SUFFIX) *outRemainder)),          \
      OPERATION(Div, (const TDECIMAL_TYPE(SUFFIX) *divisor)),                  \
      OPERATION(Mod, (const TDECIMAL_TYPE(SUFFIX) *divisor)),                  \
      OPERATION(Increment, ()),                                                \
      OPERATION(Decrement, ()),                                                \
      OPERATION(And, (const TDECIMAL_TYPE(SUFFIX) *source)),                   \
      OPERATION(Or, (const TDECIMAL_TYPE(SUFFIX) *source)),                    \
      OPERATION(Xor, (const TDECIMAL_TYPE(SUFFIX) *source)),                   \
      OPERATION(Not, ()),                                                      \
      OPERATION(ShiftLeft, (unsigned bitCount)),                               \
      OPERATION(ShiftRight, (unsigned bitCount)),                              \
      OPERATION(RotateLeft, (unsigned shift)),                                 \
      OPERATION(RotateRight, (unsigned shift)),                                \
      OPERATION(BitSet, (uint32_t bit)),                                       \
      OPERATION(BitClear, (uint32_t bit)),                                     \
      CONST_OPERATION(BitCheck, (uint32_t bit, bool *outResult)),             \
      CONST_OPERATION(Compare, (const TDECIMAL_TYPE(SUFFIX) *other,            \
                                CMPOUT *result)),                              \
      CONST_PREDICATE(Equal, (const TDECIMAL_TYPE(SUFFIX) *other)),            \
      CONST_PREDICATE(NotEqual, (const TDECIMAL_TYPE(SUFFIX) *other)),         \
      CONST_PREDICATE(LessThan, (const TDECIMAL_TYPE(SUFFIX) *other)),         \
      CONST_PREDICATE(GreaterThan, (const TDECIMAL_TYPE(SUFFIX) *other)),      \
      CONST_PREDICATE(LessOrEqual, (const TDECIMAL_TYPE(SUFFIX) *other)),      \
      CONST_PREDICATE(GreaterOrEqual, (const TDECIMAL_TYPE(SUFFIX) *other)),   \
      CONST_PREDICATE(IsZero, ()),                                             \
      CONST_OPERATION(ToCStringBase, (char *buffer, size_t bufferSize,         \
                                      unsigned base)),                         \
      CONST_OPERATION(ToCString, (char *buffer, size_t bufferSize))            \
  )

#include "Impl/TDecimal.impl"

TYPE_INSTANTIATE_WIDTHS(TDECIMAL)

/* EOF */
