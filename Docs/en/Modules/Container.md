# Container

The **Container** module provides generated C data structures for ordered storage, linked storage, associative lookup, trees, graphs, iteration and bit collections. It belongs to **Modules**. Its public API is built around typed `MODULE_TYPED` specializations, `MODULE_FUNC` package functions and `OBJECT_CALL` object methods.

~~~c
#include "Cosmeron/Modules/Container/Container.h"
~~~

The aggregate header includes DynamicBitset, Iterator, Array, Linked, Tree, Hash and Graph. Individual package headers may be included when a translation unit needs only a subset of the module.

---

# Overview

| Family | Aggregate header | Structures |
| --- | --- | --- |
| Array | `Array/Array.h` | Vector, Queue, Stack, Ring, Span, Heap |
| Linked | `Linked/Linked.h` | Forward, List, Deque, Queue, Stack, Ring and node types |
| Hash | `Hash/Hash.h` | Set and Map |
| Tree | `Tree/Tree.h` | BST, AVL, RedBlack and BTree; Set and Map variants |
| Graph | `Graph/Graph.h` | Directed graph with typed vertices and edges |
| Iterator | `Iterator/Iterator.h` | Forward, Bidirectional, Contiguous, Circular and Stream iterators |
| Bitset | `Bitset/DynamicBitset.h` | Runtime-sized bitset |

Array and Linked templates are instantiated from the ordered-container type table. Hash and Tree use key/value type tables appropriate to their storage and comparison contracts. Use the suffixes declared by those tables; a generated specialization is not a run-time generic type.

---

# Calling convention and lifecycle

A generated object type is named with `MODULE_TYPED(Module, Family, Suffix)`. Package-level functions are accessed with `MODULE_FUNC(Module, Family, Suffix, Function)`. After creation, object methods are called through `OBJECT_CALL` (or `OBJECT_CALL_0` for a method without arguments).

~~~c
MODULE_TYPED(Array, Vector, Int32) values;
OPSTATUS status = MODULE_FUNC(Array, Vector, Int32, Create)(&values, 4U);

if (status == STATUS_CONST(SUCCESS)) {
    (void)OBJECT_CALL(&values, PushBack, 10);
    (void)OBJECT_CALL(&values, PushBack, 20);

    size_t count = OBJECT_CALL_0(&values, Count);
    (void)count;

    OBJECT_CALL_0(&values, Destroy);
}
~~~

`Create` initializes an object and acquires any backing storage it needs. `Dispose` is the status-returning cleanup operation; `Destroy` is a convenience procedure that invokes disposal. Check `OPSTATUS` for fallible operations. Do not use an object after it has been disposed or destroyed.

Most containers store elements by value. If the element type is a pointer, the container stores that pointer value; it does not automatically own or free the pointed-to object. Pointers returned by accessors and iterators can be invalidated by erase, reserve, reallocation, rehash or destruction, depending on the structure.

---

# Array containers

The Array aggregate is `Array/Array.h`.

## Vector

Header: `Array/Vector.h`.

Vector is a dynamically growing contiguous array. It is appropriate when indexed access and cache-friendly iteration matter.

| Group | Methods |
| --- | --- |
| Construction | `Create(initialCapacity)`, `FromBuffer(buffer, count)`, `Clone(outClone)` |
| Storage | `Reserve(newCapacity)`, `ShrinkToFit`, `Resize(newSize, fillValue)`, `Clear`, `Dispose`, `Destroy` |
| Access | `At`, `ConstAt`, `Get`, `Set`, `Front`, `Back`, `Data`, `ConstData` |
| End operations | `PushBack`, `PopBack`, `PushFront`, `PopFront` |
| Editing | `Insert(position, value)`, `Erase(position, count)`, `RemoveAt`, `SwapRemove` |
| State | `IsEmpty`, `Empty`, `Size`, `Count`, `Capacity` |

