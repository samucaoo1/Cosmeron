#pragma once
#include "../../../Core/Module/Package.h"
#include "Geometry.space"

#if defined(GEOMETRY_DISABLE_FUNCTION_TABLE)
#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
#define PACKAGE_DISABLE_FUNCTION_TABLE
#endif
#endif

PACKAGE(LIB_PREFIX(GEOMETRY_MOD),
  FUNCTION(float, Distance, (GEOMETRY_TYPE(TPoint2D) a, GEOMETRY_TYPE(TPoint2D) b)),
  FUNCTION(float, DistanceSquared, (GEOMETRY_TYPE(TPoint2D) a, GEOMETRY_TYPE(TPoint2D) b)),
  FUNCTION(bool, PointInRect, (GEOMETRY_TYPE(TPoint2D) p, const GEOMETRY_TYPE(TRect2D) *rect)),
  FUNCTION(bool, PointInCircle, (GEOMETRY_TYPE(TPoint2D) p, const GEOMETRY_TYPE(TCircle2D) *circle)),
  FUNCTION(bool, PointInTriangle, (GEOMETRY_TYPE(TPoint2D) p, const GEOMETRY_TYPE(TTriangle2D) *tri)),
  FUNCTION(bool, PointInPolygon, (GEOMETRY_TYPE(TPoint2D) p, const GEOMETRY_TYPE(TPoint2D) *vertices, size_t count)),
  FUNCTION(bool, RectIntersectsRect, (const GEOMETRY_TYPE(TRect2D) *a, const GEOMETRY_TYPE(TRect2D) *b)),
  FUNCTION(bool, RectIntersection, (const GEOMETRY_TYPE(TRect2D) *a, const GEOMETRY_TYPE(TRect2D) *b, GEOMETRY_TYPE(TRect2D) *outRect)),
  FUNCTION(bool, CircleIntersectsCircle, (const GEOMETRY_TYPE(TCircle2D) *a, const GEOMETRY_TYPE(TCircle2D) *b)),
  FUNCTION(bool, LineIntersectsLine, (const GEOMETRY_TYPE(TLine2D) *a, const GEOMETRY_TYPE(TLine2D) *b, GEOMETRY_TYPE(TPoint2D) *outIntersection)),
  FUNCTION(bool, RayIntersectsCircle, (const GEOMETRY_TYPE(TRay2D) *ray, const GEOMETRY_TYPE(TCircle2D) *circle, GEOMETRY_TYPE(THit2D) *outHit)),
  FUNCTION(bool, RayIntersectsRect, (const GEOMETRY_TYPE(TRay2D) *ray, const GEOMETRY_TYPE(TRect2D) *rect, GEOMETRY_TYPE(THit2D) *outHit)),
  FUNCTION(float, TriangleArea, (const GEOMETRY_TYPE(TTriangle2D) *tri)),
  FUNCTION(float, PolygonArea, (const GEOMETRY_TYPE(TPoint2D) *vertices, size_t count)),
  FUNCTION(OPSTATUS, PolygonCentroid, (const GEOMETRY_TYPE(TPoint2D) *vertices, size_t count, GEOMETRY_TYPE(TPoint2D) *outCentroid)),
  FUNCTION(OPSTATUS, BoundingBox, (const GEOMETRY_TYPE(TPoint2D) *vertices, size_t count, GEOMETRY_TYPE(TBoundingBox2D) *outBox))
);

#define GEOMETRY_DISTANCE_PROTOTYPE \
  static inline float GEOMETRY_FUNC(Distance)( \
      GEOMETRY_TYPE(TPoint2D) a, GEOMETRY_TYPE(TPoint2D) b)
GEOMETRY_DISTANCE_PROTOTYPE;

#define GEOMETRY_DISTANCE_SQUARED_PROTOTYPE \
  static inline float GEOMETRY_FUNC(DistanceSquared)( \
      GEOMETRY_TYPE(TPoint2D) a, GEOMETRY_TYPE(TPoint2D) b)
GEOMETRY_DISTANCE_SQUARED_PROTOTYPE;

#define GEOMETRY_POINT_IN_RECT_PROTOTYPE \
  static inline bool GEOMETRY_FUNC(PointInRect)( \
      GEOMETRY_TYPE(TPoint2D) p, const GEOMETRY_TYPE(TRect2D) *rect)
GEOMETRY_POINT_IN_RECT_PROTOTYPE;

#define GEOMETRY_POINT_IN_CIRCLE_PROTOTYPE \
  static inline bool GEOMETRY_FUNC(PointInCircle)( \
      GEOMETRY_TYPE(TPoint2D) p, const GEOMETRY_TYPE(TCircle2D) *circle)
GEOMETRY_POINT_IN_CIRCLE_PROTOTYPE;

