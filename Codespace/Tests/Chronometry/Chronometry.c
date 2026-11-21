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
  assert(MODULE_FUNC(Chronometry, Timer, FrameLimiter, Begin)(&limiter) ==
         STATUS_CONST(SUCCESS));
  MODULE_TYPE(Chronometry, Duration) remaining = {0};
  assert(MODULE_FUNC(Chronometry, Timer, FrameLimiter, Remaining)(
             &limiter, &remaining) == STATUS_CONST(SUCCESS));
  assert(remaining.nanoseconds >= 0);
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
  MODULE_TYPE(Chronometry, TimeStamp) dt = {
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
  MODULE_TYPE(Chronometry, TimeStamp) prevDay = {0};
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
  assert(MODULE_FUNC(Chronometry, Epoch, DateTimeToUnix)(dt, &inst) ==
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

  MODULE_TYPE(Chronometry, TimeStamp) roundTrip = {0};
  assert(MODULE_FUNC(Chronometry, Epoch, DateTimeFromUnix)(inst, &roundTrip) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Timestamp, Compare)(dt, roundTrip) ==
         COMPARISON_CONST(EQUAL));

  MODULE_TYPE(Chronometry, UtcOffset) utcMinus3 = {-180};
  MODULE_TYPE(Chronometry, UtcOffset) invalidOffset = {1500};
  assert(MODULE_CALL(Chronometry, Timezone, Offset, IsValid, (utcMinus3)));
  assert(!MODULE_FUNC(Chronometry, Timezone, Offset, IsValid)(invalidOffset));
  MODULE_TYPE(Chronometry, OffsetDateTime) odt = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, FromUTC)(inst, utcMinus3, &odt) ==
         STATUS_CONST(SUCCESS));
  assert(odt.local.date.year == 1969 && odt.local.date.month == 12 &&
         odt.local.date.day == 31 && odt.local.time.hour == 21);

  MODULE_TYPE(Chronometry, Instant) backToUtc = {0};
  assert(MODULE_FUNC(Chronometry, Timezone, ToUTC)(odt, &backToUtc) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Instant, Compare)(inst, backToUtc) ==
         COMPARISON_CONST(EQUAL));

  char isoDate[16];
  assert(MODULE_CALL(Chronometry, Format, ISO, Date,
                     (unixEpoch, isoDate, sizeof(isoDate))) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(isoDate, "+1970-01-01") == 0);

  MODULE_TYPE(Chronometry, Date) parsedDate = {0};
  assert(MODULE_FUNC(Chronometry, Format, ISO, ParseDate)(
             isoDate, &parsedDate) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Calendar, Date, Compare)(
             unixEpoch, parsedDate) == COMPARISON_CONST(EQUAL));

  char isoDateTime[32];
  assert(MODULE_FUNC(Chronometry, Format, ISO, DateTime)(
             dt, isoDateTime, sizeof(isoDateTime)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoDateTime, "+1970-01-01T00:00:00.000") == 0);

  MODULE_TYPE(Chronometry, TimeStamp) parsedDt = {0};
  assert(MODULE_FUNC(Chronometry, Format, ISO, ParseDateTime)(
             isoDateTime, &parsedDt) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chronometry, Timestamp, Compare)(dt, parsedDt) ==
         COMPARISON_CONST(EQUAL));
}

int main(void) {
  TestDuration();
  TestClockAndTimer();
  TestInstant();
  TestCalendarDateTimeEpochTimezoneFormat();
  printf("Chronometry tests passed!\n");
  return 0;
}
