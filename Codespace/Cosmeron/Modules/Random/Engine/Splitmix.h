#pragma once
#include "Engine.space"
#include "FunctionTable.h"

typedef struct { uint64_t value; } RANDOM_ENGINE_TYPE(Splitmix);
RANDOM_ENGINE_SEED_PROTOTYPE(Splitmix);
RANDOM_ENGINE_NEXT_PROTOTYPE(Splitmix);
#include "Impl/Splitmix.impl"
#ifndef RANDOM_DISABLE_FUNCTION_TABLE
RANDOM_ENGINE_FUNCTION_TABLE_INSTANCE(Splitmix);
#endif
