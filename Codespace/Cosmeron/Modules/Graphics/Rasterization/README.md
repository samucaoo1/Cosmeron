# Rasterization — 2D drawing over Canvas

**Project specification only.**

## Objective

Provide reusable geometrical drawing primitives for a **textual** Canvas. Rasterization does not own stdout, terminal handles or a parallel framebuffer. Cell-style and character output are delegated to Canvas.

## Required reuse

- Geometry: `TDUAL_TYPE(int32)` for signed positions, `TDUAL_TYPE(float)` for geometric points, `TQUAD_TYPE(uint16)` for **inclusive** bounded cell rectangles.
- Cell output: `TERMINAL_FUNC(Canvas_Set)`, `Canvas_FillRegion`, `Canvas_Blit`; reuse existing `TEXT_GRID_ATTRIBUTE_TYPE(Cell)`.
- Math algorithms reuse `Math` when available; do not reinvent general-purpose vector/matrix/geometry primitives inside this package.

## Planned operations

```c
OPSTATUS RASTER_FUNC(Point)(
    TERMINAL_TYPE(TCanvas) *canvas, TDUAL_TYPE(int32) at,
    uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
OPSTATUS RASTER_FUNC(Line)(
    TERMINAL_TYPE(TCanvas) *canvas,
    TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) to,
    uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
OPSTATUS RASTER_FUNC(Rectangle)(
    TERMINAL_TYPE(TCanvas) *canvas,
    TQUAD_TYPE(uint16) bounds, uint32_t codepoint,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
OPSTATUS RASTER_FUNC(FillRectangle)(
    TERMINAL_TYPE(TCanvas) *canvas,
    TQUAD_TYPE(uint16) bounds, uint32_t codepoint,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
OPSTATUS RASTER_FUNC(Circle)(
    TERMINAL_TYPE(TCanvas) *canvas,
    TDUAL_TYPE(int32) center, uint32_t radius,
    uint32_t codepoint, TEXT_GRID_ATTRIBUTE_TYPE(Cell) style);
```

Future: ellipses, triangles, scanline fills, polygons, gradients, masks and paths; named font/character ramps for text-space coverage.

## Contracts

- **Draw-only**: no immediate terminal I/O and no implicit Update.
- Bounds and clipping are evaluated against the canvas/view; off-screen drawing is clipped safely.
- Wide glyphs must never leave half-character corruption.
- Signed intermediate coordinates are required even though final Text/Grid indexing is unsigned.
- No mandatory `-lm` link requirement; use Math's existing portability policy.

## Tests

Octants and endpoints for line drawing, circles, negative coordinates, clipped views, degenerate shapes, overflow, character width and style preservation.
