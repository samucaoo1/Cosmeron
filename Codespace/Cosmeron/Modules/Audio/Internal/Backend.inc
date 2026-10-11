#pragma once
#define AUDIO_MINIAUDIO_BACKEND_INCLUDED 1
#include "../Audio.space"
#include "NativeMath.inc"
/* Desktop miniaudio requires the real OS pthread library. For Clang+lld
 * ELF toolchains this records the dependency automatically, with no manual
 * pthread link flags. GCC and linkers without ELF autolink still require
 * explicit native link options. This is NOT a no-library-dependency build.
 * Keep decoder-only mode independent of pthread and libm linker options. */
#if (defined(__FreeBSD__) || defined(__OpenBSD__) || \
     defined(__NetBSD__) || defined(__DragonFly__)) && \
    defined(__clang__) && !defined(COSMERON_NO_IMPLICIT_LINK) && \
    !defined(AUDIO_MINIAUDIO_CODEC_MINIMAL)
#pragma comment(lib, "pthread")
#endif

/* Warning isolation applies only to the pinned third-party implementation. */
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined(_MSC_VER)
#pragma warning(push, 0)
#endif
#define cosmeron_audio_vendor_MINIAUDIO_IMPLEMENTATION
#define cosmeron_audio_vendor_MA_API static
#define cosmeron_audio_vendor_MA_NO_NULL
#define cosmeron_audio_vendor_MA_NO_JACK
#define cosmeron_audio_vendor_MA_NO_ENCODING
/* Optional decoder-only vendor profile.  Miniaudio's documented build
 * switches are scoped to this single header inclusion, so a separate TU
 * can select full desktop support without changing public decoder types.
 * No promise about -lm is made: only thread/device code is excluded. */
#if defined(AUDIO_MINIAUDIO_CODEC_MINIMAL)
#define AUDIO_MINIAUDIO_CODEC_MINIMAL_SELECTED 1
#define cosmeron_audio_vendor_MA_NO_DEVICE_IO
#define cosmeron_audio_vendor_MA_NO_THREADING
#define cosmeron_audio_vendor_MA_NO_RESOURCE_MANAGER
#define cosmeron_audio_vendor_MA_NO_ENGINE
#define cosmeron_audio_vendor_MA_NO_NODE_GRAPH
#define cosmeron_audio_vendor_MA_NO_GENERATION
#endif
#include "Miniaudio.inc"
#if defined(AUDIO_MINIAUDIO_CODEC_MINIMAL_SELECTED)
#undef cosmeron_audio_vendor_MA_NO_DEVICE_IO
#undef cosmeron_audio_vendor_MA_NO_THREADING
#undef cosmeron_audio_vendor_MA_NO_RESOURCE_MANAGER
#undef cosmeron_audio_vendor_MA_NO_ENGINE
#undef cosmeron_audio_vendor_MA_NO_NODE_GRAPH
#undef cosmeron_audio_vendor_MA_NO_GENERATION
#endif
#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

static inline OPSTATUS
AUDIO_INS(BackendStatus)(cosmeron_audio_vendor_ma_result result) {
  switch (result) {
  case cosmeron_audio_vendor_MA_SUCCESS:
    return STATUS_CONST(SUCCESS);
  case cosmeron_audio_vendor_MA_INVALID_ARGS:
    return STATUS_CONST(INVALID_ARGUMENT);
  case cosmeron_audio_vendor_MA_OUT_OF_MEMORY:
    return STATUS_CONST(OUT_OF_MEMORY);
  case cosmeron_audio_vendor_MA_DOES_NOT_EXIST:
    return STATUS_CONST(NOT_FOUND);
  case cosmeron_audio_vendor_MA_NOT_IMPLEMENTED:
  case cosmeron_audio_vendor_MA_FORMAT_NOT_SUPPORTED:
    return STATUS_CONST(NOT_SUPPORTED);
  case cosmeron_audio_vendor_MA_NO_DEVICE:
    return STATUS_CONST(NOT_AVAILABLE);
  case cosmeron_audio_vendor_MA_OUT_OF_RANGE:
    return STATUS_CONST(OUT_OF_RANGE);
  default:
    return STATUS_CONST(GENERIC_ERROR);
  }
}
