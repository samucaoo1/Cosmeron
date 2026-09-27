#pragma once
#include "Engine.space"
#include "VTable.h"

#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#endif

typedef struct { uint64_t value; } RANDOM_ENGINE_TYPE(WyRand);
RANDOM_ENGINE_SEED_PROTOTYPE(WyRand);
RANDOM_ENGINE_NEXT_PROTOTYPE(WyRand);
#include "Impl/WyRand.impl"
RANDOM_ENGINE_VTABLE_DEFINE(WyRand);
