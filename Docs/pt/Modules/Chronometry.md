# Chronometry

O módulo **Chronometry** fornece primitivas de medição de tempo, durações, relógios monotônicos e de parede, temporizadores, calendários históricos e modernos, conversão de épocas (incluindo TAI $\leftrightarrow$ UTC com segundos bissextos), fusos horários IANA com regras de horário de verão (DST), catálogo país $\leftrightarrow$ zona e formatação/parsing ISO 8601 sem dependência de `<stdio.h>`.

## Cabeçalhos

~~~c
#include "Cosmeron/Modules/Chronometry/Chronometry.h"
~~~

O cabeçalho agregado inclui todos os subpacotes do módulo:

- `Modules/Chronometry/Duration/Duration.h`
- `Modules/Chronometry/Clock/Clock.h`
- `Modules/Chronometry/Timer/Timer.h`
- `Modules/Chronometry/Instant/Instant.h`
- `Modules/Chronometry/Calendar/Calendar.h`
- `Modules/Chronometry/Timestamp/Timestamp.h`
- `Modules/Chronometry/Epoch/Epoch.h`
- `Modules/Chronometry/Timezone/Timezone.h`
- `Modules/Chronometry/Format/Format.h`

---

# Visão geral

| Subpacote | Tipos POD principais | Responsabilidade |
| --- | --- | --- |
| `Duration` | `TDuration` | Intervalos assinados em nanossegundos (`int64_t`) com aritmética segura contra overflow. |
| `Clock` | `TClockTimePoint` | Leitura do relógio monotônico do SO e pausa (`Sleep`). |
| `Timer` | `TTimer`, `TDelta`, `TFrameLimiter` | Cronometragem, delta entre frames e limitador de FPS. |
| `Instant` | `TInstant` | Instantes absolutos em segundos + nanossegundos UTC (`CLOCK_REALTIME` / `FILETIME`). |
| `Calendar` | `TDate`, `TIsoWeekDate`, `TCalendarPolicy` | Calendários Gregoriano, Juliano e reformas históricas; aritmética e semana ISO 8601. |
| `Timestamp` | `TTimeOfDay`, `TTimestamp` | Data + hora civil (precisão de milissegundos) sujeita a uma `CalendarPolicy`. |
| `Epoch` | — | Conversões JDN, Rata Die, Unix e TAI $\leftrightarrow$ UTC (27 segundos bissextos). |
| `Timezone` | `TUtcOffset`, `TOffsetTimestamp`, `TDstTransitionRule`, `TTimezoneRule` | Banco IANA (`tzdata 2026f`, 344 zonas, 253 links, 249 países), DST e fuso local do SO. |
| `Format` | — | Formatação e parsing estrito ISO 8601 (`ToISO` / `FromISO`), 100% freestanding. |

Todos os tipos expostos são **POD puros** (`STRUCT`), sem alocação dinâmica nem ponteiros de `FunctionTable` embutidos. O `FunctionTable` de pacotes pode ser desabilitado em tempo de compilação via `CHRONOMETRY_DISABLE_FUNCTION_TABLE` (global) ou `CHRONOMETRY_<SUBPACOTE>_DISABLE_FUNCTION_TABLE` (por subpacote).

---

# Referência da API

## Duration

`MODULE_TYPE(Chronometry, Duration)` armazena `int64_t nanoseconds`.

- **Constantes (`Unit`):** `NANOSECONDS_PER_MICROSECOND`, `NANOSECONDS_PER_MILLISECOND`, `NANOSECONDS_PER_SECOND`, `MICROSECONDS_PER_MILLISECOND`, `MICROSECONDS_PER_SECOND`, `MILLISECONDS_PER_SECOND`, `SECONDS_PER_MINUTE`, `MINUTES_PER_HOUR`, `HOURS_PER_DAY`, `MINUTES_PER_DAY`, `SECONDS_PER_HOUR`, `SECONDS_PER_DAY`.
- **`VARIANT(From, ...)`:** `FromNanoseconds` (infalível), `FromMicroseconds`, `FromMilliseconds`, `FromSeconds`, `FromMinutes`, `FromHours`, `FromDays` (retornam `OPSTATUS`, detectando `ARITHMETIC_OVERFLOW`).
- **`VARIANT(To, ...)`:** `ToNanoseconds`, `ToMicroseconds`, `ToMilliseconds`, `ToSeconds`.
- **Predicados e aritmética:** `IsZero`, `IsNegative`, `Add`, `Subtract`, `Compare`, `Absolute`, `Negate`, `Scale`.

