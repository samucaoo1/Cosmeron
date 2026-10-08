# Container

The **Container** module provides type-specialized containers in C11: vectors, flat queues and stacks, code-unit strings, linked lists and deques, hash maps, directed weighted graphs, and ordered trees. Its functions are generated from header macros, use the library's configurable namespace, and follow an explicit ownership/status contract.

There is **no single Container.h umbrella header** in this branch. Include the specific packages needed.

---

# Overview

| Family | Package | Header | Description |
| --- | --- | --- | --- |
| Array | [Vector](#vector-package) | `Array/Vector.h` | Contiguous dynamic array |
| Array | [Flat Queue](#flat-queue-package) | `Array/Queue.h` | Array-based FIFO |
| Array | [Flat Stack](#flat-stack-package) | `Array/Stack.h` | Array-based LIFO |
| Array | [TString](#tstring-package) | `Array/String.h` | Growable zero-terminated code units |
| Linked | [Forward List](#forward-list-package) | `Linked/Forward.h` | Singly linked list |
| Linked | [Linked List](#linked-list-package) | `Linked/List.h` | Doubly linked list |
| Linked | [Linked Deque](#linked-deque-package) | `Linked/Deque.h` | Doubly linked deque |
| Linked | [Linked Queue](#linked-queue-package) | `Linked/Queue.h` | Linked FIFO |
| Linked | [Linked Stack](#linked-stack-package) | `Linked/Stack.h` | Linked LIFO |
| Hash | [Hash Map](#hash-map-package) | `Hash/Hash.h` | Open-addressed hash table |
| Graph | [Graph](#graph-package) | `Graph/Graph.h` | Directed weighted adjacency graph |
| Tree | [Tree](#tree-package) | `Tree/Tree.h` | BST, AVL and red-black sets and maps |

### Macro form

```c
#include "Cosmeron/Modules/Container/Array/Vector.h"

FLAT_VECTOR_TYPE(int) vec = {0};
FLAT_VECTOR_FUNC(int, Init)(&vec);
FLAT_VECTOR_FUNC(int, PushBack)(&vec, 42);
FLAT_VECTOR_FUNC(int, Destroy)(&vec);
```

### Direct form

```c
#include "Cosmeron/Modules/Container/Array/Vector.h"

Container_Flat_TVector_int vec = {0};
Container_Flat_Vector_int_Init(&vec);
Container_Flat_Vector_int_PushBack(&vec, 42);
Container_Flat_Vector_int_Destroy(&vec);
```

These are the two naming forms of the same generated functions when using the default namespace. The macro form continues to work after configuring `COSMERON_NAMESPACE` before inclusion.

---

# Shared conventions

- Containers own their arrays, buckets, and linked nodes; they store ordinary C values by copying. A stored pointer is **not** a request to free the object it points to.
- Call the package's `Init` or an appropriate declaration helper before using an instance. Release owned storage with `Destroy`.
- Fallible operations return `OPSTATUS`; element outputs are generally passed through an out pointer. Some observers return `bool`, `size_t`, or node pointers directly.
- Pointers returned by accessors refer to container-managed storage. Erase/delete, relocation or rehash may invalidate them.
- Containers do not provide automatic synchronization. Protect simultaneous accesses when concurrent mutation is possible.
- Type-generating macros instantiate C declarations and `static inline` functions at **compile time**, not runtime.

---

# Supported built-in types

| Family | Built-in instantiations |
| --- | --- |
| Flat Vector/Queue/Stack | `int`, `float`, `double` |
| TString | `uint8_t` (suffix `8`), `uint16_t` (`16`), `uint32_t` (`32`) |
| Forward/List/Deque/Linked Queue/Linked Stack | `int`, `float`, `double`, `char`; List also includes internal `void_ptr` |
| Hash Map | int→int, int→void*, C-string→int, C-string→void* |
| Graph | int vertex data, int or float weights |
| BST/AVL/RB | Set int/float/double; Map int→int and int→float |

### Function table switch

By default, generated objects can carry an optional `api` pointer to a typed function table. Set `CONTAINER_DISABLE_FUNCTION_TABLE` before package includes to disable it; the Tree family also supports `TREE_DISABLE_FUNCTION_TABLE`. This changes the generated type layout. Keep build configuration consistent across translation units.

---

# Operation reference

Each function section below includes **macro form**, **default direct form**, **parameters**, **return value**, **remarks**, and an example. Examples for one operation may assume a previously initialized container and appropriately typed output variables; complete standalone examples appear after the per-package reference.

---

# Vector package

Header: `Cosmeron/Modules/Container/Array/Vector.h`.

Growable contiguous storage; growth, insertion and erasure may move values and invalidate internal pointers.

### Instantiation

```c
FLAT_VECTOR_TYPE(int) vec = {0};
FLAT_VECTOR_FUNC(int, Init)(&vec);
```

Functions in this section use the `int` specialization. Built-in variants are listed in the Overview. The short examples assume the object has been initialized and output variables have the matching C type.

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#vector-init) | Initializes the container. |
| [`Destroy`](#vector-destroy) | Releases the container-owned storage. |
| [`Reserve`](#vector-reserve) | Reserves storage for a requested capacity. |
| [`ShrinkToFit`](#vector-shrinktofit) | Attempts to reduce unused reserved storage. |
| [`At`](#vector-at) | Returns an element pointer at an index. |
| [`Front`](#vector-front) | Returns a pointer to the first element. |
| [`Back`](#vector-back) | Returns a pointer to the last element. |
| [`Data`](#vector-data) | Returns a pointer to internal contiguous data. |
| [`PushBack`](#vector-pushback) | Appends a value. |
| [`PopBack`](#vector-popback) | Removes and optionally reports the last value. |
| [`Insert`](#vector-insert) | Inserts an element or string at the requested index. |
| [`Erase`](#vector-erase) | Removes an indexed range. |
| [`Clear`](#vector-clear) | Removes elements without destroying the object. |
| [`PushFront`](#vector-pushfront) | Prepends a value. |
| [`PopFront`](#vector-popfront) | Removes and optionally reports the first value. |
| [`Empty`](#vector-empty) | Returns true when the collection is empty. |
| [`Size`](#vector-size) | Returns the element count. |
| [`Capacity`](#vector-capacity) | Returns the currently allocated element capacity. |

---

# Vector Init

Initializes the container.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, Init)(FLAT_VECTOR_TYPE(int) * vec);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_Init(Container_Flat_TVector_int * vec);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Initializes the container.

---

### Example

```c
OPSTATUS status = FLAT_VECTOR_FUNC(int, Init)(&vec);
```

---

# Vector Destroy

Releases the container-owned storage.

### Syntax

#### Macro form

```c
void FLAT_VECTOR_FUNC(int, Destroy)(FLAT_VECTOR_TYPE(int) * vec);
```

#### Direct form

```c
void Container_Flat_Vector_int_Destroy(Container_Flat_TVector_int * vec);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |

---

### Return value

None.

---

### Remarks

Releases the container-owned storage.

---

### Example

```c
FLAT_VECTOR_FUNC(int, Destroy)(&vec);
```

---

# Vector Reserve

Reserves storage for a requested capacity.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, Reserve)(FLAT_VECTOR_TYPE(int) * vec, size_t newCapacity);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_Reserve(Container_Flat_TVector_int * vec, size_t newCapacity);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |
| `newCapacity` | `size_t newCapacity` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Reserves storage for a requested capacity. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = FLAT_VECTOR_FUNC(int, Reserve)(&vec, 16);
```

---

# Vector ShrinkToFit

Attempts to reduce unused reserved storage.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, ShrinkToFit)(FLAT_VECTOR_TYPE(int) * vec);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_ShrinkToFit(Container_Flat_TVector_int * vec);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Attempts to reduce unused reserved storage. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = FLAT_VECTOR_FUNC(int, ShrinkToFit)(&vec);
```

---

# Vector At

Returns an element pointer at an index.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, At)(FLAT_VECTOR_TYPE(int) * vec, size_t index, int **out);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_At(Container_Flat_TVector_int * vec, size_t index, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |
| `index` | `size_t index` | Scalar argument or value. |
| `out` | `int **out` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns an element pointer at an index. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
int *found = NULL;
OPSTATUS status = FLAT_VECTOR_FUNC(int, At)(&vec, 0, &found);
```

---

# Vector Front

Returns a pointer to the first element.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, Front)(FLAT_VECTOR_TYPE(int) * vec, int **out);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_Front(Container_Flat_TVector_int * vec, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |
| `out` | `int **out` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns a pointer to the first element. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
int *found = NULL;
OPSTATUS status = FLAT_VECTOR_FUNC(int, Front)(&vec, &found);
```

---

# Vector Back

Returns a pointer to the last element.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, Back)(FLAT_VECTOR_TYPE(int) * vec, int **out);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_Back(Container_Flat_TVector_int * vec, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |
| `out` | `int **out` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns a pointer to the last element. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
int *found = NULL;
OPSTATUS status = FLAT_VECTOR_FUNC(int, Back)(&vec, &found);
```

---

# Vector Data

Returns a pointer to internal contiguous data.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, Data)(FLAT_VECTOR_TYPE(int) *vec, int **outData);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_Data(Container_Flat_TVector_int *vec, int **outData);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) *vec` | Pointer argument; see the operation's contract. |
| `outData` | `int **outData` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns a pointer to internal contiguous data. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
int *found = NULL;
OPSTATUS status = FLAT_VECTOR_FUNC(int, Data)(&vec, &found);
```

---

# Vector PushBack

Appends a value.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, PushBack)(FLAT_VECTOR_TYPE(int) * vec, int value);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_PushBack(Container_Flat_TVector_int * vec, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |
| `value` | `int value` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Appends a value. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = FLAT_VECTOR_FUNC(int, PushBack)(&vec, 42);
```

---

# Vector PopBack

Removes and optionally reports the last value.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, PopBack)(FLAT_VECTOR_TYPE(int) * vec, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_PopBack(Container_Flat_TVector_int * vec, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |
| `outValue` | `int * outValue` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Removes and optionally reports the last value.

---

### Example

```c
int removed = 0;
OPSTATUS status = FLAT_VECTOR_FUNC(int, PopBack)(&vec, &removed);
```

---

# Vector Insert

Inserts an element or string at the requested index.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, Insert)(FLAT_VECTOR_TYPE(int) * vec, size_t position, int value);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_Insert(Container_Flat_TVector_int * vec, size_t position, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |
| `position` | `size_t position` | Scalar argument or value. |
| `value` | `int value` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Inserts an element or string at the requested index. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = FLAT_VECTOR_FUNC(int, Insert)(&vec, 0, 42);
```

---

# Vector Erase

Removes an indexed range.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, Erase)(FLAT_VECTOR_TYPE(int) * vec, size_t position, size_t count);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_Erase(Container_Flat_TVector_int * vec, size_t position, size_t count);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |
| `position` | `size_t position` | Scalar argument or value. |
| `count` | `size_t count` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Removes an indexed range. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = FLAT_VECTOR_FUNC(int, Erase)(&vec, 0, 1);
```

---

# Vector Clear

Removes elements without destroying the object.

### Syntax

#### Macro form

```c
void FLAT_VECTOR_FUNC(int, Clear)(FLAT_VECTOR_TYPE(int) * vec);
```

#### Direct form

```c
void Container_Flat_Vector_int_Clear(Container_Flat_TVector_int * vec);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |

---

### Return value

None.

---

### Remarks

Removes elements without destroying the object.

---

### Example

```c
FLAT_VECTOR_FUNC(int, Clear)(&vec);
```

---

# Vector PushFront

Prepends a value.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, PushFront)(FLAT_VECTOR_TYPE(int) * vec, int value);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_PushFront(Container_Flat_TVector_int * vec, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |
| `value` | `int value` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Prepends a value. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = FLAT_VECTOR_FUNC(int, PushFront)(&vec, 42);
```

---

# Vector PopFront

Removes and optionally reports the first value.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_VECTOR_FUNC(int, PopFront)(FLAT_VECTOR_TYPE(int) * vec, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Flat_Vector_int_PopFront(Container_Flat_TVector_int * vec, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |
| `outValue` | `int * outValue` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Removes and optionally reports the first value.

---

### Example

```c
int removed = 0;
OPSTATUS status = FLAT_VECTOR_FUNC(int, PopFront)(&vec, &removed);
```

---

# Vector Empty

Returns true when the collection is empty.

### Syntax

#### Macro form

```c
bool FLAT_VECTOR_FUNC(int, Empty)(const FLAT_VECTOR_TYPE(int) * vec);
```

#### Direct form

```c
bool Container_Flat_Vector_int_Empty(const Container_Flat_TVector_int * vec);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `const FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |

---

### Return value

`bool` predicate.

---

### Remarks

Returns true when the collection is empty.

---

### Example

```c
bool empty = FLAT_VECTOR_FUNC(int, Empty)(&vec);
```

---

# Vector Size

Returns the element count.

### Syntax

#### Macro form

```c
size_t FLAT_VECTOR_FUNC(int, Size)(const FLAT_VECTOR_TYPE(int) * vec);
```

#### Direct form

```c
size_t Container_Flat_Vector_int_Size(const Container_Flat_TVector_int * vec);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `const FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |

---

### Return value

`size_t` count or capacity.

---

### Remarks

Returns the element count.

---

### Example

```c
size_t count = FLAT_VECTOR_FUNC(int, Size)(&vec);
```

---

# Vector Capacity

Returns the currently allocated element capacity.

### Syntax

#### Macro form

```c
size_t FLAT_VECTOR_FUNC(int, Capacity)(const FLAT_VECTOR_TYPE(int) * vec);
```

#### Direct form

```c
size_t Container_Flat_Vector_int_Capacity(const Container_Flat_TVector_int * vec);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `vec` | `const FLAT_VECTOR_TYPE(int) * vec` | Pointer argument; see the operation's contract. |

---

### Return value

`size_t` count or capacity.

---

### Remarks

Returns the currently allocated element capacity.

---

### Example

```c
size_t count = FLAT_VECTOR_FUNC(int, Capacity)(&vec);
```

---

# Flat Queue package

Header: `Cosmeron/Modules/Container/Array/Queue.h`.

FIFO queue with a head index. Compact shifts live elements toward the start; pointers into storage may become stale.

### Instantiation

```c
FLAT_QUEUE_TYPE(int) queue = {0};
FLAT_QUEUE_FUNC(int, Init)(&queue);
```

Functions in this section use the `int` specialization. Built-in variants are listed in the Overview. The short examples assume the object has been initialized and output variables have the matching C type.

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#flat-queue-init) | Initializes the container. |
| [`Destroy`](#flat-queue-destroy) | Releases the container-owned storage. |
| [`Reserve`](#flat-queue-reserve) | Reserves storage for a requested capacity. |
| [`ShrinkToFit`](#flat-queue-shrinktofit) | Attempts to reduce unused reserved storage. |
| [`Compact`](#flat-queue-compact) | Moves remaining queue items toward the beginning. |
| [`Push`](#flat-queue-push) | Enqueues/pushes a value. |
| [`Pop`](#flat-queue-pop) | Dequeues/pops and reports a value. |
| [`Front`](#flat-queue-front) | Returns a pointer to the first element. |
| [`Back`](#flat-queue-back) | Returns a pointer to the last element. |
| [`Empty`](#flat-queue-empty) | Returns true when the collection is empty. |
| [`Size`](#flat-queue-size) | Returns the element count. |
| [`Capacity`](#flat-queue-capacity) | Returns the currently allocated element capacity. |

---

# Flat Queue Init

Initializes the container.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_QUEUE_FUNC(int, Init)(FLAT_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
OPSTATUS Container_Flat_Queue_int_Init(Container_Flat_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Initializes the container.

---

### Example

```c
OPSTATUS status = FLAT_QUEUE_FUNC(int, Init)(&queue);
```

---

# Flat Queue Destroy

Releases the container-owned storage.

### Syntax

#### Macro form

```c
void FLAT_QUEUE_FUNC(int, Destroy)(FLAT_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
void Container_Flat_Queue_int_Destroy(Container_Flat_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |

---

### Return value

None.

---

### Remarks

Releases the container-owned storage.

---

### Example

```c
FLAT_QUEUE_FUNC(int, Destroy)(&queue);
```

---

# Flat Queue Reserve

Reserves storage for a requested capacity.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_QUEUE_FUNC(int, Reserve)(FLAT_QUEUE_TYPE(int) * queue, size_t newCapacity);
```

#### Direct form

```c
OPSTATUS Container_Flat_Queue_int_Reserve(Container_Flat_TQueue_int * queue, size_t newCapacity);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |
| `newCapacity` | `size_t newCapacity` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Reserves storage for a requested capacity. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = FLAT_QUEUE_FUNC(int, Reserve)(&queue, 16);
```

---

# Flat Queue ShrinkToFit

Attempts to reduce unused reserved storage.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_QUEUE_FUNC(int, ShrinkToFit)(FLAT_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
OPSTATUS Container_Flat_Queue_int_ShrinkToFit(Container_Flat_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Attempts to reduce unused reserved storage. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = FLAT_QUEUE_FUNC(int, ShrinkToFit)(&queue);
```

---

# Flat Queue Compact

Moves remaining queue items toward the beginning.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_QUEUE_FUNC(int, Compact)(FLAT_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
OPSTATUS Container_Flat_Queue_int_Compact(Container_Flat_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Moves remaining queue items toward the beginning.

---

### Example

```c
OPSTATUS status = FLAT_QUEUE_FUNC(int, Compact)(&queue);
```

---

# Flat Queue Push

Enqueues/pushes a value.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_QUEUE_FUNC(int, Push)(FLAT_QUEUE_TYPE(int) * queue, int value);
```

#### Direct form

```c
OPSTATUS Container_Flat_Queue_int_Push(Container_Flat_TQueue_int * queue, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |
| `value` | `int value` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Enqueues/pushes a value.

---

### Example

```c
OPSTATUS status = FLAT_QUEUE_FUNC(int, Push)(&queue, 42);
```

---

# Flat Queue Pop

Dequeues/pops and reports a value.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_QUEUE_FUNC(int, Pop)(FLAT_QUEUE_TYPE(int) * queue, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Flat_Queue_int_Pop(Container_Flat_TQueue_int * queue, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |
| `outValue` | `int * outValue` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Dequeues/pops and reports a value.

---

### Example

```c
int removed = 0;
OPSTATUS status = FLAT_QUEUE_FUNC(int, Pop)(&queue, &removed);
```

---

# Flat Queue Front

Returns a pointer to the first element.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_QUEUE_FUNC(int, Front)(FLAT_QUEUE_TYPE(int) * queue, int **out);
```

#### Direct form

```c
OPSTATUS Container_Flat_Queue_int_Front(Container_Flat_TQueue_int * queue, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |
| `out` | `int **out` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns a pointer to the first element. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
int *found = NULL;
OPSTATUS status = FLAT_QUEUE_FUNC(int, Front)(&queue, &found);
```

---

# Flat Queue Back

Returns a pointer to the last element.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_QUEUE_FUNC(int, Back)(FLAT_QUEUE_TYPE(int) * queue, int **out);
```

#### Direct form

```c
OPSTATUS Container_Flat_Queue_int_Back(Container_Flat_TQueue_int * queue, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |
| `out` | `int **out` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns a pointer to the last element. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
int *found = NULL;
OPSTATUS status = FLAT_QUEUE_FUNC(int, Back)(&queue, &found);
```

---

# Flat Queue Empty

Returns true when the collection is empty.

### Syntax

#### Macro form

```c
bool FLAT_QUEUE_FUNC(int, Empty)(const FLAT_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
bool Container_Flat_Queue_int_Empty(const Container_Flat_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `const FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |

---

### Return value

`bool` predicate.

---

### Remarks

Returns true when the collection is empty.

---

### Example

```c
bool empty = FLAT_QUEUE_FUNC(int, Empty)(&queue);
```

---

# Flat Queue Size

Returns the element count.

### Syntax

#### Macro form

```c
size_t FLAT_QUEUE_FUNC(int, Size)(const FLAT_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
size_t Container_Flat_Queue_int_Size(const Container_Flat_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `const FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |

---

### Return value

`size_t` count or capacity.

---

### Remarks

Returns the element count.

---

### Example

```c
size_t count = FLAT_QUEUE_FUNC(int, Size)(&queue);
```

---

# Flat Queue Capacity

Returns the currently allocated element capacity.

### Syntax

#### Macro form

```c
size_t FLAT_QUEUE_FUNC(int, Capacity)(const FLAT_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
size_t Container_Flat_Queue_int_Capacity(const Container_Flat_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `const FLAT_QUEUE_TYPE(int) * queue` | Pointer argument; see the operation's contract. |

---

### Return value

`size_t` count or capacity.

---

### Remarks

Returns the currently allocated element capacity.

---

### Example

```c
size_t count = FLAT_QUEUE_FUNC(int, Capacity)(&queue);
```

---

# Flat Stack package

Header: `Cosmeron/Modules/Container/Array/Stack.h`.

LIFO contiguous stack; growth and resizing may invalidate pointers returned by Top.

### Instantiation

```c
FLAT_STACK_TYPE(int) stack = {0};
FLAT_STACK_FUNC(int, Init)(&stack);
```

Functions in this section use the `int` specialization. Built-in variants are listed in the Overview. The short examples assume the object has been initialized and output variables have the matching C type.

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#flat-stack-init) | Initializes the container. |
| [`Destroy`](#flat-stack-destroy) | Releases the container-owned storage. |
| [`Reserve`](#flat-stack-reserve) | Reserves storage for a requested capacity. |
| [`ShrinkToFit`](#flat-stack-shrinktofit) | Attempts to reduce unused reserved storage. |
| [`Push`](#flat-stack-push) | Enqueues/pushes a value. |
| [`Pop`](#flat-stack-pop) | Dequeues/pops and reports a value. |
| [`Top`](#flat-stack-top) | Returns a pointer to the last pushed item. |
| [`Empty`](#flat-stack-empty) | Returns true when the collection is empty. |
| [`Size`](#flat-stack-size) | Returns the element count. |
| [`Capacity`](#flat-stack-capacity) | Returns the currently allocated element capacity. |

---

# Flat Stack Init

Initializes the container.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_STACK_FUNC(int, Init)(FLAT_STACK_TYPE(int) * stack);
```

#### Direct form

```c
OPSTATUS Container_Flat_Stack_int_Init(Container_Flat_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `FLAT_STACK_TYPE(int) * stack` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Initializes the container.

---

### Example

```c
OPSTATUS status = FLAT_STACK_FUNC(int, Init)(&stack);
```

---

# Flat Stack Destroy

Releases the container-owned storage.

### Syntax

#### Macro form

```c
void FLAT_STACK_FUNC(int, Destroy)(FLAT_STACK_TYPE(int) * stack);
```

#### Direct form

```c
void Container_Flat_Stack_int_Destroy(Container_Flat_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `FLAT_STACK_TYPE(int) * stack` | Pointer argument; see the operation's contract. |

---

### Return value

None.

---

### Remarks

Releases the container-owned storage.

---

### Example

```c
FLAT_STACK_FUNC(int, Destroy)(&stack);
```

---

# Flat Stack Reserve

Reserves storage for a requested capacity.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_STACK_FUNC(int, Reserve)(FLAT_STACK_TYPE(int) * stack, size_t newCapacity);
```

#### Direct form

```c
OPSTATUS Container_Flat_Stack_int_Reserve(Container_Flat_TStack_int * stack, size_t newCapacity);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `FLAT_STACK_TYPE(int) * stack` | Pointer argument; see the operation's contract. |
| `newCapacity` | `size_t newCapacity` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Reserves storage for a requested capacity. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = FLAT_STACK_FUNC(int, Reserve)(&stack, 16);
```

---

# Flat Stack ShrinkToFit

Attempts to reduce unused reserved storage.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_STACK_FUNC(int, ShrinkToFit)(FLAT_STACK_TYPE(int) * stack);
```

#### Direct form

```c
OPSTATUS Container_Flat_Stack_int_ShrinkToFit(Container_Flat_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `FLAT_STACK_TYPE(int) * stack` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Attempts to reduce unused reserved storage. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = FLAT_STACK_FUNC(int, ShrinkToFit)(&stack);
```

---

# Flat Stack Push

Enqueues/pushes a value.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_STACK_FUNC(int, Push)(FLAT_STACK_TYPE(int) * stack, int value);
```

#### Direct form

```c
OPSTATUS Container_Flat_Stack_int_Push(Container_Flat_TStack_int * stack, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `FLAT_STACK_TYPE(int) * stack` | Pointer argument; see the operation's contract. |
| `value` | `int value` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Enqueues/pushes a value.

---

### Example

```c
OPSTATUS status = FLAT_STACK_FUNC(int, Push)(&stack, 42);
```

---

# Flat Stack Pop

Dequeues/pops and reports a value.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_STACK_FUNC(int, Pop)(FLAT_STACK_TYPE(int) * stack, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Flat_Stack_int_Pop(Container_Flat_TStack_int * stack, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `FLAT_STACK_TYPE(int) * stack` | Pointer argument; see the operation's contract. |
| `outValue` | `int * outValue` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Dequeues/pops and reports a value.

---

### Example

```c
int removed = 0;
OPSTATUS status = FLAT_STACK_FUNC(int, Pop)(&stack, &removed);
```

---

# Flat Stack Top

Returns a pointer to the last pushed item.

### Syntax

#### Macro form

```c
OPSTATUS FLAT_STACK_FUNC(int, Top)(FLAT_STACK_TYPE(int) * stack, int **out);
```

#### Direct form

```c
OPSTATUS Container_Flat_Stack_int_Top(Container_Flat_TStack_int * stack, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `FLAT_STACK_TYPE(int) * stack` | Pointer argument; see the operation's contract. |
| `out` | `int **out` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns a pointer to the last pushed item. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
int *found = NULL;
OPSTATUS status = FLAT_STACK_FUNC(int, Top)(&stack, &found);
```

---

# Flat Stack Empty

Returns true when the collection is empty.

### Syntax

#### Macro form

```c
bool FLAT_STACK_FUNC(int, Empty)(const FLAT_STACK_TYPE(int) * stack);
```

#### Direct form

```c
bool Container_Flat_Stack_int_Empty(const Container_Flat_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `const FLAT_STACK_TYPE(int) * stack` | Pointer argument; see the operation's contract. |

---

### Return value

`bool` predicate.

---

### Remarks

Returns true when the collection is empty.

---

### Example

```c
bool empty = FLAT_STACK_FUNC(int, Empty)(&stack);
```

---

# Flat Stack Size

Returns the element count.

### Syntax

#### Macro form

```c
size_t FLAT_STACK_FUNC(int, Size)(const FLAT_STACK_TYPE(int) * stack);
```

#### Direct form

```c
size_t Container_Flat_Stack_int_Size(const Container_Flat_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `const FLAT_STACK_TYPE(int) * stack` | Pointer argument; see the operation's contract. |

---

### Return value

`size_t` count or capacity.

---

### Remarks

Returns the element count.

---

### Example

```c
size_t count = FLAT_STACK_FUNC(int, Size)(&stack);
```

---

# Flat Stack Capacity

Returns the currently allocated element capacity.

### Syntax

#### Macro form

```c
size_t FLAT_STACK_FUNC(int, Capacity)(const FLAT_STACK_TYPE(int) * stack);
```

#### Direct form

```c
size_t Container_Flat_Stack_int_Capacity(const Container_Flat_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `const FLAT_STACK_TYPE(int) * stack` | Pointer argument; see the operation's contract. |

---

### Return value

`size_t` count or capacity.

---

### Remarks

Returns the currently allocated element capacity.

---

### Example

```c
size_t count = FLAT_STACK_FUNC(int, Capacity)(&stack);
```

---

# TString package

Header: `Cosmeron/Modules/Container/Array/String.h`.

Zero-terminated code units; suffix 8/16/32 specializes uint8_t/uint16_t/uint32_t, not Unicode validation or grapheme awareness.

### Instantiation

```c
TSTRING_TYPE(8) str = {0};
TSTRING_FUNC(8, Init)(&str);
```

Functions in this section use the `8` specialization. Built-in variants are listed in the Overview. The short examples assume the object has been initialized and output variables have the matching C type.

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#tstring-init) | Initializes the container. |
| [`Destroy`](#tstring-destroy) | Releases the container-owned storage. |
| [`FromCStr`](#tstring-fromcstr) | Copies a zero-terminated code-unit string into owned storage. |
| [`CStr`](#tstring-cstr) | Provides read-only access to a terminated code-unit array. |
| [`Length`](#tstring-length) | Returns the number of string code units, excluding NUL. |
| [`Empty`](#tstring-empty) | Returns true when the collection is empty. |
| [`Clear`](#tstring-clear) | Removes elements without destroying the object. |
| [`Reserve`](#tstring-reserve) | Reserves storage for a requested capacity. |
| [`PushBack`](#tstring-pushback) | Appends a value. |
| [`PopBack`](#tstring-popback) | Removes and optionally reports the last value. |
| [`Capacity`](#tstring-capacity) | Returns the currently allocated element capacity. |
| [`InsertChar`](#tstring-insertchar) | Inserts one code unit at an index. |
| [`At`](#tstring-at) | Returns an element pointer at an index. |
| [`Front`](#tstring-front) | Returns a pointer to the first element. |
| [`Back`](#tstring-back) | Returns a pointer to the last element. |
| [`Append`](#tstring-append) | Appends a zero-terminated code-unit sequence. |
| [`AppendStr`](#tstring-appendstr) | Appends another same-width TString. |
| [`Insert`](#tstring-insert) | Inserts an element or string at the requested index. |
| [`Erase`](#tstring-erase) | Removes an indexed range. |
| [`Substr`](#tstring-substr) | Copies a selected range into another TString. |
| [`Compare`](#tstring-compare) | Writes a CMPOUT lexical comparison via an output pointer. |
| [`Find`](#tstring-find) | Searches for a code unit starting at a specified index. |
| [`FindStr`](#tstring-findstr) | Searches for a zero-terminated substring. |
| [`Data`](#tstring-data) | Returns a pointer to internal contiguous data. |

---

# TString Init

Initializes the container.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Init)(TSTRING_TYPE(8) * str);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Init(Container_Flat_TString_8 * str);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Initializes the container.

---

### Example

```c
OPSTATUS status = TSTRING_FUNC(8, Init)(&str);
```

---

# TString Destroy

Releases the container-owned storage.

### Syntax

#### Macro form

```c
void TSTRING_FUNC(8, Destroy)(TSTRING_TYPE(8) * str);
```

#### Direct form

```c
void Container_Flat_String_8_Destroy(Container_Flat_TString_8 * str);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |

---

### Return value

None.

---

### Remarks

Releases the container-owned storage.

---

### Example

```c
TSTRING_FUNC(8, Destroy)(&str);
```

---

# TString FromCStr

Copies a zero-terminated code-unit string into owned storage.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, FromCStr)(TSTRING_TYPE(8) * str, const uint8_t *cstr);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_FromCStr(Container_Flat_TString_8 * str, const uint8_t *cstr);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `cstr` | `const uint8_t *cstr` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Copies a zero-terminated code-unit string into owned storage. Input must be terminated and have matching code-unit width; the library does not validate encoding.

---

### Example

```c
const uint8_t letters[] = {'a', 'b', 0};
OPSTATUS status = TSTRING_FUNC(8, FromCStr)(&str, letters);
```

---

# TString CStr

Provides read-only access to a terminated code-unit array.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, CStr)(TSTRING_TYPE(8) *str, const uint8_t **outCStr);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_CStr(Container_Flat_TString_8 *str, const uint8_t **outCStr);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) *str` | Pointer argument; see the operation's contract. |
| `outCStr` | `const uint8_t **outCStr` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Provides read-only access to a terminated code-unit array. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
const uint8_t *found = NULL;
OPSTATUS status = TSTRING_FUNC(8, CStr)(&str, &found);
```

---

# TString Length

Returns the number of string code units, excluding NUL.

### Syntax

#### Macro form

```c
size_t TSTRING_FUNC(8, Length)(const TSTRING_TYPE(8) *str);
```

#### Direct form

```c
size_t Container_Flat_String_8_Length(const Container_Flat_TString_8 *str);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `const TSTRING_TYPE(8) *str` | Pointer argument; see the operation's contract. |

---

### Return value

`size_t` count or capacity.

---

### Remarks

Returns the number of string code units, excluding NUL.

---

### Example

```c
size_t count = TSTRING_FUNC(8, Length)(&str);
```

---

# TString Empty

Returns true when the collection is empty.

### Syntax

#### Macro form

```c
bool TSTRING_FUNC(8, Empty)(const TSTRING_TYPE(8) *str);
```

#### Direct form

```c
bool Container_Flat_String_8_Empty(const Container_Flat_TString_8 *str);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `const TSTRING_TYPE(8) *str` | Pointer argument; see the operation's contract. |

---

### Return value

`bool` predicate.

---

### Remarks

Returns true when the collection is empty.

---

### Example

```c
bool empty = TSTRING_FUNC(8, Empty)(&str);
```

---

# TString Clear

Removes elements without destroying the object.

### Syntax

#### Macro form

```c
void TSTRING_FUNC(8, Clear)(TSTRING_TYPE(8) * str);
```

#### Direct form

```c
void Container_Flat_String_8_Clear(Container_Flat_TString_8 * str);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |

---

### Return value

None.

---

### Remarks

Removes elements without destroying the object.

---

### Example

```c
TSTRING_FUNC(8, Clear)(&str);
```

---

# TString Reserve

Reserves storage for a requested capacity.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Reserve)(TSTRING_TYPE(8) * str, size_t newCapacity);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Reserve(Container_Flat_TString_8 * str, size_t newCapacity);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `newCapacity` | `size_t newCapacity` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Reserves storage for a requested capacity. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = TSTRING_FUNC(8, Reserve)(&str, 16);
```

---

# TString PushBack

Appends a value.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, PushBack)(TSTRING_TYPE(8) * str, uint8_t ch);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_PushBack(Container_Flat_TString_8 * str, uint8_t ch);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `ch` | `uint8_t ch` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Appends a value. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = TSTRING_FUNC(8, PushBack)(&str, 'A');
```

---

# TString PopBack

Removes and optionally reports the last value.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, PopBack)(TSTRING_TYPE(8) * str, uint8_t *outValue);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_PopBack(Container_Flat_TString_8 * str, uint8_t *outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `outValue` | `uint8_t *outValue` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Removes and optionally reports the last value.

---

### Example

```c
uint8_t removed = 0;
OPSTATUS status = TSTRING_FUNC(8, PopBack)(&str, &removed);
```

---

# TString Capacity

Returns the currently allocated element capacity.

### Syntax

#### Macro form

```c
size_t TSTRING_FUNC(8, Capacity)(const TSTRING_TYPE(8) *str);
```

#### Direct form

```c
size_t Container_Flat_String_8_Capacity(const Container_Flat_TString_8 *str);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `const TSTRING_TYPE(8) *str` | Pointer argument; see the operation's contract. |

---

### Return value

`size_t` count or capacity.

---

### Remarks

Returns the currently allocated element capacity.

---

### Example

```c
size_t count = TSTRING_FUNC(8, Capacity)(&str);
```

---

# TString InsertChar

Inserts one code unit at an index.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, InsertChar)(TSTRING_TYPE(8) * str, size_t pos, uint8_t ch);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_InsertChar(Container_Flat_TString_8 * str, size_t pos, uint8_t ch);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `pos` | `size_t pos` | Scalar argument or value. |
| `ch` | `uint8_t ch` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Inserts one code unit at an index.

---

### Example

```c
OPSTATUS status = TSTRING_FUNC(8, InsertChar)(&str, 0, 'A');
```

---

# TString At

Returns an element pointer at an index.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, At)(TSTRING_TYPE(8) * str, size_t index, uint8_t **out);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_At(Container_Flat_TString_8 * str, size_t index, uint8_t **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `index` | `size_t index` | Scalar argument or value. |
| `out` | `uint8_t **out` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns an element pointer at an index. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
uint8_t *found = NULL;
OPSTATUS status = TSTRING_FUNC(8, At)(&str, 0, &found);
```

---

# TString Front

Returns a pointer to the first element.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Front)(TSTRING_TYPE(8) * str, uint8_t **out);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Front(Container_Flat_TString_8 * str, uint8_t **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `out` | `uint8_t **out` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns a pointer to the first element. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
uint8_t *found = NULL;
OPSTATUS status = TSTRING_FUNC(8, Front)(&str, &found);
```

---

# TString Back

Returns a pointer to the last element.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Back)(TSTRING_TYPE(8) * str, uint8_t **out);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Back(Container_Flat_TString_8 * str, uint8_t **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `out` | `uint8_t **out` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns a pointer to the last element. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
uint8_t *found = NULL;
OPSTATUS status = TSTRING_FUNC(8, Back)(&str, &found);
```

---

# TString Append

Appends a zero-terminated code-unit sequence.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Append)(TSTRING_TYPE(8) * str, const uint8_t *cstr);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Append(Container_Flat_TString_8 * str, const uint8_t *cstr);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `cstr` | `const uint8_t *cstr` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Appends a zero-terminated code-unit sequence. Input must be terminated and have matching code-unit width; the library does not validate encoding.

---

### Example

```c
const uint8_t letters[] = {'a', 'b', 0};
OPSTATUS status = TSTRING_FUNC(8, Append)(&str, letters);
```

---

# TString AppendStr

Appends another same-width TString.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, AppendStr)(TSTRING_TYPE(8) * str, TSTRING_TYPE(8) * other);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_AppendStr(Container_Flat_TString_8 * str, Container_Flat_TString_8 * other);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `other` | `TSTRING_TYPE(8) * other` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Appends another same-width TString.

---

### Example

```c
/* other: separately initialized TString(8) */
OPSTATUS status = TSTRING_FUNC(8, AppendStr)(&str, &other);
```

---

# TString Insert

Inserts an element or string at the requested index.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Insert)(TSTRING_TYPE(8) * str, size_t pos, const uint8_t *cstr);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Insert(Container_Flat_TString_8 * str, size_t pos, const uint8_t *cstr);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `pos` | `size_t pos` | Scalar argument or value. |
| `cstr` | `const uint8_t *cstr` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Inserts an element or string at the requested index. Array-backed operations can move or reallocate values; avoid retaining old pointers. Input must be terminated and have matching code-unit width; the library does not validate encoding.

---

### Example

```c
const uint8_t letters[] = {'a', 'b', 0};
OPSTATUS status = TSTRING_FUNC(8, Insert)(&str, 0, letters);
```

---

# TString Erase

Removes an indexed range.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Erase)(TSTRING_TYPE(8) * str, size_t pos, size_t count);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Erase(Container_Flat_TString_8 * str, size_t pos, size_t count);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `pos` | `size_t pos` | Scalar argument or value. |
| `count` | `size_t count` | Scalar argument or value. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Removes an indexed range. Array-backed operations can move or reallocate values; avoid retaining old pointers.

---

### Example

```c
OPSTATUS status = TSTRING_FUNC(8, Erase)(&str, 0, 1);
```

---

# TString Substr

Copies a selected range into another TString.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Substr)(TSTRING_TYPE(8) * str, size_t pos, size_t count, TSTRING_TYPE(8) * out);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Substr(Container_Flat_TString_8 * str, size_t pos, size_t count, Container_Flat_TString_8 * out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) * str` | Pointer argument; see the operation's contract. |
| `pos` | `size_t pos` | Scalar argument or value. |
| `count` | `size_t count` | Scalar argument or value. |
| `out` | `TSTRING_TYPE(8) * out` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Copies a selected range into another TString.

---

### Example

```c
uint8_t *found = NULL;
OPSTATUS status = TSTRING_FUNC(8, Substr)(&str, 0, 1, &found);
```

---

# TString Compare

Writes a CMPOUT lexical comparison via an output pointer.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Compare)(const TSTRING_TYPE(8) *str, const TSTRING_TYPE(8) *other, CMPOUT *outResult);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Compare(const Container_Flat_TString_8 *str, const Container_Flat_TString_8 *other, CMPOUT *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `const TSTRING_TYPE(8) *str` | Pointer argument; see the operation's contract. |
| `other` | `const TSTRING_TYPE(8) *other` | Pointer argument; see the operation's contract. |
| `outResult` | `CMPOUT *outResult` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Writes a CMPOUT lexical comparison via an output pointer. The function returns OPSTATUS; the ordering is written through the CMPOUT output.

---

### Example

```c
/* other: separately initialized TString(8) */
CMPOUT comparison;
OPSTATUS status = TSTRING_FUNC(8, Compare)(&str, &other, &comparison);
```

---

# TString Find

Searches for a code unit starting at a specified index.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Find)(const TSTRING_TYPE(8) *str, uint8_t ch, size_t start, size_t *index);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Find(const Container_Flat_TString_8 *str, uint8_t ch, size_t start, size_t *index);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `const TSTRING_TYPE(8) *str` | Pointer argument; see the operation's contract. |
| `ch` | `uint8_t ch` | Scalar argument or value. |
| `start` | `size_t start` | Scalar argument or value. |
| `index` | `size_t *index` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Searches for a code unit starting at a specified index.

---

### Example

```c
OPSTATUS status = TSTRING_FUNC(8, Find)(&str, 'A', 0, 0);
```

---

# TString FindStr

Searches for a zero-terminated substring.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, FindStr)(const TSTRING_TYPE(8) *str, const uint8_t *needle, size_t start, size_t *index);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_FindStr(const Container_Flat_TString_8 *str, const uint8_t *needle, size_t start, size_t *index);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `const TSTRING_TYPE(8) *str` | Pointer argument; see the operation's contract. |
| `needle` | `const uint8_t *needle` | Pointer argument; see the operation's contract. |
| `start` | `size_t start` | Scalar argument or value. |
| `index` | `size_t *index` | Pointer argument; see the operation's contract. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Searches for a zero-terminated substring. Input must be terminated and have matching code-unit width; the library does not validate encoding.

---

### Example

```c
const uint8_t letters[] = {'a', 'b', 0};
OPSTATUS status = TSTRING_FUNC(8, FindStr)(&str, letters, 0, 0);
```

---

# TString Data

Returns a pointer to internal contiguous data.

### Syntax

#### Macro form

```c
OPSTATUS TSTRING_FUNC(8, Data)(TSTRING_TYPE(8) *str, uint8_t **outData);
```

#### Direct form

```c
OPSTATUS Container_Flat_String_8_Data(Container_Flat_TString_8 *str, uint8_t **outData);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `str` | `TSTRING_TYPE(8) *str` | Pointer argument; see the operation's contract. |
| `outData` | `uint8_t **outData` | Output pointer to an element inside the collection. |

---

### Return value

`OPSTATUS` (success or an operation-specific status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `OUT_OF_MEMORY` or `NOT_FOUND`).

---

### Remarks

Returns a pointer to internal contiguous data. Returned pointers borrow container storage and can become invalid after mutation or destruction.

---

### Example

```c
uint8_t *found = NULL;
OPSTATUS status = TSTRING_FUNC(8, Data)(&str, &found);
```

---

# Forward List package

Header: `Cosmeron/Modules/Container/Linked/Forward.h`.

Singly linked list with head/tail and node-oriented InsertAfter/EraseAfter.

Built-in specializations: `int`, `float`, `double`, `char`. The examples below use `int`.

### Instantiation

```c
LINKED_FORWARD_LIST_TYPE(int) container = {0};
LINKED_FORWARD_LIST_FUNC(int, Init)(&container);
```

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#forward-list-init) | Initializes bookkeeping fields to an empty state. |
| [`Destroy`](#forward-list-destroy) | Releases the linked nodes and clears the container. |
| [`PushFront`](#forward-list-pushfront) | Inserts a copied value at the front. |
| [`PopFront`](#forward-list-popfront) | Removes the first value and reports it. |
| [`InsertAfter`](#forward-list-insertafter) | Inserts a new node after the given node. |
| [`EraseAfter`](#forward-list-eraseafter) | Removes the node immediately following the supplied node. |
| [`Clear`](#forward-list-clear) | Removes all nodes without requiring a new instance. |
| [`Front`](#forward-list-front) | Obtains a pointer to the first node's value. |
| [`Back`](#forward-list-back) | Obtains a pointer to the last node's value. |
| [`Begin`](#forward-list-begin) | Returns a pointer to the first node for iteration. |
| [`Empty`](#forward-list-empty) | Reports whether the collection has no nodes. |
| [`Size`](#forward-list-size) | Returns the stored node count. |

Node pointers obtained from a list become invalid when their node is erased, or when the collection is cleared/destroyed. Never pass node positions taken from another container.

---

# Forward List Init

Initializes bookkeeping fields to an empty state.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_FORWARD_LIST_FUNC(int, Init)( LINKED_FORWARD_LIST_TYPE(int) * container);
```

#### Direct form

```c
OPSTATUS Container_Linked_ForwardList_int_Init( Container_Linked_TForwardList_int * container);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `container` | `LINKED_FORWARD_LIST_TYPE(int) * container` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Initializes bookkeeping fields to an empty state. Calling Init on an already-owning live object would lose node references; destroy it first.

---

### Example

```c
OPSTATUS status = LINKED_FORWARD_LIST_FUNC(int, Init)(&container);
```

---

# Forward List Destroy

Releases the linked nodes and clears the container.

### Syntax

#### Macro form

```c
void LINKED_FORWARD_LIST_FUNC(int, Destroy)( LINKED_FORWARD_LIST_TYPE(int) * container);
```

#### Direct form

```c
void Container_Linked_ForwardList_int_Destroy( Container_Linked_TForwardList_int * container);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `container` | `LINKED_FORWARD_LIST_TYPE(int) * container` | Container/node pointer. |

---

### Return value

None.

---

### Remarks

Releases the linked nodes and clears the container. If values are pointers, independently owned pointees are not automatically destroyed.

---

### Example

```c
LINKED_FORWARD_LIST_FUNC(int, Destroy)(&container);
```

---

# Forward List PushFront

Inserts a copied value at the front.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_FORWARD_LIST_FUNC(int, PushFront)( LINKED_FORWARD_LIST_TYPE(int) * container, int value);
```

#### Direct form

```c
OPSTATUS Container_Linked_ForwardList_int_PushFront( Container_Linked_TForwardList_int * container, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `container` | `LINKED_FORWARD_LIST_TYPE(int) * container` | Container/node pointer. |
| `value` | `int value` | Scalar value or index. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Inserts a copied value at the front.

---

### Example

```c
OPSTATUS status = LINKED_FORWARD_LIST_FUNC(int, PushFront)(&container, 42);
```

---

# Forward List PopFront

Removes the first value and reports it.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_FORWARD_LIST_FUNC(int, PopFront)( LINKED_FORWARD_LIST_TYPE(int) * container, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Linked_ForwardList_int_PopFront( Container_Linked_TForwardList_int * container, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `container` | `LINKED_FORWARD_LIST_TYPE(int) * container` | Container/node pointer. |
| `outValue` | `int * outValue` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Removes the first value and reports it. The removed node ceases to exist and all pointers to it become invalid.

---

### Example

```c
int value = 0;
OPSTATUS status = LINKED_FORWARD_LIST_FUNC(int, PopFront)(&container, &value);
```

---

# Forward List InsertAfter

Inserts a new node after the given node.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_FORWARD_LIST_FUNC(int, InsertAfter)( LINKED_FORWARD_LIST_TYPE(int) * container, LINKED_FORWARD_LIST_NODE_TYPE(int) * pos, int value, LINKED_FORWARD_LIST_NODE_TYPE(int) * *outNode);
```

#### Direct form

```c
OPSTATUS Container_Linked_ForwardList_int_InsertAfter( Container_Linked_TForwardList_int * container, Container_Linked_ForwardList_TNode_int * pos, int value, Container_Linked_ForwardList_TNode_int * *outNode);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `container` | `LINKED_FORWARD_LIST_TYPE(int) * container` | Container/node pointer. |
| `pos` | `LINKED_FORWARD_LIST_NODE_TYPE(int) * pos` | Container/node pointer. |
| `value` | `int value` | Scalar value or index. |
| `outNode` | `LINKED_FORWARD_LIST_NODE_TYPE(int) * *outNode` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Inserts a new node after the given node. The newly allocated node belongs to the container.

---

### Example

```c
LINKED_FORWARD_LIST_NODE_TYPE(int) *position = LINKED_FORWARD_LIST_FUNC(int, Begin)(&container);
LINKED_FORWARD_LIST_NODE_TYPE(int) *inserted = NULL;
OPSTATUS status = LINKED_FORWARD_LIST_FUNC(int, InsertAfter)(&container, position, 42, &inserted);
```

---

# Forward List EraseAfter

Removes the node immediately following the supplied node.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_FORWARD_LIST_FUNC(int, EraseAfter)( LINKED_FORWARD_LIST_TYPE(int) * container, LINKED_FORWARD_LIST_NODE_TYPE(int) * pos);
```

#### Direct form

```c
OPSTATUS Container_Linked_ForwardList_int_EraseAfter( Container_Linked_TForwardList_int * container, Container_Linked_ForwardList_TNode_int * pos);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `container` | `LINKED_FORWARD_LIST_TYPE(int) * container` | Container/node pointer. |
| `pos` | `LINKED_FORWARD_LIST_NODE_TYPE(int) * pos` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Removes the node immediately following the supplied node. The removed node ceases to exist and all pointers to it become invalid.

---

### Example

```c
LINKED_FORWARD_LIST_NODE_TYPE(int) *position = LINKED_FORWARD_LIST_FUNC(int, Begin)(&container);
OPSTATUS status = LINKED_FORWARD_LIST_FUNC(int, EraseAfter)(&container, position);
```

---

# Forward List Clear

Removes all nodes without requiring a new instance.

### Syntax

#### Macro form

```c
void LINKED_FORWARD_LIST_FUNC(int, Clear)( LINKED_FORWARD_LIST_TYPE(int) * container);
```

#### Direct form

```c
void Container_Linked_ForwardList_int_Clear( Container_Linked_TForwardList_int * container);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `container` | `LINKED_FORWARD_LIST_TYPE(int) * container` | Container/node pointer. |

---

### Return value

None.

---

### Remarks

Removes all nodes without requiring a new instance. If values are pointers, independently owned pointees are not automatically destroyed.

---

### Example

```c
LINKED_FORWARD_LIST_FUNC(int, Clear)(&container);
```

---

# Forward List Front

Obtains a pointer to the first node's value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_FORWARD_LIST_FUNC(int, Front)( LINKED_FORWARD_LIST_TYPE(int) * list, int **out);
```

#### Direct form

```c
OPSTATUS Container_Linked_ForwardList_int_Front( Container_Linked_TForwardList_int * list, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_FORWARD_LIST_TYPE(int) * list` | Container/node pointer. |
| `out` | `int **out` | Typed pointer to an output. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Obtains a pointer to the first node's value.

---

### Example

```c
int *element = NULL;
OPSTATUS status = LINKED_FORWARD_LIST_FUNC(int, Front)(&list, &element);
```

---

# Forward List Back

Obtains a pointer to the last node's value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_FORWARD_LIST_FUNC(int, Back)( LINKED_FORWARD_LIST_TYPE(int) * list, int **out);
```

#### Direct form

```c
OPSTATUS Container_Linked_ForwardList_int_Back( Container_Linked_TForwardList_int * list, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_FORWARD_LIST_TYPE(int) * list` | Container/node pointer. |
| `out` | `int **out` | Typed pointer to an output. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Obtains a pointer to the last node's value.

---

### Example

```c
int *element = NULL;
OPSTATUS status = LINKED_FORWARD_LIST_FUNC(int, Back)(&list, &element);
```

---

# Forward List Begin

Returns a pointer to the first node for iteration.

### Syntax

#### Macro form

```c
LINKED_FORWARD_LIST_NODE_TYPE(int) * LINKED_FORWARD_LIST_FUNC(int, Begin)( LINKED_FORWARD_LIST_TYPE(int) * container);
```

#### Direct form

```c
Container_Linked_ForwardList_TNode_int * Container_Linked_ForwardList_int_Begin( Container_Linked_TForwardList_int * container);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `container` | `LINKED_FORWARD_LIST_TYPE(int) * container` | Container/node pointer. |

---

### Return value

Pointer to node or NULL when appropriate.

---

### Remarks

Returns a pointer to the first node for iteration.

---

### Example

```c
LINKED_FORWARD_LIST_NODE_TYPE(int) *node = LINKED_FORWARD_LIST_FUNC(int, Begin)(&container);
```

---

# Forward List Empty

Reports whether the collection has no nodes.

### Syntax

#### Macro form

```c
bool LINKED_FORWARD_LIST_FUNC(int, Empty)( const LINKED_FORWARD_LIST_TYPE(int) * container);
```

#### Direct form

```c
bool Container_Linked_ForwardList_int_Empty( const Container_Linked_TForwardList_int * container);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `container` | `const LINKED_FORWARD_LIST_TYPE(int) * container` | Container/node pointer. |

---

### Return value

`bool` emptiness predicate.

---

### Remarks

Reports whether the collection has no nodes.

---

### Example

```c
bool empty = LINKED_FORWARD_LIST_FUNC(int, Empty)(&container);
```

---

# Forward List Size

Returns the stored node count.

### Syntax

#### Macro form

```c
size_t LINKED_FORWARD_LIST_FUNC(int, Size)( const LINKED_FORWARD_LIST_TYPE(int) * container);
```

#### Direct form

```c
size_t Container_Linked_ForwardList_int_Size( const Container_Linked_TForwardList_int * container);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `container` | `const LINKED_FORWARD_LIST_TYPE(int) * container` | Container/node pointer. |

---

### Return value

`size_t` live element count.

---

### Remarks

Returns the stored node count.

---

### Example

```c
size_t size = LINKED_FORWARD_LIST_FUNC(int, Size)(&container);
```

---

# Linked List package

Header: `Cosmeron/Modules/Container/Linked/List.h`.

Doubly linked list with stable node addresses until deletion, and node-oriented Insert/Erase.

Built-in specializations: `int`, `float`, `double`, `char` and an internal `void_ptr`. The examples below use `int`.

### Instantiation

```c
LINKED_LIST_TYPE(int) list = {0};
LINKED_LIST_FUNC(int, Init)(&list);
```

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#linked-list-init) | Initializes bookkeeping fields to an empty state. |
| [`Destroy`](#linked-list-destroy) | Releases the linked nodes and clears the container. |
| [`Front`](#linked-list-front) | Obtains a pointer to the first node's value. |
| [`Back`](#linked-list-back) | Obtains a pointer to the last node's value. |
| [`Begin`](#linked-list-begin) | Returns a pointer to the first node for iteration. |
| [`End`](#linked-list-end) | Returns the past-end sentinel for iteration. |
| [`PushFront`](#linked-list-pushfront) | Inserts a copied value at the front. |
| [`PushBack`](#linked-list-pushback) | Inserts a copied value at the back. |
| [`PopFront`](#linked-list-popfront) | Removes the first value and reports it. |
| [`PopBack`](#linked-list-popback) | Removes the last value and reports it. |
| [`Insert`](#linked-list-insert) | Inserts a node before the specified list position. |
| [`Erase`](#linked-list-erase) | Erases the given node and returns the following node through an out parameter. |
| [`Clear`](#linked-list-clear) | Removes all nodes without requiring a new instance. |
| [`Empty`](#linked-list-empty) | Reports whether the collection has no nodes. |
| [`Size`](#linked-list-size) | Returns the stored node count. |

Node pointers obtained from a list become invalid when their node is erased, or when the collection is cleared/destroyed. Never pass node positions taken from another container.

---

# Linked List Init

Initializes bookkeeping fields to an empty state.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_LIST_FUNC(int, Init)( LINKED_LIST_TYPE(int) * list);
```

#### Direct form

```c
OPSTATUS Container_Linked_List_int_Init( Container_Linked_TList_int * list);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Initializes bookkeeping fields to an empty state. Calling Init on an already-owning live object would lose node references; destroy it first.

---

### Example

```c
OPSTATUS status = LINKED_LIST_FUNC(int, Init)(&list);
```

---

# Linked List Destroy

Releases the linked nodes and clears the container.

### Syntax

#### Macro form

```c
void LINKED_LIST_FUNC(int, Destroy)( LINKED_LIST_TYPE(int) * list);
```

#### Direct form

```c
void Container_Linked_List_int_Destroy( Container_Linked_TList_int * list);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |

---

### Return value

None.

---

### Remarks

Releases the linked nodes and clears the container. If values are pointers, independently owned pointees are not automatically destroyed.

---

### Example

```c
LINKED_LIST_FUNC(int, Destroy)(&list);
```

---

# Linked List Front

Obtains a pointer to the first node's value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_LIST_FUNC(int, Front)( LINKED_LIST_TYPE(int) * list, int **out);
```

#### Direct form

```c
OPSTATUS Container_Linked_List_int_Front( Container_Linked_TList_int * list, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |
| `out` | `int **out` | Typed pointer to an output. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Obtains a pointer to the first node's value.

---

### Example

```c
int *element = NULL;
OPSTATUS status = LINKED_LIST_FUNC(int, Front)(&list, &element);
```

---

# Linked List Back

Obtains a pointer to the last node's value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_LIST_FUNC(int, Back)( LINKED_LIST_TYPE(int) * list, int **out);
```

#### Direct form

```c
OPSTATUS Container_Linked_List_int_Back( Container_Linked_TList_int * list, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |
| `out` | `int **out` | Typed pointer to an output. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Obtains a pointer to the last node's value.

---

### Example

```c
int *element = NULL;
OPSTATUS status = LINKED_LIST_FUNC(int, Back)(&list, &element);
```

---

# Linked List Begin

Returns a pointer to the first node for iteration.

### Syntax

#### Macro form

```c
LINKED_LIST_NODE_TYPE(int) * LINKED_LIST_FUNC(int, Begin)(LINKED_LIST_TYPE(int) * list);
```

#### Direct form

```c
Container_Linked_List_TNode_int * Container_Linked_List_int_Begin(Container_Linked_TList_int * list);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |

---

### Return value

Pointer to node or NULL when appropriate.

---

### Remarks

Returns a pointer to the first node for iteration.

---

### Example

```c
LINKED_LIST_NODE_TYPE(int) *node = LINKED_LIST_FUNC(int, Begin)(&list);
```

---

# Linked List End

Returns the past-end sentinel for iteration.

### Syntax

#### Macro form

```c
LINKED_LIST_NODE_TYPE(int) * LINKED_LIST_FUNC(int, End)(LINKED_LIST_TYPE(int) * list);
```

#### Direct form

```c
Container_Linked_List_TNode_int * Container_Linked_List_int_End(Container_Linked_TList_int * list);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |

---

### Return value

Pointer to node or NULL when appropriate.

---

### Remarks

Returns the past-end sentinel for iteration.

---

### Example

```c
LINKED_LIST_NODE_TYPE(int) *end = LINKED_LIST_FUNC(int, End)(&list); /* End is the past-end position. */
```

---

# Linked List PushFront

Inserts a copied value at the front.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_LIST_FUNC(int, PushFront)( LINKED_LIST_TYPE(int) * list, int value);
```

#### Direct form

```c
OPSTATUS Container_Linked_List_int_PushFront( Container_Linked_TList_int * list, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |
| `value` | `int value` | Scalar value or index. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Inserts a copied value at the front.

---

### Example

```c
OPSTATUS status = LINKED_LIST_FUNC(int, PushFront)(&list, 42);
```

---

# Linked List PushBack

Inserts a copied value at the back.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_LIST_FUNC(int, PushBack)( LINKED_LIST_TYPE(int) * list, int value);
```

#### Direct form

```c
OPSTATUS Container_Linked_List_int_PushBack( Container_Linked_TList_int * list, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |
| `value` | `int value` | Scalar value or index. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Inserts a copied value at the back.

---

### Example

```c
OPSTATUS status = LINKED_LIST_FUNC(int, PushBack)(&list, 42);
```

---

# Linked List PopFront

Removes the first value and reports it.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_LIST_FUNC(int, PopFront)( LINKED_LIST_TYPE(int) * list, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Linked_List_int_PopFront( Container_Linked_TList_int * list, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |
| `outValue` | `int * outValue` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Removes the first value and reports it. The removed node ceases to exist and all pointers to it become invalid.

---

### Example

```c
int value = 0;
OPSTATUS status = LINKED_LIST_FUNC(int, PopFront)(&list, &value);
```

---

# Linked List PopBack

Removes the last value and reports it.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_LIST_FUNC(int, PopBack)( LINKED_LIST_TYPE(int) * list, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Linked_List_int_PopBack( Container_Linked_TList_int * list, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |
| `outValue` | `int * outValue` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Removes the last value and reports it. The removed node ceases to exist and all pointers to it become invalid.

---

### Example

```c
int value = 0;
OPSTATUS status = LINKED_LIST_FUNC(int, PopBack)(&list, &value);
```

---

# Linked List Insert

Inserts a node before the specified list position.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_LIST_FUNC(int, Insert)( LINKED_LIST_TYPE(int) * list, LINKED_LIST_NODE_TYPE(int) * pos, int value, LINKED_LIST_NODE_TYPE(int) * *outNode);
```

#### Direct form

```c
OPSTATUS Container_Linked_List_int_Insert( Container_Linked_TList_int * list, Container_Linked_List_TNode_int * pos, int value, Container_Linked_List_TNode_int * *outNode);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |
| `pos` | `LINKED_LIST_NODE_TYPE(int) * pos` | Container/node pointer. |
| `value` | `int value` | Scalar value or index. |
| `outNode` | `LINKED_LIST_NODE_TYPE(int) * *outNode` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Inserts a node before the specified list position. The newly allocated node belongs to the container.

---

### Example

```c
LINKED_LIST_NODE_TYPE(int) *position = LINKED_LIST_FUNC(int, Begin)(&list);
LINKED_LIST_NODE_TYPE(int) *inserted = NULL;
OPSTATUS status = LINKED_LIST_FUNC(int, Insert)(&list, position, 42, &inserted);
```

---

# Linked List Erase

Erases the given node and returns the following node through an out parameter.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_LIST_FUNC(int, Erase)( LINKED_LIST_TYPE(int) * list, LINKED_LIST_NODE_TYPE(int) * pos, LINKED_LIST_NODE_TYPE(int) * *outNode);
```

#### Direct form

```c
OPSTATUS Container_Linked_List_int_Erase( Container_Linked_TList_int * list, Container_Linked_List_TNode_int * pos, Container_Linked_List_TNode_int * *outNode);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |
| `pos` | `LINKED_LIST_NODE_TYPE(int) * pos` | Container/node pointer. |
| `outNode` | `LINKED_LIST_NODE_TYPE(int) * *outNode` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Erases the given node and returns the following node through an out parameter. The removed node ceases to exist and all pointers to it become invalid.

---

### Example

```c
LINKED_LIST_NODE_TYPE(int) *position = LINKED_LIST_FUNC(int, Begin)(&list);
LINKED_LIST_NODE_TYPE(int) *inserted = NULL;
OPSTATUS status = LINKED_LIST_FUNC(int, Erase)(&list, position, &inserted);
```

---

# Linked List Clear

Removes all nodes without requiring a new instance.

### Syntax

#### Macro form

```c
void LINKED_LIST_FUNC(int, Clear)(LINKED_LIST_TYPE(int) * list);
```

#### Direct form

```c
void Container_Linked_List_int_Clear(Container_Linked_TList_int * list);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `LINKED_LIST_TYPE(int) * list` | Container/node pointer. |

---

### Return value

None.

---

### Remarks

Removes all nodes without requiring a new instance. If values are pointers, independently owned pointees are not automatically destroyed.

---

### Example

```c
LINKED_LIST_FUNC(int, Clear)(&list);
```

---

# Linked List Empty

Reports whether the collection has no nodes.

### Syntax

#### Macro form

```c
bool LINKED_LIST_FUNC(int, Empty)(const LINKED_LIST_TYPE(int) * list);
```

#### Direct form

```c
bool Container_Linked_List_int_Empty(const Container_Linked_TList_int * list);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `const LINKED_LIST_TYPE(int) * list` | Container/node pointer. |

---

### Return value

`bool` emptiness predicate.

---

### Remarks

Reports whether the collection has no nodes.

---

### Example

```c
bool empty = LINKED_LIST_FUNC(int, Empty)(&list);
```

---

# Linked List Size

Returns the stored node count.

### Syntax

#### Macro form

```c
size_t LINKED_LIST_FUNC(int, Size)(const LINKED_LIST_TYPE(int) * list);
```

#### Direct form

```c
size_t Container_Linked_List_int_Size(const Container_Linked_TList_int * list);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `list` | `const LINKED_LIST_TYPE(int) * list` | Container/node pointer. |

---

### Return value

`size_t` live element count.

---

### Remarks

Returns the stored node count.

---

### Example

```c
size_t size = LINKED_LIST_FUNC(int, Size)(&list);
```

---

# Linked Deque package

Header: `Cosmeron/Modules/Container/Linked/Deque.h`.

Doubly linked FIFO/LIFO operations from both ends.

Built-in specializations: `int`, `float`, `double`, `char`. The examples below use `int`.

### Instantiation

```c
LINKED_DEQUE_TYPE(int) deque = {0};
LINKED_DEQUE_FUNC(int, Init)(&deque);
```

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#linked-deque-init) | Initializes bookkeeping fields to an empty state. |
| [`Destroy`](#linked-deque-destroy) | Releases the linked nodes and clears the container. |
| [`PushFront`](#linked-deque-pushfront) | Inserts a copied value at the front. |
| [`PushBack`](#linked-deque-pushback) | Inserts a copied value at the back. |
| [`PopFront`](#linked-deque-popfront) | Removes the first value and reports it. |
| [`PopBack`](#linked-deque-popback) | Removes the last value and reports it. |
| [`Clear`](#linked-deque-clear) | Removes all nodes without requiring a new instance. |
| [`Front`](#linked-deque-front) | Obtains a pointer to the first node's value. |
| [`Back`](#linked-deque-back) | Obtains a pointer to the last node's value. |
| [`Empty`](#linked-deque-empty) | Reports whether the collection has no nodes. |
| [`Size`](#linked-deque-size) | Returns the stored node count. |

Node pointers obtained from a list become invalid when their node is erased, or when the collection is cleared/destroyed. Never pass node positions taken from another container.

---

# Linked Deque Init

Initializes bookkeeping fields to an empty state.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_DEQUE_FUNC(int, Init)( LINKED_DEQUE_TYPE(int) * deque);
```

#### Direct form

```c
OPSTATUS Container_Linked_Deque_int_Init( Container_Linked_TDeque_int * deque);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Initializes bookkeeping fields to an empty state. Calling Init on an already-owning live object would lose node references; destroy it first.

---

### Example

```c
OPSTATUS status = LINKED_DEQUE_FUNC(int, Init)(&deque);
```

---

# Linked Deque Destroy

Releases the linked nodes and clears the container.

### Syntax

#### Macro form

```c
void LINKED_DEQUE_FUNC(int, Destroy)( LINKED_DEQUE_TYPE(int) * deque);
```

#### Direct form

```c
void Container_Linked_Deque_int_Destroy( Container_Linked_TDeque_int * deque);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |

---

### Return value

None.

---

### Remarks

Releases the linked nodes and clears the container. If values are pointers, independently owned pointees are not automatically destroyed.

---

### Example

```c
LINKED_DEQUE_FUNC(int, Destroy)(&deque);
```

---

# Linked Deque PushFront

Inserts a copied value at the front.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_DEQUE_FUNC(int, PushFront)( LINKED_DEQUE_TYPE(int) * deque, int value);
```

#### Direct form

```c
OPSTATUS Container_Linked_Deque_int_PushFront( Container_Linked_TDeque_int * deque, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |
| `value` | `int value` | Scalar value or index. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Inserts a copied value at the front.

---

### Example

```c
OPSTATUS status = LINKED_DEQUE_FUNC(int, PushFront)(&deque, 42);
```

---

# Linked Deque PushBack

Inserts a copied value at the back.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_DEQUE_FUNC(int, PushBack)( LINKED_DEQUE_TYPE(int) * deque, int value);
```

#### Direct form

```c
OPSTATUS Container_Linked_Deque_int_PushBack( Container_Linked_TDeque_int * deque, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |
| `value` | `int value` | Scalar value or index. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Inserts a copied value at the back.

---

### Example

```c
OPSTATUS status = LINKED_DEQUE_FUNC(int, PushBack)(&deque, 42);
```

---

# Linked Deque PopFront

Removes the first value and reports it.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_DEQUE_FUNC(int, PopFront)( LINKED_DEQUE_TYPE(int) * deque, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Linked_Deque_int_PopFront( Container_Linked_TDeque_int * deque, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |
| `outValue` | `int * outValue` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Removes the first value and reports it. The removed node ceases to exist and all pointers to it become invalid.

---

### Example

```c
int value = 0;
OPSTATUS status = LINKED_DEQUE_FUNC(int, PopFront)(&deque, &value);
```

---

# Linked Deque PopBack

Removes the last value and reports it.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_DEQUE_FUNC(int, PopBack)( LINKED_DEQUE_TYPE(int) * deque, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Linked_Deque_int_PopBack( Container_Linked_TDeque_int * deque, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |
| `outValue` | `int * outValue` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Removes the last value and reports it. The removed node ceases to exist and all pointers to it become invalid.

---

### Example

```c
int value = 0;
OPSTATUS status = LINKED_DEQUE_FUNC(int, PopBack)(&deque, &value);
```

---

# Linked Deque Clear

Removes all nodes without requiring a new instance.

### Syntax

#### Macro form

```c
void LINKED_DEQUE_FUNC(int, Clear)( LINKED_DEQUE_TYPE(int) * deque);
```

#### Direct form

```c
void Container_Linked_Deque_int_Clear( Container_Linked_TDeque_int * deque);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |

---

### Return value

None.

---

### Remarks

Removes all nodes without requiring a new instance. If values are pointers, independently owned pointees are not automatically destroyed.

---

### Example

```c
LINKED_DEQUE_FUNC(int, Clear)(&deque);
```

---

# Linked Deque Front

Obtains a pointer to the first node's value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_DEQUE_FUNC(int, Front)( LINKED_DEQUE_TYPE(int) * deque, int **out);
```

#### Direct form

```c
OPSTATUS Container_Linked_Deque_int_Front( Container_Linked_TDeque_int * deque, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |
| `out` | `int **out` | Typed pointer to an output. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Obtains a pointer to the first node's value.

---

### Example

```c
int *element = NULL;
OPSTATUS status = LINKED_DEQUE_FUNC(int, Front)(&deque, &element);
```

---

# Linked Deque Back

Obtains a pointer to the last node's value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_DEQUE_FUNC(int, Back)( LINKED_DEQUE_TYPE(int) * deque, int **out);
```

#### Direct form

```c
OPSTATUS Container_Linked_Deque_int_Back( Container_Linked_TDeque_int * deque, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |
| `out` | `int **out` | Typed pointer to an output. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Obtains a pointer to the last node's value.

---

### Example

```c
int *element = NULL;
OPSTATUS status = LINKED_DEQUE_FUNC(int, Back)(&deque, &element);
```

---

# Linked Deque Empty

Reports whether the collection has no nodes.

### Syntax

#### Macro form

```c
bool LINKED_DEQUE_FUNC(int, Empty)( const LINKED_DEQUE_TYPE(int) * deque);
```

#### Direct form

```c
bool Container_Linked_Deque_int_Empty( const Container_Linked_TDeque_int * deque);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `const LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |

---

### Return value

`bool` emptiness predicate.

---

### Remarks

Reports whether the collection has no nodes.

---

### Example

```c
bool empty = LINKED_DEQUE_FUNC(int, Empty)(&deque);
```

---

# Linked Deque Size

Returns the stored node count.

### Syntax

#### Macro form

```c
size_t LINKED_DEQUE_FUNC(int, Size)( const LINKED_DEQUE_TYPE(int) * deque);
```

#### Direct form

```c
size_t Container_Linked_Deque_int_Size( const Container_Linked_TDeque_int * deque);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `deque` | `const LINKED_DEQUE_TYPE(int) * deque` | Container/node pointer. |

---

### Return value

`size_t` live element count.

---

### Remarks

Returns the stored node count.

---

### Example

```c
size_t size = LINKED_DEQUE_FUNC(int, Size)(&deque);
```

---

# Linked Queue package

Header: `Cosmeron/Modules/Container/Linked/Queue.h`.

Linked FIFO queue with node allocations on each push.

Built-in specializations: `int`, `float`, `double`, `char`. The examples below use `int`.

### Instantiation

```c
LINKED_QUEUE_TYPE(int) queue = {0};
LINKED_QUEUE_FUNC(int, Init)(&queue);
```

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#linked-queue-init) | Initializes bookkeeping fields to an empty state. |
| [`Destroy`](#linked-queue-destroy) | Releases the linked nodes and clears the container. |
| [`Push`](#linked-queue-push) | Pushes/enqueues a copied value. |
| [`Pop`](#linked-queue-pop) | Pops/dequeues a value. |
| [`Clear`](#linked-queue-clear) | Removes all nodes without requiring a new instance. |
| [`Front`](#linked-queue-front) | Obtains a pointer to the first node's value. |
| [`Back`](#linked-queue-back) | Obtains a pointer to the last node's value. |
| [`Empty`](#linked-queue-empty) | Reports whether the collection has no nodes. |
| [`Size`](#linked-queue-size) | Returns the stored node count. |

Node pointers obtained from a list become invalid when their node is erased, or when the collection is cleared/destroyed. Never pass node positions taken from another container.

---

# Linked Queue Init

Initializes bookkeeping fields to an empty state.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_QUEUE_FUNC(int, Init)( LINKED_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
OPSTATUS Container_Linked_Queue_int_Init( Container_Linked_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `LINKED_QUEUE_TYPE(int) * queue` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Initializes bookkeeping fields to an empty state. Calling Init on an already-owning live object would lose node references; destroy it first.

---

### Example

```c
OPSTATUS status = LINKED_QUEUE_FUNC(int, Init)(&queue);
```

---

# Linked Queue Destroy

Releases the linked nodes and clears the container.

### Syntax

#### Macro form

```c
void LINKED_QUEUE_FUNC(int, Destroy)( LINKED_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
void Container_Linked_Queue_int_Destroy( Container_Linked_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `LINKED_QUEUE_TYPE(int) * queue` | Container/node pointer. |

---

### Return value

None.

---

### Remarks

Releases the linked nodes and clears the container. If values are pointers, independently owned pointees are not automatically destroyed.

---

### Example

```c
LINKED_QUEUE_FUNC(int, Destroy)(&queue);
```

---

# Linked Queue Push

Pushes/enqueues a copied value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_QUEUE_FUNC(int, Push)( LINKED_QUEUE_TYPE(int) * queue, int value);
```

#### Direct form

```c
OPSTATUS Container_Linked_Queue_int_Push( Container_Linked_TQueue_int * queue, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `LINKED_QUEUE_TYPE(int) * queue` | Container/node pointer. |
| `value` | `int value` | Scalar value or index. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Pushes/enqueues a copied value.

---

### Example

```c
OPSTATUS status = LINKED_QUEUE_FUNC(int, Push)(&queue, 42);
```

---

# Linked Queue Pop

Pops/dequeues a value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_QUEUE_FUNC(int, Pop)( LINKED_QUEUE_TYPE(int) * queue, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Linked_Queue_int_Pop( Container_Linked_TQueue_int * queue, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `LINKED_QUEUE_TYPE(int) * queue` | Container/node pointer. |
| `outValue` | `int * outValue` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Pops/dequeues a value. The removed node ceases to exist and all pointers to it become invalid.

---

### Example

```c
int value = 0;
OPSTATUS status = LINKED_QUEUE_FUNC(int, Pop)(&queue, &value);
```

---

# Linked Queue Clear

Removes all nodes without requiring a new instance.

### Syntax

#### Macro form

```c
void LINKED_QUEUE_FUNC(int, Clear)( LINKED_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
void Container_Linked_Queue_int_Clear( Container_Linked_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `LINKED_QUEUE_TYPE(int) * queue` | Container/node pointer. |

---

### Return value

None.

---

### Remarks

Removes all nodes without requiring a new instance. If values are pointers, independently owned pointees are not automatically destroyed.

---

### Example

```c
LINKED_QUEUE_FUNC(int, Clear)(&queue);
```

---

# Linked Queue Front

Obtains a pointer to the first node's value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_QUEUE_FUNC(int, Front)( LINKED_QUEUE_TYPE(int) * queue, int **out);
```

#### Direct form

```c
OPSTATUS Container_Linked_Queue_int_Front( Container_Linked_TQueue_int * queue, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `LINKED_QUEUE_TYPE(int) * queue` | Container/node pointer. |
| `out` | `int **out` | Typed pointer to an output. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Obtains a pointer to the first node's value.

---

### Example

```c
int *element = NULL;
OPSTATUS status = LINKED_QUEUE_FUNC(int, Front)(&queue, &element);
```

---

# Linked Queue Back

Obtains a pointer to the last node's value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_QUEUE_FUNC(int, Back)( LINKED_QUEUE_TYPE(int) * queue, int **out);
```

#### Direct form

```c
OPSTATUS Container_Linked_Queue_int_Back( Container_Linked_TQueue_int * queue, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `LINKED_QUEUE_TYPE(int) * queue` | Container/node pointer. |
| `out` | `int **out` | Typed pointer to an output. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Obtains a pointer to the last node's value.

---

### Example

```c
int *element = NULL;
OPSTATUS status = LINKED_QUEUE_FUNC(int, Back)(&queue, &element);
```

---

# Linked Queue Empty

Reports whether the collection has no nodes.

### Syntax

#### Macro form

```c
bool LINKED_QUEUE_FUNC(int, Empty)( const LINKED_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
bool Container_Linked_Queue_int_Empty( const Container_Linked_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `const LINKED_QUEUE_TYPE(int) * queue` | Container/node pointer. |

---

### Return value

`bool` emptiness predicate.

---

### Remarks

Reports whether the collection has no nodes.

---

### Example

```c
bool empty = LINKED_QUEUE_FUNC(int, Empty)(&queue);
```

---

# Linked Queue Size

Returns the stored node count.

### Syntax

#### Macro form

```c
size_t LINKED_QUEUE_FUNC(int, Size)( const LINKED_QUEUE_TYPE(int) * queue);
```

#### Direct form

```c
size_t Container_Linked_Queue_int_Size( const Container_Linked_TQueue_int * queue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `queue` | `const LINKED_QUEUE_TYPE(int) * queue` | Container/node pointer. |

---

### Return value

`size_t` live element count.

---

### Remarks

Returns the stored node count.

---

### Example

```c
size_t size = LINKED_QUEUE_FUNC(int, Size)(&queue);
```

---

# Linked Stack package

Header: `Cosmeron/Modules/Container/Linked/Stack.h`.

Linked LIFO stack with node allocations on each push.

Built-in specializations: `int`, `float`, `double`, `char`. The examples below use `int`.

### Instantiation

```c
LINKED_STACK_TYPE(int) stack = {0};
LINKED_STACK_FUNC(int, Init)(&stack);
```

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#linked-stack-init) | Initializes bookkeeping fields to an empty state. |
| [`Destroy`](#linked-stack-destroy) | Releases the linked nodes and clears the container. |
| [`Push`](#linked-stack-push) | Pushes/enqueues a copied value. |
| [`Pop`](#linked-stack-pop) | Pops/dequeues a value. |
| [`Clear`](#linked-stack-clear) | Removes all nodes without requiring a new instance. |
| [`Top`](#linked-stack-top) | Obtains a pointer to the top node's value. |
| [`Empty`](#linked-stack-empty) | Reports whether the collection has no nodes. |
| [`Size`](#linked-stack-size) | Returns the stored node count. |

Node pointers obtained from a list become invalid when their node is erased, or when the collection is cleared/destroyed. Never pass node positions taken from another container.

---

# Linked Stack Init

Initializes bookkeeping fields to an empty state.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_STACK_FUNC(int, Init)( LINKED_STACK_TYPE(int) * stack);
```

#### Direct form

```c
OPSTATUS Container_Linked_Stack_int_Init( Container_Linked_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `LINKED_STACK_TYPE(int) * stack` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Initializes bookkeeping fields to an empty state. Calling Init on an already-owning live object would lose node references; destroy it first.

---

### Example

```c
OPSTATUS status = LINKED_STACK_FUNC(int, Init)(&stack);
```

---

# Linked Stack Destroy

Releases the linked nodes and clears the container.

### Syntax

#### Macro form

```c
void LINKED_STACK_FUNC(int, Destroy)( LINKED_STACK_TYPE(int) * stack);
```

#### Direct form

```c
void Container_Linked_Stack_int_Destroy( Container_Linked_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `LINKED_STACK_TYPE(int) * stack` | Container/node pointer. |

---

### Return value

None.

---

### Remarks

Releases the linked nodes and clears the container. If values are pointers, independently owned pointees are not automatically destroyed.

---

### Example

```c
LINKED_STACK_FUNC(int, Destroy)(&stack);
```

---

# Linked Stack Push

Pushes/enqueues a copied value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_STACK_FUNC(int, Push)( LINKED_STACK_TYPE(int) * stack, int value);
```

#### Direct form

```c
OPSTATUS Container_Linked_Stack_int_Push( Container_Linked_TStack_int * stack, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `LINKED_STACK_TYPE(int) * stack` | Container/node pointer. |
| `value` | `int value` | Scalar value or index. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Pushes/enqueues a copied value.

---

### Example

```c
OPSTATUS status = LINKED_STACK_FUNC(int, Push)(&stack, 42);
```

---

# Linked Stack Pop

Pops/dequeues a value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_STACK_FUNC(int, Pop)( LINKED_STACK_TYPE(int) * stack, int * outValue);
```

#### Direct form

```c
OPSTATUS Container_Linked_Stack_int_Pop( Container_Linked_TStack_int * stack, int * outValue);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `LINKED_STACK_TYPE(int) * stack` | Container/node pointer. |
| `outValue` | `int * outValue` | Container/node pointer. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Pops/dequeues a value. The removed node ceases to exist and all pointers to it become invalid.

---

### Example

```c
int value = 0;
OPSTATUS status = LINKED_STACK_FUNC(int, Pop)(&stack, &value);
```

---

# Linked Stack Clear

Removes all nodes without requiring a new instance.

### Syntax

#### Macro form

```c
void LINKED_STACK_FUNC(int, Clear)( LINKED_STACK_TYPE(int) * stack);
```

#### Direct form

```c
void Container_Linked_Stack_int_Clear( Container_Linked_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `LINKED_STACK_TYPE(int) * stack` | Container/node pointer. |

---

### Return value

None.

---

### Remarks

Removes all nodes without requiring a new instance. If values are pointers, independently owned pointees are not automatically destroyed.

---

### Example

```c
LINKED_STACK_FUNC(int, Clear)(&stack);
```

---

# Linked Stack Top

Obtains a pointer to the top node's value.

### Syntax

#### Macro form

```c
OPSTATUS LINKED_STACK_FUNC(int, Top)( LINKED_STACK_TYPE(int) * stack, int **out);
```

#### Direct form

```c
OPSTATUS Container_Linked_Stack_int_Top( Container_Linked_TStack_int * stack, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `LINKED_STACK_TYPE(int) * stack` | Container/node pointer. |
| `out` | `int **out` | Typed pointer to an output. |

---

### Return value

`OPSTATUS`; check success before accessing outputs.

---

### Remarks

Obtains a pointer to the top node's value.

---

### Example

```c
int *element = NULL;
OPSTATUS status = LINKED_STACK_FUNC(int, Top)(&stack, &element);
```

---

# Linked Stack Empty

Reports whether the collection has no nodes.

### Syntax

#### Macro form

```c
bool LINKED_STACK_FUNC(int, Empty)( const LINKED_STACK_TYPE(int) * stack);
```

#### Direct form

```c
bool Container_Linked_Stack_int_Empty( const Container_Linked_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `const LINKED_STACK_TYPE(int) * stack` | Container/node pointer. |

---

### Return value

`bool` emptiness predicate.

---

### Remarks

Reports whether the collection has no nodes.

---

### Example

```c
bool empty = LINKED_STACK_FUNC(int, Empty)(&stack);
```

---

# Linked Stack Size

Returns the stored node count.

### Syntax

#### Macro form

```c
size_t LINKED_STACK_FUNC(int, Size)( const LINKED_STACK_TYPE(int) * stack);
```

#### Direct form

```c
size_t Container_Linked_Stack_int_Size( const Container_Linked_TStack_int * stack);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `stack` | `const LINKED_STACK_TYPE(int) * stack` | Container/node pointer. |

---

### Return value

`size_t` live element count.

---

### Remarks

Returns the stored node count.

---

### Example

```c
size_t size = LINKED_STACK_FUNC(int, Size)(&stack);
```
