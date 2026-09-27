#pragma once

#include "../Random.space"

typedef uint64_t (*RANDOM_MIXER_FUNCTION)(uint64_t value);

#define RANDOM_MIXER_PROTOTYPE(NAME)                                          \
  inline static uint64_t RANDOM_MIXER_FUNC(NAME)(uint64_t value)

RANDOM_MIXER_PROTOTYPE(Jenkins);
RANDOM_MIXER_PROTOTYPE(Knuth);
RANDOM_MIXER_PROTOTYPE(Murmur3);
RANDOM_MIXER_PROTOTYPE(Splitmix64);
RANDOM_MIXER_PROTOTYPE(Stafford);
RANDOM_MIXER_PROTOTYPE(Wang);
RANDOM_MIXER_PROTOTYPE(WyHash);
RANDOM_MIXER_PROTOTYPE(Xorshift);

#include "Impl/Mixer.impl"

