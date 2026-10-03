#pragma once
#include "../Audio.space"
#include "NativeMath.inc"

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
#include "Miniaudio.inc"
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
