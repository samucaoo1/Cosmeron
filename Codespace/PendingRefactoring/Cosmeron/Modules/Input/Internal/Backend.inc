#pragma once
#include "../../../Core/Preprocessor/Detect/OperationSystem.h"
#if defined(INPUT_NO_NATIVE)
static inline OPSTATUS INPUT_INS(BackendUpdate)(void) { return STATUS_CONST(NOT_SUPPORTED); }
static inline OPSTATUS INPUT_INS(BackendRumble)(INPUT_INS(Device) *device, float low, float high, uint32_t ms) {
  (void)device; (void)low; (void)high; (void)ms;
  return STATUS_CONST(NOT_SUPPORTED);
}
#elif OS_WINDOWS
#include "Windows.inc"
#elif OS_MAC && !OS_IOS
#include "MacOS.inc"
#elif OS_LINUX || OS_FREEBSD
#include "Linux.inc"
#elif OS_BSD
#include "BSD.inc"
#else
static inline OPSTATUS INPUT_INS(BackendUpdate)(void) { return STATUS_CONST(NOT_SUPPORTED); }
static inline OPSTATUS INPUT_INS(BackendRumble)(INPUT_INS(Device) *device, float low, float high, uint32_t ms) {
  (void)device; (void)low; (void)high; (void)ms;
  return STATUS_CONST(NOT_SUPPORTED);
}
#endif
