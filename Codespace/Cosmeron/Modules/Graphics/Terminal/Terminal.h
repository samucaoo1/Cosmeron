#pragma once
#include "Terminal.space"
#include <stdarg.h>

/* Immediate I/O. No initialization required for process standard streams. */
#define TERMINAL_STANDARD_PROTOTYPE \
  static inline TERMINAL_TYPE(TTerminal) *TERMINAL_FUNC(Standard)(void)
#define TERMINAL_STANDARD_HANDLE_PROTOTYPE \
  static inline TERMINAL_TYPE(THandle) *TERMINAL_FUNC(StandardHandle)( \
      TERMINAL_TYPE(StandardStream) stream)
#define TERMINAL_WRITE_TO_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(WriteTo)( \
      TERMINAL_TYPE(THandle) *handle, const void *data, size_t size, size_t *outWritten)
#define TERMINAL_READ_FROM_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(ReadFrom)( \
      TERMINAL_TYPE(THandle) *handle, void *buffer, size_t capacity, size_t *outRead)
#define TERMINAL_FLUSH_HANDLE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(FlushHandle)(TERMINAL_TYPE(THandle) *handle)
#define TERMINAL_PRINT_TO_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(PrintTo)( \
      TERMINAL_TYPE(THandle) *handle, const char *format, ...)
#define TERMINAL_WRITE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Write)(const void *data, size_t size)
#define TERMINAL_READ_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Read)(void *buffer, size_t capacity, size_t *outRead)
#define TERMINAL_PRINT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Print)(const char *format, ...)
#define TERMINAL_PRINT_LN_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(PrintLn)(const char *format, ...)
#define TERMINAL_PRINT_AT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(PrintAt)( \
      TDUAL_TYPE(uint16) position, const char *format, ...)
#define TERMINAL_PRINT_STYLED_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(PrintStyled)( \
      TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...)
#define TERMINAL_PRINT_STYLED_LN_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(PrintStyledLn)( \
      TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...)
#define TERMINAL_PRINT_STYLED_AT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(PrintStyledAt)( \
      TDUAL_TYPE(uint16) position, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, \
      const char *format, ...)
#define TERMINAL_WRITE_LINE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(WriteLine)(const char *text)
#define TERMINAL_READ_LINE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(ReadLine)( \
      char *buffer, size_t capacity, size_t *outLength)
#define TERMINAL_INPUT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Input)( \
      const char *prompt, char *buffer, size_t capacity, size_t *outLength)
#define TERMINAL_SCAN_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Scan)(const char *format, ...)
#define TERMINAL_PUT_CHAR_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(PutChar)(uint32_t codepoint)
#define TERMINAL_GET_CHAR_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(GetChar)(uint32_t *outCodepoint)
#define TERMINAL_FLUSH_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Flush)(void)

/* Interactive console operations reject redirected stdout. */
#define TERMINAL_CURSOR_GET_POSITION_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Cursor_GetPosition)(TDUAL_TYPE(uint16) *outPosition)
#define TERMINAL_CURSOR_SET_POSITION_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Cursor_SetPosition)(TDUAL_TYPE(uint16) position)
#define TERMINAL_CURSOR_MOVE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Cursor_Move)(int32_t deltaX, int32_t deltaY)
#define TERMINAL_CURSOR_SHOW_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Cursor_Show)(void)
#define TERMINAL_CURSOR_HIDE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Cursor_Hide)(void)
#define TERMINAL_SIZE_GET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Size_Get)(TDUAL_TYPE(uint16) *outSize)
#define TERMINAL_CLEAR_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Clear)(void)
#define TERMINAL_CLEAR_LINE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(ClearLine)(void)
#define TERMINAL_TITLE_SET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Title_Set)(const char *title)
#define TERMINAL_BELL_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Bell)(void)

/* Style is Text/Grid's attribute cell, not a duplicate Terminal style type. */
#define TERMINAL_STYLE_SET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Style_Set)(TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)
#define TERMINAL_STYLE_GET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Style_Get)(TEXT_GRID_ATTRIBUTE_TYPE(Cell) *outStyle)
#define TERMINAL_STYLE_RESET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Style_Reset)(void)
#define TERMINAL_COLOR_SET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Color_Set)( \
      TEXT_GRID_COLOR_TYPE(RGB) foreground, TEXT_GRID_COLOR_TYPE(RGB) background)
