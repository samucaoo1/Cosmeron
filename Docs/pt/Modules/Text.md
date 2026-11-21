# Text

O módulo **Text** fornece classificação e transcodificação Unicode, utilitários para C strings, views sem ownership, strings mutáveis com ownership, formatação desacoplada por streaming e grids de caracteres e atributos apoiados em memória. É uma API C11 organizada em packages, structs POD puras e objetos.

## Headers

~~~c
#include "Cosmeron/Modules/Text/Text.h"
~~~

O agregador inclui as seguintes áreas públicas:

| Header | Recurso |
| --- | --- |
| `Unicode/Codepoint.h` | Classificação de pontos de código Unicode. |
| `Unicode/UTF8.h`, `Unicode/UTF16.h` | Codificação, decodificação, validação, contagem e transcodificação UTF-8 e UTF-16. |
| `Unicode/Width.h` | Largura aproximada em colunas de exibição. |
| `CString/CString.h` | Operações stateless com C strings terminadas em NUL. |
| `View/View.h` | Views tipadas de strings sem ownership (`View8`, `View16`, `View32`). |
| `String/String.h` | Strings mutáveis com ownership (`String8`, `String16`, `String32`) e utilitários UTF-8. |
| `Format/Writer.h` | Sink de texto desacoplado (`Counter`, `ToBuffer`, `ToClosure`, `ToString`) e escritores tipados. |
| `Format/Format.h` | Formatação segura limitada e baseada em `Writer` com adaptadores `%!`. |
| `Format/BytesHuman.h` | Formatação e parsing de tamanhos de bytes legíveis por humanos. |
| `Grid/Attribute.h`, `Grid/Char.h`, `Grid/Grid.h` | Planos de atributos, utilitários de cor e grids de texto apoiados em memória. |

---

# Visão geral

Text separa primitivas de baixo nível (Unicode e C strings) de views sem ownership, strings mutáveis com ownership, formatação desacoplada (`Writer` + `Format` + `BytesHuman`) e grids off-screen de caracteres e atributos. Nenhuma dessas áreas realiza E/S de terminal nem cria acoplamento de `Text` com módulos externos como `Tuple` ou `Numeric` na camada de formatação.

---

# Referência da API

## Unicode e codificações

### Codepoint

Header: `Unicode/Codepoint.h`. Package: `Text_Codepoint`.

O package expõe predicados sob a família `Is` (`VARIANT`):

| Função | Significado |
| --- | --- |
| `IsValid(codepoint)` | Verifica se o valor está no intervalo U+0000..U+10FFFF. |
| `IsScalar(codepoint)` | Verifica se o valor é um escalar Unicode (não um surrogate). |
| `IsAscii(codepoint)` | Verifica se o valor está no intervalo U+0000..U+007F. |
| `IsControl(codepoint)` | Verifica os intervalos de controle C0 e C1. |
| `IsWhitespace(codepoint)` | Verifica os intervalos de whitespace Unicode implementados pelo módulo. |

~~~c
bool scalar = MODULE_FUNC(Text, Codepoint, IsScalar)(codepoint);
~~~

Use `IsScalar` antes de codificar. `IsValid` sozinho permite pontos de código surrogate, que não são valores escalares Unicode.

### UTF8

Header: `Unicode/UTF8.h`. Buffers de entrada e saída usam bytes `unsigned char`.

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

### UTF16

Header: `Unicode/UTF16.h`. Entrada e saída usam unidades de código `MODULE_TYPE(Text, Char16)`.

| Função | Contrato |
| --- | --- |
| `EncodedLength(codepoint, outUnits)` | Informa uma ou duas unidades de código UTF-16. |
| `Encode(codepoint, output, capacity, outUnits)` | Codifica um escalar em um buffer UTF-16 fornecido pelo chamador. |
| `Decode(input, size, outCodepoint, outUnits)` | Decodifica um ponto de código e informa as unidades consumidas. |
| `Validate(input, size)` | Valida um intervalo completo de unidades de código UTF-16. |
| `Count(input, size, outCodepoints, outUnits)` | Conta pontos de código e informa as unidades consumidas. |
| `FromUTF8(input, size, output, capacity, outUnits)` | Transcodifica bytes UTF-8 para unidades UTF-16 (passe `NULL, 0U` para medir). |
| `ToUTF8(input, size, output, capacity, outBytes)` | Transcodifica unidades UTF-16 para bytes UTF-8 (passe `NULL, 0U` para medir). |

