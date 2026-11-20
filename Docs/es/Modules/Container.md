# Contenedores (Container)

El módulo **Container** proporciona estructuras de datos C generadas para almacenamiento ordenado, almacenamiento enlazado, búsqueda asociativa, árboles, grafos, iteración y colecciones de bits. Pertenece a **Modules**. Su API pública se basa en especializaciones tipadas `MODULE_TYPED`, funciones de paquete `MODULE_FUNC` y métodos de objeto `OBJECT_CALL`.

## Encabezados

~~~c
#include "Cosmeron/Modules/Container/Container.h"
~~~

El encabezado agregado incluye DynamicBitset, Iterator, Array, Linked, Tree, Hash y Graph. Se pueden incluir encabezados de paquetes individuales cuando una unidad de traducción solo necesite un subconjunto del módulo.

---

# Visión general

| Familia | Encabezado agregado | Estructuras |
| --- | --- | --- |
| Array | `Array/Array.h` | Vector, Queue, Stack, Ring, Span, Heap |
| Linked | `Linked/Linked.h` | Forward, List, Deque, Queue, Stack, Ring y tipos de nodos |
| Hash | `Hash/Hash.h` | Set y Map |
| Tree | `Tree/Tree.h` | BST, AVL, RedBlack y BTree; variantes Set y Map |
| Graph | `Graph/Graph.h` | Grafo dirigido con vértices y aristas tipados |
| Iterator | `Iterator/Iterator.h` | Iteradores Forward, Bidirectional, Contiguous, Circular y Stream |
| Bitset | `Bitset/DynamicBitset.h` | Bitset de tamaño en tiempo de ejecución |

Los templates Array y Linked se instancian a partir de la tabla de tipos de contenedores ordenados. Hash y Tree utilizan tablas de tipos de clave/valor adecuadas a sus contratos de almacenamiento y comparación. Utiliza los sufijos declarados por esas tablas; una especialización generada no es un tipo genérico en tiempo de ejecución.

---

# Referencia de la API

## Convención de llamadas y ciclo de vida

Un tipo de objeto generado se nombra mediante `MODULE_TYPED(Module, Family, Suffix)`. Las funciones a nivel de paquete se acceden con `MODULE_FUNC(Module, Family, Suffix, Function)`. Después de la creación, los métodos del objeto se llaman mediante `OBJECT_CALL` (o `OBJECT_CALL_0` para un método sin argumentos).

~~~c
MODULE_TYPED(Array, Vector, Int32) values;
OPSTATUS status = OBJECT_CALL(&values, Create, 4U);

if (status == STATUS_CONST(SUCCESS)) {
    (void)OBJECT_CALL(&values, PushBack, 10);
    (void)OBJECT_CALL(&values, PushBack, 20);

    size_t count = OBJECT_CALL_0(&values, Count);
    (void)count;

    OBJECT_CALL_0(&values, Destroy);
}
~~~

`Create` inicializa un objeto y adquiere el almacenamiento auxiliar que necesita. `Dispose` es la operación de limpieza que devuelve un estado; `Destroy` es un procedimiento práctico que invoca la liberación. Comprueba `OPSTATUS` en las operaciones que pueden fallar. No utilices un objeto después de haberlo liberado o destruido.

La mayoría de los contenedores almacenan elementos por valor. Si el tipo del elemento es un puntero, el contenedor almacena ese valor de puntero; no asume automáticamente la propiedad ni libera el objeto apuntado. Los punteros devueltos por accesores e iteradores pueden quedar invalidados por borrado, reserva, realocación, rehash o destrucción, según la estructura.

---

## Contenedores Array

El agregado Array es `Array/Array.h`.

### Vector

Encabezado: `Array/Vector.h`.

Vector es un array contiguo de crecimiento dinámico. Es apropiado cuando importan el acceso indexado y la iteración eficiente para la caché.

