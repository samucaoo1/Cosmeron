# Sort

The **Sort** module provides generic C11 sorting, integer radix ordering, typed wrappers, and intentionally impractical algorithms for teaching. It is header-only and requires no additional link libraries.

```c
#include "Cosmeron/Modules/Sort/Sort.h"
```

---

# Overview

Sort operates on a contiguous array of elements, with a three-way comparator from [Algorithm](Algorithm.md). The direct and namespace-macro names designate the same functions when using the default Cosmeron prefix.

### Macro form

```c
OPSTATUS status = SORT_FUNC(Array)(values, count, sizeof values[0], CompareInt);
```

### Direct form

```c
OPSTATUS status = Sort_Array(values, count, sizeof values[0], CompareInt);
```

For the examples below, these comparison callbacks operate on real array element addresses:

```c
static CMPOUT CompareInt(const void *left, const void *right) {
    return COMPARISON_FUNC(int)(*(const int *)left, *(const int *)right);
}
static CMPOUT CompareIntContext(const void *left, const void *right,
                                void *context) {
    (void)context;
    return CompareInt(left, right);
}
```

These helpers are example functions defined by the application, not library APIs.

# Shared contracts

- All generic sorting functions accept `count`, `elementSize`, and an appropriate comparator; `elementSize` must be nonzero.
- A `NULL` array is accepted only for `count == 0`; valid comparators are still required even for empty arrays.
- `COMPARISON_TYPE(Comparator)` receives two addresses. `COMPARISON_TYPE(ContextComparator)` also receives a caller-provided context. Comparators should be consistent and return `COMPARISON_LOWER_CONST`, `COMPARISON_EQUAL_CONST`, or `COMPARISON_HIGHER_CONST`.
- `WithContext` variants permit instance-specific sorting without global state.
- General in-place algorithms do not allocate; merge/stable implementations allocate scratch storage unless you supply it explicitly.
- Invalid arguments and size multiplication overflow are reported with `OPSTATUS`. A successful status does not imply esoteric algorithms preserve all elements.
- These functions process memory; the caller owns the array and retains ownership after sorting.
- `SORT_TYPE(TOrder)` defines `SORT_CONST(ASCENDING)` and `SORT_CONST(DESCENDING)` for radix operations.

# API reference

## Function summary

| Operation | Package | Meaning |
| --- | --- | --- |
| `SORT_FUNC(Array)` | Array | Introsort padrão; delega a Intro. |
| `SORT_FUNC(Stable)` | Array | Merge Sort estável. |
| `SORT_FUNC(Insertion)` | Array | Insertion Sort in-place. |
| `SORT_FUNC(Heap)` | Array | Heap Sort in-place. |
| `SORT_FUNC(Intro)` | Array | Introsort com limite de profundidade. |
| `SORT_FUNC(Merge)` | Array | Merge Sort estável. |
| `SORT_FUNC(Bubble)` | Classical | Bubble Sort. |
| `SORT_FUNC(Cocktail)` | Classical | Cocktail shaker. |
| `SORT_FUNC(Comb)` | Classical | Comb Sort. |
| `SORT_FUNC(Gnome)` | Classical | Gnome Sort. |
| `SORT_FUNC(OddEven)` | Classical | Odd-even transposition sort. |
| `SORT_FUNC(Selection)` | Classical | Selection Sort. |
| `SORT_FUNC(DoubleSelection)` | Classical | Seleção simultânea de menor e maior. |
| `SORT_FUNC(Shell)` | Classical | Shell Sort com gaps. |
| `SORT_FUNC(Pancake)` | Classical | Pancake Sort por reversões de prefixos. |
| `SORT_FUNC(Quick)` | Classical | Quicksort clássico. |
| `SORT_FUNC(QuickThreeWay)` | Classical | Quicksort particionado em três faixas. |
| `SORT_FUNC(Stalin)` | Esoteric | Filtro de subsequência não decrescente. |
| `SORT_FUNC(Bogo)` | Esoteric | Embaralha o array inteiro e verifica ordenação. |
| `SORT_FUNC(Bozo)` | Esoteric | Troca pares aleatórios e verifica ordenação. |
| `SORT_FUNC(Stooge)` | Esoteric | Stooge Sort recursivo. |
| `SORT_FUNC(Slow)` | Esoteric | Slowsort recursivo. |
| `SORT_FUNC(Miracle)` | Esoteric | Verificação repetida sem modificar o array. |
| `SORT_FUNC(IsSorted)` | Utility | Consulta se já está ordenado. |
| `SORT_FUNC(Reverse)` | Utility | Inverte a sequência de elementos. |
| `SORT_FUNC(StableWithBuffer)` | Array | Merge Sort com buffer do chamador. |
| `SORT_FUNC(ArrayWithContext)` | Array | Introsort padrão; delega a Intro. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(StableWithContext)` | Array | Merge Sort estável. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(InsertionWithContext)` | Array | Insertion Sort in-place. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(HeapWithContext)` | Array | Heap Sort in-place. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(IntroWithContext)` | Array | Introsort com limite de profundidade. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(MergeWithContext)` | Array | Merge Sort estável. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(BubbleWithContext)` | Classical | Bubble Sort. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(CocktailWithContext)` | Classical | Cocktail shaker. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(CombWithContext)` | Classical | Comb Sort. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(GnomeWithContext)` | Classical | Gnome Sort. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(OddEvenWithContext)` | Classical | Odd-even transposition sort. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(SelectionWithContext)` | Classical | Selection Sort. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(DoubleSelectionWithContext)` | Classical | Seleção simultânea de menor e maior. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(ShellWithContext)` | Classical | Shell Sort com gaps. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(PancakeWithContext)` | Classical | Pancake Sort por reversões de prefixos. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(QuickWithContext)` | Classical | Quicksort clássico. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(QuickThreeWayWithContext)` | Classical | Quicksort particionado em três faixas. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(StalinWithContext)` | Esoteric | Filtro de subsequência não decrescente. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(BogoWithContext)` | Esoteric | Embaralha o array inteiro e verifica ordenação. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(BozoWithContext)` | Esoteric | Troca pares aleatórios e verifica ordenação. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(StoogeWithContext)` | Esoteric | Stooge Sort recursivo. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(SlowWithContext)` | Esoteric | Slowsort recursivo. Variante com contexto fornecido ao comparador. |
| `SORT_FUNC(StableWithBufferWithContext)` | Array | Merge Sort com buffer do chamador. Variante com contexto fornecido ao comparador. |

Integer radix API: `SORT_TYPED_FUNC(Radix, U32/I32/U64/I64)`, and `SORT_TYPED_FUNC(RadixWithBuffer, U32/I32/U64/I64)`. Typed user-defined wrappers are instantiated using `SORT_IMPLEMENT_ALL` (see below).

# Array package

# Array

Introsort padrão; delega a Intro.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Array)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Array(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n log n) no pior caso; instável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Array)(values, 4, sizeof values[0], CompareInt);
```

