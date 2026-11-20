# Conversão de tipos (Cast)

O módulo **Cast** fornece conversões geradas e verificadas entre tipos primitivos C compatíveis. Ele pertence ao **Core** e complementa `Types`: Types define aliases e metadados de tipos primitivos, enquanto Cast define conversões com verificações de faixa quando necessário.

## Cabeçalhos

```c
#include "Cosmeron/Core/Types/Cast/Cast.h"
```

O cabeçalho agregado inclui `Converting.h`, `Integer.h` e `Float.h`.

---

# Visão geral

| Cabeçalho | Finalidade |
| --- | --- |
| `Converting.h` | Macros de especificação de conversões e mecanismo de geração de código |
| `Integer.h` | Conversões verificadas entre inteiros |
| `Float.h` | Conversões entre ponto flutuante e inteiros |

As funções geradas retornam `OPSTATUS` e escrevem o valor convertido por meio de um ponteiro de saída. Isso torna explícitas as conversões que podem reduzir a faixa ou alterar o sinal.

---

# Referência da API

## Conversões entre inteiros

O pacote Integer gera operações de conversão para os tipos de origem `Uint64`, `Uint32`, `Uint16`, `Uint8`, `Int64`, `Int32`, `Int16`, `Int8` e `Size`. Ele gera somente os pares de destino declarados em `Integer.h`; um par arbitrário não fica disponível automaticamente.

O nome da operação gerada concatena o tipo de origem, o literal `to` e o sufixo de destino. A tabela a seguir lista os sufixos de destino exatos declarados em `Integer.h`; ela não representa o produto cartesiano completo de tipos de origem e destino.

| Origem | Sufixos de destino gerados |
| --- | --- |
| `Uint64` | `32`, `16`, `8`, `I64`, `I32`, `I16`, `I8`, `Size` |
| `Uint32` | `16`, `8`, `I64`, `I32`, `I16`, `I8` |
| `Uint16` | `8`, `I64`, `I32`, `I16`, `I8` |
| `Uint8` | `I64`, `I32`, `I16`, `I8` |
| `Int64` | `32`, `16`, `8`, `U64`, `U32`, `U16`, `U8`, `Size` |
| `Int32` | `16`, `8`, `U64`, `U32`, `U16`, `U8`, `Size` |
| `Int16` | `8`, `U64`, `U32`, `U16`, `U8` |
| `Int8` | `U64`, `U32`, `U16`, `U8` |
| `Size` | `U64`, `U32`, `U16`, `U8`, `I64`, `I32` |

Por exemplo:

```c
uint32_t out = 0;
OPSTATUS status =
    MODULE_FUNC(Cast, Integer, Uint64to32)(value, &out);
```

A operação valida o ponteiro de saída e retorna `ARITHMETIC_OVERFLOW` quando o valor está fora da faixa do tipo de destino. Uma conversão malsucedida não escreve o resultado.

---

## Conversões de ponto flutuante

O pacote Float declara conversões entre `Float`, `Double`, `LongDouble`, tipos inteiros com e sem sinal selecionados e `Size`. Sua tabela de conversões é explícita; ela não representa o produto cartesiano completo de todos os tipos primitivos.

Os pares exatos de sufixos de origem e destino são:

| Origem | Sufixos de destino gerados |
| --- | --- |
| `Float` | `Double`, `LongDouble`, `I64`, `I32`, `I16`, `I8`, `U64`, `U32`, `U16`, `U8`, `Size` |
| `Double` | `Float`, `LongDouble`, `I64`, `I32`, `I16`, `I8`, `U64`, `U32`, `U16`, `U8`, `Size` |
| `LongDouble` | `Float`, `Double`, `I64`, `I32`, `I16`, `I8`, `U64`, `U32`, `U16`, `U8`, `Size` |
| `Int64`, `Int32`, `Uint64`, `Uint32`, `Size` | `Float`, `Double`, `LongDouble` |

```c
double out = 0.0;
OPSTATUS status =
    MODULE_FUNC(Cast, Float, FloattoDouble)(input, &out);
```

Operações de ponto flutuante para inteiro verificam os limites configurados do destino antes da conversão e rejeitam NaN e valores fora da faixa. A redução de precisão entre tipos de ponto flutuante verifica o limite máximo fornecido pela entrada da tabela. Conversões de inteiro para ponto flutuante são conversões diretas e podem perder precisão quando o formato de destino não consegue representar exatamente o valor de origem. Conversões de ponto flutuante para inteiro usam o cast de C após a validação; portanto, valores fracionários são truncados em direção a zero, e não arredondados para o inteiro mais próximo.

Verifique `OPSTATUS` antes de usar a saída. Overflow é informado como `ARITHMETIC_OVERFLOW`; um ponteiro de saída nulo retorna `INVALID_ARGUMENT`.

---

## Gerador de conversões

`Converting.h` expõe o vocabulário de especificação usado pelos cabeçalhos dos pacotes:

| Macro | Significado |
| --- | --- |
| `TO(name, type, category)` | Declara uma conversão sem uma tupla explícita de limites |
| `TO_EX(name, type, category, limits...)` | Declara uma conversão com informações explícitas de faixa/limites |
| `CONVERTING_OPERATIONS(source, ...)` | Gera operações comuns de pacote |
| `CONST_CONVERTING_OPERATIONS(source, ...)` | Gera operações const |
| `CONVERTING_INLINE(source, ...)` | Gera operações de conversão inline |
| `CONST_CONVERTING_INLINE(source, ...)` | Gera operações const inline |

Essas macros são principalmente ferramentas de geração de código para definir tabelas de conversão. A maioria dos códigos de aplicação deve chamar as funções de conversão geradas, em vez de usar as macros diretamente.

---

# Contratos e limitações

- Prefira Cast a um cast C sem verificação quando o par necessário estiver disponível e for preciso validar a faixa em tempo de execução.
- Segurança de faixa não significa exatidão: um inteiro representável ainda pode perder precisão ao ser convertido para ponto flutuante.
- Somente as conversões listadas em `Integer.h` e `Float.h` são geradas.
- Cast não fornece conversões para structs definidos pelo usuário, ponteiros, strings ou tipos numéricos do módulo separado `Numeric`.
- Conversões de unidades, política de arredondamento e validação semântica continuam sendo responsabilidade da aplicação.

## Módulos relacionados

- [Types](Types.md) — aliases e metadados de tipos primitivos.
- [Numeric](Numeric.md) — representações racionais, ponto fixo, complexas e numéricas de múltiplos limbs.
- [Module](Module.md) — funções de pacotes geradas e acesso a namespaces.

## Cabeçalhos de origem

- `Codespace/Cosmeron/Core/Types/Cast/Cast.h`
- `Codespace/Cosmeron/Core/Types/Cast/Converting.h`
- `Codespace/Cosmeron/Core/Types/Cast/Integer.h`
- `Codespace/Cosmeron/Core/Types/Cast/Float.h`
