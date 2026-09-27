#pragma once
#include "Engine.space"
#include "VTable.h"

typedef struct { uint64_t state; uint64_t stream; } RANDOM_ENGINE_TYPE(Pcg);
RANDOM_ENGINE_SEED_PROTOTYPE(Pcg);
RANDOM_ENGINE_NEXT_PROTOTYPE(Pcg);
#include "Impl/Pcg.impl"
RANDOM_ENGINE_VTABLE_DEFINE(Pcg);
