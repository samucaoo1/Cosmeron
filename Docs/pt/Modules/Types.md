# Types

O módulo **Types** reúne primitivas reutilizáveis do sistema de tipos: catálogo canônico de tipos primitivos de C, traits de tipos, valores tagged, armazenamento com apagamento de tipo, valores opcionais, valores de resultado e tags de bytes de largura fixa.

## Headers

~~~c
#include "Cosmeron/Core/Types/Primitive.h"
#include "Cosmeron/Core/Types/Traits.h"
#include "Cosmeron/Core/Types/TaggedUnion.h"
#include "Cosmeron/Core/Types/Any.h"
#include "Cosmeron/Core/Types/Optional.h"
#include "Cosmeron/Core/Types/Result.h"
#include "Cosmeron/Core/Types/ByteTag.h"
~~~

Inclua headers individuais quando apenas um recurso for necessário. Os arquivos `Impl` e `TypeTable` são detalhes de implementação e não devem ser incluídos diretamente pelos consumidores.

---

# Visão geral

| Recurso | Finalidade | Armazenamento/ownership |
| --- | --- | --- |
| Primitive | Nomes canônicos para tipos nativos de C, categorias e tabelas de tipos primitivos. | Typedefs e enums; sem alocação. |
| Traits | Descreve tamanho, alinhamento, categorias, flags e callbacks de ciclo de vida/comparação/hash de um tipo. | Descritor POD estático, normalmente compartilhado por ponteiro. |
| TaggedUnion | Armazena um primitivo ou valor especial identificado por tag. | Tagged union inline; sem alocação para payloads nativos. |
| Any | Armazena um valor selecionado em runtime usando um descritor de tipo e small-buffer optimization. | Armazenamento inline quando couber; caso contrário, heap. |
| Optional | Representa um valor ou a ausência de valor para um tipo definido em compilação. | Valor inline. |
| Result | Representa um valor ou um erro Condition para um tipo definido em compilação. | Tagged union inline. |
| ByteTag | Armazena uma tag de bytes de largura fixa com conversões numéricas, binárias e de texto imprimível. | Inteiro de largura fixa e array de caracteres inline. |

---

# Referência da API

## Tipos primitivos

Header: `Primitive.h`.

O catálogo primitivo `Type` fornece typedefs C com namespace por meio de `MODULE_TYPE(Type, Name)`, incluindo:

- Booleano: `Boolean`;
- inteiros com sinal: `Int8`, `Int16`, `Int32`, `Int64`;
- inteiros sem sinal: `UInt8`, `UInt16`, `UInt32`, `UInt64`;
- tipos de caractere: `Char`, `Char8`, `Char16`, `Char32`;
- ponto flutuante: `Float32`, `Float64`, `LongDouble`;
- tipos dimensionados pela arquitetura: `Int`, `UInt`, `Size`, `PtrDiff`, `IntPtr`, `UIntPtr`;
- tipos complexos de ponto flutuante quando suportados pelo compilador.

O header também expõe o enum de categorias primitivas e as tabelas X-macro usadas por outros módulos. As tabelas agregadas mais úteis são:

| Tabela | Conteúdo |
| --- | --- |
| `PRIMITIVE_TYPE_TABLE_EXACT_INTEGER` | Inteiros com e sem sinal de largura fixa. |
| `PRIMITIVE_TYPE_TABLE_INTEGER` | Tipos inteiros de largura fixa e dimensionados pela arquitetura. |
| `PRIMITIVE_TYPE_TABLE_NUMERIC` | Tipos inteiros, de ponto flutuante e numéricos complexos. |
| `PRIMITIVE_TYPE_TABLE_ORDERED` | Tipos com ordenação nativa usados por algoritmos ordenados. |
| `PRIMITIVE_TYPE_TABLE` | Tipos ordenados mais tipos complexos. |

Use os aliases com namespace ao declarar interfaces compartilhadas com outros módulos do Cosmeron. Eles deixam explícita a família de tipos pretendida sem substituir os tipos ABI subjacentes de C.

---

## Type traits

Header: `Traits.h`.

`MODULE_TYPE(Type, Traits)` é um descritor POD que contém:

- nome, tamanho e alinhamento do tipo;
- um valor `Type_Category` e flags de traits;
- callbacks opcionais para inicialização, descarte, cópia, movimentação, comparação e hashing.

O package fornece descritores nativos para tipos primitivos e um descritor para C strings.

### Introspecção