# Stable

Merge Sort estável.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Stable)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Stable(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Alocação temporária pode retornar `STATUS_CONST(OUT_OF_MEMORY)`.

### Remarks

O(n log n); aloca buffer temporário.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Stable)(values, 4, sizeof values[0], CompareInt);
```

# Insertion

Insertion Sort in-place.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Insertion)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Insertion(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²) no pior caso; estável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Insertion)(values, 4, sizeof values[0], CompareInt);
```

# Heap

Heap Sort in-place.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Heap)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Heap(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n log n) no pior caso; instável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Heap)(values, 4, sizeof values[0], CompareInt);
```

# Intro

Introsort com limite de profundidade.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Intro)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Intro(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n log n) no pior caso; instável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Intro)(values, 4, sizeof values[0], CompareInt);
```

# Merge

Merge Sort estável.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Merge)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Merge(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Alocação temporária pode retornar `STATUS_CONST(OUT_OF_MEMORY)`.

### Remarks

O(n log n); aloca buffer temporário.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Merge)(values, 4, sizeof values[0], CompareInt);
```

# StableWithBuffer

Merge Sort com buffer do chamador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(StableWithBuffer)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare, void *buffer, size_t bufferSize);
```

#### Direct form

```c
OPSTATUS Sort_StableWithBuffer(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare, void *buffer, size_t bufferSize);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Buffer insuficiente retorna `STATUS_CONST(INSUFFICIENT_SPACE)`.

### Remarks

Estável; buffer deve ser separado de data.

### Example

```c
int values[] = {4, 1, 3, 2};
unsigned char scratch[sizeof values];
OPSTATUS status = SORT_FUNC(StableWithBuffer)(values, 4, sizeof values[0], CompareInt, scratch, sizeof scratch);
```

# ArrayWithContext

Introsort padrão; delega a Intro. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(ArrayWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_ArrayWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n log n) no pior caso; instável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(ArrayWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# StableWithContext

Merge Sort estável. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(StableWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_StableWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Alocação temporária pode retornar `STATUS_CONST(OUT_OF_MEMORY)`.

### Remarks

O(n log n); aloca buffer temporário. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(StableWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# InsertionWithContext

Insertion Sort in-place. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(InsertionWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_InsertionWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²) no pior caso; estável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(InsertionWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# HeapWithContext

Heap Sort in-place. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(HeapWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_HeapWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n log n) no pior caso; instável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(HeapWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# IntroWithContext

Introsort com limite de profundidade. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(IntroWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_IntroWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n log n) no pior caso; instável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(IntroWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# MergeWithContext

Merge Sort estável. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(MergeWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_MergeWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Alocação temporária pode retornar `STATUS_CONST(OUT_OF_MEMORY)`.

### Remarks

O(n log n); aloca buffer temporário. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(MergeWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# StableWithBufferWithContext

Merge Sort com buffer do chamador. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(StableWithBufferWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context, void *buffer, size_t bufferSize);
```

#### Direct form

```c
OPSTATUS Sort_StableWithBufferWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context, void *buffer, size_t bufferSize);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Buffer insuficiente retorna `STATUS_CONST(INSUFFICIENT_SPACE)`.

### Remarks

Estável; buffer deve ser separado de data. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
unsigned char scratch[sizeof values];
OPSTATUS status = SORT_FUNC(StableWithBufferWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL, scratch, sizeof scratch);
```


