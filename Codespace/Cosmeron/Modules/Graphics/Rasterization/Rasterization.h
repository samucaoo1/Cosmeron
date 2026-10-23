#pragma once
#include "../Canvas/Canvas.h"

#define RASTER_MOD Raster
#define RASTER_CMOD RASTER
#ifndef RASTER_NS
#define RASTER_NS(NAME) GNS2(LIB_PREFIX(RASTER_MOD), NAME)
#endif
#define RASTER_CNS(NAME) CNS2(LIB_PREFIX_CONST(RASTER_CMOD), NAME)
#define RASTER_INS(NAME) GNS3(LIB_PREFIX(RASTER_MOD), Internal, NAME)
#define RASTER_TYPE(NAME) RASTER_NS(NAME)
#define RASTER_FUNC(NAME) RASTER_NS(NAME)
#define RASTER_CONST(NAME) RASTER_CNS(NAME)

typedef enum RASTER_TYPE(LineStyle) {
  RASTER_CONST(LINE_SOLID),
  RASTER_CONST(LINE_DASHED),
  RASTER_CONST(LINE_DOTTED),
  RASTER_CONST(LINE_DASH_DOT)
} RASTER_TYPE(LineStyle);

/* Cell coordinates; clipped drawing, no terminal I/O. Brush must be width 1. */
#define RASTER_POINT_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(Point)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) at, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_LINE_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(Line)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) to, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_LINE_STYLED_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(LineStyled)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) to, \
      RASTER_TYPE(LineStyle) lineStyle, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_LINE_PATTERN_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(LinePattern)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) to, \
      uint16_t pattern, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_FLOOD_FILL_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(FloodFill)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) start, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_RECTANGLE_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(Rectangle)( \
      TERMINAL_TYPE(TCanvas) *canvas, TQUAD_TYPE(uint16) bounds, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_FILL_RECTANGLE_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(FillRectangle)( \
      TERMINAL_TYPE(TCanvas) *canvas, TQUAD_TYPE(uint16) bounds, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_CIRCLE_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(Circle)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) center, uint32_t radius, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_FILL_CIRCLE_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(FillCircle)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) center, uint32_t radius, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_ELLIPSE_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(Ellipse)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) center, TDUAL_TYPE(uint32) radii, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_FILL_ELLIPSE_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(FillEllipse)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) center, TDUAL_TYPE(uint32) radii, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_TRIANGLE_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(Triangle)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) a, TDUAL_TYPE(int32) b, TDUAL_TYPE(int32) c, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_FILL_TRIANGLE_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(FillTriangle)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) a, TDUAL_TYPE(int32) b, TDUAL_TYPE(int32) c, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_POLYLINE_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(Polyline)( \
      TERMINAL_TYPE(TCanvas) *canvas, const TDUAL_TYPE(int32) *points, size_t count, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_POLYGON_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(Polygon)( \
      TERMINAL_TYPE(TCanvas) *canvas, const TDUAL_TYPE(int32) *points, size_t count, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

#define RASTER_FILL_POLYGON_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(FillPolygon)( \
      TERMINAL_TYPE(TCanvas) *canvas, const TDUAL_TYPE(int32) *points, size_t count, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)

RASTER_POINT_PROTOTYPE;
RASTER_LINE_PROTOTYPE;
RASTER_LINE_STYLED_PROTOTYPE;
RASTER_LINE_PATTERN_PROTOTYPE;
RASTER_FLOOD_FILL_PROTOTYPE;
RASTER_RECTANGLE_PROTOTYPE;
RASTER_FILL_RECTANGLE_PROTOTYPE;
RASTER_CIRCLE_PROTOTYPE;
RASTER_FILL_CIRCLE_PROTOTYPE;
RASTER_ELLIPSE_PROTOTYPE;
RASTER_FILL_ELLIPSE_PROTOTYPE;
RASTER_TRIANGLE_PROTOTYPE;
RASTER_FILL_TRIANGLE_PROTOTYPE;
RASTER_POLYLINE_PROTOTYPE;
RASTER_POLYGON_PROTOTYPE;
RASTER_FILL_POLYGON_PROTOTYPE;

#define RASTER_ARC_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(Arc)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) center, \
      TDUAL_TYPE(uint32) radii, TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) to, \
      bool clockwise, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)
#define RASTER_BEZIER_QUADRATIC_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(BezierQuadratic)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) from, \
      TDUAL_TYPE(int32) control, TDUAL_TYPE(int32) to, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)
#define RASTER_BEZIER_CUBIC_PROTOTYPE \
  static inline OPSTATUS RASTER_FUNC(BezierCubic)( \
      TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) from, \
      TDUAL_TYPE(int32) control1, TDUAL_TYPE(int32) control2, TDUAL_TYPE(int32) to, \
      uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style)
RASTER_ARC_PROTOTYPE;
RASTER_BEZIER_QUADRATIC_PROTOTYPE;
RASTER_BEZIER_CUBIC_PROTOTYPE;
#include "Impl/Rasterization.impl"
#include "Impl/Curves.impl"
