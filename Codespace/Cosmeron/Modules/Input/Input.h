#pragma once

#include "Input.space"
#include "Internal/Core.inc"
#include "Internal/Backend.inc"

/* General device input; independent of Terminal and stdin.
 * One shared snapshot per linked image. All calls belong to one thread.
 * Include individual package headers when only their queries are needed. */

/* Collect input without waiting for a new event. Renews transient flags and deltas.
 * Initializes native resources automatically; no public Init/Free is required.
 * A failed update can leave a partially refreshed snapshot. */
#define INPUT_UPDATE_PROTOTYPE \
  static inline OPSTATUS INPUT_FUNC(Update)( \
      void)
INPUT_UPDATE_PROTOTYPE;

/* Select once before the first Update. APPLICATION takes keyboard/mouse events from Submit. */
#define INPUT_SET_CAPTURE_MODE_PROTOTYPE \
  static inline OPSTATUS INPUT_FUNC(SetCaptureMode)( \
      INPUT_TYPE(CaptureMode) mode)
INPUT_SET_CAPTURE_MODE_PROTOTYPE;

/* Available features observed at the last Update; a bitmask, not a promise about every device. */
#define INPUT_GET_CAPABILITIES_PROTOTYPE \
  static inline INPUT_TYPE(Capabilities) INPUT_FUNC(GetCapabilities)( \
      void)
INPUT_GET_CAPABILITIES_PROTOTYPE;

#include "Impl/Input.impl"

#include "Keyboard/Keyboard.h"
#include "Mouse/Mouse.h"
#include "Controller/Controller.h"
#include "Gamepad/Gamepad.h"
#include "Event/Event.h"
