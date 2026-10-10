#pragma once
#include "Pseudo3DRoad.h"
#include "../Rasterization/Rasterization.h"
#include "../../Math/Value/Clamp.h"

typedef struct ROAD_TYPE(TStyle) {
  uint32_t codepoint;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) sky,grass[2],road[2],edge[2],lane;
  uint16_t lanes;
  float edgeFraction,laneFraction; /* Fractions of the full road width. */
  size_t stripeSegments; /* Alternate palettes every N segments; nonzero. */
} ROAD_TYPE(TStyle);
#define ROAD_STYLE_DEFAULT_PROTOTYPE \
  static inline ROAD_TYPE(TStyle) ROAD_FUNC(StyleDefault)(void)
#define ROAD_RENDER_PROTOTYPE \
  static inline OPSTATUS ROAD_FUNC(Render)( \
      TERMINAL_TYPE(TCanvas) *canvas, const ROAD_TYPE(TTrack) *track, \
      const ROAD_TYPE(TCamera) *camera, const ROAD_TYPE(TStyle) *style)
ROAD_STYLE_DEFAULT_PROTOTYPE;
ROAD_RENDER_PROTOTYPE;
#include "Impl/Render.impl"
