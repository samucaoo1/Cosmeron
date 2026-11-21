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
---

# Portabilidade

- `Chronometry/Impl/Common.inc` define `_POSIX_C_SOURCE 200809L` caso não esteja definido, permitindo compilação direta com `-std=c11` em sistemas POSIX.
- Toda detecção de sistema operacional utiliza exclusivamente `Detect.h` (`OS_WINDOWS`, `OS_MAC`, POSIX).

## Módulos relacionados

- [Module](../Modules/Module.md) — DSL declarativa (`MODULE`, `PACKAGE`, `VARIANT`, `STRUCT`, `ENUM_DEF_DIRECT`).
- [Condition](../Modules/Condition.md) — contrato de retorno `OPSTATUS`.
- [Algorithm](../Modules/Algorithm.md) — contrato `CMPOUT` para comparações.