| Grupo | Métodos |
| --- | --- |
| Construcción | `Create(initialCapacity)`, `FromBuffer(buffer, count)`, `Clone(outClone)` |
| Almacenamiento | `Reserve(newCapacity)`, `ShrinkToFit`, `Resize(newSize, fillValue)`, `Clear`, `Dispose`, `Destroy` |
| Acceso | `At`, `ConstAt`, `Get`, `Set`, `Front`, `Back`, `Data`, `ConstData` |
| Operaciones en extremos | `PushBack`, `PopBack`, `PushFront`, `PopFront` |
| Edición | `Insert(position, value)`, `Erase(position, count)`, `RemoveAt`, `SwapRemove` |
| Estado | `IsEmpty`, `Empty`, `Size`, `Count`, `Capacity` |

`At` devuelve un puntero al elemento mediante un parámetro de salida; `Get` copia un elemento a un valor de salida. `Set` reemplaza un elemento. `SwapRemove` elimina un elemento moviendo el último a su posición, por lo que no conserva el orden de los elementos restantes.

### Queue

Encabezado: `Array/Queue.h`.

La Queue de array es una estructura FIFO respaldada por un array dinámico. Proporciona `Create`, `Compact`, `Reserve`, `ShrinkToFit`, `Clear`, `Push`/`Enqueue`, `Pop`/`Dequeue`, `Front`, `Back`, `Peek`, `IsEmpty`, `Size`, `Count` y `Capacity`.

Utiliza Queue para programar trabajo en orden first-in, first-out. `Peek` lee el siguiente valor sin eliminarlo; `Pop`/`Dequeue` elimina el valor del frente y lo escribe mediante el puntero de salida proporcionado.

### Stack

Encabezado: `Array/Stack.h`.

La Stack de array es LIFO. Sus métodos principales son `Create`, `Reserve`, `ShrinkToFit`, `Clear`, `Push`, `Pop`, `Top`, `Peek`, `IsEmpty`, `Size`, `Count` y `Capacity`.

Utiliza Stack para trabajo anidado, análisis sintáctico, historial de deshacer y recorridos en profundidad. `Top` proporciona acceso al elemento almacenado; `Peek` copia el valor a un parámetro de salida sin eliminarlo.

### Ring

Encabezado: `Array/Ring.h`.

Ring es un búfer circular de capacidad fija. Proporciona `Create(capacity)`, `FromBuffer`, `Dispose`, `Destroy`, `Clear`, `Push`/`PushBack`, `Pop`/`PopFront`, `Peek`, `Overwrite`, `Front`, `Back`, `At`, `IsEmpty`, `IsFull`, `Size`, `Count`, `Capacity` y `Available`.

Las operaciones `Read`/`Write` y `GetReadSpan`/`GetWriteSpan` con `AdvanceRead`/`AdvanceWrite` exponen partes contiguas del búfer circular para transferencias en bloque. `Overwrite` puede devolver un elemento descartado mediante un puntero de salida; utiliza `Push` normal cuando no se deban descartar los datos existentes.

### Span

Encabezado: `Array/Span.h`.

Span es una vista no propietaria sobre un búfer contiguo existente. Proporciona `FromBuffer`, `At`, `ConstAt`, `Get`, `Set`, `Front`, `Back`, `Data`, `ConstData`, `Size`, `Count`, `SizeBytes` e `IsEmpty`.

Span no asigna, realoca ni libera el búfer de origen. Es útil para pasar un rango acotado a una API sin copiarlo; su vida útil no debe superar la del almacenamiento subyacente.

### Heap de prioridad

Encabezado: `Array/Heap.h`.

Este es un heap binario de prioridad, distinto de `Core/Memory/Heap.h`. Admite la construcción de min-heap y max-heap mediante `CreateMin` y `CreateMax`, además de `FromBuffer`, `Dispose`, `Destroy`, `Clear`, `Reserve`, `ShrinkToFit`, `Heapify`, `Push`, `Pop`, `Peek`, `Top`, `Replace`, `PushPop`, `Size`, `Count` y `Capacity`.

Utilízalo para colas de prioridad y selección repetida del elemento de mayor o menor prioridad. Su closure de comparación determina el orden; utiliza el tipo de comparador generado que espera el template en lugar de convertir un puntero a función no relacionado.

