#pragma once
#include "FIGlet.space"
#define FIGLET_PARSE_PROTOTYPE \
  static inline OPSTATUS FIGLET_FUNC(Parse)( \
      const char *data,size_t size,FIGLET_TYPE(TFont)*outFont)
#define FIGLET_MEASURE_PROTOTYPE \
  static inline OPSTATUS FIGLET_FUNC(Measure)( \
      const FIGLET_TYPE(TFont)*font,const char*asciiText,TDUAL_TYPE(uint16)*outSize)
#define FIGLET_DRAW_PROTOTYPE \
  static inline OPSTATUS FIGLET_FUNC(Draw)( \
      TERMINAL_TYPE(TCanvas)*canvas,TDUAL_TYPE(uint16)position, \
      const FIGLET_TYPE(TFont)*font,const char*asciiText, \
      TEXT_GRID_ATTRIBUTE_TYPE(Cell)defaultStyle)
FIGLET_PARSE_PROTOTYPE;
FIGLET_MEASURE_PROTOTYPE;
FIGLET_DRAW_PROTOTYPE;
#include "Impl/FIGlet.impl"
