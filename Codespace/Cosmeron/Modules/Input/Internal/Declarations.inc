#define INPUT_UPDATE_PROTOTYPE \
  static inline OPSTATUS INPUT_FUNC(Update)( \
      void)
INPUT_UPDATE_PROTOTYPE;

#define INPUT_KEYBOARD_IS_DOWN_PROTOTYPE \
  static inline bool INPUT_KEYBOARD_FUNC(IsDown)( \
      INPUT_TYPE(Key) key)
INPUT_KEYBOARD_IS_DOWN_PROTOTYPE;

#define INPUT_KEYBOARD_IS_PRESSED_PROTOTYPE \
  static inline bool INPUT_KEYBOARD_FUNC(IsPressed)( \
      INPUT_TYPE(Key) key)
INPUT_KEYBOARD_IS_PRESSED_PROTOTYPE;

#define INPUT_KEYBOARD_IS_RELEASED_PROTOTYPE \
  static inline bool INPUT_KEYBOARD_FUNC(IsReleased)( \
      INPUT_TYPE(Key) key)
INPUT_KEYBOARD_IS_RELEASED_PROTOTYPE;

#define INPUT_KEYBOARD_IS_REPEATED_PROTOTYPE \
  static inline bool INPUT_KEYBOARD_FUNC(IsRepeated)( \
      INPUT_TYPE(Key) key)
INPUT_KEYBOARD_IS_REPEATED_PROTOTYPE;

#define INPUT_KEYBOARD_GET_MODIFIERS_PROTOTYPE \
  static inline INPUT_TYPE(Modifiers) INPUT_KEYBOARD_FUNC(GetModifiers)( \
      void)
INPUT_KEYBOARD_GET_MODIFIERS_PROTOTYPE;

#define INPUT_MOUSE_IS_DOWN_PROTOTYPE \
  static inline bool INPUT_MOUSE_FUNC(IsDown)( \
      INPUT_TYPE(MouseButton) button)
INPUT_MOUSE_IS_DOWN_PROTOTYPE;

#define INPUT_MOUSE_IS_PRESSED_PROTOTYPE \
  static inline bool INPUT_MOUSE_FUNC(IsPressed)( \
      INPUT_TYPE(MouseButton) button)
INPUT_MOUSE_IS_PRESSED_PROTOTYPE;

#define INPUT_MOUSE_IS_RELEASED_PROTOTYPE \
  static inline bool INPUT_MOUSE_FUNC(IsReleased)( \
      INPUT_TYPE(MouseButton) button)
INPUT_MOUSE_IS_RELEASED_PROTOTYPE;

#define INPUT_MOUSE_GET_POSITION_PROTOTYPE \
  static inline void INPUT_MOUSE_FUNC(GetPosition)( \
      float *x, \
      float *y)
INPUT_MOUSE_GET_POSITION_PROTOTYPE;

#define INPUT_MOUSE_GET_DELTA_PROTOTYPE \
  static inline void INPUT_MOUSE_FUNC(GetDelta)( \
      float *x, \
      float *y)
INPUT_MOUSE_GET_DELTA_PROTOTYPE;

#define INPUT_MOUSE_GET_SCROLL_PROTOTYPE \
  static inline void INPUT_MOUSE_FUNC(GetScroll)( \
      float *x, \
      float *y)
INPUT_MOUSE_GET_SCROLL_PROTOTYPE;

#define INPUT_CONTROLLER_ENUMERATE_PROTOTYPE \
  static inline OPSTATUS INPUT_CONTROLLER_FUNC(Enumerate)( \
      INPUT_TYPE(ControllerId) *controllers, \
      size_t capacity, \
      size_t *count)
INPUT_CONTROLLER_ENUMERATE_PROTOTYPE;

#define INPUT_CONTROLLER_IS_CONNECTED_PROTOTYPE \
  static inline bool INPUT_CONTROLLER_FUNC(IsConnected)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_IS_CONNECTED_PROTOTYPE;

#define INPUT_CONTROLLER_GET_NAME_PROTOTYPE \
  static inline OPSTATUS INPUT_CONTROLLER_FUNC(GetName)( \
      INPUT_TYPE(ControllerId) controller, \
      char *buffer, \
      size_t capacity, \
      size_t *required)
INPUT_CONTROLLER_GET_NAME_PROTOTYPE;

#define INPUT_CONTROLLER_GET_BUTTON_COUNT_PROTOTYPE \
  static inline uint32_t INPUT_CONTROLLER_FUNC(GetButtonCount)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_GET_BUTTON_COUNT_PROTOTYPE;

#define INPUT_CONTROLLER_GET_AXIS_COUNT_PROTOTYPE \
  static inline uint32_t INPUT_CONTROLLER_FUNC(GetAxisCount)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_GET_AXIS_COUNT_PROTOTYPE;

#define INPUT_CONTROLLER_GET_HAT_COUNT_PROTOTYPE \
  static inline uint32_t INPUT_CONTROLLER_FUNC(GetHatCount)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_GET_HAT_COUNT_PROTOTYPE;

