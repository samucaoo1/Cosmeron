#pragma once
#include "Engine.space"
#include "FunctionTable.h"

typedef struct { uint64_t value; } RANDOM_ENGINE_TYPE(Lcg);
RANDOM_ENGINE_SEED_PROTOTYPE(Lcg);
RANDOM_ENGINE_NEXT_PROTOTYPE(Lcg);
#include "Impl/Lcg.impl"
#ifndef RANDOM_DISABLE_FUNCTION_TABLE
RANDOM_ENGINE_FUNCTION_TABLE_INSTANCE(Lcg);
#endif
