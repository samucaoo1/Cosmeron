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