#define INPUT_CONTROLLER_HAS_RUMBLE_PROTOTYPE \
  static inline bool INPUT_CONTROLLER_FUNC(HasRumble)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_HAS_RUMBLE_PROTOTYPE;

#define INPUT_CONTROLLER_IS_DOWN_PROTOTYPE \
  static inline bool INPUT_CONTROLLER_FUNC(IsDown)( \
      INPUT_TYPE(ControllerId) controller, \
      uint32_t button)
INPUT_CONTROLLER_IS_DOWN_PROTOTYPE;

#define INPUT_CONTROLLER_IS_PRESSED_PROTOTYPE \
  static inline bool INPUT_CONTROLLER_FUNC(IsPressed)( \
      INPUT_TYPE(ControllerId) controller, \
      uint32_t button)
INPUT_CONTROLLER_IS_PRESSED_PROTOTYPE;

#define INPUT_CONTROLLER_IS_RELEASED_PROTOTYPE \
  static inline bool INPUT_CONTROLLER_FUNC(IsReleased)( \
      INPUT_TYPE(ControllerId) controller, \
      uint32_t button)
INPUT_CONTROLLER_IS_RELEASED_PROTOTYPE;

#define INPUT_CONTROLLER_GET_AXIS_PROTOTYPE \
  static inline float INPUT_CONTROLLER_FUNC(GetAxis)( \
      INPUT_TYPE(ControllerId) controller, \
      uint32_t axis)
INPUT_CONTROLLER_GET_AXIS_PROTOTYPE;

#define INPUT_CONTROLLER_GET_HAT_PROTOTYPE \
  static inline INPUT_TYPE(Hat) INPUT_CONTROLLER_FUNC(GetHat)( \
      INPUT_TYPE(ControllerId) controller, \
      uint32_t hat)
INPUT_CONTROLLER_GET_HAT_PROTOTYPE;

#define INPUT_CONTROLLER_RUMBLE_PROTOTYPE \
  static inline OPSTATUS INPUT_CONTROLLER_FUNC(Rumble)( \
      INPUT_TYPE(ControllerId) controller, \
      float lowFrequency, \
      float highFrequency, \
      uint32_t durationMs)
INPUT_CONTROLLER_RUMBLE_PROTOTYPE;

#define INPUT_CONTROLLER_STOP_RUMBLE_PROTOTYPE \
  static inline OPSTATUS INPUT_CONTROLLER_FUNC(StopRumble)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_CONTROLLER_STOP_RUMBLE_PROTOTYPE;

#define INPUT_GAMEPAD_IS_MAPPED_PROTOTYPE \
  static inline bool INPUT_GAMEPAD_FUNC(IsMapped)( \
      INPUT_TYPE(ControllerId) controller)
INPUT_GAMEPAD_IS_MAPPED_PROTOTYPE;

#define INPUT_GAMEPAD_IS_DOWN_PROTOTYPE \
  static inline bool INPUT_GAMEPAD_FUNC(IsDown)( \
      INPUT_TYPE(ControllerId) controller, \
      INPUT_TYPE(GamepadButton) button)
INPUT_GAMEPAD_IS_DOWN_PROTOTYPE;

#define INPUT_GAMEPAD_IS_PRESSED_PROTOTYPE \
  static inline bool INPUT_GAMEPAD_FUNC(IsPressed)( \
      INPUT_TYPE(ControllerId) controller, \
      INPUT_TYPE(GamepadButton) button)
INPUT_GAMEPAD_IS_PRESSED_PROTOTYPE;

#define INPUT_GAMEPAD_IS_RELEASED_PROTOTYPE \
  static inline bool INPUT_GAMEPAD_FUNC(IsReleased)( \
      INPUT_TYPE(ControllerId) controller, \
      INPUT_TYPE(GamepadButton) button)
INPUT_GAMEPAD_IS_RELEASED_PROTOTYPE;

#define INPUT_GAMEPAD_GET_AXIS_PROTOTYPE \
  static inline float INPUT_GAMEPAD_FUNC(GetAxis)( \
      INPUT_TYPE(ControllerId) controller, \
      INPUT_TYPE(GamepadAxis) axis)
INPUT_GAMEPAD_GET_AXIS_PROTOTYPE;

#define INPUT_EVENT_POLL_PROTOTYPE \
  static inline bool INPUT_EVENT_FUNC(Poll)( \
      INPUT_TYPE(TEvent) *event)
INPUT_EVENT_POLL_PROTOTYPE;

#define INPUT_EVENT_CLEAR_PROTOTYPE \
  static inline void INPUT_EVENT_FUNC(Clear)( \
      void)
INPUT_EVENT_CLEAR_PROTOTYPE;

#define INPUT_EVENT_GET_DROPPED_COUNT_PROTOTYPE \
  static inline uint64_t INPUT_EVENT_FUNC(GetDroppedCount)( \
      void)
INPUT_EVENT_GET_DROPPED_COUNT_PROTOTYPE;
