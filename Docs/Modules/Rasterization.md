# Rasterization

Cell-based rasterization onto a borrowed terminal Canvas. It draws geometry without writing to the terminal, allocating a framebuffer, or requiring -lm.

```c
#include "Cosmeron/Modules/Graphics/Rasterization/Rasterization.h"
```

---

# Overview

Geometry is in zero-based canvas cells. Rectangle bounds are inclusive; drawing is clipped and off-screen shapes do not create extra outline edges.

### Macro form

```c
/* See the package's *_FUNC namespace macro for calling public operations. */
```

### Direct form

```c
/* With the default prefix, RASTER_FUNC(Name) resolves to Raster_Name. */
```

# Types and ownership

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal. The package reuses `Struct` tuples, `Text/Grid` styles and `Terminal/Canvas` instead of duplicating primitive types. Check the header for the layout of each type.

# API reference

## Function summary

| Function | Return type | Role |
| --- | --- | --- |
| `RASTER_FUNC(Point)` | `OPSTATUS` | Writes one cell at a signed coordinate. |
| `RASTER_FUNC(Line)` | `OPSTATUS` | Rasterizes a clipped line between two signed endpoints. |
| `RASTER_FUNC(Rectangle)` | `OPSTATUS` | Draws the outline of an inclusive rectangle. |
| `RASTER_FUNC(FillRectangle)` | `OPSTATUS` | Fills every covered cell of an inclusive rectangle. |
| `RASTER_FUNC(Circle)` | `OPSTATUS` | Draws a circle outline in cell geometry. |
| `RASTER_FUNC(FillCircle)` | `OPSTATUS` | Fills a circle interior. |
| `RASTER_FUNC(Ellipse)` | `OPSTATUS` | Draws an ellipse outline with independent horizontal and vertical radii. |
| `RASTER_FUNC(FillEllipse)` | `OPSTATUS` | Fills an ellipse interior. |
| `RASTER_FUNC(Triangle)` | `OPSTATUS` | Draws a three-edge triangle. |
| `RASTER_FUNC(FillTriangle)` | `OPSTATUS` | Rasterizes a filled triangle. |
| `RASTER_FUNC(Polyline)` | `OPSTATUS` | Draws adjacent edges through all supplied vertices without closing the shape. |
| `RASTER_FUNC(Polygon)` | `OPSTATUS` | Draws a closed polygon outline. |
| `RASTER_FUNC(FillPolygon)` | `OPSTATUS` | Fills the polygon interior. |
| `RASTER_FUNC(Arc)` | `OPSTATUS` | Draws a selected elliptical arc between endpoints, choosing clockwise sweep as directed. |
| `RASTER_FUNC(BezierQuadratic)` | `OPSTATUS` | Draws a quadratic Bézier curve using one control point. |
| `RASTER_FUNC(BezierCubic)` | `OPSTATUS` | Draws a cubic Bézier curve using two control points. |

# Point

Writes one cell at a signed coordinate.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(Point)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) at, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_Point(Terminal_TCanvas *canvas, TDual_int32 at, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `at`: signed position in canvas cells.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(Point)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, '#', style);
```

# Line

Rasterizes a clipped line between two signed endpoints.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(Line)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) to, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_Line(Terminal_TCanvas *canvas, TDual_int32 from, TDual_int32 to, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `from`: first endpoint (cell pair).
- `to`: last endpoint (cell pair).
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(Line)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, '#', style);
```

# Rectangle

