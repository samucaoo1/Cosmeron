#pragma once

#if defined(CONTAINER_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(OBJECT_DISABLE_FUNCTION_TABLE)
#define OBJECT_DISABLE_FUNCTION_TABLE 1
#endif

#include "../../Core/Memory/Alloc.h"
#include "../../Core/Module/Module.h"
#include "../../Core/Outcome/Status.h"
#include "../../Core/Preprocessor/Compiling.inc"
#include "../Type/Common.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Declarative Object: Container_TDynamicBitset                               */
/* ========================================================================== */

MODULE(Container,
  OBJECT(DynamicBitset,
      FIELDS(
          uint64_t *limbs;
          size_t bitCount;
          size_t limbCount;
          size_t limbCapacity;
      ),
      OPERATION(Create, (size_t bitCount)),
      OPERATION(Dispose, ()),
      OPERATION(Reserve, (size_t minBitCapacity)),
      OPERATION(Resize, (size_t newBitCount)),
      CONST_OPERATION(Clone,
                      (MODULE_TYPE(Container, DynamicBitset) *outClone)),
      OPERATION(Clear, ()),
      OPERATION(SetAll, ()),
      OPERATION(And, (const MODULE_TYPE(Container, DynamicBitset) *source)),
      OPERATION(Or, (const MODULE_TYPE(Container, DynamicBitset) *source)),
      OPERATION(Xor, (const MODULE_TYPE(Container, DynamicBitset) *source)),
      OPERATION(Not, ()),
      OPERATION(ShiftLeft, (size_t bitCount)),
      OPERATION(ShiftRight, (size_t bitCount)),
      OPERATION(BitSet, (size_t bit)),
      OPERATION(BitClear, (size_t bit)),
      OPERATION(BitFlip, (size_t bit)),
      CONST_OPERATION(BitCheck, (size_t bit, bool *outResult)),
      CONST_PREDICATE(BitTest, (size_t bit)),
      OPERATION(SetRange, (size_t startBit, size_t endBit)),
      OPERATION(ClearRange, (size_t startBit, size_t endBit)),
      OPERATION(FlipRange, (size_t startBit, size_t endBit)),
      CONST_FUNCTION(size_t, Count, ()),
      CONST_FUNCTION(size_t, Size, ()),
      CONST_FUNCTION(size_t, Capacity, ()),
      CONST_PREDICATE(Any, ()),
      CONST_PREDICATE(All, ()),
      CONST_PREDICATE(None, ()),
      CONST_PREDICATE(Equal,
                      (const MODULE_TYPE(Container, DynamicBitset) *other)),
      CONST_PREDICATE(IsSubset,
                      (const MODULE_TYPE(Container, DynamicBitset) *other)),
      CONST_PREDICATE(IsDisjoint,
                      (const MODULE_TYPE(Container, DynamicBitset) *other)),
      CONST_PREDICATE(Intersects,
                      (const MODULE_TYPE(Container, DynamicBitset) *other)),
      CONST_OPERATION(FindFirstSet, (size_t *outBit)),
      CONST_OPERATION(FindFirstZero, (size_t *outBit)),
      CONST_OPERATION(FindNextSet, (size_t startBit, size_t *outBit))
  )
);

#include "Impl/DynamicBitset.impl"

/* EOF */