---

## Contenedores Linked

El agregado Linked es `Linked/Linked.h`. Los contenedores enlazados utilizan nodos asignados por separado y son adecuados para cargas de trabajo en las que las inserciones y eliminaciones en extremos o posiciones conocidas importan más que el almacenamiento contiguo.

| Encabezado | Estructura | Operaciones principales |
| --- | --- | --- |
| `Linked/Forward.h` | Cadena simplemente enlazada | `Create`, `Clear`, `PushFront`, `PushBack`, `PopFront`, `Front`, `Back`, `Head`, `Tail`, `Size`, `Count`, `IsEmpty` |
| `Linked/List.h` | Lista doblemente enlazada | `Create`, `Clear`, `PushFront`, `PushBack`, `PopFront`, `PopBack`, `Front`, `Back`, `Head`, `Tail`, `Size`, `Count`, `IsEmpty` |
| `Linked/Deque.h` | Cola de doble extremo | `Create`, `Clear`, `PushFront`, `PushBack`, `PopFront`, `PopBack`, `Front`, `Back`, `Size`, `Count`, `IsEmpty` |
| `Linked/Queue.h` | Cola FIFO enlazada | `Create`, `Clear`, `Push`/`Enqueue`, `Pop`/`Dequeue`, `Peek`, `Front`, `Back`, `Size`, `Count`, `IsEmpty` |
| `Linked/Stack.h` | Pila LIFO enlazada | `Create`, `Clear`, `Push`, `Pop`, `Peek`, `Top`, `Size`, `Count`, `IsEmpty` |
| `Linked/Ring.h` | Estructura circular enlazada | Operaciones de creación, inserción/eliminación y recorrido definidas por el template Ring |

Los templates enlazados también proporcionan acceso orientado a nodos y closures visitor. Utiliza el tipo de closure `Visitor` declarado por el template correspondiente al recorrer elementos. Los nodos y punteros a elementos son administrados por el contenedor; no liberes manualmente un nodo devuelto por un accesor.

Las estructuras enlazadas siguen la misma convención de ciclo de vida que los contenedores de array: crea o inicializa antes de usar y después llama a `Dispose` o `Destroy`. `Clear` elimina los elementos y mantiene utilizable el objeto contenedor.

---

## Contenedores Hash

El agregado Hash es `Hash/Hash.h` e incluye Set y Map. Ambos utilizan búsqueda basada en hash; no son contenedores ordenados.

### Hash Set

Encabezado: `Hash/Set.h`.

Set almacena claves únicas. Su interfaz incluye `Create`, `Clear`, `Dispose`, `Destroy`, `Rehash`, `Reserve`, `Insert`, `Remove`, `Find`, `Contains`, `IsEmpty`, `Size`, `Count` y `Capacity`. Su iterador expone `IsValid`, `HasNext`, `Reset`, `Get`, `ConstGet`, `Key`, `Value`, `StepNext` y `Next`.

Utiliza Set para comprobar pertenencia y unicidad. La igualdad de claves y el hashing deben ser coherentes: las claves iguales deben producir el mismo valor hash. El rehash puede invalidar iteradores y punteros a elementos de la tabla.

### Hash Map

Encabezado: `Hash/Map.h`.

Map asocia una clave con un valor. Proporciona `Create`, `Clear`, `Dispose`, `Destroy`, `Rehash`, `Reserve`, `Insert`, `Upsert`, `Remove`, `Find`, `Contains`, `IsEmpty`, `Size`, `Count` y `Capacity`. Su iterador proporciona acceso a clave/valor y recorrido hacia delante.

`Insert` sigue el contrato de inserción de una clave nueva; `Upsert` inserta o reemplaza el valor de una clave existente. `Find(key, &valuePointer)` devuelve la dirección de un valor almacenado cuando lo encuentra.

~~~c
MODULE_TYPED(Hash, Map, Int32_Int32) map;
OPSTATUS status = OBJECT_CALL(&map, Create);

