#pragma once

#include "../Module/Module.h"
#include "../Outcome/Status.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "Common.inc"

/* ========================================================================== */
/* 1. Swap Configuration Table (Decoupled Row-Tuple Pattern)                  */
/* ========================================================================== */

#define SWAP_CONFIG_COL_NAME(NAME, VALUE) NAME
#define SWAP_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define SWAP_CONFIG_GET(FIELD, ROW) PP_OP_CAT2(SWAP_CONFIG_COL_, FIELD) ROW

#define SWAP_CONFIG_TABLE(X) X((BLOCK_SIZE, 64U))

/* ========================================================================== */
/* 2. Package Declaration: Memory_Swap                                        */
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
       PACKAGE(Swap, ENUM(Config, SWAP_CONFIG_TABLE),
               OPERATION(Bytes, (void *left, void *right, size_t size)),
               OPERATION(Array, (void *left, void *right, size_t count,
                                 size_t elementSize))));

#include "Impl/Swap.impl"
#include "Swap.def"

/* EOF */
