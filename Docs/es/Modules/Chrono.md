# Chrono

El módulo **Chrono** proporciona primitivas de medición de tiempo, spans e intervalos con signo, relojes monotónico y de pared, temporizadores, calendarios históricos y modernos, conversiones de escala temporal (Unix y TAI $\leftrightarrow$ UTC con segundos intercalares), zonas horarias IANA con reglas de horario de verano (DST), catálogo país $\leftrightarrow$ zona y formateo/parsing freestanding (sin `<stdio.h>`) para ISO 8601, RFC 3339, RFC 2822, patrones personalizados y spans legibles.

## Encabezados

~~~c
#include "Cosmeron/Modules/Chrono/Chrono.h"
~~~

El encabezado agregado incluye los 6 subpaquetes del módulo:

- `Modules/Chrono/Math/Math.h`
- `Modules/Chrono/Clock/Clock.h`
- `Modules/Chrono/Calendar/Calendar.h`
- `Modules/Chrono/Civil/Civil.h`
- `Modules/Chrono/Timezone/Timezone.h`
- `Modules/Chrono/Format/Format.h`

---

# Visión general

| Subpaquete | Tipos POD principales (`TChrono_*`) | Responsabilidad |
| --- | --- | --- |
| `Math` | `Span`, `Interval` | Duraciones con signo en nanosegundos (`int64_t`), aritmética segura/saturada, Hz, redondeo e intervalos semiabiertos. |
| `Clock` | `ClockPoint`, `Instant`, `Timer`, `TimerDelta`, `TimerLimiter` | Reloj monotónico del SO, `Sleep`/`SleepUntil`, resolución, cronómetro pausable, delta, limitador de tasa, `Instant` UTC, Unix y TAI $\leftrightarrow$ UTC. |
| `Calendar` | `Date`, `DateIsoWeek`, `CalendarReform`, `CalendarPolicy` | Calendarios Gregoriano, Juliano y reformas históricas; JDN/Rata Die, semanas ISO 8601, trimestres y días hábiles en $O(1)$. |
| `Civil` | `TimeOfDay`, `CivilStamp`, `UtcOffset`, `CivilOffset` | Fecha + hora civil (precisión de milisegundos), anclas del día, truncamiento, aritmética de `TimeOfDay` con acarreo y conversión UTC `Instant` $\leftrightarrow$ `CivilStamp`. |
| `Timezone` | `TimezoneDstRule`, `TimezoneRule` | Base IANA (`tzdata 2026f`, 344 zonas, 253 enlaces, 249 países), desambiguación DST, conversión directa entre zonas, consulta de transiciones y zona del SO. |
| `Format` | — | Formateo/parsing freestanding ISO 8601, RFC 3339, RFC 2822, formateador por patrón (`Pattern`) y `ToHumanSpan`. |

Todos los tipos expuestos son **POD puros** (`STRUCT`), sin asignación dinámica ni punteros de `FunctionTable` incrustados. La emisión de `FunctionTable` puede deshabilitarse mediante `CHRONO_DISABLE_FUNCTION_TABLE` (módulo completo) o `CHRONO_<SUBPAQUETE>_DISABLE_FUNCTION_TABLE` (por subpaquete).

---

# Referencia de la API

## Math

`MODULE_TYPE(Chrono, Span)` almacena `int64_t nanoseconds`. `MODULE_TYPE(Chrono, Interval)` representa un intervalo semiabierto `[start, end)`.

- **Constantes (`Unit`):** `NANOSECONDS_PER_MICROSECOND` .. `SECONDS_PER_DAY`.
- **Granularidad (`Granularity`):** `GRANULARITY_NANOSECOND` a `GRANULARITY_DAY`.
- **`VARIANT(From, ...)`:** `FromNanoseconds`, `FromMicroseconds`, `FromMilliseconds`, `FromSeconds`, `FromMinutes`, `FromHours`, `FromDays`, `FromHz(double hz, &outSpan)`.
- **`VARIANT(To, ...)`:** `ToNanoseconds`, `ToMicroseconds`, `ToMilliseconds`, `ToSeconds`, `ToHz(span, &outHz)`.
- **Aritmética y límites:** `IsZero`, `IsNegative`, `Compare`, `Add`, `Subtract`, `Absolute`, `Negate`, `Scale`, `Divide`, `Remainder`, `Ratio`, `AddSaturating`, `SubtractSaturating`, `Min`, `Max`, `Clamp`, `Truncate`, `Round`.
- **`SPACE(Interval)`:** `IsValid`, `Length`, `Contains`, `Overlaps`, `Intersection`, `Union`.

