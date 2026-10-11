#pragma once
#include "../../../Core/Namespace.h"
#include "../../Struct/TDual.h"
#include <stdbool.h>
#define COSMERON_MACRO_INTERNAL_GRAPHICS_SHARED(NAME) GNS3(LIB_PREFIX(Graphics), Shared, NAME)
/* Canonical Bresenham walk, clipped by major axis; reversal preserves ties. */
static inline void COSMERON_MACRO_INTERNAL_GRAPHICS_SHARED(WalkLine)(
    uint32_t width, uint32_t height, TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) to,
    void (*plot)(void *user, int64_t x, int64_t y), void *user) {
  int64_t dx = (int64_t)to.x - from.x, dy = (int64_t)to.y - from.y;
  bool steep = (dy < 0 ? -dy : dy) > (dx < 0 ? -dx : dx);
  int64_t a = steep ? from.y : from.x, b = steep ? from.x : from.y;
  int64_t end = steep ? to.y : to.x, other = steep ? to.x : to.y;
  int64_t first, last, p, delta, sign;
  uint64_t major, minor;
  if (a > end) {
    int64_t temp = a;
    a = end;
    end = temp;
    temp = b;
    b = other;
    other = temp;
  }
  major = (uint64_t)(end - a);
  delta = other - b;
  sign = delta < 0 ? -1 : 1;
  minor = (uint64_t)(delta < 0 ? -delta : delta);
  first = a < 0 ? 0 : a;
  last = (steep ? height : width) - 1;
  if (last > end)
    last = end;
  for (p = first; p <= last; p++) {
    int64_t q = b + (major ? sign * (int64_t)(((uint64_t)(p - a) * minor + major / 2) / major) : 0);
    {
      int64_t x = steep ? q : p, y = steep ? p : q;
      if (x >= 0 && y >= 0 && x < width && y < height)
        plot(user, x, y);
    }
  }
}
