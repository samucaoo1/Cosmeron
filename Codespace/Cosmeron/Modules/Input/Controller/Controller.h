#pragma once

#include "../Input.space"
#include "../Internal/Core.inc"
#include "../Internal/Backend.inc"

/* Generic controller queries use connection IDs and zero-based indices.
 * Stale IDs and invalid indices yield neutral values in direct queries.
 * Fallible operations report OPSTATUS; no controller object lifecycle is exposed. */

/* Copy connected IDs. NULL/zero queries the required count. count is required
 * and is also produced on INSUFFICIENT_SPACE; the array then stays untouched. */
#define INPUT_CONTROLLER_ENUMERATE_PROTOTYPE \
  static inline OPSTATUS INPUT_CONTROLLER_FUNC(Enumerate)( \
      INPUT_TYPE(ControllerId) *controllers, \
      size_t capacity, \
      size_t *count)
INPUT_CONTROLLER_ENUMERATE_PROTOTYPE;

/* Check an ID against the latest snapshot; this does not poll the OS. */
#define INPUT_CONTROLLER_IS_CONNECTED_PROTOTYPE \
  static inline bool INPUT_CONTROLLER_FUNC(IsConnected)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_IS_CONNECTED_PROTOTYPE;

/* Copy a NUL-terminated name. NULL/zero queries the size including NUL.
 * required is mandatory and is produced even on INSUFFICIENT_SPACE. */
#define INPUT_CONTROLLER_GET_NAME_PROTOTYPE \
  static inline OPSTATUS INPUT_CONTROLLER_FUNC(GetName)( \
      INPUT_TYPE(ControllerId) controller, \
      char *buffer, \
      size_t capacity, \
      size_t *required)
INPUT_CONTROLLER_GET_NAME_PROTOTYPE;

/* Return the generic button count; zero for an invalid or stale ID. */
#define INPUT_CONTROLLER_GET_BUTTON_COUNT_PROTOTYPE \
  static inline uint32_t INPUT_CONTROLLER_FUNC(GetButtonCount)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_GET_BUTTON_COUNT_PROTOTYPE;

/* Return the generic absolute-axis count; zero for an invalid or stale ID. */
#define INPUT_CONTROLLER_GET_AXIS_COUNT_PROTOTYPE \
  static inline uint32_t INPUT_CONTROLLER_FUNC(GetAxisCount)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_GET_AXIS_COUNT_PROTOTYPE;

/* Return the directional-hat count; zero for an invalid or stale ID. */
#define INPUT_CONTROLLER_GET_HAT_COUNT_PROTOTYPE \
  static inline uint32_t INPUT_CONTROLLER_FUNC(GetHatCount)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_GET_HAT_COUNT_PROTOTYPE;

/* Check backend/device support; a later native write can still fail. */
#define INPUT_CONTROLLER_HAS_RUMBLE_PROTOTYPE \
  static inline bool INPUT_CONTROLLER_FUNC(HasRumble)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_HAS_RUMBLE_PROTOTYPE;

/* Read the current held state; does not consume input. */
#define INPUT_CONTROLLER_IS_DOWN_PROTOTYPE \
  static inline bool INPUT_CONTROLLER_FUNC(IsDown)( \
      INPUT_TYPE(ControllerId) controller, \
      uint32_t button)
INPUT_CONTROLLER_IS_DOWN_PROTOTYPE;

/* True if a down transition was observed during the latest Update. */
#define INPUT_CONTROLLER_IS_PRESSED_PROTOTYPE \
  static inline bool INPUT_CONTROLLER_FUNC(IsPressed)( \
      INPUT_TYPE(ControllerId) controller, \
      uint32_t button)
INPUT_CONTROLLER_IS_PRESSED_PROTOTYPE;

/* True if an up transition was observed during the latest Update. */
#define INPUT_CONTROLLER_IS_RELEASED_PROTOTYPE \
  static inline bool INPUT_CONTROLLER_FUNC(IsReleased)( \
      INPUT_TYPE(ControllerId) controller, \
      uint32_t button)
INPUT_CONTROLLER_IS_RELEASED_PROTOTYPE;

/* Read an axis; invalid indices or unavailable input return zero.
 * Generic axes, including triggers, are normalized to [-1,1]. */
#define INPUT_CONTROLLER_GET_AXIS_PROTOTYPE \
  static inline float INPUT_CONTROLLER_FUNC(GetAxis)( \
      INPUT_TYPE(ControllerId) controller, \
      uint32_t axis)
INPUT_CONTROLLER_GET_AXIS_PROTOTYPE;

/* Read a direction mask, including diagonals; invalid input returns CENTERED. */
#define INPUT_CONTROLLER_GET_HAT_PROTOTYPE \
  static inline INPUT_TYPE(Hat) INPUT_CONTROLLER_FUNC(GetHat)( \
      INPUT_TYPE(ControllerId) controller, \
      uint32_t hat)
INPUT_CONTROLLER_GET_HAT_PROTOTYPE;

/* Replace the vibration effect. Intensities must be finite and in [0,1].
 * Zero duration or zero intensities stops it. Linux: maximum 65535 ms.
 * Windows requires continued Update calls to stop the effect on schedule. */
#define INPUT_CONTROLLER_RUMBLE_PROTOTYPE \
  static inline OPSTATUS INPUT_CONTROLLER_FUNC(Rumble)( \
      INPUT_TYPE(ControllerId) controller, \
      float lowFrequency, \
      float highFrequency, \
      uint32_t durationMs)
INPUT_CONTROLLER_RUMBLE_PROTOTYPE;

/* Stop vibration without disconnecting the controller. */
#define INPUT_CONTROLLER_STOP_RUMBLE_PROTOTYPE \
  static inline OPSTATUS INPUT_CONTROLLER_FUNC(StopRumble)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_STOP_RUMBLE_PROTOTYPE;

#include "Impl/Controller.impl"