#define TERMINAL_COLOR_RESET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Color_Reset)(void)
#define TERMINAL_ATTRIBUTE_SET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Attribute_Set)(TEXT_GRID_ATTRIBUTE_TYPE(Flags) attributes)
#define TERMINAL_ATTRIBUTE_ADD_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Attribute_Add)(TEXT_GRID_ATTRIBUTE_TYPE(Flags) attributes)
#define TERMINAL_ATTRIBUTE_REMOVE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Attribute_Remove)(TEXT_GRID_ATTRIBUTE_TYPE(Flags) attributes)
#define TERMINAL_ATTRIBUTE_RESET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Attribute_Reset)(void)

/* Poll one event without blocking; all outputs are assigned on success.
 * Mouse_Enable is optional and explicitly scoped to the process console. */
#define TERMINAL_EVENT_POLL_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Event_Poll)( \
      TERMINAL_TYPE(TEvent) *outEvent, bool *outAvailable)
#define TERMINAL_MOUSE_ENABLE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Mouse_Enable)(bool enabled)

/* Terminal keyboard input is separate from general device Input. */
#define TERMINAL_KEY_GET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Key_Get)(TERMINAL_TYPE(TKey) *outKey)
#define TERMINAL_KEY_GET_ECHO_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Key_GetEcho)(TERMINAL_TYPE(TKey) *outKey)
#define TERMINAL_KEY_HIT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Key_Hit)(bool *outAvailable)
#define TERMINAL_INPUT_MODE_GET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(InputMode_Get)(TERMINAL_TYPE(InputMode) *outMode)
#define TERMINAL_INPUT_MODE_SET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(InputMode_Set)(TERMINAL_TYPE(InputMode) mode)

TERMINAL_STANDARD_PROTOTYPE;
TERMINAL_STANDARD_HANDLE_PROTOTYPE;
TERMINAL_WRITE_TO_PROTOTYPE;
TERMINAL_READ_FROM_PROTOTYPE;
TERMINAL_FLUSH_HANDLE_PROTOTYPE;
TERMINAL_PRINT_TO_PROTOTYPE;
TERMINAL_WRITE_PROTOTYPE;
TERMINAL_READ_PROTOTYPE;
TERMINAL_PRINT_PROTOTYPE;
TERMINAL_PRINT_LN_PROTOTYPE;
TERMINAL_PRINT_AT_PROTOTYPE;
TERMINAL_PRINT_STYLED_PROTOTYPE;
TERMINAL_PRINT_STYLED_LN_PROTOTYPE;
TERMINAL_PRINT_STYLED_AT_PROTOTYPE;
TERMINAL_WRITE_LINE_PROTOTYPE;
TERMINAL_READ_LINE_PROTOTYPE;
TERMINAL_INPUT_PROTOTYPE;
TERMINAL_SCAN_PROTOTYPE;
TERMINAL_PUT_CHAR_PROTOTYPE;
TERMINAL_GET_CHAR_PROTOTYPE;
TERMINAL_FLUSH_PROTOTYPE;
TERMINAL_CURSOR_GET_POSITION_PROTOTYPE;
TERMINAL_CURSOR_SET_POSITION_PROTOTYPE;
TERMINAL_CURSOR_MOVE_PROTOTYPE;
TERMINAL_CURSOR_SHOW_PROTOTYPE;
TERMINAL_CURSOR_HIDE_PROTOTYPE;
TERMINAL_SIZE_GET_PROTOTYPE;
TERMINAL_CLEAR_PROTOTYPE;
TERMINAL_CLEAR_LINE_PROTOTYPE;
TERMINAL_TITLE_SET_PROTOTYPE;
TERMINAL_BELL_PROTOTYPE;
TERMINAL_STYLE_SET_PROTOTYPE;
TERMINAL_STYLE_GET_PROTOTYPE;
TERMINAL_STYLE_RESET_PROTOTYPE;
TERMINAL_COLOR_SET_PROTOTYPE;
TERMINAL_COLOR_RESET_PROTOTYPE;
TERMINAL_ATTRIBUTE_SET_PROTOTYPE;
TERMINAL_ATTRIBUTE_ADD_PROTOTYPE;
TERMINAL_ATTRIBUTE_REMOVE_PROTOTYPE;
TERMINAL_ATTRIBUTE_RESET_PROTOTYPE;
TERMINAL_EVENT_POLL_PROTOTYPE;
TERMINAL_MOUSE_ENABLE_PROTOTYPE;
TERMINAL_KEY_GET_PROTOTYPE;
TERMINAL_KEY_GET_ECHO_PROTOTYPE;
TERMINAL_KEY_HIT_PROTOTYPE;
TERMINAL_INPUT_MODE_GET_PROTOTYPE;
TERMINAL_INPUT_MODE_SET_PROTOTYPE;

#include "Impl/Terminal.impl"
