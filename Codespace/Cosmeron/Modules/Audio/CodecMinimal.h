#pragma once
/*
 * Explicit Miniaudio 0.11.23 decoder-only build profile.
 * Removes native device I/O, native threading, engine and resource manager.
 * Preserves the exact same Codec API as CodecOnly.h.
 *
 * A single TU must choose EITHER CodecMinimal.h OR Desktop.h; do not
 * include both. Full CodecOnly.h remains backwards-compatible.
 *
 * This profile does not guarantee no runtime dependency or -lm. See the
 * independent native WAV decoder in Codec/WAV.h for strict C11 zero-link.
 */
#include "Offline.h"
#if defined(AUDIO_MINIAUDIO_BACKEND_INCLUDED) && \
    !defined(AUDIO_MINIAUDIO_CODEC_MINIMAL_SELECTED)
#error "Import CodecMinimal.h BEFORE any full Miniaudio-backed header in this TU"
#endif
#ifndef AUDIO_MINIAUDIO_BACKEND_INCLUDED
#define AUDIO_MINIAUDIO_CODEC_MINIMAL 1
#include "Codec/Codec.h"
#undef AUDIO_MINIAUDIO_CODEC_MINIMAL
#else
#include "Codec/Codec.h"
#endif
