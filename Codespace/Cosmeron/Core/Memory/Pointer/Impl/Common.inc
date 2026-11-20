#pragma once

#include "../../../Module/Module.h"
#include "../../../Condition/Status.h"
#include "../../Alloc.h"
#include "../../Impl/Common.inc"

#if defined(POINTER_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Pointer)
#define COSMERON_DISABLE_FT_Pointer 1
#endif
#if defined(POINTER_UNIQUE_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Pointer_Unique)
#define COSMERON_DISABLE_FT_Pointer_Unique 1
#endif
#if defined(POINTER_SHARED_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Pointer_Shared)
#define COSMERON_DISABLE_FT_Pointer_Shared 1
#endif
#if defined(POINTER_WEAK_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Pointer_Weak)
#define COSMERON_DISABLE_FT_Pointer_Weak 1
#endif
#if defined(POINTER_INTRUSIVE_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Pointer_Intrusive)
#define COSMERON_DISABLE_FT_Pointer_Intrusive 1
#endif
#if defined(POINTER_ATOMICSHARED_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Pointer_AtomicShared)
#define COSMERON_DISABLE_FT_Pointer_AtomicShared 1
#endif
#if defined(POINTER_ATOMICWEAK_DISABLE_FUNCTION_TABLE) && !defined(COSMERON_DISABLE_FT_Pointer_AtomicWeak)
#define COSMERON_DISABLE_FT_Pointer_AtomicWeak 1
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Internal Control Blocks & Helpers for Shared/Weak Smart Pointers           */
/* ========================================================================== */

