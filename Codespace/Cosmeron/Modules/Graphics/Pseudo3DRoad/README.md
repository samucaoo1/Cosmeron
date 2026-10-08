# Pseudo3DRoad — perspective road algorithm

**Project specification only.**

## Objective

Implement a pseudo-3D road/terrain algorithm inspired by traditional scanline road renderers. The render output is a supplied Canvas; `Terminal` is only the final presentation target, not a required calculation dependency.

## Reused foundations

- Geometry: `TDUAL_TYPE(float)`, `TTRIPLE_TYPE(float)` (and existing integer variants), from `Struct`.
- Math: interpolation/clamp/trigonometric helpers or appropriately contained replacements consistent with the **no `-lm` mandatory link** policy.
- Rasterization: horizontal spans/scanline fill and cell output.
- Canvas: clipping, UTF-8 glyph selection/attributes and explicit Update.
- Chronometry: optional frame timing/delta. No duplicate timers.

## Proposed subpackages

| Component | Work |
|---|---|
| Track | road curvature, elevation, width and segment data |
| Camera | horizon, lateral offset, depth and field-of-view |
| Projection | segment-to-screen/scanline transform |
| Render | grass/road/edge/shading by row or cell |
| Demo | scroll/steer, resize and frames through Canvas_Update |

## Conceptual signatures

```c
OPSTATUS ROAD_FUNC(Project)(
    const ROAD_TYPE(TTrack) *track,
    const ROAD_TYPE(TCamera) *camera,
    ROAD_TYPE(TProjection) *outProjection);
OPSTATUS ROAD_FUNC(Render)(
    TERMINAL_TYPE(TCanvas) *canvas,
    const ROAD_TYPE(TTrack) *track,
    const ROAD_TYPE(TCamera) *camera);
```

These are tentative domain types/functions; do not commit implementations before their layout/ownership semantics are reviewed.

## Contracts

- No raw terminal output from rendering.
- Row/segment traversal must remain bounded; handle horizon, clipping and zero-sized canvases.
- Prefer reused Struct aggregates for positions/vectors; add only semantic Track/Camera/Projection structures.
- Defer to Canvas's glyph-width and attribute fallback policies.

## Tests

Straight road; high curvature; hills; camera movement; clipping; aspect ratios; terminal resize; rendering into a nested borrowed Canvas view.
