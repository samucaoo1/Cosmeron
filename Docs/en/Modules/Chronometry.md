# Chronometry

The **Chronometry** module provides time measurement primitives, signed durations, monotonic and wall-clock time sources, timers, historical and modern calendars, epoch conversions (including TAI $\leftrightarrow$ UTC with leap seconds), IANA timezones with daylight saving time (DST) rules, a country-to-timezone catalog, and `<stdio.h>`-free ISO 8601 formatting and parsing.

## Headers

~~~c
#include "Cosmeron/Modules/Chronometry/Chronometry.h"
~~~

The aggregate header includes all module subpackages:

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

# Overview

| Subpackage | Primary POD Types | Responsibility |
| --- | --- | --- |
| `Duration` | `TDuration` | Signed nanosecond intervals (`int64_t`) with overflow-checked arithmetic. |
| `Clock` | `TClockTimePoint` | OS monotonic clock readings and `Sleep`. |
| `Timer` | `TTimer`, `TDelta`, `TFrameLimiter` | Stopwatch timing, frame delta tracking, and FPS limiting. |
| `Instant` | `TInstant` | Absolute UTC instants in seconds + nanoseconds (`CLOCK_REALTIME` / `FILETIME`). |
| `Calendar` | `TDate`, `TIsoWeekDate`, `TCalendarPolicy` | Gregorian, Julian, and historical reform calendars; date arithmetic and ISO 8601 week dates. |
| `Timestamp` | `TTimeOfDay`, `TTimestamp` | Civil date + time (millisecond precision) governed by a `CalendarPolicy`. |
| `Epoch` | — | JDN, Rata Die, Unix, and TAI $\leftrightarrow$ UTC conversions (27 leap seconds). |
| `Timezone` | `TUtcOffset`, `TOffsetTimestamp`, `TDstTransitionRule`, `TTimezoneRule` | IANA database (`tzdata 2026f`, 344 zones, 253 links, 249 countries), DST disambiguation, and OS local offset. |
| `Format` | — | Strict freestanding ISO 8601 formatting and parsing (`ToISO` / `FromISO`). |

All exposed data types are **pure POD** (`STRUCT`) with zero heap allocation and no embedded `FunctionTable` pointers. Package `FunctionTable` emission can be disabled at compile time via `CHRONOMETRY_DISABLE_FUNCTION_TABLE` (module-wide) or `CHRONOMETRY_<SUBPACKAGE>_DISABLE_FUNCTION_TABLE` (per subpackage).

---

# API Reference

## Duration

`MODULE_TYPE(Chronometry, Duration)` stores `int64_t nanoseconds`.

- **Constants (`Unit`):** `NANOSECONDS_PER_MICROSECOND`, `NANOSECONDS_PER_MILLISECOND`, `NANOSECONDS_PER_SECOND`, `MICROSECONDS_PER_MILLISECOND`, `MICROSECONDS_PER_SECOND`, `MILLISECONDS_PER_SECOND`, `SECONDS_PER_MINUTE`, `MINUTES_PER_HOUR`, `HOURS_PER_DAY`, `MINUTES_PER_DAY`, `SECONDS_PER_HOUR`, `SECONDS_PER_DAY`.
- **`VARIANT(From, ...)`:** `FromNanoseconds` (infallible), `FromMicroseconds`, `FromMilliseconds`, `FromSeconds`, `FromMinutes`, `FromHours`, `FromDays` (return `OPSTATUS`, reporting `ARITHMETIC_OVERFLOW`).
- **`VARIANT(To, ...)`:** `ToNanoseconds`, `ToMicroseconds`, `ToMilliseconds`, `ToSeconds`.
- **Predicates and arithmetic:** `IsZero`, `IsNegative`, `Add`, `Subtract`, `Compare`, `Absolute`, `Negate`, `Scale`.

~~~c
MODULE_TYPE(Chronometry, Duration) delay = {0};
OPSTATUS status = MODULE_FUNC(Chronometry, Duration, FromMilliseconds)(16, &delay);
~~~

## Clock and Timer