### Largura de exibição

Header: `Unicode/Width.h`. Package: `Text_Width`.

| Função | Contrato |
| --- | --- |
| `Codepoint(codepoint, outWidth)` | Informa a largura de exibição (`0`, `1` ou `2`) de um ponto de código. |
| `UTF8(input, size, outColumns, outUnits)` | Mede as colunas de exibição de um intervalo UTF-8 e informa os bytes consumidos. |

---

## C strings, Views e Strings

### CString

Header: `CString/CString.h`. Package: `Text_CString`.

| Grupo | Métodos |
| --- | --- |
| Comprimento | `Length`, `LengthBounded` |
| Classificação | `IsEmpty`, `IsAscii`, `IsAsciiBounded` |
| Cópia e concatenação | `Copy`, `CopyBounded`, `Concat`, `ConcatBounded` |
| Comparação | `Compare`, `CompareBounded`, `CompareIgnoreCase`, `CompareIgnoreCaseBounded` |
| Igualdade | `Equal`, `EqualBounded`, `EqualIgnoreCase`, `EqualIgnoreCaseBounded` |
| Prefixo/sufixo | `StartsWith`, `StartsWithBounded`, `StartsWithIgnoreCase`, `EndsWith`, `EndsWithBounded`, `EndsWithIgnoreCase` |
| Busca | `FindChar`, `FindCharBounded`, `FindLastChar`, `FindLastCharBounded`, `FindCStr`, `FindCStrBounded` |
| Span | `Span`, `ComplementSpan` |
| Trim | `Trim`, `TrimLeft`, `TrimRight` |
| Contagem/substituição | `CountChar`, `CountCharBounded`, `ReplaceChar`, `ReplaceCharBounded` |
| Alocação | `Duplicate`, `DuplicateBounded`, `Free` |

Prefira operações `*Bounded` sempre que a extensão disponível for conhecida e a entrada puder não conter terminador dentro desse intervalo.

### View

Header: `View/View.h`.

Os tipos sem ownership gerados são `MODULE_TYPE(Text, View8)`, `View16` e `View32`. Comprimentos e índices são em unidades de código, salvo ao usar os helpers conscientes de pontos de código UTF-8 (`View8_*Codepoints`, `View8_Validate`, `View8_Trim*`).

| Função | Descrição |
| --- | --- |
| `Create(data, length)`, `FromCStr(cstr)`, `Empty()` | Criam views sem alocação. |
| `Length`, `IsEmpty`, `Data`, `At(index, outValue)` | Inspecionam o conteúdo da view. |
| `Slice(start, count)`, `Prefix(count)`, `Suffix(count)` | Retornam subviews. |
| `Compare(left, right)`, `Equal`, `EqualCStr` | Comparam views. |
| `Find`, `FindView`, `FindCStr`, `StartsWith*`, `EndsWith*` | Buscam e testam prefixos/sufixos. |
| `View8_Validate`, `View8_CountCodepoints`, `View8_NextCodepoint`, `View8_Trim*`, `View8_SliceCodepoints` | Operações UTF-8 por pontos de código em `View8`. |

### Strings com ownership

Header: `String/String.h`. Gera `String8` (`char`), `String16` (`Text_Char16`) e `String32` (`Text_Char32`).

