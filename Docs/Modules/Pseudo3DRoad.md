# Pseudo3DRoad

Pseudo-3D road projection, track sampling, screen-space rendering and depth-aware world objects. Borrowed track nodes use world x/y/z for lateral position, elevation and depth.

```c
#include "Cosmeron/Modules/Graphics/Pseudo3DRoad/Render.h"
```

---

# Overview

No hidden initialization, internal framebuffer or mandatory -lm link. Public calls use `ROAD_FUNC(Name)` and default direct form `Road_Name`. All source Canvas, node, style, animation and frame data are borrowed; the caller owns them.

# API reference

## Function summary

| Function | Return | Purpose |
|---|---|---|
| `ROAD_FUNC(Project)` | `OPSTATUS` | Project a road node into viewport perspective. |
| `ROAD_FUNC(TrackValidate)` | `OPSTATUS` | Check at least two valid nodes and increasing z. |
| `ROAD_FUNC(TrackSample)` | `OPSTATUS` | Sample a node at a requested forward z. |
| `ROAD_FUNC(RenderDepth)` | `OPSTATUS` | Draw road, also writing per-row depth. |
| `ROAD_FUNC(RenderObjects)` | `OPSTATUS` | Draw world objects with row-depth occlusion. |
| `ROAD_FUNC(StyleDefault)` | `ROAD_TYPE(TStyle)` | Create a default road rendering style. |
| `ROAD_FUNC(Render)` | `OPSTATUS` | Draw a road scene into Canvas. |

# Project

Project a road node into viewport perspective.

### Syntax

#### Macro form

```c
OPSTATUS ROAD_FUNC(Project)( const ROAD_TYPE(TCamera) *camera, TDUAL_TYPE(uint16) viewport, const ROAD_TYPE(TNode) *node, ROAD_TYPE(TProjection) *outProjection);
```

#### Direct form

```c
OPSTATUS Road_Project( const Road_TCamera *camera, TDual_uint16 viewport, const Road_TNode *node, Road_TProjection *outProjection);
```

### Parameters

- `camera`: camera configuration.
- `viewport`: width and height in cells.
- `node`: road node in world space.
- `outProjection`: projection output.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Track nodes must have finite coordinates, positive width and strictly increasing z; the viewport and camera must meet their documented constraints.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
ROAD_TYPE(TProjection) projected = {0};
OPSTATUS status = ROAD_FUNC(Project)(&camera, (TDUAL_TYPE(uint16)){.x=80,.y=25}, &node, &projected);
```

# TrackValidate

Check at least two valid nodes and increasing z.

### Syntax

#### Macro form

```c
OPSTATUS ROAD_FUNC(TrackValidate)(const ROAD_TYPE(TTrack) *track);
```

#### Direct form

```c
OPSTATUS Road_TrackValidate(const Road_TTrack *track);
```

### Parameters

- `track`: borrowed valid track.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Track nodes must have finite coordinates, positive width and strictly increasing z; the viewport and camera must meet their documented constraints.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
OPSTATUS status = ROAD_FUNC(TrackValidate)(&track);
```

# TrackSample

Sample a node at a requested forward z.

### Syntax

#### Macro form

```c
OPSTATUS ROAD_FUNC(TrackSample)( const ROAD_TYPE(TTrack) *track, float z, ROAD_TYPE(TNode) *outNode);
```

#### Direct form

```c
OPSTATUS Road_TrackSample( const Road_TTrack *track, float z, Road_TNode *outNode);
```

### Parameters

