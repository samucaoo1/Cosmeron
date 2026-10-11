# Surface — pixel storage

Header: `Codespace/Cosmeron/Modules/Graphics/Surface/Surface.h`.
Use `GRAPHICS_TYPE(TSurface)` and `GRAPHICS_FUNC(Surface_*)`.

A surface stores numeric `0xAARRGGBB` pixels with straight alpha. Stride and
capacity are counts of uint32_t pixels, not bytes. Rows can contain padding.
Operations touch only visible pixels. Pixel layout in memory follows host
endianness; native presenters convert as needed.

| Operation | Contract |
|---|---|
| IsValid | Validates dimensions, stride, capacity and overflow |
| Create | Allocates zeroed storage; output must be `{0}` |
| Borrow | Borrows an application buffer with explicit stride/capacity |
| Free | Frees owned storage and clears the struct; views only detach |
| Resize | Reallocates owned storage, preserving the intersection; borrowed storage returns NOT_SUPPORTED |
| View | Borrows a rectangular portion; region is x/y/width/height |
| Clear | Fills visible pixels without changing row padding |
| Blend | Returns straight-alpha source-over composition of two pixels |
| Overlaps | Reports overlapping valid backing storage |
| Line | Draws a clipped, alpha-blended line with Struct/TDual int32 endpoints |

Create/Borrow/View require an output with no live storage. Failed operations leave
outputs unchanged. Resizing/freeing an owner invalidates all its views. Copying an
owning struct does not transfer or duplicate ownership. Do not double-free copies.

Line reuses the same canonical, clipped Bresenham walker as terminal
Rasterization. Reversing endpoints preserves halfway ties, and extreme int32
coordinates do not force an unbounded off-screen walk.

---

# API reference

The entries below follow the [Bit.md](Bit.md) convention: direct and namespace-macro signatures, inputs, results, remarks and examples. Examples are illustrative fragments; variables not explicitly declared must already have the correct initialized state.

## Function summary

| Function | Return type | Description |
| --- | --- | --- |
| `GRAPHICS_FUNC(Surface_IsValid)` | `bool` | Validates surface dimensions, stride, capacity and ownership consistency. |
| `GRAPHICS_FUNC(Surface_Create)` | `OPSTATUS` | Allocates a zeroed owned ARGB pixel surface. |
| `GRAPHICS_FUNC(Surface_Borrow)` | `OPSTATUS` | Creates non-owning surface metadata over caller-provided pixels. |
| `GRAPHICS_FUNC(Surface_Free)` | `void` | Frees owned pixels or detaches a borrowed surface, resetting the struct. |
| `GRAPHICS_FUNC(Surface_Resize)` | `OPSTATUS` | Resizes owned pixel storage, preserving the overlapping region. |
| `GRAPHICS_FUNC(Surface_View)` | `OPSTATUS` | Builds a borrowed view of a region in an existing surface. |
| `GRAPHICS_FUNC(Surface_Clear)` | `OPSTATUS` | Sets all visible pixels to an ARGB color without overwriting row padding. |
| `GRAPHICS_FUNC(Surface_Blend)` | `uint32_t` | Composites straight-alpha source over destination. |
| `GRAPHICS_FUNC(Surface_Overlaps)` | `bool` | Tests whether valid surfaces have overlapping backing memory. |
| `GRAPHICS_FUNC(Surface_Line)` | `OPSTATUS` | Draws a clipped alpha-blended line. |

# Surface_IsValid

Validates surface dimensions, stride, capacity and ownership consistency.

### Syntax

#### Macro form

```c
bool GRAPHICS_FUNC(Surface_IsValid)(const GRAPHICS_TYPE(TSurface) * surface);
```

#### Direct form

```c
bool Graphics_Surface_IsValid(const Graphics_TSurface * surface);
```

### Parameters

- `surface`: surface whose pixels may be owned or borrowed.

### Return value

Returns `bool` by value.

### Remarks

Surface pixels are straight-alpha 0xAARRGGBB. Stride and capacity count pixels, not bytes; borrowed views become invalid after owner resize/free.

### Example

```c
bool valid = GRAPHICS_FUNC(Surface_IsValid)(&surface);
```

# Surface_Create

Allocates a zeroed owned ARGB pixel surface.

### Syntax

#### Macro form

```c
OPSTATUS GRAPHICS_FUNC(Surface_Create)(GRAPHICS_TYPE(TSurface) * outSurface, TDUAL_TYPE(uint32) size);
```

