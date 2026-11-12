#pragma once

#include "../../Module/Module.h"
#include "../../Outcome/Status.h"
#include <stddef.h>
#include <stdint.h>

/* ========================================================================== */
/* Internal Control Blocks for Shared & Weak Smart Pointers                   */
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
    )
);

/* EOF */

