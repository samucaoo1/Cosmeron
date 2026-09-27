#pragma once
#include "Engine.space"
#include "FunctionTable.h"

typedef struct { uint64_t state[2]; } RANDOM_ENGINE_TYPE(Xoroshiro);
RANDOM_ENGINE_SEED_PROTOTYPE(Xoroshiro);
RANDOM_ENGINE_NEXT_PROTOTYPE(Xoroshiro);
#include "Impl/Xoroshiro.impl"
#ifndef RANDOM_DISABLE_FUNCTION_TABLE
RANDOM_ENGINE_FUNCTION_TABLE_INSTANCE(Xoroshiro);
#endif
