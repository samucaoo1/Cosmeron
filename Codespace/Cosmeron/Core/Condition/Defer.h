#pragma once

#include "../Module/Closure/Closure.h"
#include "../Module/Module.h"
#include "../Preprocessor/Detect.h"
#include "../Preprocessor/Operations.h"
#include "../Primitive/Traits.h"
#include "Exception.h"

#include <stdbool.h>
#include <stddef.h>

/* ========================================================================== */
/* 1. Declarative Module: Condition (DeferNode, DeferTraitsTarget, Package)   */
/* ========================================================================== */

MODULE(DOMAIN(Core), Condition,
    CLOSURE(
        PROCEDURE(DeferAction, ())
    ),

    STRUCT(DeferNode,
        FIELDS(
            MODULE_CLOSURE(Condition, DeferAction) action;
            struct MODULE_TYPE(Condition, DeferNode) *previous;
            bool active;
        )
    ),

    STRUCT(DeferTraitsTarget,
        FIELDS(
            const MODULE_TYPE(Primitive, Traits) *traits;
            void *instance;
        )
    ),

    PACKAGE(Defer,
        PROCEDURE_INLINE(Cancel, (MODULE_TYPE(Condition, DeferNode) *node), {
          if (node != NULL) {
            node->active = false;
          }
        }),

        CONST_PREDICATE_INLINE(IsActive,
                               (const MODULE_TYPE(Condition, DeferNode) *node), {
          return node != NULL && node->active;
        }),

        PROCEDURE_INLINE(Execute, (MODULE_TYPE(Condition, DeferNode) *node), {
          if (node != NULL && node->active && node->action.function != NULL) {
            node->active = false;
            node->action.function(node->action.context);
          }
        }),

        PROCEDURE_INLINE(DisposeTraitsAdapter, (void *context), {
          MODULE_TYPE(Condition, DeferTraitsTarget) *target =
              (MODULE_TYPE(Condition, DeferTraitsTarget) *)context;
          if (target != NULL && target->traits != NULL &&
              target->instance != NULL) {
            MODULE_FUNC(Primitive, Traits, Dispose)(target->traits,
                                                    target->instance);
          }
        })
    ),

    INTERNAL_EX(Defer,
        PROCEDURE_INLINE(UnwindTo, (void *watermark), {
          MODULE_TYPE(Condition, DeferNode) *target =
              (MODULE_TYPE(Condition, DeferNode) *)watermark;
          while (MODULE_IFUNC(Condition, Exception_DeferWatermark) != NULL &&
                 MODULE_IFUNC(Condition, Exception_DeferWatermark) != target) {
            MODULE_TYPE(Condition, DeferNode) *current =
                (MODULE_TYPE(Condition, DeferNode) *)
                    MODULE_IFUNC(Condition, Exception_DeferWatermark);
            MODULE_IFUNC(Condition, Exception_DeferWatermark) = current->previous;
            MODULE_FUNC(Condition, Defer, Execute)(current);
          }
        }),

        FUNCTION_INLINE(MODULE_TYPE(Condition, DeferNode) *, LinkNode,
                        (MODULE_TYPE(Condition, DeferNode) *node), {
          MODULE_TYPE(Condition, DeferNode) *previous =
              (MODULE_TYPE(Condition, DeferNode) *)
                  MODULE_IFUNC(Condition, Exception_DeferWatermark);
          MODULE_IFUNC(Condition, Exception_DeferWatermark) = node;
          MODULE_IFUNC(Condition, Exception_DeferUnwind) =
              MODULE_IFUNC(Condition, Defer, UnwindTo);
          return previous;
        }),

        PROCEDURE_INLINE(CleanupNode, (MODULE_TYPE(Condition, DeferNode) *node), {
          if (node == NULL) {
            return;
          }
          if (MODULE_IFUNC(Condition, Exception_DeferWatermark) == node) {
            MODULE_IFUNC(Condition, Exception_DeferWatermark) = node->previous;
          }
          MODULE_FUNC(Condition, Defer, Execute)(node);
        })
    )
);

/* ========================================================================== */
/* 2. Portable Scope-Based Defer Macros (ISO C11)                             */
/* ========================================================================== */

#define COSMERON_MACRO_INTERNAL_DEFER_SCOPE_IMPL(STEP_VAR, ONCE_VAR,           \
                                                 EXIT_ACTION)                  \
  for (int STEP_VAR = 0; STEP_VAR < 2; ++STEP_VAR)                             \
    if (STEP_VAR == 1) {                                                       \
      EXIT_ACTION;                                                             \
    } else                                                                     \
      for (int ONCE_VAR = 0; !ONCE_VAR; ONCE_VAR = 1)

