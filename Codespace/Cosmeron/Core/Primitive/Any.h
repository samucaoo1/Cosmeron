#pragma once

#include "../Algorithm/Comparison.h"
#include "../Memory/Alloc.h"
#include "../Module/Module.h"
#include "../Outcome/Status.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "TaggedUnion.h"
#include "Traits.h"
#include "Types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Any Configuration & Size Specialization Tables                          */
/* ========================================================================== */

#define PRIMITIVE_ANY_CONFIG_COL_NAME(NAME, VALUE)  NAME
#define PRIMITIVE_ANY_CONFIG_COL_VALUE(NAME, VALUE) VALUE
#define PRIMITIVE_ANY_CONFIG_GET(FIELD, ROW)                                   \
  PP_OP_CAT2(PRIMITIVE_ANY_CONFIG_COL_, FIELD) ROW

#define PRIMITIVE_ANY_CONFIG_TABLE(X)                                          \
  X((SBO_CAPACITY, 16))

#define PRIMITIVE_ANY_SIZE_COL_NAME(NAME, SBO_BYTES)      NAME
#define PRIMITIVE_ANY_SIZE_COL_SBO_BYTES(NAME, SBO_BYTES) SBO_BYTES
#define PRIMITIVE_ANY_SIZE_GET(FIELD, ROW)                                     \
  PP_OP_CAT2(PRIMITIVE_ANY_SIZE_COL_, FIELD) ROW

#define PRIMITIVE_ANY_SIZE_TABLE(X)                                            \
  X((Any32,  32))                                                              \
  X((Any64,  64))                                                              \
  X((Any128, 128))                                                             \
  X((Any256, 256))

/* ========================================================================== */
/* 2. Declarative Module: Primitive (AnyStorage, TAny, AnyConversion)         */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_ANY_PRIMITIVE_API(ROW)                         \
  OPERATION(PP_OP_CAT2(Set_, PRIMITIVE_TYPE_GET(NAME, ROW)),                   \
            (MODULE_TYPE(Primitive, Any) * self,                               \
             PRIMITIVE_TYPE_GET(TYPE, ROW) value)),                            \
  OPERATION(PP_OP_CAT2(Get_, PRIMITIVE_TYPE_GET(NAME, ROW)),                   \
            (const MODULE_TYPE(Primitive, Any) * self,                         \
             PRIMITIVE_TYPE_GET(TYPE, ROW) * outValue)),

