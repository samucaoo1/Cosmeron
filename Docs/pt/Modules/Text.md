# Text

O módulo **Text** fornece classificação e codificação Unicode, views sem ownership, strings mutáveis, utilitários para C strings, formatação, texto com atributos e grids de caracteres apoiados em memória. É uma API C11 organizada em packages, estruturas tipadas e objetos.

## Headers

~~~c
#include "Cosmeron/Modules/Text/Text.h"
~~~

O agregador inclui as seguintes áreas públicas:

| Header | Recurso |
| --- | --- |
| `Unicode/Codepoint.h` | Classificação de pontos de código Unicode. |
| `Encoding/UTF8.h`, `Encoding/UTF16.h` | Codificação, decodificação e validação UTF-8 e UTF-16. |
| `Unicode/Width.h` | Largura aproximada em colunas de exibição. |
| `View/View.h` | Views tipadas de strings sem ownership. |
| `String/String.h` | Strings mutáveis com ownership e utilitários UTF-8. |
| `String/CString/CString.h` | Operações com C strings. |
| `String/Builder/Builder.h` | Construção incremental de strings UTF-8. |
| `Format/Format.h`, `Format/BytesHuman.h` | Formatação limitada e conversão de tamanhos de bytes em texto. |
| `Grid/Attribute.h`, `Grid/Char.h`, `Grid/Grid.h` | Planos de atributos e grids de texto apoiados em memória. |
| `Attributed/Attributed.h` | Texto UTF-8 com trechos estilizados. |

---

# Visão geral

Text está dividido em validação/codificação Unicode, views sem ownership, strings com ownership, builders incrementais, formatação limitada, grids de caracteres/atributos em memória e saída UTF-8 com atributos. Esses recursos são primitivas de dados e formatação; não fornecem entrada de terminal nem backend de renderização.

---

# Referência da API

## Unicode e codificações

### Codepoint

Header: `Unicode/Codepoint.h`. Package: `Text_Codepoint`.

O package expõe predicados sob `Is`:

| Função | Significado |
| --- | --- |
| `Valid(codepoint)` | Verifica se o valor está no intervalo U+0000..U+10FFFF. |
| `Scalar(codepoint)` | Verifica se o valor é um escalar Unicode (não um surrogate). |
| `ASCII(codepoint)` | Verifica se o valor está no intervalo U+0000..U+007F. |
| `Control(codepoint)` | Verifica os intervalos de controle C0 e C1. |
| `Whitespace(codepoint)` | Verifica os intervalos de whitespace explicitamente implementados pelo módulo. |

~~~c
bool scalar = MODULE_FUNC(Text, Codepoint, Is, Scalar)(codepoint);
~~~

Use `Scalar` antes de codificar. Apenas `Valid` permite pontos de código surrogate, que não são valores escalares Unicode.

### UTF8

Header: `Encoding/UTF8.h`. Os buffers de entrada e saída usam bytes `unsigned char`.

| Função | Contrato |
| --- | --- |
| `EncodedLength(codepoint, outUnits)` | Informa o número de bytes UTF-8 necessários. |
| `Encode(codepoint, output, capacity, outUnits)` | Codifica um escalar em um buffer de bytes fornecido pelo chamador. |
| `Decode(input, size, outCodepoint, outUnits)` | Decodifica uma sequência e informa os bytes consumidos. |
| `Validate(input, size)` | Retorna se todo o intervalo de bytes é UTF-8 válido. |
| `Count(input, size, outCodepoints, outUnits)` | Conta pontos de código e informa as unidades consumidas. |

~~~c
unsigned char encoded[4];
size_t written = 0U;

OPSTATUS status = MODULE_FUNC(Text, UTF8, Encode)(
    (MODULE_TYPE(Text, Char32))0x1F642,
    encoded, sizeof(encoded), &written);
~~~

A capacidade do buffer de saída é medida em bytes. A decodificação recebe a quantidade de bytes disponíveis e grava o ponto de código decodificado e a quantidade de bytes consumidos por meio de parâmetros de saída. Verifique cada status antes de ler os valores de saída.

### UTF16

Header: `Encoding/UTF16.h`. A entrada e a saída usam unidades de código `MODULE_TYPE(Text, Char16)`.