#### Direct form

```c
OPSTATUS Graphics_Surface_Create(Graphics_TSurface * outSurface, TDual_uint32 size);
```

### Parameters

- `outSurface`: zero-initialized surface to receive ownership/borrowed metadata.
- `size`: data length or dimensions as declared.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Surface pixels are straight-alpha 0xAARRGGBB. Stride and capacity count pixels, not bytes; borrowed views become invalid after owner resize/free.

### Example

```c
GRAPHICS_TYPE(TSurface) surface = {0};
OPSTATUS status = GRAPHICS_FUNC(Surface_Create)(&surface, (TDUAL_TYPE(uint32)){.x=320,.y=200});
GRAPHICS_FUNC(Surface_Free)(&surface);
```

# Surface_Borrow

Creates non-owning surface metadata over caller-provided pixels.

### Syntax

#### Macro form

```c
OPSTATUS GRAPHICS_FUNC(Surface_Borrow)(GRAPHICS_TYPE(TSurface) * outSurface, uint32_t * pixels, TDUAL_TYPE(uint32) size, size_t stride, size_t capacity);
```

#### Direct form

```c
OPSTATUS Graphics_Surface_Borrow(Graphics_TSurface * outSurface, uint32_t * pixels, TDual_uint32 size, size_t stride, size_t capacity);
```

### Parameters

- `outSurface`: zero-initialized surface to receive ownership/borrowed metadata.
- `pixels`: caller-owned array of uint32_t ARGB pixels.
- `size`: data length or dimensions as declared.
- `stride`: row stride in pixels, not bytes.
- `capacity`: available uint32_t pixel elements, not bytes.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Surface pixels are straight-alpha 0xAARRGGBB. Stride and capacity count pixels, not bytes; borrowed views become invalid after owner resize/free. Caller must keep pixel memory valid and free it independently.

### Example

```c
GRAPHICS_TYPE(TSurface) surface = {0};
uint32_t pixels[100] = {0};
OPSTATUS status = GRAPHICS_FUNC(Surface_Borrow)(&surface, pixels, (TDUAL_TYPE(uint32)){.x=10,.y=10}, 10, 100);
```

# Surface_Free

Frees owned pixels or detaches a borrowed surface, resetting the struct.

### Syntax

#### Macro form

```c
void GRAPHICS_FUNC(Surface_Free)(GRAPHICS_TYPE(TSurface) * surface);
```

#### Direct form

```c
void Graphics_Surface_Free(Graphics_TSurface * surface);
```

### Parameters

- `surface`: surface whose pixels may be owned or borrowed.

### Return value

Does not return a value.

### Remarks

Surface pixels are straight-alpha 0xAARRGGBB. Stride and capacity count pixels, not bytes; borrowed views become invalid after owner resize/free. Repeated cleanup is safe; a view only detaches.

### Example

```c
GRAPHICS_FUNC(Surface_Free)(&surface);
```

# Surface_Resize

Resizes owned pixel storage, preserving the overlapping region.

### Syntax

#### Macro form

```c
OPSTATUS GRAPHICS_FUNC(Surface_Resize)(GRAPHICS_TYPE(TSurface) * surface, TDUAL_TYPE(uint32) size);
```

#### Direct form

```c
OPSTATUS Graphics_Surface_Resize(Graphics_TSurface * surface, TDual_uint32 size);
```

### Parameters

- `surface`: surface whose pixels may be owned or borrowed.
- `size`: data length or dimensions as declared.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Surface pixels are straight-alpha 0xAARRGGBB. Stride and capacity count pixels, not bytes; borrowed views become invalid after owner resize/free.

### Example

```c
OPSTATUS status = GRAPHICS_FUNC(Surface_Resize)(&surface, (TDUAL_TYPE(uint32)){.x=640,.y=480});
```

# Surface_View

Builds a borrowed view of a region in an existing surface.

### Syntax

#### Macro form

```c
OPSTATUS GRAPHICS_FUNC(Surface_View)(GRAPHICS_TYPE(TSurface) * source, TQUAD_TYPE(uint32) region, GRAPHICS_TYPE(TSurface) * outView);
```

#### Direct form

```c
OPSTATUS Graphics_Surface_View(Graphics_TSurface * source, TQuad_uint32 region, Graphics_TSurface * outView);
```

### Parameters