~~~c
MODULE_TYPE(Chronometry, Duration) delay = {0};
OPSTATUS status = MODULE_FUNC(Chronometry, Duration, FromMilliseconds)(16, &delay);
~~~

## Clock e Timer

- **`Clock`:**
  - `Now(&outTimePoint)`: lê o relógio monotônico (`CLOCK_MONOTONIC` / `QueryPerformanceCounter` / `mach_absolute_time`).
  - `DurationBetween(start, end, &outDuration)`: calcula a duração entre dois `ClockTimePoint`.
  - `Sleep(duration)`: aguarda duração não-negativa (`nanosleep` com retomada em `EINTR` / `Sleep` no Windows).
- **`Timer`:**
  - `Start(&timer)`, `Elapsed(&timer, &outElapsed)`, `Restart(&timer, &outElapsed)`.
  - `SPACE(Delta)`: `Delta_Start(&delta)`, `Delta_Update(&delta, &outElapsed)`.
  - `SPACE(FrameLimiter)`: `Create(target, &limiter)`, `FromFPS(fps, &limiter)`, `Begin(&limiter)`, `Remaining(&limiter, &outRemaining)`, `Wait(&limiter)`. `Remaining` e `Wait` retornam `INVALID_ARGUMENT` se `Begin` ainda não tiver sido chamado.

## Instant

`MODULE_TYPE(Chronometry, Instant)` armazena `int64_t seconds` e `uint32_t nanoseconds` (`0 <= nanoseconds < 10^9`).

- `Now(&outInstant)`: lê o relógio de parede UTC do sistema (`CLOCK_REALTIME` / `GetSystemTimeAsFileTime`).
- `IsValid(instant)`, `Compare(left, right)`, `AddDuration(instant, duration, &outResult)`, `DurationBetween(start, end, &outResult)`.

## Calendar

- **Políticas e sistemas:** `GregorianPolicy()`, `SystemPolicy(system, &outPolicy)`, `ReformPolicy(reformId, &outPolicy)` (`CATHOLIC_1582`, `BRITISH_1752`, `RUSSIAN_1918`).
- **Utilidades:** `IsLeapYear(system, year)`, `DaysInMonth(system, year, month, &outDays)`, `DayOfYear(policy, date, &outDoy)`, `DayOfYearCumulative(system, date, &outDoy)`.
- **`SPACE(Date)`:**
  - `Compare`, `IsValid`, `ToDayOfYear`, `ToOrdinal`, `FromOrdinal`, `Difference`.
  - `DayOfWeek(policy, date, &outWeekday)` (`MONDAY = 1` .. `SUNDAY = 7`).
  - `FromNthWeekday(system, year, month, occurrence, weekday, &outDate)`.
  - `FromWeekdayAnchor(system, year, month, anchorKind, anchorDay, weekday, &outDate)` (`WEEKDAY_EXACT`, `WEEKDAY_ON_OR_AFTER`, `WEEKDAY_ON_OR_BEFORE`, `WEEKDAY_LAST`).
  - `AddDays`, `AddMonths`, `AddYears` (com ajuste de fim de mês em anos comuns/bissextos).
  - `ToIsoWeek(policy, date, &outIsoWeek)` e `FromIsoWeek(policy, isoWeek, &outDate)`.

## Timestamp

`MODULE_TYPE(Chronometry, Timestamp)` combina `Date date` e `TimeOfDay time` (`hour`, `minute`, `second`, `millisecond`).

- `IsValid(policy, value)`, `Compare(left, right)`, `AddDuration(policy, value, duration, &outResult)`, `Difference(policy, left, right, &outResult)`.
- `SPACE(Time)`: `Time_IsValid`, `Time_Compare`.

## Epoch

Organizado em famílias `VARIANT(...)`:

- **`VARIANT(DateTo)`:** `DateToJulianDayNumber(policy, date, &jdn)`, `DateToRataDie(date, &rataDie)`.
- **`VARIANT(DateFrom)`:** `DateFromJulianDayNumber(policy, jdn, &date)`, `DateFromRataDie(rataDie, &date)`.
- **`VARIANT(TimestampTo)`:** `TimestampToUnix(timestamp, &instant)`.
- **`VARIANT(TimestampFrom)`:** `TimestampFromUnix(instant, &timestamp)`.
- **`VARIANT(TAI)`:** `TAIMinusUtcSeconds(utc, &outSeconds)`, `TAIFromUtc(utc, &outTai)`, `TAIToUtc(tai, &outUtc)` (usando a tabela oficial de 27 segundos bissextos de `tzdata`, válida a partir de `1972-01-01T00:00:00Z`).

## Timezone

- **Conversões UTC:** `ToUTC(offsetTimestamp, &instant)`, `FromUTC(instant, offset, &offsetTimestamp)`, `FromUTCWithRule(instant, &rule, &offsetTimestamp)`, `FromUTCWithZone(instant, zoneId, &offsetTimestamp)`.
- **Fuso do sistema operacional:** `SystemOffsetAt(instant, &outOffset, &outIsDst)`, `SystemNow(&outOffsetTimestamp)`.
- **`SPACE(Offset)`:** `IsValid(offset)` (`[-1439, +1439]` minutos), `FromHoursMinutes(hours, minutes, &outOffset)`.
- **`SPACE(Zone)`:**
  - `DataVersion()` (retorna `"2026f"`).
  - `FromId(zoneId, &outRule)`, `FromName(ianaName, &outRule)` (busca binária nas 344 zonas canônicas e nos 253 aliases de `backward`).
  - `IsRuleValid(&rule)`, `IsDstAtLocal(&rule, local, &outIsDst)`.
  - `ClassifyLocal(&rule, local, &outStatus, &outEarlier, &outLater)` (`LOCAL_TIME_UNIQUE`, `LOCAL_TIME_AMBIGUOUS`, `LOCAL_TIME_NONEXISTENT`).
  - `ResolveLocal(&rule, local, &outResolved)` e `ResolveLocalWithPolicy(&rule, local, policy, &outResolved)` (`DISAMBIGUATE_COMPATIBLE`, `DISAMBIGUATE_EARLIER`, `DISAMBIGUATE_LATER`, `DISAMBIGUATE_REJECT`).
- **`SPACE(Country)`:** `NameFromCode(code, &outName)`, `ZoneCount(code, &outCount)`, `ZoneIdAt(code, index, &outZoneId)`, `ZoneRuleAt(code, index, &outRule)`.

### Atualização da base `tzdata`

Os arquivos `IanaZones.inc` e `LeapSeconds.inc` são gerados a partir dos fontes em `Codespace/Cosmeron/Asset/TimeZone/` via:

~~~sh
python3 .github/scripts/generate-tzdata.py
~~~

## Format

- **Capacidades (`BufferCapacity`):** `ISO_DATE_BUFFER_SIZE` (`16`), `ISO_TIMESTAMP_BUFFER_SIZE` (`32`), `ISO_OFFSET_TIMESTAMP_BUFFER_SIZE` (`40`), `ISO_INSTANT_BUFFER_SIZE` (`32`), `ISO_DURATION_BUFFER_SIZE` (`48`).
- **`VARIANT(ToISO)`:** `ToISODate`, `ToISOTimestamp`, `ToISOOffsetTimestamp`, `ToISOInstant`, `ToISODuration`.
- **`VARIANT(FromISO)`:** `FromISODate`, `FromISOTimestamp`, `FromISOOffsetTimestamp`, `FromISOInstant`, `FromISODuration`.
- **Regras ISO 8601:** anos entre `0000` e `9999` são emitidos sem sinal `+`; anos fora desse intervalo emitem sinal explícito (`-0044-03-15`, `+10000-01-01`). O parsing é estrito (rejeita espaços e campos sem padding) e não utiliza `<stdio.h>`.


## Semântica, limites e uso prático

### Escolhendo a representação temporal correta

Chronometry separa intencionalmente a medição de intervalos do tempo civil:

- **ClockTimePoint / Clock:** contador monotônico destinado à medição de tempo decorrido. Sua origem não é especificada; trate o valor como opaco, não o serialize como data de calendário e não o compare com um Instant.
- **Instant:** instante absoluto de relógio de parede, representado por segundos Unix e nanossegundos. É apropriado para timestamps e intercâmbio, mas o relógio do sistema pode ser ajustado; não o utilize para medir intervalos.
- **Duration:** intervalo assinado em nanossegundos. Utilize-o para tempo decorrido, operações aritméticas e pausas.
- **Timestamp:** data e hora civil com precisão de milissegundos. Não possui fuso horário nem offset UTC por si só. Uma CalendarPolicy determina como sua data é interpretada.
- **OffsetTimestamp:** Timestamp local associado a um offset UTC numérico e a um indicador de horário de verão. O offset é um dado, não um fuso horário nomeado.

Utilize Clock ou Timer para temporização de frames, timeouts e medições de desempenho. Utilize Instant, Timestamp e Timezone para datas de relógio de parede, agendamentos e conversões entre horário local e UTC.

### Política de calendário

Uma Date não é intrinsecamente gregoriana ou juliana. Passe a mesma CalendarPolicy para validação, conversão ordinal, aritmética e operações com timestamps sempre que os valores pertencerem a um sistema de calendário específico. GregorianPolicy() é a opção conveniente para datas civis modernas. SystemPolicy() seleciona um sistema gregoriano ou juliano puro; ReformPolicy() modela uma das transições históricas predefinidas e as datas civis suprimidas.

AddMonths() e AddYears() ajustam o dia quando o mês de destino é mais curto (por exemplo, ao mover uma data do dia 31 para um mês de 30 dias). Operações que atravessam uma lacuna de reforma ou excedem o intervalo suportado retornam um status, em vez de inventar silenciosamente uma data.

### Fusos horários e transições de horário de verão

O catálogo de fusos é gerado a partir dos dados IANA incorporados ao repositório. TimezoneRule é um perfil compacto de regras: offset UTC padrão, ajuste de DST e regras recorrentes de início/fim das transições. Não se trata de um banco TZif histórico completo. Não presuma que essa representação reproduza todas as mudanças políticas históricas ou todas as transições passadas de uma localidade.

Um horário civil local pode ser:

- LOCAL_TIME_UNIQUE: corresponde a exatamente um instante UTC.
- LOCAL_TIME_AMBIGUOUS: o relógio voltou e aquele horário local ocorreu duas vezes.
- LOCAL_TIME_NONEXISTENT: o relógio avançou e aquele horário foi suprimido.

ClassifyLocal() informa a classificação e os candidatos anterior/posterior em OffsetTimestamp. ResolveLocal() utiliza a política compatível: seleciona a ocorrência anterior em uma sobreposição e o candidato posterior em uma lacuna. ResolveLocalWithPolicy() permite escolher explicitamente DISAMBIGUATE_EARLIER, DISAMBIGUATE_LATER ou DISAMBIGUATE_REJECT; a política de rejeição retorna OUT_OF_RANGE quando o horário local não é único. Prefira uma política explícita ao processar agendamentos informados pelo usuário.

FromUTC() com offset numérico realiza uma conversão de offset fixo. FromUTCWithRule() e FromUTCWithZone() aplicam o perfil de regras selecionado. SystemOffsetAt() e SystemNow() consultam a configuração de fuso local do sistema operacional; não alteram essa configuração.

### Entrada e saída ISO 8601

O pacote Format escreve em buffers fornecidos pelo chamador e não aloca memória nem depende de <stdio.h>. As constantes de capacidade incluem o caractere nulo terminador. Sempre informe a capacidade real do buffer e verifique o OPSTATUS retornado; capacidade insuficiente é reportada como OUT_OF_RANGE.

- Uma data é interpretada usando o calendário gregoriano.
- Um timestamp é uma data-hora local (date + T + hora) e não possui offset.
- Um timestamp com offset inclui Z ou um offset numérico.
- Um instante é interpretado por meio de um timestamp com offset e convertido para UTC.
- Uma duração utiliza os componentes de dia/tempo suportados (D, H, M, S) e segundos fracionários opcionais; meses e anos de calendário não são unidades de duração com tamanho fixo.

