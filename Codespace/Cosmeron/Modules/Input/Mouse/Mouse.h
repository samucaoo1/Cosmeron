#pragma once

#include "../Input.space"
#include "../Internal/Core.inc"

/* Mouse queries read the shared snapshot. Invalid buttons return false. */

/* Read the current held state; does not consume input. */
#define INPUT_MOUSE_IS_DOWN_PROTOTYPE \
  static inline bool INPUT_MOUSE_FUNC(IsDown)( \
      INPUT_TYPE(MouseButton) button)
INPUT_MOUSE_IS_DOWN_PROTOTYPE;

/* True if a down transition was observed during the latest Update. */
#define INPUT_MOUSE_IS_PRESSED_PROTOTYPE \
  static inline bool INPUT_MOUSE_FUNC(IsPressed)( \
      INPUT_TYPE(MouseButton) button)
INPUT_MOUSE_IS_PRESSED_PROTOTYPE;

/* True if an up transition was observed during the latest Update. */
#define INPUT_MOUSE_IS_RELEASED_PROTOTYPE \
  static inline bool INPUT_MOUSE_FUNC(IsReleased)( \
      INPUT_TYPE(MouseButton) button)
INPUT_MOUSE_IS_RELEASED_PROTOTYPE;

/* Copy persistent position. Either output may be NULL. Windows: desktop
 * coordinates. Linux: integrated raw displacement, NOT desktop cursor pixels. */
#define INPUT_MOUSE_GET_POSITION_PROTOTYPE \
  static inline void INPUT_MOUSE_FUNC(GetPosition)( \
      float *x, \
      float *y)
INPUT_MOUSE_GET_POSITION_PROTOTYPE;

/* Copy accumulated raw relative movement for this update; outputs may be NULL. */
#define INPUT_MOUSE_GET_DELTA_PROTOTYPE \
  static inline void INPUT_MOUSE_FUNC(GetDelta)( \
      float *x, \
      float *y)
INPUT_MOUSE_GET_DELTA_PROTOTYPE;

/* Copy accumulated horizontal/vertical wheel steps; outputs may be NULL. */
#define INPUT_MOUSE_GET_SCROLL_PROTOTYPE \
  static inline void INPUT_MOUSE_FUNC(GetScroll)( \
      float *x, \
      float *y)
INPUT_MOUSE_GET_SCROLL_PROTOTYPE;

#include "Impl/Mouse.impl"
