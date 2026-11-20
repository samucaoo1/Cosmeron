# Numeric

O módulo **Numeric** fornece representações numéricas adicionais construídas a partir de tipos inteiros e de ponto flutuante de C: frações racionais exatas, valores complexos, ponto fixo e inteiros/valores decimais de largura fixa com múltiplos limbs.

## Cabeçalhos

~~~c
#include "Cosmeron/Modules/Numeric/Numeric.h"
~~~

O cabeçalho agregado inclui as declarações Rational, Complex, Fixed, Bigint e Decimal. Cabeçalhos individuais também podem ser incluídos quando apenas uma representação é necessária.

---

# Visão geral

## Especializações disponíveis

| Família | Representação | Especializações integradas atuais |
| --- | --- | --- |
| Rational | Numerador e denominador de um tipo inteiro. | `Int32`, `Int64` |
| Complex | Componentes real e imaginário de um tipo de ponto flutuante. | `Float32`, `Float64`, `Float128` |
| Fixed | Inteiro bruto com sinal mais uma quantidade de bits fracionários definida em tempo de compilação. | `Q8_8`, `Q16_16` |
| Bigint | Inteiro sem sinal de largura fixa com múltiplos limbs. | 128, 256, 512, 1024 bits |
| Decimal | Valor de largura fixa com múltiplos limbs e a mesma interface aritmética de inteiros. | 128, 256, 512, 1024 bits |

Os sufixos são gerados pelas tabelas em `Modules/Numeric/Impl/Common.inc`. Objetos Bigint/Decimal com sufixo de largura armazenam limbs de 64 bits; sua largura é fixa conforme o tipo selecionado.

---

# Referência da API

## Rational

Cabeçalho: `Rational.h`. Tipo: `MODULE_TYPED(Numeric, Rational, Int32)` ou `MODULE_TYPED(Numeric, Rational, Int64)`.

Um racional é armazenado como `numerator / denominator`. `Make` rejeita denominador zero, normaliza o sinal do denominador e reduz a fração pelo máximo divisor comum.

| Função | Contrato |
| --- | --- |
| `Gcd(a, b)` | Retorna o resultado auxiliar do máximo divisor comum. |
| `Make(numerator, denominator, outValue)` | Constrói um racional normalizado. |
| `Add(a, b, outValue)` | Soma duas frações. |
| `Sub(a, b, outValue)` | Subtrai duas frações. |
| `Mul(a, b, outValue)` | Multiplica duas frações. |
| `Div(a, b, outValue)` | Divide pela segunda fração; seu numerador não pode ser zero. |
| `Compare(a, b)` | Retorna a ordenação `CMPOUT`. |
| `ToFloat64(value)` | Converte a fração para `double`. |

~~~c
MODULE_TYPED(Numeric, Rational, Int32) third, sixth, half;

OPSTATUS status = MODULE_FUNC(Numeric, Rational, Int32, Make)(1, 3, &third);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Numeric, Rational, Int32, Make)(1, 6, &sixth);
}
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Numeric, Rational, Int32, Add)(third, sixth, &half);
}

double result = MODULE_FUNC(Numeric, Rational, Int32, ToFloat64)(half);
~~~

As operações Rational utilizam o tipo inteiro C subjacente selecionado. Não são operações racionais de precisão arbitrária; o chamador deve manter os valores intermediários dentro da faixa representável por esse tipo.

## Complex

Cabeçalho: `Complex.h`. Especializações: `Float32`, `Float64` e `Float128`.

Um valor complexo armazena os campos `real` e `imag` e representa `real + imag * i`.

| Função | Contrato |
| --- | --- |
| `Make(real, imag)` | Constrói um valor complexo. |
| `Add(a, b)`, `Sub(a, b)`, `Mul(a, b)` | Retornam o resultado aritmético. |
| `Magnitude.Squared(value)` | Retorna `real² + imag²`. |
| `Magnitude(value)` | Calcula a magnitude. |
| `Div(a, b, outValue)` | Divide em um valor de saída; rejeita divisor de magnitude zero. |
| `Conjugate(value)` | Retorna o conjugado complexo. |
| `Equals(a, b)` | Testa a igualdade exata dos componentes. |