O parsing é estrito: toda a entrada deve corresponder ao formato esperado. Espaços no início/fim, campos sem padding e caracteres excedentes são rejeitados. FromISODate() e FromISOTimestamp() utilizam a política gregoriana; para calendários históricos ou julianos, utilize diretamente a API Calendar.

### Tratamento de erros e propriedade

Operações que podem falhar retornam OPSTATUS e escrevem os resultados por parâmetros de saída. Verifique o status antes de consumir o valor de saída. Os tipos públicos do módulo são estruturas POD: o chamador controla seu armazenamento e os valores temporais centrais não exigem alocação dinâmica. Strings retornadas pelas funções de consulta de fusos e países referenciam dados estáticos incorporados; não devem ser liberadas pelo chamador.


## Contratos detalhados

### Duration

TDuration.nanoseconds é um int64_t com sinal. Os construtores de unidades validam os ponteiros de saída e detectam overflow de multiplicação; FromNanoseconds é um construtor direto. ToMicroseconds, ToMilliseconds e ToSeconds usam divisão inteira, descartam frações e truncam em direção a zero. Não arredondam. Absolute e Negate retornam ARITHMETIC_OVERFLOW para INT64_MIN. Add, Subtract e Scale também detectam overflow em vez de permitir wraparound.

### Ciclo de vida de Clock e Timer

Clock.Now retorna uma leitura monotônica em nanossegundos. Sua época é indefinida; leituras só fazem sentido quando comparadas dentro do mesmo domínio de relógio. DurationBetween é assinado e retorna valor negativo quando end antecede start.

Clock.Sleep rejeita durações negativas com INVALID_ARGUMENT; zero é uma operação bem-sucedida sem espera. O sistema operacional pode retomar a thread depois do tempo solicitado; Sleep não é uma garantia de escalonamento em tempo real.

Timer.Start estabelece o início. Elapsed consulta o intervalo sem alterá-lo; Restart retorna o intervalo e move o início para a nova leitura. Timer.Delta.Start estabelece a leitura anterior; Delta.Update retorna o intervalo desde a atualização anterior e avança essa leitura.

Use o limitador de frames nesta ordem: Create ou FromFPS; Begin no início de cada frame; execute o trabalho; depois Wait ou Remaining. Wait não inicia automaticamente o próximo frame. Chame Begin novamente. Remaining e Wait retornam INVALID_ARGUMENT antes da primeira chamada bem-sucedida a Begin. FromFPS calcula nanossegundos inteiros por frame (1.000.000.000 / fps), portanto taxas que não dividem um bilhão usam intervalo truncado.

### Instant e conversões de época

Instant é normalizado como segundos mais nanossegundos no intervalo [0, 1.000.000.000). Instantes negativos usam a mesma representação; meio segundo antes da época Unix é {-1, 500000000}.

Instant.Now lê o relógio de parede do sistema operacional, que pode ser ajustado por sincronização, alterações administrativas ou virtualização. DurationBetween calcula a diferença entre os instantes fornecidos; não garante progressão monotônica do relógio de parede.

Epoch.TimestampToUnix interpreta Timestamp como data-hora civil UTC no calendário gregoriano. Timestamp não contém fuso horário: não use essa conversão para hora local sem convertê-la primeiro para UTC. TimestampFromUnix realiza a conversão inversa UTC/gregoriana.

DateToJulianDayNumber e DateFromJulianDayNumber exigem política de calendário explícita. Rata Die utiliza a convenção gregoriana desta implementação e não recebe política.

TAI utiliza a tabela incorporada de segundos intercalares. TAIMinusUtcSeconds retorna OUT_OF_RANGE antes do período coberto. A tabela não é obtida nem atualizada em tempo de execução. TAI usa o mesmo contêiner de segundos/nanossegundos de Instant, mas representa outra escala temporal; não trate TAI como timestamp Unix UTC.

### Políticas de calendário e intervalo de datas

CalendarPolicy é uma regra de interpretação, não metadado armazenado em Date. Mantenha a mesma política em operações relacionadas de validação, aritmética, ordinal e dia da semana. GregorianPolicy retorna a política gregoriana por valor; SystemPolicy cria uma política gregoriana ou juliana pura; ReformPolicy cria uma das reformas históricas predefinidas.

