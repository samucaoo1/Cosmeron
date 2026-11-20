# Bits (Bit)

O módulo **Bit** fornece operações de bits para inteiros sem sinal e bitsets de capacidade fixa. `Bit` trabalha diretamente sobre valores inteiros escalares; `Bitset` armazena uma quantidade de bits selecionada em tempo de compilação usando limbs de 64 bits.

## Cabeçalhos

~~~c
#include "Cosmeron/Core/Bit/Bit.h"
#include "Cosmeron/Core/Bit/Bitset.h"
~~~

---

# Visão geral

## Operações escalares

O template `Bit` gera um pacote para um tipo inteiro sem sinal. Os pacotes padrão são instanciados para as larguras declaradas por `BIT_WIDTH_TYPE_TABLE`. A API abrange bits individuais, máscaras, contagens de bits, contagens de zeros, auxiliares de potências de dois, rotações, extração/inserção de campos, inversão de bits e troca de bytes.

## Bitsets de tamanho fixo

`Bitset` gera um objeto cuja capacidade é fixada em tempo de compilação. Seu armazenamento é um array de limbs `uint64_t`; o último limb é mascarado para que bits fora da capacidade declarada não se tornem observáveis. Um bitset é útil para flags, conjuntos de recursos, mapas compactos de pertencimento e álgebra de conjuntos por operações de bits quando a quantidade de posições é conhecida antecipadamente.

As especializações padrão são `Bitset128`, `Bitset256`, `Bitset512` e `Bitset1024`.

---

# Referência da API

## Pacote Bit escalar

Cabeçalho: `Cosmeron/Core/Bit/Bit.h`.

Para um pacote escalar gerado com sufixo `<Width>`, o prefixo de função direta é `MODULE_FUNC(Bit, <Width>, ...)`. Sufixos comuns incluem `8`, `16`, `32` e `64`.

| Grupo | Funções | Contrato |
| --- | --- | --- |
| Bit individual | `Set`, `Clear`, `Flip`, `Check` | Define, limpa, inverte ou testa um índice de bit iniciado em zero. |
| Máscara | `Mask.Set`, `Mask.Clear`, `Mask.Flip`, `Mask.CheckAll`, `Mask.CheckAny`, `Mask.Range` | Aplica ou testa uma máscara, ou constrói uma máscara de intervalo inclusivo alto/baixo. |
| Contagens | `Popcount`, `Count.LeadingZeros`, `Count.TrailingZeros` | Conta bits definidos ou zeros. |
| Propriedades | `Parity`, `IsPowerOfTwo` | Testa paridade ou se um valor não nulo possui exatamente um bit definido. |
| Potências e logaritmos | `FloorPowerOfTwo`, `NextPowerOfTwo`, `Log2.Floor`, `Log2.Ceil` | Arredonda para potências de dois ou calcula logaritmos inteiros na base dois. |
| Índices de bits | `Index.LSB`, `Index.MSB` | Encontra o índice do bit definido menos ou mais significativo. |
| Bit definido mais baixo | `IsolateLowestSet`, `ClearLowestSet` | Isola ou limpa o bit definido mais baixo. |
| Rotação | `Rotate.Left`, `Rotate.Right` | Rotaciona dentro da largura do inteiro selecionado. |
| Representação | `ReverseBits`, `ByteSwap` | Inverte a ordem dos bits ou dos bytes. |
| Campos de bits | `Extract`, `Insert` | Lê ou substitui um campo de bits com extremidades inclusivas. |

### Bits individuais

~~~c
uint32_t flags = 0U;

OPSTATUS status = MODULE_FUNC(Bit, 32, Set)(&flags, 5U);
bool enabled = MODULE_FUNC(Bit, 32, Check)(flags, 5U);
~~~

Operações de mutação recebem um ponteiro para o valor e retornam `OPSTATUS`. `Check` recebe o valor por cópia e retorna `bool`. Índices de bits começam em zero e devem ser menores que a largura do tipo.

### Máscaras

`Mask.Set`, `Mask.Clear` e `Mask.Flip` são procedimentos: modificam todos os bits selecionados pela máscara e não fazem nada quando o ponteiro do valor é nulo. `Mask.CheckAll` testa se todos os bits solicitados estão definidos; `Mask.CheckAny` testa se pelo menos um está definido. `Mask.Range` pode falhar e informa ponteiro de saída inválido ou extremidades invertidas/fora da largura por meio de `OPSTATUS`.

`Mask.Range(high, low, outMask)` cria uma máscara que cobre ambas as extremidades. Rejeita intervalo invertido e índices fora da largura escalar.

~~~c
uint32_t fieldMask = 0U;
OPSTATUS status = MODULE_FUNC(Bit, 32, Mask, Range)(15U, 8U, &fieldMask);
~~~

