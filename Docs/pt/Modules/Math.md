# Math

O módulo **Math** oferece operações aritméticas inteiras com verificação de limites, aritmética com restrição de faixa e saturação, auxiliares para intervalos de valores, funções de máximo e mínimo e solucionadores de equações lineares e quadráticas com valores reais. Suas declarações são templates C11 instanciados para os sufixos de tipo listados nas tabelas de tipos do módulo.

## Cabeçalhos

~~~c
#include "Cosmeron/Modules/Math/Math.h"
~~~

O cabeçalho agregador inclui todos os pacotes atuais:

| Pacote | Cabeçalho público | Finalidade |
| --- | --- | --- |
| Arithmetic | `Arithmetic/Basic.h` | Operações inteiras com e sem sinal verificadas. |
| ClampedArithmetic | `Arithmetic/Clamped.h` | Aritmética inteira limitada e saturada. |
| Between | `Value/Between.h` | Classificação e verificação de pertencimento a intervalos. |
| Clamp | `Value/Clamp.h` | Restringe um valor a um intervalo. |
| MaxAndMin | `Value/MaxAndMin.h` | Mínimo e máximo entre dois valores. |
| LinearEquation | `Equation/Linear.h` | Resolve uma equação linear. |
| QuadraticEquation | `Equation/Quadratic.h` | Discriminante, auxiliar de raiz quadrada e resolução de equações quadráticas. |

---

# Visão geral

## Especializações de tipos

Os sufixos públicos fazem parte dos nomes gerados. Use o sufixo exato, em vez de presumir um alias de tipo C.

| Sufixo | Tipo C | Pacotes disponíveis |
| --- | --- | --- |
| `Bool` | `bool` | Value |
| `Int8`, `Int16`, `Int32`, `Int64` | Inteiros com sinal de largura fixa | Arithmetic, ClampedArithmetic, Value |
| `UInt8`, `UInt16`, `UInt32`, `UInt64` | Inteiros sem sinal de largura fixa | Arithmetic, ClampedArithmetic, Value |
| `Char` | `char` | Value |
| `Float32` | `float` | Value, Equation |
| `Float64` | `double` | Value, Equation |
| `Float128` | `long double` | Value, Equation |
| `Int`, `UInt`, `Size`, `PtrDiff`, `IntPtr`, `UIntPtr` | Tipos C cujo tamanho depende da arquitetura | Value |

`Float128` é o sufixo correspondente a `long double` em C. Ele não garante que todas as plataformas utilizem uma representação efetiva de 128 bits.

---

# Referência da API

## Arithmetic

Cabeçalho: `Arithmetic/Basic.h`.

Os pacotes para tipos com e sem sinal são nomeados por `MODULE_FUNC(Math, Arithmetic, <Suffix>, <Operation>)`.

| Função | Assinatura | Comportamento |
| --- | --- | --- |
| `Add` | `(TYPE a, TYPE b, TYPE *outResult)` | Adição com verificação de limites. |
| `Sub` | `(TYPE a, TYPE b, TYPE *outResult)` | Subtração com verificação de limites. |
| `Mul` | `(TYPE a, TYPE b, TYPE *outResult)` | Multiplicação com verificação de limites. |
| `Div` | `(TYPE a, TYPE b, TYPE *outResult)` | Divisão com verificação de limites. |
| `Mod` | `(TYPE a, TYPE b, TYPE *outResult)` | Resto com verificação de limites. |

~~~c
int32_t sum = 0;
OPSTATUS status = MODULE_FUNC(Math, Arithmetic, Int32, Add)(
    20, 22, &sum);

if (status == STATUS_CONST(SUCCESS)) {
    /* sum is 42 */
}
~~~

Todas as operações exigem um ponteiro de saída não nulo. Overflow e operações aritméticas inválidas são informados por meio de `OPSTATUS`; o valor de saída só é escrito quando a operação tem sucesso. Divisão ou resto por zero são inválidos. A divisão do menor inteiro com sinal por -1 é informada como valor fora da faixa representável.

## ClampedArithmetic

Cabeçalho: `Arithmetic/Clamped.h`.

Cada especialização com ou sem sinal oferece dois grupos:

| Variante | Métodos | Parâmetros | Resultado |
| --- | --- | --- | --- |
| `Clamp` | `Add`, `Sub`, `Mul` | `a, b, minimum, maximum` | Retorna o resultado aritmético restringido ao intervalo informado. |
| `Saturating` | `Add`, `Sub`, `Mul` | `a, b` | Retorna um valor saturado nos limites representáveis do tipo. |

~~~c
int32_t bounded = MODULE_FUNC(Math, ClampedArithmetic, Int32, Clamp, Add)(
    INT32_MAX, 10, -100, 100);

int32_t saturated = MODULE_FUNC(Math, ClampedArithmetic, Int32, Saturating, Add)(
    INT32_MAX, 10);
~~~

A variante `Clamp` utiliza aritmética verificada. Se a operação intermediária causar overflow, escolhe o limite correspondente fornecido; caso contrário, restringe o valor calculado ao intervalo. A variante `Saturating` utiliza os limites mínimo e máximo próprios do tipo inteiro selecionado. Essas funções retornam um valor diretamente, e não um `OPSTATUS`.

## Between

Cabeçalho: `Value/Between.h`.