## Clock

- **Reloj monotónico:** `Now(&outPoint)`, `SpanBetween(start, end, &outSpan)`, `Sleep(span)`, `SleepUntil(targetPoint)`, `Resolution(&outSpan)`.
- **`SPACE(Instant)`:**
  - `Now(&outInstant)`, `IsValid(instant)`, `Compare(left, right)`, `AddSpan(instant, span, &outResult)`, `SpanBetween(start, end, &outResult)`.
  - `ToUnixSeconds(instant)`, `FromUnixSeconds(seconds)`, `ToUnixMillis(instant, &outMillis)`, `FromUnixMillis(millis, &outInstant)`.
  - `TaiMinusUtcSeconds(utc, &outSeconds)`, `ToTAI(utc, &outTai)`, `FromTAI(tai, &outUtc)` (27 segundos intercalares de `tzdata`, válida desde `1972-01-01T00:00:00Z`).
- **`SPACE(Timer)`:** `Start(&timer)`, `Pause(&timer)`, `Resume(&timer)`, `IsRunning(&timer)`, `Elapsed(&timer, &outElapsed)`, `Restart(&timer, &outElapsed)`.
- **`SPACE(Delta)`:** `Start(&delta)`, `Update(&delta, &outElapsed)`.
- **`SPACE(Limiter)`:** `Create(target, &limiter)`, `FromHz(hz, &limiter)`, `Begin(&limiter)`, `Remaining(&limiter, &outRemaining)`, `Wait(&limiter)`.

## Calendar

- **Políticas y sistemas:** `GregorianPolicy()`, `SystemPolicy(system, &outPolicy)`, `ReformPolicy(reformId, &outPolicy)` (`CATHOLIC_1582`, `BRITISH_1752`, `RUSSIAN_1918`).
- **Utilidades:** `IsLeapYear(system, year)`, `DaysInMonth(system, year, month, &outDays)`, `DaysInYear(policy, year, &outDays)`, `DayOfYear(policy, date, &outDoy)`, `DayOfYearCumulative(system, date, &outDoy)`.
- **`SPACE(Date)`:**
  - `Compare`, `IsValid`, `ToDayOfYear`, `ToOrdinal`, `FromOrdinal`, `ToJDN`, `FromJDN`, `ToRataDie`, `FromRataDie`, `Difference`.
  - `DayOfWeek(policy, date, &outWeekday)`, `FromNthWeekday(...)`, `FromWeekdayAnchor(...)`.
  - `AddDays`, `AddMonths`, `AddYears`, `ToIsoWeek`, `FromIsoWeek`.
  - `Quarter(policy, date, &outQuarter)`, `DaysRemainingInYear(policy, date, &outDays)`, `IsEndOfMonth(policy, date)`, `IsWeekend(policy, date)`, `NextBusinessDay(policy, date, &outDate)`, `AddBusinessDays(policy, date, businessDays, &outDate)`, `BusinessDaysBetween(policy, start, end, &outDays)`.

## Civil

`MODULE_TYPE(Chrono, CivilStamp)` combina `Date date` y `TimeOfDay time` (`hour`, `minute`, `second`, `millisecond`). `MODULE_TYPE(Chrono, CivilOffset)` asocia un `CivilStamp local`, un `UtcOffset offset` y el indicador `daylightSavings`.

- `IsValid(policy, value)`, `Compare(left, right)`, `AddSpan(policy, value, span, &outResult)`, `Difference(policy, left, right, &outResult)`.
- `ToInstantUTC(value, &outInstant)`, `FromInstantUTC(instant, &outValue)`.
- `StartOfDay(date)`, `Midday(date)`, `EndOfDay(date)`, `TruncateToHour(value)`, `TruncateToMinute(value)`, `TruncateToSecond(value)`.
- **`SPACE(Time)`:** `IsValid`, `Compare`, `ToMilliseconds`, `FromMilliseconds`, `AddSpan(time, span, &outTime, &outDayCarry)`.

