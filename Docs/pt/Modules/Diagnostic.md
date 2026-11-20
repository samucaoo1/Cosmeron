# Diagnóstico (Diagnostic)

O módulo **Diagnostic** armazena metadados de origem do código, formata erros em buffers pertencentes ao chamador e captura stack traces quando a plataforma de destino oferece suporte. Seus cabeçalhos `Context`, `Format` e `Stacktrace` desabilitam function tables; utilize chamadas diretas `MODULE_FUNC(Diagnostic, ...)`.

## Cabeçalhos

~~~c
#include "Cosmeron/Core/Diagnostic/Context.h"
#include "Cosmeron/Core/Diagnostic/Format.h"
#include "Cosmeron/Core/Diagnostic/Stacktrace.h"
~~~

## Visão geral

| Componente | Finalidade |
| --- | --- |
| `Context` | Associa um diagnóstico a arquivo, linha, função, descrição e dados opacos do usuário. |
| `Format` | Produz texto simples, detalhado, encadeado ou somente de contexto em um buffer fornecido pelo chamador. |
| `Stacktrace` | Captura endereços de frames e os formata sem possuir um array de frames alocado dinamicamente. |

Os metadados de diagnóstico são separados do código de erro. Utilize `Condition_Error` para a falha e anexe ou formate um `Diagnostic_Context` quando a localização e a explicação forem relevantes.

## Referência da API

### Objeto Context

Cabeçalho: `Context.h`. Tipo: `MODULE_TYPE(Diagnostic, Context)`.

| Método | Descrição |
| --- | --- |
| `Create(file, line, function, description, userData)` | Define a origem do contexto e um ponteiro opcional para dados opacos. |
| `Clear()` | Redefine todos os campos do contexto. |
| `HasOrigin()` | Informa se há um arquivo de origem. |
| `File()`, `Line()`, `Function()`, `Description()`, `UserData()` | Retorna os campos armazenados. |

~~~c
MODULE_TYPE(Diagnostic, Context) context = {0};

OPSTATUS status = MODULE_FUNC(Diagnostic, Context, Create)(
    &context, __FILE__, (uint32_t)__LINE__, __func__,
    "Loading configuration", NULL);
~~~

As strings e o ponteiro `userData` são emprestados; o objeto não copia seu conteúdo. Garanta que o armazenamento referenciado permaneça válido durante o período em que o contexto for utilizado.

### Pacote Format

Cabeçalho: `Format.h`.

Cada formatador é uma `OPERATION` e recebe um buffer de saída, sua capacidade e um contador de saída opcional para a quantidade de bytes escritos.

| Função | Descrição |
| --- | --- |
| `Simple(error, outBuffer, capacity, outWritten)` | Formata o status e a mensagem de um `Condition_Error`. |
| `Detailed(error, outBuffer, capacity, outWritten)` | Formata um erro com seu contexto de diagnóstico. |
| `Chain(error, outBuffer, capacity, outWritten)` | Formata a cadeia de causas do erro. |
| `Context(context, outBuffer, capacity, outWritten)` | Formata um valor de contexto de diagnóstico. |

~~~c
char output[256];
size_t written = 0U;

OPSTATUS status = MODULE_FUNC(Diagnostic, Format, Simple)(
    &error, output, sizeof(output), &written);
if (status == STATUS_CONST(SUCCESS)) {
    /* output contains the formatted text */
}
~~~

O chamador é proprietário do buffer. Sempre informe sua capacidade real e verifique o status retornado antes de consumir a saída. Uma operação pode informar espaço insuficiente; não presuma que um buffer truncado contenha um diagnóstico completo.

### Objeto Stacktrace

Cabeçalho: `Stacktrace.h`. Tipo: `MODULE_TYPE(Diagnostic, Stacktrace)`.

O objeto armazena até o máximo configurado de endereços em seu próprio array fixo de frames; a configuração atual define o máximo padrão como 32.

| Método | Descrição |
| --- | --- |
| `Capture(skipFrames, maxFrames)` | Captura até a quantidade solicitada de frames após ignorar os frames iniciais indicados. |
| `Clear()` | Redefine a quantidade de frames capturados. |
| `FrameCount()` | Retorna a quantidade de endereços capturados. |
| `FrameAt(index, outAddress)` | Copia um endereço de frame para o ponteiro de saída. |
| `Format(outBuffer, capacity, outWritten)` | Formata os endereços capturados em um buffer pertencente ao chamador. |

~~~c
MODULE_TYPE(Diagnostic, Stacktrace) trace = {0};

OPSTATUS status = MODULE_FUNC(Diagnostic, Stacktrace, Capture)(
    &trace, 0U, 16U);

size_t count = MODULE_FUNC(Diagnostic, Stacktrace, FrameCount)(&trace);
~~~

A captura depende da plataforma e deve ser tratada como uma tentativa de melhor esforço. O objeto armazena endereços brutos; não promete nomes de símbolos, resolução de linhas de origem ou um mecanismo de unwind portátil em todos os destinos. Verifique o status de `Capture`, `FrameAt` e `Format`.

## Fluxo de trabalho típico

1. Construa um `Condition_Error` com status e mensagem.
2. Anexe um `Diagnostic_Context` para preservar a origem, quando disponível.
3. Utilize `Format.Simple` para logs compactos, `Format.Detailed` para erro e origem, ou `Format.Chain` quando as causas forem relevantes.
4. Capture um stack trace em um limite onde a plataforma ofereça suporte e depois formate-o em um buffer do chamador.

## Módulos relacionados

- [Condition](Condition.md) — valores de status, objetos de erro, panic e exceções.
- [Memory](Memory.md) — buffers gerenciados pelo chamador e propriedade de memória.
- [Module](Module.md) — convenções de métodos de objetos e pacotes.

## Cabeçalhos de origem

- `Codespace/Cosmeron/Core/Diagnostic/Context.h`
- `Codespace/Cosmeron/Core/Diagnostic/Format.h`
- `Codespace/Cosmeron/Core/Diagnostic/Stacktrace.h`
