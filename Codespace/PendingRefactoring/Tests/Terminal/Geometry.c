#include <assert.h>
#include <math.h>
#include "../../Cosmeron/Modules/Graphics/Geometry/Geometry.h"

#define OK STATUS_CONST(SUCCESS)

int main(void) {
  /* Distance & DistanceSquared */
  GEOMETRY_TYPE(TPoint2D) p1 = {.x = 0.0f, .y = 0.0f};
  GEOMETRY_TYPE(TPoint2D) p2 = {.x = 3.0f, .y = 4.0f};
  assert(fabsf(GEOMETRY_FUNC(Distance)(p1, p2) - 5.0f) < 1e-5f);
  assert(fabsf(GEOMETRY_FUNC(DistanceSquared)(p1, p2) - 25.0f) < 1e-5f);

  /* PointInRect */
  GEOMETRY_TYPE(TRect2D) rect = {.origin = {.x = 10.0f, .y = 20.0f}, .size = {.x = 30.0f, .y = 40.0f}};
  assert(GEOMETRY_FUNC(PointInRect)((GEOMETRY_TYPE(TPoint2D)){.x = 15.0f, .y = 25.0f}, &rect));
  assert(!GEOMETRY_FUNC(PointInRect)((GEOMETRY_TYPE(TPoint2D)){.x = 5.0f, .y = 25.0f}, &rect));
  assert(!GEOMETRY_FUNC(PointInRect)((GEOMETRY_TYPE(TPoint2D)){.x = 15.0f, .y = 65.0f}, &rect));

  /* PointInCircle */
  GEOMETRY_TYPE(TCircle2D) circle = {.center = {.x = 5.0f, .y = 5.0f}, .radius = 5.0f};
  assert(GEOMETRY_FUNC(PointInCircle)((GEOMETRY_TYPE(TPoint2D)){.x = 5.0f, .y = 5.0f}, &circle));
  assert(GEOMETRY_FUNC(PointInCircle)((GEOMETRY_TYPE(TPoint2D)){.x = 8.0f, .y = 9.0f}, &circle)); /* dist^2 = 3^2+4^2=25 */
  assert(!GEOMETRY_FUNC(PointInCircle)((GEOMETRY_TYPE(TPoint2D)){.x = 9.0f, .y = 9.0f}, &circle));

  /* PointInTriangle */
  GEOMETRY_TYPE(TTriangle2D) tri = {
    .a = {.x = 0.0f, .y = 0.0f},
    .b = {.x = 10.0f, .y = 0.0f},
    .c = {.x = 0.0f, .y = 10.0f}
  };
  assert(GEOMETRY_FUNC(PointInTriangle)((GEOMETRY_TYPE(TPoint2D)){.x = 2.0f, .y = 2.0f}, &tri));
  assert(!GEOMETRY_FUNC(PointInTriangle)((GEOMETRY_TYPE(TPoint2D)){.x = 8.0f, .y = 8.0f}, &tri));

  /* PointInPolygon */
  GEOMETRY_TYPE(TPoint2D) poly[4] = {
    {.x = 0.0f, .y = 0.0f},
    {.x = 10.0f, .y = 0.0f},
    {.x = 10.0f, .y = 10.0f},
    {.x = 0.0f, .y = 10.0f}
  };
  assert(GEOMETRY_FUNC(PointInPolygon)((GEOMETRY_TYPE(TPoint2D)){.x = 5.0f, .y = 5.0f}, poly, 4));
  assert(!GEOMETRY_FUNC(PointInPolygon)((GEOMETRY_TYPE(TPoint2D)){.x = 15.0f, .y = 5.0f}, poly, 4));

  /* RectIntersectsRect & RectIntersection */
  GEOMETRY_TYPE(TRect2D) r1 = {.origin = {.x = 0.0f, .y = 0.0f}, .size = {.x = 10.0f, .y = 10.0f}};
  GEOMETRY_TYPE(TRect2D) r2 = {.origin = {.x = 5.0f, .y = 5.0f}, .size = {.x = 10.0f, .y = 10.0f}};
  GEOMETRY_TYPE(TRect2D) r3 = {.origin = {.x = 20.0f, .y = 20.0f}, .size = {.x = 5.0f, .y = 5.0f}};
  assert(GEOMETRY_FUNC(RectIntersectsRect)(&r1, &r2));
  assert(!GEOMETRY_FUNC(RectIntersectsRect)(&r1, &r3));

  GEOMETRY_TYPE(TRect2D) isect = {0};
  assert(GEOMETRY_FUNC(RectIntersection)(&r1, &r2, &isect));
  assert(fabsf(isect.origin.x - 5.0f) < 1e-5f);
  assert(fabsf(isect.origin.y - 5.0f) < 1e-5f);
  assert(fabsf(isect.size.x - 5.0f) < 1e-5f);
  assert(fabsf(isect.size.y - 5.0f) < 1e-5f);

  /* CircleIntersectsCircle */
  GEOMETRY_TYPE(TCircle2D) c1 = {.center = {.x = 0.0f, .y = 0.0f}, .radius = 5.0f};
  GEOMETRY_TYPE(TCircle2D) c2 = {.center = {.x = 8.0f, .y = 0.0f}, .radius = 5.0f};
  GEOMETRY_TYPE(TCircle2D) c3 = {.center = {.x = 20.0f, .y = 0.0f}, .radius = 5.0f};
  assert(GEOMETRY_FUNC(CircleIntersectsCircle)(&c1, &c2));
  assert(!GEOMETRY_FUNC(CircleIntersectsCircle)(&c1, &c3));

  /* LineIntersectsLine */
  GEOMETRY_TYPE(TLine2D) l1 = {.start = {.x = 0.0f, .y = 5.0f}, .end = {.x = 10.0f, .y = 5.0f}};
  GEOMETRY_TYPE(TLine2D) l2 = {.start = {.x = 5.0f, .y = 0.0f}, .end = {.x = 5.0f, .y = 10.0f}};
  GEOMETRY_TYPE(TPoint2D) linePt = {0};
  assert(GEOMETRY_FUNC(LineIntersectsLine)(&l1, &l2, &linePt));
  assert(fabsf(linePt.x - 5.0f) < 1e-5f);
  assert(fabsf(linePt.y - 5.0f) < 1e-5f);

  /* RayIntersectsCircle */
  GEOMETRY_TYPE(TRay2D) ray = {.origin = {.x = -10.0f, .y = 0.0f}, .direction = {.x = 1.0f, .y = 0.0f}};
  GEOMETRY_TYPE(THit2D) hit = {0};
  assert(GEOMETRY_FUNC(RayIntersectsCircle)(&ray, &c1, &hit));
  assert(hit.hit);
  assert(fabsf(hit.distance - 5.0f) < 1e-5f);
  assert(fabsf(hit.point.x - (-5.0f)) < 1e-5f);

  /* RayIntersectsRect */
  GEOMETRY_TYPE(TRect2D) rayBox = {.origin = {.x = 5.0f, .y = -5.0f}, .size = {.x = 10.0f, .y = 10.0f}};
  assert(GEOMETRY_FUNC(RayIntersectsRect)(&ray, &rayBox, &hit));
  assert(hit.hit);
  assert(fabsf(hit.distance - 15.0f) < 1e-5f);

  /* TriangleArea */
  assert(fabsf(GEOMETRY_FUNC(TriangleArea)(&tri) - 50.0f) < 1e-5f);

  /* PolygonArea & Centroid */
  assert(fabsf(GEOMETRY_FUNC(PolygonArea)(poly, 4) - 100.0f) < 1e-5f);
  GEOMETRY_TYPE(TPoint2D) centroid = {0};
  assert(GEOMETRY_FUNC(PolygonCentroid)(poly, 4, &centroid) == OK);
  assert(fabsf(centroid.x - 5.0f) < 1e-5f);
  assert(fabsf(centroid.y - 5.0f) < 1e-5f);

  /* BoundingBox */
  GEOMETRY_TYPE(TBoundingBox2D) bbox = {0};
  assert(GEOMETRY_FUNC(BoundingBox)(poly, 4, &bbox) == OK);
  assert(bbox.min.x == 0.0f && bbox.min.y == 0.0f);
  assert(bbox.max.x == 10.0f && bbox.max.y == 10.0f);

  return 0;
}
