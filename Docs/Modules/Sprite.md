# Sprite

Canvas-backed layers with cropping, scaling, flips, transparency and explicit frame animation; no separate sprite allocation or hidden clock.

```c
#include "Cosmeron/Modules/Graphics/Sprite/Sprite.h"
```

---

# Overview

No hidden initialization, internal framebuffer or mandatory -lm link. Public calls use `SPRITE_FUNC(Name)` and default direct form `Sprite_Name`. All source Canvas, node, style, animation and frame data are borrowed; the caller owns them.

# API reference

## Function summary

| Function | Return | Purpose |
|---|---|---|
| `SPRITE_FUNC(Layer)` | `SPRITE_TYPE(TLayer)` | Create a borrowed layer description from a Canvas. |
| `SPRITE_FUNC(Draw)` | `OPSTATUS` | Draw an image directly at a signed position. |
| `SPRITE_FUNC(DrawWith)` | `OPSTATUS` | Draw a configured layer. |
| `SPRITE_FUNC(Compose)` | `OPSTATUS` | Draw ordered layers with a camera offset. |
| `SPRITE_FUNC(FrameAt)` | `OPSTATUS` | Select an animation frame for elapsed time. |
| `SPRITE_FUNC(DrawFrame)` | `OPSTATUS` | Draw a chosen animation frame with placement. |

# Layer

Create a borrowed layer description from a Canvas.

### Syntax

#### Macro form

```c
SPRITE_TYPE(TLayer) SPRITE_FUNC(Layer)(const TERMINAL_TYPE(TCanvas) *source);
```

#### Direct form

```c
Sprite_TLayer Sprite_Layer(const Terminal_TCanvas *source);
```

### Parameters

- `source`: borrowed image Canvas.

### Return value

Returns `SPRITE_TYPE(TLayer)` by value.

### Remarks

Layers borrow their source and do not clear or present the destination automatically. Animation time must be supplied explicitly.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
SPRITE_TYPE(TLayer) layer = SPRITE_FUNC(Layer)(&source);
```

# Draw

Draw an image directly at a signed position.

### Syntax

#### Macro form

```c
OPSTATUS SPRITE_FUNC(Draw)( TERMINAL_TYPE(TCanvas) *canvas, const TERMINAL_TYPE(TCanvas) *source, TDUAL_TYPE(int32) position);
```

#### Direct form

```c
OPSTATUS Sprite_Draw( Terminal_TCanvas *canvas, const Terminal_TCanvas *source, TDual_int32 position);
```

### Parameters

- `canvas`: destination initialized Canvas.
- `source`: borrowed image Canvas.
- `position`: signed cell position.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Layers borrow their source and do not clear or present the destination automatically. Animation time must be supplied explicitly.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
OPSTATUS status = SPRITE_FUNC(Draw)(&canvas, &source, (TDUAL_TYPE(int32)){.x=10,.y=4});
```

# DrawWith

Draw a configured layer.

### Syntax

#### Macro form

```c
OPSTATUS SPRITE_FUNC(DrawWith)( TERMINAL_TYPE(TCanvas) *canvas, const SPRITE_TYPE(TLayer) *layer);
```

#### Direct form

```c
OPSTATUS Sprite_DrawWith( Terminal_TCanvas *canvas, const Sprite_TLayer *layer);
```

### Parameters

- `canvas`: destination initialized Canvas.
- `layer`: layer description.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Layers borrow their source and do not clear or present the destination automatically. Animation time must be supplied explicitly.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
OPSTATUS status = SPRITE_FUNC(DrawWith)(&canvas, &layer);
```

# Compose

Draw ordered layers with a camera offset.

### Syntax

#### Macro form

```c
OPSTATUS SPRITE_FUNC(Compose)( TERMINAL_TYPE(TCanvas) *canvas, const SPRITE_TYPE(TLayer) *layers, size_t count, TDUAL_TYPE(int32) camera);
```

#### Direct form

```c
OPSTATUS Sprite_Compose( Terminal_TCanvas *canvas, const Sprite_TLayer *layers, size_t count, TDual_int32 camera);
```

### Parameters

- `canvas`: destination initialized Canvas.
- `layers`: array of layers drawn in order.
- `count`: number of objects or layers.
- `camera`: camera configuration.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Layers borrow their source and do not clear or present the destination automatically. Animation time must be supplied explicitly.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
OPSTATUS status = SPRITE_FUNC(Compose)(&canvas, layers, layerCount, (TDUAL_TYPE(int32)){.x=0,.y=0});
```

# FrameAt

Select an animation frame for elapsed time.

### Syntax

#### Macro form

```c
OPSTATUS SPRITE_FUNC(FrameAt)( const SPRITE_TYPE(TAnimation) *animation, uint64_t elapsed, size_t *outIndex);
```

#### Direct form

```c
OPSTATUS Sprite_FrameAt( const Sprite_TAnimation *animation, uint64_t elapsed, size_t *outIndex);
```

### Parameters

- `animation`: frames and durations borrowed from caller.
- `elapsed`: caller-supplied elapsed time.
- `outIndex`: receives selected frame index.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Layers borrow their source and do not clear or present the destination automatically. Animation time must be supplied explicitly. outIndex is written when a frame is selected successfully.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
size_t index = 0;
OPSTATUS status = SPRITE_FUNC(FrameAt)(&animation, 125, &index);
```

# DrawFrame

Draw a chosen animation frame with placement.

### Syntax

#### Macro form

```c
OPSTATUS SPRITE_FUNC(DrawFrame)( TERMINAL_TYPE(TCanvas) *canvas, const SPRITE_TYPE(TAnimation) *animation, uint64_t elapsed, const SPRITE_TYPE(TLayer) *placement);
```

#### Direct form

```c
OPSTATUS Sprite_DrawFrame( Terminal_TCanvas *canvas, const Sprite_TAnimation *animation, uint64_t elapsed, const Sprite_TLayer *placement);
```

### Parameters

- `canvas`: destination initialized Canvas.
- `animation`: frames and durations borrowed from caller.
- `elapsed`: caller-supplied elapsed time.
- `placement`: source placement options.

### Return value

`OPSTATUS`: SUCCESS on completion; invalid parameters, unsupported conditions and other errors return the corresponding status.

### Remarks

Layers borrow their source and do not clear or present the destination automatically. Animation time must be supplied explicitly.

### Example

```c
/* Fragment: named objects must be initialized beforehand. */
OPSTATUS status = SPRITE_FUNC(DrawFrame)(&canvas, &animation, 125, &placement);
```

# Requirements and see also

C11, header-only. No automatic terminal presentation, global clock or mandatory -lm. See [Canvas](Canvas.md), [Terminal](Terminal.md), [Struct](Struct.md) and [Bit](Bit.md). The package implementation notes are in [README](../../Codespace/Cosmeron/Modules/Graphics/Sprite/README.md).
