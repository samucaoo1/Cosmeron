# Chrono

O módulo **Chrono** fornece primitivas de medição de tempo, spans e intervalos assinados, relógios monotônico e de parede, cronômetros, calendários históricos e modernos, conversões de escala temporal (Unix e TAI $\leftrightarrow$ UTC com segundos bissextos), fusos horários IANA com regras de horário de verão (DST), catálogo país $\leftrightarrow$ fuso e formatação/parsing freestanding (sem `<stdio.h>`) para ISO 8601, RFC 3339, RFC 2822, padrões customizados e spans legíveis.

## Cabeçalhos

~~~c
#include "Cosmeron/Modules/Chrono/Chrono.h"
~~~

O cabeçalho agregado inclui os 6 subpacotes do módulo:

- `Modules/Chrono/Math/Math.h`
- `Modules/Chrono/Clock/Clock.h`
- `Modules/Chrono/Calendar/Calendar.h`
- `Modules/Chrono/Civil/Civil.h`
- `Modules/Chrono/Timezone/Timezone.h`
- `Modules/Chrono/Format/Format.h`

---

# Visão geral

| Subpacote | Tipos POD principais (`TChrono_*`) | Responsabilidade |
| --- | --- | --- |
| `Math` | `Span`, `Interval` | Durações assinadas em nanossegundos (`int64_t`), aritmética segura/saturada, Hz, arredondamento e intervalos semi-abertos. |
| `Clock` | `ClockPoint`, `Instant`, `Timer`, `TimerDelta`, `TimerLimiter` | Relógio monotônico do SO, `Sleep`/`SleepUntil`, resolução, cronômetro pausável, delta, limitador de taxa, `Instant` UTC, Unix e TAI $\leftrightarrow$ UTC. |
| `Calendar` | `Date`, `DateIsoWeek`, `CalendarReform`, `CalendarPolicy` | Calendários Gregoriano, Juliano e reformas históricas; JDN/Rata Die, semanas ISO 8601, trimestres e dias úteis em $O(1)$. |
| `Civil` | `TimeOfDay`, `CivilStamp`, `UtcOffset`, `CivilOffset` | Data + hora civil (precisão de milissegundos), âncoras do dia, truncamento, aritmética de `TimeOfDay` com carry e conversão UTC `Instant` $\leftrightarrow$ `CivilStamp`. |
| `Timezone` | `TimezoneDstRule`, `TimezoneRule` | Base IANA (`tzdata 2026f`, 344 zonas, 253 links, 249 países), desambiguação DST, conversão direta entre fusos, consulta de transições e fuso do SO. |
| `Format` | — | Formatação/parsing freestanding ISO 8601, RFC 3339, RFC 2822, formatador por padrão (`Pattern`) e `ToHumanSpan`. |

Todos os tipos expostos são **POD puros** (`STRUCT`), sem alocação dinâmica nem ponteiros de `FunctionTable` embutidos. A emissão de `FunctionTable` pode ser desabilitada via `CHRONO_DISABLE_FUNCTION_TABLE` (módulo completo) ou `CHRONO_<SUBPACOTE>_DISABLE_FUNCTION_TABLE` (por subpacote).

---

# Referência da API

## Math

`MODULE_TYPE(Chrono, Span)` armazena `int64_t nanoseconds`. `MODULE_TYPE(Chrono, Interval)` representa um intervalo semi-aberto `[start, end)`.

- **Constantes (`Unit`):** `NANOSECONDS_PER_MICROSECOND` .. `SECONDS_PER_DAY`.
- **Granularidade (`Granularity`):** `GRANULARITY_NANOSECOND` a `GRANULARITY_DAY`.
- **`VARIANT(From, ...)`:** `FromNanoseconds`, `FromMicroseconds`, `FromMilliseconds`, `FromSeconds`, `FromMinutes`, `FromHours`, `FromDays`, `FromHz(double hz, &outSpan)`.
- **`VARIANT(To, ...)`:** `ToNanoseconds`, `ToMicroseconds`, `ToMilliseconds`, `ToSeconds`, `ToHz(span, &outHz)`.
- **Aritmética e limites:** `IsZero`, `IsNegative`, `Compare`, `Add`, `Subtract`, `Absolute`, `Negate`, `Scale`, `Divide`, `Remainder`, `Ratio`, `AddSaturating`, `SubtractSaturating`, `Min`, `Max`, `Clamp`, `Truncate`, `Round`.
- **`SPACE(Interval)`:** `IsValid`, `Length`, `Contains`, `Overlaps`, `Intersection`, `Union`.

