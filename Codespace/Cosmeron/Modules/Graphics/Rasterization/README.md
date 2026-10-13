# Rasterization — 2D drawing over Canvas

Header-only C11 geometric drawing for the textual Canvas. Include
`Rasterization.h` directly or the `Graphics.h` umbrella header. No allocation,
initialization, terminal I/O, implicit update or `-lm` dependency.

## API

Every operation returns `OPSTATUS` and receives `canvas` first, followed by
geometry, then `uint32_t codepoint` and `TEXT_GRID_ATTRIBUTE_TYPE(Cell) style`.
Public names use `RASTER_FUNC(Name)` (default: `Raster_Name`).

| Operation | Geometry arguments |
| --- | --- |
| `Point` | `TDUAL_TYPE(int32) at` |
| `Line` | `TDUAL_TYPE(int32) from, to` |
| `Rectangle`, `FillRectangle` | `TQUAD_TYPE(uint16) bounds` |
| `Circle`, `FillCircle` | `TDUAL_TYPE(int32) center, uint32_t radius` |
| `Ellipse`, `FillEllipse` | `TDUAL_TYPE(int32) center, TDUAL_TYPE(uint32) radii` |
| `Triangle`, `FillTriangle` | `TDUAL_TYPE(int32) a, b, c` |
| `Polyline`, `Polygon`, `FillPolygon` | `const TDUAL_TYPE(int32) *points, size_t count` |

## Example

```c
#include "Codespace/Cosmeron/Modules/Graphics/Rasterization/Rasterization.h"

int main(void) {
  TERMINAL_TYPE(TCanvas) canvas = {0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  OPSTATUS status = TERMINAL_FUNC(Canvas_Create)(
      &canvas, (TDUAL_TYPE(uint16)){.col=60, .row=20});
  if (status != STATUS_CONST(SUCCESS)) return 1;

  style.foreground = 0x00CCFF;
  status = RASTER_FUNC(FillEllipse)(
      &canvas, (TDUAL_TYPE(int32)){.x=30, .y=10},
      (TDUAL_TYPE(uint32)){.x=16, .y=6}, '#', style);
  if (status == STATUS_CONST(SUCCESS))
    status = TERMINAL_FUNC(Canvas_Update)(&canvas);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return status == STATUS_CONST(SUCCESS) ? 0 : 1;
}
```

## Contracts

- Coordinates are zero-based **cells**, relative to the canvas/view. Circles are
  circular in cell coordinates; terminal cells usually are taller than wide.
  Use ellipse radii to compensate for the display's cell aspect ratio.
- Rectangles use inclusive `left/top/right/bottom`. Inverted bounds return
  `INVALID_ARGUMENT`; otherwise out-of-view cells are clipped. For shapes with
  negative rectangle coordinates, use a four-vertex polygon.
- Clipping does not create new outline edges. Fully invisible shapes succeed
  without changing the canvas. Canvas cursor and default style are unchanged.
- Brushes must occupy exactly one cell (`#`, space, `U+2588`, etc.). Wide and
  combining characters return `NOT_SUPPORTED` before drawing. Invalid Unicode
  is rejected by Text/Unicode validation.
- Existing wide glyphs wholly inside the view are erased safely by `Canvas_Set`.
  Cells belonging to a wide glyph crossing a view boundary are skipped, so
  neither the outside half nor its inside continuation is corrupted.
- A zero-radius circle becomes a point. An ellipse with one zero radius becomes
  a line. Repeated/collinear polygon vertices are accepted; one vertex becomes
  a point, two vertices a segment. A null vertex array or zero count is invalid.
- Polylines are open; polygons close the last-to-first edge. Polygon filling
  uses even/odd crossings (including concave/self-intersecting polygons) plus
  the rasterized boundary. The vertex array is borrowed for the call.
- Lines use an integer nearest-cell/Bresenham lattice with a canonical major
  direction, ensuring reversal invariance. Only visible major-axis positions
  are visited, even for full-range signed 32-bit endpoints.
- Ellipses fill integer cell centers satisfying the normalized ellipse equation;
  the outline is the four-neighbor boundary of that filled set. Extents use
  binary search and `long double`, with no square root or libm calls. Floating
  rounding can affect boundary ties at extreme radii.
- Polygon filling costs O(visible bounding-box area × vertex count), without
  heap storage. Ellipse work is bounded by visible rows/columns and logarithmic
  radius searches. All geometry arithmetic widens before subtraction.

## Tests

`make -C Codespace/Tests/Terminal` includes raster tests for all public functions,
line octants/reversal/endpoints, inclusive clipping, integer fill masks, concavity,
degeneracy, extreme coordinates/radii, Unicode validation, styles and wide-glyph
view boundaries.

## Arcs and Bézier curves

- `RASTER_FUNC(Arc)(canvas, center, radii, from, to, clockwise, codepoint, style)`
  draws an ellipse boundary between two nonzero **direction vectors** measured
  from the center. For example, from `{1,0}` to `{0,1}` clockwise draws the
  bottom-right quarter (screen y points down). Both radii must be positive.
  Equal directions mean a full ellipse. Angles use polar order without trig/libm.
- `RASTER_FUNC(BezierQuadratic)(canvas, from, control, to, codepoint, style)`
  draws a quadratic curve. All points reuse `TDUAL_TYPE(int32)`.
- `RASTER_FUNC(BezierCubic)(canvas, from, control1, control2, to, codepoint, style)`
  draws a cubic curve, including loops and coincident endpoints.

Bézier drawing uses de Casteljau subdivision, convex-hull viewport culling and
quarter-cell control deviation before line rasterization. Recursion is capped
at 20 subdivisions: at extreme coordinates the cap takes precedence over the
flatness target. No heap allocation. Rounding and clipping preserve the existing
width-one character and Canvas-view contracts.