Draws the outline of an inclusive rectangle.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(Rectangle)(TERMINAL_TYPE(TCanvas) *canvas, TQUAD_TYPE(uint16) bounds, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_Rectangle(Terminal_TCanvas *canvas, TQuad_uint16 bounds, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `bounds`: inclusive rectangle with left/top/right/bottom fields.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(Rectangle)(&canvas, (TQUAD_TYPE(uint16)){.left = 1, .top = 1, .right = 8, .bottom = 5}, '#', style);
```

# FillRectangle

Fills every covered cell of an inclusive rectangle.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(FillRectangle)(TERMINAL_TYPE(TCanvas) *canvas, TQUAD_TYPE(uint16) bounds, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_FillRectangle(Terminal_TCanvas *canvas, TQuad_uint16 bounds, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `bounds`: inclusive rectangle with left/top/right/bottom fields.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(FillRectangle)(&canvas, (TQUAD_TYPE(uint16)){.left = 1, .top = 1, .right = 8, .bottom = 5}, '#', style);
```

# Circle

Draws a circle outline in cell geometry.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(Circle)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) center, uint32_t radius, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_Circle(Terminal_TCanvas *canvas, TDual_int32 center, uint32_t radius, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `center`: center of shape.
- `radius`: circle radius in cells.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(Circle)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, 5, '#', style);
```

# FillCircle

Fills a circle interior.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(FillCircle)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) center, uint32_t radius, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_FillCircle(Terminal_TCanvas *canvas, TDual_int32 center, uint32_t radius, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `center`: center of shape.
- `radius`: circle radius in cells.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(FillCircle)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, 5, '#', style);
```

# Ellipse

Draws an ellipse outline with independent horizontal and vertical radii.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(Ellipse)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) center, TDUAL_TYPE(uint32) radii, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_Ellipse(Terminal_TCanvas *canvas, TDual_int32 center, TDual_uint32 radii, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `center`: center of shape.
- `radii`: two ellipse radii in cells.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(Ellipse)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(uint32)){.x = 5, .y = 3}, '#', style);
```

# FillEllipse

Fills an ellipse interior.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(FillEllipse)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) center, TDUAL_TYPE(uint32) radii, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_FillEllipse(Terminal_TCanvas *canvas, TDual_int32 center, TDual_uint32 radii, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `center`: center of shape.
- `radii`: two ellipse radii in cells.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(FillEllipse)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(uint32)){.x = 5, .y = 3}, '#', style);
```

# Triangle

Draws a three-edge triangle.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(Triangle)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) a, TDUAL_TYPE(int32) b, TDUAL_TYPE(int32) c, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_Triangle(Terminal_TCanvas *canvas, TDual_int32 a, TDual_int32 b, TDual_int32 c, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `a`: first triangle vertex.
- `b`: second triangle vertex.
- `c`: third triangle vertex.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(Triangle)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, '#', style);
```

# FillTriangle

Rasterizes a filled triangle.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(FillTriangle)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) a, TDUAL_TYPE(int32) b, TDUAL_TYPE(int32) c, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_FillTriangle(Terminal_TCanvas *canvas, TDual_int32 a, TDual_int32 b, TDual_int32 c, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `a`: first triangle vertex.
- `b`: second triangle vertex.
- `c`: third triangle vertex.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(FillTriangle)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, '#', style);
```

# Polyline

