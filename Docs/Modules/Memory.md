# Memory

The **Memory** module provides memory allocation, byte operations, swapping, and arena allocation with a consistent `OPSTATUS` error model. Its public APIs operate on caller-owned pointers and buffers; the module does not add garbage collection or automatic ownership tracking.

Memory is a **Core** module. Include only the packages you need:

```c
#include "Cosmeron/Core/Memory/Alloc.h"
#include "Cosmeron/Core/Memory/Operations.h"
#include "Cosmeron/Core/Memory/Swap.h"
#include "Cosmeron/Core/Memory/Arena.h"
```

---

# Overview

| Package | Header | Purpose |
| --- | --- | --- |
| [Alloc](#allocation) | `Alloc.h` | Heap allocation, reallocation, and freeing |
| [Operations](#memory-operations) | `Operations.h` | Copy, move, fill, and zero memory |
| [Swap](#swapping) | `Swap.h` | Exchange non-overlapping object bytes |
| [Arena](#arena-allocation) | `Arena.h` | Linear allocation from a reusable buffer |

The library's naming macros resolve automatically to the configured namespace. For example, these two calls are equivalent with the default namespace:

### Macro form

```c
OPSTATUS status = MEMORY_FUNC(Zero)(buffer, sizeof buffer);
```

### Direct form

```c
OPSTATUS status = Memory_Zero(buffer, sizeof buffer);
```

The package-specific functions use `ALLOC_FUNC`, `SWAP_FUNC`, or `ARENA_FUNC`.

With a custom namespace:

```c
#define COSMERON_NAMESPACE MyProject
#include "Cosmeron/Core/Memory/Operations.h"
```

the direct symbol is `MyProject_Memory_Zero`, while `MEMORY_FUNC(Zero)` continues to work without changes.

All regular public functions documented here return `OPSTATUS`; the special `Memory_Free` macro is a statement with no return value.

---

# API reference

## Function summary

| Function | Description | Changes memory |
| --- | --- | --- |
| [`AllocBytes`](#allocbytes) | Allocates an uninitialized byte buffer | Allocates |
| [`AllocArray`](#allocarray) | Allocates a checked count of elements | Allocates |
| [`ReallocBytes`](#reallocbytes) | Resizes a heap buffer | Reallocates |
| [`ReallocArray`](#reallocarray) | Resizes a checked array allocation | Reallocates |
| [`FreePointer`](#freepointer) | Releases a `void *` and clears it | Frees |
| [`Memory_Free`](#memory_free) | Releases and clears a typed pointer lvalue | Frees |
| [`Copy`](#copy) | Copies non-overlapping bytes | Yes |
| [`Move`](#move) | Copies bytes even when regions overlap | Yes |
| [`Set`](#set) | Fills bytes with a value | Yes |
| [`Zero`](#zero) | Sets bytes to zero | Yes |
| [`CopyArray`](#copyarray) | Copies an element count | Yes |
| [`MoveArray`](#movearray) | Moves an element count | Yes |
| [`ZeroArray`](#zeroarray) | Zeros an element count | Yes |
| [`Memory_CopyTyped`](#memory_copytyped) | Typed copy helper | Yes |
| [`Memory_MoveTyped`](#memory_movetyped) | Typed move helper | Yes |
| [`Memory_ZeroTyped`](#memory_zerotyped) | Typed zero helper | Yes |
| [`Swap Bytes`](#swap-bytes) | Swaps two non-overlapping byte ranges | Yes |
| [`Memory_Swap`](#memory_swap) | Typed swap helper | Yes |
| [`Arena Create`](#arena-create) | Creates an arena | Allocates |
| [`Arena AllocBytes`](#arena-allocbytes) | Reserves bytes from an arena | Advances offset |
| [`Arena AllocAligned`](#arena-allocaligned) | Reserves bytes with an alignment | Advances offset |
| [`Arena AllocArray`](#arena-allocarray) | Reserves a checked array | Advances offset |
| [`Arena AllocArrayAligned`](#arena-allocarrayaligned) | Reserves an aligned array | Advances offset |
| [`Arena AllocTyped`](#arena-alloctyped) | Writes an allocated pointer to a pointer object | Advances offset |
| [`Memory_Arena_Alloc`](#memory_arena_alloc) | Typed arena convenience macro | Advances offset |
| [`Arena Reset`](#arena-reset) | Reuses the arena storage | Resets offset |
| [`Arena Destroy`](#arena-destroy) | Releases arena storage | Frees |

---

# Status and ownership rules

- **Success:** `STATUS_CONST(SUCCESS)`. Recoverable failures include `INVALID_ARGUMENT`, `OUT_OF_MEMORY`, `ARITHMETIC_OVERFLOW`, and `OUT_OF_RANGE`, depending on the operation.
- **Output parameters:** allocate/reallocate functions generally change their output only on success; the zero-size success case explicitly clears the output pointer.
- **Ownership:** allocations created by `Memory_AllocBytes` and `Memory_AllocArray` belong to the caller and must be released. Arena allocations belong to their arena and are **not** individually freed.
- **Pointer types:** functions taking `void **` require a pointer to an actual `void *` object. Do **not** cast `T **` to `void **` to suppress the type mismatch. Use a temporary `void *`, check the status, then assign the returned address to the typed pointer.
- **Uninitialized storage:** heap and arena allocation do not zero newly reserved bytes.
- **No implicit rollback:** `Memory_Copy`, `Memory_Move`, and `Memory_Set` operate on raw bytes. They do not validate whether the source or destination addresses reference sufficiently large buffers.

Example of a correct typed heap allocation:

```c
int *numbers = NULL;
void *allocated = NULL;

OPSTATUS status =
    ALLOC_FUNC(AllocArray)(&allocated, 8, sizeof *numbers);

if (status == STATUS_CONST(SUCCESS))
    numbers = allocated;

/* ...use numbers after checking success... */

Memory_Free(numbers);
```

---

# Allocation

The allocation package wraps `malloc`, `realloc`, and `free`, with checks for invalid output pointers and size multiplication overflow. It does **not** overwrite old allocations safely if `AllocBytes` or `AllocArray` is called on an already-owning output variable: use the `Realloc...` functions for resizing.


---

# AllocBytes

Allocates an uninitialized heap block of the requested byte size.

### Syntax

#### Macro form

```c
OPSTATUS status = ALLOC_FUNC(AllocBytes)(&out, size);
```

#### Direct form

``c
OPSTATUS status = Memory_AllocBytes(&out, size);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `out` | Pointer to a writable `void *` output object. |
| `size` | Number of bytes requested. |

---

### Return value

`SUCCESS` on allocation (also for zero size); `INVALID_ARGUMENT` if `out == NULL`; `OUT_OF_MEMORY` if `malloc` fails.

---

### Remarks

- If `size == 0`, stores `NULL` in `*out` and returns success.
- On a failed nonzero allocation, `*out` remains unchanged.
- Bytes are uninitialized. The caller owns the returned allocation.
- Pass a fresh `void *` output object; existing allocations are not freed before `*out` is replaced.

---

### Example

```c
void *buffer = NULL;
OPSTATUS status = ALLOC_FUNC(AllocBytes)(&buffer, 64);
if (status == STATUS_CONST(SUCCESS)) {
    /* Use buffer for 64 bytes. */
    Memory_Free(buffer);
}
```


---

# AllocArray

Allocates `count * elementSize` bytes, checking for multiplication overflow first.

### Syntax

#### Macro form

```c
OPSTATUS status = ALLOC_FUNC(AllocArray)(&out, count, elementSize);
```

#### Direct form

``c
OPSTATUS status = Memory_AllocArray(&out, count, elementSize);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `out` | Pointer to a `void *` output object. |
| `count` | Number of elements. |
| `elementSize` | Size of one element, in bytes. |

---

### Return value

`SUCCESS`, `INVALID_ARGUMENT`, `ARITHMETIC_OVERFLOW`, or `OUT_OF_MEMORY`.

---

### Remarks

- Returns `ARITHMETIC_OVERFLOW` when the byte-size multiplication would exceed `SIZE_MAX`.
- If the product is zero, stores `NULL` in `*out` and returns success.
- On overflow or allocation failure, the output remains unchanged.
- This function does not initialize elements.

---

### Example

```c
void *allocated = NULL;
int *items = NULL;
OPSTATUS status = ALLOC_FUNC(AllocArray)(&allocated, 10, sizeof(int));
if (status == STATUS_CONST(SUCCESS)) {
    items = allocated;
    /* Initialize items before reading them. */
    Memory_Free(items);
}
```


---

# ReallocBytes

Resizes a buffer allocated by the C heap allocator.

### Syntax

#### Macro form

```c
OPSTATUS status = ALLOC_FUNC(ReallocBytes)(&ptr, newSize);
```

#### Direct form

``c
OPSTATUS status = Memory_ReallocBytes(&ptr, newSize);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `ptr` | Pointer to the `void *` heap-pointer object. |
| `newSize` | Requested size in bytes. |

---

### Return value

`SUCCESS`; `INVALID_ARGUMENT` for a null pointer-to-pointer; `OUT_OF_MEMORY` when `realloc` fails.

---

### Remarks

- If `*ptr == NULL`, a nonzero request behaves like a new allocation.
- If `newSize == 0`, frees `*ptr`, sets it to `NULL`, and returns success.
- On allocation failure the original pointer and allocation remain valid.
- On a successful resize, the returned address may differ; previously held interior pointers may become invalid.
- Newly added bytes are uninitialized.

---

### Example

```c
void *bytes = NULL;
OPSTATUS status = ALLOC_FUNC(AllocBytes)(&bytes, 16);
if (status == STATUS_CONST(SUCCESS))
    status = ALLOC_FUNC(ReallocBytes)(&bytes, 32);
/* bytes still owns its allocation if resizing failed. */
Memory_Free(bytes);
```


---

# ReallocArray

Resizes an array with checked multiplication of count and element size.

### Syntax

#### Macro form

```c
OPSTATUS status = ALLOC_FUNC(ReallocArray)(&ptr, count, elementSize);
```

#### Direct form

``c
OPSTATUS status = Memory_ReallocArray(&ptr, count, elementSize);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `ptr` | Pointer to a `void *` heap-pointer object. |
| `count` | New number of elements. |
| `elementSize` | Size of each element in bytes. |

---

### Return value

`SUCCESS`, `INVALID_ARGUMENT`, `ARITHMETIC_OVERFLOW`, or `OUT_OF_MEMORY`.

---

### Remarks

- Overflow is detected before calling `realloc`, keeping the prior allocation unchanged.
- A zero-byte product frees the original allocation and sets the output to `NULL`.
- As with `ReallocBytes`, failure with a nonzero size retains the old pointer.
- For a typed pointer, use an intermediate `void *` storage object.

---

### Example

```c
int *items = NULL;
void *raw = NULL;
OPSTATUS status = ALLOC_FUNC(AllocArray)(&raw, 4, sizeof(int));
if (status == STATUS_CONST(SUCCESS)) {
    items = raw;
    raw = items;
    status = ALLOC_FUNC(ReallocArray)(&raw, 8, sizeof(int));
    if (status == STATUS_CONST(SUCCESS))
        items = raw;
    /* On failure, items still owns the original allocation. */
    Memory_Free(items);
}
```


---

# FreePointer

Frees an allocation through a `void *` variable and clears that variable.

### Syntax

#### Macro form

```c
OPSTATUS status = ALLOC_FUNC(FreePointer)(&ptr);
```

#### Direct form

``c
OPSTATUS status = Memory_FreePointer(&ptr);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `ptr` | Address of a writable `void *` variable containing a heap allocation or `NULL`. |

---

### Return value

`SUCCESS` for any valid pointer-to-pointer, even if `*ptr == NULL`; `INVALID_ARGUMENT` when `ptr == NULL`.

---

### Remarks

- Calls `free(*ptr)` and then sets `*ptr = NULL`.
- Do not pass `&typedPointer` by casting its type to `void **`.
- Does not clear other aliases to the freed allocation.
- Do not free arena-owned storage through this function.

---

### Example

```c
void *buffer = NULL;
OPSTATUS status = ALLOC_FUNC(AllocBytes)(&buffer, 128);
if (status == STATUS_CONST(SUCCESS))
    status = ALLOC_FUNC(FreePointer)(&buffer);
/* buffer == NULL after successful free */
```


---

# Memory_Free

A public convenience macro that releases a pointer lvalue and assigns `NULL` to it.

### Syntax

#### Macro form

```c
Memory_Free(pointerLvalue);
```

#### Direct form

No standalone public direct form; this is a convenience macro.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `pointerLvalue` | A modifiable pointer lvalue allocated through a compatible heap allocator. |

---

### Return value

None. This is a `do { ... } while (0)` statement macro, not an `OPSTATUS` function.

---

### Remarks

- Directly invokes `free((void *)pointerLvalue)` and then assigns `NULL` to `pointerLvalue`.
- Works with typed pointer variables such as `int *`.
- The lvalue expression occurs twice in the macro; avoid expressions with side effects such as `pointers[index++]`.
- Does not make other aliases safe and must not be used with individual arena allocations.
- A direct function cannot set the caller's typed pointer to `NULL` without a different call contract.

---

### Example

```c
int *values = NULL;
void *raw = NULL;
if (ALLOC_FUNC(AllocArray)(&raw, 8, sizeof *values)
    == STATUS_CONST(SUCCESS)) {
    values = raw;
    Memory_Free(values);
    /* values == NULL */
}
```


---

# Memory operations

The `Operations` package wraps byte-level routines using `OPSTATUS`. Functions taking a `size` in bytes treat a zero-byte operation as success before checking the buffer addresses. Nonzero operations need valid, appropriately sized buffers.

Use **`Move`** rather than **`Copy`** when the ranges overlap. All array variants multiply `count * elementSize` with an overflow check.


---

# Copy

Copies bytes from one memory range to another using `memcpy`.

### Syntax

#### Macro form

```c
OPSTATUS status = MEMORY_FUNC(Copy)(destination, source, size);
```

#### Direct form

``c
OPSTATUS status = Memory_Copy(destination, source, size);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `destination` | Writable destination buffer. |
| `source` | Readable source buffer. |
| `size` | Bytes to copy. |

---

### Return value

`SUCCESS`, or `INVALID_ARGUMENT` if a required pointer is null for a nonzero size.

---

### Remarks

- Source and destination ranges **must not overlap**; use `Move` for overlap.
- `size == 0` returns success, including with null pointers.
- The function cannot check physical buffer capacities.

---

### Example

```c
unsigned char source[3] = {1, 2, 3};
unsigned char destination[3];
OPSTATUS status = MEMORY_FUNC(Copy)(destination, source, sizeof source);
```


---

# Move

Moves bytes using `memmove`, supporting overlapping ranges.

### Syntax

#### Macro form

```c
OPSTATUS status = MEMORY_FUNC(Move)(destination, source, size);
```

#### Direct form

``c
OPSTATUS status = Memory_Move(destination, source, size);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `destination` | Writable destination buffer. |
| `source` | Readable source buffer. |
| `size` | Number of bytes. |

---

### Return value

`SUCCESS`, or `INVALID_ARGUMENT` if a required pointer is null for a nonzero size.

---

### Remarks

- Overlapping regions are supported.
- `size == 0` succeeds even for null pointers.
- The destination must have enough accessible storage.

---

### Example

```c
char text[] = "abcde";
OPSTATUS status = MEMORY_FUNC(Move)(text + 1, text, 4);
/* text now begins with "aabcd" */
```


---

# Set

Fills the selected memory bytes with the low-order byte of an integer value.

### Syntax

#### Macro form

```c
OPSTATUS status = MEMORY_FUNC(Set)(destination, value, size);
```

#### Direct form

``c
OPSTATUS status = Memory_Set(destination, value, size);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `destination` | Writable destination buffer. |
| `value` | Byte pattern, converted as by `memset` to `unsigned char`. |
| `size` | Number of bytes to write. |

---

### Return value

`SUCCESS`, or `INVALID_ARGUMENT` if `destination == NULL` with nonzero size.

---

### Remarks

- Writes **each byte**, not each typed array element.
- When `size == 0`, returns success without touching the destination.
- For a typed non-byte array, a byte pattern does not generally mean the corresponding numeric value.

---

### Example

```c
unsigned char flags[4];
OPSTATUS status = MEMORY_FUNC(Set)(flags, 0xFF, sizeof flags);
/* Each of the four bytes is 0xFF. */
```


---

# Zero

Writes zero bytes across a memory region.

### Syntax

#### Macro form

```c
OPSTATUS status = MEMORY_FUNC(Zero)(destination, size);
```

#### Direct form

``c
OPSTATUS status = Memory_Zero(destination, size);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `destination` | Writable destination buffer. |
| `size` | Number of bytes to clear. |

---

### Return value

The status returned by `Set(destination, 0, size)`: `SUCCESS` or `INVALID_ARGUMENT`.

---

### Remarks

- Clears **object representation bytes**; it is not a general-purpose typed initializer.
- All-zero bytes are not guaranteed by ISO C to be a valid null pointer or floating-point zero representation on every target.
- A zero-byte request succeeds without requiring a non-null destination.

---

### Example

```c
unsigned char packet[32];
OPSTATUS status = MEMORY_FUNC(Zero)(packet, sizeof packet);
```


---

# CopyArray

Copies a specified number of same-sized elements by copying their raw bytes.

### Syntax

#### Macro form

```c
OPSTATUS status = MEMORY_FUNC(CopyArray)(destination, source, count, elementSize);
```

#### Direct form

``c
OPSTATUS status = Memory_CopyArray(destination, source, count, elementSize);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `destination` | Writable destination buffer. |
| `source` | Readable source buffer. |
| `count` | Number of elements. |
| `elementSize` | Bytes per element. |

---

### Return value

`SUCCESS`, `ARITHMETIC_OVERFLOW` if the element-size product overflows, or `INVALID_ARGUMENT` for a required null buffer on a nonzero byte count.

---

### Remarks

- Computes `count * elementSize` with overflow protection.
- Like `Copy`, source and destination **must not overlap**.
- Zero total bytes succeeds even when the buffers are null.

---

### Example

```c
int source[3] = {1, 2, 3};
int target[3];
OPSTATUS status = MEMORY_FUNC(CopyArray)(target, source, 3, sizeof(int));
```


---

# MoveArray

Moves a specified number of elements using byte-wise overlap-safe semantics.

### Syntax

#### Macro form

```c
OPSTATUS status = MEMORY_FUNC(MoveArray)(destination, source, count, elementSize);
```

#### Direct form

``c
OPSTATUS status = Memory_MoveArray(destination, source, count, elementSize);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `destination` | Writable destination buffer. |
| `source` | Readable source buffer. |
| `count` | Number of elements. |
| `elementSize` | Bytes per element. |

---

### Return value

`SUCCESS`, `ARITHMETIC_OVERFLOW` if the element-size product overflows, or `INVALID_ARGUMENT` for a required null buffer on a nonzero byte count.

---

### Remarks

- Computes `count * elementSize` with overflow protection.
- Overlap is supported, but the caller must still ensure buffer validity.
- Zero total bytes succeeds even when the buffers are null.

---

### Example

```c
int values[5] = {1, 2, 3, 4, 5};
OPSTATUS status = MEMORY_FUNC(MoveArray)(values + 1, values, 4, sizeof(int));
```


---

# ZeroArray

Clears the raw bytes of a specified number of elements.

### Syntax

#### Macro form

```c
OPSTATUS status = MEMORY_FUNC(ZeroArray)(destination, count, elementSize);
```

#### Direct form

``c
OPSTATUS status = Memory_ZeroArray(destination, count, elementSize);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `destination` | Writable destination buffer. |
| `count` | Number of elements. |
| `elementSize` | Bytes per element. |

---

### Return value

`SUCCESS`, `ARITHMETIC_OVERFLOW` if the element-size product overflows, or `INVALID_ARGUMENT` for a required null buffer on a nonzero byte count.

---

### Remarks

- Computes `count * elementSize` with overflow protection.
- Produces all-zero bytes, which is not a portable general initializer for every C type.
- Zero total bytes succeeds even when the buffers are null.

---

### Example

```c
unsigned char values[10];
OPSTATUS status = MEMORY_FUNC(ZeroArray)(values, 10, sizeof values[0]);
```


---

# Typed operation helpers

The following convenience macros use `sizeof(type)` to select the element size. Unlike the ordinary `MEMORY_FUNC` routines, these helpers have no standalone direct-function symbols with the same signatures.


---

# Memory_CopyTyped

Copies `count` elements of a specified type using `CopyArray`.

### Syntax

#### Macro form

```c
OPSTATUS status = Memory_CopyTyped(type, destination, source, count);
```

#### Direct form

No standalone public direct form; this is a convenience macro.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `type` | C type used for `sizeof(type)`. |
| `destination` | Writable output array. |
| `source` | Source array. |
| `count` | Number of elements. |

---

### Return value

Returns the `OPSTATUS` from `MEMORY_FUNC(CopyArray)`.

---

### Remarks

- Expands to `MEMORY_FUNC(CopyArray)(destination, source, count, sizeof(type))`.
- The buffers must not overlap.

---

### Example

```c
int source[2] = {10, 20};
int dest[2];
OPSTATUS status = Memory_CopyTyped(int, dest, source, 2);
```


---

# Memory_MoveTyped

Moves `count` elements of a specified type using `MoveArray`.

### Syntax

#### Macro form

```c
OPSTATUS status = Memory_MoveTyped(type, destination, source, count);
```

#### Direct form

No standalone public direct form; this is a convenience macro.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `type` | C type used for `sizeof(type)`. |
| `destination` | Writable destination. |
| `source` | Readable source. |
| `count` | Number of elements. |

---

### Return value

Returns the `OPSTATUS` from `MEMORY_FUNC(MoveArray)`.

---

### Remarks

- Expands to `MoveArray` using `sizeof(type)`.
- Overlap is supported.

---

### Example

```c
int values[4] = {1, 2, 3, 4};
OPSTATUS status = Memory_MoveTyped(int, values + 1, values, 3);
```


---

# Memory_ZeroTyped

Clears the raw bytes of `count` objects of a specified type.

### Syntax

#### Macro form

```c
OPSTATUS status = Memory_ZeroTyped(type, destination, count);
```

#### Direct form

No standalone public direct form; this is a convenience macro.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `type` | C type used for `sizeof(type)`. |
| `destination` | Writable destination. |
| `count` | Number of elements. |

---

### Return value

Returns the `OPSTATUS` from `MEMORY_FUNC(ZeroArray)`.

---

### Remarks

- Expands to `ZeroArray(destination, count, sizeof(type))`.
- It does not guarantee semantically valid zero-initialized values for every C type.

---

### Example

```c
unsigned char buffer[16];
OPSTATUS status = Memory_ZeroTyped(unsigned char, buffer, 16);
```


---

# Swapping

The Swap package exchanges two regions without allocating heap memory. `Bytes` processes data in fixed 64-byte temporary blocks and requires **non-overlapping ranges**, except that swapping the very same address is treated as a no-op.


---

# Swap Bytes

Swaps the contents of two equally sized, non-overlapping byte ranges.

### Syntax

#### Macro form

```c
OPSTATUS status = SWAP_FUNC(Bytes)(left, right, size);
```

#### Direct form

``c
OPSTATUS status = Memory_Swap_Bytes(left, right, size);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `left` | Writable first memory region. |
| `right` | Writable second memory region. |
| `size` | Number of bytes to exchange in each region. |

---

### Return value

`SUCCESS`, `INVALID_ARGUMENT` for nonzero null buffers or detected partial overlap, or `OUT_OF_RANGE` if its address-range arithmetic check fails.

---

### Remarks

- A zero size succeeds without pointer checks.
- Identical pointers return success without changes.
- Overlapping but nonidentical ranges are rejected when detected by the address checks.
- Both locations must refer to accessible writable ranges of at least `size` bytes.
- Uses bounded stack storage (64 bytes), even when exchanging larger buffers.

---

### Example

```c
int left = 7;
int right = 9;
OPSTATUS status = SWAP_FUNC(Bytes)(&left, &right, sizeof left);
/* left == 9; right == 7 */
```


---

# Memory_Swap

Swaps two assignable lvalues using their addresses and the size of a supplied type.

### Syntax

#### Macro form

```c
OPSTATUS status = Memory_Swap(type, left, right);
```

#### Direct form

No standalone public direct form; this is a convenience macro.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `type` | C type whose `sizeof` determines the byte count. |
| `left` | Modifiable lvalue for the first object. |
| `right` | Modifiable lvalue for the second object. |

---

### Return value

Returns the `OPSTATUS` from `SWAP_FUNC(Bytes)`.

---

### Remarks

- Expands to `SWAP_FUNC(Bytes)(&(left), &(right), sizeof(type))`.
- Each lvalue is evaluated exactly once because only its address is taken.
- The type must match the actual objects' sizes and representations; do not use unrelated types.
- Nonidentical objects may not overlap. Use `SWAP_FUNC(Bytes)` explicitly when working with byte buffers.
- The source also declares generic `SWAP_PROTOTYPE` / `SWAP_IMPLEMENT` generation macros, but does not instantiate a separate public typed swap-function family.

---

### Example

```c
int valuesA[] = {1, 2};
int valuesB[] = {3, 4};
size_t i = 0, j = 0;
OPSTATUS status = Memory_Swap(int, valuesA[i++], valuesB[j++]);
/* i == 1; j == 1; valuesA[0] == 3; valuesB[0] == 1 */
```


---

# Arena allocation

An arena stores a heap-allocated buffer and dispenses successive slices using a linear offset. Allocating a slice does not call `malloc` again. `Reset` makes its storage available for reuse without freeing it, while `Destroy` releases the entire buffer.

### Arena type

#### Macro form

```c
ARENA_TYPE(TArena) arena = {0};
```

#### Direct form

```c
Memory_Arena_TArena arena = {0};
```

| Field | Meaning |
| --- | --- |
| `buffer` | Pointer to the arena-owned backing storage |
| `capacity` | Total reserved storage in bytes |
| `offset` | Current allocation position |

The caller must initialize and manage the arena's lifetime. **Create once, allocate, optionally reset, and destroy.** Do not call `Create` again on an arena that still owns a buffer: the implementation would overwrite its pointer and leak the previous allocation.

Arena allocations remain owned by the arena, even when returned as typed pointers. `Memory_Free` and `FreePointer` must **not** be called on these slices.

### Alignment contract

The current arena implementation defines its maximum allowed alignment using an internal union with `long double`, `double`, `long long`, and `void *` members. `AllocAligned` rejects zero alignment and alignments greater than this internal limit, but **does not validate powers of two**. It aligns an offset rather than the absolute backing-buffer address; therefore, for portable expectations, request conventional power-of-two alignments that divide the backing buffer's guaranteed alignment. Over-aligned types and arbitrary non-power-of-two alignments are not assured.


---

# Arena Create

Creates the backing buffer for an arena and resets its allocation offset.

### Syntax

#### Macro form

```c
OPSTATUS status = ARENA_FUNC(Create)(&arena, capacity);
```

#### Direct form

``c
OPSTATUS status = Memory_Arena_Create(&arena, capacity);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `arena` | Pointer to a writable arena object. |
| `capacity` | Desired buffer capacity in bytes. |

---

### Return value

`SUCCESS`, `INVALID_ARGUMENT` if arena is null, or `OUT_OF_MEMORY` if backing allocation fails.

---

### Remarks

- Uses `Memory_AllocBytes` for the backing buffer.
- On successful creation, sets `capacity`, `offset = 0`, and the backing buffer.
- A zero capacity is valid and yields a null buffer.
- Do not recreate an already-owning arena without destroying it first.

---

### Example

```c
ARENA_TYPE(TArena) arena = {0};
OPSTATUS status = ARENA_FUNC(Create)(&arena, 1024);
if (status == STATUS_CONST(SUCCESS))
    ARENA_FUNC(Destroy)(&arena);
```


---

# Arena AllocBytes

Allocates a byte sequence inside an arena using the internal default alignment.

### Syntax

#### Macro form

```c
OPSTATUS status = ARENA_FUNC(AllocBytes)(&arena, &out, size);
```

#### Direct form

``c
OPSTATUS status = Memory_Arena_AllocBytes(&arena, &out, size);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `arena` | Pointer to an initialized arena. |
| `out` | Address of a `void *` output object. |
| `size` | Number of bytes to reserve. |

---

### Return value

`SUCCESS`, `INVALID_ARGUMENT`, `OUT_OF_RANGE` if capacity is insufficient, or `ARITHMETIC_OVERFLOW` if padding arithmetic overflows.

---

### Remarks

- Delegates to `AllocAligned` with the implementation's default alignment.
- If `size == 0`, stores `NULL` without advancing the offset.
- On failure, does not update the output pointer or allocation offset.
- Allocation is valid until the arena is reset or destroyed, or its storage is otherwise overwritten.

---

### Example

```c
ARENA_TYPE(TArena) arena = {0};
if (ARENA_FUNC(Create)(&arena, 256) == STATUS_CONST(SUCCESS)) {
    void *bytes = NULL;
    OPSTATUS status = ARENA_FUNC(AllocBytes)(&arena, &bytes, 32);
    /* No individual free for bytes. */
    ARENA_FUNC(Destroy)(&arena);
}
```


---

# Arena AllocAligned

Reserves an aligned byte region from an arena.

### Syntax

#### Macro form

```c
OPSTATUS status = ARENA_FUNC(AllocAligned)(&arena, &out, size, alignment);
```

#### Direct form

``c
OPSTATUS status = Memory_Arena_AllocAligned(&arena, &out, size, alignment);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `arena` | Pointer to an initialized arena. |
| `out` | Address of a `void *` output object. |
| `size` | Number of bytes to reserve. |
| `alignment` | Requested byte alignment. |

---

### Return value

`SUCCESS`, `INVALID_ARGUMENT` for null arguments or invalid alignment, `ARITHMETIC_OVERFLOW` for offset-padding overflow, or `OUT_OF_RANGE` for insufficient capacity.

---

### Remarks

- Alignment must be nonzero and no greater than the internal arena alignment limit.
- The implementation computes padding relative to `arena.offset`, not to the absolute buffer address. Prefer valid power-of-two alignments compatible with the buffer base.
- If `size == 0`, returns a null output without advancing the offset.
- On failure, keeps the arena offset and output pointer unchanged.

---

### Example

```c
ARENA_TYPE(TArena) arena = {0};
if (ARENA_FUNC(Create)(&arena, 256) == STATUS_CONST(SUCCESS)) {
    void *data = NULL;
    OPSTATUS status = ARENA_FUNC(AllocAligned)(
        &arena, &data, 24, _Alignof(int));
    ARENA_FUNC(Destroy)(&arena);
}
```


---

# Arena AllocArray

Reserves space for an array using checked multiplication and the default alignment.

### Syntax

#### Macro form

```c
OPSTATUS status = ARENA_FUNC(AllocArray)(&arena, &out, count, elementSize);
```

#### Direct form

``c
OPSTATUS status = Memory_Arena_AllocArray(&arena, &out, count, elementSize);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `arena` | Initialized arena. |
| `out` | Address of a `void *` output variable. |
| `count` | Number of array elements. |
| `elementSize` | Bytes per element. |

---

### Return value

`SUCCESS`, `INVALID_ARGUMENT`, `ARITHMETIC_OVERFLOW`, or `OUT_OF_RANGE`.

---

### Remarks

- Uses checked `count * elementSize`.
- On successful zero-byte request, outputs `NULL` without advancing the offset.
- No separate heap allocation is made per array.

---

### Example

```c
ARENA_TYPE(TArena) arena = {0};
if (ARENA_FUNC(Create)(&arena, 512) == STATUS_CONST(SUCCESS)) {
    void *raw = NULL;
    OPSTATUS status = ARENA_FUNC(AllocArray)(&arena, &raw, 8, sizeof(int));
    if (status == STATUS_CONST(SUCCESS)) {
        int *values = raw;
        /* Initialize values before reading. */
        (void)values;
    }
    ARENA_FUNC(Destroy)(&arena);
}
```


---

# Arena AllocArrayAligned

Reserves a counted array with an explicit alignment.

### Syntax

#### Macro form

```c
OPSTATUS status = ARENA_FUNC(AllocArrayAligned)(&arena, &out, count, elementSize, alignment);
```

#### Direct form

``c
OPSTATUS status = Memory_Arena_AllocArrayAligned(&arena, &out, count, elementSize, alignment);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `arena` | Initialized arena. |
| `out` | Address of a `void *` result variable. |
| `count` | Number of elements. |
| `elementSize` | Bytes per element. |
| `alignment` | Requested alignment in bytes. |

---

### Return value

`SUCCESS`, `INVALID_ARGUMENT`, `ARITHMETIC_OVERFLOW`, or `OUT_OF_RANGE`.

---

### Remarks

- Checks the byte-size product before delegating to `AllocAligned`.
- Subject to the alignment limitations described above.
- Leaves the output and offset unchanged on failure.

---

### Example

```c
ARENA_TYPE(TArena) arena = {0};
if (ARENA_FUNC(Create)(&arena, 512) == STATUS_CONST(SUCCESS)) {
    void *raw = NULL;
    OPSTATUS status = ARENA_FUNC(AllocArrayAligned)(
        &arena, &raw, 6, sizeof(double), _Alignof(double));
    ARENA_FUNC(Destroy)(&arena);
}
```


---

# Arena AllocTyped

Allocates an aligned array and writes its pointer representation to a caller-supplied pointer object.

### Syntax

#### Macro form

```c
OPSTATUS status = ARENA_FUNC(AllocTyped)(&arena, outPointerObject, count, elementSize, alignment);
```

#### Direct form

``c
OPSTATUS status = Memory_Arena_AllocTyped(&arena, outPointerObject, count, elementSize, alignment);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `arena` | Initialized arena. |
| `outPointerObject` | Pointer to writable pointer-object storage receiving the result. |
| `count` | Number of elements. |
| `elementSize` | Bytes per element. |
| `alignment` | Requested alignment. |

---

### Return value

`SUCCESS`, `INVALID_ARGUMENT`, `ARITHMETIC_OVERFLOW`, or `OUT_OF_RANGE`.

---

### Remarks

- Delegates to `AllocArrayAligned`, then copies `sizeof(void *)` bytes from a temporary `void *` into `outPointerObject` using `memcpy`.
- The code does not validate the size or representation of the destination pointer object. ISO C does not guarantee identical representations for `void *` and every typed object pointer, so this typed convenience mechanism has portability limitations.
- On failure, the destination pointer object is not modified.
- Using the `AllocArrayAligned` variant with a `void *` temporary and ordinary assignment is the more portable typed-pointer pattern.

---

### Example

```c
ARENA_TYPE(TArena) arena = {0};
if (ARENA_FUNC(Create)(&arena, 256) == STATUS_CONST(SUCCESS)) {
    void *raw = NULL;
    OPSTATUS status = ARENA_FUNC(AllocTyped)(
        &arena, &raw, 4, sizeof(int), _Alignof(int));
    if (status == STATUS_CONST(SUCCESS)) {
        int *items = raw;
        (void)items;
    }
    ARENA_FUNC(Destroy)(&arena);
}
```


---

# Arena Reset

Resets the arena offset so its capacity can be reused.

### Syntax

#### Macro form

```c
OPSTATUS status = ARENA_FUNC(Reset)(&arena);
```

#### Direct form

``c
OPSTATUS status = Memory_Arena_Reset(&arena);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `arena` | Pointer to an arena object. |

---

### Return value

`SUCCESS` or `INVALID_ARGUMENT` when the arena pointer is null.

---

### Remarks

- Sets `arena.offset = 0`, without freeing or zeroing the buffer.
- All prior allocations must be treated as no longer reserved; later arena allocations can overwrite their contents.
- A reset does not call destructors or run cleanup for objects stored in the buffer.

---

### Example

```c
ARENA_TYPE(TArena) arena = {0};
if (ARENA_FUNC(Create)(&arena, 128) == STATUS_CONST(SUCCESS)) {
    void *scratch = NULL;
    ARENA_FUNC(AllocBytes)(&arena, &scratch, 32);
    ARENA_FUNC(Reset)(&arena);
    /* New allocations reuse the beginning of the same buffer. */
    ARENA_FUNC(Destroy)(&arena);
}
```


---

# Arena Destroy

Frees the entire arena backing buffer and clears its management fields.

### Syntax

#### Macro form

```c
OPSTATUS status = ARENA_FUNC(Destroy)(&arena);
```

#### Direct form

``c
OPSTATUS status = Memory_Arena_Destroy(&arena);
```

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `arena` | Pointer to an arena object. |

---

### Return value

`SUCCESS` for a valid arena pointer; `INVALID_ARGUMENT` when the arena pointer is null.

---

### Remarks

- Releases the backing buffer through `FreePointer`.
- Sets `buffer = NULL`, `capacity = 0`, and `offset = 0`.
- Previously obtained pointers into that buffer are invalid after destruction.
- Destroy can be called repeatedly on an otherwise valid initialized/cleared arena object.

---

### Example

```c
ARENA_TYPE(TArena) arena = {0};
OPSTATUS status = ARENA_FUNC(Create)(&arena, 1024);
if (status == STATUS_CONST(SUCCESS))
    status = ARENA_FUNC(Destroy)(&arena);
/* arena.buffer == NULL; arena.capacity == 0; arena.offset == 0 */
```


---

# Memory_Arena_Alloc

Convenience macro that allocates an arena array for a named C type.

### Syntax

#### Macro form

```c
OPSTATUS status = Memory_Arena_Alloc(&arena, type, &typedPointer, count);
```

#### Direct form

No standalone public direct form; this is a convenience macro.

---

### Parameters

| Parameter | Explanation |
| --- | --- |
| `arena` | Pointer to a live arena. |
| `type` | C type whose size and alignment are used. |
| `out` | Address of a writable typed pointer object. |
| `count` | Number of objects. |

---

### Return value

Returns the `OPSTATUS` from `ARENA_FUNC(AllocTyped)`.

---

### Remarks

- Expands to `ARENA_FUNC(AllocTyped)(arena, (void *)out, count, sizeof(type), _Alignof(type))`.
- Writes pointer bytes via `memcpy`; see the representation-portability caveat in [Arena AllocTyped](#arena-alloctyped).
- Does not require separate freeing of the returned pointer.
- Use `AllocArrayAligned` plus a `void *` temporary when strict portability matters.

---

### Example

```c
ARENA_TYPE(TArena) arena = {0};
if (ARENA_FUNC(Create)(&arena, 512) == STATUS_CONST(SUCCESS)) {
    int *numbers = NULL;
    OPSTATUS status = Memory_Arena_Alloc(&arena, int, &numbers, 8);
    if (status == STATUS_CONST(SUCCESS) && numbers != NULL)
        numbers[0] = 42;
    ARENA_FUNC(Destroy)(&arena);
}
```


---

# Complete example

The following example combines owned heap allocation, copying, and arena scratch storage. The heap allocation is released through `Memory_Free`; the arena contents are released only when the arena is destroyed.

```c
#include "Cosmeron/Core/Memory/Alloc.h"
#include "Cosmeron/Core/Memory/Operations.h"
#include "Cosmeron/Core/Memory/Arena.h"

int main(void) {
    unsigned char input[4] = {1, 2, 3, 4};
    unsigned char *copy = NULL;
    void *raw = NULL;
    OPSTATUS status;

    status = ALLOC_FUNC(AllocBytes)(&raw, sizeof input);
    if (status != STATUS_CONST(SUCCESS))
        return 1;

    copy = raw;
    status = MEMORY_FUNC(Copy)(copy, input, sizeof input);
    if (status != STATUS_CONST(SUCCESS)) {
        Memory_Free(copy);
        return 2;
    }

    ARENA_TYPE(TArena) arena = {0};
    status = ARENA_FUNC(Create)(&arena, 128);
    if (status != STATUS_CONST(SUCCESS)) {
        Memory_Free(copy);
        return 3;
    }

    void *scratch = NULL;
    status = ARENA_FUNC(AllocBytes)(&arena, &scratch, sizeof input);
    if (status == STATUS_CONST(SUCCESS))
        status = MEMORY_FUNC(Copy)(scratch, copy, sizeof input);

    ARENA_FUNC(Destroy)(&arena);
    Memory_Free(copy);

    return status == STATUS_CONST(SUCCESS) ? 0 : 4;
}
```

---

# Notes

- All current public functions are defined as `static inline` in header-included `.impl` files. No dedicated Memory library needs to be linked.
- `Alloc.h` contains the heap functions, `Operations.h` the byte functions, `Swap.h` the swap function and helper, and `Arena.h` the arena type and operations.
- Array-based functions detect `count * elementSize` overflow before touching output buffers.
- A successful allocation of zero total bytes returns a null pointer rather than allocating storage.
- `Memory_Free` is a macro with a typed-pointer ergonomic benefit, whereas `FreePointer` is a status-returning function for `void *` variables.
- An arena is a linear allocator, not an ownership-tracking collector. Individual arena allocations cannot be resized or freed.
- No `Try`, `Safe`, `Debug`, or garbage-collection API is present in this branch's Memory module; this reference covers **implemented interfaces only**.

