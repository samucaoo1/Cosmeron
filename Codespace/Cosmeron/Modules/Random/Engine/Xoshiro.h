#pragma once
#include "Engine.space"
#include "FunctionTable.h"

typedef struct { uint64_t state[4]; } RANDOM_ENGINE_TYPE(Xoshiro);
RANDOM_ENGINE_SEED_PROTOTYPE(Xoshiro);
RANDOM_ENGINE_NEXT_PROTOTYPE(Xoshiro);
#include "Impl/Xoshiro.impl"
#ifndef RANDOM_DISABLE_FUNCTION_TABLE
RANDOM_ENGINE_FUNCTION_TABLE_INSTANCE(Xoshiro);
#endif
