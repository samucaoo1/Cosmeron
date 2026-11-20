# Contêineres (Container)

O módulo **Container** fornece estruturas de dados C geradas para armazenamento ordenado, armazenamento encadeado, busca associativa, árvores, grafos, iteração e coleções de bits. Ele pertence a **Modules**. Sua API pública é baseada em especializações tipadas `MODULE_TYPED`, funções de pacote `MODULE_FUNC` e métodos de objeto `OBJECT_CALL`.

## Cabeçalhos

~~~c
#include "Cosmeron/Modules/Container/Container.h"
~~~

O cabeçalho agregado inclui DynamicBitset, Iterator, Array, Linked, Tree, Hash e Graph. Cabeçalhos de pacotes individuais podem ser incluídos quando uma unidade de tradução precisar apenas de um subconjunto do módulo.

---

# Visão geral

| Família | Cabeçalho agregado | Estruturas |
| --- | --- | --- |
| Array | `Array/Array.h` | Vector, Queue, Stack, Ring, Span, Heap |
| Linked | `Linked/Linked.h` | Forward, List, Deque, Queue, Stack, Ring e tipos de nós |
| Hash | `Hash/Hash.h` | Set e Map |
| Tree | `Tree/Tree.h` | BST, AVL, RedBlack e BTree; variantes Set e Map |
| Graph | `Graph/Graph.h` | Grafo direcionado com vértices e arestas tipados |
| Iterator | `Iterator/Iterator.h` | Iteradores Forward, Bidirectional, Contiguous, Circular e Stream |
| Bitset | `Bitset/DynamicBitset.h` | Bitset de tamanho em tempo de execução |

Os templates Array e Linked são instanciados a partir da tabela de tipos de contêineres ordenados. Hash e Tree utilizam tabelas de tipos de chave/valor adequadas aos seus contratos de armazenamento e comparação. Utilize os sufixos declarados por essas tabelas; uma especialização gerada não é um tipo genérico em tempo de execução.

---

# Referência da API

## Convenção de chamada e ciclo de vida

Um tipo de objeto gerado é nomeado com `MODULE_TYPED(Module, Family, Suffix)`. Funções no nível do pacote são acessadas com `MODULE_FUNC(Module, Family, Suffix, Function)`. Após a criação, os métodos do objeto são chamados por meio de `OBJECT_CALL` (ou `OBJECT_CALL_0` para um método sem argumentos).

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

`Create` inicializa um objeto e adquire o armazenamento de apoio necessário. `Dispose` é a operação de limpeza que retorna status; `Destroy` é um procedimento conveniente que invoca a liberação. Verifique `OPSTATUS` nas operações sujeitas a falhas. Não utilize um objeto após ele ter sido liberado ou destruído.

A maioria dos contêineres armazena elementos por valor. Se o tipo do elemento for um ponteiro, o contêiner armazena esse valor de ponteiro; ele não assume automaticamente a propriedade nem libera o objeto apontado. Ponteiros retornados por acessores e iteradores podem ser invalidados por remoção, reserva, realocação, rehash ou destruição, dependendo da estrutura.

---

## Contêineres Array

O agregado Array é `Array/Array.h`.

### Vector

Cabeçalho: `Array/Vector.h`.

Vector é um array contíguo de crescimento dinâmico. É apropriado quando o acesso indexado e a iteração amigável ao cache são importantes.

| Grupo | Métodos |
| --- | --- |
| Construção | `Create(initialCapacity)`, `FromBuffer(buffer, count)`, `Clone(outClone)` |
| Armazenamento | `Reserve(newCapacity)`, `ShrinkToFit`, `Resize(newSize, fillValue)`, `Clear`, `Dispose`, `Destroy` |
| Acesso | `At`, `ConstAt`, `Get`, `Set`, `Front`, `Back`, `Data`, `ConstData` |
| Operações nas extremidades | `PushBack`, `PopBack`, `PushFront`, `PopFront` |
| Edição | `Insert(position, value)`, `Erase(position, count)`, `RemoveAt`, `SwapRemove` |
| Estado | `IsEmpty`, `Empty`, `Size`, `Count`, `Capacity` |

`At` retorna um ponteiro para elemento por meio de um parâmetro de saída; `Get` copia um elemento para um valor de saída. `Set` substitui um elemento. `SwapRemove` remove um elemento movendo o último elemento para sua posição, portanto não preserva a ordem dos elementos restantes.

### Queue

Cabeçalho: `Array/Queue.h`.

A Queue de array é uma estrutura FIFO apoiada por um array dinâmico. Ela fornece `Create`, `Compact`, `Reserve`, `ShrinkToFit`, `Clear`, `Push`/`Enqueue`, `Pop`/`Dequeue`, `Front`, `Back`, `Peek`, `IsEmpty`, `Size`, `Count` e `Capacity`.