`At` returns an element pointer through an output parameter; `Get` copies an element into an output value. `Set` replaces an element. `SwapRemove` removes an element by moving the final element into its position, so it does not preserve the order of the remaining elements.

## Queue

Header: `Array/Queue.h`.

The array Queue is a FIFO structure backed by a dynamic array. It provides `Create`, `Compact`, `Reserve`, `ShrinkToFit`, `Clear`, `Push`/`Enqueue`, `Pop`/`Dequeue`, `Front`, `Back`, `Peek`, `IsEmpty`, `Size`, `Count` and `Capacity`.

Use Queue for first-in/first-out work scheduling. `Peek` reads the next value without removing it; `Pop`/`Dequeue` removes the front value and writes it through the supplied output pointer.

## Stack

Header: `Array/Stack.h`.

The array Stack is LIFO. Its main methods are `Create`, `Reserve`, `ShrinkToFit`, `Clear`, `Push`, `Pop`, `Top`, `Peek`, `IsEmpty`, `Size`, `Count` and `Capacity`.

Use Stack for nested work, parsing, undo history and depth-first traversal. `Top` returns access to the stored element; `Peek` copies the value to an output parameter without removing it.

## Ring

Header: `Array/Ring.h`.

Ring is a fixed-capacity circular buffer. It provides `Create(capacity)`, `FromBuffer`, `Dispose`, `Destroy`, `Clear`, `Push`/`PushBack`, `Pop`/`PopFront`, `Peek`, `Overwrite`, `Front`, `Back`, `At`, `IsEmpty`, `IsFull`, `Size`, `Count`, `Capacity` and `Available`.

The `Read`/`Write` operations and `GetReadSpan`/`GetWriteSpan` with `AdvanceRead`/`AdvanceWrite` expose contiguous portions of the circular buffer for bulk transfer. `Overwrite` can return a discarded element through an output pointer; use ordinary `Push` when existing data must not be discarded.

## Span

Header: `Array/Span.h`.

Span is a non-owning view over an existing contiguous buffer. It provides `FromBuffer`, `At`, `ConstAt`, `Get`, `Set`, `Front`, `Back`, `Data`, `ConstData`, `Size`, `Count`, `SizeBytes` and `IsEmpty`.

Span does not allocate, reallocate or free the source buffer. It is useful for passing a bounded range to an API without copying; its lifetime must not exceed that of the underlying storage.

## Priority Heap

Header: `Array/Heap.h`.

This is a binary priority heap, distinct from `Core/Memory/Heap.h`. It supports min-heap and max-heap construction through `CreateMin` and `CreateMax`, plus `FromBuffer`, `Dispose`, `Destroy`, `Clear`, `Reserve`, `ShrinkToFit`, `Heapify`, `Push`, `Pop`, `Peek`, `Top`, `Replace`, `PushPop`, `Size`, `Count` and `Capacity`.

Use it for priority queues and repeated selection of the highest- or lowest-priority item. Its comparison closure determines ordering; use the generated comparator type expected by the template rather than casting an unrelated function pointer.

---

# Linked containers

The Linked aggregate is `Linked/Linked.h`. Linked containers use separately allocated nodes and are suited to workloads where insertion and removal at known ends or positions matter more than contiguous storage.

| Header | Structure | Principal operations |
| --- | --- | --- |
| `Linked/Forward.h` | Singly linked forward chain | `Create`, `Clear`, `PushFront`, `PushBack`, `PopFront`, `Front`, `Back`, `Head`, `Tail`, `Size`, `Count`, `IsEmpty` |
| `Linked/List.h` | Doubly linked list | `Create`, `Clear`, `PushFront`, `PushBack`, `PopFront`, `PopBack`, `Front`, `Back`, `Head`, `Tail`, `Size`, `Count`, `IsEmpty` |
| `Linked/Deque.h` | Double-ended queue | `Create`, `Clear`, `PushFront`, `PushBack`, `PopFront`, `PopBack`, `Front`, `Back`, `Size`, `Count`, `IsEmpty` |
| `Linked/Queue.h` | Linked FIFO queue | `Create`, `Clear`, `Push`/`Enqueue`, `Pop`/`Dequeue`, `Peek`, `Front`, `Back`, `Size`, `Count`, `IsEmpty` |
| `Linked/Stack.h` | Linked LIFO stack | `Create`, `Clear`, `Push`, `Pop`, `Peek`, `Top`, `Size`, `Count`, `IsEmpty` |
| `Linked/Ring.h` | Linked circular structure | Creation, insertion/removal and traversal operations defined by the Ring template |

