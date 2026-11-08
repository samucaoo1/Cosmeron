#pragma once

#include "../Outcome/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "Common.inc"

/* ========================================================================== */
/* 1. Swap Configuration Table (Decoupled Row-Tuple Pattern)                   */
/* ========================================================================== */

#define SWAP_CONFIG_COL_NAME(NAME, VALUE) NAME
#define SWAP_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define SWAP_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(SWAP_CONFIG_COL_, FIELD) ROW

#define SWAP_CONFIG_TABLE(X) \
  X((BLOCK_SIZE, 64U))

typedef enum MODULE_ENUM(Memory, Swap, Config) {
#define SWAP_CONFIG_ENUM_ITEM(ROW) \
  MODULE_CONST(Memory, Swap, SWAP_CONFIG_GET(NAME, ROW)) = SWAP_CONFIG_GET(VALUE, ROW),
  SWAP_CONFIG_TABLE(SWAP_CONFIG_ENUM_ITEM)
#undef SWAP_CONFIG_ENUM_ITEM
} MODULE_ENUM(Memory, Swap, Config);

/* ========================================================================== */
/* 2. Package & Template Declaration: Memory_Swap                             */
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
    PACKAGE(Swap,
        OPERATION(Bytes, (void *left, void *right, size_t size)),
        OPERATION(Array, (void *left, void *right, size_t count, size_t elementSize))
    ),
    TEMPLATE_PACKAGE(Swap, (SUFFIX, TYPE),
        PROCEDURE_INLINE(Values, (TYPE *left, TYPE *right), {
          if (!left || !right || left == right) return;
          TYPE temporary = *left;
          *left = *right;
          *right = temporary;
        })
    )
);

#include "Impl/Swap.impl"

/* EOF */