| Função | Descrição |
| --- | --- |
| `Classify(value, minimum, maximum)` | Retorna um `MODULE_ENUM(Math, RangeResult)` que indica se o valor está abaixo, dentro, acima ou diante de um intervalo inválido. |
| `Is(value, minimum, maximum)` | Retorna verdadeiro somente quando o valor está dentro do intervalo inclusivo. |

Os valores de `RangeResult` são `IN_RANGE`, `BELOW_RANGE`, `ABOVE_RANGE` e `INVALID_RANGE`. O intervalo inclui as duas extremidades. Se `minimum > maximum`, `Classify` retorna `INVALID_RANGE` e `Is` retorna falso.

~~~c
MODULE_ENUM(Math, RangeResult) position =
    MODULE_FUNC(Math, Between, Int32, Classify)(value, 10, 20);

bool accepted = MODULE_FUNC(Math, Between, Int32, Is)(value, 10, 20);
~~~

## Clamp

Cabeçalho: `Value/Clamp.h`.

`Apply(value, minimum, maximum)` retorna o valor restringido ao intervalo inclusivo. Se os extremos estiverem invertidos, a implementação os troca antes de limitar o valor.

~~~c
int32_t bounded = MODULE_FUNC(Math, Clamp, Int32, Apply)(42, 0, 10);
/* bounded is 10 */
~~~

Este auxiliar retorna um valor e não informa intervalo inválido; limites invertidos são normalizados.

## MaxAndMin

Cabeçalho: `Value/MaxAndMin.h`.

O pacote oferece somente as funções binárias `Max(a, b)` e `Min(a, b)` para cada especialização de tipo ordenável. Elas retornam diretamente o valor selecionado.

~~~c
int32_t maximum = MODULE_FUNC(Math, MaxAndMin, Int32, Max)(left, right);
int32_t minimum = MODULE_FUNC(Math, MaxAndMin, Int32, Min)(left, right);
~~~

A implementação atual não oferece APIs variádicas `Smallest` ou `Biggest`.

## LinearEquation

Cabeçalho: `Equation/Linear.h`.

Resolve a equação:

\[
a x + b = 0
\]

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

Os dois ponteiros de saída são obrigatórios. Uma chamada bem-sucedida indica que a equação foi classificada, mas não significa que exista uma raiz única; verifique o valor de `count`.

## QuadraticEquation

Cabeçalho: `Equation/Quadratic.h`.

O pacote oferece `Sqrt(x)`, `Discriminant(a, b, c)` e `Solve(a, b, c, outResult1, outResult2, outSolution)`.

Para:

\[
a x^2 + b x + c = 0
\]

o discriminante é:

\[
\Delta = b^2 - 4ac
\]

| Condição | Classificação da solução |
| --- | --- |
| `a == 0` | Encaminha a resolução ao solucionador linear. |
| `Δ < 0` | `COMPLEX`; o solucionador real não retorna raízes complexas. |
| `Δ == 0` | `ONE`; os dois valores de saída recebem a raiz repetida. |
| `Δ > 0` | `TWO`; os valores de saída recebem as duas raízes reais. |

~~~c
double x1 = 0.0;
double x2 = 0.0;
MODULE_ENUM(Math, Solution) count;

OPSTATUS status = MODULE_FUNC(Math, QuadraticEquation, Float64, Solve)(
    1.0, -3.0, 2.0, &x1, &x2, &count);
~~~

Os ponteiros dos resultados e o ponteiro da classificação da solução são obrigatórios. A API atual de Quadratic classifica um discriminante negativo como complexo, mas não oferece um objeto de resultado para raízes complexas nem um solucionador complexo separado. `Sqrt` utiliza um cálculo iterativo limitado; use entradas finitas e considere a precisão de ponto flutuante, overflow e underflow.

---

# Contratos e tratamento de erros

- As operações de `Arithmetic` retornam `OPSTATUS` e escrevem no ponteiro de saída somente quando têm sucesso.
- `ClampedArithmetic`, `Clamp`, `Between.Is` e `MaxAndMin` retornam valores ou classificações booleanas diretamente.
- `Between.Classify` é o único auxiliar de intervalos que distingue um intervalo inválido de um valor simplesmente fora da faixa.
- `LinearEquation.Solve` e `QuadraticEquation.Solve` podem ter sucesso e ainda informar `NONE`, `INFINITE` ou `COMPLEX`. Sempre verifique o enum da solução.
- O enum de solução é `MODULE_ENUM(Math, Solution)`; a classificação de intervalos utiliza `MODULE_ENUM(Math, RangeResult)`.
- As operações inteiras são limitadas pelo tipo C selecionado. As operações de ponto flutuante herdam a precisão e o comportamento de valores excepcionais definidos pela implementação de ponto flutuante de C.

## Implementação e portabilidade

Os templates e as tabelas de instanciação estão nos cabeçalhos de cada pacote e em `Impl/Common.inc`. No código consumidor, inclua o cabeçalho público, não arquivos `Impl/*.impl` ou `.def`. O cabeçalho agregador `Math.h` não depende de uma biblioteca matemática separada para a implementação atual das equações.

## Módulos relacionados

- [Numeric](Numeric.md) — valores de ponto fixo, racionais e de múltiplos limbs.
- [Algorithm](Algorithm.md) — comparação e repetição genérica.
- [Types](Types.md) — nomes de tipos primitivos e traits.
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
