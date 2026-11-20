# Memory

O módulo **Memory** oferece alocação, operações com bytes, interfaces de alocadores, arenas, pools, um heap de largura fixa, streams binários limitados, um coletor por rastreamento, famílias de smart pointers e blocos de memória de largura fixa. Operações que podem falhar retornam `OPSTATUS` e utilizam parâmetros de saída pertencentes ao chamador. `Arena`, `Pool`, `Heap` e `Stream` desativam explicitamente suas tabelas de funções de objeto. Use chamadas diretas `MODULE_FUNC(Memory, Type, Method)` para os objetos Memory ao longo deste guia; a forma direta continua válida independentemente de um cabeçalho específico habilitar ou não uma tabela de funções.

## Cabeçalhos

~~~c
#include "Cosmeron/Core/Memory/Memory.h"
~~~

O cabeçalho agregador inclui:

| Cabeçalho | Recurso |
| --- | --- |
| `Operations.h` | Cópia, movimentação, preenchimento, zeragem e comparação de trechos de bytes. |
| `Alloc.h` | Alocação, redimensionamento e liberação de memória bruta. |
| `Allocator.h` | Descritor de alocador com apagamento de tipo e capacidades. |
| `../Algorithm/Swap.h` | Troca de bytes e arrays exposta pelo módulo Memory. |
| `Arena.h` | Alocação linear a partir de um buffer ou de uma alocação de apoio própria. |
| `Pool.h` | Pool de elementos de tamanho fixo. |
| `Heap.h` | Heap baseado em páginas e classes de tamanho. |
| `Stream.h` | Stream limitado de leitura/escrita com operações de bits e endianness. |
| `Collector.h` | Coletor de lixo por rastreamento e fachada do coletor global. |
| `Pointer/Pointer.h` | Wrappers de ponteiros unique, shared, weak, atomic e intrusive. |

`Block.h` é um cabeçalho separado para blocos de memória de largura fixa e não é incluído por `Memory.h`; inclua-o diretamente quando necessário.

---

# Visão geral

Memory abrange diversas estratégias distintas de alocação e armazenamento: alocação bruta para blocos individuais, operações com bytes para trechos pertencentes ao chamador, arenas e pools para alocação por escopo ou de tamanho fixo, um heap baseado em páginas, streams limitados para E/S binária e um coletor por rastreamento ou smart pointers para propriedade gerenciada. Escolha o recurso mais específico que corresponda ao modelo de vida útil e propriedade, em vez de tratar todos os alocadores como intercambiáveis.

---

# Referência da API

## Alocação bruta

Cabeçalho: `Alloc.h`. Pacote: `Memory_Alloc`.

| Função | Assinatura | Descrição |
| --- | --- | --- |
| `Bytes` | `(void *outPointerObject, size_t size)` | Aloca uma quantidade de bytes e grava o ponteiro resultante por meio do parâmetro de saída. |
| `Array` | `(void *outPointerObject, size_t count, size_t elementSize)` | Aloca uma quantidade de elementos com verificação da multiplicação dos tamanhos. |
| `Zero.Bytes` | `(void *outPointerObject, size_t size)` | Aloca bytes inicializados com zero. |
| `Zero.Array` | `(void *outPointerObject, size_t count, size_t elementSize)` | Aloca um array inicializado com zero. |
| `Realloc.Bytes` | `(void *inoutPointerObject, size_t newSize)` | Redimensiona a alocação e atualiza o ponteiro. |
| `Realloc.Array` | `(void *inoutPointerObject, size_t count, size_t elementSize)` | Redimensiona a alocação para uma quantidade de elementos. |
| `Free` | `(void *inoutPointerObject)` | Libera a alocação e limpa o ponteiro. |

O primeiro argumento das funções de alocação e realocação é o endereço de uma variável ponteiro, passado como `void *`. O alocador grava o novo ponteiro nessa variável. Não passe o próprio ponteiro da alocação no lugar do endereço da variável ponteiro.

~~~c
int *values = NULL;

OPSTATUS status = MODULE_FUNC(Memory, Alloc, Array)(
    &values, 16U, sizeof(*values));

