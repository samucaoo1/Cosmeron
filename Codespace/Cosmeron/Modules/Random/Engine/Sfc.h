#pragma once
#include "Engine.space"
#include "VTable.h"

typedef struct { uint64_t a; uint64_t b; uint64_t c; uint64_t counter; } RANDOM_ENGINE_TYPE(Sfc);
RANDOM_ENGINE_SEED_PROTOTYPE(Sfc);
RANDOM_ENGINE_NEXT_PROTOTYPE(Sfc);
#include "Impl/Sfc.impl"
RANDOM_ENGINE_VTABLE_DEFINE(Sfc);