~~~c
MODULE_TYPED(Numeric, Complex, Float64) value =
    MODULE_FUNC(Numeric, Complex, Float64, Make)(3.0, 4.0);

double magnitude =
    MODULE_FUNC(Numeric, Complex, Float64, Magnitude)(value);
~~~

`Equals` compara ambos os componentes exatamente; não é uma comparação por epsilon/tolerância. A implementação de magnitude utiliza um cálculo iterativo interno e os casos extremos de ponto flutuante continuam sujeitos aos limites normais de precisão.

## Fixed-point

Cabeçalho: `Fixed.h`. Tipos integrados: `Q8_8` e `Q16_16`.

Um valor de ponto fixo armazena um inteiro bruto com sinal. O sufixo codifica a configuração de armazenamento e dos bits fracionários: `Q8_8` utiliza 8 bits fracionários e `Q16_16` utiliza 16.

| Função | Contrato |
| --- | --- |
| `From.Raw(raw)` | Constrói diretamente a partir do inteiro bruto armazenado. |
| `From.Int(value)` | Converte um inteiro para a escala de ponto fixo. |
| `From.Float64(value)` | Converte um double para a escala de ponto fixo. |
| `To.Int(value)` | Converte para a parte inteira. |
| `To.Float64(value)` | Converte para double. |
| `Add(a, b)`, `Sub(a, b)`, `Mul(a, b)` | Retornam um valor de ponto fixo. |
| `Div(a, b, outValue)` | Divide em um valor de saída; retorna erro para divisor zero. |
| `Compare(a, b)` | Retorna a ordenação `CMPOUT`. |

~~~c
MODULE_TYPED(Numeric, Fixed, Q16_16) a =
    MODULE_FUNC(Numeric, Fixed, Q16_16, From_Float64)(2.5);
MODULE_TYPED(Numeric, Fixed, Q16_16) b =
    MODULE_FUNC(Numeric, Fixed, Q16_16, From_Int)(3);

MODULE_TYPED(Numeric, Fixed, Q16_16) product =
    MODULE_FUNC(Numeric, Fixed, Q16_16, Mul)(a, b);
double converted =
    MODULE_FUNC(Numeric, Fixed, Q16_16, To_Float64)(product);
~~~

A multiplicação e a conversão utilizam o tipo inteiro intermediário mais largo configurado para a especialização. A aritmética tem largura fixa; verifique a faixa de entrada em vez de presumir saturação.

## Bigint e Decimal

Cabeçalhos: `Bigint.h` e `Decimal.h`. Tipos padrão: de `MODULE_TYPE(Numeric, Bigint128)` até `Bigint1024`, e de `MODULE_TYPE(Numeric, Decimal128)` até `Decimal1024`.

São objetos de largura fixa respaldados por arrays de limbs de 64 bits, com o limb menos significativo no índice zero. Não são inteiros que crescem dinamicamente. `Decimal` não é um tipo decimal de ponto flutuante nem de escala fixa: utiliza a mesma representação de limbs inteiros e o mesmo modelo aritmético, com formatação de texto decimal. Utilize `Fixed` ou `Rational` quando precisar de valores fracionários.

| Grupo | Métodos |
| --- | --- |
| Estado | `Clear` |
| Aritmética | `Add`, `Sub`, `Mul`, `Div`, `Mod`, `Increment`, `Decrement` |
| Saídas da divisão | `Div.Mod(divisor, outQuotient, outRemainder)` |
| Bit a bit | `And`, `Or`, `Xor`, `Not` |
| Deslocamentos | `Shift.Left`, `Shift.Right` |
| Rotações | `Rotate.Left`, `Rotate.Right` |
| Bits individuais | `Bit.Set`, `Bit.Clear`, `Bit.Check` |
| Comparação | `Compare(other, outResult)`, `Equal`, `NotEqual`, `LessThan`, `GreaterThan`, `LessOrEqual`, `GreaterOrEqual` |
| Formatação | `ToCString.Base(base, buffer, bufferSize)`, `ToCString(buffer, bufferSize)` |
| Consulta | `IsZero` |

