#pragma once

#include "../Preprocessor/Compiling.inc"
#include "Memory.space"

#define SWAP_NS(NAME) GNS2(MEMORY_NS(Swap), NAME)
#define SWAP_CNS(NAME) CNS3(LIB_PREFIX_CONST(MEMORY_CMOD), SWAP, NAME)

#define SWAP_FUNC(NAME) SWAP_NS(NAME)
#define SWAP_CONST(NAME) SWAP_CNS(NAME)

#define COSMERON_MACRO_INTERNAL_SWAP_BLOCK_SIZE 64u

#define SWAP_BYTES_PROTOTYPE                                                   \
  static inline OPSTATUS SWAP_FUNC(Bytes)(void *left, void *right, size_t size)

SWAP_BYTES_PROTOTYPE;

#define SWAP_PROTOTYPE(TYPE, SUFFIX)                                           \
  static inline void SWAP_FUNC(SUFFIX)(TYPE *left, TYPE *right)

#define SWAP_IMPLEMENT(TYPE, SUFFIX)                                           \
  SWAP_PROTOTYPE(TYPE, SUFFIX) {                                               \
    TYPE temporary = *left;                                                    \
    *left = *right;                                                            \
    *right = temporary;                                                        \
  }

/* Each lvalue is evaluated exactly once: only its address is passed onward. */
#define Memory_Swap(TYPE, LEFT, RIGHT)                                         \
  SWAP_FUNC(Bytes)(&(LEFT), &(RIGHT), sizeof(TYPE))

#include "Impl/Swap.impl"
/* EOF */
