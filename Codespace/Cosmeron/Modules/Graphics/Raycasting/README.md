# Raycasting — ray traversal and text projection

**Project specification only.**

## Objective

Reusable raycasting logic and a small adapter for textual visualization. Ray calculations belong to the numerical/geometry layer; visual output is drawn into a supplied Canvas via Rasterization. The core must not depend on the terminal backend.

## Required reuse

- `Struct`: `TDUAL_TYPE(float)`, `TTRIPLE_TYPE(float)`, `TDUAL_TYPE(int32)`; use specialized mathematical types only when semantics genuinely require more than a tuple.
- `Math`: arithmetic/clamping/projection helpers and numeric predicates.
- `Canvas` and `Rasterization`: render columns/segments/cells, never a parallel grid.
- `Terminal` only optionally for interactive demos (input, presentation), not for computational kernels.

## Suggested subpackages

| Area | Responsibility |
|---|---|
| Ray | origin, direction and distance parameters |
| Intersect | line/wall/grid intersection and hit filtering |
| GridDDA | efficient 2D tile map traversal |
| Projection | camera plane, depth and column projection |
| Render | project result to Canvas using raster primitives |

## Illustrative signatures

```c
OPSTATUS RAYCAST_FUNC(GridCast)(
    const RAYCAST_TYPE(TGridMap) *map,
    const RAYCAST_TYPE(TRay2D) *ray,
    RAYCAST_TYPE(THit) *outHit);
OPSTATUS RAYCAST_FUNC(RenderColumns)(
    TERMINAL_TYPE(TCanvas) *canvas,
    const RAYCAST_TYPE(TScene) *scene,
    const RAYCAST_TYPE(TCamera) *camera);
```

Map/ray/hit/camera types are semantic raycasting constructs, not generic replacements for existing Struct tuples. Exact layouts remain to be designed; these are conceptual signatures.

## Contracts and tests

- Deterministic ray hit ordering; configurable distance limits; out result only on success.
- Consistent handling for zero-direction components, map edges, no hit, and overflow.
- Rendering has no terminal side effects until `Canvas_Update`.
- Test DDA traversal, fisheye correction, offscreen clipping, perspective stability and terminal width changes.