`Compare` escreve um valor `CMPOUT` por meio de `outResult` e retorna `OPSTATUS`; os predicados relacionais retornam `bool`. Os métodos aritméticos modificam o receptor e retornam `OPSTATUS`. `Div` escreve o quociente no receptor. A variante `Div.Mod` escreve quociente e resto em dois objetos de saída distintos sem alterar o receptor. `Mod` escreve o resto no receptor. Os métodos de formatação escrevem uma representação terminada em nulo em um buffer pertencente ao chamador; verifique o status e a capacidade.

~~~c
MODULE_TYPE(Numeric, Bigint128) left = {0};
MODULE_TYPE(Numeric, Bigint128) right = {0};

OBJECT_CALL_0(&left, Clear);
OBJECT_CALL_0(&right, Clear);

/* Set the low limb when constructing a small test value. */
left.limb[0] = 40U;
right.limb[0] = 2U;

OPSTATUS status = OBJECT_CALL(&left, Add, &right);
~~~

A representação por limbs está exposta no tipo gerado. Escrever limbs diretamente é apropriado para inicialização de baixo nível e serialização, mas a aritmética comum deve utilizar as operações fornecidas para que overflow seja informado.

---

# Erros e comportamento aritmético

- Operações sujeitas a falhas retornam `OPSTATUS`. Uma operação com falha pode deixar o receptor inalterado quando a implementação utiliza um resultado temporário; ainda assim, siga o contrato documentado e não presuma comportamento transacional em todos os casos.
- `INVALID_ARGUMENT` é utilizado para ponteiros inválidos e combinações de saída inválidas. O status para divisor zero depende da API: a divisão de limbs Bigint/Decimal informa `DIVISION_BY_ZERO`, enquanto Rational, Fixed e Complex atualmente informam `INVALID_ARGUMENT` para denominadores/divisores zero.
- `ARITHMETIC_OVERFLOW` é retornado quando uma operação de largura fixa com múltiplos limbs não consegue representar o resultado.
- O armazenamento Rational e de ponto fixo é limitado pelos tipos inteiros C subjacentes.
- Valores Complex são de ponto flutuante e podem apresentar arredondamento, overflow, underflow e comportamento NaN.
- A capacidade de Bigint/Decimal é fixa conforme a largura do tipo; overflow é informado, não se adiciona outro limb.

## Templates

As especializações Rational e Complex são geradas a partir de suas tabelas de tipos. Fixed utiliza uma linha de especialização de quatro campos: sufixo, tipo de armazenamento, tipo intermediário mais largo e quantidade de bits fracionários. Bigint e Decimal utilizam a tabela de larguras.

O mecanismo comum de templates permite instanciação personalizada quando um cabeçalho de módulo expõe a definição correspondente. Mantenha o formato da linha consistente com os parâmetros do template; uma largura ausente da tabela integrada atual não é fornecida automaticamente pelo cabeçalho agregado.

## Módulos relacionados

- [Math](Math.md) — aritmética escalar verificada, limites e equações.
- [Bit](Bit.md) — operações escalares de bits.
- [Types](Types.md) — traits de tipos e valores etiquetados.
- [Module](Module.md) — modelo de declaração de templates e objetos.

## Cabeçalhos de origem

- `Codespace/Cosmeron/Modules/Numeric/Rational.def`
- `Codespace/Cosmeron/Modules/Numeric/Complex.def`
- `Codespace/Cosmeron/Modules/Numeric/Fixed.def`
- `Codespace/Cosmeron/Modules/Numeric/Bigint.def`
- `Codespace/Cosmeron/Modules/Numeric/Decimal.def`
- `Codespace/Cosmeron/Modules/Numeric/Impl/Common.inc`
