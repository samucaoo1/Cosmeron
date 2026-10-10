#pragma once
#include "Raycasting.h"
#include "../Rasterization/Rasterization.h"
#include "../../Math/Value/Clamp.h"

typedef struct RAYCAST_TYPE(TScene) {
  RAYCAST_TYPE(TGridMap) map;
  uint32_t wallCodepoint,ceilingCodepoint,floorCodepoint;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) wallStyle,sideStyle,ceilingStyle,floorStyle;
} RAYCAST_TYPE(TScene);
#define RAYCAST_RENDER_COLUMNS_PROTOTYPE \
  static inline OPSTATUS RAYCAST_FUNC(RenderColumns)( \
      TERMINAL_TYPE(TCanvas) *canvas, const RAYCAST_TYPE(TScene) *scene, \
      const RAYCAST_TYPE(TCamera) *camera)
RAYCAST_RENDER_COLUMNS_PROTOTYPE;
#include "Impl/Render.impl"
