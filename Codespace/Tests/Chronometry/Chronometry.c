#include "../../Cosmeron/Modules/Chronometry/Chronometry.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static void TestDuration(void) {
  MODULE_TYPE(Chronometry, Duration) d1 =
      MODULE_FUNC(Chronometry, Duration, FromNanoseconds)(500);
  assert(d1.nanoseconds == 500);

  MODULE_TYPE(Chronometry, Duration) d2 = {0};
  assert(MODULE_CALL(Chronometry, Duration, FromSeconds, (3, &d2)) ==
         STATUS_CONST(SUCCESS));
  assert(d2.nanoseconds ==
         3LL * MODULE_CONST(Chronometry, Duration, NANOSECONDS_PER_SECOND));

  MODULE_TYPE(Chronometry, Duration) dMicro = {0};
  assert(MODULE_FUNC(Chronometry, Duration, FromMicroseconds)(5, &dMicro) ==
         STATUS_CONST(SUCCESS));
  assert(dMicro.nanoseconds ==
         5LL * MODULE_CONST(Chronometry, Duration, NANOSECONDS_PER_MICROSECOND));

  MODULE_TYPE(Chronometry, Duration) dMilli = {0};
  assert(MODULE_FUNC(Chronometry, Duration, FromMilliseconds)(7, &dMilli) ==
         STATUS_CONST(SUCCESS));
  assert(dMilli.nanoseconds ==
         7LL * MODULE_CONST(Chronometry, Duration, NANOSECONDS_PER_MILLISECOND));

  assert(MODULE_FUNC(Chronometry, Duration, FromSeconds)(1, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Chronometry, Duration, FromSeconds)(INT64_MAX, &d2) ==
         STATUS_CONST(ARITHMETIC_OVERFLOW));

  MODULE_TYPE(Chronometry, Duration) sum = {0};
  assert(MODULE_FUNC(Chronometry, Duration, Add)(d1, d2, &sum) ==
         STATUS_CONST(SUCCESS));
  assert(sum.nanoseconds == d2.nanoseconds + 500);

  MODULE_TYPE(Chronometry, Duration) diff = {0};
  assert(MODULE_FUNC(Chronometry, Duration, Subtract)(d1, d2, &diff) ==
         STATUS_CONST(SUCCESS));
  assert(diff.nanoseconds == 500 - d2.nanoseconds);

  assert(MODULE_FUNC(Chronometry, Duration, Compare)(d1, d2) ==
         COMPARISON_CONST(LOWER));
  assert(MODULE_FUNC(Chronometry, Duration, Compare)(d2, d1) ==
         COMPARISON_CONST(HIGHER));
  assert(MODULE_FUNC(Chronometry, Duration, Compare)(d1, d1) ==
         COMPARISON_CONST(EQUAL));

  MODULE_TYPE(Chronometry, Duration) absVal = {0};
  assert(MODULE_FUNC(Chronometry, Duration, Absolute)(diff, &absVal) ==
         STATUS_CONST(SUCCESS));
  assert(absVal.nanoseconds == d2.nanoseconds - 500);

  MODULE_TYPE(Chronometry, Duration) dMin = {0};
  MODULE_TYPE(Chronometry, Duration) dHour = {0};
  MODULE_TYPE(Chronometry, Duration) dDay = {0};
  assert(MODULE_FUNC(Chronometry, Duration, FromMinutes)(2, &dMin) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Duration, ToSeconds)(dMin) == 120);
  assert(MODULE_FUNC(Chronometry, Duration, FromHours)(3, &dHour) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Duration, ToMilliseconds)(dHour) ==
         10800000LL);
  assert(MODULE_FUNC(Chronometry, Duration, FromDays)(1, &dDay) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Duration, ToMicroseconds)(dDay) ==
         86400000000LL);
  assert(MODULE_FUNC(Chronometry, Duration, ToNanoseconds)(d1) == 500);

  MODULE_TYPE(Chronometry, Duration) zero =
      MODULE_FUNC(Chronometry, Duration, FromNanoseconds)(0);
  assert(MODULE_FUNC(Chronometry, Duration, IsZero)(zero));
  assert(!MODULE_FUNC(Chronometry, Duration, IsNegative)(zero));
  assert(MODULE_FUNC(Chronometry, Duration, IsNegative)(diff));

  MODULE_TYPE(Chronometry, Duration) neg = {0};
  assert(MODULE_FUNC(Chronometry, Duration, Negate)(d2, &neg) ==
         STATUS_CONST(SUCCESS));
  assert(neg.nanoseconds == -d2.nanoseconds);

  MODULE_TYPE(Chronometry, Duration) scaled = {0};
  assert(MODULE_FUNC(Chronometry, Duration, Scale)(d1, -4, &scaled) ==
         STATUS_CONST(SUCCESS));
  assert(scaled.nanoseconds == -2000);
}