if (status == STATUS_CONST(SUCCESS)) {
    /* use values */
    MODULE_FUNC(Memory, Alloc, Free)(&values);
}
~~~

`Memory_Free(pointerLvalue)` é uma macro de conveniência em torno de `Memory_Alloc_Free` que recebe um lvalue de ponteiro. É um auxiliar de limpeza no estilo de instrução; prefira a operação do pacote quando o status for necessário.

---

## Operações com bytes

Cabeçalho: `Operations.h`. Pacote: `Memory_Operations`.

| Função | Contrato |
| --- | --- |
| `Copy(destination, source, size)` | Copia um trecho de bytes; origem e destino não podem se sobrepor. |
| `Copy.Array(destination, source, count, elementSize)` | Copia um array após verificar a multiplicação dos tamanhos. |
| `Move(destination, source, size)` | Move um trecho de bytes e permite sobreposição. |
| `Move.Array(destination, source, count, elementSize)` | Move um array após verificar a multiplicação dos tamanhos. |
| `Set(destination, value, size)` | Preenche bytes com o byte menos significativo do valor inteiro. |
| `Zero(destination, size)` | Define como zero um trecho de bytes. |
| `Zero.Array(destination, count, elementSize)` | Zera um array após verificar a multiplicação dos tamanhos. |
| `Equals(left, right, size)` | Retorna se os trechos de bytes são iguais. |

~~~c
MODULE_FUNC(Memory, Operations, Copy)(
    destination, source, byteCount);

MODULE_FUNC(Memory, Operations, Move)(
    destination, source, byteCount);
~~~

Use `Copy` somente para intervalos sem sobreposição; use `Move` quando eles puderem se sobrepor. `Set` repete um valor de byte e não é um construtor de arrays tipados. Utilize inicialização tipada explícita quando a representação do elemento não for um padrão de bytes repetido.

### Troca

Cabeçalho: `Core/Algorithm/Swap.h`. Esse cabeçalho declara sua API pública no namespace do módulo Memory.

| Função | Descrição |
| --- | --- |
| `Swap.Bytes(left, right, size)` | Troca dois intervalos de bytes sem sobreposição. |
| `Swap.Array(left, right, count, elementSize)` | Troca dois arrays com verificação da multiplicação dos tamanhos. |
| Especializações de templates tipados | Geram procedimentos de troca de valores tipados para um tipo C selecionado. |

Swap exige intervalos válidos e sem sobreposição. A implementação trata ponteiros idênticos e solicitações de tamanho zero como operações sem efeito. Não substitui `Move` quando há sobreposição.

---

## Interface Allocator

Cabeçalho: `Allocator.h`.

`MODULE_TYPE(Memory, Allocator)` é um valor de alocador com apagamento de tipo que contém uma tabela de operações, contexto e flags de capacidade. O pacote `Allocator` fornece `Default`, `IsValid`, `Alloc` (incluindo `Aligned`), `Calloc`, `Realloc` e `Free`.

Use um alocador quando um componente precisar aceitar uma política de memória escolhida pelo chamador sem depender de uma implementação concreta de alocador. O valor Allocator não é proprietário do contexto externo referenciado por seus ponteiros de função; esse contexto deve permanecer válido durante todo o seu uso.

---

## Arena

Cabeçalho: `Arena.h`. Tipo: `MODULE_TYPE(Memory, Arena)`.

A alocação por Arena é útil para grupos de objetos de vida curta que podem ser liberados em conjunto. A arena armazena um buffer, sua capacidade e o deslocamento atual.

| Método | Descrição |
| --- | --- |
| `Create(capacity)` | Cria o armazenamento da arena com a capacidade solicitada. |
| `Create.Buffer(buffer, capacity)` | Inicializa uma arena sobre um armazenamento fornecido pelo chamador. |
| `Alloc.Bytes`, `Alloc.Aligned` | Aloca bytes com alinhamento opcional. |
| `Alloc.Array`, `Alloc.ArrayAligned` | Aloca uma quantidade de elementos com alinhamento opcional. |
| `Save(outMark)` | Grava o deslocamento atual em uma marca de saída. |
| `Restore(mark)` | Restaura um deslocamento registrado anteriormente. |
| `Contains(pointer)` | Verifica se um ponteiro está dentro do buffer da arena. |
| `Reset()` | Redefine o deslocamento para o início. |
| `Destroy()` | Libera o armazenamento próprio e limpa o estado da arena. |
| `AsAllocator()` | Retorna uma interface de alocador respaldada pela arena. |