- `source`: source surface or pixel color, according to its type.
- `region`: region or bounds, with coordinates described by the graphics type.
- `outView`: zero-initialized borrowed view.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Surface pixels are straight-alpha 0xAARRGGBB. Stride and capacity count pixels, not bytes; borrowed views become invalid after owner resize/free.

### Example

```c
GRAPHICS_TYPE(TSurface) view = {0};
OPSTATUS status = GRAPHICS_FUNC(Surface_View)(&surface, (TQUAD_TYPE(uint32)){.x=0,.y=0,.z=32,.w=32}, &view);
```

# Surface_Clear

Sets all visible pixels to an ARGB color without overwriting row padding.

### Syntax

#### Macro form

```c
OPSTATUS GRAPHICS_FUNC(Surface_Clear)(GRAPHICS_TYPE(TSurface) * surface, uint32_t color);
```

#### Direct form

```c
OPSTATUS Graphics_Surface_Clear(Graphics_TSurface * surface, uint32_t color);
```

### Parameters

- `surface`: surface whose pixels may be owned or borrowed.
- `color`: numeric 0xAARRGGBB straight-alpha color.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Surface pixels are straight-alpha 0xAARRGGBB. Stride and capacity count pixels, not bytes; borrowed views become invalid after owner resize/free.

### Example

```c
OPSTATUS status = GRAPHICS_FUNC(Surface_Clear)(&surface, UINT32_C(0xFF102030));
```

# Surface_Blend

Composites straight-alpha source over destination.

### Syntax

#### Macro form

```c
uint32_t GRAPHICS_FUNC(Surface_Blend)(uint32_t destination, uint32_t source);
```

#### Direct form

```c
uint32_t Graphics_Surface_Blend(uint32_t destination, uint32_t source);
```

### Parameters

- `destination`: destination color or pixel surface, according to its type.
- `source`: source surface or pixel color, according to its type.

### Return value

Returns `uint32_t` by value.

### Remarks

Surface pixels are straight-alpha 0xAARRGGBB. Stride and capacity count pixels, not bytes; borrowed views become invalid after owner resize/free.

### Example

```c
uint32_t pixel = GRAPHICS_FUNC(Surface_Blend)(UINT32_C(0xFF000000), UINT32_C(0x80FFFFFF));
```

# Surface_Overlaps

Tests whether valid surfaces have overlapping backing memory.

### Syntax

#### Macro form

```c
bool GRAPHICS_FUNC(Surface_Overlaps)(const GRAPHICS_TYPE(TSurface) * left, const GRAPHICS_TYPE(TSurface) * right);
```

#### Direct form

```c
bool Graphics_Surface_Overlaps(const Graphics_TSurface * left, const Graphics_TSurface * right);
```

### Parameters

- `left`: first borrowed surface.
- `right`: second borrowed surface.

### Return value

Returns `bool` by value.

### Remarks

Surface pixels are straight-alpha 0xAARRGGBB. Stride and capacity count pixels, not bytes; borrowed views become invalid after owner resize/free.

### Example

```c
bool shared = GRAPHICS_FUNC(Surface_Overlaps)(&left, &right);
```

# Surface_Line

Draws a clipped alpha-blended line.

### Syntax

#### Macro form

```c
OPSTATUS GRAPHICS_FUNC(Surface_Line)(GRAPHICS_TYPE(TSurface) * surface, TDUAL_TYPE(int32) from, TDUAL_TYPE(int32) to, uint32_t color);
```

#### Direct form

```c
OPSTATUS Graphics_Surface_Line(Graphics_TSurface * surface, TDual_int32 from, TDual_int32 to, uint32_t color);
```

### Parameters

- `surface`: surface whose pixels may be owned or borrowed.
- `from`: first point of a line.
- `to`: second point of a line.
- `color`: numeric 0xAARRGGBB straight-alpha color.

### Return value

Returns OPSTATUS: SUCCESS on completion, or INVALID_ARGUMENT, BUSY, NOT_SUPPORTED, NOT_AVAILABLE or native/size errors when applicable.

### Remarks

Surface pixels are straight-alpha 0xAARRGGBB. Stride and capacity count pixels, not bytes; borrowed views become invalid after owner resize/free.

### Example

```c
OPSTATUS status = GRAPHICS_FUNC(Surface_Line)(&surface, (TDUAL_TYPE(int32)){.x=0,.y=0}, (TDUAL_TYPE(int32)){.x=20,.y=10}, UINT32_C(0xFFFFFFFF));
```

