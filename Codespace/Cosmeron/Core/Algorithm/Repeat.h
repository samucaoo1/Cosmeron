#pragma once

#include "../Error/Status.h"
#include "../Module/Callback/Callback.h"
#include "../Module/Common/Template.impl"
#include "../Module/Package.h"
#include "../Preprocessor/Compiling.inc"
#include "../Primitive/Types.h"
#include "Algorithm.space"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define REPEAT_MOD Repeat
#define REPEAT_CMOD REPEAT

#define REPEAT_NS(NAME) GNS2(ALGORITHM_NS(REPEAT_MOD), NAME)
#define REPEAT_CNS(NAME)                                                       \
  CNS3(LIB_PREFIX_CONST(ALGORITHM_CMOD), REPEAT_CMOD, NAME)
#define REPEAT_INS(NAME) GNS3(ALGORITHM_NS(REPEAT_MOD), Internal, NAME)
#define REPEAT_CINS(NAME) CNS3(ALGORITHM_CNS(REPEAT_CMOD), INTERNAL, NAME)

#define REPEAT_TYPE(NAME) REPEAT_NS(NAME)
#define REPEAT_FUNC(NAME) REPEAT_NS(NAME)
#define REPEAT_CONST(NAME) REPEAT_CNS(NAME)

/* ========================================================================== */
/* 1. Callback Declarations                                                   */
/* ========================================================================== */

CALLBACK_DECLARE_NULLARY_PROCEDURE(RepeatProcedure);
CALLBACK_DECLARE_PROCEDURE(RepeatIndexedProcedure, (size_t index), (index));

CALLBACK_DECLARE_NULLARY_OPERATION(RepeatOperation);
CALLBACK_DECLARE_OPERATION(RepeatIndexedOperation, (size_t index), (index));

CALLBACK_DECLARE_NULLARY_PREDICATE(RepeatPredicate);
CALLBACK_DECLARE_PREDICATE(RepeatIndexedPredicate, (size_t index), (index));

/* ========================================================================== */
/* 2. Package Declaration: Algorithm_Repeat                                   */
/* ========================================================================== */

#define REPEAT_PRIMITIVE_DECL(ROW)                                             \
  OPERATION(PRIMITIVE_TYPE_GET(NAME, ROW),                                     \
            (PRIMITIVE_TYPE_GET(TYPE, ROW) * destination,                      \
             PRIMITIVE_TYPE_GET(TYPE, ROW) value, size_t count)),

PACKAGE(
    ALGORITHM_NS(REPEAT_MOD),
    PROCEDURE(Procedure, (size_t count, TCALLBACK(RepeatProcedure) action)),
    PROCEDURE(IndexedProcedure,
              (size_t count, TCALLBACK(RepeatIndexedProcedure) action)),
    OPERATION(Operation, (size_t count, TCALLBACK(RepeatOperation) operation)),
    OPERATION(IndexedOperation,
              (size_t count, TCALLBACK(RepeatIndexedOperation) operation)),
    PREDICATE(Predicate, (size_t count, TCALLBACK(RepeatPredicate) predicate)),
    PREDICATE(IndexedPredicate,
              (size_t count, TCALLBACK(RepeatIndexedPredicate) predicate)),
    OPERATION(Bytes, (void *destination, const void *pattern,
                      size_t patternSize, size_t count)),
    PRIMITIVE_TYPE_TABLE(REPEAT_PRIMITIVE_DECL)
        OPERATION(CString, (const char **destination, const char *value,
                            size_t count)));

#undef REPEAT_PRIMITIVE_DECL

/* ========================================================================== */
/* 3. Universal Template Definition: REPEAT                                   */
/* ========================================================================== */

#define TEMPLATE_REPEAT(SUFFIX, TYPE)                                          \
  CALLBACK_DECLARE_NULLARY(REPEAT_INS(PP_OP_CAT2(Generator_, SUFFIX)), TYPE);  \
  CALLBACK_DECLARE(REPEAT_INS(PP_OP_CAT2(IndexedGenerator_, SUFFIX)), TYPE,    \
                   (size_t index), (index));                                   \
  PACKAGE(                                                                     \
      GNS2(ALGORITHM_NS(REPEAT_MOD), SUFFIX),                                  \
      OPERATION_INLINE(Fill, (TYPE * destination, TYPE value, size_t count), { \
        if (count == 0U)                                                       \
          return STATUS_CONST(SUCCESS);                                        \
        if (!destination)                                                      \
          return STATUS_CONST(INVALID_ARGUMENT);                               \
        if (sizeof(TYPE) != 0U && count > SIZE_MAX / sizeof(TYPE))             \
          return STATUS_CONST(ARITHMETIC_OVERFLOW);                            \
        for (size_t i = 0U; i < count; ++i) {                                  \
          destination[i] = value;                                              \
        }                                                                      \
        return STATUS_CONST(SUCCESS);                                          \
      }),                                                                      \
      OPERATION_INLINE(                                                        \
          Generate,                                                            \
          (TYPE * destination, size_t count,                                   \
           TCALLBACK(REPEAT_INS(PP_OP_CAT2(Generator_, SUFFIX))) generator),   \
          {                                                                    \
            if (count == 0U)                                                   \
              return STATUS_CONST(SUCCESS);                                    \
            if (!destination ||                                                \
                !GNS2(CALLBACK_FUNC(                                           \
                          REPEAT_INS(PP_OP_CAT2(Generator_, SUFFIX))),         \
                      IsValid)(generator))                                     \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            if (sizeof(TYPE) != 0U && count > SIZE_MAX / sizeof(TYPE))         \
              return STATUS_CONST(ARITHMETIC_OVERFLOW);                        \
            for (size_t i = 0U; i < count; ++i) {                              \
              destination[i] =                                                 \
                  GNS2(CALLBACK_FUNC(                                          \
                           REPEAT_INS(PP_OP_CAT2(Generator_, SUFFIX))),        \
                       Invoke)(generator);                                     \
            }                                                                  \
            return STATUS_CONST(SUCCESS);                                      \
          }),                                                                  \
      OPERATION_INLINE(                                                        \
          GenerateIndexed,                                                     \
          (TYPE * destination, size_t count,                                   \
           TCALLBACK(REPEAT_INS(PP_OP_CAT2(IndexedGenerator_, SUFFIX)))        \
               generator),                                                     \
          {                                                                    \
            if (count == 0U)                                                   \
              return STATUS_CONST(SUCCESS);                                    \
            if (!destination ||                                                \
                !GNS2(CALLBACK_FUNC(                                           \
                          REPEAT_INS(PP_OP_CAT2(IndexedGenerator_, SUFFIX))),  \
                      IsValid)(generator))                                     \
              return STATUS_CONST(INVALID_ARGUMENT);                           \
            if (sizeof(TYPE) != 0U && count > SIZE_MAX / sizeof(TYPE))         \
              return STATUS_CONST(ARITHMETIC_OVERFLOW);                        \
            for (size_t i = 0U; i < count; ++i) {                              \
              destination[i] =                                                 \
                  GNS2(CALLBACK_FUNC(                                          \
                           REPEAT_INS(PP_OP_CAT2(IndexedGenerator_, SUFFIX))), \
                       Invoke)(generator, i);                                  \
            }                                                                  \
            return STATUS_CONST(SUCCESS);                                      \
          }))

#define TEMPLATE_REPEAT_INLINE_ONLY 1
#define TEMPLATE_REPEAT_IMPLEMENT(SUFFIX, TYPE) /* empty */

#include "Impl/Repeat.impl"
/* EOF */

