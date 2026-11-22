# Algoritmos (Algorithm)

O módulo **Algorithm** fornece primitivas reutilizáveis de comparação e repetição. Sua API pública é dividida nos pacotes `Comparison` e `Repeat`; templates de repetição tipada estão disponíveis para tipos de elementos definidos pelo usuário.

## Cabeçalhos

~~~c
#include "Cosmeron/Core/Algorithm/Comparison.h"
#include "Cosmeron/Core/Algorithm/Repeat.h"
~~~

---

# Visão geral

## Comparison

Uma comparação retorna `CMPOUT`, um resultado de três valores:

| Constante | Valor | Significado |
| --- | ---: | --- |
| `COMPARISON_CONST(LOWER)` | -1 | O operando à esquerda precede o operando à direita. |
| `COMPARISON_CONST(EQUAL)` | 0 | Os operandos são iguais na comparação. |
| `COMPARISON_CONST(HIGHER)` | 1 | O operando à esquerda sucede o operando à direita. |

Utilize esse resultado em vez de depender de qualquer inteiro negativo ou positivo. `COMPARISON_CONST` fornece as constantes com namespace e `MODULE_ENUM(Algorithm, Comparison, Result)` nomeia o tipo enum.

O pacote também expõe `ToString`, comparações de tipos primitivos ordenados, `CString` e `Bytes`. Quando o compilador oferece suporte, a implementação adiciona comparações de números complexos.

## Repeat

O pacote `Repeat` executa um delegate uma quantidade fixa de vezes, opcionalmente passando um índice iniciado em zero. Ele oferece suporte a procedimentos, operações sujeitas a falhas e predicados, além de padrões de bytes repetidos, valores primitivos e ponteiros para strings C.

O template tipado adiciona geração de valores para um tipo C escolhido. É útil quando o valor repetido precisa ser produzido por um delegate, em vez de copiado de uma constante.

---

# Referência da API

## Pacote Comparison

Cabeçalho: `Cosmeron/Core/Algorithm/Comparison.h`.

| Função | Contrato | Resultado |
| --- | --- | --- |
| `ToString` | Converte um valor `CMPOUT` em sua descrição simbólica. | `const char *` |
| Funções de tipos primitivos | Comparam os valores primitivos correspondentes. Os nomes das funções vêm de `PRIMITIVE_TYPE_TABLE`. | `CMPOUT` |
| `CString` | Compara duas strings terminadas em nulo. | `CMPOUT` |
| `Bytes` | Compara lexicograficamente dois intervalos de bytes, usando seus comprimentos para desempatar um prefixo comum. | `CMPOUT` |

A tabela de tipos primitivos inclui tipos booleanos, inteiros com e sem sinal, caracteres, ponto flutuante e tipos de tamanho dependente da arquitetura. A implementação também fornece comparações de ponto flutuante complexo em compiladores diferentes do MSVC. Os sufixos das funções de tipos primitivos seguem os nomes exatos de `PRIMITIVE_TYPE_TABLE` (por exemplo, `Int32`), e não abreviações.

### Comparação primitiva

Utilize `MODULE_FUNC(Algorithm, Comparison, <PrimitiveName>)` com o tipo C correspondente. Por exemplo:

~~~c
CMPOUT order = MODULE_FUNC(Algorithm, Comparison, Int32)(left, right);
if (order == COMPARISON_CONST(LOWER)) {
    /* left is smaller */
}
~~~

### CString

~~~c
CMPOUT order = MODULE_FUNC(Algorithm, Comparison, CString)(left, right);
~~~

Um ponteiro nulo é ordenado antes de um ponteiro não nulo. Dois ponteiros idênticos são comparados como iguais. Strings não nulas são comparadas usando a ordenação de strings C fornecida pela implementação.

### Bytes

~~~c
CMPOUT order = MODULE_FUNC(Algorithm, Comparison, Bytes)(
    leftData, leftSize, rightData, rightSize);
~~~

O prefixo comum é comparado primeiro; se os bytes forem iguais, o intervalo mais curto é ordenado primeiro. Um intervalo nulo é ordenado antes de um intervalo não nulo. Não passe intervalos de memória inválidos.

## Pacote Repeat

Cabeçalho: `Cosmeron/Core/Algorithm/Repeat.h`.

| Função | Descrição |
| --- | --- |
| `Procedure` | Chama um procedimento sem argumentos `count` vezes. |
| `Procedure.Indexed` | Chama um procedimento com índices de zero a `count - 1`. |
| `Operation` | Chama uma operação sujeita a falhas até que ela falhe ou todas as repetições terminem. |
| `Operation.Indexed` | Igual a `Operation`, passando o índice atual iniciado em zero. |
| `Predicate` | Retorna se todas as chamadas repetidas do predicado têm sucesso. |
| `Predicate.Indexed` | Igual a `Predicate`, passando o índice atual. |
| `Bytes` | Copia um padrão de bytes para posições consecutivas em um buffer de destino. |
| Funções de tipos primitivos | Preenche um array com um valor primitivo; os nomes seguem `PRIMITIVE_TYPE_TABLE`. |
| `CString` | Preenche posições de um array `const char *` com o mesmo ponteiro. |

### Assinaturas dos delegates

Os tipos invocáveis são delegates gerados pelo pacote:

| Delegate | Assinatura | Uso |
| --- | --- | --- |
| `Procedure` | `void (void)` | Efeito colateral sem resultado. |
| `ProcedureIndexed` | `void (size_t index)` | Efeito colateral dependente da iteração. |
| `Operation` | `OPSTATUS (void)` | Trabalho sujeito a falhas. |
| `OperationIndexed` | `OPSTATUS (size_t index)` | Trabalho indexado sujeito a falhas. |
| `Predicate` | `bool (void)` | Condição que deve permanecer verdadeira. |
| `PredicateIndexed` | `bool (size_t index)` | Condição indexada. |

