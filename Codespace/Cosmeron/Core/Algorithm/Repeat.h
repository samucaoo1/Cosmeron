#pragma once

#include "../Condition/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "../Types/Primitive.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Module Declaration: Algorithm (Package Repeat + Template Package Repeat)*/
/* ========================================================================== */

#if defined(ALGORITHM_DISABLE_FUNCTION_TABLE) &&                               \
    !defined(COSMERON_DISABLE_FT_Algorithm)
#define COSMERON_DISABLE_FT_Algorithm 1
#endif
#if defined(ALGORITHM_REPEAT_DISABLE_FUNCTION_TABLE) &&                       \
    !defined(COSMERON_DISABLE_FT_Algorithm_Repeat)
#define COSMERON_DISABLE_FT_Algorithm_Repeat 1
#endif

#define COSMERON_MACRO_INTERNAL_REPEAT_PRIMITIVE_DECL(ROW)                     \
  OPERATION(PRIMITIVE_TYPE_GET(NAME, ROW),                                     \
            (PRIMITIVE_TYPE_GET(C_TYPE, ROW) * destination,                      \
             PRIMITIVE_TYPE_GET(C_TYPE, ROW) value, size_t count)),

MODULE(DOMAIN(Core), Algorithm,

    /* 1.1 Stateless Package: Algorithm_Repeat */
    PACKAGE(Repeat,
        DELEGATE(
            PROCEDURE(Procedure, ()),
            PROCEDURE(ProcedureIndexed, (size_t index)),
            OPERATION(Operation, ()),
            OPERATION(OperationIndexed, (size_t index)),
            PREDICATE(Predicate, ()),
            PREDICATE(PredicateIndexed, (size_t index))
        ),

        VARIANT(Procedure,
            PROCEDURE((size_t count,
                       MODULE_DELEGATE(Algorithm, Repeat, Procedure) action)),
            PROCEDURE(Indexed,
                       (size_t count,
                        MODULE_DELEGATE(Algorithm, Repeat, ProcedureIndexed) action))
        ),

        VARIANT(Operation,
            OPERATION((size_t count,
                       MODULE_DELEGATE(Algorithm, Repeat, Operation) operation)),
            OPERATION(Indexed,
                       (size_t count,
                        MODULE_DELEGATE(Algorithm, Repeat, OperationIndexed) operation))
        ),

        VARIANT(Predicate,
            PREDICATE((size_t count,
                       MODULE_DELEGATE(Algorithm, Repeat, Predicate) predicate)),
            PREDICATE(Indexed,
                       (size_t count,
                        MODULE_DELEGATE(Algorithm, Repeat, PredicateIndexed) predicate))
        ),

        OPERATION(Bytes, (void *destination, const void *pattern,
                          size_t patternSize, size_t count)),
        PRIMITIVE_TYPE_TABLE(COSMERON_MACRO_INTERNAL_REPEAT_PRIMITIVE_DECL)
        OPERATION(CString,
                  (const char **destination, const char *value, size_t count))
    )
);

#undef COSMERON_MACRO_INTERNAL_REPEAT_PRIMITIVE_DECL

/* ========================================================================== */
/* 2. Template Package: Algorithm_Repeat                                      */
/* ========================================================================== */

#include "Repeat.def"

/* ========================================================================== */
/* 3. Implementation Include                                                  */
/* ========================================================================== */

#include "Impl/Repeat.impl"

/* EOF */