As reformas representam uma descontinuidade entre a última data do sistema antigo e a primeira do novo. Datas no intervalo suprimido são inválidas nessa política. O modelo cobre apenas as três transições predefinidas, não um banco histórico de calendários por país.

AddMonths limita o dia original ao último dia do mês de destino. AddYears usa aritmética de meses; mover 29 de fevereiro para um ano não bissexto resulta no último dia de fevereiro. FromNthWeekday oferece FIRST_WEEKDAY a FOURTH_WEEKDAY e LAST_WEEKDAY. FromWeekdayAnchor oferece correspondência exata, no dia ou após, no dia ou antes e última ocorrência relativa a um dia âncora.

Datas de semana ISO usam segunda-feira como dia 1 e domingo como dia 7. O ano ISO pode diferir do ano civil próximo ao Ano-Novo. A política é necessária para converter entre data civil e data de semana ISO.

### Modelo de regras de fuso horário

UtcOffset.minutesEastOfUtc é o deslocamento assinado em minutos a leste de UTC: UTC−03:00 equivale a -180 e UTC+05:30 a 330. O intervalo aceito é de -1439 a +1439 minutos. FromHoursMinutes recebe componentes de hora e minuto com sinal; use sinais iguais para offsets negativos.

TimezoneRule contém offset padrão, ajuste DST e duas regras recorrentes de transição. Cada regra registra mês, âncora de dia da semana, dia da semana, hora/minuto local e base temporal (TIME_BASE_WALL, TIME_BASE_STANDARD ou TIME_BASE_UTC). É um modelo compacto, não um histórico TZif completo. Não representa todas as mudanças históricas, alterações políticas não recorrentes ou sequências arbitrárias de transições.

FromName resolve nomes IANA canônicos e aliases incorporados. FromId resolve o ZoneId gerado. ianaName aponta para armazenamento estático e não deve ser liberado. Consultas de país também retornam strings estáticas e registros de fusos incorporados. Códigos e índices inválidos retornam status de erro.

FromUTC aplica offset numérico fixo. FromUTCWithRule e FromUTCWithZone aplicam um perfil a um instante absoluto. SystemOffsetAt consulta o offset local do sistema operacional no instante informado; SystemNow combina o instante atual com o offset local. Essas funções dependem da configuração de fusos do sistema, que pode diferir do catálogo incorporado.

Para horário local inexistente, ClassifyLocal retorna candidatos dos dois lados da transição para frente. Para horário ambíguo, retorna as ocorrências anterior e posterior. Para horário único, ambas as saídas são iguais. ResolveLocalWithPolicy rejeita horários não únicos com DISAMBIGUATE_REJECT; EARLIER e LATER selecionam o candidato correspondente. COMPATIBLE escolhe a ocorrência anterior em sobreposição e o candidato posterior em lacuna.

### Contratos ISO 8601

Format é um formatador/parser com buffer limitado. Não aloca memória nem utiliza <stdio.h>. Funções de saída exigem buffer não nulo e capacidade diferente de zero; parsing exige entrada e ponteiros de saída não nulos. A capacidade deve incluir o byte nulo terminador. Buffer insuficiente retorna OUT_OF_RANGE.

- ToISODate e FromISODate usam calendário gregoriano.
- ToISOTimestamp e FromISOTimestamp operam sobre data-hora civil local sem offset.
- ToISOOffsetTimestamp e FromISOOffsetTimestamp preservam offset UTC numérico. O indicador DST não é codificado no texto ISO.
- ToISOInstant sempre formata UTC com Z. FromISOInstant aceita timestamp com offset e converte para UTC.
- ToISODuration emite duração ISO assinada de dia/tempo. O formatador normaliza dias inteiros em horas; o parser aceita componentes de dia e tempo.

O parsing consome toda a entrada. Espaços iniciais/finais, campos incompletos, datas/horas inválidas e caracteres excedentes são rejeitados. Datas civis ISO usam calendário gregoriano, independentemente de políticas históricas usadas em outras partes do módulo. O parser/formatador de datas aceita anos estendidos com sinal.

## Exemplos práticos

### Medir um intervalo

~~~c
#include "Cosmeron/Modules/Chronometry/Chronometry.h"

