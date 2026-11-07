#pragma once

#include "../Module/Package.h"
#include "../Preprocessor/Compiling.inc"
#include "Common.h"

#define SWAP_NS(NAME) NS(LIB_PREFIX(Memory_Swap), NAME)
#define SWAP_CNS(NAME) CNS(LIB_PREFIX_CONST(MEMORY), SWAP, NAME)
#define SWAP_INS(NAME) NS(LIB_PREFIX(Memory_Swap), Internal, NAME)
#define SWAP_CINS(NAME) CNS(LIB_PREFIX_CONST(MEMORY), SWAP, INTERNAL, NAME)

#define SWAP_TYPE(NAME) SWAP_NS(NAME)
#define SWAP_FUNC(NAME) SWAP_NS(NAME)
#define SWAP_CONST(NAME) SWAP_CNS(NAME)

#define SWAP_CONFIG_COL_NAME(NAME, VALUE) NAME
#define SWAP_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define SWAP_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(SWAP_CONFIG_COL_, FIELD) ROW

#define SWAP_CONFIG_TABLE(X) \
  X((BLOCK_SIZE, 64U))

enum SWAP_TYPE(Config) {
#define SWAP_CONFIG_ENUM_ITEM(ROW) \
  SWAP_CONST(SWAP_CONFIG_GET(NAME, ROW)) = SWAP_CONFIG_GET(VALUE, ROW),
  SWAP_CONFIG_TABLE(SWAP_CONFIG_ENUM_ITEM)
#undef SWAP_CONFIG_ENUM_ITEM
};

PACKAGE(LIB_PREFIX(Memory_Swap),
    OPERATION(Bytes, (void *left, void *right, size_t size)),
    OPERATION(Array, (void *left, void *right, size_t count, size_t elementSize))
);

#define TEMPLATE_SWAP(SUFFIX, TYPE)                                            \
  PACKAGE(NS(LIB_PREFIX(Memory_Swap), SUFFIX),                                  \
      PROCEDURE_INLINE(Values, (TYPE *left, TYPE *right), {                    \
        if (!left || !right || left == right) return;                          \
        TYPE temporary = *left;                                                \
        *left = *right;                                                        \
        *right = temporary;                                                    \
      })                                                                       \
  )
#define TEMPLATE_SWAP_INLINE_ONLY 1
#define TEMPLATE_SWAP_IMPLEMENT(SUFFIX, TYPE)

#include "Impl/Swap.impl"
/* EOF */
