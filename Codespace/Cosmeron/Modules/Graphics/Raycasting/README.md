# Raycasting — grid DDA and Canvas projection

Header-only C11, no allocation and no mandatory `-lm`. Include `Raycasting.h`
for calculations alone; `Render.h` adds the Canvas adapter. `Graphics.h`
includes both. No initialization or terminal output is performed by this package.

## Types and ownership

| Type | Fields and meaning |
| --- | --- |
| `RAYCAST_TYPE(TGridMap)` | Borrowed `const uint16_t *cells`; `TDual_uint16 size`; `size_t stride` in cells and `cellCount` capacity. Zero is empty; nonzero is a material ID. |
| `RAYCAST_TYPE(TRay2D)` | `TDual_float origin, direction`; `float maxDistance`. The ray is `origin + t * direction`, with `0 <= t <= maxDistance`. |
| `RAYCAST_TYPE(THit)` | Integer `cell`, float `position` and outward `normal`, `distance` (t), fractional `wallCoordinate`, `material`, `side`. |
| `RAYCAST_TYPE(TCamera)` | Float pairs `position`, unit `direction`, perpendicular `plane`; `maxDistance`, positive `wallHeight`, `horizon` in [0,1]. Plane length is tan(horizontal FOV/2). |
| `RAYCAST_TYPE(TScene)` | `map`; wall/ceiling/floor codepoints; `wallStyle`, `sideStyle`, `ceilingStyle`, `floorStyle` from Text/Grid. |

All pointers are borrowed for the call. Tuples reuse `Struct`. No duplicated
framebuffer, vector, color, cell or timer types.

## API

```c
OPSTATUS RAYCAST_FUNC(GridCast)(
    const RAYCAST_TYPE(TGridMap) *map,
    const RAYCAST_TYPE(TRay2D) *ray, RAYCAST_TYPE(THit) *outHit);
OPSTATUS RAYCAST_FUNC(CameraRay)(
    const RAYCAST_TYPE(TCamera) *camera, uint16_t column, uint16_t columns,
    RAYCAST_TYPE(TRay2D) *outRay);
OPSTATUS RAYCAST_FUNC(RenderColumns)(
    TERMINAL_TYPE(TCanvas) *canvas, const RAYCAST_TYPE(TScene) *scene,
    const RAYCAST_TYPE(TCamera) *camera);
```

Default names: `Raycast_GridCast`, `Raycast_CameraRay`, `Raycast_RenderColumns`.

- `GridCast` clips against the map rectangle, so rays may begin outside it.
  Traversal is bounded by width + height + 1; axis-aligned rays are supported.
- `SUCCESS` returns the first occupied cell; `NOT_FOUND` means no wall within
  the limit. Invalid inputs return `INVALID_ARGUMENT`. Outputs remain untouched
  on failure. Stride/capacity arithmetic is checked before accessing cells.
- Distances are ray parameters, not necessarily Euclidean lengths. Unit
  directions produce world-unit lengths. Camera rays are intentionally **not**
  normalized: their t is perpendicular depth, avoiding fisheye distortion.
- DDA uses X first on exact corner ties, then Y if the X neighbor is empty.
  On a grid boundary, negative direction starts in the preceding cell.
  Starting inside a solid cell returns t=0, `SIDE_NONE`, zero normal.
- `CameraRay` samples column centers. Forward vectors must have squared length
  within 0.0001 of one and planes must be perpendicular within that tolerance.
  Rotation can be supplied directly as direction and perpendicular plane;
  no trigonometry dependency is imposed by the API.
- The renderer paints ceiling/floor and projects solid, constant-height walls.
  `wallHeight * canvasHeight / depth` determines wall height in cells.
  Y-side hits use `sideStyle`; all material IDs currently share these styles.
- Rendering respects Canvas views, uses Rasterization's width-one brush policy,
  and changes neither canvas cursor nor its default style. Present with
  `TERMINAL_FUNC(Canvas_Update)` explicitly. No collision handling is implied.
- Floating-point inputs are rejected when nonfinite. Geometry at extreme float
  scales can lose spatial precision; keep local map coordinates near the map.

## Example and tests

See `Codespace/Examples/Graphics/Raycasting.c` for an 8×8 map and 80×24 view.
From the repository root:

```sh
cc -std=c11 -D_POSIX_C_SOURCE=200809L Codespace/Examples/Graphics/Raycasting.c -o raycasting
./raycasting
make -C Codespace/Tests/Terminal
```

Run the example in an interactive terminal. Tests cover axes, outside origins,
boundaries, corner ties, limits, unchanged error outputs, invalid numbers,
400 rays checked against independent slab intersections, fisheye correction,
and Canvas view isolation. `ProjectionCore.c` enforces no Terminal dependency
in the computational headers.

Textures, sprites, arbitrary wall segments and collision/movement systems are
not part of this renderer.
