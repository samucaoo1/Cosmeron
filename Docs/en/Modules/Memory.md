# Memory

The **Memory** module provides allocation, byte operations, allocator interfaces, arenas, pools, a fixed-width heap, bounded binary streams, a tracing collector, smart-pointer families and fixed-width memory blocks. Fallible operations return `OPSTATUS` and use caller-owned output parameters. `Arena`, `Pool`, `Heap` and `Stream` explicitly disable their object function tables. Use direct `MODULE_FUNC(Memory, Type, Method)` calls for Memory objects throughout this guide; the direct form remains valid regardless of whether a particular header enables a function table.

## Headers

~~~c
#include "Cosmeron/Core/Memory/Memory.h"
~~~

The aggregate includes:

| Header | Facility |
| --- | --- |
| `Operations.h` | Copy, move, set, zero and equality of byte spans. |
| `Alloc.h` | Allocate, resize and free raw memory. |
| `Allocator.h` | Type-erased allocator descriptor and capabilities. |
| `../Algorithm/Swap.h` | Byte and array swapping exposed in the Memory module. |
| `Arena.h` | Linear allocation from a buffer or owned backing allocation. |
| `Pool.h` | Fixed-size element pool. |
| `Heap.h` | Page- and size-class-based heap. |
| `Stream.h` | Bounded read/write stream with bit and endian-aware operations. |
| `Collector.h` | Tracing garbage collector and global collector facade. |
| `Pointer/Pointer.h` | Unique, shared, weak, atomic and intrusive pointer wrappers. |

`Block.h` is a separate fixed-width memory-block header and is not included by `Memory.h`; include it directly when required.

---

# Overview

Memory covers several distinct allocation and storage strategies: raw allocation for individual blocks, byte operations for caller-owned spans, arenas and pools for scoped or fixed-size allocation, a page-based heap, bounded streams for binary I/O, and a tracing collector or smart pointers for managed ownership. Choose the narrowest facility that matches the lifetime and ownership model instead of treating every allocator as interchangeable.

---

# API reference

## Raw allocation

Header: `Alloc.h`. Package: `Memory_Alloc`.

| Function | Signature | Description |
| --- | --- | --- |
| `Bytes` | `(void *outPointerObject, size_t size)` | Allocates a byte count and writes the resulting pointer through the output parameter. |
| `Array` | `(void *outPointerObject, size_t count, size_t elementSize)` | Allocates a count of elements with checked size multiplication. |
| `Zero.Bytes` | `(void *outPointerObject, size_t size)` | Allocates zero-initialized bytes. |
| `Zero.Array` | `(void *outPointerObject, size_t count, size_t elementSize)` | Allocates a zero-initialized array. |
| `Realloc.Bytes` | `(void *inoutPointerObject, size_t newSize)` | Resizes the allocation and updates the pointer. |
| `Realloc.Array` | `(void *inoutPointerObject, size_t count, size_t elementSize)` | Resizes the allocation for an element count. |
| `Free` | `(void *inoutPointerObject)` | Releases the allocation and clears the pointer. |

The first argument of allocation and reallocation functions is the address of a pointer variable, passed as `void *`. The allocator writes the new pointer into that variable. Do not pass the allocation pointer itself in place of the pointer variable's address.

~~~c
int *values = NULL;

OPSTATUS status = MODULE_FUNC(Memory, Alloc, Array)(
    &values, 16U, sizeof(*values));

if (status == STATUS_CONST(SUCCESS)) {
    /* use values */
    MODULE_FUNC(Memory, Alloc, Free)(&values);
}
~~~

`Memory_Free(pointerLvalue)` is a convenience macro around `Memory_Alloc_Free` that takes a pointer lvalue. It is a statement-style cleanup helper; the package operation is preferable when its status is needed.

---

## Byte operations

Header: `Operations.h`. Package: `Memory_Operations`.

| Function | Contract |
| --- | --- |
| `Copy(destination, source, size)` | Copies a byte span; source and destination must not overlap. |
| `Copy.Array(destination, source, count, elementSize)` | Copies an array after checked size multiplication. |
| `Move(destination, source, size)` | Moves a byte span and permits overlap. |
| `Move.Array(destination, source, count, elementSize)` | Moves an array after checked size multiplication. |
| `Set(destination, value, size)` | Fills bytes with the low byte of the integer value. |
| `Zero(destination, size)` | Sets a byte span to zero. |
| `Zero.Array(destination, count, elementSize)` | Zeroes an array after checked size multiplication. |
| `Equals(left, right, size)` | Returns whether the byte spans are equal. |

