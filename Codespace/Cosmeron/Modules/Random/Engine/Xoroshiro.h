#pragma once
#include "Engine.space"
#include "VTable.h"

typedef struct { uint64_t state[2]; } RANDOM_ENGINE_TYPE(Xoroshiro);
RANDOM_ENGINE_SEED_PROTOTYPE(Xoroshiro);
RANDOM_ENGINE_NEXT_PROTOTYPE(Xoroshiro);
#include "Impl/Xoroshiro.impl"
RANDOM_ENGINE_VTABLE_DEFINE(Xoroshiro);
