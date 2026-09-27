#pragma once
#include "Engine.space"
#include "VTable.h"

typedef struct { uint64_t value; } RANDOM_ENGINE_TYPE(Splitmix);
RANDOM_ENGINE_SEED_PROTOTYPE(Splitmix);
RANDOM_ENGINE_NEXT_PROTOTYPE(Splitmix);
#include "Impl/Splitmix.impl"
RANDOM_ENGINE_VTABLE_DEFINE(Splitmix);