MODULE(DOMAIN(Core), Primitive,
    /* 2.1 Internal SBO / Heap Storage Union (Default 16B) */
    ISTRUCT(AnyStorage,
        FIELDS(
            union {
                void *heapPointer;
                max_align_t alignDummy;
                unsigned char inlineBytes[16];
            } as;
        )
    ),

    /* 2.2 Canonical Pure POD Type-Erased Box (TAny, 16B SBO) */
    STRUCT(Any,
        FIELDS(
            const MODULE_TYPE(Primitive, Traits) *traits;
            MODULE_ITYPE(Primitive, AnyStorage) storage;
        )
    ),

    /* 2.3 Package: Primitive_AnyConversion (Shared Engine & Cross-Size Conversion) */
    PACKAGE(AnyConversion,
        PREDICATE(FitsInline, (const MODULE_TYPE(Primitive, Traits) * traits,
                               size_t sboCapacity)),
        FUNCTION(const void *, RawPointerOf,
                 (const MODULE_TYPE(Primitive, Traits) * traits,
                  void *heapPointer, const unsigned char *inlineBytes,
                  size_t sboCapacity)),
        FUNCTION(void *, MutableRawPointerOf,
                 (const MODULE_TYPE(Primitive, Traits) * traits,
                  void *heapPointer, unsigned char *inlineBytes,
                  size_t sboCapacity)),
        PROCEDURE(DisposeRaw,
                  (const MODULE_TYPE(Primitive, Traits) **traitsSlot,
                   void **heapSlot, unsigned char *inlineBytes,
                   size_t sboCapacity)),
        OPERATION(EmplaceRaw,
                  (const MODULE_TYPE(Primitive, Traits) **traitsSlot,
                   void **heapSlot, unsigned char *inlineBytes,
                   size_t sboCapacity,
                   const MODULE_TYPE(Primitive, Traits) * traits,
                   const void *sourceInstance)),
        OPERATION(MoveFromRaw,
                  (const MODULE_TYPE(Primitive, Traits) **traitsSlot,
                   void **heapSlot, unsigned char *inlineBytes,
                   size_t sboCapacity,
                   const MODULE_TYPE(Primitive, Traits) * traits,
                   void *sourceInstance)),
        OPERATION(Clone,
                  (const MODULE_TYPE(Primitive, Traits) **destTraits,
                   void **destHeap, unsigned char *destInline, size_t destSbo,
                   const MODULE_TYPE(Primitive, Traits) *const *srcTraits,
                   void *const *srcHeap, const unsigned char *srcInline,
                   size_t srcSbo)),
        OPERATION(Move,
                  (const MODULE_TYPE(Primitive, Traits) **destTraits,
                   void **destHeap, unsigned char *destInline, size_t destSbo,
                   const MODULE_TYPE(Primitive, Traits) **srcTraits,
                   void **srcHeap, unsigned char *srcInline, size_t srcSbo)),
        OPERATION(SwapRaw,
                  (const MODULE_TYPE(Primitive, Traits) **leftTraits,
                   void **leftHeap, unsigned char *leftInline, size_t leftSbo,
                   const MODULE_TYPE(Primitive, Traits) **rightTraits,
                   void **rightHeap, unsigned char *rightInline,
                   size_t rightSbo)),
        OPERATION(FromTaggedUnionRaw,
                  (const MODULE_TYPE(Primitive, Traits) **traitsSlot,
                   void **heapSlot, unsigned char *inlineBytes,
                   size_t sboCapacity,
                   const MODULE_TYPE(Primitive, TaggedUnion) * source)),
        OPERATION(ToTaggedUnionRaw,
                  (const MODULE_TYPE(Primitive, Traits) * traits,
                   void *heapPointer, const unsigned char *inlineBytes,
                   size_t sboCapacity,
                   MODULE_TYPE(Primitive, TaggedUnion) * outUnion)),
        COMPARISON(CompareRaw,
                   (const MODULE_TYPE(Primitive, Traits) * leftTraits,
                    void *leftHeap, const unsigned char *leftInline,
                    size_t leftSbo,
                    const MODULE_TYPE(Primitive, Traits) * rightTraits,
                    void *rightHeap, const unsigned char *rightInline,
                    size_t rightSbo)),
        PREDICATE(EqualsRaw,
                  (const MODULE_TYPE(Primitive, Traits) * leftTraits,
                   void *leftHeap, const unsigned char *leftInline,
                   size_t leftSbo,
                   const MODULE_TYPE(Primitive, Traits) * rightTraits,
                   void *rightHeap, const unsigned char *rightInline,
                   size_t rightSbo)),
        FUNCTION(size_t, HashRaw,
                 (const MODULE_TYPE(Primitive, Traits) * traits,
                  void *heapPointer, const unsigned char *inlineBytes,
                  size_t sboCapacity))
    ),

    /* 2.4 Package: Primitive_Any */
    PACKAGE(Any,
        ENUM(Config, PRIMITIVE_ANY_CONFIG_TABLE),

        /* Lifecycle */
        PROCEDURE(Init, (MODULE_TYPE(Primitive, Any) * self)),
        PROCEDURE(Reset, (MODULE_TYPE(Primitive, Any) * self)),
        PROCEDURE(Dispose, (MODULE_TYPE(Primitive, Any) * self)),

        /* Construction, Cloning, Moving & Swapping */
        OPERATION(Emplace, (MODULE_TYPE(Primitive, Any) * self,
                            const MODULE_TYPE(Primitive, Traits) * traits,
                            const void *sourceInstance)),
        OPERATION(MoveFrom, (MODULE_TYPE(Primitive, Any) * self,
                             const MODULE_TYPE(Primitive, Traits) * traits,
                             void *sourceInstance)),
        OPERATION(Clone, (MODULE_TYPE(Primitive, Any) * destination,
                          const MODULE_TYPE(Primitive, Any) * source)),
        OPERATION(Move, (MODULE_TYPE(Primitive, Any) * destination,
                         MODULE_TYPE(Primitive, Any) * source)),
        OPERATION(Swap, (MODULE_TYPE(Primitive, Any) * left,
                         MODULE_TYPE(Primitive, Any) * right)),

        /* Introspection */
        PREDICATE(HasValue, (const MODULE_TYPE(Primitive, Any) * self)),
        PREDICATE(IsEmpty, (const MODULE_TYPE(Primitive, Any) * self)),
        PREDICATE(IsInline, (const MODULE_TYPE(Primitive, Any) * self)),
        PREDICATE(IsType, (const MODULE_TYPE(Primitive, Any) * self,
                           const MODULE_TYPE(Primitive, Traits) * expectedTraits)),
        FUNCTION(const MODULE_TYPE(Primitive, Traits) *, TraitsOf,
                 (const MODULE_TYPE(Primitive, Any) * self)),
        FUNCTION(size_t, Capacity, (void)),

        /* Type-Safe Extraction & Borrowing */
        FUNCTION(const void *, RawPointer,
                 (const MODULE_TYPE(Primitive, Any) * self)),
        FUNCTION(void *, MutableRawPointer,
                 (MODULE_TYPE(Primitive, Any) * self)),
        OPERATION(Get, (const MODULE_TYPE(Primitive, Any) * self,
                        const MODULE_TYPE(Primitive, Traits) * expectedTraits,
                        void *outValue)),
        OPERATION(Borrow, (const MODULE_TYPE(Primitive, Any) * self,
                           const MODULE_TYPE(Primitive, Traits) * expectedTraits,
                           const void **outPointer)),
        OPERATION(BorrowMutable,
                  (MODULE_TYPE(Primitive, Any) * self,
                   const MODULE_TYPE(Primitive, Traits) * expectedTraits,
                   void **outPointer)),

        /* Typed Primitive Shortcuts */
        PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_ANY_PRIMITIVE_API)

        /* TaggedUnion Interoperability */
        OPERATION(FromTaggedUnion,
                  (MODULE_TYPE(Primitive, Any) * self,
                   const MODULE_TYPE(Primitive, TaggedUnion) * source)),
        OPERATION(ToTaggedUnion,
                  (const MODULE_TYPE(Primitive, Any) * self,
                   MODULE_TYPE(Primitive, TaggedUnion) * outUnion)),

        /* Comparison, Equality, Hashing & Self-Traits */
        COMPARISON(Compare, (const MODULE_TYPE(Primitive, Any) * left,
                             const MODULE_TYPE(Primitive, Any) * right)),
        PREDICATE(Equals, (const MODULE_TYPE(Primitive, Any) * left,
                           const MODULE_TYPE(Primitive, Any) * right)),
        FUNCTION(size_t, Hash, (const MODULE_TYPE(Primitive, Any) * self)),
        FUNCTION(const MODULE_TYPE(Primitive, Traits) *, Traits, (void))
    )
);

