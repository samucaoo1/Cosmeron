#pragma once
/*
 * Decoder/encoder-facing Audio API. Does not include Device, Player or Mixer
 * public interfaces. WAV uses native C11 frame decoding; MP3 and FLAC still import Miniaudio.
 * Including the whole CodecOnly.h thus does NOT certify zero-link or remove
 * the remaining decoder/runtime dependencies.
 */
#include "Offline.h"
#include "Codec/Codec.h"
