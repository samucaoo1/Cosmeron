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
        CALLBACK(PROCEDURE(Procedure, ())),
        CALLBACK(PROCEDURE(ProcedureIndexed, (size_t index))),
        CALLBACK(PROCEDURE(IndexedProcedure, (size_t index))),
        CALLBACK(OPERATION(Operation, ())),
        CALLBACK(OPERATION(OperationIndexed, (size_t index))),
        CALLBACK(OPERATION(IndexedOperation, (size_t index))),
        CALLBACK(PREDICATE(Predicate, ())),
        CALLBACK(PREDICATE(PredicateIndexed, (size_t index))),
        CALLBACK(PREDICATE(IndexedPredicate, (size_t index))),

        VARIANT(Procedure,
            PROCEDURE((size_t count,
                       TCALLBACK(MODULE_NS(Algorithm, Repeat, Procedure)) action)),
            PROCEDURE(Indexed,
                       (size_t count,
                        TCALLBACK(MODULE_NS(Algorithm, Repeat, ProcedureIndexed)) action))
        ),

        VARIANT(Operation,
            OPERATION((size_t count,
                       TCALLBACK(MODULE_NS(Algorithm, Repeat, Operation)) operation)),
            OPERATION(Indexed,
                       (size_t count,
                        TCALLBACK(MODULE_NS(Algorithm, Repeat, OperationIndexed)) operation))
        ),

        VARIANT(Predicate,
            PREDICATE((size_t count,
                       TCALLBACK(MODULE_NS(Algorithm, Repeat, Predicate)) predicate)),
            PREDICATE(Indexed,
                       (size_t count,
                        TCALLBACK(MODULE_NS(Algorithm, Repeat, PredicateIndexed)) predicate))
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
