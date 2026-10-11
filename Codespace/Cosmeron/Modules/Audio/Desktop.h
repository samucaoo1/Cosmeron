#pragma once
/*
 * Desktop playback API: audio device, player and mixer. The Miniaudio
 * backend may require OS-dependent linker flags; use Offline.h for pure
 * synthesis and CodecOnly.h when playback is not needed.
 */
#include "CodecOnly.h"
#include "Device/Device.h"
#include "Player/Player.h"
#include "Mixer/Mixer.h"
#include "Beep/Playback.inc"
