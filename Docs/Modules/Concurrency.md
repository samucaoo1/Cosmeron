# Concurrency

The **Concurrency** module supplies portable concurrency building blocks: atomics, cooperative coroutines, native threads, synchronization, tasks, futures, executors and thread pools. This is the public API reference for the `audio-module` branch, patterned after [Bit.md](Bit.md).

```c
#include "Cosmeron/Modules/Concurrency/Concurrency.h" /* native umbrella */
#include "Cosmeron/Modules/Concurrency/Cooperative.h" /* cooperative facilities */
```

---

# Overview

A header-only C11 module using the Cosmeron namespace conventions. Cooperative tasks execute on the caller's thread until yielded or returned; native thread primitives invoke OS facilities. Support depends on the target platform and compiler.

### Macro form

```c
OPSTATUS status = MUTEX_FUNC(Init)(&mutex);
```

### Direct form

```c
OPSTATUS status = Concurrency_Mutex_Init(&mutex);
```

These two forms are equivalent under the default namespace. The examples below are **usage fragments**: named variables (such as `mutex`, `pool`, `task`) must be previously declared and initialized when the operation requires it; do not use a native object before initialization.

# Shared types, ownership and lifecycle

| Family | Main types |
| --- | --- |
| Atomic | `TAtomicU32`, conditionally `TAtomicU64`, `TAtomicPtr`, `TAtomicBool`, `MemoryOrder` |
| Coroutine | `TCoroutine`, `CoroutineFunction`, `CoroutineState` |
| Thread | `TThread`, `ThreadFunction`, `ThreadNative` |
| Synchronization | `TMutex`, `TRWLock`, `TCondition`, `TSemaphore`, `TBarrier`, `TOnce` |
| Task and executor | `TTask`, `TaskFunction`, `TExecutor`, `ExecutorSubmit` |
| Future and pool | `TFuture`, `FutureFunction`, `TFutureTask`, `TThreadPool` |

Use `CONCURRENCY_TYPE(NAME)` to qualify these types. Atomics expose `ATOMIC_*_INIT` initializers. `ONCE_INIT` initializes Once state. Native mutex/condition/barrier/semaphore/thread-pool objects follow explicit Init/Destroy, Create/Join or Create/Detach lifecycles; do not copy live instances or destroy while other threads use them.

`OPSTATUS` distinguishes SUCCESS, INVALID_ARGUMENT, BUSY, NOT_AVAILABLE, WOULD_BLOCK, CANCELLED, TIMEOUT and resource/backend failures where applicable. A try-acquire returning SUCCESS can still write false to its output flag. The caller owns callback context data and any pointed-to future results.

# API reference

## Function summary

| Package | Operations |
| --- | --- |
| Atomic | `U32_Load`, `U64_Load`, `Ptr_Load`, `U32_Store`, `U64_Store`, `Ptr_Store`, `U32_Exchange`, `U64_Exchange`, `Ptr_Exchange`, `U32_CompareExchange`, `U64_CompareExchange`, `Ptr_CompareExchange`, `U32_FetchAdd`, `U64_FetchAdd`, `U32_FetchSub`, `U64_FetchSub`, `U32_FetchAnd`, `U64_FetchAnd`, `U32_FetchOr`, `U64_FetchOr`, `U32_FetchXor`, `U64_FetchXor`, `Bool_Load`, `Bool_Store`, `Bool_Exchange` |
| Coroutine | `Init`, `Resume`, `Reset`, `GetState`, `IsReady`, `IsRunning`, `IsSuspended`, `IsFinished` |
| Thread | `Create`, `Join`, `Detach`, `Yield`, `Current`, `Equal`, `SleepFor`, `SleepUntil` |
| Mutex | `Init`, `Destroy`, `Lock`, `TryLock`, `Unlock` |
| RWLock | `Init`, `Destroy`, `ReadLock`, `TryReadLock`, `ReadUnlock`, `WriteLock`, `TryWriteLock`, `WriteUnlock` |
| Condition | `Init`, `Destroy`, `Wait`, `NotifyOne`, `NotifyAll` |
| Semaphore | `Init`, `Destroy`, `Wait`, `TryWait`, `Post` |
| Barrier | `Init`, `Destroy`, `Wait` |
| Once | `Call` |
| Task | `Create`, `IsValid`, `Run` |
| Executor | `Submit`, `RunInline` |
| Future | `Init`, `Destroy`, `IsReady`, `Complete`, `Wait`, `Get`, `Task` |
| ThreadPool | `Init`, `Submit`, `TrySubmit`, `Shutdown`, `Destroy` |

# Atomic package

Operações atômicas com ordenação de memória. U32/Ptr exigem atomics lock-free, enquanto U64 só existe quando `CONCURRENCY_HAS_ATOMIC_U64` é verdadeiro.

# U32_Load

Lê um valor atomicamente.

### Syntax

#### Macro form

```c
uint32_t ATOMIC_FUNC(U32_Load)( const CONCURRENCY_TYPE(TAtomicU32) *atomic, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint32_t Concurrency_Atomic_U32_Load( const Concurrency_TAtomicU32 *atomic, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint32_t`: Lê um valor atomicamente.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads.

### Example

