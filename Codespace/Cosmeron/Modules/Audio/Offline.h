#pragma once
/*
 * Offline C11 audio: PCM, oscillator, envelope, MML/sequence and beep render.
 * Including this header NEVER imports Miniaudio, device I/O or OS threads.
 */
#include "Audio.space"
#include "PCM/PCM.h"
#include "Oscillator/Oscillator.h"
#include "Envelope/Envelope.h"
#include "Sequence/Sequence.h"
#include "Beep/Render.h"
#include "Codec/WAV.h"
#include "Codec/FLAC.h"