static void TestClockAndTimer(void) {
  MODULE_TYPE(Chronometry, ClockTimePoint) t1 = {0};
  MODULE_TYPE(Chronometry, ClockTimePoint) t2 = {0};
  assert(MODULE_FUNC(Chronometry, Clock, Now)(&t1) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Clock, Now)(&t2) == STATUS_CONST(SUCCESS));

  MODULE_TYPE(Chronometry, Duration) between = {0};
  assert(MODULE_FUNC(Chronometry, Clock, DurationBetween)(t1, t2, &between) ==
         STATUS_CONST(SUCCESS));
  assert(between.nanoseconds >= 0);

  MODULE_TYPE(Chronometry, Timer) timer = {0};
  MODULE_TYPE(Chronometry, Duration) elapsed = {0};
  assert(MODULE_CALL(Chronometry, Timer, Start, (&timer)) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Timer, Elapsed)(&timer, &elapsed) ==
         STATUS_CONST(SUCCESS));
  assert(elapsed.nanoseconds >= 0);
  assert(MODULE_FUNC(Chronometry, Timer, Restart)(&timer, &elapsed) ==
         STATUS_CONST(SUCCESS));

  MODULE_TYPE(Chronometry, Delta) delta = {0};
  assert(MODULE_CALL(Chronometry, Timer, Delta, Start, (&delta)) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Timer, Delta, Update)(&delta, &elapsed) ==
         STATUS_CONST(SUCCESS));

  MODULE_TYPE(Chronometry, FrameLimiter) limiter = {0};
  assert(MODULE_FUNC(Chronometry, Timer, FrameLimiter, FromFPS)(
             60U, &limiter) == STATUS_CONST(SUCCESS));
  MODULE_TYPE(Chronometry, Duration) remaining = {0};
  assert(MODULE_FUNC(Chronometry, Timer, FrameLimiter, Remaining)(
             &limiter, &remaining) == STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Chronometry, Timer, FrameLimiter, Begin)(&limiter) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Timer, FrameLimiter, Remaining)(
             &limiter, &remaining) == STATUS_CONST(SUCCESS));
  assert(remaining.nanoseconds >= 0);

  MODULE_TYPE(Chronometry, Duration) shortSleep = {0};
  assert(MODULE_FUNC(Chronometry, Duration, FromMilliseconds)(1, &shortSleep) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Clock, Sleep)(shortSleep) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Timer, FrameLimiter, Wait)(&limiter) ==
         STATUS_CONST(SUCCESS));
}

static void TestInstant(void) {
  MODULE_TYPE(Chronometry, Instant) a = {10, 500000000U};
  MODULE_TYPE(Chronometry, Instant) b = {10, 600000000U};
  MODULE_TYPE(Chronometry, Instant) invalidInst = {
      0, (uint32_t)MODULE_CONST(Chronometry, Duration, NANOSECONDS_PER_SECOND)};
  assert(MODULE_FUNC(Chronometry, Instant, IsValid)(a));
  assert(!MODULE_FUNC(Chronometry, Instant, IsValid)(invalidInst));
  assert(MODULE_FUNC(Chronometry, Instant, Compare)(a, b) ==
         COMPARISON_CONST(LOWER));

  MODULE_TYPE(Chronometry, Duration) delta =
      MODULE_FUNC(Chronometry, Duration, FromNanoseconds)(600000000LL);
  MODULE_TYPE(Chronometry, Instant) c = {0};
  assert(MODULE_FUNC(Chronometry, Instant, AddDuration)(a, delta, &c) ==
         STATUS_CONST(SUCCESS));
  assert(c.seconds == 11 && c.nanoseconds == 100000000U);

  MODULE_TYPE(Chronometry, Duration) measured = {0};
  assert(MODULE_FUNC(Chronometry, Instant, DurationBetween)(a, c, &measured) ==
         STATUS_CONST(SUCCESS));
  assert(measured.nanoseconds == 600000000LL);

  MODULE_TYPE(Chronometry, Instant) wallNow = {0};
  assert(MODULE_FUNC(Chronometry, Instant, Now)(&wallNow) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Instant, IsValid)(wallNow));
  assert(wallNow.seconds > 0);
}