The linked templates also provide node-oriented access and visitor closures. Use the `Visitor` closure type declared by the relevant template when visiting elements. Nodes and element pointers are container-managed; do not manually free a node returned by an accessor.

The linked structures have the same lifecycle convention as array containers: create or initialize before use, then call `Dispose` or `Destroy`. `Clear` removes elements while retaining a usable container object.

---

# Hash containers

The Hash aggregate is `Hash/Hash.h` and includes Set and Map. Both use hash-based lookup; they are not ordered containers.

## Hash Set

Header: `Hash/Set.h`.

A Set stores unique keys. Its interface includes `Create`, `Clear`, `Dispose`, `Destroy`, `Rehash`, `Reserve`, `Insert`, `Remove`, `Find`, `Contains`, `IsEmpty`, `Size`, `Count` and `Capacity`. Its iterator exposes `IsValid`, `HasNext`, `Reset`, `Get`, `ConstGet`, `Key`, `Value`, `StepNext` and `Next`.

Use Set for membership tests and uniqueness. Key equality and hashing must be consistent: equal keys must produce the same hash value. Rehashing can invalidate iterators and pointers into the table.

## Hash Map

Header: `Hash/Map.h`.

A Map associates a key with a value. It provides `Create`, `Clear`, `Dispose`, `Destroy`, `Rehash`, `Reserve`, `Insert`, `Upsert`, `Remove`, `Find`, `Contains`, `IsEmpty`, `Size`, `Count` and `Capacity`. Its iterator provides key/value access and forward traversal.

`Insert` follows the insert contract for a new key; `Upsert` inserts or replaces the value for an existing key. `Find(key, &valuePointer)` returns the address of a stored value when found.

~~~c
MODULE_TYPED(Hash, Map, Int32_Int32) map;
OPSTATUS status = MODULE_FUNC(Hash, Map, Int32_Int32, Create)(&map);

if (status == STATUS_CONST(SUCCESS)) {
    (void)OBJECT_CALL(&map, Upsert, 1, 100);

    int32_t *found = NULL;
    if (OBJECT_CALL(&map, Find, 1, &found) == STATUS_CONST(SUCCESS) && found)
        printf("%d\n", (int)*found);

    OBJECT_CALL_0(&map, Destroy);
}
~~~

---

# Trees

The Tree aggregate is `Tree/Tree.h`. It includes Binary Search Tree (BST), AVL, Red-Black and B-Tree implementations. Each implementation has Set and Map variants generated from key and value type combinations.

Shared tree operations include `Create`, `Clear`, `Dispose`, `Destroy`, `Insert`, `Remove`, `Find`, `Contains`, `IsEmpty`, `Size` and `Count`. Depending on the implementation, additional operations include `Min`/`Max`, `MinKey`/`MaxKey`, `Height` and forward or bidirectional iterator traversal. Map variants also expose `Upsert`.

| Structure | Characteristic | Use when |
| --- | --- | --- |
| BST | Simple binary search tree; shape depends on insertion order | A basic ordered tree is sufficient and input shape is controlled |
| AVL | Height-balanced binary search tree | Bounded height and lookup performance are priorities |
| RedBlack | Red-black balanced search tree | A balanced ordered tree with update-friendly balancing is required |
| BTree | Multiway search tree with configurable minimum degree | Node fan-out and tree height matter, including page-oriented storage |

