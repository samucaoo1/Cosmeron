# Memory

El módulo **Memory** proporciona asignación de memoria, operaciones con bytes, interfaces de asignadores, arenas, pools, un heap de ancho fijo, streams binarios acotados, un collector de trazas, familias de smart pointers y bloques de memoria de ancho fijo. Las operaciones que pueden fallar devuelven `OPSTATUS` y utilizan parámetros de salida propiedad del llamador. `Arena`, `Pool`, `Heap` y `Stream` deshabilitan explícitamente sus function tables de objeto. Utiliza llamadas directas `MODULE_FUNC(Memory, Type, Method)` para los objetos Memory en toda esta guía; esta forma directa sigue siendo válida independientemente de que un encabezado habilite una function table.

## Encabezados

~~~c
#include "Cosmeron/Core/Memory/Memory.h"
~~~

El encabezado agregado incluye:

| Encabezado | Recurso |
| --- | --- |
| `Operations.h` | Copia, movimiento, asignación, puesta a cero e igualdad de tramos de bytes. |
| `Alloc.h` | Asignación, redimensionamiento y liberación de memoria sin tipo. |
| `Allocator.h` | Descriptor de asignador con borrado de tipo y capacidades. |
| `../Algorithm/Swap.h` | Intercambio de bytes y arrays expuesto en el módulo Memory. |
| `Arena.h` | Asignación lineal desde un búfer o una reserva de respaldo propia. |
| `Pool.h` | Pool de elementos de tamaño fijo. |
| `Heap.h` | Heap basado en páginas y clases de tamaño. |
| `Stream.h` | Stream acotado de lectura/escritura con operaciones de bits y endian-aware. |
| `Collector.h` | Recolector de basura por trazado y fachada global del collector. |
| `Pointer/Pointer.h` | Wrappers de punteros unique, shared, weak, atomic e intrusive. |

`Block.h` es un encabezado separado para bloques de memoria de ancho fijo y no está incluido por `Memory.h`; inclúyelo directamente cuando sea necesario.

---

# Visión general

Memory cubre varias estrategias distintas de asignación y almacenamiento: asignación sin tipo para bloques individuales, operaciones de bytes para tramos propiedad del llamador, arenas y pools para asignación por ámbito o de tamaño fijo, un heap basado en páginas, streams acotados para E/S binaria y un collector de trazas o smart pointers para propiedad administrada. Elige el recurso más específico que coincida con el modelo de vida útil y propiedad, en lugar de tratar todos los asignadores como intercambiables.

---

# Referencia de la API

## Asignación sin tipo

Encabezado: `Alloc.h`. Paquete: `Memory_Alloc`.

| Función | Firma | Descripción |
| --- | --- | --- |
| `Bytes` | `(void *outPointerObject, size_t size)` | Asigna una cantidad de bytes y escribe el puntero resultante mediante el parámetro de salida. |
| `Array` | `(void *outPointerObject, size_t count, size_t elementSize)` | Asigna una cantidad de elementos con multiplicación de tamaño comprobada. |
| `Zero.Bytes` | `(void *outPointerObject, size_t size)` | Asigna bytes inicializados en cero. |
| `Zero.Array` | `(void *outPointerObject, size_t count, size_t elementSize)` | Asigna un array inicializado en cero. |
| `Realloc.Bytes` | `(void *inoutPointerObject, size_t newSize)` | Redimensiona la asignación y actualiza el puntero. |
| `Realloc.Array` | `(void *inoutPointerObject, size_t count, size_t elementSize)` | Redimensiona la asignación para una cantidad de elementos. |
| `Free` | `(void *inoutPointerObject)` | Libera la asignación y limpia el puntero. |

El primer argumento de las funciones de asignación y reasignación es la dirección de una variable puntero, pasada como `void *`. El asignador escribe el nuevo puntero en esa variable. No pases el puntero de la asignación en sí en lugar de la dirección de la variable puntero.

~~~c
int *values = NULL;

OPSTATUS status = MODULE_FUNC(Memory, Alloc, Array)(
    &values, 16U, sizeof(*values));

