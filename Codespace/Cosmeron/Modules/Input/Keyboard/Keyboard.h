#pragma once

#include "../Input.space"
#include "../Internal/Core.inc"

/* Keyboard queries read the shared snapshot. Unknown keys return false. */

/* Read the current held state; does not consume input. */
#define INPUT_KEYBOARD_IS_DOWN_PROTOTYPE \
  static inline bool INPUT_KEYBOARD_FUNC(IsDown)( \
      INPUT_TYPE(Key) key)
INPUT_KEYBOARD_IS_DOWN_PROTOTYPE;

/* True if a down transition was observed during the latest Update. */
#define INPUT_KEYBOARD_IS_PRESSED_PROTOTYPE \
  static inline bool INPUT_KEYBOARD_FUNC(IsPressed)( \
      INPUT_TYPE(Key) key)
INPUT_KEYBOARD_IS_PRESSED_PROTOTYPE;

/* True if an up transition was observed during the latest Update. */
#define INPUT_KEYBOARD_IS_RELEASED_PROTOTYPE \
  static inline bool INPUT_KEYBOARD_FUNC(IsReleased)( \
      INPUT_TYPE(Key) key)
INPUT_KEYBOARD_IS_RELEASED_PROTOTYPE;

/* True if native keyboard repetition was observed during the latest Update. */
#define INPUT_KEYBOARD_IS_REPEATED_PROTOTYPE \
  static inline bool INPUT_KEYBOARD_FUNC(IsRepeated)( \
      INPUT_TYPE(Key) key)
INPUT_KEYBOARD_IS_REPEATED_PROTOTYPE;

/* Return the combined held Shift/Control/Alt/Super mask, not lock toggles. */
#define INPUT_KEYBOARD_GET_MODIFIERS_PROTOTYPE \
  static inline INPUT_TYPE(Modifiers) INPUT_KEYBOARD_FUNC(GetModifiers)( \
      void)
INPUT_KEYBOARD_GET_MODIFIERS_PROTOTYPE;

#include "Impl/Keyboard.impl"
