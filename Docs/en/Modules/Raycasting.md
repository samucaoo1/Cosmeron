# Raycasting

2D grid DDA ray traversal, camera rays, depth-buffered wall columns and billboards rendered to Canvas.

```c
#include "Cosmeron/Modules/Graphics/Raycasting/Render.h"
```

---

# Overview

Grid data and textures are borrowed. Ray distance is a ray parameter, not necessarily Euclidean length unless direction is unit length; material 0 means empty.

### Macro form

```c
/* See the package's *_FUNC namespace macro for calling public operations. */
```

### Direct form

```c
/* With the default prefix, RAYCAST_FUNC(Name) resolves to Raycast_Name. */
```

# Types and ownership

All inputs are borrowed and all outputs belong to the caller. No hidden frame state or memory allocation is performed by the core DDA routine. The package reuses `Struct` tuples, `Text/Grid` styles and `Terminal/Canvas` instead of duplicating primitive types. Check the header for the layout of each type.

# API reference

## Function summary

| Function | Return type | Role |
| --- | --- | --- |
| `RAYCAST_FUNC(GridCast)` | `OPSTATUS` | Traverses grid cells to find the first occupied material hit. |
| `RAYCAST_FUNC(CameraRay)` | `OPSTATUS` | Constructs a screen-column ray from the camera direction and projection plane. |
| `RAYCAST_FUNC(RenderColumnsDepth)` | `OPSTATUS` | Renders wall columns and stores depth per column for further sprite occlusion. |
| `RAYCAST_FUNC(RenderSprites)` | `OPSTATUS` | Projects sprites and clips them against column wall-depth values. |
| `RAYCAST_FUNC(RenderColumns)` | `OPSTATUS` | Renders wall columns, ceiling and floor with default intermediate-depth management. |

# GridCast

Traverses grid cells to find the first occupied material hit.

### Syntax

#### Macro form

```c
OPSTATUS RAYCAST_FUNC(GridCast)(const RAYCAST_TYPE(TGridMap) *map, const RAYCAST_TYPE(TRay2D) *ray, RAYCAST_TYPE(THit) *outHit);
```

#### Direct form

```c
OPSTATUS Raycast_GridCast(const Raycast_TGridMap *map, const Raycast_TRay2D *ray, Raycast_THit *outHit);
```

### Parameters

- `map`: borrowed row-major grid with explicit dimensions/stride/cellCount.
- `ray`: ray origin, direction and maximum traversal parameter.
- `outHit`: receives first intersected occupied cell on success.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

All inputs are borrowed and all outputs belong to the caller. No hidden frame state or memory allocation is performed by the core DDA routine. Returns `NOT_FOUND` if no wall is hit within the configured range, leaving outHit unchanged.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
RAYCAST_TYPE(THit) hit = {0};
OPSTATUS status = RAYCAST_FUNC(GridCast)(&map, &ray, &hit);
```

# CameraRay

Constructs a screen-column ray from the camera direction and projection plane.

### Syntax

#### Macro form

```c
OPSTATUS RAYCAST_FUNC(CameraRay)(const RAYCAST_TYPE(TCamera) *camera, uint16_t column, uint16_t columns, RAYCAST_TYPE(TRay2D) *outRay);
```

#### Direct form

```c
OPSTATUS Raycast_CameraRay(const Raycast_TCamera *camera, uint16_t column, uint16_t columns, Raycast_TRay2D *outRay);
```

### Parameters

- `camera`: camera parameters controlling view and clipping.
- `column`: zero-based viewport column.
- `columns`: number of viewport columns.
- `outRay`: receives one camera ray.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

All inputs are borrowed and all outputs belong to the caller. No hidden frame state or memory allocation is performed by the core DDA routine.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
RAYCAST_TYPE(TRay2D) ray = {0};
OPSTATUS status = RAYCAST_FUNC(CameraRay)(&camera, 0, 80, &ray);
```

# RenderColumnsDepth

Renders wall columns and stores depth per column for further sprite occlusion.

### Syntax

#### Macro form