Um delegate armazena uma função e um ponteiro de contexto opcional. `DELEGATE_IS_VALID` testa se sua função está presente e `DELEGATE_CALL` a invoca. Utilize os auxiliares de vinculação ou promoção gerados para o tipo de delegate declarado no pacote.

### Repetição sujeita a falhas

~~~c
OPSTATUS status = MODULE_FUNC(Algorithm, Repeat, Operation)(count, operation);
~~~

Para uma contagem diferente de zero, um delegate de operação inválido retorna `INVALID_ARGUMENT`. O primeiro status diferente de `SUCCESS` é retornado imediatamente; as iterações seguintes não são executadas. Uma contagem zero tem sucesso sem invocar o delegate.

### Repetindo bytes

~~~c
unsigned char pattern[2] = {0xAA, 0x55};
unsigned char output[6];

OPSTATUS status = MODULE_FUNC(Algorithm, Repeat, Bytes)(
    output, pattern, sizeof(pattern), 3U);
~~~

O buffer resultante contém três cópias do padrão de dois bytes. Uma contagem zero ou tamanho de padrão zero tem sucesso sem escrita. Para trabalho diferente de zero, ambos os ponteiros devem ser válidos, os intervalos de origem e destino não podem se sobrepor e o tamanho total do destino deve ser representável.

### Preenchendo arrays primitivos

~~~c
int32_t values[4];

OPSTATUS status = MODULE_FUNC(Algorithm, Repeat, Int32)(
    values, 42, 4U);
~~~

A operação escreve o valor fornecido em cada um dos `count` elementos. Uma contagem zero tem sucesso sem exigir um ponteiro de destino. Escritas não nulas exigem um destino válido e rejeitam overflow na multiplicação da quantidade de elementos.

### Preenchendo arrays de ponteiros para strings C

~~~c
const char *labels[3];

OPSTATUS status = MODULE_FUNC(Algorithm, Repeat, CString)(
    labels, "ready", 3U);
~~~

Somente os ponteiros são copiados; a string apontada não é duplicada e continua pertencendo ao seu armazenamento original.

## Template Repeat tipado

`Repeat.def` define `TEMPLATE_Algorithm_Repeat(SUFFIX, TYPE)`. Ele gera uma especialização de pacote com:

| Função | Descrição |
| --- | --- |
| `Fill` | Preenche um array tipado com um valor. |
| `Generate` | Preenche um array invocando um gerador de valor sem argumentos. |
| `Generate.Indexed` | Preenche um array invocando um gerador com o índice do elemento. |

As assinaturas dos delegates gerados são `TYPE (void)` e `TYPE (size_t index)`. Cada gerador deve ser válido quando a contagem for diferente de zero. As operações retornam `OPSTATUS` e verificam a validade do destino e a aritmética dos tamanhos antes de escrever.

Instancie o template pelo mecanismo comum:

~~~c
TEMPLATE_INSTANTIATE(Algorithm_Repeat, Point, Point);
~~~

Os nomes do tipo público e das funções da especialização gerada utilizam o sufixo fornecido. Defina o tipo do template e as implementações de delegates antes de utilizar o pacote gerado.

---

# Contratos e comportamento

- Funções `OPERATION` retornam `OPSTATUS`; verifique `STATUS_CONST(SUCCESS)` antes de presumir que os buffers de saída foram escritos.
- Funções `PROCEDURE` não possuem resultado de status. Um delegate de procedimento inválido é tratado como uma operação sem efeito.
- Funções `PREDICATE` retornam `bool`; uma contagem de repetição zero é verdadeira por vacuidade.
- Delegates indexados recebem índices iniciados em zero.
- A repetição é síncrona. O pacote não cria threads nem agenda trabalho em segundo plano.
- `CString` repete valores de ponteiro, não o conteúdo das strings.
- `Bytes` rejeita intervalos de origem e destino sobrepostos; não é uma operação genérica de movimentação com sobreposição.

## Despacho

As declarações atuais de `Comparison.h` e `Repeat.h` desabilitam function tables. Chame as funções dos pacotes diretamente por meio de `MODULE_FUNC`; não utilize `PACKAGE_API` para esses pacotes. Essa é uma propriedade de compilação desses cabeçalhos, não uma opção em tempo de execução.

~~~c
CMPOUT order = MODULE_FUNC(Algorithm, Comparison, Int32)(left, right);
OPSTATUS status = MODULE_FUNC(Algorithm, Repeat, Operation)(count, operation);
~~~

`MODULE_FUNC` mantém a chamada vinculada ao símbolo público gerado e evita exigir uma function table de pacote.

## Módulos relacionados

- [Bit](Bit.md) — primitivas de bits e bitsets.
- [Memory](Memory.md) — operações de bytes, troca e alocação.
- [Module](Module.md) — declarações de pacotes, callables, delegates e invocação.
- [Preprocessor](Preprocessor.md) — mecanismos de templates e macros.

## Cabeçalhos de origem

- `Codespace/Cosmeron/Core/Algorithm/Comparison.h`
- `Codespace/Cosmeron/Core/Algorithm/Repeat.h`
- `Codespace/Cosmeron/Core/Algorithm/Repeat.def`
- `Codespace/Cosmeron/Core/Algorithm/Impl/Comparison.impl`
- `Codespace/Cosmeron/Core/Algorithm/Impl/Repeat.impl`