- **`Clock`:**
  - `Now(&outTimePoint)`: reads the monotonic clock (`CLOCK_MONOTONIC` / `QueryPerformanceCounter` / `mach_absolute_time`).
  - `DurationBetween(start, end, &outDuration)`: computes the signed duration between two `ClockTimePoint`s.
  - `Sleep(duration)`: sleeps for a non-negative duration (`nanosleep` retrying on `EINTR` / Windows `Sleep`).
- **`Timer`:**
  - `Start(&timer)`, `Elapsed(&timer, &outElapsed)`, `Restart(&timer, &outElapsed)`.
  - `SPACE(Delta)`: `Delta_Start(&delta)`, `Delta_Update(&delta, &outElapsed)`.
  - `SPACE(FrameLimiter)`: `Create(target, &limiter)`, `FromFPS(fps, &limiter)`, `Begin(&limiter)`, `Remaining(&limiter, &outRemaining)`, `Wait(&limiter)`. `Remaining` and `Wait` return `INVALID_ARGUMENT` if `Begin` has not been called yet.

## Instant

`MODULE_TYPE(Chronometry, Instant)` stores `int64_t seconds` and `uint32_t nanoseconds` (`0 <= nanoseconds < 10^9`).

- `Now(&outInstant)`: reads the OS UTC wall-clock (`CLOCK_REALTIME` / `GetSystemTimeAsFileTime`).
- `IsValid(instant)`, `Compare(left, right)`, `AddDuration(instant, duration, &outResult)`, `DurationBetween(start, end, &outResult)`.

## Calendar

- **Policies and systems:** `GregorianPolicy()`, `SystemPolicy(system, &outPolicy)`, `ReformPolicy(reformId, &outPolicy)` (`CATHOLIC_1582`, `BRITISH_1752`, `RUSSIAN_1918`).
- **Utilities:** `IsLeapYear(system, year)`, `DaysInMonth(system, year, month, &outDays)`, `DayOfYear(policy, date, &outDoy)`, `DayOfYearCumulative(system, date, &outDoy)`.
- **`SPACE(Date)`:**
  - `Compare`, `IsValid`, `ToDayOfYear`, `ToOrdinal`, `FromOrdinal`, `Difference`.
  - `DayOfWeek(policy, date, &outWeekday)` (`MONDAY = 1` .. `SUNDAY = 7`).
  - `FromNthWeekday(system, year, month, occurrence, weekday, &outDate)`.
  - `FromWeekdayAnchor(system, year, month, anchorKind, anchorDay, weekday, &outDate)` (`WEEKDAY_EXACT`, `WEEKDAY_ON_OR_AFTER`, `WEEKDAY_ON_OR_BEFORE`, `WEEKDAY_LAST`).
  - `AddDays`, `AddMonths`, `AddYears` (clamping end-of-month days across shorter months and leap years).
  - `ToIsoWeek(policy, date, &outIsoWeek)` and `FromIsoWeek(policy, isoWeek, &outDate)`.

## Timestamp

`MODULE_TYPE(Chronometry, Timestamp)` pairs `Date date` and `TimeOfDay time` (`hour`, `minute`, `second`, `millisecond`).

- `IsValid(policy, value)`, `Compare(left, right)`, `AddDuration(policy, value, duration, &outResult)`, `Difference(policy, left, right, &outResult)`.
- `SPACE(Time)`: `Time_IsValid`, `Time_Compare`.

## Epoch

Organized into `VARIANT(...)` families:

- **`VARIANT(DateTo)`:** `DateToJulianDayNumber(policy, date, &jdn)`, `DateToRataDie(date, &rataDie)`.
- **`VARIANT(DateFrom)`:** `DateFromJulianDayNumber(policy, jdn, &date)`, `DateFromRataDie(rataDie, &date)`.
- **`VARIANT(TimestampTo)`:** `TimestampToUnix(timestamp, &instant)`.
- **`VARIANT(TimestampFrom)`:** `TimestampFromUnix(instant, &timestamp)`.
- **`VARIANT(TAI)`:** `TAIMinusUtcSeconds(utc, &outSeconds)`, `TAIFromUtc(utc, &outTai)`, `TAIToUtc(tai, &outUtc)` (backed by the official 27-entry `tzdata` leap-second table, valid from `1972-01-01T00:00:00Z` onward).