if (status == STATUS_CONST(SUCCESS)) {
    (void)OBJECT_CALL(&map, Upsert, 1, 100);

    int32_t *found = NULL;
    if (OBJECT_CALL(&map, Find, 1, &found) == STATUS_CONST(SUCCESS) && found)
        printf("%d\n", (int)*found);

    OBJECT_CALL_0(&map, Destroy);
}
~~~

---

## Árboles

El agregado Tree es `Tree/Tree.h`. Incluye implementaciones de árbol binario de búsqueda (BST), AVL, Red-Black y B-Tree. Cada implementación tiene variantes Set y Map generadas a partir de combinaciones de tipos de clave y valor.

Las operaciones compartidas incluyen `Create`, `Clear`, `Dispose`, `Destroy`, `Insert`, `Remove`, `Find`, `Contains`, `IsEmpty`, `Size` y `Count`. Según la implementación, las operaciones adicionales incluyen `Min`/`Max`, `MinKey`/`MaxKey`, `Height` y recorrido mediante iteradores hacia delante o bidireccionales. Las variantes Map también exponen `Upsert`.

| Estructura | Característica | Úsala cuando |
| --- | --- | --- |
| BST | Árbol binario de búsqueda simple; su forma depende del orden de inserción | Basta con un árbol ordenado básico y la forma de entrada está controlada |
| AVL | Árbol binario de búsqueda equilibrado por altura | La altura acotada y el rendimiento de búsqueda son prioritarios |
| RedBlack | Árbol de búsqueda equilibrado rojo-negro | Se necesita un árbol ordenado equilibrado con balanceo favorable a las actualizaciones |
| BTree | Árbol de búsqueda multiway con grado mínimo configurable | Importan la ramificación de los nodos y la altura del árbol, incluido el almacenamiento orientado a páginas |

Un Set almacena claves; un Map almacena pares clave/valor. Utiliza el tipo de iterador y el contrato de ordenación de la estructura seleccionada. Las modificaciones estructurales pueden invalidar iteradores y punteros a valores almacenados.

---

## Graph

Encabezado: `Graph/Graph.h`.

Graph genera objetos tipados de vértices y aristas, así como un contenedor de grafo. Sus operaciones públicas incluyen `Create`, `Clear`, `Dispose`, `Destroy`, `AddVertex`, `RemoveVertex`, `AddEdge`, `RemoveEdge`, `GetVertex`, `HasEdge`, `OutDegree`, `VertexCount` y `EdgeCount`. Los iteradores de vértices y aristas proporcionan comprobaciones de validez, acceso y recorrido.

La API actual de Graph modela aristas dirigidas. Una conexión bidireccional debe representarse añadiendo ambas direcciones. Ten en cuenta los cambios de índices cuando se eliminen vértices y comprueba el estado devuelto antes de utilizar un índice o puntero producido mediante un parámetro de salida.

---

## Iteradores

El agregado Iterator es `Iterator/Iterator.h` e incluye templates de iteradores tipados Stream, Contiguous, Circular, Forward y Bidirectional. Sus operaciones dependen de la categoría de recorrido subyacente; los conceptos compartidos incluyen `IsValid`, `HasNext`, `HasPrev`, `Remaining`, `Reset`, `Get`, `ConstGet`, `Value`, `Set`, `Next`, `Prev`, `StepNext`, `StepPrev`, `StepValue`, `StepPtr`, `Seek`, `Index` y `Advance`.

- Utiliza Contiguous con almacenamiento respaldado por arrays y movimiento basado en índices.
- Utiliza Forward para recorridos en una dirección.
- Utiliza Bidirectional cuando el origen admita movimiento en ambas direcciones.
- Utiliza Circular cuando el recorrido dé la vuelta a la secuencia subyacente.
- Utiliza Stream cuando el recorrido esté controlado por un contrato de callback/estado de stream.

Por lo general, los iteradores toman prestado el contenedor o búfer. Destruir, aplicar rehash, redimensionar o modificar estructuralmente el origen puede invalidarlos.

---

## Bitset dinámico

Encabezado: `Bitset/DynamicBitset.h`.