# Classical package

All classic methods below have ordinary and `WithContext` versions. Algorithms listed as O(n²) are primarily educational or useful for very small inputs; choose `Array` or `Stable` for general-purpose sorting.

| Algorithm | Typical time | Stable |
| --- | --- | --- |
| Bubble | O(n²), O(n) best | Yes |
| Cocktail | O(n²), O(n) best | Yes |
| Comb | O(n²) worst | No |
| Gnome | O(n²), O(n) best | Yes |
| OddEven | O(n²), O(n) best | Yes |
| Selection | O(n²) | No |
| DoubleSelection | O(n²) | No |
| Shell | Gap-dependent | No |
| Pancake | O(n²) | No |
| Quick | O(n log n) average, O(n²) worst | No |
| QuickThreeWay | O(n log n) average, O(n²) worst | No |

# Bubble

Bubble Sort.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Bubble)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Bubble(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); estável; sem heap.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Bubble)(values, 4, sizeof values[0], CompareInt);
```

# Cocktail

Cocktail shaker.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Cocktail)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Cocktail(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); estável; sem heap.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Cocktail)(values, 4, sizeof values[0], CompareInt);
```

# Comb

Comb Sort.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Comb)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Comb(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²) pior caso; instável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Comb)(values, 4, sizeof values[0], CompareInt);
```

# Gnome

Gnome Sort.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Gnome)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Gnome(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); estável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Gnome)(values, 4, sizeof values[0], CompareInt);
```

# OddEven

Odd-even transposition sort.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(OddEven)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_OddEven(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); estável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(OddEven)(values, 4, sizeof values[0], CompareInt);
```

# Selection

Selection Sort.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Selection)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Selection(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); instável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Selection)(values, 4, sizeof values[0], CompareInt);
```

# DoubleSelection

Seleção simultânea de menor e maior.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(DoubleSelection)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_DoubleSelection(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); instável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(DoubleSelection)(values, 4, sizeof values[0], CompareInt);
```

# Shell

Shell Sort com gaps.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Shell)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Shell(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

Complexidade depende dos gaps; instável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Shell)(values, 4, sizeof values[0], CompareInt);
```

# Pancake

Pancake Sort por reversões de prefixos.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Pancake)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Pancake(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); instável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Pancake)(values, 4, sizeof values[0], CompareInt);
```

# Quick

Quicksort clássico.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Quick)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Quick(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n log n) médio; O(n²) pior caso; instável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Quick)(values, 4, sizeof values[0], CompareInt);
```

# QuickThreeWay

Quicksort particionado em três faixas.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(QuickThreeWay)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_QuickThreeWay(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n log n) médio; O(n²) pior caso; instável.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(QuickThreeWay)(values, 4, sizeof values[0], CompareInt);
```

# BubbleWithContext

Bubble Sort. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(BubbleWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_BubbleWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); estável; sem heap. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(BubbleWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# CocktailWithContext

Cocktail shaker. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(CocktailWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_CocktailWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); estável; sem heap. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(CocktailWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# CombWithContext

Comb Sort. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(CombWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_CombWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²) pior caso; instável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(CombWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# GnomeWithContext

Gnome Sort. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(GnomeWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_GnomeWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); estável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(GnomeWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# OddEvenWithContext

Odd-even transposition sort. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(OddEvenWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_OddEvenWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); estável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(OddEvenWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# SelectionWithContext

Selection Sort. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(SelectionWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_SelectionWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); instável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(SelectionWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# DoubleSelectionWithContext

Seleção simultânea de menor e maior. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(DoubleSelectionWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_DoubleSelectionWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); instável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(DoubleSelectionWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# ShellWithContext

Shell Sort com gaps. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(ShellWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_ShellWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

Complexidade depende dos gaps; instável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(ShellWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# PancakeWithContext

Pancake Sort por reversões de prefixos. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(PancakeWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_PancakeWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n²); instável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(PancakeWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# QuickWithContext

Quicksort clássico. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(QuickWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_QuickWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n log n) médio; O(n²) pior caso; instável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(QuickWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# QuickThreeWayWithContext

Quicksort particionado em três faixas. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(QuickThreeWayWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_QuickThreeWayWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

O(n log n) médio; O(n²) pior caso; instável. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(QuickThreeWayWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```


# Esoteric package

These algorithms are provided for study. `Stalin` discards values; `Bogo` and `Bozo` may exhaust their budget; `Stooge` and `Slow` reject large arrays; `Miracle` does not perform sorting. Do not select them for ordinary production sorting.

# Stalin

Filtro de subsequência não decrescente.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Stalin)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare, size_t *outCount);
```

#### Direct form

```c
OPSTATUS Sort_Stalin(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare, size_t *outCount);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.
- `outCount`: recebe o tamanho do prefixo retido (apenas em SUCCESS).

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

Perde elementos; outCount retorna tamanho válido. A operação conserva uma subsequência estável a partir do primeiro elemento; os elementos depois de `outCount` não formam parte do resultado e não devem ser usados como dados ordenados.