if (status == STATUS_CONST(SUCCESS)) {
    /* use values */
    MODULE_FUNC(Memory, Alloc, Free)(&values);
}
~~~

`Memory_Free(pointerLvalue)` es una macro de conveniencia alrededor de `Memory_Alloc_Free` que recibe un lvalue de puntero. Es un auxiliar de limpieza estilo sentencia; se prefiere la operación del paquete cuando se necesita su estado.

## Operaciones con bytes

Encabezado: `Operations.h`. Paquete: `Memory_Operations`.

| Función | Contrato |
| --- | --- |
| `Copy(destination, source, size)` | Copia un tramo de bytes; origen y destino no deben solaparse. |
| `Copy.Array(destination, source, count, elementSize)` | Copia un array tras comprobar la multiplicación del tamaño. |
| `Move(destination, source, size)` | Mueve un tramo de bytes y permite solapamiento. |
| `Move.Array(destination, source, count, elementSize)` | Mueve un array tras comprobar la multiplicación del tamaño. |
| `Set(destination, value, size)` | Rellena bytes con el byte menos significativo del valor entero. |
| `Zero(destination, size)` | Pone a cero un tramo de bytes. |
| `Zero.Array(destination, count, elementSize)` | Pone a cero un array tras comprobar la multiplicación del tamaño. |
| `Equals(left, right, size)` | Devuelve si los tramos de bytes son iguales. |

~~~c
MODULE_FUNC(Memory, Operations, Copy)(destination, source, byteCount);
MODULE_FUNC(Memory, Operations, Move)(destination, source, byteCount);
~~~

Elige `Copy` solo para rangos que no se solapan; utiliza `Move` cuando puedan solaparse. `Set` repite un valor de byte y no es un constructor de arrays tipados. Utiliza inicialización tipada explícita cuando la representación del elemento no sea un patrón de bytes repetido.

### Intercambio

Encabezado: `Core/Algorithm/Swap.h`. Este encabezado declara su API pública bajo el namespace del módulo Memory.

| Función | Descripción |
| --- | --- |
| `Swap.Bytes(left, right, size)` | Intercambia dos rangos de bytes que no se solapan. |
| `Swap.Array(left, right, count, elementSize)` | Intercambia dos arrays con multiplicación de tamaño comprobada. |
| Especializaciones de template tipado | Generan procedimientos de intercambio de valores tipados para un tipo C seleccionado. |

Swap requiere rangos válidos y no solapados. La implementación trata los punteros idénticos y las solicitudes de tamaño cero como casos sin efecto. No sustituye a `Move` cuando hay solapamiento.

## Interfaz Allocator

Encabezado: `Allocator.h`.

`MODULE_TYPE(Memory, Allocator)` es un valor de asignador con borrado de tipo que contiene una tabla de operaciones, contexto y flags de capacidad. El paquete `Allocator` proporciona `Default`, `IsValid`, `Alloc` (incluido `Aligned`), `Calloc`, `Realloc` y `Free`.

Utiliza un asignador cuando un componente deba aceptar una política de memoria elegida por el llamador sin depender de una implementación concreta. El valor Allocator no posee el contexto externo detrás de sus punteros de función; ese contexto debe sobrevivir a su uso.

## Arena

Encabezado: `Arena.h`. Tipo: `MODULE_TYPE(Memory, Arena)`.

La asignación Arena es útil para grupos de objetos de vida corta que pueden liberarse juntos. La arena almacena un búfer, capacidad y desplazamiento actual.

| Método | Descripción |
| --- | --- |
| `Create(capacity)` | Crea almacenamiento de arena con la capacidad solicitada. |
| `Create.Buffer(buffer, capacity)` | Inicializa una arena sobre almacenamiento proporcionado por el llamador. |
| `Alloc.Bytes`, `Alloc.Aligned` | Asigna bytes con alineación opcional. |
| `Alloc.Array`, `Alloc.ArrayAligned` | Asigna una cantidad de elementos con alineación opcional. |
| `Save(outMark)` | Escribe el desplazamiento actual en una marca de salida. |
| `Restore(mark)` | Restaura un desplazamiento registrado previamente. |
| `Contains(pointer)` | Comprueba si un puntero está dentro del búfer de la arena. |
| `Reset()` | Restablece el desplazamiento al inicio. |
| `Destroy()` | Libera el almacenamiento propio y limpia el estado de la arena. |
| `AsAllocator()` | Devuelve una interfaz de asignador respaldada por la arena. |

