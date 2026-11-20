# Memory

O módulo **Memory** fornece alocação de memoria, operaciones con bytes, interfaces de alocadores, arenas, pools, un heap de ancho fijo, streams binarios acotados, un collector de trazas, famílias de smart pointers y bloques de memoria de ancho fijo. Las operaciones que pueden fallar retornan `OPSTATUS` y utilizan parámetros de salida propriedade del chamador. `Arena`, `Pool`, `Heap` y `Stream` deshabilitan explícitamente sus function tables de objeto. Utilize llamadas directas `MODULE_FUNC(Memory, Type, Method)` para los objetos Memory en toda esta guía; esta forma directa sigue siendo válida independientemente de que un encabezado habilite una function table.

## Cabeçalhos

~~~c
#include "Cosmeron/Core/Memory/Memory.h"
~~~

El encabezado agregado incluye:

| Encabezado | Recurso |
| --- | --- |
| `Operations.h` | Copia, movimiento, alocação, puesta a cero e igualdad de trechos de bytes. |
| `Alloc.h` | Asignación, redimensionamiento y liberación de memoria sin tipo. |
| `Allocator.h` | Descriptor de alocador con borrado de tipo y capacidades. |
| `../Algorithm/Swap.h` | Troca de bytes y arrays expuesto en el módulo Memory. |
| `Arena.h` | Asignación lineal desde un buffer o una reserva de respaldo propia. |
| `Pool.h` | Pool de elementos de tamanho fijo. |
| `Heap.h` | Heap basado en páginas y clases de tamanho. |
| `Stream.h` | Stream acotado de lectura/escritura con operaciones de bits y endian-aware. |
| `Collector.h` | Recolector de basura por trazado y fachada global del collector. |
| `Pointer/Pointer.h` | Wrappers de ponteiros unique, shared, weak, atomic e intrusive. |

`Block.h` es un encabezado separado para bloques de memoria de ancho fijo y no está incluido por `Memory.h`; inclúyelo directamente cuando sea necesario.

---

# Visão geral

Memory cubre varias estrategias distintas de alocação y armazenamento: alocação sin tipo para bloques individuales, operaciones de bytes para trechos propriedade del chamador, arenas y pools para alocação por ámbito o de tamanho fijo, un heap basado en páginas, streams acotados para E/S binaria y un collector de trazas o smart pointers para propriedade administrada. Escolha el recurso más específico que coincida con el modelo de vida útil y propriedade, en lugar de tratar todos los alocadores como intercambiables.

---

# Referência da API

## Alocação sem tipo

Cabeçalho: `Alloc.h`. Paquete: `Memory_Alloc`.

| Función | Firma | Descrição |
| --- | --- | --- |
| `Bytes` | `(void *outPointerObject, size_t size)` | Asigna una cantidad de bytes y escribe el ponteiro resultante mediante el parámetro de salida. |
| `Array` | `(void *outPointerObject, size_t count, size_t elementSize)` | Asigna una cantidad de elementos con multiplicación de tamanho comprobada. |
| `Zero.Bytes` | `(void *outPointerObject, size_t size)` | Asigna bytes inicializados en cero. |
| `Zero.Array` | `(void *outPointerObject, size_t count, size_t elementSize)` | Asigna un array inicializado en cero. |
| `Realloc.Bytes` | `(void *inoutPointerObject, size_t newSize)` | Redimensiona la alocação y actualiza el ponteiro. |
| `Realloc.Array` | `(void *inoutPointerObject, size_t count, size_t elementSize)` | Redimensiona la alocação para una cantidad de elementos. |
| `Free` | `(void *inoutPointerObject)` | Libera la alocação y limpia el ponteiro. |

El primer argumento de las funciones de alocação y realocação es la dirección de una variable ponteiro, pasada como `void *`. El alocador escribe el nuevo ponteiro en esa variable. No pases el ponteiro de la alocação en sí en lugar de la dirección de la variable ponteiro.

~~~c
int *values = NULL;

OPSTATUS status = MODULE_FUNC(Memory, Alloc, Array)(
    &values, 16U, sizeof(*values));

