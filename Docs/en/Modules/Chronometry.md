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


## Semantics, boundaries and practical use

### Choosing the correct time representation

Chronometry deliberately separates elapsed-time measurement from civil time:

- **ClockTimePoint / Clock:** a monotonic counter intended for measuring elapsed time. Its origin is unspecified; treat the value as opaque, do not serialize it as a calendar timestamp, and do not compare it with an Instant.
- **Instant:** an absolute wall-clock point represented as Unix seconds plus nanoseconds. It is suitable for timestamps and interchange, but the system clock can be adjusted; do not use it as the source for measuring elapsed time.
- **Duration:** a signed interval in nanoseconds. Use it for elapsed time, arithmetic, and delays.
- **Timestamp:** a civil date and time with millisecond precision. It has no timezone or UTC offset by itself. A CalendarPolicy determines how its date is interpreted.
- **OffsetTimestamp:** a local Timestamp paired with a numeric UTC offset and a daylight-saving flag. The offset is data; it is not a named timezone.

Use Clock or Timer for frame timing, timeouts, and performance measurements. Use Instant, Timestamp, and Timezone for wall-clock dates, schedules, and conversion between local time and UTC.

### Calendar policy

A Date is not intrinsically Gregorian or Julian. Pass the same CalendarPolicy through validation, ordinal conversion, arithmetic, and timestamp operations whenever those values belong to a particular calendar system. GregorianPolicy() is the convenient default for modern civil dates. SystemPolicy() selects a pure Gregorian or Julian system; ReformPolicy() models one of the built-in historical transitions and its skipped civil dates.

AddMonths() and AddYears() clamp the day when the destination month is shorter (for example, moving a date on the 31st into a 30-day month). Operations that cross a reform gap or leave the supported range report a status instead of silently inventing a date.

### Timezones and daylight-saving transitions

The timezone catalogue is generated from the repository's bundled IANA-derived data. A TimezoneRule is a compact rule profile: standard UTC offset, DST adjustment, and recurring start/end transition rules. It is not a full historical TZif database. Do not assume that this representation reproduces every historical political change or every past transition for a location.

A local civil time can be:

- LOCAL_TIME_UNIQUE: exactly one UTC instant corresponds to it.
- LOCAL_TIME_AMBIGUOUS: the clock moved backward and the local time occurs twice.
- LOCAL_TIME_NONEXISTENT: the clock moved forward and the local time was skipped.

ClassifyLocal() reports the classification and candidate earlier/later offset timestamps. ResolveLocal() uses the compatible policy: it selects the earlier occurrence for an overlap and the later candidate for a gap. ResolveLocalWithPolicy() makes the choice explicit with DISAMBIGUATE_EARLIER, DISAMBIGUATE_LATER, or DISAMBIGUATE_REJECT; the reject policy returns OUT_OF_RANGE when the local time is not unique. Prefer explicit policy selection when processing user-entered schedules.

FromUTC() with a numeric offset performs a fixed-offset conversion. FromUTCWithRule() and FromUTCWithZone() apply the selected rule profile. SystemOffsetAt() and SystemNow() query the host operating system's local timezone configuration; they do not change that configuration.

### ISO 8601 input and output

The Format package writes into caller-owned buffers and does not allocate memory or depend on <stdio.h>. The capacity constants include the terminating null character. Always pass the actual buffer capacity and check the returned OPSTATUS; insufficient capacity is reported as OUT_OF_RANGE.

- A date is parsed using the Gregorian calendar.
- A timestamp is a local date-time (date + T + time) and has no offset.
- An offset timestamp includes Z or a numeric offset.
- An instant is parsed through an offset timestamp and converted to UTC.
- A duration uses the supported day/time components (D, H, M, S) and optional fractional seconds; calendar months and years are not fixed-length duration units.

Parsing is strict: the entire input must match the expected form. Leading/trailing whitespace, unpadded fields, and trailing characters are rejected. FromISODate() and FromISOTimestamp() use the Gregorian policy; use the Calendar API directly when a historical or Julian calendar is required.

### Error handling and ownership

Operations that can fail return OPSTATUS and write results through output parameters. Check the status before consuming an output value. The module's public value types are POD structures: callers own their storage, and the core time values do not require heap allocation. Strings returned by timezone and country lookup functions refer to bundled static data; callers must not free them.
---

# Portability

- `Chronometry/Impl/Common.inc` defines `_POSIX_C_SOURCE 200809L` when not already set, allowing clean compilation under strict `-std=c11` on POSIX hosts.
- All platform detection uses `Detect.h` (`OS_WINDOWS`, `OS_MAC`, POSIX).

## Related Modules

- [Module](../Modules/Module.md) — declarative DSL (`MODULE`, `PACKAGE`, `VARIANT`, `STRUCT`, `ENUM_DEF_DIRECT`).
- [Condition](../Modules/Condition.md) — `OPSTATUS` return contract.
- [Algorithm](../Modules/Algorithm.md) — `CMPOUT` comparison contract.