MODULE_TYPE(Chronometry, Timer) timer = {0};
MODULE_TYPE(Chronometry, Duration) elapsed = {0};

if (MODULE_FUNC(Chronometry, Timer, Start)(&timer) != STATUS_CONST(SUCCESS)) {
    return 1;
}

/* Execute aqui a operação que será medida. */

if (MODULE_FUNC(Chronometry, Timer, Elapsed)(&timer, &elapsed) !=
    STATUS_CONST(SUCCESS)) {
    return 1;
}
~~~

### Aritmética de calendário

~~~c
MODULE_TYPE(Chronometry, CalendarPolicy) gregorian =
    MODULE_FUNC(Chronometry, Calendar, GregorianPolicy)();
MODULE_TYPE(Chronometry, Date) nextMonth = {0};

OPSTATUS status = MODULE_FUNC(Chronometry, Calendar, Date, AddMonths)(
    &gregorian, (MODULE_TYPE(Chronometry, Date)){2024, 1, 31}, 1, &nextMonth);

/* Em caso de sucesso, nextMonth será 2024-02-29. */
~~~

### Resolver sobreposição de horário de verão

~~~c
MODULE_TYPE(Chronometry, TimezoneRule) newYork = {0};
MODULE_TYPE(Chronometry, Timestamp) local = {
    .date = {2026, 11, 1},
    .time = {.hour = 1, .minute = 30}
};
MODULE_TYPE(Chronometry, OffsetTimestamp) resolved = {0};

OPSTATUS status = MODULE_FUNC(Chronometry, Timezone, Zone, FromName)(
    "America/New_York", &newYork);

if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Chronometry, Timezone, Zone,
                         ResolveLocalWithPolicy)(
        &newYork, local,
        MODULE_CONST(Chronometry, Timezone, DISAMBIGUATE_LATER),
        &resolved);
}
~~~

### Formatar instante

~~~c
MODULE_TYPE(Chronometry, Instant) instant = {0};
char text[MODULE_CONST(Chronometry, Format, ISO_INSTANT_BUFFER_SIZE)];

OPSTATUS status = MODULE_FUNC(Chronometry, Instant, Now)(&instant);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Chronometry, Format, ToISOInstant)(
        instant, text, sizeof(text));
}
~~~

## Cabeçalhos públicos e mapa da implementação

| Subpacote | Cabeçalho público | Implementação |
| --- | --- | --- |
| Duration | Duration/Duration.h | Duration/Impl/Duration.impl |
| Clock | Clock/Clock.h | Clock/Impl/Clock.impl |
| Timer | Timer/Timer.h | Timer/Impl/Timer.impl |
| Instant | Instant/Instant.h | Instant/Impl/Instant.impl |
| Calendar | Calendar/Calendar.h | Calendar/Impl/Calendar.impl |
| Timestamp | Timestamp/Timestamp.h | Timestamp/Impl/Timestamp.impl |
| Epoch | Epoch/Epoch.h | Epoch/Impl/Epoch.impl e Epoch/Impl/LeapSeconds.inc |
| Timezone | Timezone/Timezone.h | Timezone/Impl/Timezone.impl e Timezone/Impl/IanaZones.inc |
| Format | Format/Format.h | Format/Impl/Format.impl |

O teste de integração está em Codespace/Tests/Chronometry/Chronometry.c. Ele cobre conversões, reformas de calendário, semanas ISO, TAI/UTC, lacunas e sobreposições DST, aliases de fusos, consulta de países e formatação/parsing.
---

# Portabilidade

- `Chronometry/Impl/Common.inc` define `_POSIX_C_SOURCE 200809L` caso não esteja definido, permitindo compilação direta com `-std=c11` em sistemas POSIX.
- Toda detecção de sistema operacional utiliza exclusivamente `Detect.h` (`OS_WINDOWS`, `OS_MAC`, POSIX).

## Módulos relacionados

- [Module](../Modules/Module.md) — DSL declarativa (`MODULE`, `PACKAGE`, `VARIANT`, `STRUCT`, `ENUM_DEF_DIRECT`).
- [Condition](../Modules/Condition.md) — contrato de retorno `OPSTATUS`.
- [Algorithm](../Modules/Algorithm.md) — contrato `CMPOUT` para comparações.
