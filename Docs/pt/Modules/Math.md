# Matemática (Math)

O módulo **Math** fornece aritmética inteira verificada, aritmética limitada e saturada, auxiliares de intervalo, extremos e solucionadores de equações lineares/quadráticas de valores reais. Suas declarações são templates C11 instanciados para os sufixos de tipo listados nas tabelas de tipos do módulo.

## Cabeçalhos

~~~c
#include "Cosmeron/Modules/Math/Math.h"
~~~

O cabeçalho agregado inclui todos os pacotes atuais:

| Pacote | Cabeçalho público | Finalidade |
| --- | --- | --- |
| Arithmetic | `Arithmetic/Basic.h` | Operações inteiras verificadas com e sem sinal. |
| ClampedArithmetic | `Arithmetic/Clamped.h` | Aritmética inteira limitada e saturada. |
| Between | `Value/Between.h` | Classificação e pertencimento a intervalos. |
| Clamp | `Value/Clamp.h` | Limita um valor a um intervalo. |
| MaxAndMin | `Value/MaxAndMin.h` | Mínimo e máximo binários. |
| LinearEquation | `Equation/Linear.h` | Resolve uma equação linear. |
| QuadraticEquation | `Equation/Quadratic.h` | Discriminante, auxiliar de raiz quadrada e resolução quadrática. |

---

# Visão geral

## Especializações de tipos

Os sufixos públicos fazem parte dos nomes gerados. Utilize o sufixo exato em vez de supor um alias C.

| Sufixo | Tipo C | Pacotes disponíveis |
| --- | --- | --- |
| `Bool` | `bool` | Value |
| `Int8`, `Int16`, `Int32`, `Int64` | Inteiros com sinal de largura fixa | Arithmetic, ClampedArithmetic, Value |
| `UInt8`, `UInt16`, `UInt32`, `UInt64` | Inteiros sem sinal de largura fixa | Arithmetic, ClampedArithmetic, Value |
| `Char` | `char` | Value |
| `Float32` | `float` | Value, Equation |
| `Float64` | `double` | Value, Equation |
| `Float128` | `long double` | Value, Equation |
| `Int`, `UInt`, `Size`, `PtrDiff`, `IntPtr`, `UIntPtr` | Tipos C de tamanho dependente da arquitetura | Value |

`Float128` é o sufixo para `long double` em C. Ele não garante que todas as plataformas utilizem uma representação real de 128 bits.

---

# Referência da API

## Arithmetic

Cabeçalho: `Arithmetic/Basic.h`.

Os pacotes com e sem sinal são nomeados como `MODULE_FUNC(Math, Arithmetic, <Suffix>, <Operation>)`.

| Função | Assinatura | Comportamento |
| --- | --- | --- |
| `Add` | `(TYPE a, TYPE b, TYPE *outResult)` | Adição verificada. |
| `Sub` | `(TYPE a, TYPE b, TYPE *outResult)` | Subtração verificada. |
| `Mul` | `(TYPE a, TYPE b, TYPE *outResult)` | Multiplicação verificada. |
| `Div` | `(TYPE a, TYPE b, TYPE *outResult)` | Divisão verificada. |
| `Mod` | `(TYPE a, TYPE b, TYPE *outResult)` | Resto verificado. |

~~~c
int32_t sum = 0;
OPSTATUS status = MODULE_FUNC(Math, Arithmetic, Int32, Add)(
    20, 22, &sum);

if (status == STATUS_CONST(SUCCESS)) {
    /* sum is 42 */
}
~~~

Toda operação exige um ponteiro de saída não nulo. Overflow e operações aritméticas inválidas são informados por `OPSTATUS`; a saída só é escrita quando a operação tem sucesso. Divisão ou resto por zero são inválidos. O menor inteiro com sinal dividido por -1 é informado como fora da faixa.

## ClampedArithmetic

Cabeçalho: `Arithmetic/Clamped.h`.

Cada especialização com ou sem sinal oferece dois grupos:

| Variante | Métodos | Parâmetros | Resultado |
| --- | --- | --- | --- |
| `Clamp` | `Add`, `Sub`, `Mul` | `a, b, minimum, maximum` | Retorna o resultado aritmético restrito ao intervalo fornecido. |
| `Saturating` | `Add`, `Sub`, `Mul` | `a, b` | Retorna um valor saturado aos limites representáveis do tipo. |

~~~c
int32_t bounded = MODULE_FUNC(Math, ClampedArithmetic, Int32, Clamp, Add)(
    INT32_MAX, 10, -100, 100);

int32_t saturated = MODULE_FUNC(Math, ClampedArithmetic, Int32, Saturating, Add)(
    INT32_MAX, 10);
~~~

A forma Clamp delega à aritmética verificada. Se a operação intermediária sofrer overflow, escolhe o limite correspondente; caso contrário, limita o valor calculado. A forma Saturating utiliza os valores mínimo/máximo nativos do tipo inteiro selecionado. Essas funções retornam um valor diretamente, e não um `OPSTATUS`.

## Between

Cabeçalho: `Value/Between.h`.

| Função | Descrição |
| --- | --- |
| `Classify(value, minimum, maximum)` | Retorna um `MODULE_ENUM(Math, RangeResult)` indicando abaixo, dentro, acima ou intervalo inválido. |
| `Is(value, minimum, maximum)` | Retorna true somente quando o valor está dentro do intervalo inclusivo. |

Os valores de `RangeResult` são `IN_RANGE`, `BELOW_RANGE`, `ABOVE_RANGE` e `INVALID_RANGE`. O intervalo inclui ambas as extremidades. Se `minimum > maximum`, `Classify` retorna `INVALID_RANGE` e `Is` retorna false.

~~~c
MODULE_ENUM(Math, RangeResult) position =
    MODULE_FUNC(Math, Between, Int32, Classify)(value, 10, 20);

bool accepted = MODULE_FUNC(Math, Between, Int32, Is)(value, 10, 20);
~~~

## Clamp

Cabeçalho: `Value/Clamp.h`.

`Apply(value, minimum, maximum)` retorna o valor restrito ao intervalo inclusivo. Se as extremidades estiverem invertidas, a implementação as troca antes de limitar.

~~~c
int32_t bounded = MODULE_FUNC(Math, Clamp, Int32, Apply)(42, 0, 10);
/* bounded is 10 */
~~~

Este é um auxiliar que retorna um valor e não informa intervalo inválido; limites invertidos são normalizados.

## MaxAndMin

Cabeçalho: `Value/MaxAndMin.h`.

O pacote fornece apenas funções binárias `Max(a, b)` e `Min(a, b)` para cada especialização de tipo ordenado. Elas retornam diretamente o valor selecionado.

~~~c
int32_t maximum = MODULE_FUNC(Math, MaxAndMin, Int32, Max)(left, right);
int32_t minimum = MODULE_FUNC(Math, MaxAndMin, Int32, Min)(left, right);
~~~

A implementação atual não possui APIs variádicas `Smallest` ou `Biggest`.

## LinearEquation

Cabeçalho: `Equation/Linear.h`.

Resolve a equação:

[
a x + b = 0
]

`Solve(a, b, outResult, outSolution)` retorna `OPSTATUS` e escreve a classificação da solução em `MODULE_ENUM(Math, Solution)`.

| Condição | Classificação da solução | Resultado |
| --- | --- | --- |
| `a != 0` | `ONE` | `x = -b / a` |
| `a == 0` e `b == 0` | `INFINITE` | Não há raiz única; o valor de saída é definido como zero. |
| `a == 0` e `b != 0` | `NONE` | Não há raiz; o valor de saída é definido como zero. |

~~~c
double root = 0.0;
MODULE_ENUM(Math, Solution) count;

OPSTATUS status = MODULE_FUNC(Math, LinearEquation, Float64, Solve)(
    2.0, -8.0, &root, &count);
~~~

