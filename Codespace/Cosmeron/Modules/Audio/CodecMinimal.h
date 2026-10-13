#pragma once
/*
 * Minimal decoder-only build profile.
 * Standalone native WAV, FLAC and MP3 codecs.
 */
#include "Offline.h"
#include "Internal/NativeMath.inc"
#define AUDIO_MINIAUDIO_CODEC_MINIMAL_SELECTED 1
#include "Codec/Codec.h"
