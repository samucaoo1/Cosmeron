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


## Detailed contracts

### Duration

TDuration.nanoseconds is a signed int64_t. Unit constructors validate output pointers and detect multiplication overflow; FromNanoseconds is a direct value constructor. ToMicroseconds, ToMilliseconds and ToSeconds use integer division, discard fractional units and truncate toward zero. They do not round. Absolute and Negate report ARITHMETIC_OVERFLOW for INT64_MIN. Add, Subtract and Scale also detect overflow rather than wrapping.

### Clock and timer lifecycle

Clock.Now returns a monotonic reading in nanoseconds. Its epoch is unspecified; readings are meaningful only for differences within the same clock domain. DurationBetween is signed and returns a negative duration when end precedes start.

Clock.Sleep rejects negative durations with INVALID_ARGUMENT; zero is a successful no-op. The operating system may resume the thread later than requested, so Sleep is not a real-time scheduling guarantee.

Timer.Start establishes the start point. Elapsed is observational and does not change it; Restart returns the elapsed interval and moves the start point to the new reading. Timer.Delta.Start establishes the previous reading, while Delta.Update returns the interval since the previous update and advances that reading.

Use a frame limiter in this order: Create or FromFPS; Begin at the start of each frame; perform the frame's work; then Wait or inspect Remaining. Wait does not automatically begin the next frame. Call Begin again for the next frame. Remaining and Wait return INVALID_ARGUMENT before the first successful Begin. FromFPS computes integer nanoseconds per frame (1,000,000,000 / fps), so rates that do not divide one billion are represented by a truncated interval.

### Instant and epoch conversion

Instant is normalized as seconds plus nanoseconds in the half-open range [0, 1,000,000,000). Negative instants use the same normalized representation; for example, half a second before Unix epoch is {-1, 500000000}.

Instant.Now reads the operating-system wall clock. Unlike the monotonic clock, it can be adjusted by synchronization, administrative changes or virtualization. DurationBetween is arithmetic over the supplied instants and does not guarantee that wall time progressed monotonically.

Epoch.TimestampToUnix interprets Timestamp as a UTC civil timestamp under the Gregorian calendar. Timestamp has no timezone, so do not use this conversion for local wall time unless it has first been converted to UTC. TimestampFromUnix performs the inverse UTC/Gregorian conversion.

DateToJulianDayNumber and DateFromJulianDayNumber require an explicit calendar policy. Rata Die conversion uses the Gregorian date convention exposed by this implementation and has no policy parameter.

TAI conversion uses the bundled leap-second table. TAIMinusUtcSeconds returns OUT_OF_RANGE before the UTC era covered by the table. The table is not fetched or updated at runtime. TAI uses the same seconds/nanoseconds carrier as Instant, but it is a different time scale: never treat a TAI value as a Unix UTC timestamp.

### Calendar policies and date ranges

CalendarPolicy is an explicit interpretation rule, not metadata stored in Date. Keep the same policy for related validation, arithmetic, ordinal and weekday operations. GregorianPolicy returns the Gregorian policy by value. SystemPolicy creates a pure Gregorian or Julian policy; ReformPolicy creates one of the predefined historical reform policies.

Reform policies represent a discontinuity between the last date in the old system and the first date in the new system. Dates in the suppressed interval are invalid under that policy. The model is limited to three predefined transitions; it is not a general country-specific historical calendar database.

AddMonths clamps the original day to the final day of the destination month. AddYears delegates to month arithmetic, so moving a leap day into a non-leap year clamps it to the final day of February. FromNthWeekday supports FIRST_WEEKDAY through FOURTH_WEEKDAY and LAST_WEEKDAY. FromWeekdayAnchor supports exact, on-or-after, on-or-before and last matching weekdays relative to a day anchor.

ISO week dates use Monday as weekday 1 and Sunday as weekday 7. The ISO week-year can differ from the calendar year near New Year. The calendar policy is required when converting between a date and an ISO week date.

### Timezone rule model

UtcOffset.minutesEastOfUtc is signed minutes east of UTC: UTC−03:00 is -180 and UTC+05:30 is 330. The accepted range is -1439 through +1439 minutes. FromHoursMinutes takes signed hour and minute components; use components with matching signs for a negative offset.

TimezoneRule contains a standard offset, a DST adjustment and two recurring transition rules. Each transition rule records its month, weekday anchor, weekday, local hour/minute and time basis (TIME_BASE_WALL, TIME_BASE_STANDARD or TIME_BASE_UTC). This compact model is not a full TZif transition history. It cannot represent every historical offset change, non-recurring political change or arbitrary sequence of transitions.

FromName resolves canonical IANA names and bundled link aliases. FromId resolves the generated compact ZoneId. The returned ianaName points to static bundled storage and must not be freed. Country lookup also returns static strings and bundled zone records. Invalid country codes and indexes return an error status.

FromUTC applies a fixed numeric offset. FromUTCWithRule and FromUTCWithZone apply a rule profile to an absolute instant. SystemOffsetAt asks the host OS for its local offset at the supplied instant; SystemNow combines the current wall-clock instant with the host's local offset. These system functions depend on the host timezone configuration/database, which may differ from the bundled catalogue.