- `track`: borrowed valid track.
- `z`: world-space forward coordinate.
- `outNode`: interpolated node output.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Track nodes must have finite coordinates, positive width and strictly increasing z; the viewport and camera must meet their documented constraints.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
ROAD_TYPE(TNode) sampled = {0};
OPSTATUS status = ROAD_FUNC(TrackSample)(&track, 12.0f, &sampled);
```

# RenderDepth

Draw road, also writing per-row depth.

### Syntax

#### Macro form

```c
OPSTATUS ROAD_FUNC(RenderDepth)( TERMINAL_TYPE(TCanvas) *canvas, const ROAD_TYPE(TTrack) *track, const ROAD_TYPE(TCamera) *camera, const ROAD_TYPE(TStyle) *style, float *outRowDepth, size_t depthCount);
```

#### Direct form

```c
OPSTATUS Road_RenderDepth( Terminal_TCanvas *canvas, const Road_TTrack *track, const Road_TCamera *camera, const Road_TStyle *style, float *outRowDepth, size_t depthCount);
```

### Parameters

- `canvas`: destination initialized Canvas.
- `track`: borrowed valid track.
- `camera`: camera configuration.
- `style`: render style.
- `outRowDepth`: array receiving row depth.
- `depthCount`: number of depth entries.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Track nodes must have finite coordinates, positive width and strictly increasing z; the viewport and camera must meet their documented constraints. depthCount must cover canvas rows.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
float depth[25] = {0};
OPSTATUS status = ROAD_FUNC(RenderDepth)(&canvas, &track, &camera, &style, depth, 25);
```

# RenderObjects

Draw world objects with row-depth occlusion.

### Syntax

#### Macro form

```c
OPSTATUS ROAD_FUNC(RenderObjects)( TERMINAL_TYPE(TCanvas) *canvas, const ROAD_TYPE(TCamera) *camera, const ROAD_TYPE(TObject) *objects, size_t count, const float *rowDepth, size_t depthCount);
```

#### Direct form

```c
OPSTATUS Road_RenderObjects( Terminal_TCanvas *canvas, const Road_TCamera *camera, const Road_TObject *objects, size_t count, const float *rowDepth, size_t depthCount);
```

### Parameters

- `canvas`: destination initialized Canvas.
- `camera`: camera configuration.
- `objects`: array of borrowed objects.
- `count`: number of objects or layers.
- `rowDepth`: previously generated depth array.
- `depthCount`: number of depth entries.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Track nodes must have finite coordinates, positive width and strictly increasing z; the viewport and camera must meet their documented constraints. The rowDepth data provides world-object occlusion.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
OPSTATUS status = ROAD_FUNC(RenderObjects)(&canvas, &camera, objects, objectCount, depth, rowCount);
```

# StyleDefault

Create a default road rendering style.

### Syntax

#### Macro form

```c
ROAD_TYPE(TStyle) ROAD_FUNC(StyleDefault)(void);
```

#### Direct form

```c
Road_TStyle Road_StyleDefault(void);
```

### Parameters

- None.

### Return value

Returns `ROAD_TYPE(TStyle)` by value.

### Remarks

Track nodes must have finite coordinates, positive width and strictly increasing z; the viewport and camera must meet their documented constraints.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
ROAD_TYPE(TStyle) style = ROAD_FUNC(StyleDefault)();
```

# Render

Draw a road scene into Canvas.

### Syntax

#### Macro form

```c
OPSTATUS ROAD_FUNC(Render)( TERMINAL_TYPE(TCanvas) *canvas, const ROAD_TYPE(TTrack) *track, const ROAD_TYPE(TCamera) *camera, const ROAD_TYPE(TStyle) *style);
```

#### Direct form

```c
OPSTATUS Road_Render( Terminal_TCanvas *canvas, const Road_TTrack *track, const Road_TCamera *camera, const Road_TStyle *style);
```

### Parameters

- `canvas`: destination initialized Canvas.
- `track`: borrowed valid track.
- `camera`: camera configuration.
- `style`: render style.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Track nodes must have finite coordinates, positive width and strictly increasing z; the viewport and camera must meet their documented constraints.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
OPSTATUS status = ROAD_FUNC(Render)(&canvas, &track, &camera, &style);
```

# Requirements and see also

C11, header-only. No automatic terminal presentation, global clock or mandatory -lm. See [Canvas](Canvas.md), [Terminal](Terminal.md), [Struct](Struct.md) and [Bit](Bit.md). The package implementation notes are in [README](../../Codespace/Cosmeron/Modules/Graphics/Pseudo3DRoad/README.md).
