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

#include "Impl/Input.impl"

#include "Keyboard/Keyboard.h"
#include "Mouse/Mouse.h"
#include "Controller/Controller.h"
#include "Gamepad/Gamepad.h"
#include "Event/Event.h"
