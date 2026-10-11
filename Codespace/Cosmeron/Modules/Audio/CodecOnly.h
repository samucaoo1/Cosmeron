#pragma once
/*
 * Decoder/encoder-facing Audio API. Does not include Device, Player or Mixer
 * public interfaces. The current decoder still embeds Miniaudio internally:
 * this header does NOT certify zero-link or remove its runtime dependencies.
 */
#include "Offline.h"
#include "Codec/Codec.h"
