# Chronometry

**Chronometry** provides elapsed-time arithmetic, monotonic clock readings, timers, Gregorian and Julian calendars, historical calendar reforms, civil date/time, Unix instants, fixed UTC offsets, and formatting.

```c
#include "Cosmeron/Modules/Chronometry/Chronometry.h"
```

This aggregate header includes all nine packages. The reference below follows the function-by-function layout of `Bit.md`, with two equivalent naming forms.

---

# Overview

| Package | Header | Purpose |
| --- | --- | --- |
| [Duration](#duration-package) | `Duration/Duration.h` | Signed time intervals |
| [Clock](#clock-package) | `Clock/Clock.h` | Monotonic measurements |
| [Timer](#timer-package) | `Timer/Timer.h` | Stopwatches and frame budgets |
| [Instant](#instant-package) | `Instant/Instant.h` | Timeline instants |
| [Calendar](#calendar-package) | `Calendar/Calendar.h` | Gregorian, Julian and historical reforms |
| [DateTime](#datetime-package) | `DateTime/DateTime.h` | Civil dates and times |
| [Epoch](#epoch-package) | `Epoch/Epoch.h` | JDN, Rata Die and Unix conversions |
| [Timezone](#timezone-package) | `Timezone/Timezone.h` | Fixed UTC offsets |
| [Format](#format-package) | `Format/Format.h` | Date and time text |

### Macro form

```c
CHRONOMETRY_TYPE(Duration) d = DURATION_FUNC(FromNanoseconds)(1000);
```

### Direct form

```c
Chronometry_Duration d = Chronometry_Duration_FromNanoseconds(1000);
```

With the default namespace these forms call the same function. With a configured `COSMERON_NAMESPACE`, the direct symbol changes, but the macro form remains usable.

### Time domains

- `Duration`: signed number of elapsed nanoseconds.
- `ClockTimePoint`: monotonic reading for interval measurement, **not** a Unix timestamp.
- `Instant`: absolute timeline coordinate of seconds plus nanoseconds, used for Unix conversion.
- `Date`, `DateTime`: human calendar fields under selected rules. DateTime has **millisecond**, not nanosecond, precision.
- `UtcOffset`: signed minutes east of UTC; no named-zone database or automatic daylight-saving transitions.

---

# API reference

## Function summary

| Package | Function | Purpose |
| --- | --- | --- |
| Duration | [`FromNanoseconds`](#duration-fromnanoseconds) | Creates a Duration from signed nanoseconds. |
| Duration | [`FromSeconds`](#duration-fromseconds) | Converts whole seconds to a Duration. |
| Duration | [`Add`](#duration-add) | Adds two Durations. |
| Duration | [`Subtract`](#duration-subtract) | Subtracts the right Duration from the left. |
| Duration | [`Compare`](#duration-compare) | Compares Duration values. |
| Duration | [`Absolute`](#duration-absolute) | Computes absolute magnitude of a Duration. |
| Clock | [`Now`](#clock-now) | Reads a monotonic time point. |
| Clock | [`DurationBetween`](#clock-durationbetween) | Computes end minus start of clock points. |
| Timer | [`Start`](#timer-start) | Starts or resets stopwatch start point. |
| Timer | [`Elapsed`](#timer-elapsed) | Measures elapsed time without resetting timer. |
| Timer | [`Restart`](#timer-restart) | Measures elapsed time and updates start. |
| Timer | [`DeltaStart`](#timer-deltastart) | Records initial delta timestamp. |
| Timer | [`DeltaUpdate`](#timer-deltaupdate) | Reports duration since previous delta update. |
| Timer | [`FrameLimiterCreate`](#timer-framelimitercreate) | Constructs a positive frame-time budget. |
| Timer | [`FrameLimiterFromFPS`](#timer-framelimiterfromfps) | Creates frame-time budget from FPS. |
| Timer | [`FrameLimiterBegin`](#timer-framelimiterbegin) | Marks beginning of a frame. |
| Timer | [`FrameLimiterRemaining`](#timer-framelimiterremaining) | Computes remaining frame-time budget. |
| Instant | [`Compare`](#instant-compare) | Compares seconds and fractional nanoseconds. |
| Instant | [`AddDuration`](#instant-addduration) | Adds signed Duration to Instant. |
| Instant | [`DurationBetween`](#instant-durationbetween) | Calculates end minus start as a Duration. |
| Calendar | [`SystemPolicy`](#calendar-systempolicy) | Initializes a pure calendar policy. |
| Calendar | [`ReformPolicy`](#calendar-reformpolicy) | Initializes a predefined historical reform. |
| Calendar | [`DateCompare`](#calendar-datecompare) | Compares date fields year/month/day. |
| Calendar | [`IsLeapYear`](#calendar-isleapyear) | Checks leap year under Gregorian or Julian rule. |
| Calendar | [`DaysInMonth`](#calendar-daysinmonth) | Returns month length including leap February. |
| Calendar | [`DateIsValid`](#calendar-dateisvalid) | Validates date under a calendar policy. |
| Calendar | [`DateToDayOfYear`](#calendar-datetodayofyear) | Computes one-based day number of year. |
| Calendar | [`DateToOrdinal`](#calendar-datetoordinal) | Converts civil date to absolute Julian Day Number. |
| Calendar | [`DateFromOrdinal`](#calendar-datefromordinal) | Converts JDN back into policy-specific date. |
| Calendar | [`DateDifference`](#calendar-datedifference) | Computes right minus left in elapsed days. |
| DateTime | [`TimeIsValid`](#datetime-timeisvalid) | Validates hour/minute/second/millisecond. |
| DateTime | [`IsValid`](#datetime-isvalid) | Validates civil DateTime under policy. |
| DateTime | [`Compare`](#datetime-compare) | Compares date-time fields in civil order. |
| DateTime | [`AddDuration`](#datetime-addduration) | Adds Duration to civil DateTime under policy. |
| DateTime | [`Difference`](#datetime-difference) | Computes right DateTime minus left as Duration. |
| Epoch | [`DateToJulianDayNumber`](#epoch-datetojuliandaynumber) | Converts date under policy to JDN. |
| Epoch | [`DateFromJulianDayNumber`](#epoch-datefromjuliandaynumber) | Converts JDN to date using policy. |
| Epoch | [`DateToRataDie`](#epoch-datetoratadie) | Converts Gregorian date to Rata Die count. |
| Epoch | [`DateTimeToUnix`](#epoch-datetimetounix) | Converts Gregorian UTC DateTime to Unix Instant. |
| Epoch | [`DateTimeFromUnix`](#epoch-datetimefromunix) | Converts Unix Instant to Gregorian UTC DateTime. |
| Timezone | [`OffsetIsValid`](#timezone-offsetisvalid) | Checks validity of minutes-east-of-UTC offset. |
| Timezone | [`ToUTC`](#timezone-toutc) | Converts a fixed-offset local DateTime to UTC Instant. |
| Timezone | [`FromUTC`](#timezone-fromutc) | Converts UTC Instant into fixed-offset local DateTime. |
| Format | [`DateISO`](#format-dateiso) | Formats Gregorian date into a signed-year ISO-like string. |
| Format | [`DateTimeISO`](#format-datetimeiso) | Formats Gregorian date and millisecond time. |
| Format | [`ParseDateISO`](#format-parsedateiso) | Parses Gregorian date from text. |
| Format | [`ParseDateTimeISO`](#format-parsedatetimeiso) | Parses Gregorian date and millisecond time from text. |

---

# Types

| Macro | Default direct name | Structure |
| --- | --- | --- |
| `CHRONOMETRY_TYPE(Duration)` | `Chronometry_Duration` | `int64_t nanoseconds` |
| `CHRONOMETRY_TYPE(ClockTimePoint)` | `Chronometry_ClockTimePoint` | `uint64_t nanoseconds` |
| `CHRONOMETRY_TYPE(Timer)` | `Chronometry_Timer` | `ClockTimePoint start` |
| `CHRONOMETRY_TYPE(Delta)` | `Chronometry_Delta` | `ClockTimePoint previous` |
| `CHRONOMETRY_TYPE(FrameLimiter)` | `Chronometry_FrameLimiter` | Duration target and frameStart |
| `CHRONOMETRY_TYPE(Instant)` | `Chronometry_Instant` | `int64_t seconds`, `uint32_t nanoseconds` |
| `CHRONOMETRY_TYPE(Date)` | `Chronometry_Date` | Year, month, day bit-fields |
| `CHRONOMETRY_TYPE(TimeOfDay)` | `Chronometry_TimeOfDay` | Millisecond, second, minute, hour bit-fields |
| `CHRONOMETRY_TYPE(DateTime)` | `Chronometry_DateTime` | Date and TimeOfDay |
| `CHRONOMETRY_TYPE(UtcOffset)` | `Chronometry_UtcOffset` | `int16_t minutesEastOfUtc` |
| `CHRONOMETRY_TYPE(OffsetDateTime)` | `Chronometry_OffsetDateTime` | Local DateTime, offset, daylightSavings |
| `CALENDAR_TYPE(System)` | `Chronometry_Calendar_System` | GREGORIAN or JULIAN |
| `CALENDAR_TYPE(Policy)` | `Chronometry_Calendar_Policy` | Pure calendar or historical reform |
| `CALENDAR_TYPE(Reform)` | `Chronometry_Calendar_Reform` | Julian/Gregorian cutover dates |
| `CALENDAR_TYPE(ReformId)` | `Chronometry_Calendar_ReformId` | Predefined reform identifier |
| `CALENDAR_TYPE(PolicyKind)` | `Chronometry_Calendar_PolicyKind` | PURE_SYSTEM or HISTORICAL_REFORM |

**Aggregate initializer warning:** `TimeOfDay` fields are ordered `{millisecond, second, minute, hour}`, NOT hour-first. `Date` has a signed 23-bit year plus month/day bit-fields; validate inputs before relying on arithmetic.

---

# Constants and calendar tables

`DURATION_CONST(NAME)` exposes `NANOSECONDS_PER_MICROSECOND` = 1000, `NANOSECONDS_PER_MILLISECOND` = 1000000, `NANOSECONDS_PER_SECOND` = 1000000000, `MILLISECONDS_PER_SECOND` = 1000, `SECONDS_PER_MINUTE` = 60, `MINUTES_PER_HOUR` = 60, `HOURS_PER_DAY` = 24, `SECONDS_PER_HOUR` = 3600 and `SECONDS_PER_DAY` = 86400.

`CALENDAR_CONST(NAME)` exposes `JANUARY`–`DECEMBER` (1–12), `GREGORIAN`, `JULIAN`, `PURE_SYSTEM`, `HISTORICAL_REFORM`, `CATHOLIC_1582`, `BRITISH_1752`, and `RUSSIAN_1918`. Month lengths are available through `CALENDAR_MONTH_LENGTH_CONST(NAME)`. The X-macros `CHRONOMETRY_CALENDAR_MONTH_TABLE(X)` and `CHRONOMETRY_CALENDAR_REFORM_TABLE(X)` enumerate month or reform metadata for generation.

| Reform | Last Julian date | First Gregorian date | Suppressed civil days |
| --- | --- | --- | ---: |
| `CATHOLIC_1582` | 1582-10-04 | 1582-10-15 | 10 |
| `BRITISH_1752` | 1752-09-02 | 1752-09-14 | 11 |
| `RUSSIAN_1918` | 1918-01-31 | 1918-02-14 | 13 |

To use historical rules, first create a policy using `CALENDAR_FUNC(ReformPolicy)`; suppressed dates are invalid.

---

# Result conventions

`OPSTATUS` returning functions report `STATUS_CONST(SUCCESS)` when successful; possible failures include `INVALID_ARGUMENT`, `OUT_OF_RANGE`, `ARITHMETIC_OVERFLOW` and, for platform calls, `GENERIC_ERROR`. Predicates return `bool`. Comparisons return `CMPOUT` with `COMPARISON_LOWER_CONST`, `COMPARISON_EQUAL_CONST`, `COMPARISON_HIGHER_CONST`.

Most outputs are modified only on success, but multi-field conversions can write an early field before a later operation fails; avoid depending on unchanged output in that situation.

---

# Duration package

Header: `Cosmeron/Modules/Chronometry/Duration/Duration.h`

Signed int64 nanoseconds are suitable for elapsed intervals but cannot represent arbitrarily long date differences.

## Function summary

| Function | Description |
| --- | --- |
| [`FromNanoseconds`](#duration-fromnanoseconds) | Creates a Duration from signed nanoseconds. |
| [`FromSeconds`](#duration-fromseconds) | Converts whole seconds to a Duration. |
| [`Add`](#duration-add) | Adds two Durations. |
| [`Subtract`](#duration-subtract) | Subtracts the right Duration from the left. |
| [`Compare`](#duration-compare) | Compares Duration values. |
| [`Absolute`](#duration-absolute) | Computes absolute magnitude of a Duration. |

---

# Duration FromNanoseconds

Creates a Duration from signed nanoseconds.

### Syntax

#### Macro form

```c
CHRONOMETRY_TYPE(Duration) DURATION_FUNC(FromNanoseconds)(int64_t nanoseconds);
```

#### Direct form

```c
CHRONOMETRY_TYPE(Duration) Chronometry_Duration_FromNanoseconds(int64_t nanoseconds);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `nanoseconds` | `int64_t nanoseconds` | Nanosecond count. |

---

### Return value

`CHRONOMETRY_TYPE(Duration)`: returned value directly.

---

### Remarks

Returns the value directly, without status or output pointer.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) d = DURATION_FUNC(FromNanoseconds)(500000000);
```

---

# Duration FromSeconds

Converts whole seconds to a Duration.

### Syntax

#### Macro form

```c
OPSTATUS DURATION_FUNC(FromSeconds)(int64_t seconds, CHRONOMETRY_TYPE(Duration) *outResult);
```

#### Direct form

```c
OPSTATUS Chronometry_Duration_FromSeconds(int64_t seconds, CHRONOMETRY_TYPE(Duration) *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `seconds` | `int64_t seconds` | Whole seconds. |
| `outResult` | `CHRONOMETRY_TYPE(Duration) *outResult` | Writable result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Rejects null output and signed multiplication overflow.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) d;
OPSTATUS s = DURATION_FUNC(FromSeconds)(2, &d);
```

---

# Duration Add

Adds two Durations.

### Syntax

#### Macro form

```c
OPSTATUS DURATION_FUNC(Add)(CHRONOMETRY_TYPE(Duration) left, CHRONOMETRY_TYPE(Duration) right, CHRONOMETRY_TYPE(Duration) *outResult);
```

#### Direct form

```c
OPSTATUS Chronometry_Duration_Add(CHRONOMETRY_TYPE(Duration) left, CHRONOMETRY_TYPE(Duration) right, CHRONOMETRY_TYPE(Duration) *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `CHRONOMETRY_TYPE(Duration) left` | Left input. |
| `right` | `CHRONOMETRY_TYPE(Duration) right` | Right input. |
| `outResult` | `CHRONOMETRY_TYPE(Duration) *outResult` | Writable result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Checks int64 signed addition overflow.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) sum;
OPSTATUS s = DURATION_FUNC(Add)(a, b, &sum);
```

---

# Duration Subtract

Subtracts the right Duration from the left.

### Syntax

#### Macro form

```c
OPSTATUS DURATION_FUNC(Subtract)(CHRONOMETRY_TYPE(Duration) left, CHRONOMETRY_TYPE(Duration) right, CHRONOMETRY_TYPE(Duration) *outResult);
```

#### Direct form

```c
OPSTATUS Chronometry_Duration_Subtract(CHRONOMETRY_TYPE(Duration) left, CHRONOMETRY_TYPE(Duration) right, CHRONOMETRY_TYPE(Duration) *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `CHRONOMETRY_TYPE(Duration) left` | Left input. |
| `right` | `CHRONOMETRY_TYPE(Duration) right` | Right input. |
| `outResult` | `CHRONOMETRY_TYPE(Duration) *outResult` | Writable result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Checks int64 signed subtraction overflow.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) diff;
OPSTATUS s = DURATION_FUNC(Subtract)(a, b, &diff);
```

---

# Duration Compare

Compares Duration values.

### Syntax

#### Macro form

```c
CMPOUT DURATION_FUNC(Compare)(CHRONOMETRY_TYPE(Duration) left, CHRONOMETRY_TYPE(Duration) right);
```

#### Direct form

```c
CMPOUT Chronometry_Duration_Compare(CHRONOMETRY_TYPE(Duration) left, CHRONOMETRY_TYPE(Duration) right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `CHRONOMETRY_TYPE(Duration) left` | Left input. |
| `right` | `CHRONOMETRY_TYPE(Duration) right` | Right input. |

---

### Return value

`CMPOUT`: LOWER, EQUAL or HIGHER.

---

### Remarks

Returns CMPOUT: LOWER, EQUAL or HIGHER.

---

### Example

```c
CMPOUT order = DURATION_FUNC(Compare)(a, b);
```

---

# Duration Absolute

Computes absolute magnitude of a Duration.

### Syntax

#### Macro form

```c
OPSTATUS DURATION_FUNC(Absolute)(CHRONOMETRY_TYPE(Duration) value, CHRONOMETRY_TYPE(Duration) *outResult);
```

#### Direct form

```c
OPSTATUS Chronometry_Duration_Absolute(CHRONOMETRY_TYPE(Duration) value, CHRONOMETRY_TYPE(Duration) *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `CHRONOMETRY_TYPE(Duration) value` | Value to convert/validate or output pointer. |
| `outResult` | `CHRONOMETRY_TYPE(Duration) *outResult` | Writable result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

INT64_MIN cannot be represented as a positive int64 and produces ARITHMETIC_OVERFLOW.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) positive;
OPSTATUS s = DURATION_FUNC(Absolute)(negative, &positive);
```

---

# Clock package

Header: `Cosmeron/Modules/Chronometry/Clock/Clock.h`

Use this clock for measuring intervals, never for Unix epoch or timezone conversion.

## Function summary

| Function | Description |
| --- | --- |
| [`Now`](#clock-now) | Reads a monotonic time point. |
| [`DurationBetween`](#clock-durationbetween) | Computes end minus start of clock points. |

---

# Clock Now

Reads a monotonic time point.

### Syntax

#### Macro form

```c
OPSTATUS CLOCK_FUNC(Now)(CHRONOMETRY_TYPE(ClockTimePoint) *outResult);
```

#### Direct form

```c
OPSTATUS Chronometry_Clock_Now(CHRONOMETRY_TYPE(ClockTimePoint) *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `outResult` | `CHRONOMETRY_TYPE(ClockTimePoint) *outResult` | Writable result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Windows uses performance counters; macOS uses Mach; other branch uses CLOCK_MONOTONIC. This is NOT wall-clock/Unix time.

---

### Example

```c
CHRONOMETRY_TYPE(ClockTimePoint) now;
OPSTATUS s = CLOCK_FUNC(Now)(&now);
```

---

# Clock DurationBetween

Computes end minus start of clock points.

### Syntax

#### Macro form

```c
OPSTATUS CLOCK_FUNC(DurationBetween)(CHRONOMETRY_TYPE(ClockTimePoint) start, CHRONOMETRY_TYPE(ClockTimePoint) end, CHRONOMETRY_TYPE(Duration) *outResult);
```

#### Direct form

```c
OPSTATUS Chronometry_Clock_DurationBetween(CHRONOMETRY_TYPE(ClockTimePoint) start, CHRONOMETRY_TYPE(ClockTimePoint) end, CHRONOMETRY_TYPE(Duration) *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `start` | `CHRONOMETRY_TYPE(ClockTimePoint) start` | First time point. |
| `end` | `CHRONOMETRY_TYPE(ClockTimePoint) end` | Last time point. |
| `outResult` | `CHRONOMETRY_TYPE(Duration) *outResult` | Writable result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Signed nanosecond Duration, reporting overflow if the difference exceeds int64.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) elapsed;
OPSTATUS s = CLOCK_FUNC(DurationBetween)(start, end, &elapsed);
```

---

# Timer package

Header: `Cosmeron/Modules/Chronometry/Timer/Timer.h`

FrameLimiter computes remaining time but **does not sleep or wait**. The caller is responsible for any pacing.

## Function summary

| Function | Description |
| --- | --- |
| [`Start`](#timer-start) | Starts or resets stopwatch start point. |
| [`Elapsed`](#timer-elapsed) | Measures elapsed time without resetting timer. |
| [`Restart`](#timer-restart) | Measures elapsed time and updates start. |
| [`DeltaStart`](#timer-deltastart) | Records initial delta timestamp. |
| [`DeltaUpdate`](#timer-deltaupdate) | Reports duration since previous delta update. |
| [`FrameLimiterCreate`](#timer-framelimitercreate) | Constructs a positive frame-time budget. |
| [`FrameLimiterFromFPS`](#timer-framelimiterfromfps) | Creates frame-time budget from FPS. |
| [`FrameLimiterBegin`](#timer-framelimiterbegin) | Marks beginning of a frame. |
| [`FrameLimiterRemaining`](#timer-framelimiterremaining) | Computes remaining frame-time budget. |

---

# Timer Start

Starts or resets stopwatch start point.

### Syntax

#### Macro form

```c
OPSTATUS TIMER_FUNC(Start)(CHRONOMETRY_TYPE(Timer) *timer);
```

#### Direct form

```c
OPSTATUS Chronometry_Timer_Start(CHRONOMETRY_TYPE(Timer) *timer);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `timer` | `CHRONOMETRY_TYPE(Timer) *timer` | Stopwatch object. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Calls Clock Now; stopwatch must be started before Elapsed or Restart.

---

### Example

```c
CHRONOMETRY_TYPE(Timer) timer;
OPSTATUS s = TIMER_FUNC(Start)(&timer);
```

---

# Timer Elapsed

Measures elapsed time without resetting timer.

### Syntax

#### Macro form

```c
OPSTATUS TIMER_FUNC(Elapsed)(const CHRONOMETRY_TYPE(Timer) *timer, CHRONOMETRY_TYPE(Duration) *outElapsed);
```

#### Direct form

```c
OPSTATUS Chronometry_Timer_Elapsed(const CHRONOMETRY_TYPE(Timer) *timer, CHRONOMETRY_TYPE(Duration) *outElapsed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `timer` | `const CHRONOMETRY_TYPE(Timer) *timer` | Stopwatch object. |
| `outElapsed` | `CHRONOMETRY_TYPE(Duration) *outElapsed` | Writable elapsed Duration result. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Reads monotonic clock and subtracts timer start.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) elapsed;
OPSTATUS s = TIMER_FUNC(Elapsed)(&timer, &elapsed);
```

---

# Timer Restart

Measures elapsed time and updates start.

### Syntax

#### Macro form

```c
OPSTATUS TIMER_FUNC(Restart)(CHRONOMETRY_TYPE(Timer) *timer, CHRONOMETRY_TYPE(Duration) *outElapsed);
```

#### Direct form

```c
OPSTATUS Chronometry_Timer_Restart(CHRONOMETRY_TYPE(Timer) *timer, CHRONOMETRY_TYPE(Duration) *outElapsed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `timer` | `CHRONOMETRY_TYPE(Timer) *timer` | Stopwatch object. |
| `outElapsed` | `CHRONOMETRY_TYPE(Duration) *outElapsed` | Writable elapsed Duration result. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Advances timer start only after successful duration calculation.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) elapsed;
OPSTATUS s = TIMER_FUNC(Restart)(&timer, &elapsed);
```

---

# Timer DeltaStart

Records initial delta timestamp.

### Syntax

#### Macro form

```c
OPSTATUS TIMER_FUNC(DeltaStart)(CHRONOMETRY_TYPE(Delta) *delta);
```

#### Direct form

```c
OPSTATUS Chronometry_Timer_DeltaStart(CHRONOMETRY_TYPE(Delta) *delta);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `delta` | `CHRONOMETRY_TYPE(Delta) *delta` | Delta measurement object. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Calls Clock Now into delta.previous.

---

### Example

```c
CHRONOMETRY_TYPE(Delta) delta;
OPSTATUS s = TIMER_FUNC(DeltaStart)(&delta);
```

---

# Timer DeltaUpdate

Reports duration since previous delta update.

### Syntax

#### Macro form

```c
OPSTATUS TIMER_FUNC(DeltaUpdate)(CHRONOMETRY_TYPE(Delta) *delta, CHRONOMETRY_TYPE(Duration) *outElapsed);
```

#### Direct form

```c
OPSTATUS Chronometry_Timer_DeltaUpdate(CHRONOMETRY_TYPE(Delta) *delta, CHRONOMETRY_TYPE(Duration) *outElapsed);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `delta` | `CHRONOMETRY_TYPE(Delta) *delta` | Delta measurement object. |
| `outElapsed` | `CHRONOMETRY_TYPE(Duration) *outElapsed` | Writable elapsed Duration result. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

On success stores new previous clock point. Useful for frame timing.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) step;
OPSTATUS s = TIMER_FUNC(DeltaUpdate)(&delta, &step);
```

---

# Timer FrameLimiterCreate

Constructs a positive frame-time budget.

### Syntax

#### Macro form

```c
OPSTATUS TIMER_FUNC(FrameLimiterCreate)(CHRONOMETRY_TYPE(Duration) target, CHRONOMETRY_TYPE(FrameLimiter) *outLimiter);
```

#### Direct form

```c
OPSTATUS Chronometry_Timer_FrameLimiterCreate(CHRONOMETRY_TYPE(Duration) target, CHRONOMETRY_TYPE(FrameLimiter) *outLimiter);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `target` | `CHRONOMETRY_TYPE(Duration) target` | Duration target. |
| `outLimiter` | `CHRONOMETRY_TYPE(FrameLimiter) *outLimiter` | Writable FrameLimiter output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Rejects nonpositive target duration; initializes frameStart to zero. Begin must follow.

---

### Example

```c
CHRONOMETRY_TYPE(FrameLimiter) limit;
CHRONOMETRY_TYPE(Duration) target = DURATION_FUNC(FromNanoseconds)(16666666);
OPSTATUS s = TIMER_FUNC(FrameLimiterCreate)(target, &limit);
```

---

# Timer FrameLimiterFromFPS

Creates frame-time budget from FPS.

### Syntax

#### Macro form

```c
OPSTATUS TIMER_FUNC(FrameLimiterFromFPS)(uint32_t fps, CHRONOMETRY_TYPE(FrameLimiter) *outLimiter);
```

#### Direct form

```c
OPSTATUS Chronometry_Timer_FrameLimiterFromFPS(uint32_t fps, CHRONOMETRY_TYPE(FrameLimiter) *outLimiter);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `fps` | `uint32_t fps` | Desired frames per second. |
| `outLimiter` | `CHRONOMETRY_TYPE(FrameLimiter) *outLimiter` | Writable FrameLimiter output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Target is floor(1,000,000,000/fps) ns; rejects fps=0 and results of zero. Does NOT sleep.

---

### Example

```c
CHRONOMETRY_TYPE(FrameLimiter) limit;
OPSTATUS s = TIMER_FUNC(FrameLimiterFromFPS)(60, &limit);
```

---

# Timer FrameLimiterBegin

Marks beginning of a frame.

### Syntax

#### Macro form

```c
OPSTATUS TIMER_FUNC(FrameLimiterBegin)(CHRONOMETRY_TYPE(FrameLimiter) *limiter);
```

#### Direct form

```c
OPSTATUS Chronometry_Timer_FrameLimiterBegin(CHRONOMETRY_TYPE(FrameLimiter) *limiter);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `limiter` | `CHRONOMETRY_TYPE(FrameLimiter) *limiter` | Frame budget state. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Reads monotonic Clock Now. Needed before Remaining.

---

### Example

```c
OPSTATUS s = TIMER_FUNC(FrameLimiterBegin)(&limit);
```

---

# Timer FrameLimiterRemaining

Computes remaining frame-time budget.

### Syntax

#### Macro form

```c
OPSTATUS TIMER_FUNC(FrameLimiterRemaining)(const CHRONOMETRY_TYPE(FrameLimiter) *limiter, CHRONOMETRY_TYPE(Duration) *outRemaining);
```

#### Direct form

```c
OPSTATUS Chronometry_Timer_FrameLimiterRemaining(const CHRONOMETRY_TYPE(FrameLimiter) *limiter, CHRONOMETRY_TYPE(Duration) *outRemaining);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `limiter` | `const CHRONOMETRY_TYPE(FrameLimiter) *limiter` | Frame budget state. |
| `outRemaining` | `CHRONOMETRY_TYPE(Duration) *outRemaining` | Writable remaining Duration output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Clamps negative remainder to zero. This function does NOT pause, sleep, block or enforce the limit.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) remaining;
OPSTATUS s = TIMER_FUNC(FrameLimiterRemaining)(&limit, &remaining);
```

---

# Instant package

Header: `Cosmeron/Modules/Chronometry/Instant/Instant.h`

Normalize nanoseconds to [0, 999999999] when constructing Instant manually; comparisons assume valid representations.

## Function summary

| Function | Description |
| --- | --- |
| [`Compare`](#instant-compare) | Compares seconds and fractional nanoseconds. |
| [`AddDuration`](#instant-addduration) | Adds signed Duration to Instant. |
| [`DurationBetween`](#instant-durationbetween) | Calculates end minus start as a Duration. |

---

# Instant Compare

Compares seconds and fractional nanoseconds.

### Syntax

#### Macro form

```c
CMPOUT INSTANT_FUNC(Compare)(CHRONOMETRY_TYPE(Instant) left, CHRONOMETRY_TYPE(Instant) right);
```

#### Direct form

```c
CMPOUT Chronometry_Instant_Compare(CHRONOMETRY_TYPE(Instant) left, CHRONOMETRY_TYPE(Instant) right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `CHRONOMETRY_TYPE(Instant) left` | Left input. |
| `right` | `CHRONOMETRY_TYPE(Instant) right` | Right input. |

---

### Return value

`CMPOUT`: LOWER, EQUAL or HIGHER.

---

### Remarks

Assumes normalized nanoseconds (0–999,999,999); comparison does not validate.

---

### Example

```c
CMPOUT order = INSTANT_FUNC(Compare)(a, b);
```

---

# Instant AddDuration

Adds signed Duration to Instant.

### Syntax

#### Macro form

```c
OPSTATUS INSTANT_FUNC(AddDuration)(CHRONOMETRY_TYPE(Instant) instant, CHRONOMETRY_TYPE(Duration) duration, CHRONOMETRY_TYPE(Instant) *outResult);
```

#### Direct form

```c
OPSTATUS Chronometry_Instant_AddDuration(CHRONOMETRY_TYPE(Instant) instant, CHRONOMETRY_TYPE(Duration) duration, CHRONOMETRY_TYPE(Instant) *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `instant` | `CHRONOMETRY_TYPE(Instant) instant` | Instant input/output. |
| `duration` | `CHRONOMETRY_TYPE(Duration) duration` | Signed time interval. |
| `outResult` | `CHRONOMETRY_TYPE(Instant) *outResult` | Writable result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Normalizes nanosecond carry/borrow into seconds and checks overflow.

---

### Example

```c
CHRONOMETRY_TYPE(Instant) shifted;
OPSTATUS s = INSTANT_FUNC(AddDuration)(instant, duration, &shifted);
```

---

# Instant DurationBetween

Calculates end minus start as a Duration.

### Syntax

#### Macro form

```c
OPSTATUS INSTANT_FUNC(DurationBetween)(CHRONOMETRY_TYPE(Instant) start, CHRONOMETRY_TYPE(Instant) end, CHRONOMETRY_TYPE(Duration) *outResult);
```

#### Direct form

```c
OPSTATUS Chronometry_Instant_DurationBetween(CHRONOMETRY_TYPE(Instant) start, CHRONOMETRY_TYPE(Instant) end, CHRONOMETRY_TYPE(Duration) *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `start` | `CHRONOMETRY_TYPE(Instant) start` | First time point. |
| `end` | `CHRONOMETRY_TYPE(Instant) end` | Last time point. |
| `outResult` | `CHRONOMETRY_TYPE(Duration) *outResult` | Writable result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

An interval beyond signed int64 nanoseconds (about 292 years) overflows.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) diff;
OPSTATUS s = INSTANT_FUNC(DurationBetween)(start, end, &diff);
```

---

# Calendar package

Header: `Cosmeron/Modules/Chronometry/Calendar/Calendar.h`

Calendar policies select Gregorian/Julian or one of three historical cutovers. DateToOrdinal returns a **Julian Day Number**, despite its name.

## Function summary

| Function | Description |
| --- | --- |
| [`SystemPolicy`](#calendar-systempolicy) | Initializes a pure calendar policy. |
| [`ReformPolicy`](#calendar-reformpolicy) | Initializes a predefined historical reform. |
| [`DateCompare`](#calendar-datecompare) | Compares date fields year/month/day. |
| [`IsLeapYear`](#calendar-isleapyear) | Checks leap year under Gregorian or Julian rule. |
| [`DaysInMonth`](#calendar-daysinmonth) | Returns month length including leap February. |
| [`DateIsValid`](#calendar-dateisvalid) | Validates date under a calendar policy. |
| [`DateToDayOfYear`](#calendar-datetodayofyear) | Computes one-based day number of year. |
| [`DateToOrdinal`](#calendar-datetoordinal) | Converts civil date to absolute Julian Day Number. |
| [`DateFromOrdinal`](#calendar-datefromordinal) | Converts JDN back into policy-specific date. |
| [`DateDifference`](#calendar-datedifference) | Computes right minus left in elapsed days. |

---

# Calendar SystemPolicy

Initializes a pure calendar policy.

### Syntax

#### Macro form

```c
OPSTATUS CALENDAR_FUNC(SystemPolicy)(CALENDAR_TYPE(System) system, CALENDAR_TYPE(Policy) *outPolicy);
```

#### Direct form

```c
OPSTATUS Chronometry_Calendar_SystemPolicy(CALENDAR_TYPE(System) system, CALENDAR_TYPE(Policy) *outPolicy);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `system` | `CALENDAR_TYPE(System) system` | Gregorian/Julian calendar enum. |
| `outPolicy` | `CALENDAR_TYPE(Policy) *outPolicy` | Writable Policy output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Accepts GREGORIAN or JULIAN. Returns OUT_OF_RANGE for invalid enum.

---

### Example

```c
CALENDAR_TYPE(Policy) policy;
OPSTATUS s = CALENDAR_FUNC(SystemPolicy)(CALENDAR_CONST(GREGORIAN), &policy);
```

---

# Calendar ReformPolicy

Initializes a predefined historical reform.

### Syntax

#### Macro form

```c
OPSTATUS CALENDAR_FUNC(ReformPolicy)(CALENDAR_TYPE(ReformId) id, CALENDAR_TYPE(Policy) *outPolicy);
```

#### Direct form

```c
OPSTATUS Chronometry_Calendar_ReformPolicy(CALENDAR_TYPE(ReformId) id, CALENDAR_TYPE(Policy) *outPolicy);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `id` | `CALENDAR_TYPE(ReformId) id` | Reform identifier. |
| `outPolicy` | `CALENDAR_TYPE(Policy) *outPolicy` | Writable Policy output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Supports CATHOLIC_1582, BRITISH_1752, RUSSIAN_1918. Skipped civil dates are invalid.

---

### Example

```c
CALENDAR_TYPE(Policy) policy;
OPSTATUS s = CALENDAR_FUNC(ReformPolicy)(CALENDAR_CONST(CATHOLIC_1582), &policy);
```

---

# Calendar DateCompare

Compares date fields year/month/day.

### Syntax

#### Macro form

```c
CMPOUT CALENDAR_FUNC(DateCompare)(CHRONOMETRY_TYPE(Date) left, CHRONOMETRY_TYPE(Date) right);
```

#### Direct form

```c
CMPOUT Chronometry_Calendar_DateCompare(CHRONOMETRY_TYPE(Date) left, CHRONOMETRY_TYPE(Date) right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `CHRONOMETRY_TYPE(Date) left` | Left input. |
| `right` | `CHRONOMETRY_TYPE(Date) right` | Right input. |

---

### Return value

`CMPOUT`: LOWER, EQUAL or HIGHER.

---

### Remarks

Lexicographic field comparison only, without policy/validity checks.

---

### Example

```c
CMPOUT order = CALENDAR_FUNC(DateCompare)(first, second);
```

---

# Calendar IsLeapYear

Checks leap year under Gregorian or Julian rule.

### Syntax

#### Macro form

```c
bool CALENDAR_FUNC(IsLeapYear)(CALENDAR_TYPE(System) system, int32_t year);
```

#### Direct form

```c
bool Chronometry_Calendar_IsLeapYear(CALENDAR_TYPE(System) system, int32_t year);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `system` | `CALENDAR_TYPE(System) system` | Gregorian/Julian calendar enum. |
| `year` | `int32_t year` | Civil year. |

---

### Return value

`bool`: true or false.

---

### Remarks

Julian every fourth year. Gregorian excludes non-400-century years. Non-JULIAN enum values follow Gregorian branch.

---

### Example

```c
bool leap = CALENDAR_FUNC(IsLeapYear)(CALENDAR_CONST(GREGORIAN), 2024);
```

---

# Calendar DaysInMonth

Returns month length including leap February.

### Syntax

#### Macro form

```c
OPSTATUS CALENDAR_FUNC(DaysInMonth)(CALENDAR_TYPE(System) system, int32_t year, uint8_t month, uint8_t *outDays);
```

#### Direct form

```c
OPSTATUS Chronometry_Calendar_DaysInMonth(CALENDAR_TYPE(System) system, int32_t year, uint8_t month, uint8_t *outDays);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `system` | `CALENDAR_TYPE(System) system` | Gregorian/Julian calendar enum. |
| `year` | `int32_t year` | Civil year. |
| `month` | `uint8_t month` | Month 1–12. |
| `outDays` | `uint8_t *outDays` | Writable day difference/length output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Month must be 1–12. Invalid month returns OUT_OF_RANGE. Does not independently validate system enum.

---

### Example

```c
uint8_t days;
OPSTATUS s = CALENDAR_FUNC(DaysInMonth)(CALENDAR_CONST(GREGORIAN), 2024, 2, &days);
```

---

# Calendar DateIsValid

Validates date under a calendar policy.

### Syntax

#### Macro form

```c
bool CALENDAR_FUNC(DateIsValid)(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date);
```

#### Direct form

```c
bool Chronometry_Calendar_DateIsValid(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `policy` | `const CALENDAR_TYPE(Policy) *policy` | Calendar Policy pointer. |
| `date` | `CHRONOMETRY_TYPE(Date) date` | Civil date or result pointer. |

---

### Return value

`bool`: true or false.

---

### Remarks

Rejects invalid dates, year <= -4800 and suppressed reform dates. Year stored in signed 23-bit bit-field.

---

### Example

```c
bool valid = CALENDAR_FUNC(DateIsValid)(&policy, date);
```

---

# Calendar DateToDayOfYear

Computes one-based day number of year.

### Syntax

#### Macro form

```c
OPSTATUS CALENDAR_FUNC(DateToDayOfYear)(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date, uint16_t *outDayOfYear);
```

#### Direct form

```c
OPSTATUS Chronometry_Calendar_DateToDayOfYear(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date, uint16_t *outDayOfYear);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `policy` | `const CALENDAR_TYPE(Policy) *policy` | Calendar Policy pointer. |
| `date` | `CHRONOMETRY_TYPE(Date) date` | Civil date or result pointer. |
| `outDayOfYear` | `uint16_t *outDayOfYear` | Writable one-based day number. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

January 1 is day 1. In reform years, post-cutover count excludes suppressed days.

---

### Example

```c
uint16_t day;
OPSTATUS s = CALENDAR_FUNC(DateToDayOfYear)(&policy, date, &day);
```

---

# Calendar DateToOrdinal

Converts civil date to absolute Julian Day Number.

### Syntax

#### Macro form

```c
OPSTATUS CALENDAR_FUNC(DateToOrdinal)(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date, int64_t *outOrdinal);
```

#### Direct form

```c
OPSTATUS Chronometry_Calendar_DateToOrdinal(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date, int64_t *outOrdinal);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `policy` | `const CALENDAR_TYPE(Policy) *policy` | Calendar Policy pointer. |
| `date` | `CHRONOMETRY_TYPE(Date) date` | Civil date or result pointer. |
| `outOrdinal` | `int64_t *outOrdinal` | Writable absolute JDN output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Despite its generic name, ordinal output is a JDN, not day of year.

---

### Example

```c
int64_t jdn;
OPSTATUS s = CALENDAR_FUNC(DateToOrdinal)(&policy, date, &jdn);
```

---

# Calendar DateFromOrdinal

Converts JDN back into policy-specific date.

### Syntax

#### Macro form

```c
OPSTATUS CALENDAR_FUNC(DateFromOrdinal)(const CALENDAR_TYPE(Policy) *policy, int64_t ordinal, CHRONOMETRY_TYPE(Date) *outDate);
```

#### Direct form

```c
OPSTATUS Chronometry_Calendar_DateFromOrdinal(const CALENDAR_TYPE(Policy) *policy, int64_t ordinal, CHRONOMETRY_TYPE(Date) *outDate);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `policy` | `const CALENDAR_TYPE(Policy) *policy` | Calendar Policy pointer. |
| `ordinal` | `int64_t ordinal` | Julian Day Number input. |
| `outDate` | `CHRONOMETRY_TYPE(Date) *outDate` | Writable Date output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Rejects out-of-range JDN and invalid dates; year bounds apply.

---

### Example

```c
CHRONOMETRY_TYPE(Date) date;
OPSTATUS s = CALENDAR_FUNC(DateFromOrdinal)(&policy, jdn, &date);
```

---

# Calendar DateDifference

Computes right minus left in elapsed days.

### Syntax

#### Macro form

```c
OPSTATUS CALENDAR_FUNC(DateDifference)(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) left, CHRONOMETRY_TYPE(Date) right, int64_t *outDays);
```

#### Direct form

```c
OPSTATUS Chronometry_Calendar_DateDifference(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) left, CHRONOMETRY_TYPE(Date) right, int64_t *outDays);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `policy` | `const CALENDAR_TYPE(Policy) *policy` | Calendar Policy pointer. |
| `left` | `CHRONOMETRY_TYPE(Date) left` | Left input. |
| `right` | `CHRONOMETRY_TYPE(Date) right` | Right input. |
| `outDays` | `int64_t *outDays` | Writable day difference/length output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Uses JDN for each input so policy cutovers are respected.

---

### Example

```c
int64_t days;
OPSTATUS s = CALENDAR_FUNC(DateDifference)(&policy, left, right, &days);
```

---

# DateTime package

Header: `Cosmeron/Modules/Chronometry/DateTime/DateTime.h`

Civil time represents milliseconds (0–999), not arbitrary nanoseconds. Very large durations/differences can overflow.

## Function summary

| Function | Description |
| --- | --- |
| [`TimeIsValid`](#datetime-timeisvalid) | Validates hour/minute/second/millisecond. |
| [`IsValid`](#datetime-isvalid) | Validates civil DateTime under policy. |
| [`Compare`](#datetime-compare) | Compares date-time fields in civil order. |
| [`AddDuration`](#datetime-addduration) | Adds Duration to civil DateTime under policy. |
| [`Difference`](#datetime-difference) | Computes right DateTime minus left as Duration. |

---

# DateTime TimeIsValid

Validates hour/minute/second/millisecond.

### Syntax

#### Macro form

```c
bool DATETIME_FUNC(TimeIsValid)(CHRONOMETRY_TYPE(TimeOfDay) time);
```

#### Direct form

```c
bool Chronometry_DateTime_TimeIsValid(CHRONOMETRY_TYPE(TimeOfDay) time);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `time` | `CHRONOMETRY_TYPE(TimeOfDay) time` | TimeOfDay value. |

---

### Return value

`bool`: true or false.

---

### Remarks

Accepts 00–23 hours, 00–59 minutes/seconds, 000–999 milliseconds. No leap second support.

---

### Example

```c
CHRONOMETRY_TYPE(TimeOfDay) time = {500, 0, 30, 12};
bool valid = DATETIME_FUNC(TimeIsValid)(time);
```

---

# DateTime IsValid

Validates civil DateTime under policy.

### Syntax

#### Macro form

```c
bool DATETIME_FUNC(IsValid)(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(DateTime) value);
```

#### Direct form

```c
bool Chronometry_DateTime_IsValid(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(DateTime) value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `policy` | `const CALENDAR_TYPE(Policy) *policy` | Calendar Policy pointer. |
| `value` | `CHRONOMETRY_TYPE(DateTime) value` | Value to convert/validate or output pointer. |

---

### Return value

`bool`: true or false.

---

### Remarks

Combines Calendar DateIsValid and TimeIsValid checks.

---

### Example

```c
bool valid = DATETIME_FUNC(IsValid)(&policy, dateTime);
```

---

# DateTime Compare

Compares date-time fields in civil order.

### Syntax

#### Macro form

```c
CMPOUT DATETIME_FUNC(Compare)(CHRONOMETRY_TYPE(DateTime) left, CHRONOMETRY_TYPE(DateTime) right);
```

#### Direct form

```c
CMPOUT Chronometry_DateTime_Compare(CHRONOMETRY_TYPE(DateTime) left, CHRONOMETRY_TYPE(DateTime) right);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `left` | `CHRONOMETRY_TYPE(DateTime) left` | Left input. |
| `right` | `CHRONOMETRY_TYPE(DateTime) right` | Right input. |

---

### Return value

`CMPOUT`: LOWER, EQUAL or HIGHER.

---

### Remarks

No timezone conversion is performed; compares date then hour/minute/second/millisecond.

---

### Example

```c
CMPOUT order = DATETIME_FUNC(Compare)(a, b);
```

---

# DateTime AddDuration

Adds Duration to civil DateTime under policy.

### Syntax

#### Macro form

```c
OPSTATUS DATETIME_FUNC(AddDuration)(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(DateTime) value, CHRONOMETRY_TYPE(Duration) duration, CHRONOMETRY_TYPE(DateTime) *outResult);
```

#### Direct form

```c
OPSTATUS Chronometry_DateTime_AddDuration(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(DateTime) value, CHRONOMETRY_TYPE(Duration) duration, CHRONOMETRY_TYPE(DateTime) *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `policy` | `const CALENDAR_TYPE(Policy) *policy` | Calendar Policy pointer. |
| `value` | `CHRONOMETRY_TYPE(DateTime) value` | Value to convert/validate or output pointer. |
| `duration` | `CHRONOMETRY_TYPE(Duration) duration` | Signed time interval. |
| `outResult` | `CHRONOMETRY_TYPE(DateTime) *outResult` | Writable result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Millisecond granularity: sub-millisecond nanoseconds are truncated toward zero; rolls across calendar days.

---

### Example

```c
CHRONOMETRY_TYPE(DateTime) next;
OPSTATUS s = DATETIME_FUNC(AddDuration)(&policy, dateTime, duration, &next);
```

---

# DateTime Difference

Computes right DateTime minus left as Duration.

### Syntax

#### Macro form

```c
OPSTATUS DATETIME_FUNC(Difference)(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(DateTime) left, CHRONOMETRY_TYPE(DateTime) right, CHRONOMETRY_TYPE(Duration) *outResult);
```

#### Direct form

```c
OPSTATUS Chronometry_DateTime_Difference(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(DateTime) left, CHRONOMETRY_TYPE(DateTime) right, CHRONOMETRY_TYPE(Duration) *outResult);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `policy` | `const CALENDAR_TYPE(Policy) *policy` | Calendar Policy pointer. |
| `left` | `CHRONOMETRY_TYPE(DateTime) left` | Left input. |
| `right` | `CHRONOMETRY_TYPE(DateTime) right` | Right input. |
| `outResult` | `CHRONOMETRY_TYPE(Duration) *outResult` | Writable result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Time values have millisecond precision. Multi-century differences can overflow int64 nanoseconds.

---

### Example

```c
CHRONOMETRY_TYPE(Duration) elapsed;
OPSTATUS s = DATETIME_FUNC(Difference)(&policy, first, second, &elapsed);
```

---

# Epoch package

Header: `Cosmeron/Modules/Chronometry/Epoch/Epoch.h`

Unix conversions use the **proleptic Gregorian calendar** and interpret civil DateTime as UTC.

## Function summary

| Function | Description |
| --- | --- |
| [`DateToJulianDayNumber`](#epoch-datetojuliandaynumber) | Converts date under policy to JDN. |
| [`DateFromJulianDayNumber`](#epoch-datefromjuliandaynumber) | Converts JDN to date using policy. |
| [`DateToRataDie`](#epoch-datetoratadie) | Converts Gregorian date to Rata Die count. |
| [`DateTimeToUnix`](#epoch-datetimetounix) | Converts Gregorian UTC DateTime to Unix Instant. |
| [`DateTimeFromUnix`](#epoch-datetimefromunix) | Converts Unix Instant to Gregorian UTC DateTime. |

---

# Epoch DateToJulianDayNumber

Converts date under policy to JDN.

### Syntax

#### Macro form

```c
OPSTATUS EPOCH_FUNC(DateToJulianDayNumber)(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date, int64_t *julianDayNumber);
```

#### Direct form

```c
OPSTATUS Chronometry_Epoch_DateToJulianDayNumber(const CALENDAR_TYPE(Policy) *policy, CHRONOMETRY_TYPE(Date) date, int64_t *julianDayNumber);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `policy` | `const CALENDAR_TYPE(Policy) *policy` | Calendar Policy pointer. |
| `date` | `CHRONOMETRY_TYPE(Date) date` | Civil date or result pointer. |
| `julianDayNumber` | `int64_t *julianDayNumber` | Julian Day Number input/output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Delegates to Calendar DateToOrdinal.

---

### Example

```c
int64_t jdn;
OPSTATUS s = EPOCH_FUNC(DateToJulianDayNumber)(&policy, date, &jdn);
```

---

# Epoch DateFromJulianDayNumber

Converts JDN to date using policy.

### Syntax

#### Macro form

```c
OPSTATUS EPOCH_FUNC(DateFromJulianDayNumber)(const CALENDAR_TYPE(Policy) *policy, int64_t julianDayNumber, CHRONOMETRY_TYPE(Date) *date);
```

#### Direct form

```c
OPSTATUS Chronometry_Epoch_DateFromJulianDayNumber(const CALENDAR_TYPE(Policy) *policy, int64_t julianDayNumber, CHRONOMETRY_TYPE(Date) *date);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `policy` | `const CALENDAR_TYPE(Policy) *policy` | Calendar Policy pointer. |
| `julianDayNumber` | `int64_t julianDayNumber` | Julian Day Number input/output. |
| `date` | `CHRONOMETRY_TYPE(Date) *date` | Civil date or result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Delegates to Calendar DateFromOrdinal.

---

### Example

```c
CHRONOMETRY_TYPE(Date) date;
OPSTATUS s = EPOCH_FUNC(DateFromJulianDayNumber)(&policy, jdn, &date);
```

---

# Epoch DateToRataDie

Converts Gregorian date to Rata Die count.

### Syntax

#### Macro form

```c
OPSTATUS EPOCH_FUNC(DateToRataDie)(CHRONOMETRY_TYPE(Date) date, int64_t *rataDie);
```

#### Direct form

```c
OPSTATUS Chronometry_Epoch_DateToRataDie(CHRONOMETRY_TYPE(Date) date, int64_t *rataDie);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `date` | `CHRONOMETRY_TYPE(Date) date` | Civil date or result pointer. |
| `rataDie` | `int64_t *rataDie` | Rata Die result. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Gregorian 0001-01-01 is day 1; this function does not take a calendar policy.

---

### Example

```c
int64_t rd;
OPSTATUS s = EPOCH_FUNC(DateToRataDie)((CHRONOMETRY_TYPE(Date)){2024, 1, 1}, &rd);
```

---

# Epoch DateTimeToUnix

Converts Gregorian UTC DateTime to Unix Instant.

### Syntax

#### Macro form

```c
OPSTATUS EPOCH_FUNC(DateTimeToUnix)(CHRONOMETRY_TYPE(DateTime) value, CHRONOMETRY_TYPE(Instant) *instant);
```

#### Direct form

```c
OPSTATUS Chronometry_Epoch_DateTimeToUnix(CHRONOMETRY_TYPE(DateTime) value, CHRONOMETRY_TYPE(Instant) *instant);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `CHRONOMETRY_TYPE(DateTime) value` | Value to convert/validate or output pointer. |
| `instant` | `CHRONOMETRY_TYPE(Instant) *instant` | Instant input/output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Interprets date as UTC, epoch origin 1970-01-01T00:00:00.000; milliseconds become nanoseconds.

---

### Example

```c
CHRONOMETRY_TYPE(Instant) t;
OPSTATUS s = EPOCH_FUNC(DateTimeToUnix)(dateTime, &t);
```

---

# Epoch DateTimeFromUnix

Converts Unix Instant to Gregorian UTC DateTime.

### Syntax

#### Macro form

```c
OPSTATUS EPOCH_FUNC(DateTimeFromUnix)(CHRONOMETRY_TYPE(Instant) instant, CHRONOMETRY_TYPE(DateTime) *value);
```

#### Direct form

```c
OPSTATUS Chronometry_Epoch_DateTimeFromUnix(CHRONOMETRY_TYPE(Instant) instant, CHRONOMETRY_TYPE(DateTime) *value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `instant` | `CHRONOMETRY_TYPE(Instant) instant` | Instant input/output. |
| `value` | `CHRONOMETRY_TYPE(DateTime) *value` | Value to convert/validate or output pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Rejects nanoseconds >=1e9; discards sub-millisecond fractions.

---

### Example

```c
CHRONOMETRY_TYPE(DateTime) value;
OPSTATUS s = EPOCH_FUNC(DateTimeFromUnix)(t, &value);
```

---

# Timezone package

Header: `Cosmeron/Modules/Chronometry/Timezone/Timezone.h`

All operations use fixed offsets in minutes; no zone names, seasonal transitions or DST lookup.

## Function summary

| Function | Description |
| --- | --- |
| [`OffsetIsValid`](#timezone-offsetisvalid) | Checks validity of minutes-east-of-UTC offset. |
| [`ToUTC`](#timezone-toutc) | Converts a fixed-offset local DateTime to UTC Instant. |
| [`FromUTC`](#timezone-fromutc) | Converts UTC Instant into fixed-offset local DateTime. |

---

# Timezone OffsetIsValid

Checks validity of minutes-east-of-UTC offset.

### Syntax

#### Macro form

```c
bool TIMEZONE_FUNC(OffsetIsValid)(CHRONOMETRY_TYPE(UtcOffset) offset);
```

#### Direct form

```c
bool Chronometry_Timezone_OffsetIsValid(CHRONOMETRY_TYPE(UtcOffset) offset);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `offset` | `CHRONOMETRY_TYPE(UtcOffset) offset` | UTC offset in minutes. |

---

### Return value

`bool`: true or false.

---

### Remarks

Accepts -1439 to 1439 minutes. No named timezone database.

---

### Example

```c
CHRONOMETRY_TYPE(UtcOffset) offset = {-180};
bool valid = TIMEZONE_FUNC(OffsetIsValid)(offset);
```

---

# Timezone ToUTC

Converts a fixed-offset local DateTime to UTC Instant.

### Syntax

#### Macro form

```c
OPSTATUS TIMEZONE_FUNC(ToUTC)(CHRONOMETRY_TYPE(OffsetDateTime) value, CHRONOMETRY_TYPE(Instant) *instant);
```

#### Direct form

```c
OPSTATUS Chronometry_Timezone_ToUTC(CHRONOMETRY_TYPE(OffsetDateTime) value, CHRONOMETRY_TYPE(Instant) *instant);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `CHRONOMETRY_TYPE(OffsetDateTime) value` | Value to convert/validate or output pointer. |
| `instant` | `CHRONOMETRY_TYPE(Instant) *instant` | Instant input/output. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Subtracts minutesEastOfUtc; DST Boolean is informational and not separately applied.

---

### Example

```c
CHRONOMETRY_TYPE(Instant) t;
OPSTATUS s = TIMEZONE_FUNC(ToUTC)(local, &t);
```

---

# Timezone FromUTC

Converts UTC Instant into fixed-offset local DateTime.

### Syntax

#### Macro form

```c
OPSTATUS TIMEZONE_FUNC(FromUTC)(CHRONOMETRY_TYPE(Instant) instant, CHRONOMETRY_TYPE(UtcOffset) offset, CHRONOMETRY_TYPE(OffsetDateTime) *value);
```

#### Direct form

```c
OPSTATUS Chronometry_Timezone_FromUTC(CHRONOMETRY_TYPE(Instant) instant, CHRONOMETRY_TYPE(UtcOffset) offset, CHRONOMETRY_TYPE(OffsetDateTime) *value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `instant` | `CHRONOMETRY_TYPE(Instant) instant` | Instant input/output. |
| `offset` | `CHRONOMETRY_TYPE(UtcOffset) offset` | UTC offset in minutes. |
| `value` | `CHRONOMETRY_TYPE(OffsetDateTime) *value` | Value to convert/validate or output pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Adds minutesEastOfUtc; sets daylightSavings false, with no timezone database.

---

### Example

```c
CHRONOMETRY_TYPE(OffsetDateTime) local;
OPSTATUS s = TIMEZONE_FUNC(FromUTC)(t, offset, &local);
```

---

# Format package

Header: `Cosmeron/Modules/Chronometry/Format/Format.h`

ISO-like formatters emit a **leading sign** on the year (e.g., +2024-02-29). Parsers use sscanf and accept some noncanonical widths; not a strict full ISO 8601 implementation.

## Function summary

| Function | Description |
| --- | --- |
| [`DateISO`](#format-dateiso) | Formats Gregorian date into a signed-year ISO-like string. |
| [`DateTimeISO`](#format-datetimeiso) | Formats Gregorian date and millisecond time. |
| [`ParseDateISO`](#format-parsedateiso) | Parses Gregorian date from text. |
| [`ParseDateTimeISO`](#format-parsedatetimeiso) | Parses Gregorian date and millisecond time from text. |

---

# Format DateISO

Formats Gregorian date into a signed-year ISO-like string.

### Syntax

#### Macro form

```c
OPSTATUS CHRONOMETRY_FORMAT_FUNC(DateISO)(CHRONOMETRY_TYPE(Date) date, char *buffer, size_t capacity);
```

#### Direct form

```c
OPSTATUS Chronometry_Format_DateISO(CHRONOMETRY_TYPE(Date) date, char *buffer, size_t capacity);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `date` | `CHRONOMETRY_TYPE(Date) date` | Civil date or result pointer. |
| `buffer` | `char *buffer` | Destination character array. |
| `capacity` | `size_t capacity` | Destination buffer size including NUL. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Example +2024-02-29; buffer must include NUL. On validation/space failure, no partial buffer write.

---

### Example

```c
char text[32];
OPSTATUS s = CHRONOMETRY_FORMAT_FUNC(DateISO)(date, text, sizeof text);
```

---

# Format DateTimeISO

Formats Gregorian date and millisecond time.

### Syntax

#### Macro form

```c
OPSTATUS CHRONOMETRY_FORMAT_FUNC(DateTimeISO)(CHRONOMETRY_TYPE(DateTime) value, char *buffer, size_t capacity);
```

#### Direct form

```c
OPSTATUS Chronometry_Format_DateTimeISO(CHRONOMETRY_TYPE(DateTime) value, char *buffer, size_t capacity);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `value` | `CHRONOMETRY_TYPE(DateTime) value` | Value to convert/validate or output pointer. |
| `buffer` | `char *buffer` | Destination character array. |
| `capacity` | `size_t capacity` | Destination buffer size including NUL. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Example +2024-02-29T12:30:00.500; no timezone suffix. No partial write on failure.

---

### Example

```c
char text[40];
OPSTATUS s = CHRONOMETRY_FORMAT_FUNC(DateTimeISO)(dateTime, text, sizeof text);
```

---

# Format ParseDateISO

Parses Gregorian date from text.

### Syntax

#### Macro form

```c
OPSTATUS CHRONOMETRY_FORMAT_FUNC(ParseDateISO)(const char *text, CHRONOMETRY_TYPE(Date) *date);
```

#### Direct form

```c
OPSTATUS Chronometry_Format_ParseDateISO(const char *text, CHRONOMETRY_TYPE(Date) *date);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `text` | `const char *text` | Source NUL-terminated text string. |
| `date` | `CHRONOMETRY_TYPE(Date) *date` | Civil date or result pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Rejects invalid date/trailing content. sscanf-based parser permits noncanonical digit widths/signs; not strict ISO8601.

---

### Example

```c
CHRONOMETRY_TYPE(Date) date;
OPSTATUS s = CHRONOMETRY_FORMAT_FUNC(ParseDateISO)("2024-02-29", &date);
```

---

# Format ParseDateTimeISO

Parses Gregorian date and millisecond time from text.

### Syntax

#### Macro form

```c
OPSTATUS CHRONOMETRY_FORMAT_FUNC(ParseDateTimeISO)(const char *text, CHRONOMETRY_TYPE(DateTime) *value);
```

#### Direct form

```c
OPSTATUS Chronometry_Format_ParseDateTimeISO(const char *text, CHRONOMETRY_TYPE(DateTime) *value);
```

---

### Parameters

| Parameter | C type | Explanation |
| --- | --- | --- |
| `text` | `const char *text` | Source NUL-terminated text string. |
| `value` | `CHRONOMETRY_TYPE(DateTime) *value` | Value to convert/validate or output pointer. |

---

### Return value

`OPSTATUS`: SUCCESS or an operational failure code; see remarks.

---

### Remarks

Requires T between date/time and dot before integer millisecond component. Rejects invalid date/time or suffix. No timezone parsing.

---

### Example

```c
CHRONOMETRY_TYPE(DateTime) dateTime;
OPSTATUS s = CHRONOMETRY_FORMAT_FUNC(ParseDateTimeISO)("2024-02-29T12:30:00.500", &dateTime);
```

---

# Complete examples

## Gregorian leap-day validation

```c
#include "Cosmeron/Modules/Chronometry/Chronometry.h"

int main(void) {
    CALENDAR_TYPE(Policy) gregorian;
    CHRONOMETRY_TYPE(Date) date = {2024, 2, 29};
    uint16_t dayOfYear = 0;
    if (CALENDAR_FUNC(SystemPolicy)(CALENDAR_CONST(GREGORIAN), &gregorian)
        != STATUS_CONST(SUCCESS))
        return 1;
    if (!CALENDAR_FUNC(DateIsValid)(&gregorian, date))
        return 2;
    if (CALENDAR_FUNC(DateToDayOfYear)(&gregorian, date, &dayOfYear)
        != STATUS_CONST(SUCCESS))
        return 3;
    return dayOfYear == 60 ? 0 : 4;
}
```

## Monotonic stopwatch and frame budget

```c
#include "Cosmeron/Modules/Chronometry/Chronometry.h"

int main(void) {
    CHRONOMETRY_TYPE(Timer) stopwatch;
    CHRONOMETRY_TYPE(Duration) elapsed;
    if (TIMER_FUNC(Start)(&stopwatch) != STATUS_CONST(SUCCESS))
        return 1;
    /* Perform work. */
    if (TIMER_FUNC(Elapsed)(&stopwatch, &elapsed) != STATUS_CONST(SUCCESS))
        return 2;

    CHRONOMETRY_TYPE(FrameLimiter) limiter;
    if (TIMER_FUNC(FrameLimiterFromFPS)(60, &limiter) != STATUS_CONST(SUCCESS))
        return 3;
    if (TIMER_FUNC(FrameLimiterBegin)(&limiter) != STATUS_CONST(SUCCESS))
        return 4;
    CHRONOMETRY_TYPE(Duration) remaining;
    if (TIMER_FUNC(FrameLimiterRemaining)(&limiter, &remaining)
        != STATUS_CONST(SUCCESS))
        return 5;
    /* Remaining is a measurement; no automatic sleeping happens. */
    return 0;
}
```

## UTC instant and a fixed offset

```c
#include "Cosmeron/Modules/Chronometry/Chronometry.h"

int main(void) {
    CHRONOMETRY_TYPE(DateTime) utc = {
        {1970, 1, 1}, {0, 0, 0, 0}
    };
    CHRONOMETRY_TYPE(Instant) instant;
    if (EPOCH_FUNC(DateTimeToUnix)(utc, &instant) != STATUS_CONST(SUCCESS))
        return 1;
    CHRONOMETRY_TYPE(UtcOffset) offset = {-180};
    CHRONOMETRY_TYPE(OffsetDateTime) local;
    if (TIMEZONE_FUNC(FromUTC)(instant, offset, &local) != STATUS_CONST(SUCCESS))
        return 2;
    /* local.local is 1969-12-31 21:00 in UTC-03:00. */
    return 0;
}
```

---

# Portability and limits

On Windows, Clock uses QueryPerformanceCounter; on macOS, Mach absolute time; in the other implementation branch, `clock_gettime(CLOCK_MONOTONIC)`. The test Makefile uses C11 with `-D_POSIX_C_SOURCE=200809L` and no extra link libraries. Platform-specific clock availability must still be checked.

A Gregorian civil DateTime has only millisecond precision. Converting an Instant to DateTime discards sub-millisecond nanoseconds, and adding a nanosecond Duration to DateTime truncates fractions below one millisecond. The Duration type cannot cover arbitrarily large historical intervals as a single nanosecond count.

Chronometry does not yet expose a live civil wall-clock `Now`, sleep function, named IANA timezone lookup, automatic daylight-saving rules, leap-second handling, or arbitrary custom calendar reform construction. The three predefined reforms are implemented.

---

# Notes

- All 47 function declarations were checked against the package header prototype macros, not inferred from filenames or stale documentation.
- JDN (absolute day count) is distinct from day-of-year; reform skipped dates affect civil numbering.
- Use Clock/Timer for monotonic measurements and Epoch/Timezone for civil-to-Unix conversion.
- Macro names respect configured namespaces. Direct examples assume default names.
- `.impl` source is included by headers, consistent with the library's C11 header-only design.