if (status == STATUS_CONST(SUCCESS)) {
    /* use values */
    MODULE_FUNC(Memory, Alloc, Free)(&values);
}
~~~

`Memory_Free(pointerLvalue)` es una macro de conveniencia alrededor de `Memory_Alloc_Free` que recibe un lvalue de ponteiro. Es un auxiliar de limpieza estilo sentencia; se prefiere la operación del paquete cuando se necesita su estado.

## Operações com bytes

Cabeçalho: `Operations.h`. Paquete: `Memory_Operations`.

| Función | Contrato |
| --- | --- |
| `Copy(destination, source, size)` | Copia un trecho de bytes; origen y destino no deben solaparse. |
| `Copy.Array(destination, source, count, elementSize)` | Copia un array tras comprobar la multiplicación del tamanho. |
| `Move(destination, source, size)` | Mueve un trecho de bytes y permite sobreposição. |
| `Move.Array(destination, source, count, elementSize)` | Mueve un array tras comprobar la multiplicación del tamanho. |
| `Set(destination, value, size)` | Rellena bytes con el byte menos significativo del valor entero. |
| `Zero(destination, size)` | Pone a cero un trecho de bytes. |
| `Zero.Array(destination, count, elementSize)` | Pone a cero un array tras comprobar la multiplicación del tamanho. |
| `Equals(left, right, size)` | Retorna si los trechos de bytes son iguales. |

~~~c
MODULE_FUNC(Memory, Operations, Copy)(destination, source, byteCount);
MODULE_FUNC(Memory, Operations, Move)(destination, source, byteCount);
~~~

Escolha `Copy` solo para intervalos que no se solapan; utiliza `Move` cuando puedan solaparse. `Set` repite un valor de byte y no es un constructor de arrays tipados. Utilize inicialización tipada explícita cuando la representación del elemento no sea un patrón de bytes repetido.

### Troca

Cabeçalho: `Core/Algorithm/Swap.h`. Este encabezado declara su API pública bajo el namespace del módulo Memory.

| Función | Descrição |
| --- | --- |
| `Swap.Bytes(left, right, size)` | Intercambia dos intervalos de bytes que no se solapan. |
| `Swap.Array(left, right, count, elementSize)` | Intercambia dos arrays con multiplicación de tamanho comprobada. |
| Especializaciones de template tipado | Generan procedimientos de intercambio de valores tipados para un tipo C seleccionado. |

Swap requiere intervalos válidos y no sobrepostos. La implementación trata los ponteiros idénticos y las solicitudes de tamanho cero como casos sin efecto. No sustituye a `Move` cuando hay sobreposição.

## Interface Allocator

Cabeçalho: `Allocator.h`.

`MODULE_TYPE(Memory, Allocator)` es un valor de alocador con borrado de tipo que contiene una tabla de operaciones, contexto y flags de capacidad. El paquete `Allocator` proporciona `Default`, `IsValid`, `Alloc` (incluido `Aligned`), `Calloc`, `Realloc` y `Free`.

Utilize un alocador cuando un componente deba aceptar una política de memoria elegida por el chamador sin depender de una implementación concreta. El valor Allocator no posee el contexto externo detrás de sus ponteiros de función; ese contexto debe sobrevivir a su uso.

## Arena

Cabeçalho: `Arena.h`. Tipo: `MODULE_TYPE(Memory, Arena)`.

La alocação Arena es útil para grupos de objetos de vida corta que pueden liberarse juntos. La arena almacena un buffer, capacidad y deslocamento actual.

| Método | Descrição |
| --- | --- |
| `Create(capacity)` | Crea armazenamento de arena con la capacidad solicitada. |
| `Create.Buffer(buffer, capacity)` | Inicializa una arena sobre armazenamento proporcionado por el chamador. |
| `Alloc.Bytes`, `Alloc.Aligned` | Asigna bytes con alineación opcional. |
| `Alloc.Array`, `Alloc.ArrayAligned` | Asigna una cantidad de elementos con alineación opcional. |
| `Save(outMark)` | Escribe el deslocamento actual en una marca de salida. |
| `Restore(mark)` | Restaura un deslocamento registrado previamente. |
| `Contains(pointer)` | Verifica si un ponteiro está dentro del buffer de la arena. |
| `Reset()` | Restablece el deslocamento al inicio. |
| `Destroy()` | Libera el armazenamento propio y limpia el estado de la arena. |
| `AsAllocator()` | Retorna una interfaz de alocador respaldada por la arena. |