| Grupo | Métodos |
| --- | --- |
| Ciclo de vida | `Create(initialCapacity)`, `Destroy()`, `Clone(src)` |
| Capacidade | `Reserve(requiredCapacity)`, `ShrinkToFit()` |
| Construção | `FromBuffer(buffer, count)`, `FromCStr(cstr)`, `FromView(view)` |
| Mutação | `Clear()`, `AppendBuffer(buffer, count)`, `AppendCStr(cstr)`, `AppendString(other)`, `AppendView(view)`, `PushBack(value)`, `PopBack(outChar)`, `SetAt(index, value)`, `Insert(index, buffer, count)`, `InsertChar(index, value)`, `Erase(index, count)` |
| Acesso | `Length()`, `Capacity()`, `IsEmpty()`, `CStr()`, `At(index, outChar)` |
| Views e recortes | `View()`, `Slice(start, count)`, `Substr(start, count, outSubstr)` |
| Comparação | `Compare(other)`, `Equal(other)`, `EqualCStr(cstr)`, `EqualView(view)`, `StartsWith(prefix)`, `EndsWith(suffix)` |
| Busca | `Find(needle, outIndex)`, `FindCStr(needle, outIndex)` |
| UTF-8 (`String8_*`) | `AppendCodepoint`, `CountCodepoints`, `Validate`, `Trim`, `TrimLeft`, `TrimRight`, `Format`, `FormatVariadic`, `AppendFormat`, `AppendFormatVariadic`, `SliceCodepoints` |

`String8` também expõe `MODULE_FUNC(Text, Writer, ToString)(&str)` para que `Writer` e `Format` escrevam diretamente em uma `String8` com crescimento dinâmico em uma única passagem.

~~~c
MODULE_TYPE(Text, String8) text = {0};
if (MODULE_FUNC(Text, String8, Create)(&text, 16U) == STATUS_CONST(SUCCESS)) {
    (void)MODULE_FUNC(Text, String8, Format)(&text, "Hello %s!", "Cosmeron");
    (void)MODULE_FUNC(Text, String8, Destroy)(&text);
}
~~~

---

## Formatação

### Writer

Header: `Format/Writer.h`. Tipo: `MODULE_TYPE(Text, Writer)`. Package: `Text_Writer`.

`Writer` desacopla a geração de texto do destino de saída:

| Construtor | Destino |
| --- | --- |
| `ToBuffer(buffer, capacity)` | Buffer `char` do chamador (sempre terminado em NUL; registra truncamento). |
| `Counter()` | Modo somente medição (conta bytes sem gravar). |
| `ToClosure(sink)` | Qualquer alvo `MODULE_CLOSURE(Text, WriterSink)`. |
| `ToString(str)` | Declarado em `String/String.h`; acrescenta diretamente em uma `String8`. |

Primitivas tipadas: `WriteBuffer`, `WriteChar`, `WriteCodepoint`, `WriteRepeat`, `WriteCStr`, `WriteView`, `WriteBool`, `WriteInt`, `WriteUInt`, `WriteFloat`, `WriteHex`, `WriteEscaped`, além de inspeção (`Written`, `IsTruncated`, `Status`) e `Finish`.

### Format

Header: `Format/Format.h`. Package: `Text_Format`.

| Função | Contrato |
| --- | --- |
| `To(writer, format, ...)` / `ToVariadic(writer, format, args)` | Grava saída formatada em qualquer `Text_Writer`. |
| `Buffer(buffer, capacity, outWritten, format, ...)` / `BufferVariadic(...)` | Formatador limitado seguro para buffer (`NULL, 0U` mede o total necessário). |

Suporta especificadores padrão C11/C23 (`%d`/`%i`, `%u`, `%o`, `%x`/`%X`, `%b`/`%B`, `%f`/`%F`/`%e`/`%E`/`%g`/`%G`/`%a`/`%A`, `%c`/`%lc`, `%s`/`%ls`, `%p`, `%%`, largura/precisão/`*`, `hh`/`h`/`l`/`ll`/`j`/`z`/`t`/`L`/`w8`/`w16`/`w32`/`w64`) e extensões nativas do Cosmeron:
- `%v`: `MODULE_TYPE(Text, View8)` por valor.
- `%U`: `MODULE_TYPE(Text, Char32)` codificado em UTF-8.
- `%t`: `bool` (`true` / `false`).
- `%q` / `%#q`: C-string escapada (com ou sem aspas).
- `%!` / `%(spec)!`: par adaptador customizado `(MODULE_CALLBACK(Text, Format, Adapter) adapter, const void *value)` para formatar tipos externos sem acoplar `Text` a outros módulos.