~~~c
MODULE_TYPE(Chrono, Span) frame = {0};
OPSTATUS status = MODULE_FUNC(Chrono, Math, FromHz)(60.0, &frame);
~~~

## Clock

- **Relógio monotônico:** `Now(&outPoint)`, `SpanBetween(start, end, &outSpan)`, `Sleep(span)`, `SleepUntil(targetPoint)`, `Resolution(&outSpan)`.
- **`SPACE(Instant)`:**
  - `Now(&outInstant)`, `IsValid(instant)`, `Compare(left, right)`, `AddSpan(instant, span, &outResult)`, `SpanBetween(start, end, &outResult)`.
  - `ToUnixSeconds(instant)`, `FromUnixSeconds(seconds)`, `ToUnixMillis(instant, &outMillis)`, `FromUnixMillis(millis, &outInstant)`.
  - `TaiMinusUtcSeconds(utc, &outSeconds)`, `ToTAI(utc, &outTai)`, `FromTAI(tai, &outUtc)` (27 segundos bissextos de `tzdata`, válido a partir de `1972-01-01T00:00:00Z`).
- **`SPACE(Timer)`:** `Start(&timer)`, `Pause(&timer)`, `Resume(&timer)`, `IsRunning(&timer)`, `Elapsed(&timer, &outElapsed)`, `Restart(&timer, &outElapsed)`.
- **`SPACE(Delta)`:** `Start(&delta)`, `Update(&delta, &outElapsed)`.
- **`SPACE(Limiter)`:** `Create(target, &limiter)`, `FromHz(hz, &limiter)`, `Begin(&limiter)`, `Remaining(&limiter, &outRemaining)`, `Wait(&limiter)`.

## Calendar

- **Políticas e sistemas:** `GregorianPolicy()`, `SystemPolicy(system, &outPolicy)`, `ReformPolicy(reformId, &outPolicy)` (`CATHOLIC_1582`, `BRITISH_1752`, `RUSSIAN_1918`).
- **Utilidades:** `IsLeapYear(system, year)`, `DaysInMonth(system, year, month, &outDays)`, `DaysInYear(policy, year, &outDays)`, `DayOfYear(policy, date, &outDoy)`, `DayOfYearCumulative(system, date, &outDoy)`.
- **`SPACE(Date)`:**
  - `Compare`, `IsValid`, `ToDayOfYear`, `ToOrdinal`, `FromOrdinal`, `ToJDN`, `FromJDN`, `ToRataDie`, `FromRataDie`, `Difference`.
  - `DayOfWeek(policy, date, &outWeekday)`, `FromNthWeekday(...)`, `FromWeekdayAnchor(...)`.
  - `AddDays`, `AddMonths`, `AddYears`, `ToIsoWeek`, `FromIsoWeek`.
  - `Quarter(policy, date, &outQuarter)`, `DaysRemainingInYear(policy, date, &outDays)`, `IsEndOfMonth(policy, date)`, `IsWeekend(policy, date)`, `NextBusinessDay(policy, date, &outDate)`, `AddBusinessDays(policy, date, businessDays, &outDate)`, `BusinessDaysBetween(policy, start, end, &outDays)`.

## Civil

`MODULE_TYPE(Chrono, CivilStamp)` combina `Date date` e `TimeOfDay time` (`hour`, `minute`, `second`, `millisecond`). `MODULE_TYPE(Chrono, CivilOffset)` associa um `CivilStamp local`, um `UtcOffset offset` e o indicador `daylightSavings`.

- `IsValid(policy, value)`, `Compare(left, right)`, `AddSpan(policy, value, span, &outResult)`, `Difference(policy, left, right, &outResult)`.
- `ToInstantUTC(value, &outInstant)`, `FromInstantUTC(instant, &outValue)`.
- `StartOfDay(date)`, `Midday(date)`, `EndOfDay(date)`, `TruncateToHour(value)`, `TruncateToMinute(value)`, `TruncateToSecond(value)`.
- **`SPACE(Time)`:** `IsValid`, `Compare`, `ToMilliseconds`, `FromMilliseconds`, `AddSpan(time, span, &outTime, &outDayCarry)`.

