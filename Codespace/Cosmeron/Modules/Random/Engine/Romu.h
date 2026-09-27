#pragma once
#include "Engine.space"
#include "FunctionTable.h"

typedef struct { uint64_t x; uint64_t y; uint64_t z; } RANDOM_ENGINE_TYPE(Romu);
RANDOM_ENGINE_SEED_PROTOTYPE(Romu);
RANDOM_ENGINE_NEXT_PROTOTYPE(Romu);
#include "Impl/Romu.impl"
#ifndef RANDOM_DISABLE_FUNCTION_TABLE
RANDOM_ENGINE_FUNCTION_TABLE_INSTANCE(Romu);
#endif
