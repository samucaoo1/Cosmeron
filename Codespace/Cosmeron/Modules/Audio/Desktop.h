#pragma once
/*
 * Desktop playback API: audio device, player and mixer. The Miniaudio
 * backend may require OS-dependent linker flags; use Offline.h for pure
 * synthesis and CodecOnly.h when playback is not needed.
 */
#if defined(AUDIO_MINIAUDIO_CODEC_MINIMAL_SELECTED)
#error "Miniaudio decoder-only profile cannot be combined with Desktop in the same translation unit; use another TU"
#endif
#include "CodecOnly.h"
#include "Device/Device.h"
#include "Player/Player.h"
#include "Mixer/Mixer.h"
#include "Beep/Playback.inc"
