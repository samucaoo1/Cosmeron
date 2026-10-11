#pragma once
#include "../Surface/Surface.h"
#include "../../Input/Input.space"
#include "../../Text/Encoding/UTF8.h"

#define WINDOW_MOD Window
#define WINDOW_CMOD WINDOW
#ifndef WINDOW_NS
#define WINDOW_NS(NAME) GNS2(LIB_PREFIX(WINDOW_MOD), NAME)
#endif
#define WINDOW_TYPE(NAME) WINDOW_NS(NAME)
#define WINDOW_FUNC(NAME) WINDOW_NS(NAME)
#define WINDOW_INS(NAME) GNS3(LIB_PREFIX(WINDOW_MOD), Internal, NAME)
#define WINDOW_CONST(NAME) CNS2(LIB_PREFIX_CONST(WINDOW_CMOD), NAME)
#ifndef WINDOW_EVENT_CAPACITY
#define WINDOW_EVENT_CAPACITY 128
#endif
_Static_assert(WINDOW_EVENT_CAPACITY > 0, "Window event capacity must be positive");
typedef enum WINDOW_TYPE(EventType) {
  WINDOW_CONST(EVENT_INPUT),
  WINDOW_CONST(EVENT_RESIZE),
  WINDOW_CONST(EVENT_CLOSE)
} WINDOW_TYPE(EventType);
typedef struct WINDOW_TYPE(TEvent) {
  WINDOW_TYPE(EventType) type;
  INPUT_TYPE(TEvent) input;
  TDUAL_TYPE(uint32) size;
} WINDOW_TYPE(TEvent);
/* Optional host integration (including existing windows and embedded displays).
 * user is borrowed; Close releases only resources owned by the host adapter. */
typedef struct WINDOW_TYPE(TAdapter) {
  void *user;
  OPSTATUS (*open)(void *user, const char *title, TDUAL_TYPE(uint32) size);
  OPSTATUS (*poll)(void *user, WINDOW_TYPE(TEvent) * outEvent);
  OPSTATUS (*update)(void *user, const GRAPHICS_TYPE(TSurface) * surface);
  void (*close)(void *user);
} WINDOW_TYPE(TAdapter);
typedef struct WINDOW_TYPE(TWindow) {
  void *native;
  WINDOW_TYPE(TAdapter) adapter;
  TDUAL_TYPE(uint32) size;
  bool open;
} WINDOW_TYPE(TWindow);
#define WINDOW_OPEN_PROTOTYPE                                                                      \
  static inline OPSTATUS WINDOW_FUNC(Open)(WINDOW_TYPE(TWindow) * outWindow, const char *title,    \
                                           TDUAL_TYPE(uint32) size)
#define WINDOW_OPEN_WITH_PROTOTYPE                                                                 \
  static inline OPSTATUS WINDOW_FUNC(OpenWith)(WINDOW_TYPE(TWindow) * outWindow,                   \
                                               const char *title, TDUAL_TYPE(uint32) size,         \
                                               const WINDOW_TYPE(TAdapter) * adapter)
#define WINDOW_POLL_PROTOTYPE                                                                      \
  static inline OPSTATUS WINDOW_FUNC(Poll)(WINDOW_TYPE(TWindow) * window,                          \
                                           WINDOW_TYPE(TEvent) * outEvent)
#define WINDOW_UPDATE_PROTOTYPE                                                                    \
  static inline OPSTATUS WINDOW_FUNC(Update)(WINDOW_TYPE(TWindow) * window,                        \
                                             const GRAPHICS_TYPE(TSurface) * surface)
#define WINDOW_CLOSE_PROTOTYPE static inline void WINDOW_FUNC(Close)(WINDOW_TYPE(TWindow) * window)
#define WINDOW_IS_OPEN_PROTOTYPE                                                                   \
  static inline bool WINDOW_FUNC(IsOpen)(const WINDOW_TYPE(TWindow) * window)
#define WINDOW_GET_SIZE_PROTOTYPE                                                                  \
  static inline OPSTATUS WINDOW_FUNC(GetSize)(const WINDOW_TYPE(TWindow) * window,                 \
                                              TDUAL_TYPE(uint32) * outSize)
WINDOW_OPEN_PROTOTYPE;
WINDOW_OPEN_WITH_PROTOTYPE;
WINDOW_POLL_PROTOTYPE;
WINDOW_UPDATE_PROTOTYPE;
WINDOW_CLOSE_PROTOTYPE;
WINDOW_IS_OPEN_PROTOTYPE;
WINDOW_GET_SIZE_PROTOTYPE;
#include "Impl/Window.impl"