~~~c
MODULE_TYPE(Memory, Arena) arena = {0};

OPSTATUS status = MODULE_FUNC(Memory, Arena, Create)(&arena, 4096U);
if (status == STATUS_CONST(SUCCESS)) {
    void *memory = NULL;
    status = MODULE_FUNC(Memory, Arena, Alloc, Bytes)(
        &arena, &memory, 128U);
    (void)MODULE_FUNC(Memory, Arena, Destroy)(&arena);
}
~~~

A memória retornada por uma arena permanece válida até que a arena seja reiniciada, restaurada para além da alocação, destruída ou que seu buffer de apoio se torne inválido por outro motivo. Não libere alocações individuais de arena com o alocador bruto.

---

## Pool

Cabeçalho: `Pool.h`. Tipo: `MODULE_TYPE(Memory, Pool)`.

Um pool gerencia uma quantidade fixa de elementos de mesmo tamanho.

| Método | Descrição |
| --- | --- |
| `Create(elementSize, elementCount)` | Cria o armazenamento do pool para um tamanho e uma quantidade fixos de elementos. |
| `Create.Buffer(buffer, bufferSize, elementSize)` | Inicializa um pool sobre memória pertencente ao chamador. |
| `Alloc(outPointer)` | Adquire um elemento. |
| `Free(pointer)` / `Free.Pointer(pointerLvalue)` | Devolve um elemento ao pool. |
| `Reset()` | Marca todos os elementos como disponíveis novamente. |
| `Destroy()` | Libera o armazenamento de apoio próprio. |
| `Contains(pointer)` | Verifica se um ponteiro pertence ao pool. |
| `AsAllocator()` | Expõe o pool por meio da interface Allocator. |

As alocações do Pool seguem o contrato configurado de tamanho e alinhamento dos elementos. O pool não acompanha construtores/destrutores de objetos definidos pelo usuário; combine-o com os traits de Types quando for necessário um ciclo de vida não trivial.

---

## Heap

Cabeçalho: `Heap.h`. Tipo: `MODULE_TYPE(Memory, Heap)`.

Heap é um alocador baseado em páginas e classes de tamanho. Ele expõe criação/destruição, políticas de alocação, liberação, bloqueio e varredura de páginas e um adaptador genérico de alocador.

A tabela atual de políticas inclui `MANUAL`, `CONSERVATIVE`, `TYPED`, `ATOMIC`, `IMMUTABLE`, `TRACED` e `PINNED`. Esses são valores de política de alocação, não garantias gerais de sincronização para os objetos armazenados no heap.

| Método | Descrição |
| --- | --- |
| `Create(initialPages)` / `Destroy()` | Inicia ou encerra o estado do heap. |
| `Alloc` | Aloca usando uma política selecionada e um descritor de tipo opcional. |
| `Alloc.Small` / `Alloc.Large` | Caminhos explícitos para alocações pequenas/grandes. |
| `FreeCell(pointer)` | Libera uma célula do heap. |
| `BlacklistPage(pageIndex)` | Impede que uma página participe da alocação normal. |
| `SweepPage(page, outIsPageEmpty)` | Varre uma página e informa se ela está vazia. |
| `IsBlacklisted(pageIndex)` | Verifica se um índice de página está bloqueado. |
| `LookupEntry(pointer)` | Procura a entrada do heap associada a um ponteiro. |
| `ResolvePointer(interiorPointer, outBasePointer, outSize, outPolicy)` | Resolve um ponteiro interno para os metadados de sua alocação. |
| `Contains(pointer)` | Verifica se um ponteiro pertence a esse heap. |
| `LazySweepClass(sizeClassIndex)` | Executa uma varredura tardia para uma classe de tamanho. |
| `AsAllocator()` | Retorna um adaptador Allocator. |

Heap é um alocador concreto. Use `Allocator` ao passá-lo para um componente genérico. Siga os contratos dos métodos Heap para os parâmetros de política e descritor; as funções de alocação bruta não são intercambiáveis com a alocação gerenciada pelo Collector.