~~~c
MODULE_TYPE(Memory, Arena) arena = {0};

OPSTATUS status = MODULE_FUNC(Memory, Arena, Create)(&arena, 4096U);
if (status == STATUS_CONST(SUCCESS)) {
    void *memory = NULL;
    status = MODULE_FUNC(Memory, Arena, Alloc, Bytes)(&arena, &memory, 128U);
    (void)MODULE_FUNC(Memory, Arena, Destroy)(&arena);
}
~~~

La memoria devuelta por una arena sigue siendo válida hasta que la arena se reinicia, se restaura más allá de la asignación, se destruye o su búfer de respaldo deja de ser válido. No liberes asignaciones individuales de arena con el asignador sin tipo.

## Pool

Encabezado: `Pool.h`. Tipo: `MODULE_TYPE(Memory, Pool)`.

Un pool administra una cantidad fija de elementos del mismo tamaño.

| Método | Descripción |
| --- | --- |
| `Create(elementSize, elementCount)` | Crea almacenamiento para un tamaño y cantidad de elementos fijos. |
| `Create.Buffer(buffer, bufferSize, elementSize)` | Inicializa un pool sobre memoria propiedad del llamador. |
| `Alloc(outPointer)` | Adquiere un elemento. |
| `Free(pointer)` / `Free.Pointer(pointerLvalue)` | Devuelve un elemento al pool. |
| `Reset()` | Marca todos los elementos como disponibles de nuevo. |
| `Destroy()` | Libera el almacenamiento de respaldo propio. |
| `Contains(pointer)` | Comprueba si un puntero pertenece al pool. |
| `AsAllocator()` | Expone el pool mediante la interfaz Allocator. |

Las asignaciones Pool tienen el tamaño y contrato de alineación configurados. El pool no realiza seguimiento de constructores/destructores de objetos definidos por el usuario; combínalo con traits de Types cuando se requiera un ciclo de vida no trivial.

## Heap

Encabezado: `Heap.h`. Tipo: `MODULE_TYPE(Memory, Heap)`.

Heap es un asignador basado en páginas y clases de tamaño. Expone creación/destrucción, políticas de asignación, liberación, bloqueo/recorrido de páginas y un adaptador genérico de asignador.

La tabla de políticas actual incluye `MANUAL`, `CONSERVATIVE`, `TYPED`, `ATOMIC`, `IMMUTABLE`, `TRACED` y `PINNED`. Son valores de política de asignación, no garantías generales de sincronización para los objetos almacenados en el heap.

| Método | Descripción |
| --- | --- |
| `Create(initialPages)` / `Destroy()` | Inicia o desmonta el estado del heap. |
| `Alloc` | Asigna utilizando una política seleccionada y un descriptor de tipo opcional. |
| `Alloc.Small` / `Alloc.Large` | Rutas explícitas de asignación pequeña/grande. |
| `FreeCell(pointer)` | Libera una celda del heap. |
| `BlacklistPage(pageIndex)` | Impide que una página participe en la asignación normal. |
| `SweepPage(page, outIsPageEmpty)` | Recorre una página e informa si está vacía. |
| `IsBlacklisted(pageIndex)` | Comprueba si un índice de página está bloqueado. |
| `LookupEntry(pointer)` | Busca la entrada del heap asociada a un puntero. |
| `ResolvePointer(interiorPointer, outBasePointer, outSize, outPolicy)` | Resuelve un puntero interior a los metadatos de su asignación. |
| `Contains(pointer)` | Comprueba si un puntero pertenece a este heap. |
| `LazySweepClass(sizeClassIndex)` | Realiza un recorrido diferido para una clase de tamaño. |
| `AsAllocator()` | Devuelve un adaptador Allocator. |

Heap es un asignador concreto. Utiliza `Allocator` al pasarlo a un componente genérico. Sigue los contratos de los métodos Heap para los parámetros de política y descriptor; las funciones de asignación sin tipo no son intercambiables con la asignación administrada por Collector.

