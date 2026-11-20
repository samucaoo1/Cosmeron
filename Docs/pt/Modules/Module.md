# Module

O núcleo **Module** fornece o sistema declarativo de APIs em C do Cosmeron. A partir de declarações nos cabeçalhos, ele gera tipos com namespace, enums, constantes, structs, objetos, pacotes, callbacks, closures, templates e tabelas de funções opcionais.

## Cabeçalhos

~~~c
#include "Cosmeron/Core/Module/Module.h"
~~~

O cabeçalho agregador inclui os mecanismos de namespace, declaração, objetos, pacotes, callbacks, closures, despacho, tabelas de funções, templates e escopos de implementação utilizados pelos cabeçalhos públicos dos módulos.

---

# Visão geral

## Modelo de declaração

O modelo de declaração é organizado da seguinte forma:

| Construção | Finalidade |
| --- | --- |
| `DOMAIN` | Separa áreas abrangentes da biblioteca, como `Core` e `Modules`. |
| `MODULE` | Reúne uma família relacionada de declarações sob um namespace público. |
| `PACKAGE` | Agrupa funções livres que compartilham o nome de um pacote. |
| `OBJECT` | Declara um tipo com estado e métodos que recebem um receptor implícito do objeto. |
| `STRUCT` | Declara um tipo de dados C simples, sem receptor implícito de método. |
| `ISTRUCT` | Declara uma estrutura simples de uso interno do módulo. |
| `VARIANT` | Agrupa operações relacionadas em um caminho aninhado de método ou função. |
| `CALLBACK` | Declara a assinatura de um callback do tipo ponteiro para função. |
| `CLOSURE` | Declara uma assinatura invocável que transporta um ponteiro de contexto. |

Uma declaração não constitui um sistema de classes em tempo de execução. Objetos e pacotes são structs, funções e macros C gerados. A camada Module não fornece descoberta dinâmica de tipos nem coleta automática de lixo.

---

# Referência da API

## Namespace e nomes públicos

### Acesso ao namespace

Cabeçalho: `Namespace.h` (incluído por `Module.h`).

Use as macros semânticas de nomenclatura em vez de construir identificadores públicos manualmente:

| Macro | Finalidade |
| --- | --- |
| `MODULE_TYPE(Module, Type)` | Nomeia um tipo público não parametrizado por template. |
| `MODULE_TYPED(Module, Type, Suffix)` | Nomeia uma especialização tipada gerada. |
| `MODULE_ENUM(Module, Enum)` | Nomeia um enum do módulo. |
| `MODULE_CONST(Module, Constant)` | Nomeia uma constante do módulo. |
| `MODULE_FUNC(Module, Function)` | Nomeia uma função de pacote ou objeto. |
| `MODULE_FUNC(Module, Package, Function)` | Nomeia uma função de um pacote. |
| `MODULE_CLOSURE(Module, Package, Name)` | Nomeia um tipo de closure gerado em um pacote. |
| `MODULE_CALLBACK(Module, Name)` | Nomeia um tipo de callback declarado por um módulo. |
| `MODULE_NS(...)` | Forma uma sequência de tokens de namespace do módulo. |
| `NS(...)` / `CNS(...)` | Forma tokens de namespace comuns ou constantes. |

Por exemplo:

~~~c
MODULE_TYPE(Type, Traits) descriptor;
MODULE_TYPED(Numeric, Rational, Int32) fraction;
MODULE_ENUM(Algorithm, Comparison, Result) order;
~~~

O identificador efetivo é produzido pelo prefixo de biblioteca configurado e pelas regras de namespace. O código da aplicação deve preferir essas macros de acesso a nomes expandidos codificados manualmente.

### Configuração

O cabeçalho de namespace define as macros de prefixo e as políticas de namespace utilizadas pelo projeto. Configure-as antes de incluir os cabeçalhos públicos dos módulos. A política das tabelas de funções também é uma escolha feita em tempo de compilação: um módulo, pacote ou objeto pode definir uma opção de desativação antes da inclusão de seu cabeçalho.

Não altere a configuração de namespace ou de tabelas de funções entre unidades de tradução que compartilham os tipos afetados. Isso pode produzir nomes ou layouts de objetos incompatíveis.

A configuração de namespace e as macros de acesso público são fornecidas por `Core/Module/Namespace.h`.

---

## Pacotes

Um pacote é a construção adequada para operações que não precisam de um receptor de objeto.

### Declaração

~~~c
MODULE(DOMAIN(Modules), Example,
    PACKAGE(Math,
        FUNCTION(int, Add, (int left, int right)),
        OPERATION(Divide, (int left, int right, int *outValue))
    )
);
~~~

A declaração do pacote gera funções com namespace e, quando habilitada, uma tabela de funções tipada. A categoria da função determina seu contrato:

| Declaração | Contrato de retorno |
| --- | --- |
| `PROCEDURE` | Não retorna valor; é usada para efeitos colaterais. |
| `FUNCTION` | Retorna um valor diretamente. |
| `PREDICATE` | Retorna `bool`. |
| `OPERATION` | Retorna `OPSTATUS` e pode escrever em parâmetros de saída. |
| `COMPARISON` | Retorna o resultado de comparação de três vias da biblioteca. |

Use a categoria mais específica aplicável e mantenha operações sujeitas a falhas sob `OPERATION`.

### Invocação

A macro canônica para invocar funções de pacote por meio da tabela de funções é `PACKAGE_API`:

~~~c
int sum = PACKAGE_API(Example_Math, Add, 10, 20);
~~~

Para uma chamada direta com namespace, sem utilizar a tabela de funções, use `MODULE_FUNC`:

~~~c
int sum = MODULE_FUNC(Example, Math, Add)(10, 20);
~~~

As tabelas de funções dos pacotes são opcionais. Se estiverem desativadas, use a forma direta em vez de tentar realizar o despacho pelo pacote.

---

## Objetos

Um objeto combina um struct C com métodos que recebem um ponteiro para a instância como primeiro parâmetro receptor, fornecido implicitamente.

### Declaração

~~~c
MODULE(DOMAIN(Modules), Counter,
    OBJECT(Counter,
        FIELDS(
            int value;
        ),
        PROCEDURE(Reset, ()),
        OPERATION(Add, (int amount)),
        CONST_FUNCTION(int, Value, ())
    )
);
~~~

A API de objeto gerada fornece o receptor. Um método declarado com `CONST_` é somente de leitura, de acordo com seu contrato de receptor.

### Invocação

A macro canônica de despacho de objetos é `API`; `OBJECT_CALL` e `OBJECT_CALL_0` são aliases de compatibilidade:

~~~c
MODULE_TYPE(Counter, Counter) counter = {0};

API(&counter, Reset);
OPSTATUS status = API(&counter, Add, 5);
int value = API(&counter, Value);
~~~

Para uma chamada direta, sem despacho pela tabela de funções do objeto, use o método gerado com namespace:

~~~c
MODULE_FUNC(Counter, Counter, Add)(&counter, 5);
~~~

Use `API` quando o objeto possuir uma tabela de funções válida e o despacho em tempo de execução for desejado. Use chamadas diretas com `MODULE_FUNC` quando a configuração remover as tabelas de funções ou quando o despacho direto for preferível. Nunca chame um método em um objeto não inicializado ou inválido.

### Herança e extensões

A camada atual de declarações Object inclui `OBJECT_EXTENDS` e conversões tipadas, como `OBJECT_AS`. Esses recursos são mecanismos de declaração e layout C em tempo de compilação; não fornecem herança virtual nem gerenciamento automático de tempo de vida. Use-os somente quando o layout da estrutura base/derivada e o contrato do receptor forem explícitos.

---

## Structs e dados internos

Use `STRUCT` para um tipo de dados POD público e `ISTRUCT` para um detalhe de implementação. Um struct não possui receptor gerado e não ganha despacho de métodos apenas por ser declarado dentro de um módulo.

Para tipos de valor com layout fixo, prefira a inicialização direta dos campos e a atribuição comum quando o contrato de propriedade do tipo permitir. Tipos que exijam inicialização, clonagem, movimentação ou liberação personalizadas devem utilizar os recursos específicos de objetos ou traits, em vez de serem tratados como trivialmente copiáveis.

---

## Callbacks e closures

### Callback

Um callback é um ponteiro para função com uma assinatura declarada. Por si só, ele não transporta estado.

`Callback.h` fornece `TCALLBACK(Name)` para o tipo gerado, `CALLBACK_FUNC(Name)` para os nomes auxiliares e `CALLBACK_EMPTY` como valor de callback nulo.

### Closure

Uma closure combina um ponteiro para função com um ponteiro de contexto. Ela é adequada para callbacks que precisam acessar um estado pertencente ao chamador sem recorrer a uma variável global.

`Closure.h` fornece `TCLOSURE(Name)` e `CLOSURE_FUNC(Name)`. As macros comuns de chamada e validação são:

| Macro | Comportamento |
| --- | --- |
| `CLOSURE_IS_VALID(closure)` | Verifica se a closure contém uma função. |
| `CLOSURE_CALL(closure, ...)` | Invoca a closure. |
| `CLOSURE_EMPTY` | Representa uma closure vazia. |

As declarações de closure especificam o contrato de retorno: `CLOSURE_FUNCTION`, `CLOSURE_PROCEDURE`, `CLOSURE_OPERATION`, `CLOSURE_PREDICATE` ou `CLOSURE_COMPARISON`. O ponteiro de contexto de uma closure é emprestado; a camada Module não gerencia o tempo de vida do objeto apontado.

---

## Templates

A implementação canônica de templates do módulo utiliza um arquivo `.def` de modo duplo. Um template pode gerar uma especialização de struct, objeto ou pacote. Ele deve ser incluído pelo cabeçalho público e pode ser instanciado explicitamente ou a partir de uma tabela de tipos X-macro.

