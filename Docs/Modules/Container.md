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

---

# Hash Map package

Header: `Cosmeron/Modules/Container/Hash/Hash.h`.

Open-addressing hash map with linear probing and deleted-bucket tombstones. Built-ins include integer or C-string keys and int or void* values.

### Instantiation

```c
THASH_TYPE(int, int) hash = {0};
HASH_OPERATION(int, int, Init)(&hash);
```

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#hash-map-init) | Initializes empty storage. |
| [`Destroy`](#hash-map-destroy) | Releases all container-managed memory. |
| [`Rehash`](#hash-map-rehash) | Rebuilds the hash bucket table at a requested capacity. |
| [`Find`](#hash-map-find) | Returns a pointer to a mapped value. |
| [`ConstFind`](#hash-map-constfind) | Returns a read-only pointer to a mapped value. |
| [`Contains`](#hash-map-contains) | Reports whether the key is stored. |
| [`Insert`](#hash-map-insert) | Adds a new key/value pair or overwrites the value for an existing key. |
| [`Remove`](#hash-map-remove) | Removes the indicated key. |
| [`Clear`](#hash-map-clear) | Removes all entries/edges while retaining a usable object. |
| [`IsEmpty`](#hash-map-isempty) | Reports whether no entries or vertices exist. |
| [`Size`](#hash-map-size) | Returns number of live hash entries. |
| [`Capacity`](#hash-map-capacity) | Returns allocated hash bucket count. |

### Hash contracts

- Helpers: `HASH_INT(value)`, `HASH_CSTRING(value)`, `HASH_EQUAL_INT(left,right)` and `HASH_EQUAL_CSTRING(left,right)`.
- Bucket states: `HASH_CONST(BUCKET_EMPTY)`, `HASH_CONST(BUCKET_OCCUPIED)`, `HASH_CONST(BUCKET_DELETED)`.
- Initial capacity is 16 and growth target is 75% load, accounting for tombstones.
- C-string keys are stored by pointer: keep pointed-to bytes alive and unchanged while inserted. Rehash/automatic growth invalidates previously borrowed value pointers.
- `HASH_MAP_IMPLEMENT_ALL(KEY_TYPE, KEY_SUFFIX, VALUE_TYPE, VALUE_SUFFIX, HASH_FUNCTION, EQUAL_FUNCTION)` generates custom specializations; `HASH_MAP_DECLARE(KEY_SUFFIX, VALUE_SUFFIX, NAME)` declares a bound, initialized instance.

---

# Hash Map Init

Initializes empty storage.

### Syntax

#### Macro form

```c
OPSTATUS HASH_OPERATION(int, int, Init)( THASH_TYPE(int, int) * hash);
```

#### Direct form

```c
OPSTATUS Container_Hash_int_int_Init( Container_Hash_THash_int_int * hash);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Initializes empty storage.

---

### Example

```c
THASH_TYPE(int, int) hash = {0};
OPSTATUS status = HASH_OPERATION(int, int, Init)(&hash);
```

---

# Hash Map Destroy

Releases all container-managed memory.

### Syntax

#### Macro form

```c
void HASH_OPERATION(int, int, Destroy)( THASH_TYPE(int, int) * hash);
```

#### Direct form

```c
void Container_Hash_int_int_Destroy( Container_Hash_THash_int_int * hash);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |

---

### Return value

None.

---

### Remarks

Releases all container-managed memory. Stored pointees are not automatically deep-freed.

---

### Example

```c
HASH_OPERATION(int, int, Destroy)(0, &hash);
```

---

# Hash Map Rehash

Rebuilds the hash bucket table at a requested capacity.

### Syntax

#### Macro form

```c
OPSTATUS HASH_OPERATION(int, int, Rehash)( THASH_TYPE(int, int) * hash, size_t newCapacity);
```

#### Direct form

```c
OPSTATUS Container_Hash_int_int_Rehash( Container_Hash_THash_int_int * hash, size_t newCapacity);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |
| `newCapacity` | `size_t newCapacity` | Input scalar, key, index or value. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Rebuilds the hash bucket table at a requested capacity. Bucket relocation can invalidate pointers returned by Find/ConstFind.

---

### Example

```c
OPSTATUS status = HASH_OPERATION(int, int, Rehash)(0, &hash, 32);
```

---

# Hash Map Find

Returns a pointer to a mapped value.

### Syntax

#### Macro form

```c
OPSTATUS HASH_OPERATION(int, int, Find)( THASH_TYPE(int, int) * hash, int key, int **out);
```

#### Direct form

```c
OPSTATUS Container_Hash_int_int_Find( Container_Hash_THash_int_int * hash, int key, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |
| `key` | `int key` | Input scalar, key, index or value. |
| `out` | `int **out` | Typed out pointer to container-owned data. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Returns a pointer to a mapped value.

---

### Example

```c
int *found = NULL;
OPSTATUS status = HASH_OPERATION(int, int, Find)(0, &hash, 7, &found);
```

---

# Hash Map ConstFind

Returns a read-only pointer to a mapped value.

### Syntax

#### Macro form

```c
OPSTATUS HASH_OPERATION(int, int, ConstFind)( const THASH_TYPE(int, int) * hash, int key, int const **out);
```

#### Direct form

```c
OPSTATUS Container_Hash_int_int_ConstFind( const Container_Hash_THash_int_int * hash, int key, int const **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `const THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |
| `key` | `int key` | Input scalar, key, index or value. |
| `out` | `int const **out` | Typed out pointer to container-owned data. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Returns a read-only pointer to a mapped value.

---

### Example

```c
const int *found = NULL;
OPSTATUS status = HASH_OPERATION(int, int, ConstFind)(0, &hash, 7, &found);
```

---

# Hash Map Contains

Reports whether the key is stored.

### Syntax

#### Macro form

```c
bool HASH_OPERATION(int, int, Contains)( const THASH_TYPE(int, int) * hash, int key);
```

#### Direct form

```c
bool Container_Hash_int_int_Contains( const Container_Hash_THash_int_int * hash, int key);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `const THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |
| `key` | `int key` | Input scalar, key, index or value. |

---

### Return value

`bool`: true or false.

---

### Remarks

Reports whether the key is stored.

---

### Example

```c
bool present = HASH_OPERATION(int, int, Contains)(0, &hash, 7);
```

---

# Hash Map Insert

Adds a new key/value pair or overwrites the value for an existing key.

### Syntax

#### Macro form

```c
OPSTATUS HASH_OPERATION(int, int, Insert)( THASH_TYPE(int, int) * hash, int key, int value);
```

#### Direct form

```c
OPSTATUS Container_Hash_int_int_Insert( Container_Hash_THash_int_int * hash, int key, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |
| `key` | `int key` | Input scalar, key, index or value. |
| `value` | `int value` | Input scalar, key, index or value. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Adds a new key/value pair or overwrites the value for an existing key. Bucket relocation can invalidate pointers returned by Find/ConstFind.

---

### Example

```c
OPSTATUS status = HASH_OPERATION(int, int, Insert)(0, &hash, 7, 70);
```

---

# Hash Map Remove

Removes the indicated key.

### Syntax

#### Macro form

```c
OPSTATUS HASH_OPERATION(int, int, Remove)( THASH_TYPE(int, int) * hash, int key);
```

#### Direct form

```c
OPSTATUS Container_Hash_int_int_Remove( Container_Hash_THash_int_int * hash, int key);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |
| `key` | `int key` | Input scalar, key, index or value. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Removes the indicated key.

---

### Example

```c
OPSTATUS status = HASH_OPERATION(int, int, Remove)(0, &hash, 7);
```

---

# Hash Map Clear

Removes all entries/edges while retaining a usable object.

### Syntax

#### Macro form

```c
void HASH_OPERATION(int, int, Clear)( THASH_TYPE(int, int) * hash);
```

#### Direct form

```c
void Container_Hash_int_int_Clear( Container_Hash_THash_int_int * hash);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |

---

### Return value

None.

---

### Remarks

Removes all entries/edges while retaining a usable object. Stored pointees are not automatically deep-freed.

---

### Example

```c
HASH_OPERATION(int, int, Clear)(0, &hash);
```

---

# Hash Map IsEmpty

Reports whether no entries or vertices exist.

### Syntax

#### Macro form

```c
bool HASH_OPERATION(int, int, IsEmpty)( const THASH_TYPE(int, int) * hash);
```

#### Direct form

```c
bool Container_Hash_int_int_IsEmpty( const Container_Hash_THash_int_int * hash);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `const THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |

---

### Return value

`bool`: true or false.

---

### Remarks

Reports whether no entries or vertices exist.

---

### Example

```c
bool present = HASH_OPERATION(int, int, IsEmpty)(0, &hash);
```

---

# Hash Map Size

Returns number of live hash entries.

### Syntax

#### Macro form

```c
size_t HASH_OPERATION(int, int, Size)( const THASH_TYPE(int, int) * hash);
```

#### Direct form

```c
size_t Container_Hash_int_int_Size( const Container_Hash_THash_int_int * hash);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `const THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |

---

### Return value

`size_t`: number of entries/vertices/edges or capacity.

---

### Remarks

Returns number of live hash entries.

---

### Example

```c
size_t total = HASH_OPERATION(int, int, Size)(0, &hash);
```

---

# Hash Map Capacity

Returns allocated hash bucket count.

### Syntax

#### Macro form

```c
size_t HASH_OPERATION(int, int, Capacity)( const THASH_TYPE(int, int) * hash);
```

#### Direct form

```c
size_t Container_Hash_int_int_Capacity( const Container_Hash_THash_int_int * hash);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `const THASH_TYPE(int` | Input scalar, key, index or value. |
| `hash` | `int) * hash` | Container pointer, callback, or output parameter. |

---

### Return value

`size_t`: number of entries/vertices/edges or capacity.

---

### Remarks

Returns allocated hash bucket count.

---

### Example

```c
size_t total = HASH_OPERATION(int, int, Capacity)(0, &hash);
```

---

# Graph package

Header: `Cosmeron/Modules/Container/Graph/Graph.h`.

Directed graph with dynamically allocated adjacency lists; built-in vertex data is int, edge weights are int or float.

### Instantiation

```c
TGRAPH_TYPE(int, int) graph = {0};
GRAPH_OPERATION(int, int, Init)(&graph);
```

### Function summary

| Function | Description |
| --- | --- |
| [`Init`](#graph-init) | Initializes empty storage. |
| [`Destroy`](#graph-destroy) | Releases all container-managed memory. |
| [`Clear`](#graph-clear) | Removes all entries/edges while retaining a usable object. |
| [`AddVertex`](#graph-addvertex) | Adds a new vertex and returns its numeric index. |
| [`AddEdge`](#graph-addedge) | Creates or replaces a weighted directed edge. |
| [`RemoveEdge`](#graph-removeedge) | Removes a directed edge. |
| [`RemoveVertex`](#graph-removevertex) | Deletes a vertex and reindexes later vertex numbers and edge destinations. |
| [`HasEdge`](#graph-hasedge) | Checks whether an edge exists from source to destination. |
| [`FindWeight`](#graph-findweight) | Returns a writable pointer to the weight of an edge. |
| [`ConstFindWeight`](#graph-constfindweight) | Returns a read-only pointer to the weight of an edge. |
| [`VertexCount`](#graph-vertexcount) | Returns current number of vertices. |
| [`EdgeCount`](#graph-edgecount) | Returns current number of directed edges. |
| [`IsEmpty`](#graph-isempty) | Reports whether no entries or vertices exist. |
| [`BFS`](#graph-bfs) | Calls a visitor for each reachable vertex in breadth-first order. |
| [`DFS`](#graph-dfs) | Calls a visitor for each reachable vertex in depth-first order. |

### Graph contracts

- Edges are **directed**: adding source→destination does not add destination→source.
- `GRAPH_IMPLEMENT_ALL(VERTEX_TYPE, VERTEX_SUFFIX, WEIGHT_TYPE, WEIGHT_SUFFIX)` generates a custom type family; `GRAPH_DECLARE(VERTEX_SUFFIX, WEIGHT_SUFFIX, NAME)` creates a bound initialized instance.
- Removing vertex index `i` compacts vertex storage; indexes greater than `i` decrement by one, and matching incoming edges are deleted.
- BFS/DFS call a visitor with `(graph, vertexIndex, context)`; traversal can allocate temporary memory and fail if unavailable.

---

# Graph Init

Initializes empty storage.

### Syntax

#### Macro form

```c
OPSTATUS GRAPH_OPERATION(int, int, Init)( TGRAPH_TYPE(int, int) * graph);
```

#### Direct form

```c
OPSTATUS Container_Graph_int_int_Init( Container_Graph_TGraph_int_int * graph);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Initializes empty storage.

---

### Example

```c
TGRAPH_TYPE(int, int) graph = {0};
OPSTATUS status = GRAPH_OPERATION(int, int, Init)(&graph);
```

---

# Graph Destroy

Releases all container-managed memory.

### Syntax

#### Macro form

```c
void GRAPH_OPERATION(int, int, Destroy)( TGRAPH_TYPE(int, int) * graph);
```

#### Direct form

```c
void Container_Graph_int_int_Destroy( Container_Graph_TGraph_int_int * graph);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |

---

### Return value

None.

---

### Remarks

Releases all container-managed memory. Stored pointees are not automatically deep-freed.

---

### Example

```c
GRAPH_OPERATION(int, int, Destroy)(0, &graph);
```

---

# Graph Clear

Removes all entries/edges while retaining a usable object.

### Syntax

#### Macro form

```c
void GRAPH_OPERATION(int, int, Clear)( TGRAPH_TYPE(int, int) * graph);
```

#### Direct form

```c
void Container_Graph_int_int_Clear( Container_Graph_TGraph_int_int * graph);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |

---

### Return value

None.

---

### Remarks

Removes all entries/edges while retaining a usable object. Stored pointees are not automatically deep-freed.

---

### Example

```c
GRAPH_OPERATION(int, int, Clear)(0, &graph);
```

---

# Graph AddVertex

Adds a new vertex and returns its numeric index.

### Syntax

#### Macro form

```c
OPSTATUS GRAPH_OPERATION(int, int, AddVertex)( TGRAPH_TYPE(int, int) * graph, int data, size_t *outIndex);
```

#### Direct form

```c
OPSTATUS Container_Graph_int_int_AddVertex( Container_Graph_TGraph_int_int * graph, int data, size_t *outIndex);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |
| `data` | `int data` | Input scalar, key, index or value. |
| `outIndex` | `size_t *outIndex` | Container pointer, callback, or output parameter. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Adds a new vertex and returns its numeric index.

---

### Example

```c
size_t indexOut = 0;
OPSTATUS status = GRAPH_OPERATION(int, int, AddVertex)(0, &graph, 10, &indexOut);
```

---

# Graph AddEdge

Creates or replaces a weighted directed edge.

### Syntax

#### Macro form

```c
OPSTATUS GRAPH_OPERATION(int, int, AddEdge)( TGRAPH_TYPE(int, int) * graph, size_t source, size_t destination, int weight);
```

#### Direct form

```c
OPSTATUS Container_Graph_int_int_AddEdge( Container_Graph_TGraph_int_int * graph, size_t source, size_t destination, int weight);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |
| `source` | `size_t source` | Input scalar, key, index or value. |
| `destination` | `size_t destination` | Input scalar, key, index or value. |
| `weight` | `int weight` | Input scalar, key, index or value. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Creates or replaces a weighted directed edge.

---

### Example

```c
OPSTATUS status = GRAPH_OPERATION(int, int, AddEdge)(0, &graph, 0, 1, 5);
```

---

# Graph RemoveEdge

Removes a directed edge.

### Syntax

#### Macro form

```c
OPSTATUS GRAPH_OPERATION(int, int, RemoveEdge)( TGRAPH_TYPE(int, int) * graph, size_t source, size_t destination);
```

#### Direct form

```c
OPSTATUS Container_Graph_int_int_RemoveEdge( Container_Graph_TGraph_int_int * graph, size_t source, size_t destination);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |
| `source` | `size_t source` | Input scalar, key, index or value. |
| `destination` | `size_t destination` | Input scalar, key, index or value. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Removes a directed edge.

---

### Example

```c
OPSTATUS status = GRAPH_OPERATION(int, int, RemoveEdge)(0, &graph, 0, 1);
```

---

# Graph RemoveVertex

Deletes a vertex and reindexes later vertex numbers and edge destinations.

### Syntax

#### Macro form

```c
OPSTATUS GRAPH_OPERATION(int, int, RemoveVertex)( TGRAPH_TYPE(int, int) * graph, size_t index);
```

#### Direct form

```c
OPSTATUS Container_Graph_int_int_RemoveVertex( Container_Graph_TGraph_int_int * graph, size_t index);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |
| `index` | `size_t index` | Input scalar, key, index or value. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Deletes a vertex and reindexes later vertex numbers and edge destinations. Previously stored vertex IDs may no longer identify the same vertex.

---

### Example

```c
OPSTATUS status = GRAPH_OPERATION(int, int, RemoveVertex)(0, &graph, 0);
```

---

# Graph HasEdge

Checks whether an edge exists from source to destination.

### Syntax

#### Macro form

```c
bool GRAPH_OPERATION(int, int, HasEdge)( const TGRAPH_TYPE(int, int) * graph, size_t source, size_t destination);
```

#### Direct form

```c
bool Container_Graph_int_int_HasEdge( const Container_Graph_TGraph_int_int * graph, size_t source, size_t destination);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `const TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |
| `source` | `size_t source` | Input scalar, key, index or value. |
| `destination` | `size_t destination` | Input scalar, key, index or value. |

---

### Return value

`bool`: true or false.

---

### Remarks

Checks whether an edge exists from source to destination.

---

### Example

```c
bool present = GRAPH_OPERATION(int, int, HasEdge)(0, &graph, 0, 1);
```

---

# Graph FindWeight

Returns a writable pointer to the weight of an edge.

### Syntax

#### Macro form

```c
OPSTATUS GRAPH_OPERATION(int, int, FindWeight)( TGRAPH_TYPE(int, int) * graph, size_t source, size_t destination, int **outWeight);
```

#### Direct form

```c
OPSTATUS Container_Graph_int_int_FindWeight( Container_Graph_TGraph_int_int * graph, size_t source, size_t destination, int **outWeight);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |
| `source` | `size_t source` | Input scalar, key, index or value. |
| `destination` | `size_t destination` | Input scalar, key, index or value. |
| `outWeight` | `int **outWeight` | Typed out pointer to container-owned data. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Returns a writable pointer to the weight of an edge.

---

### Example

```c
int *weightOut = NULL;
OPSTATUS status = GRAPH_OPERATION(int, int, FindWeight)(0, &graph, 0, 1, &weightOut);
```

---

# Graph ConstFindWeight

Returns a read-only pointer to the weight of an edge.

### Syntax

#### Macro form

```c
OPSTATUS GRAPH_OPERATION(int, int, ConstFindWeight)( const TGRAPH_TYPE(int, int) * graph, size_t source, size_t destination, int const **outWeight);
```

#### Direct form

```c
OPSTATUS Container_Graph_int_int_ConstFindWeight( const Container_Graph_TGraph_int_int * graph, size_t source, size_t destination, int const **outWeight);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `const TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |
| `source` | `size_t source` | Input scalar, key, index or value. |
| `destination` | `size_t destination` | Input scalar, key, index or value. |
| `outWeight` | `int const **outWeight` | Typed out pointer to container-owned data. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Returns a read-only pointer to the weight of an edge.

---

### Example

```c
const int *weightOut = NULL;
OPSTATUS status = GRAPH_OPERATION(int, int, ConstFindWeight)(0, &graph, 0, 1, &weightOut);
```

---

# Graph VertexCount

Returns current number of vertices.

### Syntax

#### Macro form

```c
size_t GRAPH_OPERATION(int, int, VertexCount)( const TGRAPH_TYPE(int, int) * graph);
```

#### Direct form

```c
size_t Container_Graph_int_int_VertexCount( const Container_Graph_TGraph_int_int * graph);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `const TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |

---

### Return value

`size_t`: number of entries/vertices/edges or capacity.

---

### Remarks

Returns current number of vertices.

---

### Example

```c
size_t total = GRAPH_OPERATION(int, int, VertexCount)(0, &graph);
```

---

# Graph EdgeCount

Returns current number of directed edges.

### Syntax

#### Macro form

```c
size_t GRAPH_OPERATION(int, int, EdgeCount)( const TGRAPH_TYPE(int, int) * graph);
```

#### Direct form

```c
size_t Container_Graph_int_int_EdgeCount( const Container_Graph_TGraph_int_int * graph);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `const TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |

---

### Return value

`size_t`: number of entries/vertices/edges or capacity.

---

### Remarks

Returns current number of directed edges.

---

### Example

```c
size_t total = GRAPH_OPERATION(int, int, EdgeCount)(0, &graph);
```

---

# Graph IsEmpty

Reports whether no entries or vertices exist.

### Syntax

#### Macro form

```c
bool GRAPH_OPERATION(int, int, IsEmpty)( const TGRAPH_TYPE(int, int) * graph);
```

#### Direct form

```c
bool Container_Graph_int_int_IsEmpty( const Container_Graph_TGraph_int_int * graph);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `const TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |

---

### Return value

`bool`: true or false.

---

### Remarks

Reports whether no entries or vertices exist.

---

### Example

```c
bool present = GRAPH_OPERATION(int, int, IsEmpty)(0, &graph);
```

---

# Graph BFS

Calls a visitor for each reachable vertex in breadth-first order.

### Syntax

#### Macro form

```c
OPSTATUS GRAPH_OPERATION(int, int, BFS)( TGRAPH_TYPE(int, int) * graph, size_t start, TGRAPH_VISITOR_TYPE(int, int) visitor, void *context);
```

#### Direct form

```c
OPSTATUS Container_Graph_int_int_BFS( Container_Graph_TGraph_int_int * graph, size_t start, Container_Graph_int_int_TVisitor visitor, void *context);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |
| `start` | `size_t start` | Input scalar, key, index or value. |
| `int` | `TGRAPH_VISITOR_TYPE(int` | Input scalar, key, index or value. |
| `visitor` | `int) visitor` | Input scalar, key, index or value. |
| `context` | `void *context` | Container pointer, callback, or output parameter. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Calls a visitor for each reachable vertex in breadth-first order. The visitor is invoked synchronously. Do not change the graph structure during traversal.

---

### Example

```c
/* VisitVertex: callback of TGRAPH_VISITOR_TYPE(int, int). */
OPSTATUS status = GRAPH_OPERATION(int, int, BFS)(0, &graph, 0, 0, VisitVertex, NULL);
```

---

# Graph DFS

Calls a visitor for each reachable vertex in depth-first order.

### Syntax

#### Macro form

```c
OPSTATUS GRAPH_OPERATION(int, int, DFS)( TGRAPH_TYPE(int, int) * graph, size_t start, TGRAPH_VISITOR_TYPE(int, int) visitor, void *context);
```

#### Direct form

```c
OPSTATUS Container_Graph_int_int_DFS( Container_Graph_TGraph_int_int * graph, size_t start, Container_Graph_int_int_TVisitor visitor, void *context);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `int` | `TGRAPH_TYPE(int` | Input scalar, key, index or value. |
| `graph` | `int) * graph` | Container pointer, callback, or output parameter. |
| `start` | `size_t start` | Input scalar, key, index or value. |
| `int` | `TGRAPH_VISITOR_TYPE(int` | Input scalar, key, index or value. |
| `visitor` | `int) visitor` | Input scalar, key, index or value. |
| `context` | `void *context` | Container pointer, callback, or output parameter. |

---

### Return value

`OPSTATUS`: success or a recoverable status such as `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `NOT_FOUND`, or allocation failure.

---

### Remarks

Calls a visitor for each reachable vertex in depth-first order. The visitor is invoked synchronously. Do not change the graph structure during traversal.

---

### Example

```c
/* VisitVertex: callback of TGRAPH_VISITOR_TYPE(int, int). */
OPSTATUS status = GRAPH_OPERATION(int, int, DFS)(0, &graph, 0, 0, VisitVertex, NULL);
```

---

# Tree package

Headers:

```c
#include "Cosmeron/Modules/Container/Tree/Tree.h"
/* Or include BST.h, AVL.h, or RedBlack.h independently. */
```

The tree family generates **ordered Set and Map collections**. It provides three algorithms, each with the same operation names:

| Algorithm | Behavior | Complexity |
| --- | --- | --- |
| BST | Plain binary search tree, no balancing | O(h), worst case O(n) |
| AVL | Height-balanced binary search tree | O(log n) |
| RB | Red-black balanced search tree | O(log n) |

Here `h` means actual tree height. The default set specializations are `int`, `float`, and `double`; default maps use int keys with int or float values. The default floating-point comparator is **not a total ordering for NaNs**; use a suitable custom comparator if NaNs may appear.

### Types, function names and generation

```c
TREE_SET_TYPE(AVL, int)              /* Container_Tree_AVL_Set_int */
TREE_MAP_TYPE(AVL, int, int)         /* Container_Tree_AVL_Map_int_int */
TREE_NODE(TREE_SET_TYPE(AVL, int))   /* Container_Tree_AVL_Set_int_Node */
TREE_FUNC(TREE_SET_TYPE(AVL, int), Insert) /* Generated insert function */
```

`TREE_PUBLIC_SET_TYPE` and `TREE_PUBLIC_MAP_TYPE` provide public typed aliases. Compatibility macros `TTREE_SET_TYPE`, `TTREE_MAP_TYPE` and `TTREE_FN` are also available.

### Function summary

| Operation | Set / Map |
| --- | --- |
| [`Init`](#tree-init) | Initializes an empty tree object. |
| [`Destroy`](#tree-destroy) | Releases every node owned by the tree. |
| [`Clear`](#tree-clear) | Deletes all nodes and keeps the tree reusable. |
| [`Insert (Set)`](#tree-insert-set) | Inserts a Set key or Map key/value pair. |
| [`Insert (Map)`](#tree-insert-map) | Inserts a Set key or Map key/value pair. |
| [`Remove`](#tree-remove) | Removes a key and its corresponding node. |
| [`FindNode`](#tree-findnode) | Returns a pointer to the node matching a key, or NULL. |
| [`Find (Set)`](#tree-find-set) | Returns a typed pointer to an existing Set key or Map value. |
| [`Find (Map)`](#tree-find-map) | Returns a typed pointer to an existing Set key or Map value. |
| [`Contains`](#tree-contains) | Checks whether the tree contains the given key. |
| [`MinNode`](#tree-minnode) | Finds the leftmost node of a given subtree. |
| [`MaxNode`](#tree-maxnode) | Finds the rightmost node of a given subtree. |
| [`Min`](#tree-min) | Returns a pointer to the minimum key. |
| [`Max`](#tree-max) | Returns a pointer to the maximum key. |
| [`Empty`](#tree-empty) | Returns true when tree size is zero. |
| [`Size`](#tree-size) | Returns the number of stored keys. |
| [`Begin`](#tree-begin) | Returns the smallest node for in-order traversal. |
| [`End`](#tree-end) | Returns NULL, the past-end iterator. |
| [`Next`](#tree-next) | Returns the in-order successor of a node. |
| [`Prev`](#tree-prev) | Returns the in-order predecessor of a node. |
| [`ConstBegin`](#tree-constbegin) | Returns the first node with read-only access. |
| [`ConstEnd`](#tree-constend) | Returns the read-only NULL past-end sentinel. |
| [`ConstNext`](#tree-constnext) | Returns a read-only successor node. |
| [`ConstPrev`](#tree-constprev) | Returns a read-only predecessor node. |

### Initializing a tree

`TREE_SET_DECLARE(ALG, KEY, NAME)` and `TREE_MAP_DECLARE(ALG, KEY, VAL, NAME)` declare an empty object and optionally bind its function table. They **do not call Init**. In comparison, `TREE_AVL_SET_INSTANCE_DECLARE` and its BST/RB/Map variants do call Init.

```c
TREE_SET_DECLARE(AVL, int, tree);
TREE_FUNC(TREE_SET_TYPE(AVL, int), Insert)(&tree, 42);
TREE_FUNC(TREE_SET_TYPE(AVL, int), Destroy)(&tree);
```

### Generator families

| Algorithm | Set generator | Map generator |
| --- | --- | --- |
| BST | `TREE_BST_SET_IMPLEMENT_ALL(KEY_TYPE)` | `TREE_BST_MAP_IMPLEMENT_ALL(KEY_TYPE, VALUE_TYPE)` |
| AVL | `TREE_AVL_SET_IMPLEMENT_ALL(KEY_TYPE)` | `TREE_AVL_MAP_IMPLEMENT_ALL(KEY_TYPE, VALUE_TYPE)` |
| RB | `TREE_RB_SET_IMPLEMENT_ALL(KEY_TYPE)` | `TREE_RB_MAP_IMPLEMENT_ALL(KEY_TYPE, VALUE_TYPE)` |

`*_IMPLEMENT_ALL_CMP(...)` variants accept custom comparison functions returning `CMPOUT`. Each algorithm also exposes `*_SET_INSTANCE_DECLARE` and `*_MAP_INSTANCE_DECLARE`. Avoid re-emitting built-in specializations already declared by a header in the same translation unit.

---

# Tree Init

Initializes an empty tree object.

### Syntax

#### Macro form

```c
OPSTATUS TREE_FUNC(TREE_SET_TYPE(AVL, int), Init)(TREE_SET_TYPE(AVL, int) *tree);
```

#### Direct form

```c
OPSTATUS Container_Tree_AVL_Set_int_Init(Container_Tree_AVL_Set_int *tree);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |

---

### Return value

`OPSTATUS`: success or an operational error, including `NOT_FOUND` when a key is missing.

---

### Remarks

Initializes an empty tree object. Calling Init on a live nonempty tree leaks existing nodes; destroy it before reinitializing.

---

### Example

```c
TREE_SET_TYPE(AVL, int) tree = {0};
OPSTATUS status = TREE_FUNC(TREE_SET_TYPE(AVL, int), Init)(&tree);
```

---

# Tree Destroy

Releases every node owned by the tree.

### Syntax

#### Macro form

```c
void TREE_FUNC(TREE_SET_TYPE(AVL, int), Destroy)(TREE_SET_TYPE(AVL, int) *tree);
```

#### Direct form

```c
void Container_Tree_AVL_Set_int_Destroy(Container_Tree_AVL_Set_int *tree);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |

---

### Return value

None.

---

### Remarks

Releases every node owned by the tree. Node memory is released, but separately owned pointees stored in keys/values are not automatically deep-freed.

---

### Example

```c
TREE_FUNC(TREE_SET_TYPE(AVL, int), Destroy)(NULL, &tree);
```

---

# Tree Clear

Deletes all nodes and keeps the tree reusable.

### Syntax

#### Macro form

```c
void TREE_FUNC(TREE_SET_TYPE(AVL, int), Clear)(TREE_SET_TYPE(AVL, int) *tree);
```

#### Direct form

```c
void Container_Tree_AVL_Set_int_Clear(Container_Tree_AVL_Set_int *tree);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |

---

### Return value

None.

---

### Remarks

Deletes all nodes and keeps the tree reusable. Node memory is released, but separately owned pointees stored in keys/values are not automatically deep-freed.

---

### Example

```c
TREE_FUNC(TREE_SET_TYPE(AVL, int), Clear)(NULL, &tree);
```

---

# Tree Insert Set

Inserts a Set key or Map key/value pair.

### Syntax

#### Macro form

```c
OPSTATUS TREE_FUNC(TREE_SET_TYPE(AVL, int), Insert)(TREE_SET_TYPE(AVL, int) *tree, int key);
```

#### Direct form

```c
OPSTATUS Container_Tree_AVL_Set_int_Insert(Container_Tree_AVL_Set_int *tree, int key);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |
| `key` | `int key` | Key or value supplied by caller. |

---

### Return value

`OPSTATUS`: success or an operational error, including `NOT_FOUND` when a key is missing.

---

### Remarks

Inserts a Set key or Map key/value pair. The set stores each key according to the generated comparator.

---

### Example

```c
OPSTATUS status = TREE_FUNC(TREE_SET_TYPE(AVL, int), Insert)(NULL, &tree, 42);
```

---

# Tree Insert Map

Inserts a Set key or Map key/value pair.

### Syntax

#### Macro form

```c
OPSTATUS TREE_FUNC(TREE_MAP_TYPE(AVL, int, int), Insert)(TREE_MAP_TYPE(AVL, int, int) *tree, int key, int value);
```

#### Direct form

```c
OPSTATUS Container_Tree_AVL_Map_int_int_Insert(Container_Tree_AVL_Map_int_int *tree, int key, int value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_MAP_TYPE(AVL` | Key or value supplied by caller. |
| `int` | `int` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |
| `key` | `int key` | Key or value supplied by caller. |
| `value` | `int value` | Key or value supplied by caller. |

---

### Return value

`OPSTATUS`: success or an operational error, including `NOT_FOUND` when a key is missing.

---

### Remarks

Inserts a Set key or Map key/value pair. The map stores a key/value pair; an existing key is handled according to the generated map implementation.

---

### Example

```c
OPSTATUS status = TREE_FUNC(TREE_MAP_TYPE(AVL, int, int), Insert)(NULL, NULL, &tree, 42, 100);
```

---

# Tree Remove

Removes a key and its corresponding node.

### Syntax

#### Macro form

```c
OPSTATUS TREE_FUNC(TREE_SET_TYPE(AVL, int), Remove)(TREE_SET_TYPE(AVL, int) *tree, int key);
```

#### Direct form

```c
OPSTATUS Container_Tree_AVL_Set_int_Remove(Container_Tree_AVL_Set_int *tree, int key);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |
| `key` | `int key` | Key or value supplied by caller. |

---

### Return value

`OPSTATUS`: success or an operational error, including `NOT_FOUND` when a key is missing.

---

### Remarks

Removes a key and its corresponding node.

---

### Example

```c
OPSTATUS status = TREE_FUNC(TREE_SET_TYPE(AVL, int), Remove)(NULL, &tree, 42);
```

---

# Tree FindNode

Returns a pointer to the node matching a key, or NULL.

### Syntax

#### Macro form

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), FindNode)(TREE_SET_TYPE(AVL, int) *tree, int key);
```

#### Direct form

```c
Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_FindNode(Container_Tree_AVL_Set_int *tree, int key);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |
| `key` | `int key` | Key or value supplied by caller. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Returns a pointer to the node matching a key, or NULL.

---

### Example

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), FindNode)(NULL, &tree, 42);
```

---

# Tree Find Set

Returns a typed pointer to an existing Set key or Map value.

### Syntax

#### Macro form

```c
OPSTATUS TREE_FUNC(TREE_SET_TYPE(AVL, int), Find)(TREE_SET_TYPE(AVL, int) *tree, int key, int **out);
```

#### Direct form

```c
OPSTATUS Container_Tree_AVL_Set_int_Find(Container_Tree_AVL_Set_int *tree, int key, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |
| `key` | `int key` | Key or value supplied by caller. |
| `out` | `int **out` | Output pointer to a tree-stored key/value. |

---

### Return value

`OPSTATUS`: success or an operational error, including `NOT_FOUND` when a key is missing.

---

### Remarks

Returns a typed pointer to an existing Set key or Map value. The pointer refers to data inside the tree, not a newly allocated result.

---

### Example

```c
int *found = NULL;
OPSTATUS status = TREE_FUNC(TREE_SET_TYPE(AVL, int), Find)(NULL, &tree, 42, &found);
```

---

# Tree Find Map

Returns a typed pointer to an existing Set key or Map value.

### Syntax

#### Macro form

```c
OPSTATUS TREE_FUNC(TREE_MAP_TYPE(AVL, int, int), Find)(TREE_MAP_TYPE(AVL, int, int) *tree, int key, int **out);
```

#### Direct form

```c
OPSTATUS Container_Tree_AVL_Map_int_int_Find(Container_Tree_AVL_Map_int_int *tree, int key, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_MAP_TYPE(AVL` | Key or value supplied by caller. |
| `int` | `int` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |
| `key` | `int key` | Key or value supplied by caller. |
| `out` | `int **out` | Output pointer to a tree-stored key/value. |

---

### Return value

`OPSTATUS`: success or an operational error, including `NOT_FOUND` when a key is missing.

---

### Remarks

Returns a typed pointer to an existing Set key or Map value. The pointer refers to data inside the tree, not a newly allocated result.

---

### Example

```c
int *found = NULL;
OPSTATUS status = TREE_FUNC(TREE_MAP_TYPE(AVL, int, int), Find)(NULL, NULL, &tree, 42, &found);
```

---

# Tree Contains

Checks whether the tree contains the given key.

### Syntax

#### Macro form

```c
bool TREE_FUNC(TREE_SET_TYPE(AVL, int), Contains)(const TREE_SET_TYPE(AVL, int) *tree, int key);
```

#### Direct form

```c
bool Container_Tree_AVL_Set_int_Contains(const Container_Tree_AVL_Set_int *tree, int key);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `const TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |
| `key` | `int key` | Key or value supplied by caller. |

---

### Return value

`bool` predicate.

---

### Remarks

Checks whether the tree contains the given key.

---

### Example

```c
bool present = TREE_FUNC(TREE_SET_TYPE(AVL, int), Contains)(NULL, &tree, 42);
```

---

# Tree MinNode

Finds the leftmost node of a given subtree.

### Syntax

#### Macro form

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), MinNode)(TREE_NODE(TREE_SET_TYPE(AVL, int)) *node);
```

#### Direct form

```c
Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_MinNode(Container_Tree_AVL_Set_int_Node *node);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_NODE(TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `node` | `int)) *node` | Tree or node pointer. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Finds the leftmost node of a given subtree.

---

### Example

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) *node = TREE_FUNC(TREE_SET_TYPE(AVL, int), Begin)(&tree);
TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), MinNode)(NULL, node);
```

---

# Tree MaxNode

Finds the rightmost node of a given subtree.

### Syntax

#### Macro form

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), MaxNode)(TREE_NODE(TREE_SET_TYPE(AVL, int)) *node);
```

#### Direct form

```c
Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_MaxNode(Container_Tree_AVL_Set_int_Node *node);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_NODE(TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `node` | `int)) *node` | Tree or node pointer. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Finds the rightmost node of a given subtree.

---

### Example

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) *node = TREE_FUNC(TREE_SET_TYPE(AVL, int), Begin)(&tree);
TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), MaxNode)(NULL, node);
```

---

# Tree Min

Returns a pointer to the minimum key.

### Syntax

#### Macro form

```c
OPSTATUS TREE_FUNC(TREE_SET_TYPE(AVL, int), Min)(TREE_SET_TYPE(AVL, int) *tree, int **out);
```

#### Direct form

```c
OPSTATUS Container_Tree_AVL_Set_int_Min(Container_Tree_AVL_Set_int *tree, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |
| `out` | `int **out` | Output pointer to a tree-stored key/value. |

---

### Return value

`OPSTATUS`: success or an operational error, including `NOT_FOUND` when a key is missing.

---

### Remarks

Returns a pointer to the minimum key. The pointer refers to data inside the tree, not a newly allocated result.

---

### Example

```c
int *found = NULL;
OPSTATUS status = TREE_FUNC(TREE_SET_TYPE(AVL, int), Min)(NULL, &tree, &found);
```

---

# Tree Max

Returns a pointer to the maximum key.

### Syntax

#### Macro form

```c
OPSTATUS TREE_FUNC(TREE_SET_TYPE(AVL, int), Max)(TREE_SET_TYPE(AVL, int) *tree, int **out);
```

#### Direct form

```c
OPSTATUS Container_Tree_AVL_Set_int_Max(Container_Tree_AVL_Set_int *tree, int **out);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |
| `out` | `int **out` | Output pointer to a tree-stored key/value. |

---

### Return value

`OPSTATUS`: success or an operational error, including `NOT_FOUND` when a key is missing.

---

### Remarks

Returns a pointer to the maximum key. The pointer refers to data inside the tree, not a newly allocated result.

---

### Example

```c
int *found = NULL;
OPSTATUS status = TREE_FUNC(TREE_SET_TYPE(AVL, int), Max)(NULL, &tree, &found);
```

---

# Tree Empty

Returns true when tree size is zero.

### Syntax

#### Macro form

```c
bool TREE_FUNC(TREE_SET_TYPE(AVL, int), Empty)(const TREE_SET_TYPE(AVL, int) *tree);
```

#### Direct form

```c
bool Container_Tree_AVL_Set_int_Empty(const Container_Tree_AVL_Set_int *tree);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `const TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |

---

### Return value

`bool` predicate.

---

### Remarks

Returns true when tree size is zero.

---

### Example

```c
bool present = TREE_FUNC(TREE_SET_TYPE(AVL, int), Empty)(NULL, &tree);
```

---

# Tree Size

Returns the number of stored keys.

### Syntax

#### Macro form

```c
size_t TREE_FUNC(TREE_SET_TYPE(AVL, int), Size)(const TREE_SET_TYPE(AVL, int) *tree);
```

#### Direct form

```c
size_t Container_Tree_AVL_Set_int_Size(const Container_Tree_AVL_Set_int *tree);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `const TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |

---

### Return value

`size_t` key count.

---

### Remarks

Returns the number of stored keys.

---

### Example

```c
size_t count = TREE_FUNC(TREE_SET_TYPE(AVL, int), Size)(NULL, &tree);
```

---

# Tree Begin

Returns the smallest node for in-order traversal.

### Syntax

#### Macro form

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), Begin)(TREE_SET_TYPE(AVL, int) *tree);
```

#### Direct form

```c
Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_Begin(Container_Tree_AVL_Set_int *tree);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Returns the smallest node for in-order traversal. The end sentinel is NULL. Iterator/node references should not be used after removal or destruction.

---

### Example

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), Begin)(NULL, &tree);
```

---

# Tree End

Returns NULL, the past-end iterator.

### Syntax

#### Macro form

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), End)(TREE_SET_TYPE(AVL, int) *tree);
```

#### Direct form

```c
Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_End(Container_Tree_AVL_Set_int *tree);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Returns NULL, the past-end iterator. The end sentinel is NULL. Iterator/node references should not be used after removal or destruction.

---

### Example

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), End)(NULL, &tree);
```

---

# Tree Next

Returns the in-order successor of a node.

### Syntax

#### Macro form

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), Next)(TREE_NODE(TREE_SET_TYPE(AVL, int)) *node);
```

#### Direct form

```c
Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_Next(Container_Tree_AVL_Set_int_Node *node);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_NODE(TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `node` | `int)) *node` | Tree or node pointer. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Returns the in-order successor of a node. The end sentinel is NULL. Iterator/node references should not be used after removal or destruction.

---

### Example

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) *node = TREE_FUNC(TREE_SET_TYPE(AVL, int), Begin)(&tree);
TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), Next)(NULL, node);
```

---

# Tree Prev

Returns the in-order predecessor of a node.

### Syntax

#### Macro form

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), Prev)(TREE_NODE(TREE_SET_TYPE(AVL, int)) *node);
```

#### Direct form

```c
Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_Prev(Container_Tree_AVL_Set_int_Node *node);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `TREE_NODE(TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `node` | `int)) *node` | Tree or node pointer. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Returns the in-order predecessor of a node. The end sentinel is NULL. Iterator/node references should not be used after removal or destruction.

---

### Example

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) *node = TREE_FUNC(TREE_SET_TYPE(AVL, int), Begin)(&tree);
TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), Prev)(NULL, node);
```

---

# Tree ConstBegin

Returns the first node with read-only access.

### Syntax

#### Macro form

```c
const TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), ConstBegin)(const TREE_SET_TYPE(AVL, int) *tree);
```

#### Direct form

```c
const Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_ConstBegin(const Container_Tree_AVL_Set_int *tree);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `const TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Returns the first node with read-only access. The end sentinel is NULL. Iterator/node references should not be used after removal or destruction.

---

### Example

```c
const TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), ConstBegin)(NULL, &tree);
```

---

# Tree ConstEnd

Returns the read-only NULL past-end sentinel.

### Syntax

#### Macro form

```c
const TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), ConstEnd)(const TREE_SET_TYPE(AVL, int) *tree);
```

#### Direct form

```c
const Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_ConstEnd(const Container_Tree_AVL_Set_int *tree);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `const TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `tree` | `int) *tree` | Tree or node pointer. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Returns the read-only NULL past-end sentinel. The end sentinel is NULL. Iterator/node references should not be used after removal or destruction.

---

### Example

```c
const TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), ConstEnd)(NULL, &tree);
```

---

# Tree ConstNext

Returns a read-only successor node.

### Syntax

#### Macro form

```c
const TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), ConstNext)(const TREE_NODE(TREE_SET_TYPE(AVL, int)) *node);
```

#### Direct form

```c
const Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_ConstNext(const Container_Tree_AVL_Set_int_Node *node);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `const TREE_NODE(TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `node` | `int)) *node` | Tree or node pointer. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Returns a read-only successor node. The end sentinel is NULL. Iterator/node references should not be used after removal or destruction.

---

### Example

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) *node = TREE_FUNC(TREE_SET_TYPE(AVL, int), Begin)(&tree);
const TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), ConstNext)(NULL, node);
```

