# Condição (Condition)

O módulo **Condition** reúne status operacionais, valores de erro detalhados, limpeza por escopo, tratamento de panic e frames estruturados de exceção. Utilize o mecanismo mais simples adequado à falha: retorne um `OPSTATUS` em operações comuns sujeitas a falhas, associe um `Condition_Error` quando contexto ou uma cadeia de causas for útil e reserve os recursos de panic/exceção para contratos de fluxo de controle que realmente os exijam. Os cabeçalhos de Condition desabilitam a chamada por function table; chame diretamente as funções dos pacotes e os métodos de `Error` por meio de `MODULE_FUNC(Condition, ...)`.

## Cabeçalhos

~~~c
#include "Cosmeron/Core/Condition/Status.h"
#include "Cosmeron/Core/Condition/Error.h"
#include "Cosmeron/Core/Condition/Defer.h"
#include "Cosmeron/Core/Condition/Panic.h"
#include "Cosmeron/Core/Condition/Exception.h"
~~~

---

# Visão geral

| Componente | Finalidade |
| --- | --- |
| `Status` | Define `OPSTATUS`, categorias de status e consulta simbólica/de mensagens. |
| `Error` | Armazena um código de status, mensagem, origem de diagnóstico e causa opcional. |
| `Defer` | Executa ações de limpeza ao sair de um escopo léxico ou durante o desenrolamento de uma exceção. |
| `Panic` | Relata uma asserção, pré-condição, invariante ou caminho inalcançável violado. |
| `Exception` | Fornece fluxo de controle `TRY`/`CATCH`/`FINALLY` baseado em `setjmp`/`longjmp`. |

---

# Referência da API

## Pacote Status

Cabeçalho: `Status.h`.

`OPSTATUS` é o tipo canônico de status. Os valores de status e categoria são gerados a partir de `StatusTable/StatusTable.h`; utilize os auxiliares públicos em vez de codificar valores numéricos.

| Função | Descrição |
| --- | --- |
| `IsSuccess(status)` | Verdadeiro somente para `STATUS_CONST(SUCCESS)`. |
| `IsFailure(status)` | Verdadeiro para qualquer status diferente de sucesso. |
| `IsSpecial(status)` | Verdadeiro para os status especiais de fluxo declarados na tabela. |
| `CategoryOf(status)` | Retorna a categoria do status. |
| `IsCategory(status, category)` | Testa a categoria do status. |
| `CategoryName(category)` | Retorna o nome da categoria. |
| `Name(status)` | Retorna o nome simbólico do status. |
| `Message(status)` | Retorna a mensagem padrão legível por humanos. |

~~~c
OPSTATUS status = STATUS_CONST(INVALID_ARGUMENT);

if (MODULE_FUNC(Condition, Status, IsFailure)(status)) {
    const char *name = MODULE_FUNC(Condition, Status, Name)(status);
    const char *message = MODULE_FUNC(Condition, Status, Message)(status);
}
~~~

## Objeto Error

Cabeçalho: `Error.h`.

`MODULE_TYPE(Condition, Error)` armazena um código `OPSTATUS`, um ponteiro de mensagem, um valor `Diagnostic_Context` e um ponteiro opcional para um erro causador.

| Método | Contrato |
| --- | --- |
| `Create(code, message)` | Define o status e a mensagem fornecida pelo chamador. |
| `FromStatus(code)` | Cria um erro usando a mensagem padrão da tabela de status. |
| `Clear()` | Limpa o valor do erro. |
| `Code()`, `Message()` | Lê o status e a mensagem. |
| `Is(code)`, `Is.Success()`, `Is.Failure()`, `Is.Category(category)` | Consulta o código armazenado. |
| `Category()` | Retorna a categoria associada ao status. |
| `Set.Context(context)` | Copia o contexto de diagnóstico para o erro. |
| `Set.Origin(file, line, function, description, userData)` | Define os metadados de origem. |
| `Set.Cause(cause)` | Associa um erro causador. |
| `Context()`, `HasContext()` | Lê ou testa o contexto de diagnóstico. |
| `Cause()`, `HasCause()`, `RootCause()`, `Depth()` | Inspeciona a cadeia de causas. |

~~~c
MODULE_TYPE(Condition, Error) error = {0};

OPSTATUS status = MODULE_FUNC(Condition, Error, Create)(
    &error, STATUS_CONST(INVALID_ARGUMENT), "Invalid account identifier");

if (status == STATUS_CONST(SUCCESS)) {
    const char *message =
        MODULE_FUNC(Condition, Error, Message)(&error);
}
~~~

Os campos de mensagem e de strings de diagnóstico são ponteiros emprestados; o objeto Error não duplica as strings. Mantenha o armazenamento delas válido enquanto o erro as utilizar. O ponteiro da causa também não transfere propriedade.

## Pacote Defer

Cabeçalho: `Defer.h`.

O pacote expõe `Cancel(node)`, `IsActive(node)` e `Execute(node)` para a representação de nó defer. A maioria dos chamadores utiliza macros em vez de manipular os nós diretamente.