## Timezone

- **UTC conversions:** `ToUTC(offsetTimestamp, &instant)`, `FromUTC(instant, offset, &offsetTimestamp)`, `FromUTCWithRule(instant, &rule, &offsetTimestamp)`, `FromUTCWithZone(instant, zoneId, &offsetTimestamp)`.
- **OS local timezone:** `SystemOffsetAt(instant, &outOffset, &outIsDst)`, `SystemNow(&outOffsetTimestamp)`.
- **`SPACE(Offset)`:** `IsValid(offset)` (`[-1439, +1439]` minutes), `FromHoursMinutes(hours, minutes, &outOffset)`.
- **`SPACE(Zone)`:**
  - `DataVersion()` (returns `"2026f"`).
  - `FromId(zoneId, &outRule)`, `FromName(ianaName, &outRule)` (binary search across 344 canonical zones and 253 `backward` aliases).
  - `IsRuleValid(&rule)`, `IsDstAtLocal(&rule, local, &outIsDst)`.
  - `ClassifyLocal(&rule, local, &outStatus, &outEarlier, &outLater)` (`LOCAL_TIME_UNIQUE`, `LOCAL_TIME_AMBIGUOUS`, `LOCAL_TIME_NONEXISTENT`).
  - `ResolveLocal(&rule, local, &outResolved)` and `ResolveLocalWithPolicy(&rule, local, policy, &outResolved)` (`DISAMBIGUATE_COMPATIBLE`, `DISAMBIGUATE_EARLIER`, `DISAMBIGUATE_LATER`, `DISAMBIGUATE_REJECT`).
- **`SPACE(Country)`:** `NameFromCode(code, &outName)`, `ZoneCount(code, &outCount)`, `ZoneIdAt(code, index, &outZoneId)`, `ZoneRuleAt(code, index, &outRule)`.

### Regenerating `tzdata` tables

`IanaZones.inc` and `LeapSeconds.inc` are generated from `Codespace/Cosmeron/Asset/TimeZone/` via:

~~~sh
python3 .github/scripts/generate-tzdata.py
~~~

## Format

- **Capacities (`BufferCapacity`):** `ISO_DATE_BUFFER_SIZE` (`16`), `ISO_TIMESTAMP_BUFFER_SIZE` (`32`), `ISO_OFFSET_TIMESTAMP_BUFFER_SIZE` (`40`), `ISO_INSTANT_BUFFER_SIZE` (`32`), `ISO_DURATION_BUFFER_SIZE` (`48`).
- **`VARIANT(ToISO)`:** `ToISODate`, `ToISOTimestamp`, `ToISOOffsetTimestamp`, `ToISOInstant`, `ToISODuration`.
- **`VARIANT(FromISO)`:** `FromISODate`, `FromISOTimestamp`, `FromISOOffsetTimestamp`, `FromISOInstant`, `FromISODuration`.
- **ISO 8601 rules:** years in `[0000, 9999]` are formatted without a `+` sign; years outside that range emit an explicit sign (`-0044-03-15`, `+10000-01-01`). Parsing is strict (rejects whitespace and unpadded fields) and does not depend on `<stdio.h>`.

---

# Portability

- `Chronometry/Impl/Common.inc` defines `_POSIX_C_SOURCE 200809L` when not already set, allowing clean compilation under strict `-std=c11` on POSIX hosts.
- All platform detection uses `Detect.h` (`OS_WINDOWS`, `OS_MAC`, POSIX).

## Related Modules

- [Module](../Modules/Module.md) — declarative DSL (`MODULE`, `PACKAGE`, `VARIANT`, `STRUCT`, `ENUM_DEF_DIRECT`).
- [Condition](../Modules/Condition.md) — `OPSTATUS` return contract.
- [Algorithm](../Modules/Algorithm.md) — `CMPOUT` comparison contract.
