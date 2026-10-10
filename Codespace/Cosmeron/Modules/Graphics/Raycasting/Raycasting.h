#pragma once
#include "../../../Core/Error/Status.h"
#include "../../Struct/TDual.h"
#include <float.h>
#include <stdbool.h>

#define RAYCAST_MOD Raycast
#define RAYCAST_CMOD RAYCAST
#ifndef RAYCAST_NS
#define RAYCAST_NS(NAME) GNS2(LIB_PREFIX(RAYCAST_MOD), NAME)
#endif
#define RAYCAST_INS(NAME) GNS3(LIB_PREFIX(RAYCAST_MOD), Internal, NAME)
#define RAYCAST_CNS(NAME) CNS2(LIB_PREFIX_CONST(RAYCAST_CMOD), NAME)
#define RAYCAST_FUNC(NAME) RAYCAST_NS(NAME)
#define RAYCAST_TYPE(NAME) RAYCAST_NS(NAME)
#define RAYCAST_CONST(NAME) RAYCAST_CNS(NAME)

typedef enum RAYCAST_TYPE(Side) {
  RAYCAST_CONST(SIDE_NONE), RAYCAST_CONST(SIDE_X), RAYCAST_CONST(SIDE_Y)
} RAYCAST_TYPE(Side);
/* Borrowed row-major cells: 0 is empty, other values are material IDs. */
typedef struct RAYCAST_TYPE(TGridMap) {
  const uint16_t *cells;
  TDUAL_TYPE(uint16) size;
  size_t stride,cellCount;
} RAYCAST_TYPE(TGridMap);
typedef struct RAYCAST_TYPE(TRay2D) {
  TDUAL_TYPE(float) origin,direction;
  float maxDistance; /* Limit of t in origin + t * direction. */
} RAYCAST_TYPE(TRay2D);
typedef struct RAYCAST_TYPE(THit) {
  TDUAL_TYPE(int32) cell;
  TDUAL_TYPE(float) position,normal;
  float distance,wallCoordinate;
  uint16_t material;
  RAYCAST_TYPE(Side) side;
} RAYCAST_TYPE(THit);
typedef struct RAYCAST_TYPE(TCamera) {
  TDUAL_TYPE(float) position,direction,plane;
  float maxDistance,wallHeight,horizon;
  /* direction must be unit length; plane perpendicular, nonzero.
   * |plane| = tan(horizontal FOV / 2). Horizon is a height fraction. */
} RAYCAST_TYPE(TCamera);

#define RAYCAST_GRID_CAST_PROTOTYPE \
  static inline OPSTATUS RAYCAST_FUNC(GridCast)( \
      const RAYCAST_TYPE(TGridMap) *map, const RAYCAST_TYPE(TRay2D) *ray, \
      RAYCAST_TYPE(THit) *outHit)
#define RAYCAST_CAMERA_RAY_PROTOTYPE \
  static inline OPSTATUS RAYCAST_FUNC(CameraRay)( \
      const RAYCAST_TYPE(TCamera) *camera, uint16_t column, uint16_t columns, \
      RAYCAST_TYPE(TRay2D) *outRay)
RAYCAST_GRID_CAST_PROTOTYPE;
RAYCAST_CAMERA_RAY_PROTOTYPE;
#include "Impl/Raycasting.impl"