Ambos os ponteiros de saída são obrigatórios. Uma chamada bem-sucedida indica que a equação foi classificada, mas não significa que exista uma raiz única; inspecione `count`.

## QuadraticEquation

Cabeçalho: `Equation/Quadratic.h`.

O pacote expõe `Sqrt(x)`, `Discriminant(a, b, c)` e `Solve(a, b, c, outResult1, outResult2, outSolution)`.

Para:

[
a x^2 + b x + c = 0
]

o discriminante é:

[
Delta = b^2 - 4ac
]

| Condição | Classificação da solução |
| --- | --- |
| `a == 0` | Delega ao solucionador linear. |
| `Δ < 0` | `COMPLEX`; o solucionador real não retorna raízes complexas. |
| `Δ == 0` | `ONE`; ambas as saídas recebem a raiz repetida. |
| `Δ > 0` | `TWO`; as saídas recebem as duas raízes reais. |

~~~c
double x1 = 0.0;
double x2 = 0.0;
MODULE_ENUM(Math, Solution) count;

OPSTATUS status = MODULE_FUNC(Math, QuadraticEquation, Float64, Solve)(
    1.0, -3.0, 2.0, &x1, &x2, &count);
~~~

Os ponteiros de saída e o ponteiro de solução são todos obrigatórios. A API Quadratic atual classifica um discriminante negativo como complexo, mas não expõe um objeto de resultado de raízes complexas nem um solucionador complexo separado. `Sqrt` utiliza um cálculo iterativo limitado; utilize entradas finitas comuns e considere a precisão de ponto flutuante, overflow e underflow.

---

# Contratos e tratamento de erros

- Operações `Arithmetic` retornam `OPSTATUS` e escrevem por meio de um ponteiro de saída somente em caso de sucesso.
- `ClampedArithmetic`, `Clamp`, `Between.Is` e `MaxAndMin` retornam valores ou classificações booleanas diretamente.
- `Between.Classify` é o único auxiliar de intervalo que distingue um intervalo inválido de um valor simplesmente fora da faixa.
- `LinearEquation.Solve` e `QuadraticEquation.Solve` podem ter sucesso enquanto informam `NONE`, `INFINITE` ou `COMPLEX`. Sempre inspecione o enum de solução.
- O enum de solução é `MODULE_ENUM(Math, Solution)`; a classificação de faixa utiliza `MODULE_ENUM(Math, RangeResult)`.
- Operações inteiras são limitadas pelo tipo C selecionado. Operações de ponto flutuante herdam a precisão e o comportamento de valores excepcionais de C.

## Implementação e portabilidade

Os templates e as tabelas de instanciação estão nos cabeçalhos dos pacotes individuais e em `Impl/Common.inc`. Inclua o cabeçalho público, não `Impl/*.impl` nem `.def`, no código consumidor. O agregado `Math.h` não depende de uma biblioteca matemática separada para sua implementação atual de equações.

## Módulos relacionados

- [Numeric](Numeric.md) — valores de ponto fixo, racionais e de múltiplos limbs.
- [Algorithm](Algorithm.md) — comparação e repetição genérica.
- [Types](Types.md) — nomes e traits de tipos primitivos.
- [Module](Module.md) — declarações de templates e acesso a funções com namespace.

## Cabeçalhos de origem

- `Codespace/Cosmeron/Modules/Math/Math.h`
- `Codespace/Cosmeron/Modules/Math/Impl/Common.inc`
- `Codespace/Cosmeron/Modules/Math/Arithmetic/Basic.def`
- `Codespace/Cosmeron/Modules/Math/Arithmetic/Clamped.def`
- `Codespace/Cosmeron/Modules/Math/Value/Between.def`
- `Codespace/Cosmeron/Modules/Math/Value/Clamp.def`
- `Codespace/Cosmeron/Modules/Math/Value/MaxAndMin.def`
- `Codespace/Cosmeron/Modules/Math/Equation/Linear.def`
- `Codespace/Cosmeron/Modules/Math/Equation/Quadratic.def`