---

## Stream binário

Cabeçalho: `Stream.h`. Tipo: `MODULE_TYPE(Memory, Stream)`.

Um Stream é uma visualização limitada sobre um buffer fornecido pelo chamador. Ele oferece inicialização somente para leitura ou com permissão de escrita, busca de posição, avanço, alinhamento em byte, leituras/escritas tipadas, operações com inteiros cientes de endianness e acesso em nível de bit.

| Grupo | Métodos |
| --- | --- |
| Inicialização | `Init.Read(buffer, size)`, `Init.Write(buffer, capacity)` |
| Posição | `Seek(offset, origin)`, `Skip(count)`, `Rewind()`, `AlignToByte()` |
| Estado | `Remaining`, `RemainingBits`, `Position`, `BitPosition`, `Capacity`, `IsAtEnd`, `IsReadOnly`, `HasRemaining(count)`, `HasRemainingBits(bitCount)`, `Data`, `Current` |
| Substream | `Slice(size, outSubStream)` |
| Leitura | `Read.Slice`, `Read.Bytes`, `Read.Bits`, `Read.UInt8`/`Int8`, `UInt16LE`/`BE`, `Int16LE`/`BE`, `UInt32LE`/`BE`, `Int32LE`/`BE`, `UInt64LE`/`BE`, `Int64LE`/`BE` |
| Inspeção sem avanço | `Peek.Bytes`, `Peek.UInt8` |
| Escrita | `Write.Bytes`, `Write.Bits` e as variantes correspondentes de inteiros/endianness. |

~~~c
MODULE_TYPE(Memory, Stream) stream = {0};
uint32_t value = 0U;

OPSTATUS status = MODULE_FUNC(Memory, Stream, Init, Read)(
    &stream, input, inputSize);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Memory, Stream, Read, UInt32BE)(
        &stream, &value);
}
~~~

A endianness é selecionada pelo nome da operação e é independente da ordem de bytes do host. O stream nunca aumenta o tamanho do buffer de apoio. Leituras e escritas validam a posição atual, o tamanho solicitado e a capacidade disponível.

---

## Collector

Cabeçalho: `Collector.h`.

O Collector combina alocação Heap com rastreamento de raízes, rastreamento de referências, pausa/retomada, suporte a finalizadores e uma interface global de conveniência.

### Collector por instância

Tipo: `MODULE_TYPE(Memory, Collector)`.

| Método | Descrição |
| --- | --- |
| `Create(initialHeapPages, stackBottom)` / `Destroy()` | Inicializa ou destrói um coletor independente. |
| `Alloc`, `Alloc.Typed`, `Alloc.Atomic` | Aloca armazenamento bruto, ciente de descritores ou atômico. |
| `SetFinalizer(pointer, finalizer)` | Associa um callback finalizador a uma alocação. |
| `PinRoot(rootPointerAddress)` / `UnpinRoot(rootPointerAddress)` | Registra ou remove o registro do endereço de um slot de ponteiro raiz. |
| `Collect()` | Executa um ciclo de coleta. |
| `Pause()` / `Resume()` | Suspende ou retoma temporariamente a coleta. |
| `AsAllocator()` | Expõe o coletor por meio da interface de alocador. |

### Collector global

O pacote `GlobalCollector` fornece `Startup(stackBottom)`, `Shutdown()`, `DefaultCollector()`, as operações correspondentes de alocação/finalizador/raiz, `Collect`, `Pause`/`Resume`, `LiveBytes` e `Contains`.

A API de raízes recebe o **endereço de uma variável ponteiro raiz**, não o endereço da alocação. Fixe e desafixe o mesmo slot raiz e mantenha esse slot válido enquanto estiver registrado. O argumento stack-bottom deve corresponder ao contrato de limites de pilha da plataforma/thread. Finalizadores podem executar código do usuário; a coleta não equivale a chamar `free` para uma única alocação.

---

## Famílias de smart pointers

Cabeçalho: `Pointer/Pointer.h`.

O agregador fornece famílias geradas para:

| Família | Modelo de propriedade |
| --- | --- |
| `Unique` | Um único identificador com propriedade exclusiva. |
| `Shared` | Propriedade compartilhada com contagem de referências. |
| `Weak` | Referência não proprietária a um bloco de controle Shared. |
| `AtomicShared` | Operações de contagem de referências Shared com atualizações atômicas do controle. |
| `AtomicWeak` | Operações atômicas de referências fracas. |
| `Intrusive` | Contagem de referências armazenada nos campos de controle do objeto do usuário. |

Os principais métodos gerados são:

| Família | Construção e propriedade | Acesso e estado |
| --- | --- | --- |
| `Unique` | `FromPointer`, `Alloc`, `AllocZero`, `Reset`, `Move`, `Release`, `Free` | `IsValid`, `Get`, `GetMutable`, `Swap` |
| `Shared` | `From.Pointer`, `From.Weak` (promoção), `Alloc`, `Alloc.Zero`, `Reset`, `Reset.WithPointer`, `Free` | `Get`, `Get.Mutable`, `UseCount`, `Unique`, `Swap` |
| `Weak` | `Clone`, `Move`, `Reset`, `Free` | `IsExpired`, `IsValid`, `UseCount`, `Swap` |
| `AtomicShared` | Operações `From`, `Alloc`, `Reset`, `Free` semelhantes às de Shared | `Get`, `Get.Mutable`, `UseCount`, `Unique` |
| `AtomicWeak` | Operações `Clone`, `Move`, `Reset`, `Free` semelhantes às de Weak | `IsExpired`, `IsValid`, `UseCount` |
| `Intrusive` | `FromPointer`, `FromExistingPointer`, `Clone`, `Move`, `Reset`, `Release`, `Free` | `Get`, `GetMutable`, `UseCount`, `Unique`, `Swap` |

O sufixo exato do tipo e os parâmetros de instanciação vêm das tabelas de tipos das famílias de ponteiros e dos templates `.def`; não presuma que todo tipo C arbitrário tenha uma especialização integrada. Cada família possui seu próprio contrato de propriedade, e `Free`/`Reset` não são intercambiáveis entre famílias.

Um ponteiro Weak não é proprietário do objeto. Promova-o a um ponteiro Shared antes de acessar o objeto e verifique o status retornado, pois o objeto pode já ter expirado. A contagem atômica de referências não torna o próprio objeto apontado seguro para uso entre threads.

---

## Block de largura fixa

Cabeçalho: `Block.h` (inclua diretamente; não faz parte de `Memory.h`).

Block gera objetos com largura definida em tempo de compilação, respaldados por limbs `uint64_t`. Oferece limpeza/definição de todos os bits, operações bit a bit `And`/`Or`/`Xor`/`Not`, deslocamentos, rotações, definição/limpeza/inversão/verificação de bits, contagem de bits, predicados any/all/none e igualdade. Use-o para máscaras de largura fixa e valores binários que não exigem redimensionamento dinâmico.

---

# Contratos e regras de propriedade

- Todo ponteiro retornado por um alocador deve ser liberado pela mesma família de alocadores que o criou.
- `Alloc` e `Realloc` recebem o endereço de uma variável ponteiro; passe `&pointer`.
- `Copy` exige intervalos sem sobreposição; `Move` permite sobreposição.
- Arena e Pool podem expor modos com buffers pertencentes ao chamador; não libere esses buffers pelo alocador bruto.
- Um Stream toma emprestado seu buffer e nunca é proprietário dele nem o aumenta.
- As raízes do Collector são endereços de slots de ponteiro, não endereços brutos de objetos.
- A semântica de propriedade dos smart pointers é específica de cada família; um ponteiro bruto retornado por um accessor não implica transferência automática de propriedade.
- Verifique cada `OPSTATUS` antes de utilizar um ponteiro de saída ou presumir que um recurso foi adquirido.

## Módulos relacionados

- [Types](Types.md) — descritores de ciclo de vida e operações com apagamento de tipo.
- [Condition](Condition.md) — convenções de status e erro.
- [Algorithm](Algorithm.md) — contratos de comparação e troca.
- [Bit](Bit.md) — operações de bits e bitsets de tamanho fixo.

## Cabeçalhos de origem

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
