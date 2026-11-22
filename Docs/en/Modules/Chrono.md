# Chrono

The **Chrono** module provides time measurement primitives, signed spans and intervals, monotonic and wall-clock time sources, timers, historical and modern calendars, physical time-scale conversions (Unix and TAI $\leftrightarrow$ UTC with leap seconds), IANA timezones with daylight saving time (DST) rules, a country-to-timezone catalog, and `<stdio.h>`-free ISO 8601, RFC 3339, RFC 2822, custom pattern, and human-readable span formatting/parsing.

## Headers

~~~c
#include "Cosmeron/Modules/Chrono/Chrono.h"
~~~

The aggregate header includes all 6 module subpackages:

- `Modules/Chrono/Math/Math.h`
- `Modules/Chrono/Clock/Clock.h`
- `Modules/Chrono/Calendar/Calendar.h`
- `Modules/Chrono/Civil/Civil.h`
- `Modules/Chrono/Timezone/Timezone.h`
- `Modules/Chrono/Format/Format.h`

---

# Overview

| Subpackage | Primary POD Types (`TChrono_*`) | Responsibility |
| --- | --- | --- |
| `Math` | `Span`, `Interval` | Signed nanosecond durations (`int64_t`), overflow-checked/saturating arithmetic, Hz, rounding, and half-open intervals. |
| `Clock` | `ClockPoint`, `Instant`, `Timer`, `TimerDelta`, `TimerLimiter` | OS monotonic clock, `Sleep`/`SleepUntil`, resolution, pausable stopwatch, delta tracking, rate limiter, UTC wall-clock `Instant`, Unix millis/seconds, and TAI $\leftrightarrow$ UTC. |
| `Calendar` | `Date`, `DateIsoWeek`, `CalendarReform`, `CalendarPolicy` | Gregorian, Julian, and historical reform calendars; JDN/Rata Die, ISO 8601 weeks, quarters, and O(1) business-day math. |
| `Civil` | `TimeOfDay`, `CivilStamp`, `UtcOffset`, `CivilOffset` | Civil date + time (millisecond precision), day anchors, truncation, time-of-day carry arithmetic, and UTC Instant $\leftrightarrow$ Civil conversion. |
| `Timezone` | `TimezoneDstRule`, `TimezoneRule` | IANA database (`tzdata 2026f`, 344 zones, 253 links, 249 countries), DST disambiguation, cross-zone conversion, transition lookup, and OS local offset. |
| `Format` | — | Freestanding ISO 8601, RFC 3339, RFC 2822, pattern formatter (`Pattern`), and human span formatter (`ToHumanSpan`). |

All exposed data types are **pure POD** (`STRUCT`) with zero heap allocation and no embedded `FunctionTable` pointers. Package `FunctionTable` emission can be disabled at compile time via `CHRONO_DISABLE_FUNCTION_TABLE` (module-wide) or `CHRONO_<SUBPACKAGE>_DISABLE_FUNCTION_TABLE` (per subpackage).

---

# API Reference

## Math

`MODULE_TYPE(Chrono, Span)` stores `int64_t nanoseconds`. `MODULE_TYPE(Chrono, Interval)` represents a half-open span interval `[start, end)`.

- **Constants (`Unit`):** `NANOSECONDS_PER_MICROSECOND`, `NANOSECONDS_PER_MILLISECOND`, `NANOSECONDS_PER_SECOND`, `MICROSECONDS_PER_MILLISECOND`, `MICROSECONDS_PER_SECOND`, `MILLISECONDS_PER_SECOND`, `SECONDS_PER_MINUTE`, `MINUTES_PER_HOUR`, `HOURS_PER_DAY`, `MINUTES_PER_DAY`, `SECONDS_PER_HOUR`, `SECONDS_PER_DAY`.
- **Granularity (`Granularity`):** `GRANULARITY_NANOSECOND` through `GRANULARITY_DAY`.
- **`VARIANT(From, ...)`:** `FromNanoseconds` (infallible), `FromMicroseconds`, `FromMilliseconds`, `FromSeconds`, `FromMinutes`, `FromHours`, `FromDays`, `FromHz(double hz, &outSpan)`.
- **`VARIANT(To, ...)`:** `ToNanoseconds`, `ToMicroseconds`, `ToMilliseconds`, `ToSeconds`, `ToHz(span, &outHz)`.
- **Arithmetic & bounds:** `IsZero`, `IsNegative`, `Compare`, `Add`, `Subtract`, `Absolute`, `Negate`, `Scale`, `Divide`, `Remainder`, `Ratio`, `AddSaturating`, `SubtractSaturating`, `Min`, `Max`, `Clamp`, `Truncate`, `Round`.
- **`SPACE(Interval)`:** `IsValid`, `Length`, `Contains`, `Overlaps`, `Intersection`, `Union`.

