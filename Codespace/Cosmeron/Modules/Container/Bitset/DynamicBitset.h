#pragma once

#if (defined(CONTAINER_DISABLE_FUNCTION_TABLE) ||                               \
     defined(BITSET_DISABLE_FUNCTION_TABLE)) &&                                \
    !defined(COSMERON_DISABLE_FT_Bitset)
#define COSMERON_DISABLE_FT_Bitset 1
#endif
#if defined(BITSET_DYNAMIC_DISABLE_FUNCTION_TABLE) &&                          \
    !defined(COSMERON_DISABLE_FT_Bitset_Dynamic)
#define COSMERON_DISABLE_FT_Bitset_Dynamic 1
#endif

#include "../../../Core/Memory/Alloc.h"
#include "../../../Core/Memory/Allocator.h"
#include "../../../Core/Module/Module.h"
#include "../../../Core/Condition/Status.h"
#include "../../../Core/Preprocessor/Compiling.inc"
#include "../../../Core/Types/Impl/Common.inc"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Declarative Object: Bitset_TDynamic                               */
/* ========================================================================== */

MODULE(DOMAIN(Container), Bitset,
  OBJECT(Dynamic,
      FIELDS(
          uint64_t *limbs;
          size_t bitCount;
          size_t limbCount;
          size_t limbCapacity;
          MODULE_TYPE(Memory, Allocator) allocator;
      ),
      VARIANT(Create,
          OPERATION((size_t bitCount)),
          OPERATION(WithAllocator,
                    (size_t bitCount, MODULE_TYPE(Memory, Allocator) allocator))
      ),
      OPERATION(Dispose, ()),
      OPERATION(Reserve, (size_t minBitCapacity)),
      OPERATION(Resize, (size_t newBitCount)),
      CONST_OPERATION(Clone,
                      (MODULE_TYPE(Bitset, Dynamic) *outClone)),
      OPERATION(Clear, ()),
      OPERATION(SetAll, ()),
      OPERATION(And, (const MODULE_TYPE(Bitset, Dynamic) *source)),
      OPERATION(Or, (const MODULE_TYPE(Bitset, Dynamic) *source)),
      OPERATION(Xor, (const MODULE_TYPE(Bitset, Dynamic) *source)),
      OPERATION(Not, ()),
      OPERATION(ShiftLeft, (size_t bitCount)),
      OPERATION(ShiftRight, (size_t bitCount)),
      VARIANT(Bit,
          OPERATION(Set, (size_t bit)),
          OPERATION(Clear, (size_t bit)),
          OPERATION(Flip, (size_t bit)),
          CONST_OPERATION(Check, (size_t bit, bool *outResult)),
          CONST_PREDICATE(Test, (size_t bit))
      ),
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
                      (const MODULE_TYPE(Bitset, Dynamic) *other)),
      VARIANT(Is,
          CONST_PREDICATE(Subset,
                          (const MODULE_TYPE(Bitset, Dynamic) *other)),
          CONST_PREDICATE(Disjoint,
                          (const MODULE_TYPE(Bitset, Dynamic) *other))
      ),
      CONST_PREDICATE(Intersects,
                      (const MODULE_TYPE(Bitset, Dynamic) *other)),
      VARIANT(Find,
          CONST_OPERATION(FirstSet, (size_t *outBit)),
          CONST_OPERATION(FirstZero, (size_t *outBit)),
          CONST_OPERATION(NextSet, (size_t startBit, size_t *outBit))
      )
  )
);

#include "Impl/DynamicBitset.impl"

/* EOF */

