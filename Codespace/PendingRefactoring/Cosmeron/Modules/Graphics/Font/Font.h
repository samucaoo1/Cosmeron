#pragma once
#include "Font.space"
#include "../Canvas/Canvas.h"
/* Only native compile-time static-const .asf definitions are accepted. */
#define FONT_MEASURE_PROTOTYPE \
  static inline OPSTATUS FONT_FUNC(Measure)( \
      const FONT_TYPE(TFont)*font,const char*asciiText,TDUAL_TYPE(uint16)*outSize)
#define FONT_DRAW_PROTOTYPE \
  static inline OPSTATUS FONT_FUNC(Draw)( \
      TERMINAL_TYPE(TCanvas)*canvas,TDUAL_TYPE(uint16) position, \
      const FONT_TYPE(TFont)*font,const char*asciiText, \
      TEXT_GRID_ATTRIBUTE_TYPE(Cell) defaultStyle)
FONT_MEASURE_PROTOTYPE;
FONT_DRAW_PROTOTYPE;
#include "Impl/Font.impl"