| Função | Contrato |
| --- | --- |
| `EncodedLength(codepoint, outUnits)` | Informa uma ou duas unidades de código UTF-16. |
| `Encode(codepoint, output, capacity, outUnits)` | Codifica um escalar em um buffer UTF-16 fornecido pelo chamador. |
| `Decode(input, size, outCodepoint, outUnits)` | Decodifica um ponto de código e informa as unidades consumidas. |
| `Validate(input, size)` | Valida um intervalo completo de unidades de código UTF-16. |

A capacidade UTF-16 e os valores de tamanho consumido são medidos em unidades de código, não em bytes. Um escalar suplementar exige um par surrogate.

### Largura de exibição

Header: `Unicode/Width.h`. Package: `Text_Width`.

| Função | Contrato |
| --- | --- |
| `Codepoint(codepoint, outWidth)` | Informa a largura de exibição de um ponto de código. |
| `UTF8(input, size, outColumns, outUnits)` | Mede as colunas de exibição de um intervalo UTF-8 e informa os bytes consumidos. |

O cálculo de largura é uma estimativa orientada à exibição, não um mecanismo completo de composição tipográfica ou layout de terminal. Marcas combinantes e caracteres largos podem afetar a quantidade de colunas de forma diferente da quantidade de unidades de código codificadas.

---

## Views e strings

### View

Header: `View/View.h`.

Os tipos não proprietários gerados são `MODULE_TYPE(Text, View8)`, `View16` e `View32`. Eles apontam para um armazenamento existente e carregam um comprimento explícito.

| Função | Descrição |
| --- | --- |
| `Create(data, length)` | Cria uma view sobre um ponteiro e uma quantidade de unidades de código. |
| `FromCStr(cstr)` | Cria uma view a partir de uma string terminada. |
| `Empty()` | Retorna uma view vazia. |
| `Length`, `IsEmpty`, `Data` | Inspecionam a view. |
| `At(index, outValue)` | Lê uma unidade de código com verificação de limites. |
| `Slice(start, count)`, `Prefix(count)`, `Suffix(count)` | Retornam uma subview. |
| `Compare(left, right)`, `Equal` | Comparam views. |
| `Find` | Procura uma unidade de código, outra view ou uma C string. |
| `StartsWith`, `EndsWith` | Testam um prefixo ou sufixo. |

Uma view não aloca, não copia e não acrescenta um terminador. Ela só é válida enquanto o armazenamento subjacente permanecer válido e não for movido. Comprimentos e índices são unidades de código, exceto quando uma operação especificamente consciente de pontos de código é utilizada.

### Strings com ownership

Header: `String/String.h`. O agregador gera `String8`, `String16` e `String32`. Os três usam a mesma API gerada; o tipo do elemento é diferente:

| Tipo | Tipo do elemento | Unidade do comprimento |
| --- | --- | --- |
| `String8` | `Text_Char8` (`uint8_t`) | Bytes/unidades de código |
| `String16` | `Text_Char16` (`uint16_t`) | Unidades de código de 16 bits |
| `String32` | `Text_Char32` (`uint32_t`) | Unidades de código de 32 bits |

Os métodos de objeto gerados são:

| Grupo | Métodos | Comportamento |
| --- | --- | --- |
| Ciclo de vida | `Create(initialCapacity)`, `Destroy()`, `Clone(source)` | Cria, libera ou clona o armazenamento próprio. |
| Capacidade | `Reserve(requiredCapacity)`, `ShrinkToFit()` | Aumenta ou reduz a capacidade. |
| Construção | `FromBuffer(buffer, count)`, `FromCStr(cstr)`, `FromView(view)` | Inicializa a partir de unidades de código, uma string terminada ou uma view. |
| Mutação | `Clear()`, `AppendBuffer(buffer, count)`, `Append(cstr)`, `AppendString(other)`, `AppendView(view)`, `PushBack(value)`, `PopBack(outValue)`, `SetAt(index, value)`, `Insert(index, buffer, count)`, `InsertChar(index, value)`, `Erase(index, count)` | Atualiza a sequência armazenada de unidades de código. |
| Acesso | `Length()`, `Capacity()`, `IsEmpty()`, `CStr()`, `At(index, outValue)` | Lê tamanho, capacidade, estado vazio, dados ou uma unidade de código. |
| Views e recortes | `View()`, `Slice(start, count)`, `Substr(start, count, outSubstr)` | Cria uma view emprestada ou uma nova substring com ownership. |
| Comparação | `Compare(other)`, `Equal(other)`, `Equal.CStr(cstr)`, `Equal.View(view)`, `StartsWith(prefix)`, `EndsWith(suffix)` | Compara conteúdo ou testa prefixos/sufixos. |
| Busca | `Find(needle, outIndex)`, `Find.Str(needle, outIndex)` | Encontra uma unidade de código ou uma string terminada. |