For a nonexistent local time, ClassifyLocal returns candidates on either side of the forward transition. For an ambiguous time, it returns the earlier and later occurrences. For a unique time, both outputs contain the same result. ResolveLocalWithPolicy rejects non-unique local times with DISAMBIGUATE_REJECT; EARLIER and LATER select the corresponding candidate. COMPATIBLE selects the earlier occurrence in an overlap and the later candidate in a gap.

### ISO 8601 contracts

Format is a bounded-buffer formatter/parser. It does not allocate memory or use <stdio.h>. Output functions require a non-null buffer and nonzero capacity; parsing functions require non-null input and output pointers. Capacity must include the terminating null byte. An undersized output buffer returns OUT_OF_RANGE.

- ToISODate and FromISODate use the Gregorian calendar.
- ToISOTimestamp and FromISOTimestamp operate on a local civil timestamp without an offset.
- ToISOOffsetTimestamp and FromISOOffsetTimestamp preserve a numeric UTC offset. The daylight-saving flag is not encoded in ISO text.
- ToISOInstant always formats UTC with Z. FromISOInstant accepts an offset timestamp and converts it to UTC.
- ToISODuration emits a signed ISO day/time duration. The formatter normalizes whole days into hours; the parser accepts day and time components.

Parsing consumes the entire input. Leading/trailing whitespace, incomplete fields, invalid calendar/time values and trailing characters are rejected. ISO civil date fields use the Gregorian calendar regardless of any historical CalendarPolicy used elsewhere in the module. Signed extended years are supported by the date formatter/parser.

## Worked examples

### Measure an interval

~~~c
#include "Cosmeron/Modules/Chronometry/Chronometry.h"

MODULE_TYPE(Chronometry, Timer) timer = {0};
MODULE_TYPE(Chronometry, Duration) elapsed = {0};

if (MODULE_FUNC(Chronometry, Timer, Start)(&timer) != STATUS_CONST(SUCCESS)) {
    return 1;
}

/* Perform the operation to be measured. */

if (MODULE_FUNC(Chronometry, Timer, Elapsed)(&timer, &elapsed) !=
    STATUS_CONST(SUCCESS)) {
    return 1;
}
~~~

### Apply calendar arithmetic

~~~c
MODULE_TYPE(Chronometry, CalendarPolicy) gregorian =
    MODULE_FUNC(Chronometry, Calendar, GregorianPolicy)();
MODULE_TYPE(Chronometry, Date) nextMonth = {0};

OPSTATUS status = MODULE_FUNC(Chronometry, Calendar, Date, AddMonths)(
    &gregorian, (MODULE_TYPE(Chronometry, Date)){2024, 1, 31}, 1, &nextMonth);

/* On success, nextMonth is 2024-02-29. */
~~~

### Resolve a daylight-saving overlap

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

### Format an instant

~~~c
MODULE_TYPE(Chronometry, Instant) instant = {0};
char text[MODULE_CONST(Chronometry, Format, ISO_INSTANT_BUFFER_SIZE)];

OPSTATUS status = MODULE_FUNC(Chronometry, Instant, Now)(&instant);
if (status == STATUS_CONST(SUCCESS)) {
    status = MODULE_FUNC(Chronometry, Format, ToISOInstant)(
        instant, text, sizeof(text));
}
~~~

## Public headers and implementation map

| Subpackage | Public header | Implementation |
| --- | --- | --- |
| Duration | Duration/Duration.h | Duration/Impl/Duration.impl |
| Clock | Clock/Clock.h | Clock/Impl/Clock.impl |
| Timer | Timer/Timer.h | Timer/Impl/Timer.impl |
| Instant | Instant/Instant.h | Instant/Impl/Instant.impl |
| Calendar | Calendar/Calendar.h | Calendar/Impl/Calendar.impl |
| Timestamp | Timestamp/Timestamp.h | Timestamp/Impl/Timestamp.impl |
| Epoch | Epoch/Epoch.h | Epoch/Impl/Epoch.impl and Epoch/Impl/LeapSeconds.inc |
| Timezone | Timezone/Timezone.h | Timezone/Impl/Timezone.impl and Timezone/Impl/IanaZones.inc |
| Format | Format/Format.h | Format/Impl/Format.impl |

The integration test is Codespace/Tests/Chronometry/Chronometry.c. It exercises conversions, calendar reforms, ISO-week round trips, TAI/UTC, DST gaps and overlaps, timezone aliases, country lookup and formatting/parsing.
---

# Portability

- `Chronometry/Impl/Common.inc` defines `_POSIX_C_SOURCE 200809L` when not already set, allowing clean compilation under strict `-std=c11` on POSIX hosts.
- All platform detection uses `Detect.h` (`OS_WINDOWS`, `OS_MAC`, POSIX).

## Related Modules

- [Module](../Modules/Module.md) — declarative DSL (`MODULE`, `PACKAGE`, `VARIANT`, `STRUCT`, `ENUM_DEF_DIRECT`).
- [Condition](../Modules/Condition.md) — `OPSTATUS` return contract.
- [Algorithm](../Modules/Algorithm.md) — `CMPOUT` comparison contract.
