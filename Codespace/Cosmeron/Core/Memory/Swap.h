#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Memory.space"

#define SWAP_FUNC(NAME) GNS2(LIB_PREFIX(Swap), NAME)
#define SWAP_CONST(NAME) CNS2(LIB_PREFIX_CONST(SWAP), NAME)

#define SWAP_BLOCK_SIZE 64u

#define SWAP_BYTES_PROTOTYPE                                                   \
  static inline OPSTATUS SWAP_FUNC(Bytes)(void *left, void *right, size_t size)

SWAP_BYTES_PROTOTYPE;

#define SWAP_DECLARE(TYPE, SUFFIX)                                             \
  static inline void SWAP_FUNC(SUFFIX)(TYPE *left, TYPE *right) {              \
    TYPE temporary = *left;                                                    \
    *left = *right;                                                            \
    *right = temporary;                                                        \
  }

/* Each lvalue is evaluated exactly once: only its address is passed onward. */
#define Memory_Swap(TYPE, LEFT, RIGHT)                                         \
  SWAP_FUNC(Bytes)(&(LEFT), &(RIGHT), sizeof(TYPE))

#include "Impl/Swap.impl"
/* EOF */