---

# Tree ConstPrev

Returns a read-only predecessor node.

### Syntax

#### Macro form

```c
const TREE_NODE(TREE_SET_TYPE(AVL, int)) * TREE_FUNC(TREE_SET_TYPE(AVL, int), ConstPrev)(const TREE_NODE(TREE_SET_TYPE(AVL, int)) *node);
```

#### Direct form

```c
const Container_Tree_AVL_Set_int_Node * Container_Tree_AVL_Set_int_ConstPrev(const Container_Tree_AVL_Set_int_Node *node);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `AVL` | `const TREE_NODE(TREE_SET_TYPE(AVL` | Key or value supplied by caller. |
| `node` | `int)) *node` | Tree or node pointer. |

---

### Return value

A typed node pointer, or NULL when there is no matching node.

---

### Remarks

Returns a read-only predecessor node. The end sentinel is NULL. Iterator/node references should not be used after removal or destruction.

---

### Example

```c
TREE_NODE(TREE_SET_TYPE(AVL, int)) *node = TREE_FUNC(TREE_SET_TYPE(AVL, int), Begin)(&tree);
const TREE_NODE(TREE_SET_TYPE(AVL, int)) * result = TREE_FUNC(TREE_SET_TYPE(AVL, int), ConstPrev)(NULL, node);
```
