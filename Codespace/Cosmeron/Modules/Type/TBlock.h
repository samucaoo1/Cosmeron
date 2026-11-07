#pragma once

#include "Type.space"
#include "../Bit/Bit.h"

#define TBLOCK_TYPE(SUFFIX) TYPE_TYPE(TBlock, SUFFIX)
#define TBLOCK_FUNC(SUFFIX, FUNCTION) NS(TBLOCK_TYPE(SUFFIX), FUNCTION)

#define TEMPLATE_TBLOCK(SUFFIX, TYPE)                                          \
  OBJECT(TBLOCK_TYPE(SUFFIX),                                                  \
      FIELDS(                                                                  \
          union {                                                              \
            TYPE limb[((SUFFIX) / 64U)];                                       \
            uint8_t byte[((SUFFIX) / 8U)];                                     \
          };                                                                   \
      ),                                                                       \
      OPERATION(Clear, ()),                                                    \
      OPERATION(And, (const TBLOCK_TYPE(SUFFIX) *source)),                     \
      OPERATION(Or, (const TBLOCK_TYPE(SUFFIX) *source)),                      \
      OPERATION(Xor, (const TBLOCK_TYPE(SUFFIX) *source)),                     \
      OPERATION(Not, ()),                                                      \
      OPERATION(ShiftLeft, (unsigned bitCount)),                               \
      OPERATION(ShiftRight, (unsigned bitCount)),                              \
      OPERATION(RotateLeft, (unsigned shift)),                                 \
      OPERATION(RotateRight, (unsigned shift)),                                \
      OPERATION(BitSet, (uint32_t bit)),                                       \
      OPERATION(BitClear, (uint32_t bit)),                                     \
      CONST_OPERATION(BitCheck, (uint32_t bit, bool *outResult))                \
  )

#include "Impl/TBlock.impl"

TYPE_INSTANTIATE_WIDTHS(TBLOCK)

/* EOF */
