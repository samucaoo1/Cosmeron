#pragma once
#include "Engine.space"
#include "VTable.h"

typedef struct { uint64_t x; uint64_t y; uint64_t z; } RANDOM_ENGINE_TYPE(Romu);
RANDOM_ENGINE_SEED_PROTOTYPE(Romu);
RANDOM_ENGINE_NEXT_PROTOTYPE(Romu);
#include "Impl/Romu.impl"
RANDOM_ENGINE_VTABLE_DEFINE(Romu);