Utilize Queue para agendamento de trabalho em ordem first-in, first-out. `Peek` lê o próximo valor sem removê-lo; `Pop`/`Dequeue` remove o valor da frente e o escreve pelo ponteiro de saída fornecido.

### Stack

Cabeçalho: `Array/Stack.h`.

A Stack de array é LIFO. Seus principais métodos são `Create`, `Reserve`, `ShrinkToFit`, `Clear`, `Push`, `Pop`, `Top`, `Peek`, `IsEmpty`, `Size`, `Count` e `Capacity`.

Utilize Stack para trabalho aninhado, análise sintática, histórico de desfazer e travessia em profundidade. `Top` fornece acesso ao elemento armazenado; `Peek` copia o valor para um parâmetro de saída sem removê-lo.

### Ring

Cabeçalho: `Array/Ring.h`.

Ring é um buffer circular de capacidade fixa. Fornece `Create(capacity)`, `FromBuffer`, `Dispose`, `Destroy`, `Clear`, `Push`/`PushBack`, `Pop`/`PopFront`, `Peek`, `Overwrite`, `Front`, `Back`, `At`, `IsEmpty`, `IsFull`, `Size`, `Count`, `Capacity` e `Available`.

As operações `Read`/`Write` e `GetReadSpan`/`GetWriteSpan` com `AdvanceRead`/`AdvanceWrite` expõem partes contíguas do buffer circular para transferência em lote. `Overwrite` pode retornar um elemento descartado por um ponteiro de saída; utilize `Push` normal quando os dados existentes não puderem ser descartados.

### Span

Cabeçalho: `Array/Span.h`.

Span é uma visualização não proprietária sobre um buffer contíguo existente. Fornece `FromBuffer`, `At`, `ConstAt`, `Get`, `Set`, `Front`, `Back`, `Data`, `ConstData`, `Size`, `Count`, `SizeBytes` e `IsEmpty`.

Span não aloca, realoca nem libera o buffer de origem. É útil para passar um intervalo delimitado a uma API sem copiar dados; seu tempo de vida não deve exceder o do armazenamento subjacente.

### Heap de prioridade

Cabeçalho: `Array/Heap.h`.

Este é um heap binário de prioridade, distinto de `Core/Memory/Heap.h`. Ele oferece construção de min-heap e max-heap por meio de `CreateMin` e `CreateMax`, além de `FromBuffer`, `Dispose`, `Destroy`, `Clear`, `Reserve`, `ShrinkToFit`, `Heapify`, `Push`, `Pop`, `Peek`, `Top`, `Replace`, `PushPop`, `Size`, `Count` e `Capacity`.

Utilize-o para filas de prioridade e seleção repetida do item de maior ou menor prioridade. Sua closure de comparação determina a ordenação; utilize o tipo de comparator gerado esperado pelo template, em vez de converter um ponteiro de função não relacionado.

---

## Contêineres Linked

O agregado Linked é `Linked/Linked.h`. Os contêineres encadeados utilizam nós alocados separadamente e são adequados a cargas de trabalho nas quais inserções e remoções em extremidades ou posições conhecidas importam mais do que o armazenamento contíguo.

| Cabeçalho | Estrutura | Operações principais |
| --- | --- | --- |
| `Linked/Forward.h` | Cadeia simplesmente encadeada | `Create`, `Clear`, `PushFront`, `PushBack`, `PopFront`, `Front`, `Back`, `Head`, `Tail`, `Size`, `Count`, `IsEmpty` |
| `Linked/List.h` | Lista duplamente encadeada | `Create`, `Clear`, `PushFront`, `PushBack`, `PopFront`, `PopBack`, `Front`, `Back`, `Head`, `Tail`, `Size`, `Count`, `IsEmpty` |
| `Linked/Deque.h` | Fila de duas extremidades | `Create`, `Clear`, `PushFront`, `PushBack`, `PopFront`, `PopBack`, `Front`, `Back`, `Size`, `Count`, `IsEmpty` |
| `Linked/Queue.h` | Fila FIFO encadeada | `Create`, `Clear`, `Push`/`Enqueue`, `Pop`/`Dequeue`, `Peek`, `Front`, `Back`, `Size`, `Count`, `IsEmpty` |
| `Linked/Stack.h` | Pilha LIFO encadeada | `Create`, `Clear`, `Push`, `Pop`, `Peek`, `Top`, `Size`, `Count`, `IsEmpty` |
| `Linked/Ring.h` | Estrutura circular encadeada | Operações de criação, inserção/remoção e travessia definidas pelo template Ring |

