#include <assert.h>
#include <math.h>
#include "../../Cosmeron/Modules/Graphics/Transform/Transform.h"

#define OK STATUS_CONST(SUCCESS)

int main(void) {
  TTransform2D t = {0};
  assert(TTransform2D_Init(&t) == OK);
  assert(t.api->Identity(&t) == OK);
  assert(fabsf(t.api->Determinant(&t) - 1.0f) < 1e-5f);

  /* Translation */
  assert(t.api->Translate(&t, 10.0f, 20.0f) == OK);
  GEOMETRY_TYPE(TPoint2D) origin = {.x = 0.0f, .y = 0.0f};
  GEOMETRY_TYPE(TPoint2D) pt = {0};
  assert(t.api->ApplyPoint(&t, origin, &pt) == OK);
  assert(fabsf(pt.x - 10.0f) < 1e-5f);
  assert(fabsf(pt.y - 20.0f) < 1e-5f);

  /* Vectors ignore translation */
  GEOMETRY_TYPE(TPoint2D) vec = {.x = 1.0f, .y = 2.0f};
  GEOMETRY_TYPE(TPoint2D) outVec = {0};
  assert(t.api->ApplyVector(&t, vec, &outVec) == OK);
  assert(fabsf(outVec.x - 1.0f) < 1e-5f);
  assert(fabsf(outVec.y - 2.0f) < 1e-5f);

  /* Scaling */
  TTransform2D s = {0};
  assert(TTransform2D_Init(&s) == OK);
  assert(s.api->Identity(&s) == OK);
  assert(s.api->Scale(&s, 2.0f, 3.0f) == OK);
  assert(s.api->ApplyPoint(&s, (GEOMETRY_TYPE(TPoint2D)){.x = 4.0f, .y = 5.0f}, &pt) == OK);
  assert(fabsf(pt.x - 8.0f) < 1e-5f);
  assert(fabsf(pt.y - 15.0f) < 1e-5f);

  /* Rotation by 90 degrees (pi/2) */
  TTransform2D r = {0};
  assert(TTransform2D_Init(&r) == OK);
  assert(r.api->Identity(&r) == OK);
  assert(r.api->Rotate(&r, 1.57079632679f) == OK);
  assert(r.api->ApplyPoint(&r, (GEOMETRY_TYPE(TPoint2D)){.x = 1.0f, .y = 0.0f}, &pt) == OK);
  assert(fabsf(pt.x - 0.0f) < 1e-4f);
  assert(fabsf(pt.y - 1.0f) < 1e-4f);

  /* Inversion */
  TTransform2D inv = {0};
  assert(TTransform2D_Init(&inv) == OK);
  assert(t.api->Invert(&t, &inv) == OK);

  GEOMETRY_TYPE(TPoint2D) back = {0};
  /* pt was (10, 20), applying inverse should yield (0, 0) */
  assert(inv.api->ApplyPoint(&inv, (GEOMETRY_TYPE(TPoint2D)){.x = 10.0f, .y = 20.0f}, &back) == OK);
  assert(fabsf(back.x) < 1e-4f);
  assert(fabsf(back.y) < 1e-4f);

  /* ApplyRect */
  GEOMETRY_TYPE(TRect2D) srcRect = {.origin = {.x = 0.0f, .y = 0.0f}, .size = {.x = 10.0f, .y = 20.0f}};
  GEOMETRY_TYPE(TRect2D) outRect = {0};
  assert(s.api->ApplyRect(&s, &srcRect, &outRect) == OK);
  assert(fabsf(outRect.size.x - 20.0f) < 1e-5f);
  assert(fabsf(outRect.size.y - 60.0f) < 1e-5f);

  return 0;
}