MODULE(DOMAIN(Core), Memory,
    ISTRUCT(SharedControlBlock,
        FIELDS(
            size_t strongCount;
            size_t weakCount;
            void *managedPointer;
        )
    ),
    ISTRUCT(SharedControlBlockAtomic,
        FIELDS(
            uintptr_t strongCount;
            uintptr_t weakCount;
            void *managedPointer;
        )
    ),

    INTERNAL(
        OPERATION(SharedControlCreate,
                  (void *rawPtr,
                   MODULE_ITYPE(Memory, SharedControlBlock) **outControl),
                  {
          if (!outControl) {
            return STATUS_CONST(INVALID_ARGUMENT);
          }
          void *cbRaw = NULL;
          OPSTATUS status = MODULE_FUNC(Memory, Alloc, Bytes)(
              &cbRaw, sizeof(MODULE_ITYPE(Memory, SharedControlBlock)));
          if (status != STATUS_CONST(SUCCESS)) {
            return status;
          }
          MODULE_ITYPE(Memory, SharedControlBlock) *cb =
              (MODULE_ITYPE(Memory, SharedControlBlock) *)cbRaw;
          cb->strongCount = 1U;
          cb->weakCount = 0U;
          cb->managedPointer = rawPtr;
          *outControl = cb;
          return STATUS_CONST(SUCCESS);
        }),

        PROCEDURE(SharedControlReleaseStrong,
                  (MODULE_ITYPE(Memory, SharedControlBlock) *control), {
          if (!control) {
            return;
          }
          if (control->strongCount > 0U) {
            control->strongCount--;
          }
          if (control->strongCount == 0U) {
            void *rawPtr = control->managedPointer;
            control->managedPointer = NULL;
            if (rawPtr != NULL) {
              MODULE_FUNC(Memory, Alloc, Free)(&rawPtr);
            }
            if (control->weakCount == 0U) {
              void *cbRaw = (void *)control;
              MODULE_FUNC(Memory, Alloc, Free)(&cbRaw);
            }
          }
        }),

        PROCEDURE(SharedControlRetainStrong,
                  (MODULE_ITYPE(Memory, SharedControlBlock) *control), {
          if (control) {
            control->strongCount++;
          }
        }),

        PROCEDURE(SharedControlRetainWeak,
                  (MODULE_ITYPE(Memory, SharedControlBlock) *control), {
          if (control) {
            control->weakCount++;
          }
        }),

        OPERATION(SharedControlTryUpgrade,
                  (MODULE_ITYPE(Memory, SharedControlBlock) *control,
                   void **outPtr),
                  {
          if (!control || !outPtr || control->strongCount == 0U) {
            return STATUS_CONST(NOT_FOUND);
          }
          control->strongCount++;
          *outPtr = control->managedPointer;
          return STATUS_CONST(SUCCESS);
        }),

        FUNCTION(size_t, SharedControlStrongCount,
                 (const MODULE_ITYPE(Memory, SharedControlBlock) *control), {
          return control ? control->strongCount : 0U;
        }),

        OPERATION(AtomicControlCreate,
                  (void *rawPtr,
                   MODULE_ITYPE(Memory, SharedControlBlockAtomic) **outControl),
                  {
          if (!outControl) {
            return STATUS_CONST(INVALID_ARGUMENT);
          }
          void *cbRaw = NULL;
          OPSTATUS status = MODULE_FUNC(Memory, Alloc, Bytes)(
              &cbRaw, sizeof(MODULE_ITYPE(Memory, SharedControlBlockAtomic)));
          if (status != STATUS_CONST(SUCCESS)) {
            return status;
          }
          MODULE_ITYPE(Memory, SharedControlBlockAtomic) *cb =
              (MODULE_ITYPE(Memory, SharedControlBlockAtomic) *)cbRaw;
          cb->strongCount = 1U;
          cb->weakCount = 0U;
          cb->managedPointer = rawPtr;
          *outControl = cb;
          return STATUS_CONST(SUCCESS);
        }),

        PROCEDURE(AtomicControlReleaseStrong,
                  (MODULE_ITYPE(Memory, SharedControlBlockAtomic) *control), {
          if (!control) {
            return;
          }
          if (__atomic_sub_fetch(&control->strongCount, 1U, __ATOMIC_ACQ_REL) ==
              0U) {
            void *rawPtr = control->managedPointer;
            control->managedPointer = NULL;
            if (rawPtr != NULL) {
              MODULE_FUNC(Memory, Alloc, Free)(&rawPtr);
            }
            if (__atomic_load_n(&control->weakCount, __ATOMIC_ACQUIRE) == 0U) {
              void *cbRaw = (void *)control;
              MODULE_FUNC(Memory, Alloc, Free)(&cbRaw);
            }
          }
        }),

        PROCEDURE(AtomicControlRetainStrong,
                  (MODULE_ITYPE(Memory, SharedControlBlockAtomic) *control), {
          if (control) {
            __atomic_add_fetch(&control->strongCount, 1U, __ATOMIC_RELAXED);
          }
        }),

        PROCEDURE(AtomicControlRetainWeak,
                  (MODULE_ITYPE(Memory, SharedControlBlockAtomic) *control), {
          if (control) {
            __atomic_add_fetch(&control->weakCount, 1U, __ATOMIC_RELAXED);
          }
        }),

        OPERATION(AtomicControlTryUpgrade,
                  (MODULE_ITYPE(Memory, SharedControlBlockAtomic) *control,
                   void **outPtr),
                  {
          if (!control || !outPtr) {
            return STATUS_CONST(NOT_FOUND);
          }
          uintptr_t current =
              __atomic_load_n(&control->strongCount, __ATOMIC_RELAXED);
          while (current != 0U) {
            if (__atomic_compare_exchange_n(&control->strongCount, &current,
                                            current + 1U, true,
                                            __ATOMIC_ACQ_REL,
                                            __ATOMIC_RELAXED)) {
              *outPtr = control->managedPointer;
              return STATUS_CONST(SUCCESS);
            }
          }
          return STATUS_CONST(NOT_FOUND);
        }),

        FUNCTION(size_t, AtomicControlStrongCount,
                 (const MODULE_ITYPE(Memory, SharedControlBlockAtomic) *control),
                 {
          if (!control) {
            return 0U;
          }
          return (size_t)__atomic_load_n(&control->strongCount,
                                         __ATOMIC_ACQUIRE);
        })
    )
);

#ifndef TEMPLATE_Pointer
#define TEMPLATE_Pointer(TARGET, ...) NS(TEMPLATE_Pointer, TARGET)(__VA_ARGS__)
#endif

/* EOF */