~~~c
MODULE_FUNC(Memory, Operations, Copy)(
    destination, source, byteCount);

MODULE_FUNC(Memory, Operations, Move)(
    destination, source, byteCount);
~~~

Choose `Copy` only for non-overlapping ranges; use `Move` when they can overlap. `Set` repeats a byte value and is not a typed array constructor. Use explicit typed initialization when the element representation is not a repeated byte pattern.

### Swapping

Header: `Core/Algorithm/Swap.h`. This header declares its public API under the Memory module namespace.

| Function | Description |
| --- | --- |
| `Swap.Bytes(left, right, size)` | Exchanges two non-overlapping byte ranges. |
| `Swap.Array(left, right, count, elementSize)` | Exchanges two arrays with checked size multiplication. |
| Typed template specializations | Generate typed value-swapping procedures for a selected C type. |

Swap requires valid, non-overlapping ranges. Identical pointers and zero-sized requests are treated as no-op cases by the implementation. It is not a replacement for overlapping `Move`.

---

## Allocator interface

Header: `Allocator.h`.

`MODULE_TYPE(Memory, Allocator)` is a type-erased allocator value containing an operations table, context and capability flags. The `Allocator` package provides `Default`, `IsValid`, `Alloc` (including `Aligned`), `Calloc`, `Realloc` and `Free`.

Use an allocator when a component should accept caller-selected memory policy without depending on a concrete allocator implementation. The allocator value does not own the external context behind its function pointers; that context must outlive its use.

---

## Arena

Header: `Arena.h`. Type: `MODULE_TYPE(Memory, Arena)`.

Arena allocation is useful for groups of short-lived objects that can be released together. The arena stores a buffer, capacity and current offset.

| Method | Description |
| --- | --- |
| `Create(capacity)` | Creates arena storage of the requested capacity. |
| `Create.Buffer(buffer, capacity)` | Initializes an arena over caller-provided storage. |
| `Alloc.Bytes`, `Alloc.Aligned` | Allocates bytes with optional alignment. |
| `Alloc.Array`, `Alloc.ArrayAligned` | Allocates a count of elements with optional alignment. |
| `Save(outMark)` | Writes the current offset to an output mark. |
| `Restore(mark)` | Restores a previously recorded offset. |
| `Contains(pointer)` | Tests whether a pointer lies within the arena buffer. |
| `Reset()` | Resets the offset to the beginning. |
| `Destroy()` | Releases owned storage and clears arena state. |
| `AsAllocator()` | Returns an allocator interface backed by the arena. |

~~~c
MODULE_TYPE(Memory, Arena) arena = {0};

OPSTATUS status = MODULE_FUNC(Memory, Arena, Create)(&arena, 4096U);
if (status == STATUS_CONST(SUCCESS)) {
    void *memory = NULL;
    status = MODULE_FUNC(Memory, Arena, Alloc, Bytes)(
        &arena, &memory, 128U);
    (void)MODULE_FUNC(Memory, Arena, Destroy)(&arena);
}
~~~

Memory returned by an arena remains valid until the arena is reset, restored past the allocation, destroyed or its backing buffer otherwise becomes invalid. Do not free individual arena allocations with the raw allocator.

---

## Pool

Header: `Pool.h`. Type: `MODULE_TYPE(Memory, Pool)`.

A pool manages a fixed number of equal-sized elements.

| Method | Description |
| --- | --- |
| `Create(elementSize, elementCount)` | Creates pool storage for a fixed element size and count. |
| `Create.Buffer(buffer, bufferSize, elementSize)` | Initializes a pool over caller-owned memory. |
| `Alloc(outPointer)` | Acquires one element. |
| `Free(pointer)` / `Free.Pointer(pointerLvalue)` | Returns an element to the pool. |
| `Reset()` | Marks all elements available again. |
| `Destroy()` | Releases owned backing storage. |
| `Contains(pointer)` | Tests whether a pointer belongs to the pool. |
| `AsAllocator()` | Exposes the pool through the allocator interface. |

Pool allocations have the configured element size and alignment contract. The pool does not track user-defined object constructors/destructors; pair it with Type traits when nontrivial lifecycle is required.

---

## Heap

Header: `Heap.h`. Type: `MODULE_TYPE(Memory, Heap)`.

Heap is a page/size-class allocator. It exposes creation/destruction, allocation policies, freeing, page blacklisting/sweeping and a generic allocator adapter.

The current policy table includes `MANUAL`, `CONSERVATIVE`, `TYPED`, `ATOMIC`, `IMMUTABLE`, `TRACED` and `PINNED`. These are allocation-policy values, not general synchronization guarantees for objects stored in the heap.

