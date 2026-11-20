#pragma once

#include "Sort.space"
#include "Array.h"
#include "Classical.h"
#include "Esoteric.h"
#include "Utility.h"
#include "Integer.h"
#include "Typed.h"

PACKAGE(LIB_PREFIX(SORT_MOD),
    SORT_ARRAY_OPERATIONS,
    SORT_CLASSICAL_OPERATIONS,
    SORT_ESOTERIC_OPERATIONS,
    SORT_UTILITY_OPERATIONS,
    SORT_RADIX_OPERATIONS
);

#include "Impl/Array.impl"
#include "Impl/Classical.impl"
#include "Impl/Esoteric.impl"
#include "Impl/Utility.impl"
#include "Impl/Integer.impl"

PACKAGE_IMPLEMENT(LIB_PREFIX(SORT_MOD));

/* EOF */