## Stream binario

Encabezado: `Stream.h`. Tipo: `MODULE_TYPE(Memory, Stream)`.

Un Stream es una vista acotada sobre un búfer proporcionado por el llamador. Admite inicialización de solo lectura y escritura, búsqueda de posición, saltos, alineación a byte, lecturas/escrituras tipadas, operaciones de enteros con endianess y acceso a nivel de bit.

| Grupo | Métodos |
| --- | --- |
| Inicialización | `Init.Read(buffer, size)`, `Init.Write(buffer, capacity)` |
| Posición | `Seek(offset, origin)`, `Skip(count)`, `Rewind()`, `AlignToByte()` |
| Estado | `Remaining`, `RemainingBits`, `Position`, `BitPosition`, `Capacity`, `IsAtEnd`, `IsReadOnly`, `HasRemaining(count)`, `HasRemainingBits(bitCount)`, `Data`, `Current` |
| Substream | `Slice(size, outSubStream)` |
| Lectura | `Read.Slice`, `Read.Bytes`, `Read.Bits`, `Read.UInt8`/`Int8`, `UInt16LE`/`BE`, `Int16LE`/`BE`, `UInt32LE`/`BE`, `Int32LE`/`BE`, `UInt64LE`/`BE`, `Int64LE`/`BE` |
| Inspección sin avanzar | `Peek.Bytes`, `Peek.UInt8` |
| Escritura | `Write.Bytes`, `Write.Bits` y las variantes de enteros/endianess correspondientes. |

~~~c
MODULE_TYPE(Memory, Stream) stream = {0};
uint32_t value = 0U;

OPSTATUS status = MODULE_FUNC(Memory, Stream, Init, Read)(
    &stream, input, inputSize);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Memory, Stream, Read, UInt32BE)(&stream, &value);
}
~~~

La endianess se selecciona por el nombre de la operación y es independiente del orden de bytes del host. El stream nunca amplía su búfer de respaldo. Las lecturas y escrituras validan la posición actual, el tamaño solicitado y la capacidad disponible.

## Collector

Encabezado: `Collector.h`.

Collector combina asignación Heap con seguimiento de raíces, trazado, pausa/reanudación, soporte de finalizadores y una interfaz global de conveniencia.

### Collector por instancia

Tipo: `MODULE_TYPE(Memory, Collector)`.

| Método | Descripción |
| --- | --- |
| `Create(initialHeapPages, stackBottom)` / `Destroy()` | Inicializa o destruye un collector independiente. |
| `Alloc`, `Alloc.Typed`, `Alloc.Atomic` | Asigna almacenamiento sin tipo, consciente de descriptores o atómico. |
| `SetFinalizer(pointer, finalizer)` | Asocia un callback finalizador a una asignación. |
| `PinRoot(rootPointerAddress)` / `UnpinRoot(rootPointerAddress)` | Registra o elimina el registro de la dirección de una ranura de puntero raíz. |
| `Collect()` | Ejecuta un ciclo de recolección. |
| `Pause()` / `Resume()` | Suspende o reanuda temporalmente la recolección. |
| `AsAllocator()` | Expone Collector mediante la interfaz Allocator. |

### Collector global

El paquete `GlobalCollector` proporciona `Startup(stackBottom)`, `Shutdown()`, `DefaultCollector()`, las operaciones correspondientes de asignación/finalizador/raíz, `Collect`, `Pause`/`Resume`, `LiveBytes` y `Contains`.

La API de raíces recibe la **dirección de una variable puntero raíz**, no la dirección de la asignación. Fija y libera la misma ranura raíz, y mantén esa ranura válida mientras esté registrada. El argumento stack-bottom debe coincidir con el contrato de límites de pila de la plataforma/hilo. Los finalizadores pueden ejecutar código del usuario; la recolección no equivale a llamar a `free` para una única asignación.

## Familias de smart pointers

Encabezado: `Pointer/Pointer.h`.

El agregado proporciona familias generadas para:

