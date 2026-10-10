#pragma once

#include "../Input.space"
#include "../Internal/Core.inc"

/* Standardized view of the same controller ID; no additional handle.
 * Missing controls and unmapped devices yield neutral values.
 * Sticks: [-1,1], positive Y up. Triggers: [0,1]. No automatic dead zone. */

/* Test whether the controller has a recognized standard gamepad layout. */
#define INPUT_GAMEPAD_IS_MAPPED_PROTOTYPE \
  static inline bool INPUT_GAMEPAD_FUNC(IsMapped)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_GAMEPAD_IS_MAPPED_PROTOTYPE;

/* Read the current held state; does not consume input. */
#define INPUT_GAMEPAD_IS_DOWN_PROTOTYPE \
  static inline bool INPUT_GAMEPAD_FUNC(IsDown)( \
      INPUT_TYPE(ControllerId) controller, \
      INPUT_TYPE(GamepadButton) button)
INPUT_GAMEPAD_IS_DOWN_PROTOTYPE;

/* True if a down transition was observed during the latest Update. */
#define INPUT_GAMEPAD_IS_PRESSED_PROTOTYPE \
  static inline bool INPUT_GAMEPAD_FUNC(IsPressed)( \
      INPUT_TYPE(ControllerId) controller, \
      INPUT_TYPE(GamepadButton) button)
INPUT_GAMEPAD_IS_PRESSED_PROTOTYPE;

/* True if an up transition was observed during the latest Update. */
#define INPUT_GAMEPAD_IS_RELEASED_PROTOTYPE \
  static inline bool INPUT_GAMEPAD_FUNC(IsReleased)( \
      INPUT_TYPE(ControllerId) controller, \
      INPUT_TYPE(GamepadButton) button)
INPUT_GAMEPAD_IS_RELEASED_PROTOTYPE;

/* Read an axis; invalid indices or unavailable input return zero.
 * Stick components use [-1,1]; triggers use [0,1]. */
#define INPUT_GAMEPAD_GET_AXIS_PROTOTYPE \
  static inline float INPUT_GAMEPAD_FUNC(GetAxis)( \
      INPUT_TYPE(ControllerId) controller, \
      INPUT_TYPE(GamepadAxis) axis)
INPUT_GAMEPAD_GET_AXIS_PROTOTYPE;

#include "Impl/Gamepad.impl"