| Método | Finalidade |
| --- | --- |
| `Is.Valid`, `Is.Equal` | Valida um descritor e compara identidade/conteúdo dos descritores. |
| `Is.Trivial` | Testa a flag de ciclo de vida trivial. |
| `Is.TrivialCopy`, `Is.TrivialCompare` | Testa se cópia/comparação podem usar operações triviais de bytes. |
| `Is.Numeric`, `Is.Signed`, `Is.Integer`, `Is.Float` | Testa categorias/flags numéricas. |
| `Is.Dynamic` | Testa se o tipo possui requisitos de ciclo de vida dinâmico. |
| `HasFlags` | Testa uma máscara de bits de flags do descritor. |
| `NeedsDispose` | Testa se o valor precisa ser descartado. |

### Operações de ciclo de vida e valor com apagamento de tipo

| Método | Contrato |
| --- | --- |
| `Init(traits, instance)` / `Init.Array` | Inicializa um valor ou um array. |
| `Dispose(traits, instance)` / `Dispose.Array` | Descarta um valor ou um array. |
| `Copy(traits, destination, source)` / `Copy.Array` | Copia um valor ou array usando a política registrada. |
| `Move(traits, destination, source)` / `Move.Array` | Move um valor ou array usando a política registrada. |
| `Swap(traits, left, right)` | Troca dois valores de acordo com o descritor. |
| `Compare(traits, left, right)` | Retorna uma comparação de três vias. |
| `Equals(traits, left, right)` | Testa igualdade. |
| `Hash(traits, instance)` | Retorna o hash do valor. |
| `Hash.Bytes(data, size)` | Calcula o hash de um intervalo bruto de bytes. |

Os acessores de descritores nativos recebem o nome de seu tipo primitivo, e `Traits.CString` fornece o descritor de C string.

Os ponteiros de callback de um descritor e o próprio descritor devem permanecer válidos durante todas as operações que os utilizarem. Um valor dinâmico não pode ser copiado com segurança usando `memcpy`, a menos que o contrato de traits permita explicitamente cópia trivial.

---

## TaggedUnion

Header: `TaggedUnion.h`.

`MODULE_TYPE(Type, TaggedUnion)` armazena um valor selecionado por um discriminador `MODULE_ENUM(Type, Tag)`. As tags nativas abrangem a tabela de tipos primitivos e categorias especiais, como vazio, C string, intervalo de bytes e ponteiro bruto.

A API pública inclui:

| Grupo | Operações |
| --- | --- |
| Ciclo de vida | `Init`, `Clear` |
| Valores primitivos | `Set_<Primitive>`, `From_<Primitive>`, `Get_<Primitive>` para entradas da tabela de tipos primitivos. |
| Valores especiais | `Set_CString`/`From_CString`/`Get_CString`; `Set_Bytes`/`From_Bytes`/`Get_Bytes`; `Set_RawPointer`/`From_RawPointer`/`Get_RawPointer`. |
| Conversão numérica | `Coerce.Int64`, `Coerce.UInt64`, `Coerce.Float64`. |
| Inspeção | `Is.Empty`, `Is.Tag`, `Is.Numeric`, `Is.Integer`, `Is.Signed`, `Is.Float`, `TagName`, `CategoryOf`, `TraitsOf`, `PayloadPointer`. |
| Operações de valor | `Compare`, `Equals`, `Hash`, `Traits`. |

~~~c
MODULE_TYPE(Type, TaggedUnion) value = {0};

OPSTATUS status = MODULE_FUNC(Type, TaggedUnion, Set_Int32)(&value, 42);
int32_t extracted = 0;

if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Type, TaggedUnion, Get_Int32)(
        &value, &extracted);
}
~~~

Os getters são estritos: esperam que a tag ativa corresponda ao tipo solicitado. Use as operações `Coerce` quando desejar uma conversão numérica verificada. Elas escrevem em um ponteiro de saída e informam falhas de conversão por meio de `OPSTATUS`.

`ByteSlice` armazena um ponteiro e um comprimento sem ownership. Payloads de C string, intervalo de bytes e ponteiro bruto são valores emprestados; TaggedUnion não duplica nem libera sua memória de suporte.

---

## Any

Header: `Any.h`.

`MODULE_TYPE(Type, Any)` é um contêiner de valores com apagamento de tipo e small-buffer optimization. O valor padrão usa a capacidade de small buffer configurada no header; variantes geradas por tamanho incluem `Any32`, `Any64`, `Any128` e `Any256`.

