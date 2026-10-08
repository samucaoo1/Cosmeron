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
