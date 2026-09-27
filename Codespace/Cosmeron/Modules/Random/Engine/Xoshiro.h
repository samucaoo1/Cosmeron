#pragma once
#include "Engine.space"
#include "VTable.h"

typedef struct { uint64_t state[4]; } RANDOM_ENGINE_TYPE(Xoshiro);
RANDOM_ENGINE_SEED_PROTOTYPE(Xoshiro);
RANDOM_ENGINE_NEXT_PROTOTYPE(Xoshiro);
#include "Impl/Xoshiro.impl"
RANDOM_ENGINE_VTABLE_DEFINE(Xoshiro);
