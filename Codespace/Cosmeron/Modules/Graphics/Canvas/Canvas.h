#pragma once
#include "../Terminal/Terminal.h"
#include "../../Text/Grid/Grid.h"
#include "../../Text/Unicode/Width.h"
#include "../../Struct/TQuad.h"

/*
 * Canvas owns Text/Grid only when created or copied. Views borrow the backing
 * grid and are invalidated when the owner is resized or freed.
 * No separate geometry, color or cell types.
 */
typedef struct TERMINAL_TYPE(TCanvas) {
  TEXT_GRID_TYPE(char32) grid;
  TEXT_GRID_TYPE(char32) *backing;
  TDUAL_TYPE(uint16) origin;
  TDUAL_TYPE(uint16) size;
  TDUAL_TYPE(uint16) cursor;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  bool owns;
} TERMINAL_TYPE(TCanvas);

#define TERMINAL_CANVAS_CREATE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Create)( \
      TERMINAL_TYPE(TCanvas) *outCanvas, TDUAL_TYPE(uint16) size)

#define TERMINAL_CANVAS_FREE_PROTOTYPE \
  static inline void TERMINAL_FUNC(Canvas_Free)( \
      TERMINAL_TYPE(TCanvas) *canvas)

#define TERMINAL_CANVAS_RESIZE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Resize)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) size)

#define TERMINAL_CANVAS_GET_SIZE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_GetSize)( \
      const TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) *outSize)

#define TERMINAL_CANVAS_PRINT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Print)( \
      TERMINAL_TYPE(TCanvas) *canvas, const char *format, ...)

#define TERMINAL_CANVAS_PRINT_LN_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_PrintLn)( \
      TERMINAL_TYPE(TCanvas) *canvas, const char *format, ...)

#define TERMINAL_CANVAS_PRINT_AT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_PrintAt)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position, \
      const char *format, ...)

#define TERMINAL_CANVAS_PRINT_STYLED_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_PrintStyled)( \
      TERMINAL_TYPE(TCanvas) *canvas, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, \
      const char *format, ...)

#define TERMINAL_CANVAS_PRINT_STYLED_LN_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_PrintStyledLn)( \
      TERMINAL_TYPE(TCanvas) *canvas, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, \
      const char *format, ...)

#define TERMINAL_CANVAS_PRINT_STYLED_AT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_PrintStyledAt)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position, \
      TEXT_GRID_ATTRIBUTE_TYPE(Cell) style, const char *format, ...)

#define TERMINAL_CANVAS_WRITE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Write)( \
      TERMINAL_TYPE(TCanvas) *canvas, const char *utf8Text, size_t byteLength)

#define TERMINAL_CANVAS_WRITE_LINE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_WriteLine)( \
      TERMINAL_TYPE(TCanvas) *canvas, const char *text)

#define TERMINAL_CANVAS_PUT_CHAR_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_PutChar)( \
      TERMINAL_TYPE(TCanvas) *canvas, uint32_t codepoint)

#define TERMINAL_CANVAS_CURSOR_GET_POSITION_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Cursor_GetPosition)( \
      const TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) *outPosition)

#define TERMINAL_CANVAS_CURSOR_SET_POSITION_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Cursor_SetPosition)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position)

#define TERMINAL_CANVAS_CURSOR_MOVE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Cursor_Move)( \
      TERMINAL_TYPE(TCanvas) *canvas, int32_t deltaX, int32_t deltaY)

#define TERMINAL_CANVAS_STYLE_SET_PROTOTYPE \
  static inline void TERMINAL_FUNC(Canvas_Style_Set)( \
      TERMINAL_TYPE(TCanvas) *canvas, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define TERMINAL_CANVAS_STYLE_RESET_PROTOTYPE \
  static inline void TERMINAL_FUNC(Canvas_Style_Reset)( \
      TERMINAL_TYPE(TCanvas) *canvas)

#define TERMINAL_CANVAS_ATTRIBUTE_SET_PROTOTYPE \
  static inline void TERMINAL_FUNC(Canvas_Attribute_Set)( \
      TERMINAL_TYPE(TCanvas) *canvas, TEXT_GRID_ATTRIBUTE_TYPE(Flags) flags)

#define TERMINAL_CANVAS_GET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Get)( \
      const TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position, \
      uint32_t *outCodepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) *outAttribute)

#define TERMINAL_CANVAS_SET_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Set)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute)

#define TERMINAL_CANVAS_CLEAR_PROTOTYPE \
  static inline void TERMINAL_FUNC(Canvas_Clear)( \
      TERMINAL_TYPE(TCanvas) *canvas)

#define TERMINAL_CANVAS_FILL_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Fill)( \
      TERMINAL_TYPE(TCanvas) *canvas, uint32_t codepoint, \
      TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute)

#define TERMINAL_CANVAS_FILL_REGION_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_FillRegion)( \
      TERMINAL_TYPE(TCanvas) *canvas, TQUAD_TYPE(uint16) region, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute)

#define TERMINAL_CANVAS_COPY_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Copy)( \
      const TERMINAL_TYPE(TCanvas) *source, TERMINAL_TYPE(TCanvas) *outCanvas)