| Familia | Modelo de propiedad |
| --- | --- |
| `Unique` | Un único handle propietario exclusivo. |
| `Shared` | Propiedad compartida con conteo de referencias. |
| `Weak` | Referencia no propietaria a un bloque de control Shared. |
| `AtomicShared` | Operaciones de conteo de referencias Shared con actualizaciones atómicas del control. |
| `AtomicWeak` | Operaciones de referencias débiles atómicas. |
| `Intrusive` | Conteo de referencias almacenado en los campos de control del objeto del usuario. |

Los métodos generados principales son:

| Familia | Construcción y propiedad | Acceso y estado |
| --- | --- | --- |
| `Unique` | `FromPointer`, `Alloc`, `AllocZero`, `Reset`, `Move`, `Release`, `Free` | `IsValid`, `Get`, `GetMutable`, `Swap` |
| `Shared` | `From.Pointer`, `From.Weak` (promoción), `Alloc`, `Alloc.Zero`, `Reset`, `Reset.WithPointer`, `Free` | `Get`, `Get.Mutable`, `UseCount`, `Unique`, `Swap` |
| `Weak` | `Clone`, `Move`, `Reset`, `Free` | `IsExpired`, `IsValid`, `UseCount`, `Swap` |
| `AtomicShared` | Operaciones `From`, `Alloc`, `Reset`, `Free` similares a Shared | `Get`, `Get.Mutable`, `UseCount`, `Unique` |
| `AtomicWeak` | Operaciones `Clone`, `Move`, `Reset`, `Free` similares a Weak | `IsExpired`, `IsValid`, `UseCount` |
| `Intrusive` | `FromPointer`, `FromExistingPointer`, `Clone`, `Move`, `Reset`, `Release`, `Free` | `Get`, `GetMutable`, `UseCount`, `Unique`, `Swap` |

El sufijo exacto del tipo y los parámetros de instanciación proceden de las tablas de tipos de las familias de punteros y de los templates `.def`; no presupongas que cualquier tipo C arbitrario tiene una especialización integrada. Cada familia tiene su propio contrato de propiedad, y `Free`/`Reset` no son intercambiables entre familias.

Un puntero Weak no posee el objeto. Promuévelo a un puntero Shared antes de acceder al objeto y comprueba el estado devuelto porque el objeto puede haber expirado. El conteo atómico de referencias no hace que el objeto apuntado sea seguro para hilos.

## Block de ancho fijo

Encabezado: `Block.h` (incluir directamente; no forma parte de `Memory.h`).

Block genera objetos de ancho fijado en compilación respaldados por limbs `uint64_t`. Proporciona limpiar/establecer todo, operaciones bit a bit `And`/`Or`/`Xor`/`Not`, desplazamientos, rotaciones, establecer/limpiar/invertir/comprobar bits, recuento de bits, predicados any/all/none e igualdad. Utilízalo para máscaras de ancho fijo y valores binarios que no requieren redimensionamiento dinámico.

---

# Contratos y reglas de propiedad

- Todo puntero devuelto por un asignador debe liberarse mediante la misma familia de asignadores que lo creó.
- `Alloc` y `Realloc` reciben la dirección de una variable puntero; pasa `&pointer`.
- `Copy` requiere rangos sin solapamiento; `Move` permite solapamiento.
- Arena y Pool pueden exponer modos de búfer propiedad del llamador; no liberes esos búferes mediante el asignador sin tipo.
- Stream toma prestado su búfer y nunca lo posee ni lo amplía.
- Las raíces de Collector son direcciones de ranuras de puntero, no direcciones de objetos sin procesar.
- La semántica de propiedad de smart pointers depende de la familia; un puntero sin tipo devuelto por un accessor no implica transferencia de propiedad.
- Comprueba cada `OPSTATUS` antes de utilizar un puntero de salida o asumir que se adquirió un recurso.

## Módulos relacionados

- [Types](Types.md) — descriptores de ciclo de vida y operaciones con borrado de tipo.
- [Condition](Condition.md) — convenciones de estado y error.
- [Algorithm](Algorithm.md) — contratos de comparación e intercambio.
- [Bit](Bit.md) — operaciones de bits y bitsets de tamaño fijo.

## Encabezados de origen

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