O objeto armazena um ponteiro para `Type_Traits` e bytes inline ou um ponteiro para heap. Suas operações de ciclo de vida usam os callbacks do descritor.

### Ciclo de vida e gerenciamento de valores

| Método | Contrato |
| --- | --- |
| `Init`, `Reset`, `Dispose` | Inicializa, limpa ou descarta o valor armazenado. |
| `Emplace(traits, sourceInstance)` | Copia um valor tipado para o contêiner. |
| `MoveFrom(traits, sourceInstance)` | Move um valor tipado para o contêiner. |
| `Clone(destination, source)` | Clona um valor usando a política de traits registrada. |
| `Move(destination, source)` | Transfere o valor armazenado. |
| `Swap(left, right)` | Troca dois contêineres. |

### Introspecção e acesso

| Método | Contrato |
| --- | --- |
| `HasValue`, `Is.Empty` | Testa se há um valor presente. |
| `Is.Inline` | Testa se o payload atual cabe no armazenamento inline. |
| `Is.Type(expectedTraits)` | Testa a identidade do descritor. |
| `TraitsOf` | Retorna o descritor ativo ou null para um valor vazio. |
| `Capacity` | Retorna a capacidade do armazenamento inline para a especialização padrão. |
| `RawPointer`, `MutableRawPointer` | Retorna um ponteiro emprestado para o payload armazenado. |
| `Get(expectedTraits, outValue)` | Copia estritamente um valor do tipo esperado para o armazenamento do chamador. |
| `Borrow`, `Borrow.Mutable` | Retorna um ponteiro emprestado após validar os traits esperados. |

O package também fornece atalhos primitivos `Set_<Primitive>` e `Get_<Primitive>`, conversões de/para `TaggedUnion`, comparação, igualdade e hashing.

~~~c
MODULE_TYPE(Type, Any) value = {0};
int32_t source = 42;
int32_t extracted = 0;

const MODULE_TYPE(Type, Traits) *traits =
    MODULE_FUNC(Type, Traits, Int32)();

OPSTATUS status = MODULE_FUNC(Type, Any, Emplace)(
    &value, traits, &source);

if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Type, Any, Get)(
        &value, traits, &extracted);
}

MODULE_FUNC(Type, Any, Dispose)(&value);
~~~

O descritor deve descrever corretamente o tipo de origem, e o objeto de origem deve ser válido para a operação. Um ponteiro retornado por `RawPointer` ou `Borrow` é emprestado do objeto Any; pode tornar-se inválido após substituição, movimentação, descarte ou qualquer operação que altere seu armazenamento.

Sempre descarte um valor Any antes que seu armazenamento saia de escopo, caso seu descritor exija descarte. Os setters/getters primitivos convenientes não alteram essa regra de ciclo de vida.

---

## Optional

Header: `Optional.h` e definição de template `Optional.def`.

Instancie uma especialização para o tipo C que deseja armazenar:

~~~c
TEMPLATE_INSTANTIATE(Optional, Int32, int32_t);
MODULE_TYPED(Optional, Int32) value = {0};
~~~

O objeto gerado armazena `hasValue` e um `value` inline.

| Método | Contrato |
| --- | --- |
| `Some(value)` | Armazena um valor e marca o Optional como presente. |
| `None()` / `Clear()` | Limpa o valor armazenado. |
| `FromPointer(ptr)` | Armazena o valor apontado ou cria None se o ponteiro for nulo. |
| `HasValue`, `Is.Some`, `Is.None` | Testa o estado. |
| `Value(outValue)` | Copia o valor para a saída; retorna `NOT_FOUND` quando vazio. |
| `Value.Or(fallback)` | Retorna o valor armazenado ou o fallback. |
| `Value.Pointer` / `MutablePointer` | Retorna um ponteiro emprestado ou null. |
| `Map(mapper, outOptional)` | Transforma um valor presente em outro valor do mesmo tipo de especialização. |
| `AndThen(binder, outOptional)` | Invoca um binder que grava um Optional de saída. |
| `Filter(predicate, outOptional)` | Preserva o valor somente quando o predicado retorna true. |

`Map`, `AndThen` e `Filter` usam delegates gerados. Exigem objetos de entrada/saída válidos e delegates válidos. Optional não aloca memória para o valor armazenado.

---

## Result

Header: `Result.h` e definição de template `Result.def`.

Result combina um valor de um tipo escolhido com `Condition_Error`:

