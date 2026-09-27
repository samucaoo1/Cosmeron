#pragma once

#include "Random.space"
#include "Distribution/Distribution.h"
#include "Engine/Engine.h"
#include "Entropy/Entropy.h"
#include "Entropy/Pool/Pool.h"
#include "Mixer/Mixer.h"
#include "Shuffle/Shuffle.h"
#include "Source/Source.h"

inline static uint64_t RANDOM_FUNC(U64)(void);
inline static uint64_t RANDOM_FUNC(U64_FromSeed)(uint64_t seed);
inline static uint64_t RANDOM_FUNC(RangeU64)(uint64_t minimum,
                                           uint64_t maximum);
inline static int64_t RANDOM_FUNC(RangeI64)(int64_t minimum, int64_t maximum);
inline static double RANDOM_FUNC(F64)(void);
inline static bool RANDOM_FUNC(Bool)(void);

#include "Impl/Random.impl"