~~~c
MODULE_TYPE(Memory, Arena) arena = {0};

OPSTATUS status = MODULE_FUNC(Memory, Arena, Create)(&arena, 4096U);
if (status == STATUS_CONST(SUCCESS)) {
    void *memory = NULL;
    status = MODULE_FUNC(Memory, Arena, Alloc, Bytes)(&arena, &memory, 128U);
    (void)MODULE_FUNC(Memory, Arena, Destroy)(&arena);
}
~~~

La memoria devuelta por una arena sigue siendo válida hasta que la arena se reinicia, se restaura más allá de la alocação, se destruye o su buffer de respaldo deja de ser válido. No liberes alocaciones individuales de arena con el alocador sin tipo.

## Pool

Cabeçalho: `Pool.h`. Tipo: `MODULE_TYPE(Memory, Pool)`.

Un pool administra una cantidad fija de elementos del mismo tamanho.

| Método | Descrição |
| --- | --- |
| `Create(elementSize, elementCount)` | Crea armazenamento para un tamanho y cantidad de elementos fijos. |
| `Create.Buffer(buffer, bufferSize, elementSize)` | Inicializa un pool sobre memoria propriedade del chamador. |
| `Alloc(outPointer)` | Adquiere un elemento. |
| `Free(pointer)` / `Free.Pointer(pointerLvalue)` | Retorna un elemento al pool. |
| `Reset()` | Marca todos los elementos como disponibles de nuevo. |
| `Destroy()` | Libera el armazenamento de respaldo propio. |
| `Contains(pointer)` | Verifica si un ponteiro pertenece al pool. |
| `AsAllocator()` | Expone el pool mediante la interfaz Allocator. |

Las alocaciones Pool tienen el tamanho y contrato de alineación configurados. El pool no realiza seguimiento de constructores/destructores de objetos definidos por el usuario; combínalo con traits de Types cuando se requiera un ciclo de vida no trivial.

## Heap

Cabeçalho: `Heap.h`. Tipo: `MODULE_TYPE(Memory, Heap)`.

Heap es un alocador basado en páginas y clases de tamanho. Expone creación/destrucción, políticas de alocação, liberación, bloqueo/recorrido de páginas y un adaptador genérico de alocador.

La tabla de políticas actual incluye `MANUAL`, `CONSERVATIVE`, `TYPED`, `ATOMIC`, `IMMUTABLE`, `TRACED` y `PINNED`. Son valores de política de alocação, no garantías generales de sincronización para los objetos almacenados en el heap.

| Método | Descrição |
| --- | --- |
| `Create(initialPages)` / `Destroy()` | Inicia o desmonta el estado del heap. |
| `Alloc` | Asigna utilizando una política seleccionada y un descriptor de tipo opcional. |
| `Alloc.Small` / `Alloc.Large` | Rutas explícitas de alocação pequeña/grande. |
| `FreeCell(pointer)` | Libera una celda del heap. |
| `BlacklistPage(pageIndex)` | Impide que una página participe en la alocação normal. |
| `SweepPage(page, outIsPageEmpty)` | Recorre una página e informa si está vacía. |
| `IsBlacklisted(pageIndex)` | Verifica si un índice de página está bloqueado. |
| `LookupEntry(pointer)` | Busca la entrada del heap asociada a un ponteiro. |
| `ResolvePointer(interiorPointer, outBasePointer, outSize, outPolicy)` | Resuelve un ponteiro interior a los metadatos de su alocação. |
| `Contains(pointer)` | Verifica si un ponteiro pertenece a este heap. |
| `LazySweepClass(sizeClassIndex)` | Realiza un recorrido diferido para una clase de tamanho. |
| `AsAllocator()` | Retorna un adaptador Allocator. |

Heap es un alocador concreto. Utilize `Allocator` al pasarlo a un componente genérico. Sigue los contratos de los métodos Heap para los parámetros de política y descriptor; las funciones de alocação sin tipo no son intercambiables con la alocação administrada por Collector.

