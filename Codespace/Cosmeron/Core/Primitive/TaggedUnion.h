#pragma once

#include "../Algorithm/Comparison.h"
#include "../Module/Module.h"
#include "../Outcome/Status.h"
#include "../Preprocessor/Compiling.inc"
#include "../Preprocessor/Operations.h"
#include "Traits.h"
#include "Types.h"
#include "TypeTable/Tag.impl"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Declarative Module: Primitive (Tag, ByteSlice, TaggedUnion, Package)    */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_TAGGED_UNION_FIELD(ROW)                        \
  PRIMITIVE_TYPE_GET(TYPE, ROW) PRIMITIVE_TYPE_GET(NAME, ROW);

#define COSMERON_MACRO_INTERNAL_TAGGED_UNION_PRIMITIVE_API(ROW)                \
  OPERATION(PP_OP_CAT2(Set_, PRIMITIVE_TYPE_GET(NAME, ROW)),                   \
            (MODULE_TYPE(Primitive, TaggedUnion) * self,                       \
             PRIMITIVE_TYPE_GET(TYPE, ROW) value)),                            \
  FUNCTION(MODULE_TYPE(Primitive, TaggedUnion),                                \
           PP_OP_CAT2(From_, PRIMITIVE_TYPE_GET(NAME, ROW)),                   \
           (PRIMITIVE_TYPE_GET(TYPE, ROW) value)),                             \
  OPERATION(PP_OP_CAT2(Get_, PRIMITIVE_TYPE_GET(NAME, ROW)),                   \
            (const MODULE_TYPE(Primitive, TaggedUnion) * self,                 \
             PRIMITIVE_TYPE_GET(TYPE, ROW) * outValue)),

MODULE(DOMAIN(Core), Primitive,
    /* 1.1 Closed Tag Enum */
    ENUM_EX(Tag, PRIMITIVE_TAG_TABLE, QUALIFIED_NAME),

    /* 1.2 Contiguous Byte Slice POD */
    STRUCT(ByteSlice,
        FIELDS(
            const void *data;
            size_t size;
        )
    ),

    /* 1.3 Internal Tagged Payload Union */
    ISTRUCT(TaggedPayload,
        FIELDS(
            union {
                PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_TAGGED_UNION_FIELD)
                const char *cString;
                void *rawPointer;
                MODULE_TYPE(Primitive, ByteSlice) bytes;
            } as;
        )
    ),

    /* 1.4 Canonical Pure POD TaggedUnion (TTaggedUnion) */
    STRUCT(TaggedUnion,
        FIELDS(
            MODULE_ENUM(Primitive, Tag) tag;
            MODULE_ITYPE(Primitive, TaggedPayload) value;
        )
    ),

    /* 1.5 Package: Primitive_TaggedUnion */
    PACKAGE(TaggedUnion,
        /* Lifecycle */
        PROCEDURE(Init, (MODULE_TYPE(Primitive, TaggedUnion) * self)),
        PROCEDURE(Clear, (MODULE_TYPE(Primitive, TaggedUnion) * self)),

        /* Typed Setters, Value Constructors & Strict Getters (All Primitives) */
        PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_TAGGED_UNION_PRIMITIVE_API)

        /* Special Value Setters, Constructors & Getters */
        OPERATION(Set_CString, (MODULE_TYPE(Primitive, TaggedUnion) * self,
                                const char *value)),
        FUNCTION(MODULE_TYPE(Primitive, TaggedUnion), From_CString,
                 (const char *value)),
        OPERATION(Get_CString, (const MODULE_TYPE(Primitive, TaggedUnion) * self,
                                const char **outValue)),

        OPERATION(Set_Bytes, (MODULE_TYPE(Primitive, TaggedUnion) * self,
                              const void *data, size_t size)),
        FUNCTION(MODULE_TYPE(Primitive, TaggedUnion), From_Bytes,
                 (const void *data, size_t size)),
        OPERATION(Get_Bytes, (const MODULE_TYPE(Primitive, TaggedUnion) * self,
                              MODULE_TYPE(Primitive, ByteSlice) * outValue)),

        OPERATION(Set_RawPointer, (MODULE_TYPE(Primitive, TaggedUnion) * self,
                                   void *pointer)),
        FUNCTION(MODULE_TYPE(Primitive, TaggedUnion), From_RawPointer,
                 (void *pointer)),
        OPERATION(Get_RawPointer,
                  (const MODULE_TYPE(Primitive, TaggedUnion) * self,
                   void **outValue)),

        /* Safe Numeric Coercions (Overflow-Checked) */
        OPERATION(CoerceInt64, (const MODULE_TYPE(Primitive, TaggedUnion) * self,
                                int64_t *outValue)),
        OPERATION(CoerceUInt64, (const MODULE_TYPE(Primitive, TaggedUnion) * self,
                                 uint64_t *outValue)),
        OPERATION(CoerceFloat64, (const MODULE_TYPE(Primitive, TaggedUnion) * self,
                                  double *outValue)),

        /* Introspection */
        PREDICATE(IsEmpty, (const MODULE_TYPE(Primitive, TaggedUnion) * self)),
        PREDICATE(IsTag, (const MODULE_TYPE(Primitive, TaggedUnion) * self,
                          MODULE_ENUM(Primitive, Tag) tag)),
        PREDICATE(IsNumeric, (const MODULE_TYPE(Primitive, TaggedUnion) * self)),
        PREDICATE(IsInteger, (const MODULE_TYPE(Primitive, TaggedUnion) * self)),
        PREDICATE(IsSigned, (const MODULE_TYPE(Primitive, TaggedUnion) * self)),
        PREDICATE(IsFloat, (const MODULE_TYPE(Primitive, TaggedUnion) * self)),
        FUNCTION(const char *, TagName, (MODULE_ENUM(Primitive, Tag) tag)),
        FUNCTION(MODULE_ENUM(Primitive, Category), CategoryOf,
                 (const MODULE_TYPE(Primitive, TaggedUnion) * self)),
        FUNCTION(const MODULE_TYPE(Primitive, Traits) *, TraitsOf,
                 (const MODULE_TYPE(Primitive, TaggedUnion) * self)),
        FUNCTION(const void *, PayloadPointer,
                 (const MODULE_TYPE(Primitive, TaggedUnion) * self)),

        /* Comparison, Equality, Hashing & Self-Traits */
        COMPARISON(Compare, (const MODULE_TYPE(Primitive, TaggedUnion) * left,
                             const MODULE_TYPE(Primitive, TaggedUnion) * right)),
        PREDICATE(Equals, (const MODULE_TYPE(Primitive, TaggedUnion) * left,
                           const MODULE_TYPE(Primitive, TaggedUnion) * right)),
        FUNCTION(size_t, Hash, (const MODULE_TYPE(Primitive, TaggedUnion) * self)),
        FUNCTION(const MODULE_TYPE(Primitive, Traits) *, Traits, (void))
    )
);

#undef COSMERON_MACRO_INTERNAL_TAGGED_UNION_FIELD
#undef COSMERON_MACRO_INTERNAL_TAGGED_UNION_PRIMITIVE_API

/* ========================================================================== */
/* 2. Implementation Include                                                  */
/* ========================================================================== */

#include "Impl/TaggedUnion.impl"

/* EOF */