```c
uint32_t previous = ATOMIC_FUNC(U32_Load)(&atomic, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U64_Load

Lê um valor atomicamente.

### Syntax

#### Macro form

```c
uint64_t ATOMIC_FUNC(U64_Load)( const CONCURRENCY_TYPE(TAtomicU64) *atomic, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint64_t Concurrency_Atomic_U64_Load( const Concurrency_TAtomicU64 *atomic, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint64_t`: Lê um valor atomicamente.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Só disponível quando `CONCURRENCY_HAS_ATOMIC_U64` está habilitado na plataforma.

### Example

```c
uint64_t previous = ATOMIC_FUNC(U64_Load)(&atomic, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# Ptr_Load

Lê um valor atomicamente.

### Syntax

#### Macro form

```c
uintptr_t ATOMIC_FUNC(Ptr_Load)( const CONCURRENCY_TYPE(TAtomicPtr) *atomic, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uintptr_t Concurrency_Atomic_Ptr_Load( const Concurrency_TAtomicPtr *atomic, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uintptr_t`: Lê um valor atomicamente.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads.

### Example

```c
uintptr_t previous = ATOMIC_FUNC(Ptr_Load)(&atomic, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U32_Store

Armazena um valor atomicamente.

### Syntax

#### Macro form

```c
void ATOMIC_FUNC(U32_Store)( CONCURRENCY_TYPE(TAtomicU32) *atomic, uint32_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
void Concurrency_Atomic_U32_Store( Concurrency_TAtomicU32 *atomic, uint32_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

Não retorna valor.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads.

### Example

```c
ATOMIC_FUNC(U32_Store)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U64_Store

Armazena um valor atomicamente.

### Syntax

#### Macro form

```c
void ATOMIC_FUNC(U64_Store)( CONCURRENCY_TYPE(TAtomicU64) *atomic, uint64_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
void Concurrency_Atomic_U64_Store( Concurrency_TAtomicU64 *atomic, uint64_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

Não retorna valor.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Só disponível quando `CONCURRENCY_HAS_ATOMIC_U64` está habilitado na plataforma.

### Example

```c
ATOMIC_FUNC(U64_Store)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# Ptr_Store

Armazena um valor atomicamente.

### Syntax

#### Macro form

```c
void ATOMIC_FUNC(Ptr_Store)( CONCURRENCY_TYPE(TAtomicPtr) *atomic, uintptr_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
void Concurrency_Atomic_Ptr_Store( Concurrency_TAtomicPtr *atomic, uintptr_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

Não retorna valor.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads.

### Example

```c
ATOMIC_FUNC(Ptr_Store)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U32_Exchange

Substitui o valor e devolve o anterior.

### Syntax

#### Macro form

```c
uint32_t ATOMIC_FUNC(U32_Exchange)( CONCURRENCY_TYPE(TAtomicU32) *atomic, uint32_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint32_t Concurrency_Atomic_U32_Exchange( Concurrency_TAtomicU32 *atomic, uint32_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint32_t`: Substitui o valor e devolve o anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor anterior, não o novo.

### Example

```c
uint32_t previous = ATOMIC_FUNC(U32_Exchange)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U64_Exchange

Substitui o valor e devolve o anterior.

### Syntax

#### Macro form

```c
uint64_t ATOMIC_FUNC(U64_Exchange)( CONCURRENCY_TYPE(TAtomicU64) *atomic, uint64_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint64_t Concurrency_Atomic_U64_Exchange( Concurrency_TAtomicU64 *atomic, uint64_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint64_t`: Substitui o valor e devolve o anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor anterior, não o novo. Só disponível quando `CONCURRENCY_HAS_ATOMIC_U64` está habilitado na plataforma.

### Example

```c
uint64_t previous = ATOMIC_FUNC(U64_Exchange)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# Ptr_Exchange

Substitui o valor e devolve o anterior.

### Syntax

#### Macro form

```c
uintptr_t ATOMIC_FUNC(Ptr_Exchange)( CONCURRENCY_TYPE(TAtomicPtr) *atomic, uintptr_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uintptr_t Concurrency_Atomic_Ptr_Exchange( Concurrency_TAtomicPtr *atomic, uintptr_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uintptr_t`: Substitui o valor e devolve o anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor anterior, não o novo.

### Example

```c
uintptr_t previous = ATOMIC_FUNC(Ptr_Exchange)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U32_CompareExchange

Tenta substituir o valor apenas quando igual ao esperado.

### Syntax

#### Macro form

```c
bool ATOMIC_FUNC(U32_CompareExchange)( CONCURRENCY_TYPE(TAtomicU32) *atomic, uint32_t *expected, uint32_t desired, CONCURRENCY_TYPE(MemoryOrder) success, CONCURRENCY_TYPE(MemoryOrder) failure);
```

#### Direct form

```c
bool Concurrency_Atomic_U32_CompareExchange( Concurrency_TAtomicU32 *atomic, uint32_t *expected, uint32_t desired, Concurrency_MemoryOrder success, Concurrency_MemoryOrder failure);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `expected`: ponteiro para valor esperado; em falha de CompareExchange é atualizado com o valor observado.
- `desired`: novo valor pretendido na operação compare-exchange.
- `success`: ordem de memória aplicada quando CompareExchange modifica o valor.
- `failure`: ordem de memória aplicada na falha; não deve ser Release nem AcqRel, nem mais forte que success.

### Return value

`bool`: Tenta substituir o valor apenas quando igual ao esperado.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. O resultado booleano sinaliza o sucesso da troca; `expected` é atualizado com o valor observado quando a condição falha.

### Example

```c
uint32_t expected = 1; /* tipo igual ao atomic correspondente */
bool ok = ATOMIC_FUNC(U32_CompareExchange)(&atomic, &expected, 2, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST), CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_RELAXED));
```

# U64_CompareExchange

Tenta substituir o valor apenas quando igual ao esperado.

### Syntax

#### Macro form

```c
bool ATOMIC_FUNC(U64_CompareExchange)( CONCURRENCY_TYPE(TAtomicU64) *atomic, uint64_t *expected, uint64_t desired, CONCURRENCY_TYPE(MemoryOrder) success, CONCURRENCY_TYPE(MemoryOrder) failure);
```

#### Direct form

```c
bool Concurrency_Atomic_U64_CompareExchange( Concurrency_TAtomicU64 *atomic, uint64_t *expected, uint64_t desired, Concurrency_MemoryOrder success, Concurrency_MemoryOrder failure);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `expected`: ponteiro para valor esperado; em falha de CompareExchange é atualizado com o valor observado.
- `desired`: novo valor pretendido na operação compare-exchange.
- `success`: ordem de memória aplicada quando CompareExchange modifica o valor.
- `failure`: ordem de memória aplicada na falha; não deve ser Release nem AcqRel, nem mais forte que success.

### Return value

`bool`: Tenta substituir o valor apenas quando igual ao esperado.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. O resultado booleano sinaliza o sucesso da troca; `expected` é atualizado com o valor observado quando a condição falha. Só disponível quando `CONCURRENCY_HAS_ATOMIC_U64` está habilitado na plataforma.

### Example

```c
uint32_t expected = 1; /* tipo igual ao atomic correspondente */
bool ok = ATOMIC_FUNC(U64_CompareExchange)(&atomic, &expected, 2, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST), CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_RELAXED));
```

# Ptr_CompareExchange

Tenta substituir o valor apenas quando igual ao esperado.

### Syntax

#### Macro form

```c
bool ATOMIC_FUNC(Ptr_CompareExchange)( CONCURRENCY_TYPE(TAtomicPtr) *atomic, uintptr_t *expected, uintptr_t desired, CONCURRENCY_TYPE(MemoryOrder) success, CONCURRENCY_TYPE(MemoryOrder) failure);
```

#### Direct form

```c
bool Concurrency_Atomic_Ptr_CompareExchange( Concurrency_TAtomicPtr *atomic, uintptr_t *expected, uintptr_t desired, Concurrency_MemoryOrder success, Concurrency_MemoryOrder failure);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `expected`: ponteiro para valor esperado; em falha de CompareExchange é atualizado com o valor observado.
- `desired`: novo valor pretendido na operação compare-exchange.
- `success`: ordem de memória aplicada quando CompareExchange modifica o valor.
- `failure`: ordem de memória aplicada na falha; não deve ser Release nem AcqRel, nem mais forte que success.

### Return value

`bool`: Tenta substituir o valor apenas quando igual ao esperado.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. O resultado booleano sinaliza o sucesso da troca; `expected` é atualizado com o valor observado quando a condição falha.

### Example

```c
uint32_t expected = 1; /* tipo igual ao atomic correspondente */
bool ok = ATOMIC_FUNC(Ptr_CompareExchange)(&atomic, &expected, 2, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST), CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_RELAXED));
```

# U32_FetchAdd

Soma atomicamente e devolve o valor anterior.

### Syntax

#### Macro form

```c
uint32_t ATOMIC_FUNC(U32_FetchAdd)( CONCURRENCY_TYPE(TAtomicU32) *atomic, uint32_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint32_t Concurrency_Atomic_U32_FetchAdd( Concurrency_TAtomicU32 *atomic, uint32_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint32_t`: Soma atomicamente e devolve o valor anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor que existia antes da operação.

### Example

```c
uint32_t previous = ATOMIC_FUNC(U32_FetchAdd)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U64_FetchAdd

Soma atomicamente e devolve o valor anterior.

### Syntax

#### Macro form

```c
uint64_t ATOMIC_FUNC(U64_FetchAdd)( CONCURRENCY_TYPE(TAtomicU64) *atomic, uint64_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint64_t Concurrency_Atomic_U64_FetchAdd( Concurrency_TAtomicU64 *atomic, uint64_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint64_t`: Soma atomicamente e devolve o valor anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor que existia antes da operação. Só disponível quando `CONCURRENCY_HAS_ATOMIC_U64` está habilitado na plataforma.

### Example

```c
uint64_t previous = ATOMIC_FUNC(U64_FetchAdd)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U32_FetchSub

Subtrai atomicamente e devolve o valor anterior.

### Syntax

#### Macro form

```c
uint32_t ATOMIC_FUNC(U32_FetchSub)( CONCURRENCY_TYPE(TAtomicU32) *atomic, uint32_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint32_t Concurrency_Atomic_U32_FetchSub( Concurrency_TAtomicU32 *atomic, uint32_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint32_t`: Subtrai atomicamente e devolve o valor anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor que existia antes da operação.

### Example

```c
uint32_t previous = ATOMIC_FUNC(U32_FetchSub)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U64_FetchSub

Subtrai atomicamente e devolve o valor anterior.

### Syntax

#### Macro form

```c
uint64_t ATOMIC_FUNC(U64_FetchSub)( CONCURRENCY_TYPE(TAtomicU64) *atomic, uint64_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint64_t Concurrency_Atomic_U64_FetchSub( Concurrency_TAtomicU64 *atomic, uint64_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint64_t`: Subtrai atomicamente e devolve o valor anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor que existia antes da operação. Só disponível quando `CONCURRENCY_HAS_ATOMIC_U64` está habilitado na plataforma.

### Example

```c
uint64_t previous = ATOMIC_FUNC(U64_FetchSub)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U32_FetchAnd

Aplica AND bit a bit e devolve o anterior.

### Syntax

#### Macro form

```c
uint32_t ATOMIC_FUNC(U32_FetchAnd)( CONCURRENCY_TYPE(TAtomicU32) *atomic, uint32_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint32_t Concurrency_Atomic_U32_FetchAnd( Concurrency_TAtomicU32 *atomic, uint32_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint32_t`: Aplica AND bit a bit e devolve o anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor que existia antes da operação.

### Example

```c
uint32_t previous = ATOMIC_FUNC(U32_FetchAnd)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U64_FetchAnd

Aplica AND bit a bit e devolve o anterior.

### Syntax

#### Macro form

```c
uint64_t ATOMIC_FUNC(U64_FetchAnd)( CONCURRENCY_TYPE(TAtomicU64) *atomic, uint64_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint64_t Concurrency_Atomic_U64_FetchAnd( Concurrency_TAtomicU64 *atomic, uint64_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint64_t`: Aplica AND bit a bit e devolve o anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor que existia antes da operação. Só disponível quando `CONCURRENCY_HAS_ATOMIC_U64` está habilitado na plataforma.

### Example

```c
uint64_t previous = ATOMIC_FUNC(U64_FetchAnd)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U32_FetchOr

Aplica OR bit a bit e devolve o anterior.

### Syntax

#### Macro form

```c
uint32_t ATOMIC_FUNC(U32_FetchOr)( CONCURRENCY_TYPE(TAtomicU32) *atomic, uint32_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint32_t Concurrency_Atomic_U32_FetchOr( Concurrency_TAtomicU32 *atomic, uint32_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint32_t`: Aplica OR bit a bit e devolve o anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor que existia antes da operação.

### Example

```c
uint32_t previous = ATOMIC_FUNC(U32_FetchOr)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U64_FetchOr

Aplica OR bit a bit e devolve o anterior.

### Syntax

#### Macro form

```c
uint64_t ATOMIC_FUNC(U64_FetchOr)( CONCURRENCY_TYPE(TAtomicU64) *atomic, uint64_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint64_t Concurrency_Atomic_U64_FetchOr( Concurrency_TAtomicU64 *atomic, uint64_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint64_t`: Aplica OR bit a bit e devolve o anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor que existia antes da operação. Só disponível quando `CONCURRENCY_HAS_ATOMIC_U64` está habilitado na plataforma.

### Example

```c
uint64_t previous = ATOMIC_FUNC(U64_FetchOr)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U32_FetchXor

Aplica XOR bit a bit e devolve o anterior.

### Syntax

#### Macro form

```c
uint32_t ATOMIC_FUNC(U32_FetchXor)( CONCURRENCY_TYPE(TAtomicU32) *atomic, uint32_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint32_t Concurrency_Atomic_U32_FetchXor( Concurrency_TAtomicU32 *atomic, uint32_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint32_t`: Aplica XOR bit a bit e devolve o anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor que existia antes da operação.

### Example

```c
uint32_t previous = ATOMIC_FUNC(U32_FetchXor)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# U64_FetchXor

Aplica XOR bit a bit e devolve o anterior.

### Syntax

#### Macro form

```c
uint64_t ATOMIC_FUNC(U64_FetchXor)( CONCURRENCY_TYPE(TAtomicU64) *atomic, uint64_t value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
uint64_t Concurrency_Atomic_U64_FetchXor( Concurrency_TAtomicU64 *atomic, uint64_t value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`uint64_t`: Aplica XOR bit a bit e devolve o anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor que existia antes da operação. Só disponível quando `CONCURRENCY_HAS_ATOMIC_U64` está habilitado na plataforma.

### Example

```c
uint64_t previous = ATOMIC_FUNC(U64_FetchXor)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# Bool_Load

Lê um valor atomicamente.

### Syntax

#### Macro form

```c
bool ATOMIC_FUNC(Bool_Load)( const CONCURRENCY_TYPE(TAtomicBool) *atomic, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
bool Concurrency_Atomic_Bool_Load( const Concurrency_TAtomicBool *atomic, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`bool`: Lê um valor atomicamente.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads.

### Example

```c
bool ok = ATOMIC_FUNC(Bool_Load)(&atomic, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# Bool_Store

Armazena um valor atomicamente.

### Syntax

#### Macro form

```c
void ATOMIC_FUNC(Bool_Store)( CONCURRENCY_TYPE(TAtomicBool) *atomic, bool value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
void Concurrency_Atomic_Bool_Store( Concurrency_TAtomicBool *atomic, bool value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

Não retorna valor.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads.

### Example

```c
ATOMIC_FUNC(Bool_Store)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# Bool_Exchange

Substitui o valor e devolve o anterior.

### Syntax

#### Macro form

```c
bool ATOMIC_FUNC(Bool_Exchange)( CONCURRENCY_TYPE(TAtomicBool) *atomic, bool value, CONCURRENCY_TYPE(MemoryOrder) order);
```

#### Direct form

```c
bool Concurrency_Atomic_Bool_Exchange( Concurrency_TAtomicBool *atomic, bool value, Concurrency_MemoryOrder order);
```

### Parameters

- `atomic`: ponteiro para o objeto atômico da largura especificada.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.
- `order`: ordem de memória solicitada; use valor válido de `CONCURRENCY_TYPE(MemoryOrder)`.

### Return value

`bool`: Substitui o valor e devolve o anterior.

### Remarks

A ordem de memória determina as garantias de sincronização; `Relaxed` não estabelece por si só uma relação de publicação entre threads. Retorna o valor anterior, não o novo.

### Example

```c
bool ok = ATOMIC_FUNC(Bool_Exchange)(&atomic, 1, CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST));
```

# Coroutine package

Corrotinas cooperativas explícitas baseadas em macros de continuação. Não há troca automática de threads nem pilha independente.

# Init

Inicializa um objeto e os recursos necessários, de acordo com seu tipo.

### Syntax

#### Macro form

```c
OPSTATUS COROUTINE_FUNC(Init)( CONCURRENCY_TYPE(TCoroutine) *coroutine, CONCURRENCY_TYPE(CoroutineFunction) function, void *argument);
```

#### Direct form

```c
OPSTATUS Concurrency_Coroutine_Init( Concurrency_TCoroutine *coroutine, Concurrency_CoroutineFunction function, void *argument);
```

### Parameters

- `coroutine`: ponteiro para `TCoroutine` inicializada; deve permanecer viva durante sua execução.
- `function`: callback fornecido pelo chamador, com assinatura do pacote.
- `argument`: contexto opaco emprestado ao callback; o chamador controla sua duração.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A corrotina não possui escalonador automático: é necessário chamar Resume; variáveis automáticas locais não persistem entre retornos por Yield.

### Example

```c
/* Worker / InitializeOnce: callbacks definidos pelo chamador. */
OPSTATUS status = COROUTINE_FUNC(Init)(&coroutine, Worker, NULL);
```

# Resume

Continua a execução da corrotina até ceder ou terminar.

### Syntax

#### Macro form

```c
OPSTATUS COROUTINE_FUNC(Resume)( CONCURRENCY_TYPE(TCoroutine) *coroutine);
```

#### Direct form

```c
OPSTATUS Concurrency_Coroutine_Resume( Concurrency_TCoroutine *coroutine);
```

### Parameters

- `coroutine`: ponteiro para `TCoroutine` inicializada; deve permanecer viva durante sua execução.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A corrotina não possui escalonador automático: é necessário chamar Resume; variáveis automáticas locais não persistem entre retornos por Yield. Resume pode retornar `BUSY` se o estado já é Running e `NOT_AVAILABLE` se já terminou.

### Example

```c
OPSTATUS status = COROUTINE_FUNC(Resume)(&coroutine);
```

# Reset

Reinicia estado e posição de continuação da corrotina.

### Syntax

#### Macro form

```c
OPSTATUS COROUTINE_FUNC(Reset)( CONCURRENCY_TYPE(TCoroutine) *coroutine);
```

#### Direct form

```c
OPSTATUS Concurrency_Coroutine_Reset( Concurrency_TCoroutine *coroutine);
```

### Parameters

- `coroutine`: ponteiro para `TCoroutine` inicializada; deve permanecer viva durante sua execução.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A corrotina não possui escalonador automático: é necessário chamar Resume; variáveis automáticas locais não persistem entre retornos por Yield.

### Example

```c
OPSTATUS status = COROUTINE_FUNC(Reset)(&coroutine);
```

# GetState

Obtém em parâmetro de saída o estado da corrotina.

### Syntax

#### Macro form

```c
OPSTATUS COROUTINE_FUNC(GetState)( const CONCURRENCY_TYPE(TCoroutine) *coroutine, CONCURRENCY_TYPE(CoroutineState) *outState);
```

#### Direct form

```c
OPSTATUS Concurrency_Coroutine_GetState( const Concurrency_TCoroutine *coroutine, Concurrency_CoroutineState *outState);
```

### Parameters

- `coroutine`: ponteiro para `TCoroutine` inicializada; deve permanecer viva durante sua execução.
- `outState`: recebe o estado atual da corrotina.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A corrotina não possui escalonador automático: é necessário chamar Resume; variáveis automáticas locais não persistem entre retornos por Yield.

### Example

```c
CONCURRENCY_TYPE(CoroutineState) state;
OPSTATUS status = COROUTINE_FUNC(GetState)(&coroutine, &state);
```

# IsReady

Testa se o objeto está no estado ready.

### Syntax

#### Macro form

```c
bool COROUTINE_FUNC(IsReady)( const CONCURRENCY_TYPE(TCoroutine) *coroutine);
```

#### Direct form

```c
bool Concurrency_Coroutine_IsReady( const Concurrency_TCoroutine *coroutine);
```

### Parameters

- `coroutine`: ponteiro para `TCoroutine` inicializada; deve permanecer viva durante sua execução.

### Return value

`bool`: Testa se o objeto está no estado ready.

### Remarks

A corrotina não possui escalonador automático: é necessário chamar Resume; variáveis automáticas locais não persistem entre retornos por Yield.

### Example

```c
bool ok = COROUTINE_FUNC(IsReady)(&coroutine);
```

# IsRunning

Testa se o objeto está executando.

### Syntax

#### Macro form

```c
bool COROUTINE_FUNC(IsRunning)( const CONCURRENCY_TYPE(TCoroutine) *coroutine);
```

#### Direct form

```c
bool Concurrency_Coroutine_IsRunning( const Concurrency_TCoroutine *coroutine);
```

### Parameters

- `coroutine`: ponteiro para `TCoroutine` inicializada; deve permanecer viva durante sua execução.

### Return value

`bool`: Testa se o objeto está executando.

### Remarks

A corrotina não possui escalonador automático: é necessário chamar Resume; variáveis automáticas locais não persistem entre retornos por Yield.

### Example

```c
bool ok = COROUTINE_FUNC(IsRunning)(&coroutine);
```

# IsSuspended

Testa se a corrotina cedeu o controle.

### Syntax

#### Macro form

```c
bool COROUTINE_FUNC(IsSuspended)( const CONCURRENCY_TYPE(TCoroutine) *coroutine);
```

#### Direct form

```c
bool Concurrency_Coroutine_IsSuspended( const Concurrency_TCoroutine *coroutine);
```

### Parameters

- `coroutine`: ponteiro para `TCoroutine` inicializada; deve permanecer viva durante sua execução.

### Return value

`bool`: Testa se a corrotina cedeu o controle.

### Remarks

A corrotina não possui escalonador automático: é necessário chamar Resume; variáveis automáticas locais não persistem entre retornos por Yield.

### Example

```c
bool ok = COROUTINE_FUNC(IsSuspended)(&coroutine);
```

# IsFinished

Testa se a corrotina concluiu.

### Syntax

#### Macro form

```c
bool COROUTINE_FUNC(IsFinished)( const CONCURRENCY_TYPE(TCoroutine) *coroutine);
```

#### Direct form

```c
bool Concurrency_Coroutine_IsFinished( const Concurrency_TCoroutine *coroutine);
```

### Parameters

- `coroutine`: ponteiro para `TCoroutine` inicializada; deve permanecer viva durante sua execução.

### Return value

`bool`: Testa se a corrotina concluiu.

### Remarks

A corrotina não possui escalonador automático: é necessário chamar Resume; variáveis automáticas locais não persistem entre retornos por Yield.

### Example

```c
bool ok = COROUTINE_FUNC(IsFinished)(&coroutine);
```

# Thread package

Threads nativas, identidade, join/detach e suspensão temporizada com tipos de Chronometry.

# Create

Cria um novo descritor ou recurso.

### Syntax

#### Macro form

```c
OPSTATUS THREAD_FUNC(Create)( CONCURRENCY_TYPE(TThread) *thread, CONCURRENCY_TYPE(ThreadFunction) function, void *argument);
```

#### Direct form

```c
OPSTATUS Concurrency_Thread_Create( Concurrency_TThread *thread, Concurrency_ThreadFunction function, void *argument);
```

### Parameters

- `thread`: ponteiro para objeto `TThread` de thread nativa.
- `function`: callback fornecido pelo chamador, com assinatura do pacote.
- `argument`: contexto opaco emprestado ao callback; o chamador controla sua duração.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
/* Worker / InitializeOnce: callbacks definidos pelo chamador. */
OPSTATUS status = THREAD_FUNC(Create)(&thread, Worker, NULL);
```

# Join

Aguarda o fim de uma thread joinable e consome a capacidade de join.

### Syntax

#### Macro form

```c
OPSTATUS THREAD_FUNC(Join)(CONCURRENCY_TYPE(TThread) *thread);
```

#### Direct form

```c
OPSTATUS Concurrency_Thread_Join(Concurrency_TThread *thread);
```

### Parameters

- `thread`: ponteiro para objeto `TThread` de thread nativa.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A thread só pode ter um ciclo de término (join ou detach). Join não envia solicitação de cancelamento.

### Example

```c
OPSTATUS status = THREAD_FUNC(Join)(&thread);
```

# Detach

Desvincula o thread, sem esperar sua conclusão.

### Syntax

#### Macro form

```c
OPSTATUS THREAD_FUNC(Detach)(CONCURRENCY_TYPE(TThread) *thread);
```

#### Direct form

```c
OPSTATUS Concurrency_Thread_Detach(Concurrency_TThread *thread);
```

### Parameters

- `thread`: ponteiro para objeto `TThread` de thread nativa.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A thread só pode ter um ciclo de término (join ou detach). Join não envia solicitação de cancelamento.

### Example

```c
OPSTATUS status = THREAD_FUNC(Detach)(&thread);
```

# Yield

Cede voluntariamente um quantum de execução ao escalonador nativo.

### Syntax

#### Macro form

```c
void THREAD_FUNC(Yield)(void);
```

#### Direct form

```c
void Concurrency_Thread_Yield(void);
```

### Parameters

- Nenhum.

### Return value

Não retorna valor.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
THREAD_FUNC(Yield)();
```

# Current

Obtém o identificador nativo da thread atual.

### Syntax

#### Macro form

```c
CONCURRENCY_TYPE(ThreadNative) THREAD_FUNC(Current)(void);
```

#### Direct form

```c
Concurrency_ThreadNative Concurrency_Thread_Current(void);
```

### Parameters

- Nenhum.

### Return value

`CONCURRENCY_TYPE(ThreadNative)`: Obtém o identificador nativo da thread atual.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
CONCURRENCY_TYPE(ThreadNative) current = THREAD_FUNC(Current)();
```

# Equal

Compara dois identificadores nativos de thread.

### Syntax

#### Macro form

```c
bool THREAD_FUNC(Equal)(CONCURRENCY_TYPE(ThreadNative) first, CONCURRENCY_TYPE(ThreadNative) second);
```

#### Direct form

```c
bool Concurrency_Thread_Equal(Concurrency_ThreadNative first, Concurrency_ThreadNative second);
```

### Parameters

- `first`: primeiro identificador nativo de thread.
- `second`: segundo identificador nativo de thread.

### Return value

`bool`: Compara dois identificadores nativos de thread.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
bool ok = THREAD_FUNC(Equal)(first, second);
```

# SleepFor

Suspende a thread pela duração informada.

### Syntax

#### Macro form

```c
OPSTATUS THREAD_FUNC(SleepFor)( CHRONOMETRY_TYPE(Duration) duration);
```

#### Direct form

```c
OPSTATUS Concurrency_Thread_SleepFor( Chronometry_Duration duration);
```

### Parameters

- `duration`: duração do repouso no relógio suportado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = THREAD_FUNC(SleepFor)(duration);
```

# SleepUntil

Suspende a thread até o prazo informado.

### Syntax

#### Macro form

```c
OPSTATUS THREAD_FUNC(SleepUntil)( CHRONOMETRY_TYPE(ClockTimePoint) deadline);
```

#### Direct form

```c
OPSTATUS Concurrency_Thread_SleepUntil( Chronometry_ClockTimePoint deadline);
```

### Parameters

- `deadline`: instante limite no relógio compatível.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = THREAD_FUNC(SleepUntil)(deadline);
```

# Mutex package

Exclusão mútua simples. Inicialize antes de Lock/TryLock e não destrua enquanto houver usuários ativos.

# Init

Inicializa um objeto e os recursos necessários, de acordo com seu tipo.

### Syntax

#### Macro form

```c
OPSTATUS MUTEX_FUNC(Init)(CONCURRENCY_TYPE(TMutex) *mutex);
```

#### Direct form

```c
OPSTATUS Concurrency_Mutex_Init(Concurrency_TMutex *mutex);
```

### Parameters

- `mutex`: mutex inicializado, pertencente à aplicação.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = MUTEX_FUNC(Init)(&mutex);
```

# Destroy

Encerra o recurso inicializado e libera o estado interno quando é seguro fazê-lo.

### Syntax

#### Macro form

```c
OPSTATUS MUTEX_FUNC(Destroy)(CONCURRENCY_TYPE(TMutex) *mutex);
```

#### Direct form

```c
OPSTATUS Concurrency_Mutex_Destroy(Concurrency_TMutex *mutex);
```

### Parameters

- `mutex`: mutex inicializado, pertencente à aplicação.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = MUTEX_FUNC(Destroy)(&mutex);
```

# Lock

Adquire o mutex, bloqueando se necessário.

### Syntax

#### Macro form

```c
OPSTATUS MUTEX_FUNC(Lock)(CONCURRENCY_TYPE(TMutex) *mutex);
```

#### Direct form

```c
OPSTATUS Concurrency_Mutex_Lock(Concurrency_TMutex *mutex);
```

### Parameters

- `mutex`: mutex inicializado, pertencente à aplicação.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = MUTEX_FUNC(Lock)(&mutex);
```

# TryLock

Tenta adquirir o mutex sem bloquear; escreve o resultado em `outAcquired`.

### Syntax

#### Macro form

```c
OPSTATUS MUTEX_FUNC(TryLock)(CONCURRENCY_TYPE(TMutex) *mutex, bool *outAcquired);
```

#### Direct form

```c
OPSTATUS Concurrency_Mutex_TryLock(Concurrency_TMutex *mutex, bool *outAcquired);
```

### Parameters

- `mutex`: mutex inicializado, pertencente à aplicação.
- `outAcquired`: recebe `true` se a aquisição foi efetuada, `false` se não foi possível sem bloquear.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A função pode retornar `SUCCESS` com `outAcquired=false`: não adquirir não é necessariamente erro.

### Example

```c
bool acquired = false;
OPSTATUS status = MUTEX_FUNC(TryLock)(&mutex, &acquired);
```

# Unlock

Libera um mutex adquirido.

### Syntax

#### Macro form

```c
OPSTATUS MUTEX_FUNC(Unlock)(CONCURRENCY_TYPE(TMutex) *mutex);
```

#### Direct form

```c
OPSTATUS Concurrency_Mutex_Unlock(Concurrency_TMutex *mutex);
```

### Parameters

- `mutex`: mutex inicializado, pertencente à aplicação.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = MUTEX_FUNC(Unlock)(&mutex);
```

# RWLock package

Bloqueio compartilhado para leitura e exclusivo para escrita. Pareie aquisição e liberação de mesmo modo.

# Init

Inicializa um objeto e os recursos necessários, de acordo com seu tipo.

### Syntax

#### Macro form

```c
OPSTATUS RWLOCK_FUNC(Init)(CONCURRENCY_TYPE(TRWLock) *lock);
```

#### Direct form

```c
OPSTATUS Concurrency_RWLock_Init(Concurrency_TRWLock *lock);
```

### Parameters

- `lock`: RWLock previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = RWLOCK_FUNC(Init)(&lock);
```

# Destroy

Encerra o recurso inicializado e libera o estado interno quando é seguro fazê-lo.

### Syntax

#### Macro form

```c
OPSTATUS RWLOCK_FUNC(Destroy)(CONCURRENCY_TYPE(TRWLock) *lock);
```

#### Direct form

```c
OPSTATUS Concurrency_RWLock_Destroy(Concurrency_TRWLock *lock);
```

### Parameters

- `lock`: RWLock previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = RWLOCK_FUNC(Destroy)(&lock);
```

# ReadLock

Adquire acesso compartilhado de leitura.

### Syntax

#### Macro form

```c
OPSTATUS RWLOCK_FUNC(ReadLock)(CONCURRENCY_TYPE(TRWLock) *lock);
```

#### Direct form

```c
OPSTATUS Concurrency_RWLock_ReadLock(Concurrency_TRWLock *lock);
```

### Parameters

- `lock`: RWLock previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = RWLOCK_FUNC(ReadLock)(&lock);
```

# TryReadLock

Tenta adquirir acesso de leitura sem esperar.

### Syntax

#### Macro form

```c
OPSTATUS RWLOCK_FUNC(TryReadLock)( CONCURRENCY_TYPE(TRWLock) *lock, bool *outAcquired);
```

#### Direct form

```c
OPSTATUS Concurrency_RWLock_TryReadLock( Concurrency_TRWLock *lock, bool *outAcquired);
```

### Parameters

- `lock`: RWLock previamente inicializado.
- `outAcquired`: recebe `true` se a aquisição foi efetuada, `false` se não foi possível sem bloquear.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A função pode retornar `SUCCESS` com `outAcquired=false`: não adquirir não é necessariamente erro.

### Example

```c
bool acquired = false;
OPSTATUS status = RWLOCK_FUNC(TryReadLock)(&lock, &acquired);
```

# ReadUnlock

Libera uma aquisição compartilhada.

### Syntax

#### Macro form

```c
OPSTATUS RWLOCK_FUNC(ReadUnlock)(CONCURRENCY_TYPE(TRWLock) *lock);
```

#### Direct form

```c
OPSTATUS Concurrency_RWLock_ReadUnlock(Concurrency_TRWLock *lock);
```

### Parameters

- `lock`: RWLock previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = RWLOCK_FUNC(ReadUnlock)(&lock);
```

# WriteLock

Adquire exclusividade de escrita.

### Syntax

#### Macro form

```c
OPSTATUS RWLOCK_FUNC(WriteLock)(CONCURRENCY_TYPE(TRWLock) *lock);
```

#### Direct form

```c
OPSTATUS Concurrency_RWLock_WriteLock(Concurrency_TRWLock *lock);
```

### Parameters

- `lock`: RWLock previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = RWLOCK_FUNC(WriteLock)(&lock);
```

# TryWriteLock

Tenta adquirir exclusividade de escrita sem esperar.

### Syntax

#### Macro form

```c
OPSTATUS RWLOCK_FUNC(TryWriteLock)( CONCURRENCY_TYPE(TRWLock) *lock, bool *outAcquired);
```

#### Direct form

```c
OPSTATUS Concurrency_RWLock_TryWriteLock( Concurrency_TRWLock *lock, bool *outAcquired);
```

### Parameters

- `lock`: RWLock previamente inicializado.
- `outAcquired`: recebe `true` se a aquisição foi efetuada, `false` se não foi possível sem bloquear.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A função pode retornar `SUCCESS` com `outAcquired=false`: não adquirir não é necessariamente erro.

### Example

```c
bool acquired = false;
OPSTATUS status = RWLOCK_FUNC(TryWriteLock)(&lock, &acquired);
```

# WriteUnlock

Libera uma aquisição exclusiva.

### Syntax

#### Macro form

```c
OPSTATUS RWLOCK_FUNC(WriteUnlock)(CONCURRENCY_TYPE(TRWLock) *lock);
```

#### Direct form

```c
OPSTATUS Concurrency_RWLock_WriteUnlock(Concurrency_TRWLock *lock);
```

### Parameters

- `lock`: RWLock previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = RWLOCK_FUNC(WriteUnlock)(&lock);
```

# Condition package

Variáveis de condição que esperam sobre um mutex adquirido. Reavalie o predicado protegido após acordar.

# Init

Inicializa um objeto e os recursos necessários, de acordo com seu tipo.

### Syntax

#### Macro form

```c
OPSTATUS CONDITION_FUNC(Init)( CONCURRENCY_TYPE(TCondition) *condition);
```

#### Direct form

```c
OPSTATUS Concurrency_Condition_Init( Concurrency_TCondition *condition);
```

### Parameters

- `condition`: variável de condição previamente inicializada.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = CONDITION_FUNC(Init)(&condition);
```

# Destroy

Encerra o recurso inicializado e libera o estado interno quando é seguro fazê-lo.

### Syntax

#### Macro form

```c
OPSTATUS CONDITION_FUNC(Destroy)( CONCURRENCY_TYPE(TCondition) *condition);
```

#### Direct form

```c
OPSTATUS Concurrency_Condition_Destroy( Concurrency_TCondition *condition);
```

### Parameters

- `condition`: variável de condição previamente inicializada.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = CONDITION_FUNC(Destroy)(&condition);
```

# Wait

Espera pela condição, contador, barreira ou conclusão correspondente ao pacote.

### Syntax

#### Macro form

```c
OPSTATUS CONDITION_FUNC(Wait)( CONCURRENCY_TYPE(TCondition) *condition, CONCURRENCY_TYPE(TMutex) *mutex);
```

#### Direct form

```c
OPSTATUS Concurrency_Condition_Wait( Concurrency_TCondition *condition, Concurrency_TMutex *mutex);
```

### Parameters

- `condition`: variável de condição previamente inicializada.
- `mutex`: mutex inicializado, pertencente à aplicação.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O chamador deve possuir o mutex ao esperar; a espera libera e readquire o mutex atomically. Acordar não garante que o predicado ficou verdadeiro.

### Example

```c
OPSTATUS status = CONDITION_FUNC(Wait)(&condition, &mutex);
```

# NotifyOne

Desperta pelo menos um participante elegível aguardando a condição.

### Syntax

#### Macro form

```c
OPSTATUS CONDITION_FUNC(NotifyOne)( CONCURRENCY_TYPE(TCondition) *condition);
```

#### Direct form

```c
OPSTATUS Concurrency_Condition_NotifyOne( Concurrency_TCondition *condition);
```

### Parameters

- `condition`: variável de condição previamente inicializada.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = CONDITION_FUNC(NotifyOne)(&condition);
```

# NotifyAll

Desperta todos os participantes elegíveis aguardando a condição.

### Syntax

#### Macro form

```c
OPSTATUS CONDITION_FUNC(NotifyAll)( CONCURRENCY_TYPE(TCondition) *condition);
```

#### Direct form

```c
OPSTATUS Concurrency_Condition_NotifyAll( Concurrency_TCondition *condition);
```

### Parameters

- `condition`: variável de condição previamente inicializada.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = CONDITION_FUNC(NotifyAll)(&condition);
```

# Semaphore package

Semáforo de contagem construído com mutex e variável de condição.

# Init

Inicializa contador do semáforo com o valor informado.

### Syntax

#### Macro form

```c
OPSTATUS SEMAPHORE_FUNC(Init)( CONCURRENCY_TYPE(TSemaphore) *semaphore, unsigned value);
```

#### Direct form

```c
OPSTATUS Concurrency_Semaphore_Init( Concurrency_TSemaphore *semaphore, unsigned value);
```

### Parameters

- `semaphore`: semáforo previamente inicializado.
- `value`: valor que será armazenado, trocado, somado ou passado para a operação.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = SEMAPHORE_FUNC(Init)(&semaphore, 1);
```

# Destroy

Encerra o recurso inicializado e libera o estado interno quando é seguro fazê-lo.

### Syntax

#### Macro form

```c
OPSTATUS SEMAPHORE_FUNC(Destroy)( CONCURRENCY_TYPE(TSemaphore) *semaphore);
```

#### Direct form

```c
OPSTATUS Concurrency_Semaphore_Destroy( Concurrency_TSemaphore *semaphore);
```

### Parameters

- `semaphore`: semáforo previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = SEMAPHORE_FUNC(Destroy)(&semaphore);
```

# Wait

Espera pela condição, contador, barreira ou conclusão correspondente ao pacote.

### Syntax

#### Macro form

```c
OPSTATUS SEMAPHORE_FUNC(Wait)( CONCURRENCY_TYPE(TSemaphore) *semaphore);
```

#### Direct form

```c
OPSTATUS Concurrency_Semaphore_Wait( Concurrency_TSemaphore *semaphore);
```

### Parameters

- `semaphore`: semáforo previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = SEMAPHORE_FUNC(Wait)(&semaphore);
```

# TryWait

Executa a operação pública definida no cabeçalho.

### Syntax

#### Macro form

```c
OPSTATUS SEMAPHORE_FUNC(TryWait)( CONCURRENCY_TYPE(TSemaphore) *semaphore, bool *outAcquired);
```

#### Direct form

```c
OPSTATUS Concurrency_Semaphore_TryWait( Concurrency_TSemaphore *semaphore, bool *outAcquired);
```

### Parameters

- `semaphore`: semáforo previamente inicializado.
- `outAcquired`: recebe `true` se a aquisição foi efetuada, `false` se não foi possível sem bloquear.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
bool acquired = false;
OPSTATUS status = SEMAPHORE_FUNC(TryWait)(&semaphore, &acquired);
```

# Post

Incrementa a disponibilidade do semáforo e acorda eventual participante.

### Syntax

#### Macro form

```c
OPSTATUS SEMAPHORE_FUNC(Post)( CONCURRENCY_TYPE(TSemaphore) *semaphore);
```

#### Direct form

```c
OPSTATUS Concurrency_Semaphore_Post( Concurrency_TSemaphore *semaphore);
```

### Parameters

- `semaphore`: semáforo previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = SEMAPHORE_FUNC(Post)(&semaphore);
```

# Barrier package

Barreira reutilizável por gerações; todos os participantes devem alcançá-la antes de avançar.

# Init

Inicializa a barreira para `count` participantes.

### Syntax

#### Macro form

```c
OPSTATUS BARRIER_FUNC(Init)( CONCURRENCY_TYPE(TBarrier) *barrier, unsigned count);
```

#### Direct form

```c
OPSTATUS Concurrency_Barrier_Init( Concurrency_TBarrier *barrier, unsigned count);
```

### Parameters

- `barrier`: barreira inicializada.
- `count`: quantidade de participantes exigida para liberar a barreira.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = BARRIER_FUNC(Init)(&barrier, 2);
```

# Destroy

Encerra o recurso inicializado e libera o estado interno quando é seguro fazê-lo.

### Syntax

#### Macro form

```c
OPSTATUS BARRIER_FUNC(Destroy)(CONCURRENCY_TYPE(TBarrier) *barrier);
```

#### Direct form

```c
OPSTATUS Concurrency_Barrier_Destroy(Concurrency_TBarrier *barrier);
```

### Parameters

- `barrier`: barreira inicializada.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = BARRIER_FUNC(Destroy)(&barrier);
```

# Wait

Espera pela condição, contador, barreira ou conclusão correspondente ao pacote.

### Syntax

#### Macro form

```c
OPSTATUS BARRIER_FUNC(Wait)(CONCURRENCY_TYPE(TBarrier) *barrier);
```

#### Direct form

```c
OPSTATUS Concurrency_Barrier_Wait(Concurrency_TBarrier *barrier);
```

### Parameters

- `barrier`: barreira inicializada.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Uma barreira quebrada retorna `CANCELLED`; todos os participantes devem cumprir o protocolo de sincronização.

### Example

```c
OPSTATUS status = BARRIER_FUNC(Wait)(&barrier);
```

# Once package

Execução de callback exatamente uma vez pelo backend nativo.

# Call

Executa a função uma vez, mesmo quando chamada por múltiplas threads.

### Syntax

#### Macro form

```c
OPSTATUS ONCE_FUNC(Call)( CONCURRENCY_TYPE(TOnce) *once, CONCURRENCY_TYPE(OnceFunction) function);
```

#### Direct form

```c
OPSTATUS Concurrency_Once_Call( Concurrency_TOnce *once, Concurrency_OnceFunction function);
```

### Parameters

- `once`: estado de controle inicializado com `ONCE_INIT`.
- `function`: callback fornecido pelo chamador, com assinatura do pacote.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A instância `TOnce` deve ser inicializada com `ONCE_INIT` e não por memset arbitrário.

### Example

```c
/* Worker / InitializeOnce: callbacks definidos pelo chamador. */
OPSTATUS status = ONCE_FUNC(Call)(&once, InitializeOnce);
```

# Task package

Descritor por valor de função e argumento; Run executa imediatamente no thread chamador.

# Create

Cria um novo descritor ou recurso.

### Syntax

#### Macro form

```c
CONCURRENCY_TYPE(TTask) TASK_FUNC(Create)( CONCURRENCY_TYPE(TaskFunction) function, void *argument);
```

#### Direct form

```c
Concurrency_TTask Concurrency_Task_Create( Concurrency_TaskFunction function, void *argument);
```

### Parameters

- `function`: callback fornecido pelo chamador, com assinatura do pacote.
- `argument`: contexto opaco emprestado ao callback; o chamador controla sua duração.

### Return value

`CONCURRENCY_TYPE(TTask)`: Cria um novo descritor ou recurso.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
/* Worker / InitializeOnce: callbacks definidos pelo chamador. */
CONCURRENCY_TYPE(TTask) created = TASK_FUNC(Create)(Worker, NULL);
```

# IsValid

Consulta se o descritor de tarefa contém função utilizável.

### Syntax

#### Macro form

```c
bool TASK_FUNC(IsValid)(const CONCURRENCY_TYPE(TTask) *task);
```

#### Direct form

```c
bool Concurrency_Task_IsValid(const Concurrency_TTask *task);
```

### Parameters

- `task`: descritor `TTask` válido.

### Return value

`bool`: Consulta se o descritor de tarefa contém função utilizável.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
bool ok = TASK_FUNC(IsValid)(task);
```

# Run

Executa a tarefa de forma síncrona no thread atual.

### Syntax

#### Macro form

```c
OPSTATUS TASK_FUNC(Run)(const CONCURRENCY_TYPE(TTask) *task);
```

#### Direct form

```c
OPSTATUS Concurrency_Task_Run(const Concurrency_TTask *task);
```

### Parameters

- `task`: descritor `TTask` válido.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

A execução é síncrona; não surge thread nova.

### Example

```c
OPSTATUS status = TASK_FUNC(Run)(task);
```

# Executor package

Adaptador para executor externo ou execução imediata, sem criar threads implicitamente.

# Submit

Chama o callback de submissão provido pelo executor host.

### Syntax

#### Macro form

```c
OPSTATUS CONCURRENCY_FUNC(Executor, Submit)( const CONCURRENCY_TYPE(TExecutor) *executor, CONCURRENCY_TYPE(TTask) task);
```

#### Direct form

```c
OPSTATUS Concurrency_Executor_Submit( const Concurrency_TExecutor *executor, Concurrency_TTask task);
```

### Parameters

- `executor`: executor externo com callback `submit` preenchido.
- `task`: descritor `TTask` válido.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O callback host deve copiar o par função/argumento se adiar a tarefa; quem fornece `argument` gerencia sua duração.

### Example

```c
OPSTATUS status = CONCURRENCY_FUNC(Executor, Submit)(&executor, task);
```

# RunInline

Executa a tarefa imediatamente no thread chamador.

### Syntax

#### Macro form

```c
OPSTATUS CONCURRENCY_FUNC(Executor, RunInline)( CONCURRENCY_TYPE(TTask) task);
```

#### Direct form

```c
OPSTATUS Concurrency_Executor_RunInline( Concurrency_TTask task);
```

### Parameters

- `task`: descritor `TTask` válido.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Não copie nem destrua objetos nativos ativos. Os ponteiros recebidos devem estar válidos e o estado deve respeitar o ciclo de vida do pacote.

### Example

```c
OPSTATUS status = CONCURRENCY_FUNC(Executor, RunInline)(task);
```

# Future package

Conclusão compartilhada de operação assíncrona, com mutex/condição internos e entrega de ponteiro de resultado.

# Init

Inicializa um objeto e os recursos necessários, de acordo com seu tipo.

### Syntax

#### Macro form

```c
OPSTATUS FUTURE_FUNC(Init)(CONCURRENCY_TYPE(TFuture) *future);
```

#### Direct form

```c
OPSTATUS Concurrency_Future_Init(Concurrency_TFuture *future);
```

### Parameters

- `future`: objeto `TFuture` previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O Future não libera automaticamente a memória apontada pelo resultado.

### Example

```c
OPSTATUS status = FUTURE_FUNC(Init)(&future);
```

# Destroy

Encerra o recurso inicializado e libera o estado interno quando é seguro fazê-lo.

### Syntax

#### Macro form

```c
OPSTATUS FUTURE_FUNC(Destroy)(CONCURRENCY_TYPE(TFuture) *future);
```

#### Direct form

```c
OPSTATUS Concurrency_Future_Destroy(Concurrency_TFuture *future);
```

### Parameters

- `future`: objeto `TFuture` previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O Future não libera automaticamente a memória apontada pelo resultado.

### Example

```c
OPSTATUS status = FUTURE_FUNC(Destroy)(&future);
```

# IsReady

Testa se o objeto está no estado ready.

### Syntax

#### Macro form

```c
bool FUTURE_FUNC(IsReady)(const CONCURRENCY_TYPE(TFuture) *future);
```

#### Direct form

```c
bool Concurrency_Future_IsReady(const Concurrency_TFuture *future);
```

### Parameters

- `future`: objeto `TFuture` previamente inicializado.

### Return value

`bool`: Testa se o objeto está no estado ready.

### Remarks

O Future não libera automaticamente a memória apontada pelo resultado.

### Example

```c
bool ok = FUTURE_FUNC(IsReady)(&future);
```

# Complete

Armazena resultado e sinaliza a conclusão do Future.

### Syntax

#### Macro form

```c
OPSTATUS FUTURE_FUNC(Complete)( CONCURRENCY_TYPE(TFuture) *future, void *result, OPSTATUS resultStatus);
```

#### Direct form

```c
OPSTATUS Concurrency_Future_Complete( Concurrency_TFuture *future, void *result, OPSTATUS resultStatus);
```

### Parameters

- `future`: objeto `TFuture` previamente inicializado.
- `result`: ponteiro de resultado; o Future não possui automaticamente o conteúdo apontado.
- `resultStatus`: resultado da operação produtor, armazenado no Future.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

Uma segunda conclusão retorna `ALREADY_EXISTS`. O Future não libera automaticamente a memória apontada pelo resultado.

### Example

```c
OPSTATUS status = FUTURE_FUNC(Complete)(&future, result, STATUS_CONST(SUCCESS));
```

# Wait

Espera pela condição, contador, barreira ou conclusão correspondente ao pacote.

### Syntax

#### Macro form

```c
OPSTATUS FUTURE_FUNC(Wait)(CONCURRENCY_TYPE(TFuture) *future);
```

#### Direct form

```c
OPSTATUS Concurrency_Future_Wait(Concurrency_TFuture *future);
```

### Parameters

- `future`: objeto `TFuture` previamente inicializado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O Future não libera automaticamente a memória apontada pelo resultado.

### Example

```c
OPSTATUS status = FUTURE_FUNC(Wait)(&future);
```

# Get

Aguarda a conclusão e obtém o ponteiro do resultado e seu status.

### Syntax

#### Macro form

```c
OPSTATUS FUTURE_FUNC(Get)( CONCURRENCY_TYPE(TFuture) *future, void **outResult, OPSTATUS *outStatus);
```

#### Direct form

```c
OPSTATUS Concurrency_Future_Get( Concurrency_TFuture *future, void **outResult, OPSTATUS *outStatus);
```

### Parameters

- `future`: objeto `TFuture` previamente inicializado.
- `outResult`: recebe ponteiro de resultado após a conclusão.
- `outStatus`: recebe o `OPSTATUS` produzido pelo callback.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O retorno de Get informa falha da operação de espera/leitura; o status do trabalho concluído aparece em `outStatus`. O Future não libera automaticamente a memória apontada pelo resultado.

### Example

```c
void *result = NULL;
OPSTATUS resultStatus;
OPSTATUS status = FUTURE_FUNC(Get)(&future, &result, &resultStatus);
```

# Task

Prepara o vínculo de um trabalho executável com um Future.

### Syntax

#### Macro form

```c
OPSTATUS FUTURE_FUNC(Task)( CONCURRENCY_TYPE(TFutureTask) *futureTask, CONCURRENCY_TYPE(TFuture) *future, CONCURRENCY_TYPE(FutureFunction) function, void *argument, CONCURRENCY_TYPE(TTask) *outTask);
```

#### Direct form

```c
OPSTATUS Concurrency_Future_Task( Concurrency_TFutureTask *futureTask, Concurrency_TFuture *future, Concurrency_FutureFunction function, void *argument, Concurrency_TTask *outTask);
```

### Parameters

- `futureTask`: ponteiro para vínculo de Future e função de trabalho.
- `future`: objeto `TFuture` previamente inicializado.
- `function`: callback fornecido pelo chamador, com assinatura do pacote.
- `argument`: contexto opaco emprestado ao callback; o chamador controla sua duração.
- `outTask`: valor do tipo indicado na assinatura, válido durante a chamada.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O Future não libera automaticamente a memória apontada pelo resultado.

### Example

```c
/* Worker / InitializeOnce: callbacks definidos pelo chamador. */
OPSTATUS status = FUTURE_FUNC(Task)(&futureTask, &future, Worker, NULL, &outTask);
```

# ThreadPool package

Fila com memória fornecida pelo chamador e threads nativas, encerramento controlado e destruição explícita.

# Init

Inicializa workers, fila e sincronização, usando arrays do chamador.

### Syntax

#### Macro form

```c
OPSTATUS THREAD_POOL_FUNC(Init)( CONCURRENCY_TYPE(TThreadPool) *pool, CONCURRENCY_TYPE(TThread) *threads, size_t threadCount, CONCURRENCY_TYPE(TTask) *queue, size_t queueCapacity);
```

#### Direct form

```c
OPSTATUS Concurrency_ThreadPool_Init( Concurrency_TThreadPool *pool, Concurrency_TThread *threads, size_t threadCount, Concurrency_TTask *queue, size_t queueCapacity);
```

### Parameters

- `pool`: thread pool já configurado.
- `threads`: array de `TThread` do chamador com capacidade threadCount.
- `threadCount`: número de workers nativos a iniciar.
- `queue`: array de `TTask` fornecido pelo chamador como fila.
- `queueCapacity`: número de posições disponíveis na fila.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O pool empresta os arrays de threads e fila e não os aloca nem os libera.

### Example

```c
OPSTATUS status = THREAD_POOL_FUNC(Init)(&pool, threads, 2, queue, 8);
```

# Submit

Submete uma tarefa ao executor ou à fila de workers.

### Syntax

#### Macro form

```c
OPSTATUS THREAD_POOL_FUNC(Submit)( CONCURRENCY_TYPE(TThreadPool) *pool, CONCURRENCY_TYPE(TTask) task);
```

#### Direct form

```c
OPSTATUS Concurrency_ThreadPool_Submit( Concurrency_TThreadPool *pool, Concurrency_TTask task);
```

### Parameters

- `pool`: thread pool já configurado.
- `task`: descritor `TTask` válido.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O pool empresta os arrays de threads e fila e não os aloca nem os libera.

### Example

```c
OPSTATUS status = THREAD_POOL_FUNC(Submit)(&pool, task);
```

# TrySubmit

Tenta enfileirar sem bloquear quando a fila está cheia.

### Syntax

#### Macro form

```c
OPSTATUS THREAD_POOL_FUNC(TrySubmit)( CONCURRENCY_TYPE(TThreadPool) *pool, CONCURRENCY_TYPE(TTask) task, bool *outSubmitted);
```

#### Direct form

```c
OPSTATUS Concurrency_ThreadPool_TrySubmit( Concurrency_TThreadPool *pool, Concurrency_TTask task, bool *outSubmitted);
```

### Parameters

- `pool`: thread pool já configurado.
- `task`: descritor `TTask` válido.
- `outSubmitted`: recebe se a tarefa foi efetivamente enfileirada sem esperar.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O pool empresta os arrays de threads e fila e não os aloca nem os libera. Quando a fila não comporta uma tarefa sem bloquear, pode retornar `WOULD_BLOCK`; consulte `outSubmitted` quando o status for SUCCESS.

### Example

```c
bool submitted = false;
OPSTATUS status = THREAD_POOL_FUNC(TrySubmit)(&pool, task, &submitted);
```

# Shutdown

Sinaliza o encerramento do pool e acorda participantes bloqueados.

### Syntax

#### Macro form

```c
OPSTATUS THREAD_POOL_FUNC(Shutdown)( CONCURRENCY_TYPE(TThreadPool) *pool);
```

#### Direct form

```c
OPSTATUS Concurrency_ThreadPool_Shutdown( Concurrency_TThreadPool *pool);
```

### Parameters

- `pool`: thread pool já configurado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O pool empresta os arrays de threads e fila e não os aloca nem os libera. Shutdown sinaliza parada; Destroy completa a limpeza/joins conforme o contrato de implementação.

### Example

```c
OPSTATUS status = THREAD_POOL_FUNC(Shutdown)(&pool);
```

# Destroy

Encerra o recurso inicializado e libera o estado interno quando é seguro fazê-lo.

### Syntax

#### Macro form

```c
OPSTATUS THREAD_POOL_FUNC(Destroy)( CONCURRENCY_TYPE(TThreadPool) *pool);
```

#### Direct form

```c
OPSTATUS Concurrency_ThreadPool_Destroy( Concurrency_TThreadPool *pool);
```

### Parameters

- `pool`: thread pool já configurado.

### Return value

`OPSTATUS`: `STATUS_CONST(SUCCESS)` para execução válida, ou código de erro coerente com argumentos, estado e resultado do backend.

### Remarks

O pool empresta os arrays de threads e fila e não os aloca nem os libera.

### Example

```c
OPSTATUS status = THREAD_POOL_FUNC(Destroy)(&pool);
```

# Worked examples

## Mutex lifecycle

```c
CONCURRENCY_TYPE(TMutex) mutex = {0};
OPSTATUS status = MUTEX_FUNC(Init)(&mutex);
if (status == STATUS_CONST(SUCCESS)) {
    status = MUTEX_FUNC(Lock)(&mutex);
    if (status == STATUS_CONST(SUCCESS)) {
        /* Seção crítica. */
        (void)MUTEX_FUNC(Unlock)(&mutex);
    }
    (void)MUTEX_FUNC(Destroy)(&mutex);
}
```

## Cooperative coroutine

```c
static void Step(CONCURRENCY_TYPE(TCoroutine) *self, void *argument) {
    (void)argument;
    COROUTINE_BEGIN(self);
    /* Primeira etapa. */
    COROUTINE_YIELD(self);
    /* Segunda etapa. */
    COROUTINE_END(self);
}

void RunSteps(void) {
    CONCURRENCY_TYPE(TCoroutine) co = COROUTINE_INIT(Step, NULL);
    (void)COROUTINE_FUNC(Resume)(&co);
    (void)COROUTINE_FUNC(Resume)(&co);
}
```

# Requirements

- Portable namespace macros: `CONCURRENCY_FUNC`, `CONCURRENCY_TYPE`, per-package `*_FUNC` and `CONCURRENCY_CONST`.
- POSIX backend uses pthreads; Win32 backend uses native APIs. The no-explicit-link objective does not supersede underlying platform availability.
- Atomics depend on supported compiler builtins or platform code; 64-bit atomics may not exist on all targets.
- A thread-pool shutdown and cooperative cancellation are not forceful OS thread termination.
- Coroutines in this package are stackless macros, not C++ coroutines or fibers.

# See also

- [Chronometry](Chronometry.md) — durations and clock time points.
- [Error](Error.md) — status codes.
- [Algorithm](Algorithm.md) — comparison callback patterns.
- [Bit](Bit.md) — documentation reference template.

# C11 smoke test (cooperative subset)

The executable example uses the explicitly cooperative header so that no native OS thread or extra linker flag is required. Native synchronization remains covered by the module's regular test suite.

<!-- cosmeron-c11-test -->
```c
#include "Cosmeron/Modules/Concurrency/Cooperative.h"
#include <assert.h>

typedef struct Counter {
    unsigned value;
} Counter;

static void CountSteps(CONCURRENCY_TYPE(TCoroutine) *self, void *argument) {
    Counter *counter = (Counter *)argument;
    COROUTINE_BEGIN(self);
    counter->value += 1;
    COROUTINE_YIELD(self);
    counter->value += 1;
    COROUTINE_END(self);
}

int main(void) {
    CONCURRENCY_TYPE(TAtomicU32) atomic = ATOMIC_U32_INIT(0);
    const CONCURRENCY_TYPE(MemoryOrder) order =
        CONCURRENCY_CONST(ATOMIC, MEMORY_ORDER_SEQ_CST);
    ATOMIC_FUNC(U32_Store)(&atomic, 7, order);
    assert(ATOMIC_FUNC(U32_Load)(&atomic, order) == 7);

    Counter counter = {0};
    CONCURRENCY_TYPE(TCoroutine) coroutine =
        COROUTINE_INIT(CountSteps, &counter);
    assert(COROUTINE_FUNC(Resume)(&coroutine) == STATUS_CONST(SUCCESS));
    assert(counter.value == 1);
    assert(COROUTINE_FUNC(Resume)(&coroutine) == STATUS_CONST(SUCCESS));
    assert(counter.value == 2);
    assert(COROUTINE_FUNC(IsFinished)(&coroutine));
    return 0;
}
```