## Stream binario

Cabeçalho: `Stream.h`. Tipo: `MODULE_TYPE(Memory, Stream)`.

Un Stream es una vista acotada sobre un buffer proporcionado por el chamador. Admite inicialización de solo lectura y escritura, búsqueda de posición, saltos, alineación a byte, lecturas/escrituras tipadas, operaciones de enteros con endianess y acceso a nivel de bit.

| Grupo | Métodos |
| --- | --- |
| Inicialización | `Init.Read(buffer, size)`, `Init.Write(buffer, capacity)` |
| Posición | `Seek(offset, origin)`, `Skip(count)`, `Rewind()`, `AlignToByte()` |
| Estado | `Remaining`, `RemainingBits`, `Position`, `BitPosition`, `Capacity`, `IsAtEnd`, `IsReadOnly`, `HasRemaining(count)`, `HasRemainingBits(bitCount)`, `Data`, `Current` |
| Substream | `Slice(size, outSubStream)` |
| Leitura | `Read.Slice`, `Read.Bytes`, `Read.Bits`, `Read.UInt8`/`Int8`, `UInt16LE`/`BE`, `Int16LE`/`BE`, `UInt32LE`/`BE`, `Int32LE`/`BE`, `UInt64LE`/`BE`, `Int64LE`/`BE` |
| Inspección sin avanzar | `Peek.Bytes`, `Peek.UInt8` |
| Escrita | `Write.Bytes`, `Write.Bits` y las variantes de enteros/endianess correspondientes. |

~~~c
MODULE_TYPE(Memory, Stream) stream = {0};
uint32_t value = 0U;

OPSTATUS status = MODULE_FUNC(Memory, Stream, Init, Read)(
    &stream, input, inputSize);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Memory, Stream, Read, UInt32BE)(&stream, &value);
}
~~~

La endianess se selecciona por el nombre de la operación y es independiente del orden de bytes del host. El stream nunca amplía su buffer de respaldo. Las lecturas y escrituras validan la posición actual, el tamanho solicitado y la capacidad disponible.

## Collector

Cabeçalho: `Collector.h`.

Collector combina alocação Heap con seguimiento de raíces, trazado, pausa/reanudación, soporte de finalizadores y una interfaz global de conveniencia.

### Collector por instancia

Tipo: `MODULE_TYPE(Memory, Collector)`.

| Método | Descrição |
| --- | --- |
| `Create(initialHeapPages, stackBottom)` / `Destroy()` | Inicializa o destruye un collector independiente. |
| `Alloc`, `Alloc.Typed`, `Alloc.Atomic` | Asigna armazenamento sin tipo, consciente de descriptores o atómico. |
| `SetFinalizer(pointer, finalizer)` | Asocia un callback finalizador a una alocação. |
| `PinRoot(rootPointerAddress)` / `UnpinRoot(rootPointerAddress)` | Registra o elimina el registro de la dirección de una ranura de ponteiro raíz. |
| `Collect()` | Ejecuta un ciclo de coleta. |
| `Pause()` / `Resume()` | Suspende o reanuda temporalmente la coleta. |
| `AsAllocator()` | Expone Collector mediante la interfaz Allocator. |

### Collector global

El paquete `GlobalCollector` proporciona `Startup(stackBottom)`, `Shutdown()`, `DefaultCollector()`, las operaciones correspondientes de alocação/finalizador/raíz, `Collect`, `Pause`/`Resume`, `LiveBytes` y `Contains`.

La API de raíces recibe la **dirección de una variable ponteiro raíz**, no la dirección de la alocação. Fija y libera la misma ranura raíz, y mantén esa ranura válida mientras esté registrada. El argumento stack-bottom debe coincidir con el contrato de límites de pila de la plataforma/thread. Los finalizadores pueden ejecutar código del usuario; la coleta no equivale a llamar a `free` para una única alocação.

## Famílias de smart pointers

Cabeçalho: `Pointer/Pointer.h`.

El agregado proporciona famílias generadas para:

| Familia | Modelo de propriedade |
| --- | --- |
| `Unique` | Un único handle proprietário exclusivo. |
| `Shared` | Propriedade compartida con conteo de referencias. |
| `Weak` | Referencia no propietaria a un bloque de control Shared. |
| `AtomicShared` | Operaciones de conteo de referencias Shared con actualizaciones atómicas del control. |
| `AtomicWeak` | Operaciones de referencias débiles atómicas. |
| `Intrusive` | Conteo de referencias almacenado en los campos de control del objeto del usuario. |

Los métodos generados principales son:

| Familia | Construcción y propriedade | Acceso y estado |
| --- | --- | --- |
| `Unique` | `FromPointer`, `Alloc`, `AllocZero`, `Reset`, `Move`, `Release`, `Free` | `IsValid`, `Get`, `GetMutable`, `Swap` |
| `Shared` | `From.Pointer`, `From.Weak` (promoción), `Alloc`, `Alloc.Zero`, `Reset`, `Reset.WithPointer`, `Free` | `Get`, `Get.Mutable`, `UseCount`, `Unique`, `Swap` |
| `Weak` | `Clone`, `Move`, `Reset`, `Free` | `IsExpired`, `IsValid`, `UseCount`, `Swap` |
| `AtomicShared` | Operaciones `From`, `Alloc`, `Reset`, `Free` similares a Shared | `Get`, `Get.Mutable`, `UseCount`, `Unique` |
| `AtomicWeak` | Operaciones `Clone`, `Move`, `Reset`, `Free` similares a Weak | `IsExpired`, `IsValid`, `UseCount` |
| `Intrusive` | `FromPointer`, `FromExistingPointer`, `Clone`, `Move`, `Reset`, `Release`, `Free` | `Get`, `GetMutable`, `UseCount`, `Unique`, `Swap` |

El sufijo exacto del tipo y los parámetros de instanciación proceden de las tablas de tipos de las famílias de ponteiros y de los templates `.def`; no presupongas que cualquier tipo C arbitrario tiene una especialización integrada. Cada familia tiene su propio contrato de propriedade, y `Free`/`Reset` no son intercambiables entre famílias.

Un ponteiro Weak no posee el objeto. Promuévelo a un ponteiro Shared antes de acceder al objeto y verifica el estado devuelto porque el objeto puede haber expirado. El conteo atómico de referencias no hace que el objeto apuntado sea seguro para threads.

## Block de ancho fijo

Cabeçalho: `Block.h` (incluir directamente; no forma parte de `Memory.h`).

Block genera objetos de ancho fijado en compilación respaldados por limbs `uint64_t`. Proporciona limpiar/establecer todo, operaciones bit a bit `And`/`Or`/`Xor`/`Not`, deslocamentos, rotaciones, establecer/limpiar/invertir/comprobar bits, recuento de bits, predicados any/all/none e igualdad. Utilízalo para máscaras de ancho fijo y valores binarios que no requieren redimensionamiento dinámico.

---

# Contratos y reglas de propriedade

- Todo ponteiro devuelto por un alocador debe liberarse mediante la misma familia de alocadores que lo creó.
- `Alloc` y `Realloc` reciben la dirección de una variable ponteiro; pasa `&pointer`.
- `Copy` requiere intervalos sin sobreposição; `Move` permite sobreposição.
- Arena y Pool pueden exponer modos de buffer propriedade del chamador; no liberes esos bufferes mediante el alocador sin tipo.
- Stream toma prestado su buffer y nunca lo posee ni lo amplía.
- Las raíces de Collector son direcciones de ranuras de ponteiro, no direcciones de objetos sin procesar.
- La semántica de propriedade de smart pointers depende de la familia; un ponteiro sin tipo devuelto por un accessor no implica transferencia de propriedade.
- Verifica cada `OPSTATUS` antes de utilizar un ponteiro de salida o asumir que se adquirió un recurso.

## Módulos relacionados

- [Types](Types.md) — descriptores de ciclo de vida y operaciones con borrado de tipo.
- [Condition](Condition.md) — convenciones de estado y error.
- [Algorithm](Algorithm.md) — contratos de comparación e intercambio.
- [Bit](Bit.md) — operaciones de bits y bitsets de tamanho fijo.

## Cabeçalhos de origen

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