`String` armazena uma unidade de código zero ao final, mas o comprimento informado exclui esse terminador. Comprimentos e índices são unidades de código, não contagens de escalares Unicode. Em `String8`, uma unidade de código é um byte UTF-8; operações como `At`, `SetAt`, `Erase` e `Slice` não garantem limites de pontos de código.

~~~c
MODULE_TYPE(Text, String8) text = {0};
const MODULE_TYPE(Text, Char8) input[] = "Cosmeron";

OPSTATUS status = OBJECT_CALL(&text, Create, 16U);
if (status == STATUS_CONST(SUCCESS)) {
    status = OBJECT_CALL(&text, Append, input);
}
if (status == STATUS_CONST(SUCCESS)) {
    size_t length = OBJECT_CALL_0(&text, Length);
    const MODULE_TYPE(Text, Char8) *data = OBJECT_CALL_0(&text, CStr);
    (void)length;
    (void)data;
}
(void)OBJECT_CALL_0(&text, Destroy);
~~~

A string é proprietária de sua alocação de suporte. Um ponteiro retornado por `CStr` é emprestado e pode ser invalidado por mutação, alterações de capacidade ou destruição. Use `View`/`Slice` quando um intervalo sem ownership for suficiente e mantenha o objeto proprietário vivo enquanto a view for utilizada.

### Utilitários de C strings

Header: `String/CString/CString.h`. Package: `Text_CString`.

O package fornece:

| Grupo | Métodos |
| --- | --- |
| Comprimento | `Length`, `Length.Bounded` |
| Classificação | `IsEmpty`, `IsAscii`, `IsAscii.Bounded` |
| Cópia e concatenação | `Copy`, `Copy.Bounded`, `Concat`, `Concat.Bounded` |
| Comparação | `Compare`, `Compare.Bounded`, `Compare.IgnoreCase`, `Compare.IgnoreCaseBounded` |
| Igualdade | `Equal`, `Equal.Bounded`, `Equal.IgnoreCase`, `Equal.IgnoreCaseBounded` |
| Prefixo/sufixo | `StartsWith`, `StartsWith.Bounded`, `StartsWith.IgnoreCase`, `EndsWith`, `EndsWith.Bounded`, `EndsWith.IgnoreCase` |
| Busca | `FindChar`, `FindLastChar`, `FindSubstring` e suas variantes limitadas |
| Span | `Span`, `ComplementSpan` |
| Trim | `Trim`, `Trim.Start`, `Trim.End` |
| Contagem/substituição | `CountChar`, `CountChar.Bounded`, `ReplaceChar`, `ReplaceChar.Bounded` |
| Alocação | `Duplicate`, `Duplicate.Bounded`, `Free` |

Prefira operações limitadas sempre que a extensão disponível for conhecida e a entrada puder não conter um terminador dentro desse intervalo. `Copy` e `Concat` recebem a capacidade do destino. `Duplicate` aloca uma nova string; libere-a com `Free`.

### String Builder

Header: `String/Builder/Builder.h`. Tipo: `MODULE_TYPE(Text, Builder)`.

O Builder é um buffer UTF-8 de crescimento dinâmico projetado para composição incremental.

| Método | Descrição |
| --- | --- |
| `Create(initialCapacity)`, `Destroy` | Cria/libera a alocação de suporte. |
| `Clear`, `Reserve(newCapacity)`, `ShrinkToFit` | Controla o conteúdo e a capacidade. |
| `Append.Char` | Acrescenta um caractere de um byte. |
| `Append.CStr` | Acrescenta uma C string. |
| `Append.Buffer` | Acrescenta um buffer de bytes com tamanho explícito. |
| `Append.View` | Acrescenta uma view UTF-8. |
| `Append.Codepoint` | Acrescenta um ponto de código Unicode. |
| `Append.Format`, `Append.FormatVariadic` | Acrescenta saída formatada. |
| `BuildString(outString)` | Materializa o conteúdo como uma String8 com ownership. |
| `Length`, `Capacity`, `IsEmpty`, `CStr`, `View` | Observa o conteúdo atual. |