~~~c
TEMPLATE_INSTANTIATE(Result, Int32, int32_t);
MODULE_TYPED(Result, Int32) result = {0};
~~~

| Método | Contrato |
| --- | --- |
| `Ok(value)` | Armazena um valor bem-sucedido. |
| `Err(error)` | Armazena um erro Condition. |
| `FromStatus(status)` | Cria sucesso para `SUCCESS` ou um Error a partir de outro status. |
| `Is.Ok`, `Is.Error` | Testa a alternativa ativa. |
| `Value(outValue)` | Copia o valor; na alternativa de erro, retorna o status desse erro. |
| `Value.Or(fallback)` | Retorna o valor ou o fallback. |
| `Error(outError)` | Copia o erro armazenado quando Result contém um erro. |
| `Map(mapper, outResult)` | Transforma um valor bem-sucedido e preserva um erro. |
| `AndThen(binder, outResult)` | Invoca um binder para o valor bem-sucedido e propaga um erro. |

Use Result quando uma função precisar retornar um valor tipado ou um objeto de erro mais rico. Use Optional quando a ausência for um estado válido e não exigir uma explicação de erro.

---

## ByteTag

Header: `ByteTag.h` e definição de template `ByteTag.def`.

As tags nativas são `ByteTag16`, `ByteTag32` e `ByteTag64`. Cada uma armazena um inteiro sem sinal da largura selecionada e uma view de texto terminada em NUL dos bytes da tag.

| Grupo | Operações |
| --- | --- |
| Construção | `From.String(chars)`, `From.UInt(value)`, `From.Bytes(bytes, length)` |
| Entrada binária | `Read.BE`, `Read.LE` |
| Saída binária | `Write.BE`, `Write.LE` |
| Conversão | `To.String`, `To.UInt` |
| Inspeção | `Text`, `IsPrintable`, `Equal`, `Compare` |

`From.String` e `From.Bytes` empacotam os dados na largura fixa da tag e preenchem as posições não utilizadas com espaços. `Read.BE`/`Read.LE` consomem exatamente a largura da tag a partir de um ponteiro de origem fornecido pelo chamador; `Write.BE`/`Write.LE` gravam exatamente essa largura em um ponteiro de destino. Usam ordenação explícita de bytes, independente da ordem de bytes do host. O chamador deve fornecer pelo menos a largura da tag em bytes legíveis ou graváveis. `To.String` exige um buffer de destino grande o suficiente para todos os bytes da tag mais o NUL terminador. Bytes não imprimíveis são representados por meio da formatação de texto imprimível do módulo, em vez de serem presumidos como caracteres de exibição válidos.

Os helpers em tempo de compilação no header podem construir constantes comuns a partir de literais de bytes. Trate ByteTag como um identificador compacto ou uma tag no estilo FourCC, não como uma string de caracteres genérica.

---

# Contratos e ownership

- Um descritor POD ou valor tagged não gerencia automaticamente o ciclo de vida dos ponteiros armazenados dentro dele.
- `Traits` é a autoridade para inicialização, cópia, movimentação, descarte, comparação e hashing com apagamento de tipo.
- `Any` pode alocar quando o valor não couber em sua capacidade inline; sempre siga seu contrato de descarte.
- Os getters de `TaggedUnion` fazem leituras estritas de tipo/tag; use `Coerce` para conversões numéricas.
- `Optional` e `Result` são tipados em compilação e armazenam suas alternativas inline.
- `ByteTag` tem largura fixa. Use operações endian explícitas para dados serializados.
- Especializações de templates devem usar a ordem exata de argumentos e o tipo esperado pelo template `.def` correspondente.

## Módulos relacionados

- [Condition](Condition.md) — Error e OPSTATUS usados por Result e operações de tipo que podem falhar.
- [Algorithm](Algorithm.md) — resultados de comparação e callbacks usados por traits.
- [Memory](Memory.md) — alocação bruta e interfaces de alocadores.
- [Module](Module.md) — declarações geradas de POD, package, objeto e templates.

## Headers de origem

- `Codespace/Cosmeron/Core/Types/Primitive.h`
- `Codespace/Cosmeron/Core/Types/Traits.h`
- `Codespace/Cosmeron/Core/Types/TaggedUnion.h`
- `Codespace/Cosmeron/Core/Types/Any.h`
- `Codespace/Cosmeron/Core/Types/Optional.def`
- `Codespace/Cosmeron/Core/Types/Result.def`
- `Codespace/Cosmeron/Core/Types/ByteTag.def`
