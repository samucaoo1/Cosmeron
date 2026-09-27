#pragma once
#include "Engine.space"
#include "FunctionTable.h"

typedef struct { uint64_t state; uint64_t stream; } RANDOM_ENGINE_TYPE(Pcg);
RANDOM_ENGINE_SEED_PROTOTYPE(Pcg);
RANDOM_ENGINE_NEXT_PROTOTYPE(Pcg);
#include "Impl/Pcg.impl"
#ifndef RANDOM_DISABLE_FUNCTION_TABLE
RANDOM_ENGINE_FUNCTION_TABLE_INSTANCE(Pcg);
#endif