## Timezone

- **Conversiones UTC y entre zonas:** `ToUTC(civilOffset, &instant)`, `FromUTC(instant, offset, &civilOffset)`, `FromUTCWithRule(instant, &rule, &civilOffset)`, `FromUTCWithZone(instant, zoneId, &civilOffset)`, `ConvertRule(civilOffset, &targetRule, &outConverted)`, `ConvertZone(civilOffset, targetZone, &outConverted)`.
- **Zona del SO:** `SystemOffsetAt(instant, &outOffset, &outIsDst)`, `SystemNow(&outCivilOffset)`.
- **`SPACE(Offset)`:** `IsValid(offset)` (`[-1439, +1439]` minutos), `FromHoursMinutes(hours, minutes, &outOffset)`.
- **`SPACE(Zone)`:**
  - `DataVersion()`, `FromId(zoneId, &outRule)`, `FromName(ianaName, &outRule)`.
  - `IsRuleValid(&rule)`, `OffsetAt(&rule, instant, &outOffset, &outIsDst)`, `NextTransition(&rule, instant, &outTransition, &outNewOffset)`, `PreviousTransition(&rule, instant, &outTransition, &outNewOffset)`, `IsDstAtLocal(&rule, local, &outIsDst)`.
  - `ClassifyLocal(&rule, local, &outStatus, &outEarlier, &outLater)`, `ResolveLocal(&rule, local, &outResolved)`, `ResolveLocalWithPolicy(&rule, local, policy, &outResolved)`.
- **`SPACE(Country)`:** `NameFromCode(code, &outName)`, `ZoneCount(code, &outCount)`, `ZoneIdAt(code, index, &outZoneId)`, `ZoneRuleAt(code, index, &outRule)`.

### Actualización de tablas `tzdata`

~~~sh
python3 .github/scripts/generate-tzdata.py
~~~

## Format

- **Capacidades (`BufferCapacity`):** `ISO_DATE_BUFFER_SIZE` (`16`), `ISO_TIME_BUFFER_SIZE` (`16`), `ISO_WEEK_DATE_BUFFER_SIZE` (`16`), `ISO_CIVIL_BUFFER_SIZE` (`32`), `ISO_CIVIL_OFFSET_BUFFER_SIZE` (`40`), `ISO_INSTANT_BUFFER_SIZE` (`32`), `ISO_SPAN_BUFFER_SIZE` (`48`), `RFC3339_BUFFER_SIZE` (`40`), `RFC2822_BUFFER_SIZE` (`40`), `HUMAN_SPAN_BUFFER_SIZE` (`64`).
- **`VARIANT(ToISO)` / `VARIANT(FromISO)`:** `Date`, `TimeOfDay`, `DateIsoWeek`, `CivilStamp`, `CivilOffset`, `Instant`, `Span`.
- **Formatos RFC:** `ToRFC3339`, `FromRFC3339`, `ToRFC2822`.
- **Patrones y span legible:**
  - `Pattern(buffer, capacity, &outWritten, pattern, civilOffset)` con soporte para `%Y`, `%y`, `%m`, `%d`, `%j`, `%u`, `%a`, `%b`, `%H`, `%I`, `%p`, `%M`, `%S`, `%f`, `%z`, `%:z`, `%Z`, `%%` (admite `buffer == NULL, capacity == 0` para medición de tamaño).
  - `ToHumanSpan(span, buffer, capacity)`.

---

## Prueba de integración y portabilidad

- Suite de pruebas: `Codespace/Tests/Chrono/Chrono.c`.
- `Chrono/Impl/Common.inc` define `_POSIX_C_SOURCE 200809L` si no está definido y centraliza la detección de plataforma en `Detect.h` (`OS_WINDOWS`, `OS_MAC`, POSIX), sin dependencia de `<stdio.h>` ni `-lm`.