O Builder é proprietário de seu buffer. `CStr` e `View` retornam referências emprestadas e não devem ser mantidas entre operações que possam realocar o buffer nem após sua destruição.

---

## Formatação

### Format

Header: `Format/Format.h`. Package: `Text_Format`.

| Função | Contrato |
| --- | --- |
| `Variadic(buffer, capacity, outWritten, format, arguments)` | Formatador limitado que recebe uma `va_list`. |
| `Variadic.Unsafe(buffer, outWritten, format, arguments)` | Formatador sem limite; o chamador deve garantir espaço suficiente. |
| `Buffer(buffer, capacity, outWritten, format, ...)` | Wrapper variádico seguro e conveniente. |
| `Unsafe(buffer, outWritten, format, ...)` | Wrapper variádico inseguro e conveniente. |

Prefira `Buffer` ou `Variadic`. As formas inseguras omitem intencionalmente o argumento de capacidade da saída e só devem ser usadas quando o tamanho do destino estiver garantido.

### Tamanhos de bytes legíveis por humanos

Header: `Format/BytesHuman.h`. Package: `Text_BytesHuman`.

| Função | Descrição |
| --- | --- |
| `Format(buffer, capacity, outWritten, bytes)` | Formata uma contagem de bytes usando o padrão de unidades e a precisão padrão. |
| `Format.Ex(buffer, capacity, outWritten, bytes, unitStandard, precision)` | Seleciona unidades IEC/SI e precisão. |
| `Parse(text, outBytes)` | Analisa um valor textual de tamanho em bytes para `uint64_t`. |

O enum de configuração define base IEC 1024, base SI 1000, precisão padrão 2 e precisão máxima 6. O formatador grava em um buffer pertencente ao chamador e informa a quantidade de bytes escritos.

---

## Grids de caracteres e atributos

### Escopo e layout

As operações de Grid trabalham com células apoiadas em memória. O módulo **não** escreve em stdout, controla um terminal, lê stdin, chama ncurses nem exibe um canvas. Renderização e entrada de terminal pertencem à camada da aplicação/backend.

As variantes de caracteres usam `Char8`, `Char16` e `Char32`. Dimensões e posições do grid usam `MODULE_TYPED(Tuple, Dual, UInt16)`; regiões retangulares usam `MODULE_TYPED(Tuple, Quad, UInt16)`.

### Grid somente de caracteres

Header: `Grid/Char.h`. Os tipos de objeto gerados seguem `MODULE_TYPED(Text, Char, Char8)`, `Char16` e `Char32`.

O grid Char armazena apenas caracteres. Seus métodos de objeto incluem `Create(size)`, `Destroy`, `Resize(size)`, `Recreate(size)`, `Clone(source)`, `Clear`, `ReadCell(position, outCharacter)`, `WriteCell(position, character)`, `Fill(character)`, `FillRegion(region, character)`, `Read(region, destination)` e `Write`/`Write.Region`. `IsValid` verifica o estado do objeto.

### Grid de texto composto

Header: `Grid/Grid.h`. Os tipos de objeto gerados são `MODULE_TYPED(Text, Grid, Char8)`, `Char16` e `Char32`.

Um grid composto armazena um plano de caracteres junto com um plano de atributos. Seus métodos incluem `Create`, `Destroy`, `Resize`, `Recreate`, `Clone`, `Clear`, `Clear.With`, `ReadCell`, `WriteCell`, `Fill`, `FillRegion`, `Read`, `Write`, `WriteRegion` e `Blit`/`Blit.Region`.

O grid é proprietário de seus planos de suporte. Uma operação de resize/recreate/destroy pode invalidar ponteiros de células ou dados anteriormente emprestados. Use o status retornado por todas as operações mutáveis.

### Plano de atributos e cores

Header: `Grid/Attribute.h`.

