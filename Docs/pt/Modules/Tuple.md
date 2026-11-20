# Tuple

O módulo **Tuple** fornece estruturas POD tipadas e de layout fixo para agrupar de dois a cinco valores. Inclui tanto tipos de tupla posicionais quanto aliases semânticos de dois valores, com funções geradas para empacotar e desempacotar.

## Cabeçalhos

~~~c
#include "Cosmeron/Modules/Tuple/Tuple.h"
~~~

O cabeçalho agregado inclui os templates `Pair`, `Dual`, `Triple`, `Quad` e `Penta`.

---

# Visão geral

| Tipo | Aridade | Uso pretendido |
| --- | ---: | --- |
| `Pair<T>` | 2 | Par ordenado genérico. |
| `Dual<T>` | 2 | Dois valores relacionados com aliases específicos do domínio, como coordenada, dimensão, intervalo ou componentes complexos. |
| `Triple<T>` | 3 | Três valores relacionados, como uma coordenada 3D ou canais RGB. |
| `Quad<T>` | 4 | Quatro valores relacionados, como um retângulo ou canais RGBA. |
| `Penta<T>` | 5 | Cinco valores ordenados. |

Cada especialização é um `STRUCT` com armazenamento C comum. Não possui propriedade de heap, alocação oculta nem receptor de objeto. Inicialização direta de estruturas e atribuição são suportadas.

## Tipos de elementos integrados

`TUPLE_TYPE_TABLE` em `Modules/Tuple/Impl/Common.inc` fornece estes sufixos de elementos:

- Boolean;
- inteiros com e sem sinal: Int8/16/32/64 e UInt8/16/32/64;
- larguras de caracteres: Char, Char8, Char16 e Char32;
- ponto flutuante: Float32, Float64, Float, Double e LongDouble;
- tipos inteiros/tamanho de C: Int, UInt, Size, PtrDiff, IntPtr e UIntPtr;
- FloatComplex, DoubleComplex e LongDoubleComplex quando o compilador não é MSVC.

Por exemplo, `MODULE_TYPED(Tuple, Pair, Int32)` nomeia um par de valores `int32_t`.

---

# Referência da API

Cada template de tupla fornece as mesmas duas funções de pacote:

| Função | Contrato |
| --- | --- |
| `Pack(value1, value2, ...)` | Retorna uma tupla inicializada com os valores fornecidos. |
| `Unpack(tuple, out1, out2, ...)` | Copia os elementos da tupla para ponteiros de saída do chamador e retorna `OPSTATUS`. |

A aridade determina a quantidade de argumentos. `Unpack` rejeita um ponteiro de saída nulo com `INVALID_ARGUMENT`.

## Pair

Os campos de `Pair` incluem os nomes canônicos `value1`/`first` e `value2`/`second`, os aliases `a`/`b` e `x1`/`x2`, além dos aliases de array `value[2]` e `values[2]`.

~~~c
MODULE_TYPED(Tuple, Pair, Int32) pair =
    MODULE_FUNC(Tuple, Pair, Int32, Pack)(10, 20);

int32_t first = 0;
int32_t second = 0;
OPSTATUS status = MODULE_FUNC(Tuple, Pair, Int32, Unpack)(
    pair, &first, &second);
~~~

## Dual

`Dual` é um POD de dois valores cujos aliases do primeiro campo são `value1`, `a`, `x1`, `x`, `col`, `length`, `real`, `state1`, `begin`, `first` e `start`. Os aliases do segundo campo são `value2`, `b`, `x2`, `y`, `row`, `width`, `imaginary`, `state2`, `end`, `second` e `last`.

~~~c
MODULE_TYPED(Tuple, Dual, UInt16) dimensions =
    MODULE_FUNC(Tuple, Dual, UInt16, Pack)(80U, 25U);

uint16_t columns = dimensions.col;
uint16_t rows = dimensions.row;
~~~

Utilize o alias que expresse o significado dos valores no código consumidor. Como são aliases de union, escreva uma interpretação semântica e leia por meio do alias correspondente; não trate os aliases como armazenamento independente.

## Triple, Quad e Penta

- `Triple` expõe `first`/`second`/`third`, `a`/`b`/`c`, `x`/`y`/`z`, `length`/`width`/`height`, `red`/`green`/`blue` e os aliases `value[3]`/`values[3]`.
- `Quad` expõe `first` até `fourth`, aliases posicionais, `value[4]`/`values[4]`, aliases geométricos `left`, `right`, `top`, `bottom` e aliases de cor `red`, `green`, `blue`, `alpha`.
- `Penta` expõe `first` até `fifth`, `value[5]` e `values[5]`.

Por exemplo, para um valor semelhante a um retângulo, `MODULE_FUNC(Tuple, Quad, UInt16, Pack)(left, right, top, bottom)` retorna um `Quad cujos campos podem ser lidos por meio desses aliases semânticos.

Os aliases exatos foram pensados para facilitar a nomenclatura, mas os valores continuam ocupando uma única representação POD de tupla compartilhada.

---

# Uso de templates

Os cabeçalhos de tupla instanciam automaticamente a tabela de tipos integrados atual. As definições públicas dos templates estão disponíveis nos arquivos `.def` correspondentes:

- `Pair.def`
- `Dual.def`
- `Triple.def`
- `Quad.def`
- `Penta.def`

O mecanismo comum de templates pode ser utilizado para instanciar outro tipo de elemento compatível. O sufixo do tipo gerado e o tipo de elemento C devem ser fornecidos na mesma ordem esperada pela macro correspondente `TEMPLATE_Tuple_<Name>(SUFFIX, TYPE)`.

---

# Contratos e limitações

- Valores Tuple são armazenados por valor e não alocam memória.
- `Pack` retorna a tupla completamente inicializada por valor.
- `Unpack` escreve cada elemento em um ponteiro de saída distinto, pertencente ao chamador.
- O tipo é tão seguro para cópia quanto seu tipo de elemento. As especializações integradas utilizam tipos escalares C; um tipo de elemento personalizado com propriedade ou invariantes autorreferenciais não deve ser tratado como trivialmente copiável sem verificar seu contrato.
- Tipos Tuple não fornecem comprimento dinâmico, iteração, verificações de limites nem elementos heterogêneos.
- Utilize uma estrutura dedicada quando os elementos tiverem tipos C diferentes ou precisarem de invariantes específicos do domínio.

## Módulos relacionados

- [Types](Types.md) — nomes canônicos de tipos primitivos.
- [Module](Module.md) — estruturas POD geradas e mecanismo de templates.

## Cabeçalhos de origem

- `Codespace/Cosmeron/Modules/Tuple/Tuple.h`
- `Codespace/Cosmeron/Modules/Tuple/Pair.def`
- `Codespace/Cosmeron/Modules/Tuple/Dual.def`
- `Codespace/Cosmeron/Modules/Tuple/Triple.def`
- `Codespace/Cosmeron/Modules/Tuple/Quad.def`
- `Codespace/Cosmeron/Modules/Tuple/Penta.def`
