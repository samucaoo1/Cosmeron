#pragma once
#if defined(INPUT_NO_NATIVE)
static inline OPSTATUS INPUT_INS(BackendUpdate)(void) { return STATUS_CONST(NOT_SUPPORTED); }
static inline OPSTATUS INPUT_INS(BackendRumble)(INPUT_INS(Device) *device, float low, float high, uint32_t ms) {
  (void)device; (void)low; (void)high; (void)ms;
  return STATUS_CONST(NOT_SUPPORTED);
}
#elif defined(_WIN32)
#include "Windows.inc"
#elif defined(__linux__)
#include "Linux.inc"
#else
static inline OPSTATUS INPUT_INS(BackendUpdate)(void) { return STATUS_CONST(NOT_SUPPORTED); }
static inline OPSTATUS INPUT_INS(BackendRumble)(INPUT_INS(Device) *device, float low, float high, uint32_t ms) {
  (void)device; (void)low; (void)high; (void)ms;
  return STATUS_CONST(NOT_SUPPORTED);
}
#endif