### Example

```c
int values[] = {4, 1, 3, 2};
size_t retained = 0;
OPSTATUS status = SORT_FUNC(Stalin)(values, 4, sizeof values[0], CompareInt, &retained);
```

# Bogo

Embaralha o array inteiro e verifica ordenação.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Bogo)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare, uint64_t seed, size_t maxSteps);
```

#### Direct form

```c
OPSTATUS Sort_Bogo(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare, uint64_t seed, size_t maxSteps);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.
- `outCount`: recebe o tamanho do prefixo retido (apenas em SUCCESS).
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Limite de trabalho sem atingir a ordenação retorna `STATUS_CONST(TIMEOUT)`.

### Remarks

Orçamento maxSteps; pode retornar TIMEOUT. Em TIMEOUT o array pode ter sido permutado; não há garantia de ordenação.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Bogo)(values, 4, sizeof values[0], CompareInt, UINT64_C(42), 10000);
```

# Bozo

Troca pares aleatórios e verifica ordenação.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Bozo)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare, uint64_t seed, size_t maxSteps);
```

#### Direct form

```c
OPSTATUS Sort_Bozo(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare, uint64_t seed, size_t maxSteps);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.
- `outCount`: recebe o tamanho do prefixo retido (apenas em SUCCESS).
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Limite de trabalho sem atingir a ordenação retorna `STATUS_CONST(TIMEOUT)`.

### Remarks

Orçamento maxSteps; pode retornar TIMEOUT. Em TIMEOUT o array pode ter sido permutado; não há garantia de ordenação.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Bozo)(values, 4, sizeof values[0], CompareInt, UINT64_C(42), 10000);
```

# Stooge

Stooge Sort recursivo.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Stooge)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Stooge(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.
- `outCount`: recebe o tamanho do prefixo retido (apenas em SUCCESS).
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Excesso de elementos retorna `STATUS_CONST(OUT_OF_RANGE)`.

### Remarks

Até 128 elementos; acima disso OUT_OF_RANGE sem alterar dados.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Stooge)(values, 4, sizeof values[0], CompareInt);
```

# Slow

Slowsort recursivo.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Slow)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare);
```

#### Direct form

```c
OPSTATUS Sort_Slow(void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.
- `outCount`: recebe o tamanho do prefixo retido (apenas em SUCCESS).
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Excesso de elementos retorna `STATUS_CONST(OUT_OF_RANGE)`.

### Remarks

Até 20 elementos; acima disso OUT_OF_RANGE sem alterar dados.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Slow)(values, 4, sizeof values[0], CompareInt);
```

# Miracle

Verificação repetida sem modificar o array.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Miracle)(const void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare, size_t maxChecks);
```

#### Direct form

```c
OPSTATUS Sort_Miracle(const void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare, size_t maxChecks);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.
- `outCount`: recebe o tamanho do prefixo retido (apenas em SUCCESS).
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.
- `maxChecks`: limite de verificações; não altera a entrada.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Limite de trabalho sem atingir a ordenação retorna `STATUS_CONST(TIMEOUT)`.

### Remarks

Retorna TIMEOUT se desordenado após maxChecks. O parâmetro data é const: esta função nunca efetua operações de troca.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Miracle)(values, 4, sizeof values[0], CompareInt, 100);
```

# StalinWithContext

Filtro de subsequência não decrescente. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(StalinWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context, size_t *outCount);
```

#### Direct form

```c
OPSTATUS Sort_StalinWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context, size_t *outCount);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.
- `outCount`: recebe o tamanho do prefixo retido (apenas em SUCCESS).

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

Perde elementos; outCount retorna tamanho válido. A operação conserva uma subsequência estável a partir do primeiro elemento; os elementos depois de `outCount` não formam parte do resultado e não devem ser usados como dados ordenados. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
size_t retained = 0;
OPSTATUS status = SORT_FUNC(StalinWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL, &retained);
```

# BogoWithContext

Embaralha o array inteiro e verifica ordenação. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(BogoWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context, uint64_t seed, size_t maxSteps);
```

#### Direct form

```c
OPSTATUS Sort_BogoWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context, uint64_t seed, size_t maxSteps);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Limite de trabalho sem atingir a ordenação retorna `STATUS_CONST(TIMEOUT)`.

### Remarks

Orçamento maxSteps; pode retornar TIMEOUT. Em TIMEOUT o array pode ter sido permutado; não há garantia de ordenação. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(BogoWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL, UINT64_C(42), 10000);
```

# BozoWithContext

Troca pares aleatórios e verifica ordenação. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(BozoWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context, uint64_t seed, size_t maxSteps);
```

#### Direct form

```c
OPSTATUS Sort_BozoWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context, uint64_t seed, size_t maxSteps);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Limite de trabalho sem atingir a ordenação retorna `STATUS_CONST(TIMEOUT)`.

### Remarks

Orçamento maxSteps; pode retornar TIMEOUT. Em TIMEOUT o array pode ter sido permutado; não há garantia de ordenação. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(BozoWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL, UINT64_C(42), 10000);
```

# StoogeWithContext

