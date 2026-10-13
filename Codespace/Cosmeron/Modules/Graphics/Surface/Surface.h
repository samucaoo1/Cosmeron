#pragma once
#include <stdbool.h>
#include <string.h>
#include "../../../Core/Namespace.h"
#include "../../../Core/Memory/Alloc.h"
#include "../../Struct/TDual.h"
#include "../../Struct/TQuad.h"
#include "../Internal/Primitives.inc"

#define GRAPHICS_MOD Graphics
#ifndef GRAPHICS_NS
#define GRAPHICS_NS(NAME) GNS2(LIB_PREFIX(GRAPHICS_MOD), NAME)
#endif
#define GRAPHICS_TYPE(NAME) GRAPHICS_NS(NAME)
#define GRAPHICS_FUNC(NAME) GRAPHICS_NS(NAME)
#define GRAPHICS_INS(NAME) GNS3(LIB_PREFIX(GRAPHICS_MOD), Internal, NAME)

/* Numeric 0xAARRGGBB pixels; stride and capacity are in pixels, not bytes.
 * Views borrow storage. Resizing/freeing the owner invalidates all views.
 * Create/Borrow/View require a zero-initialized output with no live storage. */
typedef struct GRAPHICS_TYPE(TSurface) {
  uint32_t *pixels;
  TDUAL_TYPE(uint32) size;
  size_t stride, capacity;
  bool owns;
} GRAPHICS_TYPE(TSurface);

#define GRAPHICS_SURFACE_IS_VALID_PROTOTYPE                                                        \
  static inline bool GRAPHICS_FUNC(Surface_IsValid)(const GRAPHICS_TYPE(TSurface) * surface)
#define GRAPHICS_SURFACE_CREATE_PROTOTYPE                                                          \
  static inline OPSTATUS GRAPHICS_FUNC(Surface_Create)(GRAPHICS_TYPE(TSurface) * outSurface,       \
                                                       TDUAL_TYPE(uint32) size)
#define GRAPHICS_SURFACE_BORROW_PROTOTYPE                                                          \
  static inline OPSTATUS GRAPHICS_FUNC(Surface_Borrow)(GRAPHICS_TYPE(TSurface) * outSurface,       \
                                                       uint32_t * pixels, TDUAL_TYPE(uint32) size, \
                                                       size_t stride, size_t capacity)
#define GRAPHICS_SURFACE_FREE_PROTOTYPE                                                            \
  static inline void GRAPHICS_FUNC(Surface_Free)(GRAPHICS_TYPE(TSurface) * surface)
#define GRAPHICS_SURFACE_RESIZE_PROTOTYPE                                                          \
  static inline OPSTATUS GRAPHICS_FUNC(Surface_Resize)(GRAPHICS_TYPE(TSurface) * surface,          \
                                                       TDUAL_TYPE(uint32) size)
#define GRAPHICS_SURFACE_VIEW_PROTOTYPE                                                            \
  static inline OPSTATUS GRAPHICS_FUNC(Surface_View)(GRAPHICS_TYPE(TSurface) * source,             \
                                                     TQUAD_TYPE(uint32) region,                    \
                                                     GRAPHICS_TYPE(TSurface) * outView)
#define GRAPHICS_SURFACE_CLEAR_PROTOTYPE                                                           \
  static inline OPSTATUS GRAPHICS_FUNC(Surface_Clear)(GRAPHICS_TYPE(TSurface) * surface,           \
                                                      uint32_t color)
#define GRAPHICS_SURFACE_BLEND_PROTOTYPE                                                           \
  static inline uint32_t GRAPHICS_FUNC(Surface_Blend)(uint32_t destination, uint32_t source)

#define GRAPHICS_SURFACE_OVERLAPS_PROTOTYPE                                                        \
  static inline bool GRAPHICS_FUNC(Surface_Overlaps)(const GRAPHICS_TYPE(TSurface) * left,         \
                                                     const GRAPHICS_TYPE(TSurface) * right)
GRAPHICS_SURFACE_OVERLAPS_PROTOTYPE;
#define GRAPHICS_SURFACE_LINE_PROTOTYPE                                                            \
  static inline OPSTATUS GRAPHICS_FUNC(Surface_Line)(GRAPHICS_TYPE(TSurface) * surface,            \
                                                     TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) to, \
                                                     uint32_t color)
GRAPHICS_SURFACE_LINE_PROTOTYPE;
GRAPHICS_SURFACE_IS_VALID_PROTOTYPE;
GRAPHICS_SURFACE_CREATE_PROTOTYPE;
GRAPHICS_SURFACE_BORROW_PROTOTYPE;
GRAPHICS_SURFACE_FREE_PROTOTYPE;
GRAPHICS_SURFACE_RESIZE_PROTOTYPE;
GRAPHICS_SURFACE_VIEW_PROTOTYPE;
GRAPHICS_SURFACE_CLEAR_PROTOTYPE;
GRAPHICS_SURFACE_BLEND_PROTOTYPE;
#include "Impl/Surface.impl"