### Instanciação de uma especialização

Os auxiliares comuns de template são:

| Macro | Finalidade |
| --- | --- |
| `TEMPLATE_INSTANTIATE(Name, args...)` | Instancia um template nomeado. |
| `TEMPLATE_INSTANTIATE_TABLE(Name, Table)` | Instancia um template para cada linha retornada pela tabela. |
| `TEMPLATE_INSTANTIATE_TABLE(Module, Name, Table)` | Instancia um template qualificado pelo módulo para cada linha. |

~~~c
TEMPLATE_INSTANTIATE_TABLE(Numeric, Rational, NUMERIC_RATIONAL_TYPE_TABLE);
~~~

Cada linha da tabela é uma tupla de argumentos. Mantenha a ordem dos elementos da linha igual à ordem dos parâmetros da macro de template correspondente.

O mecanismo de templates também expõe auxiliares de declaração e implementação de nível mais baixo, mas os cabeçalhos públicos dos módulos devem encapsular esses detalhes. Os consumidores devem utilizar os pontos de entrada de instanciação documentados pelo módulo, em vez de invocar macros internas de implementação.

### Convenção de arquivos .def de modo duplo

O arquivo `.def` de um template define a macro do template e oferece suporte à instanciação direta quando as macros de configuração esperadas estão definidas. Em geral, o cabeçalho público inclui as definições internas compartilhadas, declara seu pacote ou objeto e inclui a definição do template. Não inclua arquivos internos `Impl` ou `Common.inc` diretamente no código da aplicação.

---

## Implementação e escopos

Implementações não inline são emitidas por meio de declarações explícitas de implementação do módulo. O sistema atual de declarações fornece `MODULE_IMPLEMENT`, `OBJECT_IMPLEMENT_EX` e `PACKAGE_IMPLEMENT_EX`. Auxiliares de escopo, como `MODULE_SCOPE`, `OBJECT_SCOPE` e `PACKAGE_SCOPE`, organizam implementações relacionadas e são encerrados por suas macros correspondentes `*_SCOPE_END`.

Em geral, um cabeçalho público contém as declarações e inclui sua unidade de implementação. Mantenha os corpos das funções nos arquivos de implementação fornecidos pelo módulo; os consumidores devem incluir somente o cabeçalho público.

---

## Tabelas de funções

As tabelas de funções oferecem um caminho de despacho tipado para pacotes e objetos. São saídas opcionais de compilação, não um sistema independente de métodos virtuais em tempo de execução.

- Use `PACKAGE_API` para despachar funções de pacote por meio de uma tabela do pacote.
- Use `API` para despachar métodos de objeto por meio da tabela da instância (`OBJECT_CALL` é um alias de compatibilidade).
- Use `MODULE_FUNC` para invocar diretamente um símbolo.
- Configure as opções de desativação relevantes antes de incluir os cabeçalhos.
- Mantenha a mesma configuração entre unidades de tradução que compartilham as declarações.

Desativar tabelas de funções pode alterar o layout gerado dos objetos. Não misture objetos compilados com políticas diferentes de tabelas.

---

## Orientações práticas

- Use `PACKAGE` para operações sem estado e `OBJECT` somente quando o estado pertencer à instância.
- Use `STRUCT` para valores POD; ele não possui receptor nem tabela de funções de objeto.
- Use `OPERATION` sempre que uma rotina puder falhar e retornar `OPSTATUS`.
- Use `FUNCTION` para resultados que retornam valores diretamente e `PREDICATE` para perguntas booleanas.
- Use `CALLBACK` para ponteiros simples para funções; use `CLOSURE` quando o contexto precisar acompanhar a função invocável.
- Mantenha os detalhes dos templates nos arquivos `.def` e exponha um cabeçalho público limpo.
- Trate todos os ponteiros passados como contexto, dados de callback ou campos de acordo com o contrato de propriedade documentado; o sistema Module não assume sua propriedade automaticamente.

## Módulos relacionados

- [Algorithm](Algorithm.md) — uso prático de closures em repetição e comparação.
- [Condition](Condition.md) — status e resultados de operações sujeitas a falhas.
- [Types](Types.md) — descritores de tipos primitivos e traits.
- [Preprocessor](Preprocessor.md) — auxiliares de tokens e tabelas utilizados pela camada declarativa.

## Cabeçalhos de origem

- `Codespace/Cosmeron/Core/Module/Module.h`
- `Codespace/Cosmeron/Core/Module/Namespace.h`
- `Codespace/Cosmeron/Core/Module/Object.h`
- `Codespace/Cosmeron/Core/Module/Package.h`
- `Codespace/Cosmeron/Core/Module/Callback/Callback.h`
- `Codespace/Cosmeron/Core/Module/Closure/Closure.h`
- `Codespace/Cosmeron/Core/Module/Common/Template.impl`
- `Codespace/Cosmeron/Core/Module/Common/Declarative.impl`