A Set stores keys; a Map stores key/value pairs. Use the iterator type and ordering contract of the selected structure. Structural modifications can invalidate iterators and pointers to stored values.

---

# Graph

Header: `Graph/Graph.h`.

Graph generates typed vertex and edge objects and a graph container. Its public operations include `Create`, `Clear`, `Dispose`, `Destroy`, `AddVertex`, `RemoveVertex`, `AddEdge`, `RemoveEdge`, `GetVertex`, `HasEdge`, `OutDegree`, `VertexCount` and `EdgeCount`. Vertex and edge iterators provide validity checks, access and traversal.

The current Graph API models directed edges. A bidirectional connection must be represented by adding both directions. Account for index changes when vertices are removed, and check the returned status before using an index or pointer produced through an output parameter.

---

# Iterators

The iterator aggregate is `Iterator/Iterator.h` and includes Stream, Contiguous, Circular, Forward and Bidirectional typed iterator templates. Their operations depend on the underlying traversal category; the shared concepts include `IsValid`, `HasNext`, `HasPrev`, `Remaining`, `Reset`, `Get`, `ConstGet`, `Value`, `Set`, `Next`, `Prev`, `StepNext`, `StepPrev`, `StepValue`, `StepPtr`, `Seek`, `Index` and `Advance`.

- Use Contiguous with array-backed storage and index-based movement.
- Use Forward for one-direction traversal.
- Use Bidirectional when the source supports movement in both directions.
- Use Circular when traversal wraps around the underlying sequence.
- Use Stream when traversal is driven by a stream callback/state contract.

Iterators generally borrow the container or buffer. Destroying, rehashing, resizing or structurally modifying the source may invalidate them.

---

# Dynamic Bitset

Header: `Bitset/DynamicBitset.h`.

`MODULE_TYPE(Bitset, Dynamic)` stores a runtime-selected bit count and uses a configurable `Memory_Allocator`. Its methods include `Create`, `Create.WithAllocator`, `Dispose`, `Reserve`, `Resize`, `Clone`, `Clear`, `SetAll`, `And`, `Or`, `Xor`, `Not`, shifts, individual-bit and range operations, `Count`, `Size`, `Capacity`, `Any`, `All`, `None`, `Equal`, `Is.Subset`, `Is.Disjoint`, `Intersects` and `Find.FirstSet`/`FirstZero`/`NextSet`.

~~~c
MODULE_TYPE(Bitset, Dynamic) bits = {0};
OPSTATUS status = API(&bits, Create, 128U);
if (status == STATUS_CONST(SUCCESS)) {
    API(&bits, Bit, Set, 7U);
    API(&bits, Dispose);
}
~~~

The bitset owns its allocated limbs and must be disposed. A custom allocator must remain valid for the lifetime of the bitset.

---

# Choosing a container

| Requirement | Recommended family |
| --- | --- |
| Indexed contiguous storage and random access | Array Vector |
| FIFO or LIFO with dynamic backing storage | Array Queue / Array Stack |
| Bounded reusable FIFO storage | Array Ring |
| View over existing contiguous data without copying | Span |
| Repeated priority selection | Array Heap |
| Frequent linked insertion/removal | Linked List / Deque |
| Membership and key lookup | Hash Set / Hash Map |
| Ordered key traversal | BST / AVL / RedBlack / BTree |
| Vertices and directed edges | Graph |
| Runtime-sized bit collection | Dynamic Bitset |

# Notes

- Use `MODULE_TYPED` for generated object types, `MODULE_FUNC` for package functions and `OBJECT_CALL` for object methods.
- `Create` and fallible mutation methods return `OPSTATUS`; check it before accessing the object.
- `Destroy` is a convenience cleanup procedure; use `Dispose` when its status is needed.
- Containers store C values by value unless a particular type explicitly provides an ownership wrapper.
- Do not assume that a container is thread-safe unless its own header documents synchronization.
