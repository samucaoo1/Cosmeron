#pragma once
#include "Font.space"

/* Both parsers borrow the supplied bytes, including embedded asset arrays.
 * ASCII source characters U+0020..U+007E are supported in this iteration.
 * Non-SGR ANSI control sequences are rejected in ASF rows. */
#define FONT_PARSE_ASF_PROTOTYPE \
  static inline OPSTATUS FONT_FUNC(ParseASF)( \
      const char *data, size_t size, FONT_TYPE(TFont) *outFont)
#define FONT_PARSE_FIGLET_PROTOTYPE \
  static inline OPSTATUS FONT_FUNC(ParseFIGlet)( \
      const char *data, size_t size, FONT_TYPE(TFont) *outFont)
#define FONT_MEASURE_PROTOTYPE \
  static inline OPSTATUS FONT_FUNC(Measure)( \
      const FONT_TYPE(TFont) *font, const char *asciiText, \
      TDUAL_TYPE(uint16) *outSize)
#define FONT_DRAW_PROTOTYPE \
  static inline OPSTATUS FONT_FUNC(Draw)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(uint16) position, \
      const FONT_TYPE(TFont) *font, const char *asciiText, \
      TEXT_GRID_ATTRIBUTE_TYPE(Cell) defaultStyle)

FONT_PARSE_ASF_PROTOTYPE;
FONT_PARSE_FIGLET_PROTOTYPE;
FONT_MEASURE_PROTOTYPE;
FONT_DRAW_PROTOTYPE;

#include "Impl/Font.impl"