Os templates encadeados também fornecem acesso orientado a nós e closures visitor. Utilize o tipo de closure `Visitor` declarado pelo template correspondente ao percorrer elementos. Nós e ponteiros de elementos são gerenciados pelo contêiner; não libere manualmente um nó retornado por um acessor.

As estruturas encadeadas seguem a mesma convenção de ciclo de vida dos contêineres de array: crie ou inicialize antes de usar e depois chame `Dispose` ou `Destroy`. `Clear` remove os elementos e mantém o objeto contêiner utilizável.

---

## Contêineres Hash

O agregado Hash é `Hash/Hash.h` e inclui Set e Map. Ambos utilizam busca baseada em hash; não são contêineres ordenados.

### Hash Set

Cabeçalho: `Hash/Set.h`.

Set armazena chaves únicas. Sua interface inclui `Create`, `Clear`, `Dispose`, `Destroy`, `Rehash`, `Reserve`, `Insert`, `Remove`, `Find`, `Contains`, `IsEmpty`, `Size`, `Count` e `Capacity`. Seu iterador expõe `IsValid`, `HasNext`, `Reset`, `Get`, `ConstGet`, `Key`, `Value`, `StepNext` e `Next`.

Utilize Set para testes de pertencimento e unicidade. Igualdade de chave e hashing devem ser consistentes: chaves iguais devem produzir o mesmo valor hash. Rehash pode invalidar iteradores e ponteiros para elementos da tabela.

### Hash Map

Cabeçalho: `Hash/Map.h`.

Map associa uma chave a um valor. Fornece `Create`, `Clear`, `Dispose`, `Destroy`, `Rehash`, `Reserve`, `Insert`, `Upsert`, `Remove`, `Find`, `Contains`, `IsEmpty`, `Size`, `Count` e `Capacity`. Seu iterador fornece acesso a chave/valor e travessia para a frente.

`Insert` segue o contrato de inserção de uma nova chave; `Upsert` insere ou substitui o valor de uma chave existente. `Find(key, &valuePointer)` retorna o endereço de um valor armazenado quando encontrado.

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

## Árvores

O agregado Tree é `Tree/Tree.h`. Ele inclui implementações de árvore binária de busca (BST), AVL, Red-Black e B-Tree. Cada implementação possui variantes Set e Map geradas a partir de combinações de tipos de chave e valor.

As operações compartilhadas incluem `Create`, `Clear`, `Dispose`, `Destroy`, `Insert`, `Remove`, `Find`, `Contains`, `IsEmpty`, `Size` e `Count`. Dependendo da implementação, operações adicionais incluem `Min`/`Max`, `MinKey`/`MaxKey`, `Height` e travessia por iterador para frente ou bidirecional. Variantes Map também expõem `Upsert`.

| Estrutura | Característica | Utilize quando |
| --- | --- | --- |
| BST | Árvore binária de busca simples; a forma depende da ordem de inserção | Uma árvore ordenada básica é suficiente e o formato da entrada é controlado |
| AVL | Árvore binária de busca balanceada por altura | Altura limitada e desempenho de busca são prioridades |
| RedBlack | Árvore de busca balanceada rubro-negra | É necessária uma árvore ordenada balanceada com balanceamento favorável a atualizações |
| BTree | Árvore de busca multiway com grau mínimo configurável | A ramificação dos nós e a altura da árvore importam, inclusive em armazenamento orientado a páginas |

Um Set armazena chaves; um Map armazena pares chave/valor. Utilize o tipo de iterador e o contrato de ordenação da estrutura selecionada. Modificações estruturais podem invalidar iteradores e ponteiros para valores armazenados.

---

## Graph

Cabeçalho: `Graph/Graph.h`.

Graph gera objetos tipados de vértices e arestas e um contêiner de grafo. Suas operações públicas incluem `Create`, `Clear`, `Dispose`, `Destroy`, `AddVertex`, `RemoveVertex`, `AddEdge`, `RemoveEdge`, `GetVertex`, `HasEdge`, `OutDegree`, `VertexCount` e `EdgeCount`. Iteradores de vértices e arestas fornecem verificações de validade, acesso e travessia.

A API atual de Graph modela arestas direcionadas. Uma conexão bidirecional deve ser representada adicionando as duas direções. Considere as alterações de índices quando vértices forem removidos e verifique o status retornado antes de utilizar um índice ou ponteiro produzido por um parâmetro de saída.

---

## Iteradores

O agregado Iterator é `Iterator/Iterator.h` e inclui templates de iteradores tipados Stream, Contiguous, Circular, Forward e Bidirectional. Suas operações dependem da categoria de travessia subjacente; os conceitos compartilhados incluem `IsValid`, `HasNext`, `HasPrev`, `Remaining`, `Reset`, `Get`, `ConstGet`, `Value`, `Set`, `Next`, `Prev`, `StepNext`, `StepPrev`, `StepValue`, `StepPtr`, `Seek`, `Index` e `Advance`.