#undef COSMERON_MACRO_INTERNAL_ANY_PRIMITIVE_API

/* ========================================================================== */
/* 3. Cross-Size Any Conversion Utilities                                     */
/* ========================================================================== */

#define Primitive_Any_Conversion_Clone(destPtr, srcPtr)                        \
  MODULE_FUNC(Primitive, AnyConversion, Clone)(                                \
      &(destPtr)->traits, &(destPtr)->storage.as.heapPointer,                  \
      (destPtr)->storage.as.inlineBytes,                                       \
      sizeof((destPtr)->storage.as.inlineBytes),                               \
      &(srcPtr)->traits, &(srcPtr)->storage.as.heapPointer,                    \
      (srcPtr)->storage.as.inlineBytes,                                        \
      sizeof((srcPtr)->storage.as.inlineBytes))

#define Primitive_Any_Conversion_Move(destPtr, srcPtr)                         \
  MODULE_FUNC(Primitive, AnyConversion, Move)(                                 \
      &(destPtr)->traits, &(destPtr)->storage.as.heapPointer,                  \
      (destPtr)->storage.as.inlineBytes,                                       \
      sizeof((destPtr)->storage.as.inlineBytes),                               \
      &(srcPtr)->traits, &(srcPtr)->storage.as.heapPointer,                    \
      (srcPtr)->storage.as.inlineBytes,                                        \
      sizeof((srcPtr)->storage.as.inlineBytes))

/* ========================================================================== */
/* 4. Implementation Include & SBO Size Specializations                       */
/* ========================================================================== */

#include "Impl/Any.impl"
#include "Any.def"

#define COSMERON_MACRO_INTERNAL_ANY_SIZE_INSTANTIATE(ROW)                      \
  TEMPLATE_Primitive_Any(PRIMITIVE_ANY_SIZE_GET(NAME, ROW),                    \
                         PRIMITIVE_ANY_SIZE_GET(SBO_BYTES, ROW));

PRIMITIVE_ANY_SIZE_TABLE(COSMERON_MACRO_INTERNAL_ANY_SIZE_INSTANTIATE)
#undef COSMERON_MACRO_INTERNAL_ANY_SIZE_INSTANTIATE

/* EOF */
