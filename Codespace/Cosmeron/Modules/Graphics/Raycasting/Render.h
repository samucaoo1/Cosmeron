#pragma once
#include "Raycasting.h"
#include "../Sprite/Sprite.h"
#include "../../Math/Value/Clamp.h"

typedef struct RAYCAST_TYPE(TScene) {
  RAYCAST_TYPE(TGridMap) map;
  uint32_t wallCodepoint,ceilingCodepoint,floorCodepoint;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) wallStyle,sideStyle,ceilingStyle,floorStyle;
  const TERMINAL_TYPE(TCanvas) *const *textures; /* material - 1; NULL entry = flat. */
  size_t textureCount;
} RAYCAST_TYPE(TScene);
typedef struct RAYCAST_TYPE(TSprite) {
  SPRITE_TYPE(TLayer) image;
  TDUAL_TYPE(float) position,size; /* World position and positive width/height. */
  float elevation; /* Billboard bottom relative to camera eye level. */
} RAYCAST_TYPE(TSprite);
#define RAYCAST_RENDER_COLUMNS_DEPTH_PROTOTYPE \
  static inline OPSTATUS RAYCAST_FUNC(RenderColumnsDepth)( \
      TERMINAL_TYPE(TCanvas) *canvas, const RAYCAST_TYPE(TScene) *scene, \
      const RAYCAST_TYPE(TCamera) *camera, float *outDepth, size_t depthCount)
#define RAYCAST_RENDER_SPRITES_PROTOTYPE \
  static inline OPSTATUS RAYCAST_FUNC(RenderSprites)( \
      TERMINAL_TYPE(TCanvas) *canvas, const RAYCAST_TYPE(TCamera) *camera, \
      const RAYCAST_TYPE(TSprite) *sprites, size_t count, \
      const float *columnDepth, size_t depthCount)
RAYCAST_RENDER_COLUMNS_DEPTH_PROTOTYPE;
RAYCAST_RENDER_SPRITES_PROTOTYPE;
#define RAYCAST_RENDER_COLUMNS_PROTOTYPE \
  static inline OPSTATUS RAYCAST_FUNC(RenderColumns)( \
      TERMINAL_TYPE(TCanvas) *canvas, const RAYCAST_TYPE(TScene) *scene, \
      const RAYCAST_TYPE(TCamera) *camera)
RAYCAST_RENDER_COLUMNS_PROTOTYPE;
#include "Impl/Render.impl"