#define DEFER_SCOPE(EXIT_ACTION)                                               \
  COSMERON_MACRO_INTERNAL_DEFER_SCOPE_IMPL(                                    \
      PP_OP_CAT2(_cosm_defer_step_, __LINE__),                                 \
      PP_OP_CAT2(_cosm_defer_once_, __LINE__), EXIT_ACTION)

#define COSMERON_MACRO_INTERNAL_DEFER_TRY_SCOPE_IMPL(FRAME_VAR, STEP_VAR,      \
                                                     ONCE_VAR, EXIT_ACTION)    \
  for (MODULE_TYPE(Condition, ExceptionFrame) FRAME_VAR = {0};                 \
       FRAME_VAR.step == 0;                                                    \
       MODULE_IFUNC(Condition, Exception, Finish)(&FRAME_VAR))                 \
    for (int STEP_VAR = MODULE_IFUNC(Condition, Exception, Begin)(             \
             &FRAME_VAR, setjmp(FRAME_VAR.env));                               \
         STEP_VAR < 3;                                                         \
         STEP_VAR = MODULE_IFUNC(Condition, Exception, NextStep)(&FRAME_VAR,   \
                                                                 STEP_VAR))    \
      if (STEP_VAR == 2) {                                                     \
        EXIT_ACTION;                                                           \
      } else if (STEP_VAR == 0)                                                \
        for (int ONCE_VAR = 0; !ONCE_VAR; ONCE_VAR = 1)

#define DEFER_TRY_SCOPE(EXIT_ACTION)                                           \
  COSMERON_MACRO_INTERNAL_DEFER_TRY_SCOPE_IMPL(                                \
      PP_OP_CAT2(_cosm_defer_frame_, __LINE__),                                \
      PP_OP_CAT2(_cosm_defer_tstep_, __LINE__),                                \
      PP_OP_CAT2(_cosm_defer_tonce_, __LINE__), EXIT_ACTION)

/* ========================================================================== */
/* 3. Linear Scope-Exit Defer Macros (GCC / Clang Cleanup Attribute)          */
/* ========================================================================== */

#if COMPILER_GCC || COMPILER_CLANG

#define COSMERON_MACRO_INTERNAL_DEFER_CLEANUP(FUNC)                            \
  __attribute__((cleanup(FUNC)))

#define DEFER_NAMED(VAR_NAME, FUNC, CONTEXT)                                   \
  MODULE_TYPE(Condition, DeferNode) VAR_NAME                                   \
      COSMERON_MACRO_INTERNAL_DEFER_CLEANUP(                                   \
          MODULE_IFUNC(Condition, Defer, CleanupNode)) = {                     \
          .action = {.function = (FUNC), .context = (CONTEXT)},                \
          .previous = MODULE_IFUNC(Condition, Defer, LinkNode)(&VAR_NAME),     \
          .active = true};                                                     \
  (void)VAR_NAME

#define DEFER(FUNC, CONTEXT)                                                   \
  DEFER_NAMED(PP_OP_CAT2(_cosm_defer_node_, __LINE__), (FUNC), (CONTEXT))

#define DEFER_TRAITS_NAMED(VAR_NAME, TRAITS_PTR, INSTANCE_PTR)                 \
  MODULE_TYPE(Condition, DeferTraitsTarget)                                    \
      PP_OP_CAT2(VAR_NAME, _target) = {                                        \
          .traits = (TRAITS_PTR),                                              \
          .instance = (INSTANCE_PTR),                                          \
      };                                                                       \
  DEFER_NAMED(VAR_NAME,                                                        \
              MODULE_FUNC(Condition, Defer, DisposeTraitsAdapter),             \
              &PP_OP_CAT2(VAR_NAME, _target))

#define DEFER_TRAITS(TRAITS_PTR, INSTANCE_PTR)                                 \
  DEFER_TRAITS_NAMED(PP_OP_CAT2(_cosm_defer_traits_, __LINE__),                \
                     (TRAITS_PTR), (INSTANCE_PTR))

#define DEFER_CANCEL(VAR_NAME)                                                 \
  MODULE_FUNC(Condition, Defer, Cancel)(&(VAR_NAME))

#endif

/* EOF */