#define GEOMETRY_POINT_IN_TRIANGLE_PROTOTYPE \
  static inline bool GEOMETRY_FUNC(PointInTriangle)( \
      GEOMETRY_TYPE(TPoint2D) p, const GEOMETRY_TYPE(TTriangle2D) *tri)
GEOMETRY_POINT_IN_TRIANGLE_PROTOTYPE;

#define GEOMETRY_POINT_IN_POLYGON_PROTOTYPE \
  static inline bool GEOMETRY_FUNC(PointInPolygon)( \
      GEOMETRY_TYPE(TPoint2D) p, const GEOMETRY_TYPE(TPoint2D) *vertices, size_t count)
GEOMETRY_POINT_IN_POLYGON_PROTOTYPE;

#define GEOMETRY_RECT_INTERSECTS_RECT_PROTOTYPE \
  static inline bool GEOMETRY_FUNC(RectIntersectsRect)( \
      const GEOMETRY_TYPE(TRect2D) *a, const GEOMETRY_TYPE(TRect2D) *b)
GEOMETRY_RECT_INTERSECTS_RECT_PROTOTYPE;

#define GEOMETRY_RECT_INTERSECTION_PROTOTYPE \
  static inline bool GEOMETRY_FUNC(RectIntersection)( \
      const GEOMETRY_TYPE(TRect2D) *a, const GEOMETRY_TYPE(TRect2D) *b, \
      GEOMETRY_TYPE(TRect2D) *outRect)
GEOMETRY_RECT_INTERSECTION_PROTOTYPE;

#define GEOMETRY_CIRCLE_INTERSECTS_CIRCLE_PROTOTYPE \
  static inline bool GEOMETRY_FUNC(CircleIntersectsCircle)( \
      const GEOMETRY_TYPE(TCircle2D) *a, const GEOMETRY_TYPE(TCircle2D) *b)
GEOMETRY_CIRCLE_INTERSECTS_CIRCLE_PROTOTYPE;

#define GEOMETRY_LINE_INTERSECTS_LINE_PROTOTYPE \
  static inline bool GEOMETRY_FUNC(LineIntersectsLine)( \
      const GEOMETRY_TYPE(TLine2D) *a, const GEOMETRY_TYPE(TLine2D) *b, \
      GEOMETRY_TYPE(TPoint2D) *outIntersection)
GEOMETRY_LINE_INTERSECTS_LINE_PROTOTYPE;

#define GEOMETRY_RAY_INTERSECTS_CIRCLE_PROTOTYPE \
  static inline bool GEOMETRY_FUNC(RayIntersectsCircle)( \
      const GEOMETRY_TYPE(TRay2D) *ray, const GEOMETRY_TYPE(TCircle2D) *circle, \
      GEOMETRY_TYPE(THit2D) *outHit)
GEOMETRY_RAY_INTERSECTS_CIRCLE_PROTOTYPE;

#define GEOMETRY_RAY_INTERSECTS_RECT_PROTOTYPE \
  static inline bool GEOMETRY_FUNC(RayIntersectsRect)( \
      const GEOMETRY_TYPE(TRay2D) *ray, const GEOMETRY_TYPE(TRect2D) *rect, \
      GEOMETRY_TYPE(THit2D) *outHit)
GEOMETRY_RAY_INTERSECTS_RECT_PROTOTYPE;

#define GEOMETRY_TRIANGLE_AREA_PROTOTYPE \
  static inline float GEOMETRY_FUNC(TriangleArea)( \
      const GEOMETRY_TYPE(TTriangle2D) *tri)
GEOMETRY_TRIANGLE_AREA_PROTOTYPE;

#define GEOMETRY_POLYGON_AREA_PROTOTYPE \
  static inline float GEOMETRY_FUNC(PolygonArea)( \
      const GEOMETRY_TYPE(TPoint2D) *vertices, size_t count)
GEOMETRY_POLYGON_AREA_PROTOTYPE;

#define GEOMETRY_POLYGON_CENTROID_PROTOTYPE \
  static inline OPSTATUS GEOMETRY_FUNC(PolygonCentroid)( \
      const GEOMETRY_TYPE(TPoint2D) *vertices, size_t count, \
      GEOMETRY_TYPE(TPoint2D) *outCentroid)
GEOMETRY_POLYGON_CENTROID_PROTOTYPE;

#define GEOMETRY_BOUNDING_BOX_PROTOTYPE \
  static inline OPSTATUS GEOMETRY_FUNC(BoundingBox)( \
      const GEOMETRY_TYPE(TPoint2D) *vertices, size_t count, \
      GEOMETRY_TYPE(TBoundingBox2D) *outBox)
GEOMETRY_BOUNDING_BOX_PROTOTYPE;

#include "Impl/Geometry.impl"