~~~c
MODULE_TYPE(Chrono, Span) frame = {0};
OPSTATUS status = MODULE_FUNC(Chrono, Math, FromHz)(60.0, &frame);
~~~

## Clock

- **Monotonic clock:** `Now(&outPoint)`, `SpanBetween(start, end, &outSpan)`, `Sleep(span)`, `SleepUntil(targetPoint)`, `Resolution(&outSpan)`.
- **`SPACE(Instant)`:**
  - `Now(&outInstant)`, `IsValid(instant)`, `Compare(left, right)`, `AddSpan(instant, span, &outResult)`, `SpanBetween(start, end, &outResult)`.
  - `ToUnixSeconds(instant)`, `FromUnixSeconds(seconds)`, `ToUnixMillis(instant, &outMillis)`, `FromUnixMillis(millis, &outInstant)`.
  - `TaiMinusUtcSeconds(utc, &outSeconds)`, `ToTAI(utc, &outTai)`, `FromTAI(tai, &outUtc)` (27-entry `tzdata` leap-second table, valid from `1972-01-01T00:00:00Z`).
- **`SPACE(Timer)`:** `Start(&timer)`, `Pause(&timer)`, `Resume(&timer)`, `IsRunning(&timer)`, `Elapsed(&timer, &outElapsed)`, `Restart(&timer, &outElapsed)`.
- **`SPACE(Delta)`:** `Start(&delta)`, `Update(&delta, &outElapsed)`.
- **`SPACE(Limiter)`:** `Create(target, &limiter)`, `FromHz(hz, &limiter)`, `Begin(&limiter)`, `Remaining(&limiter, &outRemaining)`, `Wait(&limiter)`.

## Calendar

- **Policies and systems:** `GregorianPolicy()`, `SystemPolicy(system, &outPolicy)`, `ReformPolicy(reformId, &outPolicy)` (`CATHOLIC_1582`, `BRITISH_1752`, `RUSSIAN_1918`).
- **Utilities:** `IsLeapYear(system, year)`, `DaysInMonth(system, year, month, &outDays)`, `DaysInYear(policy, year, &outDays)`, `DayOfYear(policy, date, &outDoy)`, `DayOfYearCumulative(system, date, &outDoy)`.
- **`SPACE(Date)`:**
  - `Compare`, `IsValid`, `ToDayOfYear`, `ToOrdinal`, `FromOrdinal`, `ToJDN`, `FromJDN`, `ToRataDie`, `FromRataDie`, `Difference`.
  - `DayOfWeek(policy, date, &outWeekday)`, `FromNthWeekday(...)`, `FromWeekdayAnchor(...)`.
  - `AddDays`, `AddMonths`, `AddYears`, `ToIsoWeek`, `FromIsoWeek`.
  - `Quarter(policy, date, &outQuarter)`, `DaysRemainingInYear(policy, date, &outDays)`, `IsEndOfMonth(policy, date)`, `IsWeekend(policy, date)`, `NextBusinessDay(policy, date, &outDate)`, `AddBusinessDays(policy, date, businessDays, &outDate)`, `BusinessDaysBetween(policy, start, end, &outDays)`.

## Civil

`MODULE_TYPE(Chrono, CivilStamp)` pairs `Date date` and `TimeOfDay time` (`hour`, `minute`, `second`, `millisecond`). `MODULE_TYPE(Chrono, CivilOffset)` pairs a `CivilStamp local`, a `UtcOffset offset`, and a `daylightSavings` flag.

