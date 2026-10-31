#pragma once

#include "../Module/Package.h"
#include "../Preprocessor/Compiling.inc"
#include "Memory.space"

#define SWAP_MOD GNS2(MEMORY_MOD, Swap)
#define SWAP_CMOD PP_OP_CAT3(MEMORY_CMOD, _, SWAP)

#define SWAP_NS(NAME) GNS3(LIB_PREFIX(MEMORY_MOD), Swap, NAME)
#define SWAP_CNS(NAME) CNS3(LIB_PREFIX_CONST(MEMORY_CMOD), SWAP, NAME)
#define SWAP_INS(NAME) GNS4(LIB_PREFIX(MEMORY_MOD), Swap, Internal, NAME)
#define SWAP_CINS(NAME) CNS4(LIB_PREFIX_CONST(MEMORY_CMOD), SWAP, INTERNAL, NAME)

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

PACKAGE(LIB_PREFIX(SWAP_MOD),
    OPERATION(Bytes, (void *left, void *right, size_t size)),
    OPERATION(Array, (void *left, void *right, size_t count, size_t elementSize))
);

#define TEMPLATE_SWAP(SUFFIX, TYPE)                                            \
  PACKAGE(GNS2(LIB_PREFIX(SWAP_MOD), SUFFIX),                                  \
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
