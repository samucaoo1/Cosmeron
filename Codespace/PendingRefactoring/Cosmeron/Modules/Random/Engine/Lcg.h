#pragma once
#include "Engine.space"
#include "Descriptor.h"

typedef struct { uint64_t value; } RANDOM_ENGINE_TYPE(Lcg);
RANDOM_ENGINE_SEED_PROTOTYPE(Lcg);
RANDOM_ENGINE_NEXT_PROTOTYPE(Lcg);
#include "Impl/Lcg.impl"
RANDOM_ENGINE_DESCRIPTOR_INSTANCE(Lcg);