- `IsValid(policy, value)`, `Compare(left, right)`, `AddSpan(policy, value, span, &outResult)`, `Difference(policy, left, right, &outResult)`.
- `ToInstantUTC(value, &outInstant)`, `FromInstantUTC(instant, &outValue)`.
- `StartOfDay(date)`, `Midday(date)`, `EndOfDay(date)`, `TruncateToHour(value)`, `TruncateToMinute(value)`, `TruncateToSecond(value)`.
- **`SPACE(Time)`:** `IsValid`, `Compare`, `ToMilliseconds`, `FromMilliseconds`, `AddSpan(time, span, &outTime, &outDayCarry)`.

## Timezone

- **UTC & cross-zone conversions:** `ToUTC(civilOffset, &instant)`, `FromUTC(instant, offset, &civilOffset)`, `FromUTCWithRule(instant, &rule, &civilOffset)`, `FromUTCWithZone(instant, zoneId, &civilOffset)`, `ConvertRule(civilOffset, &targetRule, &outConverted)`, `ConvertZone(civilOffset, targetZone, &outConverted)`.
- **OS local timezone:** `SystemOffsetAt(instant, &outOffset, &outIsDst)`, `SystemNow(&outCivilOffset)`.
- **`SPACE(Offset)`:** `IsValid(offset)` (`[-1439, +1439]` minutes), `FromHoursMinutes(hours, minutes, &outOffset)`.
- **`SPACE(Zone)`:**
  - `DataVersion()`, `FromId(zoneId, &outRule)`, `FromName(ianaName, &outRule)` (344 canonical zones, 253 aliases).
  - `IsRuleValid(&rule)`, `OffsetAt(&rule, instant, &outOffset, &outIsDst)`, `NextTransition(&rule, instant, &outTransition, &outNewOffset)`, `PreviousTransition(&rule, instant, &outTransition, &outNewOffset)`, `IsDstAtLocal(&rule, local, &outIsDst)`.
  - `ClassifyLocal(&rule, local, &outStatus, &outEarlier, &outLater)`, `ResolveLocal(&rule, local, &outResolved)`, `ResolveLocalWithPolicy(&rule, local, policy, &outResolved)`.
- **`SPACE(Country)`:** `NameFromCode(code, &outName)`, `ZoneCount(code, &outCount)`, `ZoneIdAt(code, index, &outZoneId)`, `ZoneRuleAt(code, index, &outRule)`.

### Regenerating `tzdata` tables

~~~sh
python3 .github/scripts/generate-tzdata.py
~~~

## Format

- **Capacities (`BufferCapacity`):** `ISO_DATE_BUFFER_SIZE` (`16`), `ISO_TIME_BUFFER_SIZE` (`16`), `ISO_WEEK_DATE_BUFFER_SIZE` (`16`), `ISO_CIVIL_BUFFER_SIZE` (`32`), `ISO_CIVIL_OFFSET_BUFFER_SIZE` (`40`), `ISO_INSTANT_BUFFER_SIZE` (`32`), `ISO_SPAN_BUFFER_SIZE` (`48`), `RFC3339_BUFFER_SIZE` (`40`), `RFC2822_BUFFER_SIZE` (`40`), `HUMAN_SPAN_BUFFER_SIZE` (`64`).
- **`VARIANT(ToISO)` / `VARIANT(FromISO)`:** `Date`, `TimeOfDay`, `DateIsoWeek`, `CivilStamp`, `CivilOffset`, `Instant`, `Span`.
- **RFC formats:** `ToRFC3339`, `FromRFC3339`, `ToRFC2822`.
- **Custom pattern & human spans:**
  - `Pattern(buffer, capacity, &outWritten, pattern, civilOffset)` supporting `%Y`, `%y`, `%m`, `%d`, `%j`, `%u`, `%a`, `%b`, `%H`, `%I`, `%p`, `%M`, `%S`, `%f`, `%z`, `%:z`, `%Z`, `%%` (supports `buffer == NULL, capacity == 0` size measurement).
  - `ToHumanSpan(span, buffer, capacity)` (e.g., `"0ns"`, `"-14ms 200us"`, `"1d 1h 1m 1s 250ms"`).

---

## Integration Test & Portability

- Integration test: `Codespace/Tests/Chrono/Chrono.c`.
- `Chrono/Impl/Common.inc` defines `_POSIX_C_SOURCE 200809L` when unset and routes all platform detection through `Detect.h` (`OS_WINDOWS`, `OS_MAC`, POSIX), with zero `<stdio.h>` or `-lm` dependency.