### Contagens, índices e potências

`Popcount` retorna a quantidade de bits definidos. As contagens de zeros iniciais/finais são medidas dentro da largura do tipo selecionado. `Index.LSB` e `Index.MSB` utilizam um parâmetro de saída e retornam `NOT_FOUND` quando a entrada é zero.

`NextPowerOfTwo`, `Log2.Floor`, `Log2.Ceil` e as operações de índice podem falhar porque algumas entradas não possuem resultado válido ou provocariam overflow. Verifique o `OPSTATUS` antes de ler o parâmetro de saída.

### Rotações e campos de bits

Contagens de rotação são ajustadas pelo módulo da largura do inteiro. `Extract(value, high, low, outField)` e `Insert(value, high, low, fieldValue)` usam extremidades inclusivas. Validam o ponteiro de saída ou o ponteiro de entrada mutável e rejeitam limites inválidos.

---

## Pacote Bitset

Cabeçalho: `Cosmeron/Core/Bit/Bitset.h`.

Os tipos de objeto gerados são `MODULE_TYPE(Bitset128)`, `MODULE_TYPE(Bitset256)`, `MODULE_TYPE(Bitset512)` e `MODULE_TYPE(Bitset1024)`.

| Grupo | Métodos |
| --- | --- |
| Inicialização e estado base | `Clear`, `SetAll` |
| Álgebra de conjuntos | `And`, `Or`, `Xor`, `Not` |
| Deslocamentos | `ShiftLeft`, `ShiftRight` |
| Bit individual | `Bit.Set`, `Bit.Clear`, `Bit.Flip`, `Bit.Check`, `Bit.Test` |
| Intervalos | `SetRange`, `ClearRange`, `FlipRange` |
| Consultas | `Count`, `Capacity`, `Any`, `All`, `None`, `Equal`, `Intersects` |
| Relações | `Is.Subset`, `Is.Disjoint` |
| Busca | `Find.FirstSet`, `Find.FirstZero`, `Find.NextSet` |

### Exemplo

~~~c
MODULE_TYPE(Bitset128) permissions = {0};

OPSTATUS status = MODULE_FUNC(Bitset128, Clear)(&permissions);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Bitset128, Bit, Set)(&permissions, 7U);
}

bool hasPermission = MODULE_FUNC(Bitset128, Bit, Test)(&permissions, 7U);
size_t setCount = MODULE_FUNC(Bitset128, Count)(&permissions);
~~~

`Bitset.h` desabilita deliberadamente sua function table de objeto; portanto, chame seus métodos gerados diretamente com `MODULE_FUNC(Bitset128, ...)`; `OBJECT_CALL` não está disponível para esses tipos. Operações de bitset que modificam estado retornam `OPSTATUS`. A forma verificada `Bit.Check` escreve um Boolean em um parâmetro de saída; a forma conveniente `Bit.Test` retorna `bool` diretamente e avalia como false para um objeto inválido ou índice fora da faixa. Métodos `Find` retornam `NOT_FOUND` quando não há bit correspondente.

Operações de intervalo utilizam um intervalo semiaberto `[startBit, endBit)`: o início é incluído e o fim é excluído. A extremidade final pode ser igual à capacidade do bitset.

---

# Contratos e configuração

- As funções escalares `Bit` operam sobre tipos inteiros sem sinal; não passe um tipo com sinal presumindo semântica de deslocamento idêntica.
- Todos os índices de bits começam em zero.
- Extremidades de intervalo para extração/inserção de máscaras escalares são inclusivas; métodos de intervalo de Bitset utilizam intervalos semiabertos.
- A capacidade de `Bitset` é fixa pelo sufixo do tipo; ela não cresce em tempo de execução.
- Ponteiros inválidos e índices fora da faixa são informados por operações que retornam status.
- `Bit.h` oferece configuração de function tables por meio de switches globais e específicos do pacote. `Bitset.h` desabilita incondicionalmente sua function table na implementação atual; seus métodos sempre usam chamadas diretas `MODULE_FUNC`.

~~~c
#define BIT_DISABLE_FUNCTION_TABLE 1
#include "Cosmeron/Core/Bit/Bit.h"
#include "Cosmeron/Core/Bit/Bitset.h"
~~~

## Módulos relacionados

- [Algorithm](Algorithm.md) — auxiliares de comparação e repetição.
- [Memory](Memory.md) — alocação em tempo de execução e recursos relacionados a bitsets.
- [Types](Types.md) — catálogo de tipos primitivos e operações com limbs.

## Cabeçalhos de origem

- `Codespace/Cosmeron/Core/Bit/Bit.def`
- `Codespace/Cosmeron/Core/Bit/Bitset.def`
- `Codespace/Cosmeron/Core/Types/Impl/Common.inc`