Draws adjacent edges through all supplied vertices without closing the shape.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(Polyline)(TERMINAL_TYPE(TCanvas) *canvas, const TDUAL_TYPE(int32) *points, size_t count, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_Polyline(Terminal_TCanvas *canvas, const TDual_int32 *points, size_t count, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `points`: pointer to caller-owned array of vertices.
- `count`: number of points, sprites or objects.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(Polyline)(&canvas, points, 3, '#', style);
```

# Polygon

Draws a closed polygon outline.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(Polygon)(TERMINAL_TYPE(TCanvas) *canvas, const TDUAL_TYPE(int32) *points, size_t count, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_Polygon(Terminal_TCanvas *canvas, const TDual_int32 *points, size_t count, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `points`: pointer to caller-owned array of vertices.
- `count`: number of points, sprites or objects.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(Polygon)(&canvas, points, 3, '#', style);
```

# FillPolygon

Fills the polygon interior.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(FillPolygon)(TERMINAL_TYPE(TCanvas) *canvas, const TDUAL_TYPE(int32) *points, size_t count, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_FillPolygon(Terminal_TCanvas *canvas, const TDual_int32 *points, size_t count, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `points`: pointer to caller-owned array of vertices.
- `count`: number of points, sprites or objects.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal. The input point array must remain valid during drawing.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(FillPolygon)(&canvas, points, 3, '#', style);
```

# Arc

Draws a selected elliptical arc between endpoints, choosing clockwise sweep as directed.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(Arc)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) center, TDUAL_TYPE(uint32) radii, TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) to, bool clockwise, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_Arc(Terminal_TCanvas *canvas, TDual_int32 center, TDual_uint32 radii, TDual_int32 from, TDual_int32 to, bool clockwise, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `center`: center of shape.
- `radii`: two ellipse radii in cells.
- `from`: first endpoint (cell pair).
- `to`: last endpoint (cell pair).
- `clockwise`: choice of arc sweep direction.
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(Arc)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(uint32)){.x = 5, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, true, '#', style);
```

# BezierQuadratic

Draws a quadratic Bézier curve using one control point.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(BezierQuadratic)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) control, TDUAL_TYPE(int32) to, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_BezierQuadratic(Terminal_TCanvas *canvas, TDual_int32 from, TDual_int32 control, TDual_int32 to, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `from`: first endpoint (cell pair).
- `control`: quadratic control point.
- `to`: last endpoint (cell pair).
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(BezierQuadratic)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, '#', style);
```

# BezierCubic

Draws a cubic Bézier curve using two control points.

### Syntax

#### Macro form

```c
OPSTATUS RASTER_FUNC(BezierCubic)(TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) control1, TDUAL_TYPE(int32) control2, TDUAL_TYPE(int32) to, uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

#### Direct form

```c
OPSTATUS Raster_BezierCubic(Terminal_TCanvas *canvas, TDual_int32 from, TDual_int32 control1, TDual_int32 control2, TDual_int32 to, uint32_t codepoint, Text_Grid_Attribute_Cell style);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `from`: first endpoint (cell pair).
- `control1`: first cubic control point.
- `control2`: second cubic control point.
- `to`: last endpoint (cell pair).
- `codepoint`: width-one Unicode character drawn into target cells.
- `style`: cell rendering attributes from Text/Grid.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

Coordinates are clipped to the Canvas bounds. Use a width-one codepoint; shape operations do not implicitly present/update the terminal.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RASTER_FUNC(BezierCubic)(&canvas, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, (TDUAL_TYPE(int32)){.x = 2, .y = 3}, '#', style);
```

# Requirements and related APIs

- C11, header-only, no mandatory linking of `-lm` or other external libraries for this package.
- The package draws to or computes values for Canvas, never implicitly performs terminal presentation.
- [Canvas](Canvas.md), [Terminal](Terminal.md), [Struct](Struct.md), [Text](Text.md), [Bit](Bit.md).
- Detailed design notes: [Rasterization README](../../Codespace/Cosmeron/Modules/Graphics/Rasterization/README.md).

# C11 smoke test

The Canvas owns its backing Text/Grid storage. Rasterization writes cells only; presenting them on a terminal is a separate step.

<!-- cosmeron-c11-test -->
```c
#include "Cosmeron/Modules/Graphics/Rasterization/Rasterization.h"
#include <assert.h>

int main(void) {
    TERMINAL_TYPE(TCanvas) canvas = {0};
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style =
        TEXT_GRID_ATTRIBUTE_FUNC(Default)();
    uint32_t codepoint = 0;
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) readStyle;
    assert(TERMINAL_FUNC(Canvas_Create)(
               &canvas, (TDUAL_TYPE(uint16)){.x=12, .y=8}) ==
           STATUS_CONST(SUCCESS));
    assert(RASTER_FUNC(Point)(
               &canvas, (TDUAL_TYPE(int32)){.x=2, .y=3},
               '#', style) == STATUS_CONST(SUCCESS));
    assert(TERMINAL_FUNC(Canvas_Get)(
               &canvas, (TDUAL_TYPE(uint16)){.x=2, .y=3},
               &codepoint, &readStyle) == STATUS_CONST(SUCCESS));
    assert(codepoint == '#');
    TERMINAL_FUNC(Canvas_Free)(&canvas);
    return 0;
}
```
