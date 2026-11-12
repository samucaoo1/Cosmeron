#pragma once

#include "../Outcome/Status.h"
#include "../Module/Module.h"
#include "../Preprocessor/Compiling.inc"
#include "../Primitive/Types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Module Declaration: Algorithm (Package Repeat + Template Package Repeat)*/
/* ========================================================================== */

#define REPEAT_PRIMITIVE_DECL(ROW)                                             \
  OPERATION(PRIMITIVE_TYPE_GET(NAME, ROW),                                     \
            (PRIMITIVE_TYPE_GET(TYPE, ROW) * destination,                      \
             PRIMITIVE_TYPE_GET(TYPE, ROW) value, size_t count)),

MODULE(DOMAIN(Core), Algorithm,

    /* 1.1 Stateless Package: Algorithm_Repeat */
    PACKAGE(Repeat,
        CLOSURE(
            PROCEDURE(Procedure, ()),
            PROCEDURE(ProcedureIndexed, (size_t index)),
            PROCEDURE(IndexedProcedure, (size_t index)),
            OPERATION(Operation, ()),
            OPERATION(OperationIndexed, (size_t index)),
            OPERATION(IndexedOperation, (size_t index)),
            PREDICATE(Predicate, ()),
            PREDICATE(PredicateIndexed, (size_t index)),
            PREDICATE(IndexedPredicate, (size_t index))
        ),

        VARIANT(Procedure,
            PROCEDURE((size_t count,
                       MODULE_CLOSURE(Algorithm, Repeat, Procedure) action)),
            PROCEDURE(Indexed,
                       (size_t count,
                        MODULE_CLOSURE(Algorithm, Repeat, ProcedureIndexed) action))
        ),

        VARIANT(Operation,
            OPERATION((size_t count,
                       MODULE_CLOSURE(Algorithm, Repeat, Operation) operation)),
            OPERATION(Indexed,
                       (size_t count,
                        MODULE_CLOSURE(Algorithm, Repeat, OperationIndexed) operation))
        ),

        VARIANT(Predicate,
            PREDICATE((size_t count,
                       MODULE_CLOSURE(Algorithm, Repeat, Predicate) predicate)),
            PREDICATE(Indexed,
                       (size_t count,
                        MODULE_CLOSURE(Algorithm, Repeat, PredicateIndexed) predicate))
        ),

        OPERATION(Bytes, (void *destination, const void *pattern,
                          size_t patternSize, size_t count)),
        PRIMITIVE_TYPE_TABLE(REPEAT_PRIMITIVE_DECL)
        OPERATION(CString,
                  (const char **destination, const char *value, size_t count))
    )
);

#undef REPEAT_PRIMITIVE_DECL

/* ========================================================================== */
/* 2. Template Package: Algorithm_Repeat                                      */
/* ========================================================================== */

#ifndef TEMPLATE_Algorithm
#define TEMPLATE_Algorithm(TARGET, ...) NS(TEMPLATE_Algorithm, TARGET)(__VA_ARGS__)
#endif

#include "Repeat.def"

/* ========================================================================== */
/* 3. Implementation Include                                                  */
/* ========================================================================== */

#include "Impl/Repeat.impl"

/* EOF */