### Tamanhos de bytes legíveis por humanos

Header: `Format/BytesHuman.h`. Package: `Text_BytesHuman`. Enum: `MODULE_ENUM(Text, BytesHuman, Unit)` (`IEC`, `SI`).

| Função | Descrição |
| --- | --- |
| `Write(writer, bytes)` / `WriteEx(writer, bytes, unitStandard, precision)` | Grava tamanho formatado em um `Text_Writer`. |
| `Format(buffer, capacity, outWritten, bytes)` / `FormatEx(...)` | Formatação limitada para buffer do chamador. |
| `Parse(text, outBytes)` | Converte tamanho textual de bytes em `uint64_t` com proteção contra overflow. |

---

## Grids de caracteres e atributos

### Escopo e layout

As operações de Grid trabalham com células apoiadas em memória off-screen (`MODULE_TYPED(Tuple, Dual, UInt16)` para dimensões/posições e `MODULE_TYPED(Tuple, Quad, UInt16)` para regiões). Não escrevem em stdout nem implementam backend de terminal.

- `Grid/Attribute.h`: expõe `MODULE_TYPE(Text, Attribute, Cell)`, `MODULE_TYPE(Text, Attribute, Grid)`, `MODULE_ENUM(Text, Attribute, Flag)`, `MODULE_ENUM(Text, Attribute, Underline)`, operações de grade `Attribute_*` e `Attribute_Color` (`SPACE(Color, ...)` com `RGB`, `Red`, `Green`, `Blue` e `WindowsPalette`).
- `Grid/Char.h`: gera `MODULE_TYPED(Text, Char, Char8)`, `Char16` e `Char32`.
- `Grid/Grid.h`: gera `MODULE_TYPED(Text, Grid, Char8)`, `Char16` e `Char32`, combinando planos de caracteres e atributos com `Blit` / `BlitRegion`.

---

# Contratos e regras de ownership

- Encoders, formatadores, operações de grid e operações de string que escrevem em buffers retornam `OPSTATUS`; verifique antes de ler parâmetros de saída.
- Comprimentos UTF-8 são bytes; comprimentos UTF-16 são unidades de código; contagens de pontos de código são contagens de escalares Unicode.
- `View` não possui ownership; `String` e `Grid` são proprietários de suas alocações e devem ser destruídos.
- Ponteiros retornados por `Data`, `CStr` e `WindowsPalette` são emprestados.

## Módulos relacionados

- [Tuple](Tuple.md) — dimensões e regiões do grid.
- [Memory](Memory.md) — alocação e operações com bytes.
- [Algorithm](Algorithm.md) — resultados de comparação usados por views e C strings.
- [Module](Module.md) — packages, objetos, closures e especializações tipadas.

## Headers de origem

- `Codespace/Cosmeron/Modules/Text/Text.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Codepoint.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/UTF8.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/UTF16.h`
- `Codespace/Cosmeron/Modules/Text/Unicode/Width.h`
- `Codespace/Cosmeron/Modules/Text/CString/CString.h`
- `Codespace/Cosmeron/Modules/Text/View/View.def`
- `Codespace/Cosmeron/Modules/Text/String/String.def`
- `Codespace/Cosmeron/Modules/Text/Format/Writer.h`
- `Codespace/Cosmeron/Modules/Text/Format/Format.h`
- `Codespace/Cosmeron/Modules/Text/Format/BytesHuman.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Attribute.h`
- `Codespace/Cosmeron/Modules/Text/Grid/Char.def`
- `Codespace/Cosmeron/Modules/Text/Grid/Grid.def`
