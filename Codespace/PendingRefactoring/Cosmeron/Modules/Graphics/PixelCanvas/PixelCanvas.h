#pragma once
#include "../../../Core/Module/Object.h"
#include "PixelCanvas.space"

#if defined(PIXEL_CANVAS_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

OBJECT(TPixelCanvas,
  FIELDS(
    uint32_t *pixels;
    uint16_t width;
    uint16_t height;
    bool owns;
  ),
  METHOD(OPSTATUS, Create, (uint16_t width, uint16_t height)),
  METHOD(void, Destroy, ()),
  METHOD(OPSTATUS, Clear, (uint32_t color)),
  METHOD(OPSTATUS, SetPixel, (uint16_t x, uint16_t y, uint32_t color)),
  METHOD(OPSTATUS, GetPixel, (uint16_t x, uint16_t y, uint32_t *outColor)),
  METHOD(OPSTATUS, DrawLine, (int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)),
  METHOD(OPSTATUS, DrawRect, (int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color, bool fill)),
  METHOD(OPSTATUS, DrawCircle, (int32_t cx, int32_t cy, int32_t r, uint32_t color, bool fill)),
  METHOD(OPSTATUS, RenderToCanvas, (TERMINAL_TYPE(TCanvas) *destCanvas, TDUAL_TYPE(uint16) destOffset)),
  METHOD(uint16_t, Width, ()),
  METHOD(uint16_t, Height, ())
);

#define PIXEL_CANVAS_CREATE_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, Create, (uint16_t width, uint16_t height))
#define PIXEL_CANVAS_DESTROY_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, Destroy, ())
#define PIXEL_CANVAS_CLEAR_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, Clear, (uint32_t color))
#define PIXEL_CANVAS_SET_PIXEL_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, SetPixel, (uint16_t x, uint16_t y, uint32_t color))
#define PIXEL_CANVAS_GET_PIXEL_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, GetPixel, (uint16_t x, uint16_t y, uint32_t *outColor))
#define PIXEL_CANVAS_DRAW_LINE_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, DrawLine, (int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color))
#define PIXEL_CANVAS_DRAW_RECT_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, DrawRect, (int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color, bool fill))
#define PIXEL_CANVAS_DRAW_CIRCLE_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, DrawCircle, (int32_t cx, int32_t cy, int32_t r, uint32_t color, bool fill))
#define PIXEL_CANVAS_RENDER_TO_CANVAS_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, RenderToCanvas, (TERMINAL_TYPE(TCanvas) *destCanvas, TDUAL_TYPE(uint16) destOffset))
#define PIXEL_CANVAS_WIDTH_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, Width, ())
#define PIXEL_CANVAS_HEIGHT_PROTOTYPE OBJECT_PROTOTYPE(TPixelCanvas, Height, ())

#include "Impl/PixelCanvas.impl"
