# Sprite — Canvas images, layers, viewport and animation

Include `Sprite.h` or `Graphics.h`. Images are existing `TCanvas` owners or
borrowed views; this package has no additional pixel storage, allocation,
initialization, clock, terminal output or mandatory link dependency.

## Direct drawing and layer configuration

```c
Sprite_Draw(&destination, &image, (TDUAL_TYPE(int32)){.x=10, .y=5});

Sprite_TLayer layer = Sprite_Layer(&image);
layer.region = (TQUAD_TYPE(uint16)){.left=0, .top=0, .right=7, .bottom=7};
layer.position = (TDUAL_TYPE(int32)){.x=-2, .y=4};
layer.size = (TDUAL_TYPE(uint16)){.x=16, .y=16};
layer.flipX = true;
layer.useColorKey = true;
layer.colorKey = ' ';
Sprite_DrawWith(&destination, &layer);
```

Names above use default namespaces. Portable macros are `SPRITE_FUNC(Name)`,
`SPRITE_TYPE(TLayer)`, `TDUAL_TYPE(...)`, `TQUAD_TYPE(...)` and
`TERMINAL_TYPE(TCanvas)`. Fallible operations return `OPSTATUS`.

| API | Purpose |
| --- | --- |
| `Layer(source)` | Return a layer describing the whole source at (0,0), native size, opaque and unmirrored. |
| `Draw(destination, source, position)` | Native-size opaque drawing. |
| `DrawWith(destination, layer)` | Cropping, nearest-neighbor scaling, horizontal/vertical mirroring, transparency. |
| `Compose(destination, layers, count, camera)` | Draw layers in array order, subtracting signed world camera position; later layers cover earlier ones. |
| `FrameAt(animation, elapsed, outIndex)` | Select a frame from externally supplied integer time. |
| `DrawFrame(destination, animation, elapsed, placement)` | Use selected frame's source/crop with the placement's size, position, flips and color key. |

The destination canvas/view is the viewport: create a borrowed Canvas view for
an inset viewport. `Compose` moves the camera without mutating layer positions
and does not clear the destination. A zero layer count is a successful no-op.
Position subtraction widens before arithmetic, including extreme signed values.

## Transparency and safety

- A color key skips the **whole cell** when its character equals `colorKey`.
- `TRANSPARENT_BACKGROUND` instead keeps the destination background/default
  background flag while applying the source character, foreground and attributes.
- Source crops are inclusive and must be inside the image. Destination size must
  be nonzero; invisible destination cells are clipped and succeed.
- Source cells must be width-one glyphs; combining/wide/continuation cells return
  `NOT_SUPPORTED`. Existing destination wide glyphs crossing view boundaries are
  preserved by Rasterization. Flips reverse cells, not the glyph's actual shape.
- All layers are validated before `Compose` draws. Invalid input does not leave
  partially composed layers. Source and destination must use different backing
  grids (including views): aliasing returns `INVALID_ARGUMENT`. Use `Canvas_Copy`
  explicitly when an overlapping self-draw is required.
- Layer/image pointers are borrowed. Keep sources alive throughout each call.
  `Layer` on an invalid source produces an unusable layer; draw validation reports
  the error. Drawing preserves canvas cursor and default style.

## Animation without an internal clock

`TFrame` stores a borrowed `source`, inclusive `region` and positive `uint64_t duration`. `TAnimation` stores a borrowed frame array, count and `bool loop`.
The caller chooses a common time unit for duration and elapsed (milliseconds,
microseconds, ticks) and may obtain it from Chronometry. No implicit clock reads.

Exact duration boundaries advance to the next frame. Looping uses elapsed modulo
total duration; nonlooping animation holds its final frame. Zero durations/counts
are invalid; total-duration overflow returns `OUT_OF_RANGE`. `FrameAt` leaves
outIndex unchanged on errors and only inspects timing metadata. `DrawFrame`
validates the selected image/crop when drawing. Positive placement size is explicit;
source/crop fields of the placement are replaced by the selected frame.

See `Codespace/Examples/Graphics/Sprite.c`. Compile with C11 and the existing
POSIX feature define where appropriate, without additional link libraries.