| Macro | Disponibilidade | Comportamento |
| --- | --- | --- |
| `DEFER_NAMED(name, function, context)` | GCC e Clang | Registra um nó de limpeza nomeado usando o atributo cleanup do compilador. |
| `DEFER(function, context)` | GCC e Clang | Registra uma limpeza com nome automático. |
| `DEFER_TRAITS_NAMED(name, traits, instance)` | GCC e Clang | Registra a liberação por meio de um descritor `Type_Traits`. |
| `DEFER_TRAITS(traits, instance)` | GCC e Clang | Limpeza baseada em traits com nome automático. |
| `DEFER_CANCEL(name)` | GCC e Clang | Cancela uma limpeza nomeada antes da saída do escopo. |
| `DEFER_SCOPE(action)` | C11 | Executa a ação fornecida quando o escopo de passagem única da macro é encerrado. |
| `DEFER_TRY_SCOPE(action)` | C11 com Exception | Combina um frame try com uma ação de saída de escopo. |

~~~c
static void CloseHandle(void *context) {
    int *handle = (int *)context;
    if (handle != NULL) {
        *handle = -1;
    }
}

void UseHandle(void) {
    int handle = 12;
    DEFER(CloseHandle, &handle);
    /* handle is released when this lexical scope exits */
}
~~~

As macros de limpeza com atributo `DEFER` são extensões de compilador disponíveis somente em GCC/Clang. Utilize `DEFER_SCOPE(action)` quando for preferível um escopo de passagem única portátil em C11. Este último possui a estrutura de fluxo de controle de instruções `for` aninhadas; evite saltos que ignorem o incremento do laço gerado pela macro.

## Pacote Panic

Cabeçalho: `Panic.h`.

`Panic_Info` contém o tipo de panic, mensagem, expressão que falhou, arquivo, linha e função. O pacote expõe `SetHandler(handler)`, `GetHandler()`, `ResetHandler()`, `KindName(kind)`, `Trigger(...)` e `Now(message)`.

As macros públicas capturam automaticamente os campos de localização no código-fonte:

| Macro | Contrato pretendido |
| --- | --- |
| `PANIC(message)` | Relata explicitamente um panic. |
| `ASSERT(condition)` | Asserção de desenvolvimento; é desabilitada quando `COSMERON_DISABLE_DEBUG_ASSERT` está definido. |
| `VERIFY(condition)` | Verifica uma condição em tempo de execução e aciona o handler configurado em caso de falha. |
| `ENSURE(condition)` | Impõe um contrato de pré-condição. |
| `UNREACHABLE()` | Relata a execução de um caminho que não deveria ser alcançado. |

Um panic não é um retorno de erro comum. O handler configurado determina o que acontece após o relato; não presuma que ele retorna ou que a execução pode continuar com segurança.

## Pacote Exception

Cabeçalho: `Exception.h`.

O pacote utiliza `setjmp`/`longjmp` e um `Exception_Frame` alocado na pilha. Os métodos públicos são `PushFrame`, `PopFrame`, `RegisterCleanup`, `Throw`, `ThrowStatus`, `Rethrow`, `CurrentError` e `HasActiveFrame`.

| Macro | Finalidade |
| --- | --- |
| `TRY` | Inicia um bloco protegido. |
| `CATCH` | Trata uma exceção lançada a partir do bloco try ativo. |
| `FINALLY` | Executa o bloco final após o processamento de try/catch. |
| `THROW(errorPtr)` | Lança um Error previamente preenchido. |
| `THROW_STATUS(status, message)` | Cria e lança um erro a partir de um status e da localização no código-fonte. |
| `RETHROW()` | Marca a exceção capturada atual para propagação após a conclusão do frame. |
| `CURRENT_ERROR()` | Fornece acesso ao erro armazenado no frame ativo. |

~~~c
TRY {
    /* fallible work */
    if (failed) {
        THROW_STATUS(STATUS_CONST(INVALID_ARGUMENT), "Bad input");
    }
} CATCH {
    const MODULE_TYPE(Condition, Error) *error = CURRENT_ERROR();
    (void)error;
} FINALLY {
    /* finalization for this frame */
}
~~~

Um `longjmp` não desenrola objetos arbitrários da pilha C nem executa destrutores no estilo C++. Registre a limpeza explicitamente quando necessário. Não salte entre escopos que contenham objetos cujas invariantes dependam da limpeza normal em C. O frame é local à pilha e deve permanecer válido durante toda a sequência try/catch/finally.

---

# Contratos e escolha do mecanismo

- Retorne `OPSTATUS` para falhas rotineiras de validação, E/S, alocação e aritmética.
- Utilize `Condition_Error` quando um status precisar de mensagem, origem de diagnóstico ou cadeia causal.
- Utilize `DEFER`/`DEFER_SCOPE` para limpeza determinística no fluxo de controle C.
- Utilize `PANIC`/`ASSERT`/`VERIFY`/`ENSURE` para contratos de programação violados, não para entradas de usuário recuperáveis.
- Utilize `TRY`/`CATCH` somente quando a propagação não local de erros fizer parte deliberadamente do projeto.

## Módulos relacionados

- [Diagnostic](Diagnostic.md) — contexto de origem e formatação de erros.
- [Types](Types.md) — operações de ciclo de vida baseadas em traits.
- [Module](Module.md) — declarações geradas de objetos, pacotes e closures.

## Cabeçalhos de origem

- `Codespace/Cosmeron/Core/Condition/Status.h`
- `Codespace/Cosmeron/Core/Condition/Error.h`
- `Codespace/Cosmeron/Core/Condition/Defer.h`
- `Codespace/Cosmeron/Core/Condition/Panic.h`
- `Codespace/Cosmeron/Core/Condition/Exception.h`