static void TestCalendarDateTimeEpochTimezoneFormat(void) {
  MODULE_TYPE(Chronometry, CalendarPolicy) gregorian = {0};
  assert(MODULE_FUNC(Chronometry, Calendar, SystemPolicy)(
             MODULE_CONST(Chronometry, Calendar, GREGORIAN), &gregorian) ==
         STATUS_CONST(SUCCESS));

  assert(MODULE_FUNC(Chronometry, Calendar, IsLeapYear)(
      MODULE_CONST(Chronometry, Calendar, GREGORIAN), 2024));
  assert(!MODULE_FUNC(Chronometry, Calendar, IsLeapYear)(
      MODULE_CONST(Chronometry, Calendar, GREGORIAN), 1900));
  assert(MODULE_FUNC(Chronometry, Calendar, IsLeapYear)(
      MODULE_CONST(Chronometry, Calendar, JULIAN), 1900));

  uint8_t days = 0U;
  assert(MODULE_FUNC(Chronometry, Calendar, DaysInMonth)(
             MODULE_CONST(Chronometry, Calendar, GREGORIAN), 2024,
             MODULE_CONST(Chronometry, Calendar, FEBRUARY), &days) ==
         STATUS_CONST(SUCCESS));
  assert(days == 29U);

  MODULE_TYPE(Chronometry, CalendarPolicy) catholic = {0};
  assert(MODULE_FUNC(Chronometry, Calendar, ReformPolicy)(
             MODULE_CONST(Chronometry, Calendar, CATHOLIC_1582), &catholic) ==
         STATUS_CONST(SUCCESS));
  MODULE_TYPE(Chronometry, Date) suppressed = {
      1582, MODULE_CONST(Chronometry, Calendar, OCTOBER), 10};
  assert(!MODULE_FUNC(Chronometry, Calendar, Date, IsValid)(&catholic,
                                                            suppressed));

  uint16_t doy = 0U;
  MODULE_TYPE(Chronometry, Date) leapDay = {
      2024, MODULE_CONST(Chronometry, Calendar, MARCH), 1};
  assert(MODULE_CALL(Chronometry, Calendar, Date, ToDayOfYear,
                     (&gregorian, leapDay, &doy)) == STATUS_CONST(SUCCESS));
  assert(doy == 61U);
  assert(MODULE_FUNC(Chronometry, Calendar, DayOfYear)(
             &gregorian, leapDay, &doy) == STATUS_CONST(SUCCESS));
  assert(doy == 61U);
  assert(MODULE_FUNC(Chronometry, Calendar, DayOfYearCumulative)(
             MODULE_CONST(Chronometry, Calendar, GREGORIAN), leapDay, &doy) ==
         STATUS_CONST(SUCCESS));
  assert(doy == 61U);

  MODULE_TYPE(Chronometry, Date) afterReform = {
      1582, MODULE_CONST(Chronometry, Calendar, OCTOBER), 15};
  assert(MODULE_FUNC(Chronometry, Calendar, Date, ToDayOfYear)(
             &catholic, afterReform, &doy) == STATUS_CONST(SUCCESS));
  assert(doy == 278U);

  MODULE_TYPE(Chronometry, Date) unixEpoch = {
      1970, MODULE_CONST(Chronometry, Calendar, JANUARY), 1};
  MODULE_ENUM(Chronometry, Calendar, Weekday) weekday = 0;
  assert(MODULE_FUNC(Chronometry, Calendar, Date, DayOfWeek)(
             &gregorian, unixEpoch, &weekday) == STATUS_CONST(SUCCESS));
  assert(weekday == MODULE_CONST(Chronometry, Calendar, THURSDAY));

  MODULE_TYPE(Chronometry, Date) secondSundayMarch2026 = {0};
  assert(MODULE_FUNC(Chronometry, Calendar, Date, FromNthWeekday)(
             MODULE_CONST(Chronometry, Calendar, GREGORIAN), 2026,
             MODULE_CONST(Chronometry, Calendar, MARCH),
             MODULE_CONST(Chronometry, Calendar, SECOND_WEEKDAY),
             MODULE_CONST(Chronometry, Calendar, SUNDAY),
             &secondSundayMarch2026) == STATUS_CONST(SUCCESS));
  assert(secondSundayMarch2026.year == 2026 &&
         secondSundayMarch2026.month == 3 && secondSundayMarch2026.day == 8);

  MODULE_TYPE(Chronometry, Date) addedDate = {0};
  assert(MODULE_FUNC(Chronometry, Calendar, Date, AddDays)(
             &gregorian, (MODULE_TYPE(Chronometry, Date)){2024, 2, 28}, 2,
             &addedDate) == STATUS_CONST(SUCCESS));
  assert(addedDate.year == 2024 && addedDate.month == 3 && addedDate.day == 1);

  assert(MODULE_FUNC(Chronometry, Calendar, Date, AddMonths)(
             &gregorian, (MODULE_TYPE(Chronometry, Date)){2024, 1, 31}, 1,
             &addedDate) == STATUS_CONST(SUCCESS));
  assert(addedDate.year == 2024 && addedDate.month == 2 &&
         addedDate.day == 29);

  assert(MODULE_FUNC(Chronometry, Calendar, Date, AddYears)(
             &gregorian, (MODULE_TYPE(Chronometry, Date)){2024, 2, 29}, 1,
             &addedDate) == STATUS_CONST(SUCCESS));
  assert(addedDate.year == 2025 && addedDate.month == 2 &&
         addedDate.day == 28);

  MODULE_TYPE(Chronometry, IsoWeekDate) isoWeek = {0};
  assert(MODULE_FUNC(Chronometry, Calendar, Date, ToIsoWeek)(
             &gregorian, (MODULE_TYPE(Chronometry, Date)){2021, 1, 1},
             &isoWeek) == STATUS_CONST(SUCCESS));
  assert(isoWeek.year == 2020 && isoWeek.week == 53 &&
         isoWeek.weekday == MODULE_CONST(Chronometry, Calendar, FRIDAY));
  assert(MODULE_FUNC(Chronometry, Calendar, Date, FromIsoWeek)(
             &gregorian, isoWeek, &addedDate) == STATUS_CONST(SUCCESS));
  assert(addedDate.year == 2021 && addedDate.month == 1 && addedDate.day == 1);
  assert(MODULE_FUNC(Chronometry, Calendar, Date, FromIsoWeek)(
             &gregorian,
             (MODULE_TYPE(Chronometry, IsoWeekDate)){2021, 53, 1},
             &addedDate) == STATUS_CONST(OUT_OF_RANGE));

  MODULE_TYPE(Chronometry, Timestamp) dt = {
      .date = unixEpoch,
      .time = {.hour = 0, .minute = 0, .second = 0, .millisecond = 0}};
  assert(MODULE_FUNC(Chronometry, Timestamp, IsValid)(&gregorian, dt));
  assert(MODULE_CALL(Chronometry, Timestamp, Time, IsValid, (dt.time)));

  MODULE_TYPE(Chronometry, TimeOfDay) laterTime = {
      .hour = 1, .minute = 0, .second = 0, .millisecond = 0};
  assert(MODULE_FUNC(Chronometry, Timestamp, Time, Compare)(
             dt.time, laterTime) == COMPARISON_CONST(LOWER));

  MODULE_TYPE(Chronometry, Duration) minusOneHour =
      MODULE_FUNC(Chronometry, Duration, FromNanoseconds)(
          -3600LL * MODULE_CONST(Chronometry, Duration, NANOSECONDS_PER_SECOND));
  MODULE_TYPE(Chronometry, Timestamp) prevDay = {0};
  assert(MODULE_FUNC(Chronometry, Timestamp, AddDuration)(
             &gregorian, dt, minusOneHour, &prevDay) == STATUS_CONST(SUCCESS));
  assert(prevDay.date.year == 1969 && prevDay.date.month == 12 &&
         prevDay.date.day == 31 && prevDay.time.hour == 23);

  MODULE_TYPE(Chronometry, Duration) diffDt = {0};
  assert(MODULE_FUNC(Chronometry, Timestamp, Difference)(
             &gregorian, prevDay, dt, &diffDt) == STATUS_CONST(SUCCESS));
  assert(diffDt.nanoseconds ==
         3600LL * MODULE_CONST(Chronometry, Duration, NANOSECONDS_PER_SECOND));

  MODULE_TYPE(Chronometry, Instant) inst = {0};
  assert(MODULE_FUNC(Chronometry, Epoch, TimestampToUnix)(dt, &inst) ==
         STATUS_CONST(SUCCESS));
  assert(inst.seconds == 0 && inst.nanoseconds == 0U);

  int64_t jdn = 0;
  assert(MODULE_CALL(Chronometry, Epoch, DateToJulianDayNumber,
                     (&gregorian, unixEpoch, &jdn)) == STATUS_CONST(SUCCESS));
  assert(jdn == 2440588);

  MODULE_TYPE(Chronometry, Date) fromJdn = {0};
  assert(MODULE_FUNC(Chronometry, Epoch, DateFromJulianDayNumber)(
             &gregorian, jdn, &fromJdn) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Calendar, Date, Compare)(
             unixEpoch, fromJdn) == COMPARISON_CONST(EQUAL));

  int64_t rataDie = 0;
  assert(MODULE_FUNC(Chronometry, Epoch, DateToRataDie)(
             unixEpoch, &rataDie) == STATUS_CONST(SUCCESS));
  assert(rataDie == 719163);

  MODULE_TYPE(Chronometry, Date) fromRataDie = {0};
  assert(MODULE_FUNC(Chronometry, Epoch, DateFromRataDie)(
             rataDie, &fromRataDie) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Calendar, Date, Compare)(
             unixEpoch, fromRataDie) == COMPARISON_CONST(EQUAL));

  MODULE_TYPE(Chronometry, Timestamp) roundTrip = {0};
  assert(MODULE_FUNC(Chronometry, Epoch, TimestampFromUnix)(inst, &roundTrip) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Timestamp, Compare)(dt, roundTrip) ==
         COMPARISON_CONST(EQUAL));

  MODULE_TYPE(Chronometry, UtcOffset) utcMinus3 = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, Offset, FromHoursMinutes)(
             -3, 0, &utcMinus3) == STATUS_CONST(SUCCESS));
  assert(utcMinus3.minutesEastOfUtc == -180);
  MODULE_TYPE(Chronometry, UtcOffset) invalidOffset = {1500};
  assert(MODULE_CALL(Chronometry, Timezone, Offset, IsValid, (utcMinus3)));
  assert(!MODULE_FUNC(Chronometry, Timezone, Offset, IsValid)(invalidOffset));
  MODULE_TYPE(Chronometry, OffsetTimestamp) odt = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, FromUTC)(inst, utcMinus3, &odt) ==
         STATUS_CONST(SUCCESS));
  assert(odt.local.date.year == 1969 && odt.local.date.month == 12 &&
         odt.local.date.day == 31 && odt.local.time.hour == 21);

  MODULE_TYPE(Chronometry, Instant) backToUtc = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, ToUTC)(odt, &backToUtc) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Instant, Compare)(inst, backToUtc) ==
         COMPARISON_CONST(EQUAL));

  /* DST / IANA Zone tests */
  MODULE_TYPE(Chronometry, TimezoneRule) nyRule = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, FromName)(
             "America/New_York", &nyRule) == STATUS_CONST(SUCCESS));
  assert(nyRule.standardOffset.minutesEastOfUtc == -300);
  assert(nyRule.dstSavingsMinutes == 60);

  MODULE_TYPE(Chronometry, Timestamp) nySummerTs = {
      .date = {2026, 7, 15}, .time = {.hour = 12}};
  MODULE_TYPE(Chronometry, OffsetTimestamp) nySummerResolved = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, ResolveLocal)(
             &nyRule, nySummerTs, &nySummerResolved) == STATUS_CONST(SUCCESS));
  assert(nySummerResolved.daylightSavings == true);
  assert(nySummerResolved.offset.minutesEastOfUtc == -240);

  MODULE_TYPE(Chronometry, Instant) nySummerUtc = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, ToUTC)(
             nySummerResolved, &nySummerUtc) == STATUS_CONST(SUCCESS));
  MODULE_TYPE(Chronometry, OffsetTimestamp) nyRoundTrip = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, FromUTCWithZone)(
             nySummerUtc,
             MODULE_CONST(Chronometry, Timezone, AMERICA_NEW_YORK),
             &nyRoundTrip) == STATUS_CONST(SUCCESS));
  assert(nyRoundTrip.daylightSavings == true);
  assert(nyRoundTrip.offset.minutesEastOfUtc == -240);
  assert(MODULE_FUNC(Chronometry, Timestamp, Compare)(
             nyRoundTrip.local, nySummerTs) == COMPARISON_CONST(EQUAL));

  /* Southern Hemisphere DST (Australia/Sydney: Oct -> Apr) */
  MODULE_TYPE(Chronometry, TimezoneRule) sydneyRule = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, FromId)(
             MODULE_CONST(Chronometry, Timezone, AUSTRALIA_SYDNEY),
             &sydneyRule) == STATUS_CONST(SUCCESS));
  bool sydneyDstJan = false;
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, IsDstAtLocal)(
             &sydneyRule,
             (MODULE_TYPE(Chronometry, Timestamp)){.date = {2026, 1, 15}},
             &sydneyDstJan) == STATUS_CONST(SUCCESS));
  assert(sydneyDstJan == true);

  /* Africa/Cairo: lastThu at 24:00 (normalized to next day 00:00) */
  MODULE_TYPE(Chronometry, TimezoneRule) cairoRule = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, FromName)(
             "Africa/Cairo", &cairoRule) == STATUS_CONST(SUCCESS));
  bool cairoDstJul = false;
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, IsDstAtLocal)(
             &cairoRule,
             (MODULE_TYPE(Chronometry, Timestamp)){.date = {2026, 7, 15}},
             &cairoDstJul) == STATUS_CONST(SUCCESS));
  assert(cairoDstJul == true);

  /* Europe/London: UTC-based transition (1:00u) */
  MODULE_TYPE(Chronometry, TimezoneRule) londonRule = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, FromName)(
             "Europe/London", &londonRule) == STATUS_CONST(SUCCESS));
  assert(londonRule.start.timeBase ==
         MODULE_CONST(Chronometry, Timezone, TIME_BASE_UTC));

  /* tzdata version & Link aliases (backward / etcetera) */
  assert(strcmp(MODULE_FUNC(Chronometry, Timezone, Zone, DataVersion)(),
                "2026f") == 0);
  MODULE_TYPE(Chronometry, TimezoneRule) aliasRule = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, FromName)(
             "Brazil/East", &aliasRule) == STATUS_CONST(SUCCESS));
  assert(strcmp(aliasRule.ianaName, "America/Sao_Paulo") == 0);
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, FromName)(
             "UTC", &aliasRule) == STATUS_CONST(SUCCESS));
  assert(aliasRule.standardOffset.minutesEastOfUtc == 0);
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, FromName)(
             "Etc/GMT+3", &aliasRule) == STATUS_CONST(SUCCESS));
  assert(aliasRule.standardOffset.minutesEastOfUtc == -180);

  /* Leap seconds & TAI <-> UTC */
  int32_t taiOffset = 0;
  assert(MODULE_FUNC(Chronometry, Epoch, TAIMinusUtcSeconds)(
             (MODULE_TYPE(Chronometry, Instant)){0, 0}, &taiOffset) ==
         STATUS_CONST(OUT_OF_RANGE));
  assert(MODULE_FUNC(Chronometry, Epoch, TAIMinusUtcSeconds)(
             (MODULE_TYPE(Chronometry, Instant)){63072000LL, 0}, &taiOffset) ==
         STATUS_CONST(SUCCESS));
  assert(taiOffset == 10);
  MODULE_TYPE(Chronometry, Instant) modernUtc = {1767225600LL, 123456789U};
  MODULE_TYPE(Chronometry, Instant) modernTai = {0};
  MODULE_TYPE(Chronometry, Instant) roundTripUtc = {0};
  assert(MODULE_FUNC(Chronometry, Epoch, TAIFromUtc)(
             modernUtc, &modernTai) == STATUS_CONST(SUCCESS));
  assert(modernTai.seconds == modernUtc.seconds + 37);
  assert(MODULE_FUNC(Chronometry, Epoch, TAIToUtc)(
             modernTai, &roundTripUtc) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Instant, Compare)(
             modernUtc, roundTripUtc) == COMPARISON_CONST(EQUAL));

  /* DST Gap (Nonexistent) & Overlap (Ambiguous) Disambiguation */
  MODULE_ENUM(Chronometry, Timezone, LocalTimeStatus) lstat = 0;
  MODULE_TYPE(Chronometry, OffsetTimestamp) earlier = {0};
  MODULE_TYPE(Chronometry, OffsetTimestamp) later = {0};
  MODULE_TYPE(Chronometry, Timestamp) nySpringGap = {
      .date = {2026, 3, 8}, .time = {.hour = 2, .minute = 30}};
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, ClassifyLocal)(
             &nyRule, nySpringGap, &lstat, &earlier, &later) ==
         STATUS_CONST(SUCCESS));
  assert(lstat ==
         MODULE_CONST(Chronometry, Timezone, LOCAL_TIME_NONEXISTENT));
  assert(earlier.local.time.hour == 1 && !earlier.daylightSavings);
  assert(later.local.time.hour == 3 && later.daylightSavings);

  MODULE_TYPE(Chronometry, OffsetTimestamp) resolvedPolicy = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, ResolveLocalWithPolicy)(
             &nyRule, nySpringGap,
             MODULE_CONST(Chronometry, Timezone, DISAMBIGUATE_REJECT),
             &resolvedPolicy) == STATUS_CONST(OUT_OF_RANGE));
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, ResolveLocalWithPolicy)(
             &nyRule, nySpringGap,
             MODULE_CONST(Chronometry, Timezone, DISAMBIGUATE_COMPATIBLE),
             &resolvedPolicy) == STATUS_CONST(SUCCESS));
  assert(resolvedPolicy.local.time.hour == 3 && resolvedPolicy.daylightSavings);

  MODULE_TYPE(Chronometry, Timestamp) nyFallOverlap = {
      .date = {2026, 11, 1}, .time = {.hour = 1, .minute = 30}};
  assert(MODULE_FUNC(Chronometry, Timezone, Zone, ClassifyLocal)(
             &nyRule, nyFallOverlap, &lstat, &earlier, &later) ==
         STATUS_CONST(SUCCESS));
  assert(lstat == MODULE_CONST(Chronometry, Timezone, LOCAL_TIME_AMBIGUOUS));
  assert(earlier.daylightSavings && earlier.offset.minutesEastOfUtc == -240);
  assert(!later.daylightSavings && later.offset.minutesEastOfUtc == -300);

  /* System Timezone & Country Catalog */
  MODULE_TYPE(Chronometry, OffsetTimestamp) sysNow = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, SystemNow)(&sysNow) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Timezone, Offset, IsValid)(sysNow.offset));

  const char *countryName = NULL;
  assert(MODULE_FUNC(Chronometry, Timezone, Country, NameFromCode)(
             "BR", &countryName) == STATUS_CONST(SUCCESS));
  assert(strcmp(countryName, "Brazil") == 0);
  size_t brCount = 0U;
  assert(MODULE_FUNC(Chronometry, Timezone, Country, ZoneCount)(
             "BR", &brCount) == STATUS_CONST(SUCCESS));
  assert(brCount > 1U);
  MODULE_TYPE(Chronometry, TimezoneRule) brFirstRule = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, Country, ZoneRuleAt)(
             "BR", 0U, &brFirstRule) == STATUS_CONST(SUCCESS));
  assert(brFirstRule.ianaName != NULL);

  /* ISO Instant & Duration Formatting/Parsing (Zero stdio.h) */
  char isoInstBuf[MODULE_CONST(Chronometry, Format, ISO_INSTANT_BUFFER_SIZE)];
  assert(MODULE_FUNC(Chronometry, Format, ToISOInstant)(
             (MODULE_TYPE(Chronometry, Instant)){0, 500000000U}, isoInstBuf,
             sizeof(isoInstBuf)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoInstBuf, "1970-01-01T00:00:00.500Z") == 0);
  MODULE_TYPE(Chronometry, Instant) parsedInst = {0};
  assert(MODULE_FUNC(Chronometry, Format, FromISOInstant)(
             "1969-12-31T21:00:00.500-03:00", &parsedInst) ==
         STATUS_CONST(SUCCESS));
  assert(parsedInst.seconds == 0 && parsedInst.nanoseconds == 500000000U);

  char isoDurBuf[MODULE_CONST(Chronometry, Format, ISO_DURATION_BUFFER_SIZE)];
  MODULE_TYPE(Chronometry, Duration) durSample =
      MODULE_FUNC(Chronometry, Duration, FromNanoseconds)(-5405250000000LL);
  assert(MODULE_FUNC(Chronometry, Format, ToISODuration)(
             durSample, isoDurBuf, sizeof(isoDurBuf)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoDurBuf, "-PT1H30M5.25S") == 0);
  MODULE_TYPE(Chronometry, Duration) parsedDur = {0};
  assert(MODULE_FUNC(Chronometry, Format, FromISODuration)(
             isoDurBuf, &parsedDur) == STATUS_CONST(SUCCESS));
  assert(parsedDur.nanoseconds == durSample.nanoseconds);
  assert(MODULE_FUNC(Chronometry, Format, FromISODuration)(
             "P1DT2H", &parsedDur) == STATUS_CONST(SUCCESS));
  assert(parsedDur.nanoseconds ==
         93600LL * MODULE_CONST(Chronometry, Duration, NANOSECONDS_PER_SECOND));

  char isoDate[MODULE_CONST(Chronometry, Format, ISO_DATE_BUFFER_SIZE)];
  assert(MODULE_CALL(Chronometry, Format, ToISODate,
                     (unixEpoch, isoDate, sizeof(isoDate))) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(isoDate, "1970-01-01") == 0);

  MODULE_TYPE(Chronometry, Date) negativeYear = {-44, 3, 15};
  assert(MODULE_FUNC(Chronometry, Format, ToISODate)(
             negativeYear, isoDate, sizeof(isoDate)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoDate, "-0044-03-15") == 0);

  MODULE_TYPE(Chronometry, Date) parsedDate = {0};
  assert(MODULE_FUNC(Chronometry, Format, FromISODate)(
             "1970-01-01", &parsedDate) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Calendar, Date, Compare)(
             unixEpoch, parsedDate) == COMPARISON_CONST(EQUAL));
  assert(MODULE_FUNC(Chronometry, Format, FromISODate)(
             "1970-1-1", &parsedDate) == STATUS_CONST(OUT_OF_RANGE));
  assert(MODULE_FUNC(Chronometry, Format, FromISODate)(
             " 1970-01-01", &parsedDate) == STATUS_CONST(OUT_OF_RANGE));

  char isoTimestamp[MODULE_CONST(Chronometry, Format,
                                 ISO_TIMESTAMP_BUFFER_SIZE)];
  assert(MODULE_FUNC(Chronometry, Format, ToISOTimestamp)(
             dt, isoTimestamp, sizeof(isoTimestamp)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoTimestamp, "1970-01-01T00:00:00.000") == 0);

  MODULE_TYPE(Chronometry, Timestamp) parsedDt = {0};
  assert(MODULE_FUNC(Chronometry, Format, FromISOTimestamp)(
             isoTimestamp, &parsedDt) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Timestamp, Compare)(dt, parsedDt) ==
         COMPARISON_CONST(EQUAL));

  char isoOffset[MODULE_CONST(Chronometry, Format,
                              ISO_OFFSET_TIMESTAMP_BUFFER_SIZE)];
  assert(MODULE_FUNC(Chronometry, Format, ToISOOffsetTimestamp)(
             odt, isoOffset, sizeof(isoOffset)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoOffset, "1969-12-31T21:00:00.000-03:00") == 0);

  MODULE_TYPE(Chronometry, OffsetTimestamp) parsedOdt = {0};
  assert(MODULE_FUNC(Chronometry, Format, FromISOOffsetTimestamp)(
             isoOffset, &parsedOdt) == STATUS_CONST(SUCCESS));
  assert(parsedOdt.offset.minutesEastOfUtc == -180);
  assert(MODULE_FUNC(Chronometry, Timestamp, Compare)(
             odt.local, parsedOdt.local) == COMPARISON_CONST(EQUAL));
}

int main(void) {
  TestDuration();
  TestClockAndTimer();
  TestInstant();
  TestCalendarDateTimeEpochTimezoneFormat();
  printf("Chronometry tests passed!\n");
  return 0;
}
