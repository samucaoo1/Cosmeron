#pragma once

#include "../Outcome/Status.h"
#include "../Module/Package.h"
#include "../Preprocessor/Compiling.inc"
#include "../Primitive/Types.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* 1. Package Declaration: Algorithm_Repeat                                   */
/* ========================================================================== */

#define REPEAT_PRIMITIVE_DECL(ROW)                                             \
  OPERATION(PRIMITIVE_TYPE_GET(NAME, ROW),                                     \
            (PRIMITIVE_TYPE_GET(TYPE, ROW) * destination,                      \
             PRIMITIVE_TYPE_GET(TYPE, ROW) value, size_t count)),

PACKAGE(
    Algorithm_Repeat,
    CALLBACK(PROCEDURE(Procedure, ())),
    CALLBACK(PROCEDURE(IndexedProcedure, (size_t index))),
    CALLBACK(OPERATION(Operation, ())),
    CALLBACK(OPERATION(IndexedOperation, (size_t index))),
    CALLBACK(PREDICATE(Predicate, ())),
    CALLBACK(PREDICATE(IndexedPredicate, (size_t index))),
    PROCEDURE(Procedure,
              (size_t count, TCALLBACK(NS(Algorithm_Repeat, Procedure)) action)),
    PROCEDURE(IndexedProcedure,
              (size_t count,
               TCALLBACK(NS(Algorithm_Repeat, IndexedProcedure)) action)),
    OPERATION(Operation,
              (size_t count,
               TCALLBACK(NS(Algorithm_Repeat, Operation)) operation)),
    OPERATION(IndexedOperation,
              (size_t count,
               TCALLBACK(NS(Algorithm_Repeat, IndexedOperation)) operation)),
    PREDICATE(Predicate,
              (size_t count,
               TCALLBACK(NS(Algorithm_Repeat, Predicate)) predicate)),
    PREDICATE(IndexedPredicate,
              (size_t count,
               TCALLBACK(NS(Algorithm_Repeat, IndexedPredicate)) predicate)),
    OPERATION(Bytes, (void *destination, const void *pattern,
                      size_t patternSize, size_t count)),
    PRIMITIVE_TYPE_TABLE(REPEAT_PRIMITIVE_DECL)
    OPERATION(CString,
              (const char **destination, const char *value, size_t count))
);

#undef REPEAT_PRIMITIVE_DECL

/* ========================================================================== */
/* 2. Universal Template Definition: REPEAT                                   */
/* ========================================================================== */

#define TEMPLATE_REPEAT(SUFFIX, TYPE)                                          \
  PACKAGE(                                                                     \
      NS(Algorithm_Repeat, SUFFIX),                                            \
      CALLBACK(FUNCTION(TYPE, Generator, ())),                                 \
      CALLBACK(FUNCTION(TYPE, IndexedGenerator, (size_t index))),              \
      OPERATION_INLINE(Fill, (TYPE * destination, TYPE value, size_t count),   \
                       {                                                       \
                         if (count == 0U)                                      \
                           return STATUS(SUCCESS);                             \
                         if (!destination)                                     \
                           return STATUS(INVALID_ARGUMENT);                    \
                         if (sizeof(TYPE) != 0U &&                             \
                             count > SIZE_MAX / sizeof(TYPE))                  \
                           return STATUS(ARITHMETIC_OVERFLOW);                 \
                         for (size_t i = 0U; i < count; ++i) {                 \
                           destination[i] = value;                             \
                         }                                                     \
                         return STATUS(SUCCESS);                               \
                       }),                                                     \
      OPERATION_INLINE(                                                        \
          Generate,                                                            \
          (TYPE * destination, size_t count,                                   \
           TCALLBACK(NS(Algorithm_Repeat, SUFFIX, Generator)) generator),      \
          {                                                                    \
            if (count == 0U)                                                   \
              return STATUS(SUCCESS);                                          \
            if (!destination || !CALLBACK_IS_VALID(generator))                 \
              return STATUS(INVALID_ARGUMENT);                                 \
            if (sizeof(TYPE) != 0U && count > SIZE_MAX / sizeof(TYPE))         \
              return STATUS(ARITHMETIC_OVERFLOW);                              \
            for (size_t i = 0U; i < count; ++i) {                              \
              destination[i] = CALLBACK_CALL(generator);                       \
            }                                                                  \
            return STATUS(SUCCESS);                                            \
          }),                                                                  \
      OPERATION_INLINE(                                                        \
          GenerateIndexed,                                                     \
          (TYPE * destination, size_t count,                                   \
           TCALLBACK(NS(Algorithm_Repeat, SUFFIX, IndexedGenerator))           \
               generator),                                                     \
          {                                                                    \
            if (count == 0U)                                                   \
              return STATUS(SUCCESS);                                          \
            if (!destination || !CALLBACK_IS_VALID(generator))                 \
              return STATUS(INVALID_ARGUMENT);                                 \
            if (sizeof(TYPE) != 0U && count > SIZE_MAX / sizeof(TYPE))         \
              return STATUS(ARITHMETIC_OVERFLOW);                              \
            for (size_t i = 0U; i < count; ++i) {                              \
              destination[i] = CALLBACK_CALL(generator, i);                    \
            }                                                                  \
            return STATUS(SUCCESS);                                            \
          }))

#define TEMPLATE_REPEAT_INLINE_ONLY 1
#define TEMPLATE_REPEAT_IMPLEMENT(SUFFIX, TYPE) /* empty */

#define TEMPLATE_Repeat(SUFFIX, TYPE) TEMPLATE_REPEAT(SUFFIX, TYPE)
#define TEMPLATE_Repeat_INLINE_ONLY 1
#define TEMPLATE_Repeat_IMPLEMENT(SUFFIX, TYPE) /* empty */

#include "Impl/Repeat.impl"
/* EOF */