#define TERMINAL_CANVAS_CROP_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Crop)( \
      const TERMINAL_TYPE(TCanvas) *source, TQUAD_TYPE(uint16) region, \
      TERMINAL_TYPE(TCanvas) *outCanvas)

#define TERMINAL_CANVAS_VIEW_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_View)( \
      TERMINAL_TYPE(TCanvas) *source, TQUAD_TYPE(uint16) region, \
      TERMINAL_TYPE(TCanvas) *outView)

#define TERMINAL_CANVAS_BLIT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Blit)( \
      TERMINAL_TYPE(TCanvas) *destination, TDUAL_TYPE(uint16) position, \
      const TERMINAL_TYPE(TCanvas) *source)

#define TERMINAL_CANVAS_BLIT_REGION_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_BlitRegion)( \
      TERMINAL_TYPE(TCanvas) *destination, TDUAL_TYPE(uint16) position, \
      const TERMINAL_TYPE(TCanvas) *source, TQUAD_TYPE(uint16) region)

#define TERMINAL_CANVAS_UPDATE_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_Update)( \
      const TERMINAL_TYPE(TCanvas) *canvas)

#define TERMINAL_CANVAS_UPDATE_AT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_UpdateAt)( \
      const TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position)

#define TERMINAL_CANVAS_UPDATE_TO_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_UpdateTo)( \
      const TERMINAL_TYPE(TCanvas) *canvas, TERMINAL_TYPE(TTerminal) *terminal)

#define TERMINAL_CANVAS_UPDATE_AT_TO_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_UpdateAtTo)( \
      const TERMINAL_TYPE(TCanvas) *canvas, TERMINAL_TYPE(TTerminal) *terminal, \
      TDUAL_TYPE(uint16) position)

#define TERMINAL_CANVAS_UPDATE_DIFF_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_UpdateDiff)( \
      const TERMINAL_TYPE(TCanvas) *canvas, \
      const TERMINAL_TYPE(TCanvas) *previous)
#define TERMINAL_CANVAS_UPDATE_DIFF_AT_PROTOTYPE \
  static inline OPSTATUS TERMINAL_FUNC(Canvas_UpdateDiffAt)( \
      const TERMINAL_TYPE(TCanvas) *canvas, \
      const TERMINAL_TYPE(TCanvas) *previous, TDUAL_TYPE(uint16) position)

TERMINAL_CANVAS_UPDATE_DIFF_PROTOTYPE;
TERMINAL_CANVAS_UPDATE_DIFF_AT_PROTOTYPE;
TERMINAL_CANVAS_CREATE_PROTOTYPE;
TERMINAL_CANVAS_FREE_PROTOTYPE;
TERMINAL_CANVAS_RESIZE_PROTOTYPE;
TERMINAL_CANVAS_GET_SIZE_PROTOTYPE;
TERMINAL_CANVAS_PRINT_PROTOTYPE;
TERMINAL_CANVAS_PRINT_LN_PROTOTYPE;
TERMINAL_CANVAS_PRINT_AT_PROTOTYPE;
TERMINAL_CANVAS_PRINT_STYLED_PROTOTYPE;
TERMINAL_CANVAS_PRINT_STYLED_LN_PROTOTYPE;
TERMINAL_CANVAS_PRINT_STYLED_AT_PROTOTYPE;
TERMINAL_CANVAS_WRITE_PROTOTYPE;
TERMINAL_CANVAS_WRITE_LINE_PROTOTYPE;
TERMINAL_CANVAS_PUT_CHAR_PROTOTYPE;
TERMINAL_CANVAS_CURSOR_GET_POSITION_PROTOTYPE;
TERMINAL_CANVAS_CURSOR_SET_POSITION_PROTOTYPE;
TERMINAL_CANVAS_CURSOR_MOVE_PROTOTYPE;
TERMINAL_CANVAS_STYLE_SET_PROTOTYPE;
TERMINAL_CANVAS_STYLE_RESET_PROTOTYPE;
TERMINAL_CANVAS_ATTRIBUTE_SET_PROTOTYPE;
TERMINAL_CANVAS_GET_PROTOTYPE;
TERMINAL_CANVAS_SET_PROTOTYPE;
TERMINAL_CANVAS_CLEAR_PROTOTYPE;
TERMINAL_CANVAS_FILL_PROTOTYPE;
TERMINAL_CANVAS_FILL_REGION_PROTOTYPE;
TERMINAL_CANVAS_COPY_PROTOTYPE;
TERMINAL_CANVAS_CROP_PROTOTYPE;
TERMINAL_CANVAS_VIEW_PROTOTYPE;
TERMINAL_CANVAS_BLIT_PROTOTYPE;
TERMINAL_CANVAS_BLIT_REGION_PROTOTYPE;
TERMINAL_CANVAS_UPDATE_PROTOTYPE;
TERMINAL_CANVAS_UPDATE_AT_PROTOTYPE;
TERMINAL_CANVAS_UPDATE_TO_PROTOTYPE;
TERMINAL_CANVAS_UPDATE_AT_TO_PROTOTYPE;

#include "Impl/Canvas.impl"