Stooge Sort recursivo. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(StoogeWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_StoogeWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Excesso de elementos retorna `STATUS_CONST(OUT_OF_RANGE)`.

### Remarks

Até 128 elementos; acima disso OUT_OF_RANGE sem alterar dados. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(StoogeWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```

# SlowWithContext

Slowsort recursivo. Variante com contexto fornecido ao comparador.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(SlowWithContext)(void *data, size_t count, size_t elementSize, COMPARISON_TYPE(ContextComparator) compare, void *context);
```

#### Direct form

```c
OPSTATUS Sort_SlowWithContext(void *data, size_t count, size_t elementSize, Algorithm_Comparison_ContextComparator compare, void *context);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador `COMPARISON_TYPE(ContextComparator)`; recebe ambos os elementos e `context`.
- `context`: ponteiro opaco passado inalterado ao callback; pode ser NULL.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`. Excesso de elementos retorna `STATUS_CONST(OUT_OF_RANGE)`.

### Remarks

Até 20 elementos; acima disso OUT_OF_RANGE sem alterar dados. O campo `context` não é armazenado globalmente.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(SlowWithContext)(values, 4, sizeof values[0], CompareIntContext, NULL);
```


# Utility package

# IsSorted

Consulta se já está ordenado.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(IsSorted)(const void *data, size_t count, size_t elementSize, COMPARISON_TYPE(Comparator) compare, bool *outSorted);
```

#### Direct form

```c
OPSTATUS Sort_IsSorted(const void *data, size_t count, size_t elementSize, Algorithm_Comparison_Comparator compare, bool *outSorted);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.
- `compare`: comparador de três vias; não pode ser NULL; deve impor uma ordem consistente.
- `buffer`: área de scratch não sobreposta ao array.
- `bufferSize`: capacidade do scratch **em bytes**, pelo menos `count * elementSize` quando `count >= 2`.
- `outCount`: recebe o tamanho do prefixo retido (apenas em SUCCESS).
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.
- `seed`: semente determinística local à chamada (zero é permitido).
- `maxSteps`: máximo de embaralhamentos ou trocas tentados.
- `maxChecks`: limite de verificações; não altera a entrada.
- `outSorted`: saída `bool *` obrigatória, modificada apenas em SUCCESS.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

Não modifica dados; resultado em outSorted.

### Example

```c
int values[] = {1, 2, 3};
bool sorted = false;
OPSTATUS status = SORT_FUNC(IsSorted)(values, 3, sizeof values[0], CompareInt, &sorted);
```

# Reverse

Inverte a sequência de elementos.

### Syntax

#### Macro form

```c
OPSTATUS SORT_FUNC(Reverse)(void *data, size_t count, size_t elementSize);
```

#### Direct form

```c
OPSTATUS Sort_Reverse(void *data, size_t count, size_t elementSize);
```

### Parameters

- `data`: ponteiro para o primeiro elemento do array; pode ser NULL quando `count` é zero.
- `count`: número de elementos.
- `elementSize`: tamanho de cada elemento em bytes, maior que zero.

### Return value

Retorna `STATUS_CONST(SUCCESS)` quando a operação se conclui. Argumentos inválidos retornam `STATUS_CONST(INVALID_ARGUMENT)` e a multiplicação `count * elementSize` inválida retorna `STATUS_CONST(ARITHMETIC_OVERFLOW)`.

### Remarks

Não recebe comparador; preserva todos os elementos.

### Example

```c
int values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_FUNC(Reverse)(values, 4, sizeof values[0]);
```


# Integer radix package

The signed variants order negatives correctly by transforming the sign bit. The algorithms are stable least-significant-digit radix passes using 8-bit buckets, independent of host endianness; both ascending and descending orders are available. Scratch storage is required and can be allocated by the library or provided by the caller. For the `WithBuffer` variants, the scratch must be a separate non-overlapping array of the same element type.

# Radix_U32

uint32_t integer radix sorting, allocating and freeing temporary working storage.

### Syntax

#### Macro form

```c
OPSTATUS SORT_TYPED_FUNC(Radix, U32)(uint32_t *data, size_t count, SORT_TYPE(TOrder) order);
```

#### Direct form

```c
OPSTATUS Sort_Radix_U32(uint32_t *data, size_t count, Sort_TOrder order);
```

### Parameters

- `data`: elements of type `uint32_t`; NULL is accepted for zero elements.
- `count`: number of elements, without overflowing `count * sizeof(uint32_t)`.
- `order`: `SORT_CONST(ASCENDING)` or `SORT_CONST(DESCENDING)`.

### Return value

Returns `STATUS_CONST(SUCCESS)` or `INVALID_ARGUMENT` for invalid order/array, `ARITHMETIC_OVERFLOW` for excessive count, or `OUT_OF_MEMORY` if scratch allocation fails.

### Remarks

The signed algorithm handles `INT_MIN` without signed subtraction or signed overflow. Radix operations do not require a comparison callback. A count smaller than two succeeds without needing scratch.

### Example

```c
uint32_t values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_TYPED_FUNC(Radix, U32)(values, 4, SORT_CONST(ASCENDING));
```

# RadixWithBuffer_U32

uint32_t integer radix sorting, using a caller-provided working array.

### Syntax

#### Macro form

```c
OPSTATUS SORT_TYPED_FUNC(RadixWithBuffer, U32)(uint32_t *data, size_t count, SORT_TYPE(TOrder) order, uint32_t *buffer, size_t bufferCount);
```

#### Direct form

```c
OPSTATUS Sort_RadixWithBuffer_U32(uint32_t *data, size_t count, Sort_TOrder order, uint32_t *buffer, size_t bufferCount);
```

### Parameters

- `data`: elements of type `uint32_t`; NULL is accepted for zero elements.
- `count`: number of elements, without overflowing `count * sizeof(uint32_t)`.
- `order`: `SORT_CONST(ASCENDING)` or `SORT_CONST(DESCENDING)`.
- `buffer`: separate `uint32_t` scratch array for `count >= 2`.
- `bufferCount`: scratch capacity measured in **elements** (at least `count`), not bytes.

### Return value

Returns `STATUS_CONST(SUCCESS)` or `INVALID_ARGUMENT` for invalid order/array, `ARITHMETIC_OVERFLOW` for excessive count, or `INSUFFICIENT_SPACE` for missing/small scratch.

### Remarks

The signed algorithm handles `INT_MIN` without signed subtraction or signed overflow. Radix operations do not require a comparison callback. A count smaller than two succeeds without needing scratch.

### Example

```c
uint32_t values[] = {4, 1, 3, 2};
uint32_t scratch[4];
OPSTATUS status = SORT_TYPED_FUNC(RadixWithBuffer, U32)(values, 4, SORT_CONST(ASCENDING), scratch, 4);
```

# Radix_I32

int32_t integer radix sorting, allocating and freeing temporary working storage.

### Syntax

#### Macro form

```c
OPSTATUS SORT_TYPED_FUNC(Radix, I32)(int32_t *data, size_t count, SORT_TYPE(TOrder) order);
```

#### Direct form

```c
OPSTATUS Sort_Radix_I32(int32_t *data, size_t count, Sort_TOrder order);
```

### Parameters

- `data`: elements of type `int32_t`; NULL is accepted for zero elements.
- `count`: number of elements, without overflowing `count * sizeof(int32_t)`.
- `order`: `SORT_CONST(ASCENDING)` or `SORT_CONST(DESCENDING)`.

### Return value

Returns `STATUS_CONST(SUCCESS)` or `INVALID_ARGUMENT` for invalid order/array, `ARITHMETIC_OVERFLOW` for excessive count, or `OUT_OF_MEMORY` if scratch allocation fails.

### Remarks

The signed algorithm handles `INT_MIN` without signed subtraction or signed overflow. Radix operations do not require a comparison callback. A count smaller than two succeeds without needing scratch.

### Example

```c
int32_t values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_TYPED_FUNC(Radix, I32)(values, 4, SORT_CONST(ASCENDING));
```

# RadixWithBuffer_I32

int32_t integer radix sorting, using a caller-provided working array.

### Syntax

#### Macro form

```c
OPSTATUS SORT_TYPED_FUNC(RadixWithBuffer, I32)(int32_t *data, size_t count, SORT_TYPE(TOrder) order, int32_t *buffer, size_t bufferCount);
```

#### Direct form

```c
OPSTATUS Sort_RadixWithBuffer_I32(int32_t *data, size_t count, Sort_TOrder order, int32_t *buffer, size_t bufferCount);
```

### Parameters

- `data`: elements of type `int32_t`; NULL is accepted for zero elements.
- `count`: number of elements, without overflowing `count * sizeof(int32_t)`.
- `order`: `SORT_CONST(ASCENDING)` or `SORT_CONST(DESCENDING)`.
- `buffer`: separate `int32_t` scratch array for `count >= 2`.
- `bufferCount`: scratch capacity measured in **elements** (at least `count`), not bytes.

### Return value

Returns `STATUS_CONST(SUCCESS)` or `INVALID_ARGUMENT` for invalid order/array, `ARITHMETIC_OVERFLOW` for excessive count, or `INSUFFICIENT_SPACE` for missing/small scratch.

### Remarks

The signed algorithm handles `INT_MIN` without signed subtraction or signed overflow. Radix operations do not require a comparison callback. A count smaller than two succeeds without needing scratch.

### Example

```c
int32_t values[] = {4, 1, 3, 2};
int32_t scratch[4];
OPSTATUS status = SORT_TYPED_FUNC(RadixWithBuffer, I32)(values, 4, SORT_CONST(ASCENDING), scratch, 4);
```

# Radix_U64

uint64_t integer radix sorting, allocating and freeing temporary working storage.

### Syntax

#### Macro form

```c
OPSTATUS SORT_TYPED_FUNC(Radix, U64)(uint64_t *data, size_t count, SORT_TYPE(TOrder) order);
```

#### Direct form

```c
OPSTATUS Sort_Radix_U64(uint64_t *data, size_t count, Sort_TOrder order);
```

### Parameters

- `data`: elements of type `uint64_t`; NULL is accepted for zero elements.
- `count`: number of elements, without overflowing `count * sizeof(uint64_t)`.
- `order`: `SORT_CONST(ASCENDING)` or `SORT_CONST(DESCENDING)`.

### Return value

Returns `STATUS_CONST(SUCCESS)` or `INVALID_ARGUMENT` for invalid order/array, `ARITHMETIC_OVERFLOW` for excessive count, or `OUT_OF_MEMORY` if scratch allocation fails.

### Remarks

The signed algorithm handles `INT_MIN` without signed subtraction or signed overflow. Radix operations do not require a comparison callback. A count smaller than two succeeds without needing scratch.

### Example

```c
uint64_t values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_TYPED_FUNC(Radix, U64)(values, 4, SORT_CONST(ASCENDING));
```

# RadixWithBuffer_U64

uint64_t integer radix sorting, using a caller-provided working array.

### Syntax

#### Macro form

```c
OPSTATUS SORT_TYPED_FUNC(RadixWithBuffer, U64)(uint64_t *data, size_t count, SORT_TYPE(TOrder) order, uint64_t *buffer, size_t bufferCount);
```

#### Direct form

```c
OPSTATUS Sort_RadixWithBuffer_U64(uint64_t *data, size_t count, Sort_TOrder order, uint64_t *buffer, size_t bufferCount);
```

### Parameters

- `data`: elements of type `uint64_t`; NULL is accepted for zero elements.
- `count`: number of elements, without overflowing `count * sizeof(uint64_t)`.
- `order`: `SORT_CONST(ASCENDING)` or `SORT_CONST(DESCENDING)`.
- `buffer`: separate `uint64_t` scratch array for `count >= 2`.
- `bufferCount`: scratch capacity measured in **elements** (at least `count`), not bytes.

### Return value

Returns `STATUS_CONST(SUCCESS)` or `INVALID_ARGUMENT` for invalid order/array, `ARITHMETIC_OVERFLOW` for excessive count, or `INSUFFICIENT_SPACE` for missing/small scratch.

### Remarks

The signed algorithm handles `INT_MIN` without signed subtraction or signed overflow. Radix operations do not require a comparison callback. A count smaller than two succeeds without needing scratch.

### Example

```c
uint64_t values[] = {4, 1, 3, 2};
uint64_t scratch[4];
OPSTATUS status = SORT_TYPED_FUNC(RadixWithBuffer, U64)(values, 4, SORT_CONST(ASCENDING), scratch, 4);
```

# Radix_I64

int64_t integer radix sorting, allocating and freeing temporary working storage.

### Syntax

#### Macro form

```c
OPSTATUS SORT_TYPED_FUNC(Radix, I64)(int64_t *data, size_t count, SORT_TYPE(TOrder) order);
```

#### Direct form

```c
OPSTATUS Sort_Radix_I64(int64_t *data, size_t count, Sort_TOrder order);
```

### Parameters

- `data`: elements of type `int64_t`; NULL is accepted for zero elements.
- `count`: number of elements, without overflowing `count * sizeof(int64_t)`.
- `order`: `SORT_CONST(ASCENDING)` or `SORT_CONST(DESCENDING)`.

### Return value

Returns `STATUS_CONST(SUCCESS)` or `INVALID_ARGUMENT` for invalid order/array, `ARITHMETIC_OVERFLOW` for excessive count, or `OUT_OF_MEMORY` if scratch allocation fails.

### Remarks

The signed algorithm handles `INT_MIN` without signed subtraction or signed overflow. Radix operations do not require a comparison callback. A count smaller than two succeeds without needing scratch.

### Example

```c
int64_t values[] = {4, 1, 3, 2};
OPSTATUS status = SORT_TYPED_FUNC(Radix, I64)(values, 4, SORT_CONST(ASCENDING));
```

# RadixWithBuffer_I64

int64_t integer radix sorting, using a caller-provided working array.

### Syntax

#### Macro form

```c
OPSTATUS SORT_TYPED_FUNC(RadixWithBuffer, I64)(int64_t *data, size_t count, SORT_TYPE(TOrder) order, int64_t *buffer, size_t bufferCount);
```

#### Direct form

```c
OPSTATUS Sort_RadixWithBuffer_I64(int64_t *data, size_t count, Sort_TOrder order, int64_t *buffer, size_t bufferCount);
```

### Parameters

- `data`: elements of type `int64_t`; NULL is accepted for zero elements.
- `count`: number of elements, without overflowing `count * sizeof(int64_t)`.
- `order`: `SORT_CONST(ASCENDING)` or `SORT_CONST(DESCENDING)`.
- `buffer`: separate `int64_t` scratch array for `count >= 2`.
- `bufferCount`: scratch capacity measured in **elements** (at least `count`), not bytes.

### Return value

Returns `STATUS_CONST(SUCCESS)` or `INVALID_ARGUMENT` for invalid order/array, `ARITHMETIC_OVERFLOW` for excessive count, or `INSUFFICIENT_SPACE` for missing/small scratch.

### Remarks

The signed algorithm handles `INT_MIN` without signed subtraction or signed overflow. Radix operations do not require a comparison callback. A count smaller than two succeeds without needing scratch.

### Example

```c
int64_t values[] = {4, 1, 3, 2};
int64_t scratch[4];
OPSTATUS status = SORT_TYPED_FUNC(RadixWithBuffer, I64)(values, 4, SORT_CONST(ASCENDING), scratch, 4);
```

# Typed custom arrays

`SORT_IMPLEMENT_ALL(TYPE, SUFFIX, COMPARE)` **defines** six `static inline` wrappers over the existing generic engine; it does not provide a universal runtime object. The suffix must be unique per type in the translation unit.

### Syntax

#### Macro form

```c
SORT_IMPLEMENT_ALL(TYPE, SUFFIX, COMPARE)
OPSTATUS SORT_TYPED_FUNC(Array, SUFFIX)(TYPE *data, size_t count);
OPSTATUS SORT_TYPED_FUNC(Insertion, SUFFIX)(TYPE *data, size_t count);
OPSTATUS SORT_TYPED_FUNC(Heap, SUFFIX)(TYPE *data, size_t count);
OPSTATUS SORT_TYPED_FUNC(Stable, SUFFIX)(TYPE *data, size_t count);
OPSTATUS SORT_TYPED_FUNC(Intro, SUFFIX)(TYPE *data, size_t count);
OPSTATUS SORT_TYPED_FUNC(Merge, SUFFIX)(TYPE *data, size_t count);
```

#### Direct form

With the default namespace, the generated wrappers are named `Sort_Array_SUFFIX`, `Sort_Insertion_SUFFIX`, `Sort_Heap_SUFFIX`, `Sort_Stable_SUFFIX`, `Sort_Intro_SUFFIX`, and `Sort_Merge_SUFFIX`.

### Parameters

- `TYPE`: C element type to wrap.
- `SUFFIX`: C identifier token used in generated function names.
- `COMPARE`: callable-like macro or function taking two `const TYPE *` pointers and returning `CMPOUT`.
- `data`: mutable pointer to the first typed element.
- `count`: number of elements.

### Return value

Each generated wrapper returns the same `OPSTATUS` as its generic implementation.

### Remarks

`Intro` delegates to `Array`, and `Merge` delegates to `Stable`. Typed wrappers do not implicitly specialize every classical or esoteric algorithm. Declare the generator at file scope, and use one suffix per type.

### Example

```c
typedef struct {
    int key;
    int serial;
} TRecord;

static CMPOUT CompareRecord(const TRecord *a, const TRecord *b) {
    return COMPARISON_FUNC(int)(a->key, b->key);
}

SORT_IMPLEMENT_ALL(TRecord, Record, CompareRecord)

void Example(void) {
    TRecord items[] = {{3, 0}, {1, 1}, {3, 2}};
    OPSTATUS status = SORT_TYPED_FUNC(Stable, Record)(items, 3);
    (void)status;
}
```

# Complexity and selection

Use `Sort_Array` for a general, in-place comparison sort with O(n log n) worst-case time and no heap allocation. Choose `Sort_Stable` or `Sort_Merge` when equivalently ranked records must preserve original order. Use `Sort_StableWithBuffer` to control temporary memory. Use radix functions for supported integer widths where allocating typed scratch is acceptable.

# Function table

The umbrella header includes `FunctionTable/Sort.inc`, which supplies the optional function table. See [PATTERN.md](../Reference/PATTERN.md) for configuration and namespace rules; direct namespace functions remain available independently.

# Requirements and tests

- C11 and standard headers only; no mandatory additional link libraries.
- No global mutable sort state.
- The call-local random generator is deterministic from its seed and **not** cryptographically secure.
- Generic comparators are responsible for consistent three-way ordering.
- For the documented test harness:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic -Werror -I Codespace Codespace/Tests/Sort.c -o /tmp/cosmeron-sort
/tmp/cosmeron-sort
```

# See also

- [Algorithm](Algorithm.md) — comparison callback types and results.
- [Memory](Memory.md) — scratch allocation conventions.
- [Container](Container.md) — container operations.
- [Bit](Bit.md) — reference format.

# C11 smoke test

This is a complete executable example; the individual function-reference examples above are usage fragments.

<!-- cosmeron-c11-test -->
```c
#include "Cosmeron/Modules/Sort/Sort.h"
#include <assert.h>
#include <stdint.h>

static CMPOUT CompareInt(const void *left, const void *right) {
    const int a = *(const int *)left;
    const int b = *(const int *)right;
    return COMPARISON_FUNC(int)(a, b);
}

int main(void) {
    int values[] = {4, 1, 3, 2};
    uint32_t integers[] = {9, 1, 5, 0};
    assert(SORT_FUNC(Array)(values, 4, sizeof values[0], CompareInt) ==
           STATUS_CONST(SUCCESS));
    assert(values[0] == 1 && values[3] == 4);
    assert(SORT_TYPED_FUNC(Radix, U32)(
               integers, 4, SORT_CONST(ASCENDING)) == STATUS_CONST(SUCCESS));
    assert(integers[0] == 0 && integers[3] == 9);
    return 0;
}
```