| Method | Description |
| --- | --- |
| `Create(initialPages)` / `Destroy()` | Start or tear down heap state. |
| `Alloc` | Allocates using a selected policy and optional type descriptor. |
| `Alloc.Small` / `Alloc.Large` | Explicit small/large allocation paths. |
| `FreeCell(pointer)` | Frees a heap cell. |
| `BlacklistPage(pageIndex)` | Prevents a page from participating in normal allocation. |
| `SweepPage(page, outIsPageEmpty)` | Sweeps one page and reports whether it is empty. |
| `IsBlacklisted(pageIndex)` | Tests whether a page index is blacklisted. |
| `LookupEntry(pointer)` | Looks up the heap entry associated with a pointer. |
| `ResolvePointer(interiorPointer, outBasePointer, outSize, outPolicy)` | Resolves an interior pointer to its allocation metadata. |
| `Contains(pointer)` | Tests whether a pointer belongs to this heap. |
| `LazySweepClass(sizeClassIndex)` | Performs lazy sweeping for a size class. |
| `AsAllocator()` | Returns an allocator adapter. |

The Heap is a concrete allocator. Use `Allocator` when passing it into a generic component. Follow the Heap method contracts for policy and descriptor parameters; the raw allocation functions are not interchangeable with collector-managed allocation.

---

## Binary Stream

Header: `Stream.h`. Type: `MODULE_TYPE(Memory, Stream)`.

A Stream is a bounded view over a caller-provided buffer. It supports read-only and write-capable initialization, seeking, skipping, byte alignment, typed reads/writes, endian-aware integer operations and bit-level access.

| Group | Methods |
| --- | --- |
| Initialization | `Init.Read(buffer, size)`, `Init.Write(buffer, capacity)` |
| Position | `Seek(offset, origin)`, `Skip(count)`, `Rewind()`, `AlignToByte()` |
| State | `Remaining`, `RemainingBits`, `Position`, `BitPosition`, `Capacity`, `IsAtEnd`, `IsReadOnly`, `HasRemaining(count)`, `HasRemainingBits(bitCount)`, `Data`, `Current` |
| Substream | `Slice(size, outSubStream)` |
| Read | `Read.Slice`, `Read.Bytes`, `Read.Bits`, `Read.UInt8`/`Int8`, `UInt16LE`/`BE`, `Int16LE`/`BE`, `UInt32LE`/`BE`, `Int32LE`/`BE`, `UInt64LE`/`BE`, `Int64LE`/`BE` |
| Inspect without advancing | `Peek.Bytes`, `Peek.UInt8`. |
| Write | `Write.Bytes`, `Write.Bits` and the matching integer/endianness variants. |

~~~c
MODULE_TYPE(Memory, Stream) stream = {0};
uint32_t value = 0U;

OPSTATUS status = MODULE_FUNC(Memory, Stream, Init, Read)(
    &stream, input, inputSize);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Memory, Stream, Read, UInt32BE)(
        &stream, &value);
}
~~~

Endianness is selected by the operation name and is independent of host byte order. The stream never grows its backing buffer. Reads and writes validate the current position, requested size and available capacity.

---

## Collector

Header: `Collector.h`.

The Collector combines Heap allocation with root tracking, tracing, pause/resume, finalizer support and a global convenience interface.

### Instance collector

Type: `MODULE_TYPE(Memory, Collector)`.

| Method | Description |
| --- | --- |
| `Create(initialHeapPages, stackBottom)` / `Destroy()` | Initializes or destroys an independent collector. |
| `Alloc`, `Alloc.Typed`, `Alloc.Atomic` | Allocates raw, descriptor-aware or atomic storage. |
| `SetFinalizer(pointer, finalizer)` | Associates a finalizer callback with an allocation. |
| `PinRoot(rootPointerAddress)` / `UnpinRoot(rootPointerAddress)` | Registers or unregisters the address of a root pointer slot. |
| `Collect()` | Runs a collection cycle. |
| `Pause()` / `Resume()` | Temporarily suspends or resumes collection. |
| `AsAllocator()` | Exposes the collector through the allocator interface. |

### Global collector

The `GlobalCollector` package provides `Startup(stackBottom)`, `Shutdown()`, `DefaultCollector()`, corresponding allocation/finalizer/root operations, `Collect`, `Pause`/`Resume`, `LiveBytes` and `Contains`.