- Utilize Contiguous com armazenamento apoiado por arrays e movimentação baseada em índices.
- Utilize Forward para travessia em uma direção.
- Utilize Bidirectional quando a origem permitir movimentação nas duas direções.
- Utilize Circular quando a travessia der a volta na sequência subjacente.
- Utilize Stream quando a travessia for controlada por um contrato de callback/estado de stream.

Em geral, iteradores tomam emprestado o contêiner ou buffer. Destruir, aplicar rehash, redimensionar ou modificar estruturalmente a origem pode invalidá-los.

---

## Bitset dinâmico

Cabeçalho: `Bitset/DynamicBitset.h`.

`MODULE_TYPE(Bitset, Dynamic)` armazena uma quantidade de bits definida em tempo de execução e utiliza um `Memory_Allocator` configurável. Seus métodos incluem `Create`, `Create.WithAllocator`, `Dispose`, `Reserve`, `Resize`, `Clone`, `Clear`, `SetAll`, `And`, `Or`, `Xor`, `Not`, deslocamentos, operações de bit individual e intervalo, `Count`, `Size`, `Capacity`, `Any`, `All`, `None`, `Equal`, `Is.Subset`, `Is.Disjoint`, `Intersects` e `Find.FirstSet`/`FirstZero`/`NextSet`.

~~~c
MODULE_TYPE(Bitset, Dynamic) bits = {0};
OPSTATUS status = MODULE_FUNC(Bitset, Dynamic, Create)(&bits, 128U);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Bitset, Dynamic, Bit, Set)(&bits, 7U);
    (void)MODULE_FUNC(Bitset, Dynamic, Dispose)(&bits);
}
~~~

`DynamicBitset.h` desabilita sua function table de objeto; portanto, invoque esses métodos diretamente com `MODULE_FUNC(Bitset, Dynamic, Method)`, em vez de `OBJECT_CALL`. O bitset é proprietário dos limbs alocados e deve ser liberado. Um alocador personalizado deve permanecer válido durante toda a vida útil do bitset.

---

# Escolhendo um contêiner

| Requisito | Família recomendada |
| --- | --- |
| Armazenamento contíguo indexado e acesso aleatório | Array Vector |
| FIFO ou LIFO com armazenamento dinâmico | Array Queue / Array Stack |
| Armazenamento FIFO reutilizável e limitado | Array Ring |
| Visualização de dados contíguos existentes sem cópia | Span |
| Seleção repetida por prioridade | Array Heap |
| Inserção/remoção encadeada frequente | Linked List / Deque |
| Pertencimento e busca por chave | Hash Set / Hash Map |
| Travessia ordenada por chave | BST / AVL / RedBlack / BTree |
| Vértices e arestas direcionadas | Graph |
| Coleção de bits de tamanho em tempo de execução | Dynamic Bitset |

# Contratos e observações

- Utilize `MODULE_TYPED` para tipos de objeto gerados, `MODULE_FUNC` para funções de pacote e `OBJECT_CALL` para métodos de objeto.
- `Create` e métodos de mutação sujeitos a falhas retornam `OPSTATUS`; verifique-o antes de acessar o objeto.
- `Destroy` é um procedimento conveniente de limpeza; utilize `Dispose` quando o status for necessário.
- Contêineres armazenam valores C por valor, a menos que um tipo específico forneça explicitamente um wrapper de propriedade.
- Não presuma que um contêiner seja seguro para uso concorrente, a menos que seu próprio cabeçalho documente a sincronização.

## Módulos relacionados

- [Algorithm](Algorithm.md) — closures de comparação usadas por filas de prioridade e contêineres associativos.
- [Bit](Bit.md) — operações de bits escalares e de tamanho fixo.
- [Memory](Memory.md) — alocadores utilizados por contêineres dinâmicos e bitsets.
- [Module](Module.md) — modelo de declaração de objetos e pacotes tipados.
- [Types](Types.md) — especializações primitivas e traits de ciclo de vida.

## Cabeçalhos de origem

- `Codespace/Cosmeron/Modules/Container/Container.h`
- `Codespace/Cosmeron/Modules/Container/Array/Array.h`
- `Codespace/Cosmeron/Modules/Container/Linked/Linked.h`
- `Codespace/Cosmeron/Modules/Container/Hash/Hash.h`
- `Codespace/Cosmeron/Modules/Container/Tree/Tree.h`
- `Codespace/Cosmeron/Modules/Container/Graph/Graph.h`
- `Codespace/Cosmeron/Modules/Container/Iterator/Iterator.h`
- `Codespace/Cosmeron/Modules/Container/Bitset/DynamicBitset.h`