A API de atributos expõe `MODULE_TYPE(Text, Attribute, Cell)` e `MODULE_TYPE(Text, Attribute, Grid)`. Os flags de atributo incluem negrito, fraco, itálico, blink, inverso, oculto, tachado, sobrelinha, fundo transparente, continuação de unidade de código e primeiro plano/fundo padrão.

O package `Attribute` expõe `Default`, `IsEmpty`, `IsValid`, `Create`, `Destroy`, `Resize`, `Recreate`, `Clone`, `Clear`, `Clear.With`, `Fill`, `ReadCell`, `WriteCell`, `FillRegion`, `Read`, `Write` e `Write.Region`.

`Grid_Color` fornece `RGB(red, green, blue)`, `Red(color)`, `Green(color)`, `Blue(color)` e `WindowsPalette()`. A paleta Windows é uma tabela estática de 16 cores; é um utilitário de cores, não um backend de terminal.

---

## Texto com atributos

Header: `Attributed/Attributed.h`. Tipo: `MODULE_TYPE(Text, Attributed)`.

Texto com atributos armazena conteúdo UTF-8 com intervalos de atributos associados.

| Método | Descrição |
| --- | --- |
| `Create(initialCapacity)`, `Destroy` | Cria/libera o armazenamento de texto e intervalos. |
| `Clear` | Limpa texto e intervalos. |
| `Clear.Spans` | Remove os intervalos de estilo mantendo o texto. |
| `SetText(text, length)`, `AppendText(text, length)` | Substitui ou acrescenta um intervalo de bytes UTF-8 com tamanho explícito. |
| `AddSpan(start, length, attribute)` | Associa uma célula de atributo a um intervalo de texto. |
| `GetAttributeAt(index, outAttribute)` | Obtém o atributo em um índice de byte. |
| `RenderAnsi(outBuffer, capacity, outWritten)` | Renderiza sequências de escape ANSI em um buffer pertencente ao chamador. |
| `Text`, `Length`, `SpanCount`, `Spans` | Inspeciona o conteúdo atual. |

`Attributed.h` desativa a FunctionTable de objetos Text; chame seus métodos diretamente por `MODULE_FUNC(Text, Attributed, Method)`, em vez de usar `OBJECT_CALL`. Os arrays de texto e intervalos são gerenciados pelo objeto Attributed. Os ponteiros de texto/intervalos retornados são emprestados. `RenderAnsi` apenas produz uma string; não a imprime nem configura o terminal.

---

# Contratos e regras de ownership

- Encoders, formatadores, operações de grid e operações de string que escrevem em buffers retornam `OPSTATUS`; verifique-o antes de ler as saídas.
- Comprimentos UTF-8 são bytes; comprimentos UTF-16 são unidades de código; contagens de pontos de código são outra unidade.
- Uma View não possui os dados. String e Builder são proprietários de seus buffers.
- Objetos Grid e Attributed são proprietários de seu armazenamento de suporte e devem ser destruídos.
- Ponteiros retornados por `Data`, `CStr`, `Text` e `Spans` são emprestados e podem ser invalidados por mutação ou destruição.
- O grid é uma estrutura de memória off-screen, não uma UI de terminal, renderizador ou subsistema de entrada.

## Módulos relacionados

- [Tuple](Tuple.md) — dimensões e regiões do grid.
- [Memory](Memory.md) — alocação e operações com bytes.
- [Algorithm](Algorithm.md) — resultados de comparação usados por views de texto.
- [Module](Module.md) — packages, objetos e especializações tipadas geradas.

## Headers de origem

- `Codespace/Cosmeron/Modules/Text/Text.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Codepoint.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Width.h`
- `Codespace/Cosmeron/Modules/Text/Encoding/UTF8.h`
- `Codespace/Cosmeron/Modules/Text/Encoding/UTF16.h`
- `Codespace/Cosmeron/Modules/Text/View/View.def`
- `Codespace/Cosmeron/Modules/Text/String/String.def`
- `Codespace/Cosmeron/Modules/Text/String/CString/CString.h`
- `Codespace/Cosmeron/Modules/Text/String/Builder/Builder.h`
- `Codespace/Cosmeron/Modules/Text/Format/Format.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Attribute.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Char.def`
- `Codespace/Cosmeron/Modules/Text/Grid/Grid.def`
- `Codespace/Cosmeron/Modules/Text/Attributed/Attributed.h`
