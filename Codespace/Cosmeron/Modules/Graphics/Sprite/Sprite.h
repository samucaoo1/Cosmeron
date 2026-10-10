#pragma once
#include "../Rasterization/Rasterization.h"
#include "../../Math/Value/Clamp.h"
#include <float.h>

#define SPRITE_MOD Sprite
#ifndef SPRITE_NS
#define SPRITE_NS(NAME) GNS2(LIB_PREFIX(SPRITE_MOD), NAME)
#endif
#define SPRITE_INS(NAME) GNS3(LIB_PREFIX(SPRITE_MOD), Internal, NAME)
#define SPRITE_TYPE(NAME) SPRITE_NS(NAME)
#define SPRITE_FUNC(NAME) SPRITE_NS(NAME)

/* A layer borrows a Canvas; no separate sprite pixel storage. */
typedef struct SPRITE_TYPE(TLayer) {
  const TERMINAL_TYPE(TCanvas) *source;
  TQUAD_TYPE(uint16) region; /* Inclusive source crop. */
  TDUAL_TYPE(int32) position;
  TDUAL_TYPE(uint16) size;
  bool flipX,flipY,useColorKey;
  uint32_t colorKey;
} SPRITE_TYPE(TLayer);
typedef struct SPRITE_TYPE(TFrame) {
  const TERMINAL_TYPE(TCanvas) *source;
  TQUAD_TYPE(uint16) region;
  uint64_t duration;
} SPRITE_TYPE(TFrame);
typedef struct SPRITE_TYPE(TAnimation) {
  const SPRITE_TYPE(TFrame) *frames;
  size_t count;
  bool loop;
} SPRITE_TYPE(TAnimation);

#define SPRITE_LAYER_PROTOTYPE \
  static inline SPRITE_TYPE(TLayer) SPRITE_FUNC(Layer)(const TERMINAL_TYPE(TCanvas) *source)
#define SPRITE_DRAW_PROTOTYPE \
  static inline OPSTATUS SPRITE_FUNC(Draw)( \
      TERMINAL_TYPE(TCanvas) *canvas, const TERMINAL_TYPE(TCanvas) *source, \
      TDUAL_TYPE(int32) position)
#define SPRITE_DRAW_WITH_PROTOTYPE \
  static inline OPSTATUS SPRITE_FUNC(DrawWith)( \
      TERMINAL_TYPE(TCanvas) *canvas, const SPRITE_TYPE(TLayer) *layer)
#define SPRITE_COMPOSE_PROTOTYPE \
  static inline OPSTATUS SPRITE_FUNC(Compose)( \
      TERMINAL_TYPE(TCanvas) *canvas, const SPRITE_TYPE(TLayer) *layers, size_t count, \
      TDUAL_TYPE(int32) camera)
#define SPRITE_FRAME_AT_PROTOTYPE \
  static inline OPSTATUS SPRITE_FUNC(FrameAt)( \
      const SPRITE_TYPE(TAnimation) *animation, uint64_t elapsed, size_t *outIndex)
#define SPRITE_DRAW_FRAME_PROTOTYPE \
  static inline OPSTATUS SPRITE_FUNC(DrawFrame)( \
      TERMINAL_TYPE(TCanvas) *canvas, const SPRITE_TYPE(TAnimation) *animation, \
      uint64_t elapsed, const SPRITE_TYPE(TLayer) *placement)
SPRITE_LAYER_PROTOTYPE;
SPRITE_DRAW_PROTOTYPE;
SPRITE_DRAW_WITH_PROTOTYPE;
SPRITE_COMPOSE_PROTOTYPE;
SPRITE_FRAME_AT_PROTOTYPE;
SPRITE_DRAW_FRAME_PROTOTYPE;
#include "Impl/Sprite.impl"