`MODULE_TYPE(Bitset, Dynamic)` almacena una cantidad de bits seleccionada en tiempo de ejecución y utiliza un `Memory_Allocator` configurable. Sus métodos incluyen `Create`, `Create.WithAllocator`, `Dispose`, `Reserve`, `Resize`, `Clone`, `Clear`, `SetAll`, `And`, `Or`, `Xor`, `Not`, desplazamientos, operaciones de bit individual y rangos, `Count`, `Size`, `Capacity`, `Any`, `All`, `None`, `Equal`, `Is.Subset`, `Is.Disjoint`, `Intersects` y `Find.FirstSet`/`FirstZero`/`NextSet`.

~~~c
MODULE_TYPE(Bitset, Dynamic) bits = {0};
OPSTATUS status = MODULE_FUNC(Bitset, Dynamic, Create)(&bits, 128U);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Bitset, Dynamic, Bit, Set)(&bits, 7U);
    (void)MODULE_FUNC(Bitset, Dynamic, Dispose)(&bits);
}
~~~

`DynamicBitset.h` deshabilita su function table de objeto, por lo que debes invocar estos métodos directamente con `MODULE_FUNC(Bitset, Dynamic, Method)` en lugar de `OBJECT_CALL`. El bitset es propietario de los limbs asignados y debe liberarse. Un asignador personalizado debe permanecer válido durante toda la vida útil del bitset.

---

# Cómo elegir un contenedor

| Requisito | Familia recomendada |
| --- | --- |
| Almacenamiento contiguo indexado y acceso aleatorio | Array Vector |
| FIFO o LIFO con almacenamiento dinámico | Array Queue / Array Stack |
| Almacenamiento FIFO reutilizable y acotado | Array Ring |
| Vista de datos contiguos existentes sin copiarlos | Span |
| Selección repetida por prioridad | Array Heap |
| Inserción/eliminación enlazada frecuente | Linked List / Deque |
| Pertenencia y búsqueda por clave | Hash Set / Hash Map |
| Recorrido ordenado por clave | BST / AVL / RedBlack / BTree |
| Vértices y aristas dirigidas | Graph |
| Colección de bits de tamaño dinámico | Dynamic Bitset |

# Contratos y notas

- Utiliza `MODULE_TYPED` para tipos de objeto generados, `MODULE_FUNC` para funciones de paquete y `OBJECT_CALL` para métodos de objeto.
- `Create` y los métodos de mutación que pueden fallar devuelven `OPSTATUS`; compruébalo antes de acceder al objeto.
- `Destroy` es un procedimiento práctico de limpieza; utiliza `Dispose` cuando necesites el estado.
- Los contenedores almacenan valores C por valor, salvo que un tipo concreto proporcione explícitamente un wrapper de propiedad.
- No supongas que un contenedor es seguro para uso concurrente salvo que su propio encabezado documente la sincronización.

## Módulos relacionados

- [Algorithm](Algorithm.md) — closures de comparación utilizados por colas de prioridad y contenedores asociativos.
- [Bit](Bit.md) — operaciones de bits escalares y de tamaño fijo.
- [Memory](Memory.md) — asignadores utilizados por contenedores dinámicos y bitsets.
- [Module](Module.md) — modelo de declaración de objetos y paquetes tipados.
- [Types](Types.md) — especializaciones primitivas y traits de ciclo de vida.

## Encabezados de origen

- `Codespace/Cosmeron/Modules/Container/Container.h`
- `Codespace/Cosmeron/Modules/Container/Array/Array.h`
- `Codespace/Cosmeron/Modules/Container/Linked/Linked.h`
- `Codespace/Cosmeron/Modules/Container/Hash/Hash.h`
- `Codespace/Cosmeron/Modules/Container/Tree/Tree.h`
- `Codespace/Cosmeron/Modules/Container/Graph/Graph.h`
- `Codespace/Cosmeron/Modules/Container/Iterator/Iterator.h`
- `Codespace/Cosmeron/Modules/Container/Bitset/DynamicBitset.h`