```c
OPSTATUS RAYCAST_FUNC(RenderColumnsDepth)(TERMINAL_TYPE(TCanvas) *canvas, const RAYCAST_TYPE(TScene) *scene, const RAYCAST_TYPE(TCamera) *camera, float *outDepth, size_t depthCount);
```

#### Direct form

```c
OPSTATUS Raycast_RenderColumnsDepth(Terminal_TCanvas *canvas, const Raycast_TScene *scene, const Raycast_TCamera *camera, float *outDepth, size_t depthCount);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `scene`: scene material map and styles, with optional borrowed textures.
- `camera`: camera parameters controlling view and clipping.
- `outDepth`: destination per-column depth array.
- `depthCount`: number of depth or row-depth entries supplied.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

All inputs are borrowed and all outputs belong to the caller. No hidden frame state or memory allocation is performed by the core DDA routine. `depthCount` must cover the columns to be drawn.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
float depth[80] = {0};
OPSTATUS status = RAYCAST_FUNC(RenderColumnsDepth)(&canvas, &scene, &camera, depth, 80);
```

# RenderSprites

Projects sprites and clips them against column wall-depth values.

### Syntax

#### Macro form

```c
OPSTATUS RAYCAST_FUNC(RenderSprites)(TERMINAL_TYPE(TCanvas) *canvas, const RAYCAST_TYPE(TCamera) *camera, const RAYCAST_TYPE(TSprite) *sprites, size_t count, const float *columnDepth, size_t depthCount);
```

#### Direct form

```c
OPSTATUS Raycast_RenderSprites(Terminal_TCanvas *canvas, const Raycast_TCamera *camera, const Raycast_TSprite *sprites, size_t count, const float *columnDepth, size_t depthCount);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `camera`: camera parameters controlling view and clipping.
- `sprites`: borrowed array of world sprites.
- `count`: number of points, sprites or objects.
- `columnDepth`: wall-depth buffer for sprite occlusion.
- `depthCount`: number of depth or row-depth entries supplied.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

All inputs are borrowed and all outputs belong to the caller. No hidden frame state or memory allocation is performed by the core DDA routine.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RAYCAST_FUNC(RenderSprites)(&canvas, &camera, sprites, 3, depth, 80);
```

# RenderColumns

Renders wall columns, ceiling and floor with default intermediate-depth management.

### Syntax

#### Macro form

```c
OPSTATUS RAYCAST_FUNC(RenderColumns)(TERMINAL_TYPE(TCanvas) *canvas, const RAYCAST_TYPE(TScene) *scene, const RAYCAST_TYPE(TCamera) *camera);
```

#### Direct form

```c
OPSTATUS Raycast_RenderColumns(Terminal_TCanvas *canvas, const Raycast_TScene *scene, const Raycast_TCamera *camera);
```

### Parameters

- `canvas`: destination Canvas, previously initialized and writable.
- `scene`: scene material map and styles, with optional borrowed textures.
- `camera`: camera parameters controlling view and clipping.

### Return value

`OPSTATUS`: SUCCESS when the operation finishes successfully; INVALID_ARGUMENT, OUT_OF_RANGE, NOT_FOUND, NOT_SUPPORTED or other status as relevant to the operation.

### Remarks

All inputs are borrowed and all outputs belong to the caller. No hidden frame state or memory allocation is performed by the core DDA routine.

### Example

```c
/* Fragment: demais objetos usados aqui devem ter sido inicializados. */
OPSTATUS status = RAYCAST_FUNC(RenderColumns)(&canvas, &scene, &camera);
```

# Requirements and related APIs

- C11, header-only, no mandatory linking of `-lm` or other external libraries for this package.
- The package draws to or computes values for Canvas, never implicitly performs terminal presentation.
- [Canvas](Canvas.md), [Terminal](Terminal.md), [Struct](Struct.md), [Text](Text.md), [Bit](Bit.md).
- Detailed design notes: [Raycasting README](../../Codespace/Cosmeron/Modules/Graphics/Raycasting/README.md).
