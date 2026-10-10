# Pseudo3DRoad — perspective road with curves and hills

Header-only C11, no heap allocation, initialization or mandatory `-lm`.
`Pseudo3DRoad.h` contains the computational API without Terminal dependencies;
`Render.h` adds the Canvas renderer. Both are available through `Graphics.h`.

## Track, camera and projection

| Type | Fields and meaning |
| --- | --- |
| `ROAD_TYPE(TNode)` | `TTriple_float position`: x lateral center, y elevation (up), z forward distance; positive `halfWidth` in world units. |
| `ROAD_TYPE(TTrack)` | Borrowed `const TNode *nodes`, `size_t count`. At least two nodes, finite values, strictly increasing z. Adjacent nodes define segments. |
| `ROAD_TYPE(TCamera)` | `TTriple_float position`; positive `focalLength` in viewport heights, positive `nearClip`, `drawDistance > nearClip`, `horizon` in [0,1]. |
| `ROAD_TYPE(TProjection)` | `TDual_float center` in screen cells, projected `halfWidth`, camera-relative `depth`, perspective `scale`. |
| `ROAD_TYPE(TStyle)` | One width-one `codepoint`; Text/Grid `sky`, `grass[2]`, `road[2]`, `edge[2]`, `lane`; `lanes`, `edgeFraction`, `laneFraction`, `stripeSegments`. |

Use x changes for bends, y changes for hills, and halfWidth changes for widening.
The track stores absolute centers, so render output does not depend on hidden
accumulated curvature. Move `camera.position.z` to advance and x to steer. Adjust
y to follow road elevation. Viewport resizing is handled on every render.
All pointers are borrowed for the call; no `Init`/`Free` pair is needed.

## API

```c
OPSTATUS ROAD_FUNC(TrackValidate)(const ROAD_TYPE(TTrack) *track);
OPSTATUS ROAD_FUNC(Project)(
    const ROAD_TYPE(TCamera) *camera, TDUAL_TYPE(uint16) viewport,
    const ROAD_TYPE(TNode) *node, ROAD_TYPE(TProjection) *outProjection);
ROAD_TYPE(TStyle) ROAD_FUNC(StyleDefault)(void);
OPSTATUS ROAD_FUNC(Render)(
    TERMINAL_TYPE(TCanvas) *canvas, const ROAD_TYPE(TTrack) *track,
    const ROAD_TYPE(TCamera) *camera, const ROAD_TYPE(TStyle) *style);
```

Default names: `Road_TrackValidate`, `Road_Project`, `Road_StyleDefault`, `Road_Render`.

## Projection and drawing rules

- `depth = node.z - camera.z`; `scale = focalLength * viewportHeight / depth`.
  Screen x is viewportWidth/2 + (node.x-camera.x)×scale; screen y is
  horizon×viewportHeight − (node.y-camera.y)×scale. Projected halfWidth is
  world halfWidth×scale. Units are cells; no physical cell-aspect assumption.
- `Project` returns `OUT_OF_RANGE` for points outside the near/far range or
  projections not representable as floats. Invalid/nonfinite arguments return
  `INVALID_ARGUMENT`. The output is modified only on success.
- `Render` validates the whole track before drawing, clips segments against
  near and far depth, and interpolates clipped endpoints before projection.
  Projected spans are clipped before converting to integer cell coordinates.
- Segments are traversed near to far. A monotonically decreasing visible row
  boundary prevents distant descending road from showing through hill crests.
  This is the traditional single-road scanline model, not a general 3D depth
  buffer; bridges, tunnels, overhangs and crossing roads are unsupported.
- Curves/elevation/width are linear between nodes. Use more nodes to approximate
  smooth curves. Screen edges are interpolated across each projected trapezoid.
- Default style uses colored backgrounds and spaces, alternating road/grass/
  curb palettes, three lanes and intermittent lane markers. Call `StyleDefault`
  then edit ordinary struct fields. `stripeSegments` must be nonzero; stripes
  follow segment index (use equal z spacing for uniform world stripe lengths).
- `edgeFraction` is curb width per side divided by the full road width, in [0,1].
  `laneFraction` is marker width divided by the full road width, in [0,1/lanes].
  `lanes >= 1`; one lane disables internal markers. A zero edge/marker fraction
  disables that component. Lane marks are drawn on alternate stripe groups.
- Sky and grass fill uncovered rows. Views, including nested views, use local
  coordinates. Rasterization preserves wide glyphs crossing view boundaries.
  No terminal I/O occurs until the caller invokes `Canvas_Update`.
- Rendering costs O(node count + visible rows × (canvas width + lane count)),
  without temporary framebuffers. Extremely large float world coordinates can
  lose geometric precision; periodically rebase long-running tracks if needed.

## Example and validation

`Codespace/Examples/Graphics/Pseudo3DRoad.c` draws a curved, elevated road:

```sh
cc -std=c11 -D_POSIX_C_SOURCE=200809L Codespace/Examples/Graphics/Pseudo3DRoad.c -o road
./road
make -C Codespace/Tests/Terminal
```

Run examples in an interactive terminal. Tests cover exact perspective values,
near clipping, track validation, curves, hills/crest occlusion, movement,
resizing to 1×1, nested views, invalid styles/numbers and unchanged error outputs.
Track looping, traffic simulation and gameplay are left to the caller.

## Objects, terrain depth and track sampling

```c
OPSTATUS ROAD_FUNC(TrackSample)(
    const ROAD_TYPE(TTrack) *track, float z, ROAD_TYPE(TNode) *outNode);
OPSTATUS ROAD_FUNC(RenderDepth)(
    TERMINAL_TYPE(TCanvas) *canvas, const ROAD_TYPE(TTrack) *track,
    const ROAD_TYPE(TCamera) *camera, const ROAD_TYPE(TStyle) *style,
    float *outRowDepth, size_t depthCount);
OPSTATUS ROAD_FUNC(RenderObjects)(
    TERMINAL_TYPE(TCanvas) *canvas, const ROAD_TYPE(TCamera) *camera,
    const ROAD_TYPE(TObject) *objects, size_t count,
    const float *rowDepth, size_t depthCount);
```

`TrackSample` interpolates center/elevation/halfWidth at world z. Place a sign or
car using this position plus an x offset. It validates the track, returns
`OUT_OF_RANGE` outside the track, and leaves the output untouched on failure.

`RenderDepth` is the normal road render plus a caller-owned float buffer with
at least canvas height elements. Each row stores visible terrain depth; untouched
rows use `FLT_MAX`. Depth is interpolated reciprocally, not linearly in screen
space. Recompute after any camera, track or viewport change. `Render` continues
to work without a depth buffer.

`TObject` contains `SPRITE_TYPE(TLayer) image`, `TTriple_float position` (world
bottom center) and positive `TDual_float size` (world width/height). Image crop,
flips and transparency apply. The image's 2D position/size are superseded by
projection; initialize it with `Sprite_Layer` before changing crop/key fields.

`RenderObjects` draws billboarded images far to near, rejecting fragments behind
visible terrain per row. A tree can remain visible above a crest while its trunk
is hidden. This shares the renderer's single-road/full-width terrain model; it
is not general 3D geometry. Equal-depth objects follow input order; sorting is
O(objectCount²) without allocation or caller-array mutation. Objects outside the
near/far range are skipped. Arrays/images are borrowed and validated before draw.
The road example now includes a transparent roadside tree.