## Timezone

- **Conversões UTC e entre fusos:** `ToUTC(civilOffset, &instant)`, `FromUTC(instant, offset, &civilOffset)`, `FromUTCWithRule(instant, &rule, &civilOffset)`, `FromUTCWithZone(instant, zoneId, &civilOffset)`, `ConvertRule(civilOffset, &targetRule, &outConverted)`, `ConvertZone(civilOffset, targetZone, &outConverted)`.
- **Fuso do SO:** `SystemOffsetAt(instant, &outOffset, &outIsDst)`, `SystemNow(&outCivilOffset)`.
- **`SPACE(Offset)`:** `IsValid(offset)` (`[-1439, +1439]` minutos), `FromHoursMinutes(hours, minutes, &outOffset)`.
- **`SPACE(Zone)`:**
  - `DataVersion()`, `FromId(zoneId, &outRule)`, `FromName(ianaName, &outRule)`.
  - `IsRuleValid(&rule)`, `OffsetAt(&rule, instant, &outOffset, &outIsDst)`, `NextTransition(&rule, instant, &outTransition, &outNewOffset)`, `PreviousTransition(&rule, instant, &outTransition, &outNewOffset)`, `IsDstAtLocal(&rule, local, &outIsDst)`.
  - `ClassifyLocal(&rule, local, &outStatus, &outEarlier, &outLater)`, `ResolveLocal(&rule, local, &outResolved)`, `ResolveLocalWithPolicy(&rule, local, policy, &outResolved)`.
- **`SPACE(Country)`:** `NameFromCode(code, &outName)`, `ZoneCount(code, &outCount)`, `ZoneIdAt(code, index, &outZoneId)`, `ZoneRuleAt(code, index, &outRule)`.

### Atualização da base `tzdata`

~~~sh
python3 .github/scripts/generate-tzdata.py
~~~

## Format

- **Capacidades (`BufferCapacity`):** `ISO_DATE_BUFFER_SIZE` (`16`), `ISO_TIME_BUFFER_SIZE` (`16`), `ISO_WEEK_DATE_BUFFER_SIZE` (`16`), `ISO_CIVIL_BUFFER_SIZE` (`32`), `ISO_CIVIL_OFFSET_BUFFER_SIZE` (`40`), `ISO_INSTANT_BUFFER_SIZE` (`32`), `ISO_SPAN_BUFFER_SIZE` (`48`), `RFC3339_BUFFER_SIZE` (`40`), `RFC2822_BUFFER_SIZE` (`40`), `HUMAN_SPAN_BUFFER_SIZE` (`64`).
- **`VARIANT(ToISO)` / `VARIANT(FromISO)`:** `Date`, `TimeOfDay`, `DateIsoWeek`, `CivilStamp`, `CivilOffset`, `Instant`, `Span`.
- **Formatos RFC:** `ToRFC3339`, `FromRFC3339`, `ToRFC2822`.
- **Padrões e span legível:**
  - `Pattern(buffer, capacity, &outWritten, pattern, civilOffset)` com suporte a `%Y`, `%y`, `%m`, `%d`, `%j`, `%u`, `%a`, `%b`, `%H`, `%I`, `%p`, `%M`, `%S`, `%f`, `%z`, `%:z`, `%Z`, `%%` (aceita `buffer == NULL, capacity == 0` para cálculo de tamanho).
  - `ToHumanSpan(span, buffer, capacity)` (ex.: `"0ns"`, `"-14ms 200us"`, `"1d 1h 1m 1s 250ms"`).

---

## Teste de integração e portabilidade

- Suíte de testes: `Codespace/Tests/Chrono/Chrono.c`.
- `Chrono/Impl/Common.inc` define `_POSIX_C_SOURCE 200809L` quando ausente e centraliza a detecção de plataforma em `Detect.h` (`OS_WINDOWS`, `OS_MAC`, POSIX), sem dependência de `<stdio.h>` nem `-lm`.
