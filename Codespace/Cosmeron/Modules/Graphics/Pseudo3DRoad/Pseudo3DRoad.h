#pragma once
#include "../../../Core/Error/Status.h"
#include "../../Struct/TDual.h"
#include "../../Struct/TTriple.h"
#include <float.h>
#include <stdbool.h>

#define ROAD_MOD Road
#ifndef ROAD_NS
#define ROAD_NS(NAME) GNS2(LIB_PREFIX(ROAD_MOD), NAME)
#endif
#define ROAD_INS(NAME) GNS3(LIB_PREFIX(ROAD_MOD), Internal, NAME)
#define ROAD_TYPE(NAME) ROAD_NS(NAME)
#define ROAD_FUNC(NAME) ROAD_NS(NAME)
/* Each adjacent pair of nodes defines one trapezoidal road segment.
 * x: lateral center/curvature, y: elevation (up), z: forward distance. */
typedef struct ROAD_TYPE(TNode) {
  TTRIPLE_TYPE(float) position;
  float halfWidth;
} ROAD_TYPE(TNode);
typedef struct ROAD_TYPE(TTrack) {
  const ROAD_TYPE(TNode) *nodes;
  size_t count;
} ROAD_TYPE(TTrack);
typedef struct ROAD_TYPE(TCamera) {
  TTRIPLE_TYPE(float) position;
  float focalLength,nearClip,drawDistance,horizon;
  /* focalLength is measured in viewport heights, horizon in height fractions. */
} ROAD_TYPE(TCamera);
typedef struct ROAD_TYPE(TProjection) {
  TDUAL_TYPE(float) center;
  float halfWidth,depth,scale;
} ROAD_TYPE(TProjection);

#define ROAD_PROJECT_PROTOTYPE \
  static inline OPSTATUS ROAD_FUNC(Project)( \
      const ROAD_TYPE(TCamera) *camera, TDUAL_TYPE(uint16) viewport, \
      const ROAD_TYPE(TNode) *node, ROAD_TYPE(TProjection) *outProjection)
#define ROAD_TRACK_VALIDATE_PROTOTYPE \
  static inline OPSTATUS ROAD_FUNC(TrackValidate)(const ROAD_TYPE(TTrack) *track)
#define ROAD_TRACK_SAMPLE_PROTOTYPE \
  static inline OPSTATUS ROAD_FUNC(TrackSample)( \
      const ROAD_TYPE(TTrack) *track, float z, ROAD_TYPE(TNode) *outNode)
ROAD_TRACK_SAMPLE_PROTOTYPE;
ROAD_PROJECT_PROTOTYPE;
ROAD_TRACK_VALIDATE_PROTOTYPE;
#include "Impl/Pseudo3DRoad.impl"