The root API receives the **address of a root pointer variable**, not the address of the allocation. Pin and unpin the same root slot, and keep that slot valid while registered. The stack-bottom argument must match the platform/thread stack-boundary contract. Finalizers can run user code; collection is not equivalent to calling `free` on one allocation.

---

## Smart-pointer families

Header: `Pointer/Pointer.h`.

The aggregate provides generated families for:

| Family | Ownership model |
| --- | --- |
| `Unique` | One exclusive owning handle. |
| `Shared` | Reference-counted shared ownership. |
| `Weak` | Non-owning reference to a Shared control block. |
| `AtomicShared` | Shared reference-count operations using atomic control updates. |
| `AtomicWeak` | Atomic weak-reference operations. |
| `Intrusive` | Reference counting stored in the user object's control fields. |

The principal generated methods are:

| Family | Construction and ownership | Access and state |
| --- | --- | --- |
| `Unique` | `FromPointer`, `Alloc`, `AllocZero`, `Reset`, `Move`, `Release`, `Free` | `IsValid`, `Get`, `GetMutable`, `Swap` |
| `Shared` | `From.Pointer`, `From.Weak` (upgrade), `Alloc`, `Alloc.Zero`, `Reset`, `Reset.WithPointer`, `Free` | `Get`, `Get.Mutable`, `UseCount`, `Unique`, `Swap` |
| `Weak` | `Clone`, `Move`, `Reset`, `Free` | `IsExpired`, `IsValid`, `UseCount`, `Swap` |
| `AtomicShared` | Shared-like `From`, `Alloc`, `Reset`, `Free` operations | `Get`, `Get.Mutable`, `UseCount`, `Unique` |
| `AtomicWeak` | Weak-like `Clone`, `Move`, `Reset`, `Free` operations | `IsExpired`, `IsValid`, `UseCount` |
| `Intrusive` | `FromPointer`, `FromExistingPointer`, `Clone`, `Move`, `Reset`, `Release`, `Free` | `Get`, `GetMutable`, `UseCount`, `Unique`, `Swap` |

The exact type suffix and instantiation parameters come from the pointer-family type tables and `.def` templates; do not assume every arbitrary C type has a built-in specialization. Each family has its own ownership contract, and `Free`/`Reset` are not interchangeable across families.

A weak pointer does not own the object. Upgrade it to a Shared pointer before accessing the object, and check the returned status because the object may already have expired. Atomic reference counting does not make the pointed-to object itself thread-safe.

---

## Fixed-width Block

Header: `Block.h` (include directly; not part of `Memory.h`).

Block generates compile-time-width objects backed by `uint64_t` limbs. It provides clear/set-all, bitwise `And`/`Or`/`Xor`/`Not`, shifts, rotations, bit set/clear/flip/check, population count, any/all/none predicates and equality. Use it for fixed-width masks and binary values that do not require dynamic resizing.

---

# Contracts and ownership rules

- Every pointer returned by an allocator must be released through the same allocator family that created it.
- `Alloc` and `Realloc` receive the address of a pointer variable; pass `&pointer`.
- `Copy` requires non-overlapping ranges; `Move` permits overlap.
- Arena and Pool may expose caller-owned buffer modes; do not release those buffers through the raw allocator.
- A Stream borrows its buffer and never owns or grows it.
- Collector roots are addresses of pointer slots, not raw object addresses.
- Smart-pointer ownership semantics are family-specific; a raw pointer returned by an accessor is not automatically an ownership transfer.
- Check every `OPSTATUS` before using an output pointer or assuming that a resource was acquired.

## Related modules

- [Types](Types.md) — lifecycle descriptors and type-erased operations.
- [Condition](Condition.md) — status and error conventions.
- [Algorithm](Algorithm.md) — comparison and swapping contracts.
- [Bit](Bit.md) — bit operations and fixed-size bitsets.

## Source headers

- `Codespace/Cosmeron/Core/Memory/Memory.h`
- `Codespace/Cosmeron/Core/Memory/Alloc.h`
- `Codespace/Cosmeron/Core/Memory/Operations.h`
- `Codespace/Cosmeron/Core/Memory/Allocator.h`
- `Codespace/Cosmeron/Core/Memory/Arena.h`
- `Codespace/Cosmeron/Core/Memory/Pool.h`
- `Codespace/Cosmeron/Core/Memory/Heap.h`
- `Codespace/Cosmeron/Core/Memory/Stream.h`
- `Codespace/Cosmeron/Core/Memory/Collector.h`
- `Codespace/Cosmeron/Core/Memory/Pointer/Pointer.h`
- `Codespace/Cosmeron/Core/Algorithm/Swap.h`
