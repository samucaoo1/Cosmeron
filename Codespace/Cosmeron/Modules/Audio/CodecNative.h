#pragma once
/* Standalone native WAV, FLAC and MPEG Layer III codecs, with no imported
 * backend and no device/thread/math libraries. Select this profile before
 * Codec.h in this translation unit. */
#if defined(AUDIO_MINIAUDIO_BACKEND_INCLUDED)
#error "Include CodecNative.h before any miniaudio-backed Audio header"
#endif
#define AUDIO_CODEC_NATIVE_ONLY 1
#include "Codec/Codec.h"
