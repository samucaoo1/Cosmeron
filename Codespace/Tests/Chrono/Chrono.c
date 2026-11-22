#include "../../Cosmeron/Modules/Chrono/Chrono.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

static void TestMath(void) {
  MODULE_TYPE(Chrono, Span) s1 =
      MODULE_FUNC(Chrono, Math, FromNanoseconds)(500);
  assert(s1.nanoseconds == 500);

  MODULE_TYPE(Chrono, Span) s2 = {0};
  assert(MODULE_CALL(Chrono, Math, FromSeconds, (3, &s2)) ==
         STATUS_CONST(SUCCESS));
  assert(s2.nanoseconds ==
         3LL * MODULE_CONST(Chrono, Math, NANOSECONDS_PER_SECOND));

  MODULE_TYPE(Chrono, Span) sMicro = {0};
  assert(MODULE_FUNC(Chrono, Math, FromMicroseconds)(5, &sMicro) ==
         STATUS_CONST(SUCCESS));
  assert(sMicro.nanoseconds ==
         5LL * MODULE_CONST(Chrono, Math, NANOSECONDS_PER_MICROSECOND));

  MODULE_TYPE(Chrono, Span) sMilli = {0};
  assert(MODULE_FUNC(Chrono, Math, FromMilliseconds)(7, &sMilli) ==
         STATUS_CONST(SUCCESS));
  assert(sMilli.nanoseconds ==
         7LL * MODULE_CONST(Chrono, Math, NANOSECONDS_PER_MILLISECOND));

  assert(MODULE_FUNC(Chrono, Math, FromSeconds)(1, NULL) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Chrono, Math, FromSeconds)(INT64_MAX, &s2) ==
         STATUS_CONST(ARITHMETIC_OVERFLOW));

  MODULE_TYPE(Chrono, Span) hz60 = {0};
  assert(MODULE_FUNC(Chrono, Math, FromHz)(60.0, &hz60) ==
         STATUS_CONST(SUCCESS));
  assert(hz60.nanoseconds == 16666667LL);
  assert(MODULE_FUNC(Chrono, Math, FromHz)(0.0, &hz60) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Chrono, Math, FromHz)(2e9, &hz60) ==
         STATUS_CONST(OUT_OF_RANGE));
  double outHz = 0.0;
  assert(MODULE_FUNC(Chrono, Math, ToHz)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(10000000LL), &outHz) ==
         STATUS_CONST(SUCCESS));
  assert(outHz > 99.9 && outHz < 100.1);
  assert(MODULE_FUNC(Chrono, Math, ToHz)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(0), &outHz) ==
         STATUS_CONST(INVALID_ARGUMENT));

  MODULE_TYPE(Chrono, Span) sum = {0};
  assert(MODULE_FUNC(Chrono, Math, Add)(s1, s2, &sum) == STATUS_CONST(SUCCESS));
  assert(sum.nanoseconds == s2.nanoseconds + 500);

  MODULE_TYPE(Chrono, Span) diff = {0};
  assert(MODULE_FUNC(Chrono, Math, Subtract)(s1, s2, &diff) ==
         STATUS_CONST(SUCCESS));
  assert(diff.nanoseconds == 500 - s2.nanoseconds);

  assert(MODULE_FUNC(Chrono, Math, Compare)(s1, s2) == COMPARISON_CONST(LOWER));
  assert(MODULE_FUNC(Chrono, Math, Compare)(s2, s1) ==
         COMPARISON_CONST(HIGHER));
  assert(MODULE_FUNC(Chrono, Math, Compare)(s1, s1) == COMPARISON_CONST(EQUAL));

  MODULE_TYPE(Chrono, Span) absVal = {0};
  assert(MODULE_FUNC(Chrono, Math, Absolute)(diff, &absVal) ==
         STATUS_CONST(SUCCESS));
  assert(absVal.nanoseconds == s2.nanoseconds - 500);

  MODULE_TYPE(Chrono, Span) sMin = {0};
  MODULE_TYPE(Chrono, Span) sHour = {0};
  MODULE_TYPE(Chrono, Span) sDay = {0};
  assert(MODULE_FUNC(Chrono, Math, FromMinutes)(2, &sMin) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Math, ToSeconds)(sMin) == 120);
  assert(MODULE_FUNC(Chrono, Math, FromHours)(3, &sHour) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Math, ToMilliseconds)(sHour) == 10800000LL);
  assert(MODULE_FUNC(Chrono, Math, FromDays)(1, &sDay) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Math, ToMicroseconds)(sDay) == 86400000000LL);
  assert(MODULE_FUNC(Chrono, Math, ToNanoseconds)(s1) == 500);

  MODULE_TYPE(Chrono, Span) zero =
      MODULE_FUNC(Chrono, Math, FromNanoseconds)(0);
  assert(MODULE_FUNC(Chrono, Math, IsZero)(zero));
  assert(!MODULE_FUNC(Chrono, Math, IsNegative)(zero));
  assert(MODULE_FUNC(Chrono, Math, IsNegative)(diff));

  MODULE_TYPE(Chrono, Span) neg = {0};
  assert(MODULE_FUNC(Chrono, Math, Negate)(s2, &neg) ==
         STATUS_CONST(SUCCESS));
  assert(neg.nanoseconds == -s2.nanoseconds);

  MODULE_TYPE(Chrono, Span) scaled = {0};
  assert(MODULE_FUNC(Chrono, Math, Scale)(s1, -4, &scaled) ==
         STATUS_CONST(SUCCESS));
  assert(scaled.nanoseconds == -2000);

  MODULE_TYPE(Chrono, Span) divided = {0};
  assert(MODULE_FUNC(Chrono, Math, Divide)(s2, 2, &divided) ==
         STATUS_CONST(SUCCESS));
  assert(divided.nanoseconds ==
         1500LL * MODULE_CONST(Chrono, Math, NANOSECONDS_PER_MILLISECOND));
  assert(MODULE_FUNC(Chrono, Math, Divide)(s2, 0, &divided) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Chrono, Math, Divide)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(INT64_MIN), -1,
             &divided) == STATUS_CONST(ARITHMETIC_OVERFLOW));

  MODULE_TYPE(Chrono, Span) rem = {0};
  assert(MODULE_FUNC(Chrono, Math, Remainder)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(2500),
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(1000), &rem) ==
         STATUS_CONST(SUCCESS));
  assert(rem.nanoseconds == 500);

  double ratio = 0.0;
  assert(MODULE_FUNC(Chrono, Math, Ratio)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(1500),
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(1000), &ratio) ==
         STATUS_CONST(SUCCESS));
  assert(ratio > 1.49 && ratio < 1.51);

  assert(MODULE_FUNC(Chrono, Math, AddSaturating)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(INT64_MAX - 10),
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(50))
             .nanoseconds == INT64_MAX);
  assert(MODULE_FUNC(Chrono, Math, SubtractSaturating)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(INT64_MIN + 10),
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(50))
             .nanoseconds == INT64_MIN);
  assert(MODULE_FUNC(Chrono, Math, Min)(s1, s2).nanoseconds == s1.nanoseconds);
  assert(MODULE_FUNC(Chrono, Math, Max)(s1, s2).nanoseconds == s2.nanoseconds);

  MODULE_TYPE(Chrono, Span) clamped = {0};
  assert(MODULE_FUNC(Chrono, Math, Clamp)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(5000),
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(100),
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(1000), &clamped) ==
         STATUS_CONST(SUCCESS));
  assert(clamped.nanoseconds == 1000);

  MODULE_TYPE(Chrono, Span) truncVal = {0};
  assert(MODULE_FUNC(Chrono, Math, Truncate)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(-1750),
             MODULE_CONST(Chrono, Math, GRANULARITY_MICROSECOND), &truncVal) ==
         STATUS_CONST(SUCCESS));
  assert(truncVal.nanoseconds == -1000);

  MODULE_TYPE(Chrono, Span) roundVal = {0};
  assert(MODULE_FUNC(Chrono, Math, Round)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(1500),
             MODULE_CONST(Chrono, Math, GRANULARITY_MICROSECOND), &roundVal) ==
         STATUS_CONST(SUCCESS));
  assert(roundVal.nanoseconds == 2000);
  assert(MODULE_FUNC(Chrono, Math, Round)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(-1500),
             MODULE_CONST(Chrono, Math, GRANULARITY_MICROSECOND), &roundVal) ==
         STATUS_CONST(SUCCESS));
  assert(roundVal.nanoseconds == -2000);

  /* Interval operations */
  MODULE_TYPE(Chrono, Interval) iv1 = {{100}, {300}};
  MODULE_TYPE(Chrono, Interval) iv2 = {{200}, {500}};
  MODULE_TYPE(Chrono, Interval) ivDisjoint = {{300}, {400}};
  MODULE_TYPE(Chrono, Interval) ivInvalid = {{500}, {100}};
  assert(MODULE_CALL(Chrono, Math, Interval, IsValid, (iv1)));
  assert(!MODULE_FUNC(Chrono, Math, Interval, IsValid)(ivInvalid));

  MODULE_TYPE(Chrono, Span) ivLen = {0};
  assert(MODULE_FUNC(Chrono, Math, Interval, Length)(iv1, &ivLen) ==
         STATUS_CONST(SUCCESS));
  assert(ivLen.nanoseconds == 200);
  assert(MODULE_FUNC(Chrono, Math, Interval, Contains)(iv1, (MODULE_TYPE(Chrono, Span)){100}));
  assert(!MODULE_FUNC(Chrono, Math, Interval, Contains)(iv1, (MODULE_TYPE(Chrono, Span)){300}));
  assert(MODULE_FUNC(Chrono, Math, Interval, Overlaps)(iv1, iv2));
  assert(!MODULE_FUNC(Chrono, Math, Interval, Overlaps)(iv1, ivDisjoint));

  MODULE_TYPE(Chrono, Interval) inter = {0};
  assert(MODULE_FUNC(Chrono, Math, Interval, Intersection)(iv1, iv2, &inter) ==
         STATUS_CONST(SUCCESS));
  assert(inter.start.nanoseconds == 200 && inter.end.nanoseconds == 300);
  assert(MODULE_FUNC(Chrono, Math, Interval, Intersection)(
             iv1, ivDisjoint, &inter) == STATUS_CONST(OUT_OF_RANGE));

  MODULE_TYPE(Chrono, Interval) united = {0};
  assert(MODULE_FUNC(Chrono, Math, Interval, Union)(iv1, iv2, &united) ==
         STATUS_CONST(SUCCESS));
  assert(united.start.nanoseconds == 100 && united.end.nanoseconds == 500);
}

static void TestClockTimerAndInstant(void) {
  MODULE_TYPE(Chrono, ClockPoint) t1 = {0};
  MODULE_TYPE(Chrono, ClockPoint) t2 = {0};
  assert(MODULE_FUNC(Chrono, Clock, Now)(&t1) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Clock, Now)(&t2) == STATUS_CONST(SUCCESS));

  MODULE_TYPE(Chrono, Span) between = {0};
  assert(MODULE_FUNC(Chrono, Clock, SpanBetween)(t1, t2, &between) ==
         STATUS_CONST(SUCCESS));
  assert(between.nanoseconds >= 0);

  MODULE_TYPE(Chrono, Span) res = {0};
  assert(MODULE_FUNC(Chrono, Clock, Resolution)(&res) ==
         STATUS_CONST(SUCCESS));
  assert(res.nanoseconds > 0);

  MODULE_TYPE(Chrono, Timer) timer = {0};
  MODULE_TYPE(Chrono, Span) elapsed = {0};
  assert(MODULE_CALL(Chrono, Clock, Timer, Start, (&timer)) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Clock, Timer, IsRunning)(&timer));
  assert(MODULE_FUNC(Chrono, Clock, Timer, Elapsed)(&timer, &elapsed) ==
         STATUS_CONST(SUCCESS));
  assert(elapsed.nanoseconds >= 0);

  MODULE_TYPE(Chrono, Span) shortSleep = {0};
  assert(MODULE_FUNC(Chrono, Math, FromMilliseconds)(1, &shortSleep) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Clock, Sleep)(shortSleep) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Clock, Timer, Pause)(&timer) ==
         STATUS_CONST(SUCCESS));
  assert(!MODULE_FUNC(Chrono, Clock, Timer, IsRunning)(&timer));
  MODULE_TYPE(Chrono, Span) pausedElapsed = {0};
  assert(MODULE_FUNC(Chrono, Clock, Timer, Elapsed)(&timer, &pausedElapsed) ==
         STATUS_CONST(SUCCESS));
  assert(pausedElapsed.nanoseconds > 0);
  assert(MODULE_FUNC(Chrono, Clock, Timer, Pause)(&timer) ==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Chrono, Clock, Timer, Resume)(&timer) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Clock, Timer, IsRunning)(&timer));
  assert(MODULE_FUNC(Chrono, Clock, Timer, Restart)(&timer, &elapsed) ==
         STATUS_CONST(SUCCESS));
  assert(elapsed.nanoseconds >= pausedElapsed.nanoseconds);

  MODULE_TYPE(Chrono, TimerDelta) delta = {0};
  assert(MODULE_CALL(Chrono, Clock, Delta, Start, (&delta)) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Clock, Delta, Update)(&delta, &elapsed) ==
         STATUS_CONST(SUCCESS));

  MODULE_TYPE(Chrono, TimerLimiter) limiter = {0};
  assert(MODULE_FUNC(Chrono, Clock, Limiter, FromHz)(60U, &limiter) ==
         STATUS_CONST(SUCCESS));
  MODULE_TYPE(Chrono, Span) remaining = {0};
  assert(MODULE_FUNC(Chrono, Clock, Limiter, Remaining)(
             &limiter, &remaining) == STATUS_CONST(INVALID_ARGUMENT));
  assert(MODULE_FUNC(Chrono, Clock, Limiter, Begin)(&limiter) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Clock, Limiter, Remaining)(
             &limiter, &remaining) == STATUS_CONST(SUCCESS));
  assert(remaining.nanoseconds >= 0);
  assert(MODULE_FUNC(Chrono, Clock, Limiter, Wait)(&limiter) ==
         STATUS_CONST(SUCCESS));

  MODULE_TYPE(Chrono, ClockPoint) wakeTarget = {0};
  assert(MODULE_FUNC(Chrono, Clock, Now)(&wakeTarget) == STATUS_CONST(SUCCESS));
  wakeTarget.nanoseconds +=
      (uint64_t)MODULE_CONST(Chrono, Math, NANOSECONDS_PER_MILLISECOND);
  assert(MODULE_FUNC(Chrono, Clock, SleepUntil)(wakeTarget) ==
         STATUS_CONST(SUCCESS));

  /* Instant */
  MODULE_TYPE(Chrono, Instant) a = {10, 500000000U};
  MODULE_TYPE(Chrono, Instant) b = {10, 600000000U};
  MODULE_TYPE(Chrono, Instant) invalidInst = {
      0, (uint32_t)MODULE_CONST(Chrono, Math, NANOSECONDS_PER_SECOND)};
  assert(MODULE_FUNC(Chrono, Clock, Instant, IsValid)(a));
  assert(!MODULE_FUNC(Chrono, Clock, Instant, IsValid)(invalidInst));
  assert(MODULE_FUNC(Chrono, Clock, Instant, Compare)(a, b) ==
         COMPARISON_CONST(LOWER));

  MODULE_TYPE(Chrono, Span) spanDelta =
      MODULE_FUNC(Chrono, Math, FromNanoseconds)(600000000LL);
  MODULE_TYPE(Chrono, Instant) c = {0};
  assert(MODULE_FUNC(Chrono, Clock, Instant, AddSpan)(a, spanDelta, &c) ==
         STATUS_CONST(SUCCESS));
  assert(c.seconds == 11 && c.nanoseconds == 100000000U);

  MODULE_TYPE(Chrono, Span) measured = {0};
  assert(MODULE_FUNC(Chrono, Clock, Instant, SpanBetween)(a, c, &measured) ==
         STATUS_CONST(SUCCESS));
  assert(measured.nanoseconds == 600000000LL);

  assert(MODULE_FUNC(Chrono, Clock, Instant, ToUnixSeconds)(c) == 11);
  assert(MODULE_FUNC(Chrono, Clock, Instant, FromUnixSeconds)(42).seconds ==
         42);

  int64_t msOut = 0;
  assert(MODULE_FUNC(Chrono, Clock, Instant, ToUnixMillis)(c, &msOut) ==
         STATUS_CONST(SUCCESS));
  assert(msOut == 11100);
  MODULE_TYPE(Chrono, Instant) negMsInst = {0};
  assert(MODULE_FUNC(Chrono, Clock, Instant, FromUnixMillis)(-1, &negMsInst) ==
         STATUS_CONST(SUCCESS));
  assert(negMsInst.seconds == -1 && negMsInst.nanoseconds == 999000000U);

  MODULE_TYPE(Chrono, Instant) wallNow = {0};
  assert(MODULE_FUNC(Chrono, Clock, Instant, Now)(&wallNow) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Clock, Instant, IsValid)(wallNow));
  assert(wallNow.seconds > 0);

  /* Leap seconds & TAI <-> UTC */
  int32_t taiOffset = 0;
  assert(MODULE_FUNC(Chrono, Clock, Instant, TaiMinusUtcSeconds)(
             (MODULE_TYPE(Chrono, Instant)){0, 0}, &taiOffset) ==
         STATUS_CONST(OUT_OF_RANGE));
  assert(MODULE_FUNC(Chrono, Clock, Instant, TaiMinusUtcSeconds)(
             (MODULE_TYPE(Chrono, Instant)){63072000LL, 0}, &taiOffset) ==
         STATUS_CONST(SUCCESS));
  assert(taiOffset == 10);
  MODULE_TYPE(Chrono, Instant) modernUtc = {1767225600LL, 123456789U};
  MODULE_TYPE(Chrono, Instant) modernTai = {0};
  MODULE_TYPE(Chrono, Instant) roundTripUtc = {0};
  assert(MODULE_FUNC(Chrono, Clock, Instant, ToTAI)(modernUtc, &modernTai) ==
         STATUS_CONST(SUCCESS));
  assert(modernTai.seconds == modernUtc.seconds + 37);
  assert(MODULE_FUNC(Chrono, Clock, Instant, FromTAI)(
             modernTai, &roundTripUtc) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Clock, Instant, Compare)(
             modernUtc, roundTripUtc) == COMPARISON_CONST(EQUAL));
}

static void TestCalendarCivilTimezoneFormat(void) {
  MODULE_TYPE(Chrono, CalendarPolicy) gregorian = {0};
  assert(MODULE_FUNC(Chrono, Calendar, SystemPolicy)(
             MODULE_CONST(Chrono, Calendar, GREGORIAN), &gregorian) ==
         STATUS_CONST(SUCCESS));

  assert(MODULE_FUNC(Chrono, Calendar, IsLeapYear)(
      MODULE_CONST(Chrono, Calendar, GREGORIAN), 2024));
  assert(!MODULE_FUNC(Chrono, Calendar, IsLeapYear)(
      MODULE_CONST(Chrono, Calendar, GREGORIAN), 1900));
  assert(MODULE_FUNC(Chrono, Calendar, IsLeapYear)(
      MODULE_CONST(Chrono, Calendar, JULIAN), 1900));

  uint8_t days = 0U;
  assert(MODULE_FUNC(Chrono, Calendar, DaysInMonth)(
             MODULE_CONST(Chrono, Calendar, GREGORIAN), 2024,
             MODULE_CONST(Chrono, Calendar, FEBRUARY), &days) ==
         STATUS_CONST(SUCCESS));
  assert(days == 29U);

  MODULE_TYPE(Chrono, CalendarPolicy) catholic = {0};
  assert(MODULE_FUNC(Chrono, Calendar, ReformPolicy)(
             MODULE_CONST(Chrono, Calendar, CATHOLIC_1582), &catholic) ==
         STATUS_CONST(SUCCESS));

  uint16_t daysInYear = 0U;
  assert(MODULE_FUNC(Chrono, Calendar, DaysInYear)(
             &gregorian, 2024, &daysInYear) == STATUS_CONST(SUCCESS));
  assert(daysInYear == 366U);
  assert(MODULE_FUNC(Chrono, Calendar, DaysInYear)(
             &gregorian, 2026, &daysInYear) == STATUS_CONST(SUCCESS));
  assert(daysInYear == 365U);
  assert(MODULE_FUNC(Chrono, Calendar, DaysInYear)(
             &catholic, 1582, &daysInYear) == STATUS_CONST(SUCCESS));
  assert(daysInYear == 355U);

  MODULE_TYPE(Chrono, Date) suppressed = {
      1582, MODULE_CONST(Chrono, Calendar, OCTOBER), 10};
  assert(!MODULE_FUNC(Chrono, Calendar, Date, IsValid)(&catholic, suppressed));

  uint16_t doy = 0U;
  MODULE_TYPE(Chrono, Date) leapDay = {
      2024, MODULE_CONST(Chrono, Calendar, MARCH), 1};
  assert(MODULE_CALL(Chrono, Calendar, Date, ToDayOfYear,
                     (&gregorian, leapDay, &doy)) == STATUS_CONST(SUCCESS));
  assert(doy == 61U);
  assert(MODULE_FUNC(Chrono, Calendar, DayOfYear)(&gregorian, leapDay, &doy) ==
         STATUS_CONST(SUCCESS));
  assert(doy == 61U);
  assert(MODULE_FUNC(Chrono, Calendar, DayOfYearCumulative)(
             MODULE_CONST(Chrono, Calendar, GREGORIAN), leapDay, &doy) ==
         STATUS_CONST(SUCCESS));
  assert(doy == 61U);

  MODULE_TYPE(Chrono, Date) afterReform = {
      1582, MODULE_CONST(Chrono, Calendar, OCTOBER), 15};
  assert(MODULE_FUNC(Chrono, Calendar, Date, ToDayOfYear)(
             &catholic, afterReform, &doy) == STATUS_CONST(SUCCESS));
  assert(doy == 278U);

  MODULE_TYPE(Chrono, Date) unixEpoch = {
      1970, MODULE_CONST(Chrono, Calendar, JANUARY), 1};
  MODULE_ENUM(Chrono, Calendar, Weekday) weekday = 0;
  assert(MODULE_FUNC(Chrono, Calendar, Date, DayOfWeek)(
             &gregorian, unixEpoch, &weekday) == STATUS_CONST(SUCCESS));
  assert(weekday == MODULE_CONST(Chrono, Calendar, THURSDAY));

  /* JDN & RataDie absorbed into Calendar */
  int64_t jdn = 0;
  assert(MODULE_CALL(Chrono, Calendar, Date, ToJDN,
                     (&gregorian, unixEpoch, &jdn)) == STATUS_CONST(SUCCESS));
  assert(jdn == 2440588);
  MODULE_TYPE(Chrono, Date) fromJdn = {0};
  assert(MODULE_FUNC(Chrono, Calendar, Date, FromJDN)(
             &gregorian, jdn, &fromJdn) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Calendar, Date, Compare)(unixEpoch, fromJdn) ==
         COMPARISON_CONST(EQUAL));

  int64_t rataDie = 0;
  assert(MODULE_FUNC(Chrono, Calendar, Date, ToRataDie)(
             unixEpoch, &rataDie) == STATUS_CONST(SUCCESS));
  assert(rataDie == 719163);
  MODULE_TYPE(Chrono, Date) fromRataDie = {0};
  assert(MODULE_FUNC(Chrono, Calendar, Date, FromRataDie)(
             rataDie, &fromRataDie) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Calendar, Date, Compare)(
             unixEpoch, fromRataDie) == COMPARISON_CONST(EQUAL));

  /* Quarter, remaining days, end of month, business days */
  uint8_t quarter = 0U;
  assert(MODULE_FUNC(Chrono, Calendar, Date, Quarter)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2026, 11, 21}, &quarter) ==
         STATUS_CONST(SUCCESS));
  assert(quarter == 4U);
  uint16_t remYearDays = 0U;
  assert(MODULE_FUNC(Chrono, Calendar, Date, DaysRemainingInYear)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2026, 12, 30},
             &remYearDays) == STATUS_CONST(SUCCESS));
  assert(remYearDays == 1U);
  assert(MODULE_FUNC(Chrono, Calendar, Date, IsEndOfMonth)(
      &gregorian, (MODULE_TYPE(Chrono, Date)){2024, 2, 29}));
  assert(!MODULE_FUNC(Chrono, Calendar, Date, IsEndOfMonth)(
      &gregorian, (MODULE_TYPE(Chrono, Date)){2024, 2, 28}));
  assert(MODULE_FUNC(Chrono, Calendar, Date, IsWeekend)(
      &gregorian, (MODULE_TYPE(Chrono, Date)){2026, 11, 21}));
  assert(!MODULE_FUNC(Chrono, Calendar, Date, IsWeekend)(
      &gregorian, (MODULE_TYPE(Chrono, Date)){2026, 11, 20}));

  MODULE_TYPE(Chrono, Date) bizDate = {0};
  assert(MODULE_FUNC(Chrono, Calendar, Date, NextBusinessDay)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2026, 11, 20},
             &bizDate) == STATUS_CONST(SUCCESS));
  assert(bizDate.year == 2026 && bizDate.month == 11 && bizDate.day == 23);
  assert(MODULE_FUNC(Chrono, Calendar, Date, NextBusinessDay)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2026, 11, 21},
             &bizDate) == STATUS_CONST(SUCCESS));
  assert(bizDate.year == 2026 && bizDate.month == 11 && bizDate.day == 23);
  assert(MODULE_FUNC(Chrono, Calendar, Date, AddBusinessDays)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2026, 11, 23}, -1,
             &bizDate) == STATUS_CONST(SUCCESS));
  assert(bizDate.year == 2026 && bizDate.month == 11 && bizDate.day == 20);
  int64_t bizBetween = 0;
  assert(MODULE_FUNC(Chrono, Calendar, Date, BusinessDaysBetween)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2026, 11, 16},
             (MODULE_TYPE(Chrono, Date)){2026, 11, 23}, &bizBetween) ==
         STATUS_CONST(SUCCESS));
  assert(bizBetween == 5);
  assert(MODULE_FUNC(Chrono, Calendar, Date, BusinessDaysBetween)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2026, 11, 23},
             (MODULE_TYPE(Chrono, Date)){2026, 11, 20}, &bizBetween) ==
         STATUS_CONST(SUCCESS));
  assert(bizBetween == -1);

  MODULE_TYPE(Chrono, Date) secondSundayMarch2026 = {0};
  assert(MODULE_FUNC(Chrono, Calendar, Date, FromNthWeekday)(
             MODULE_CONST(Chrono, Calendar, GREGORIAN), 2026,
             MODULE_CONST(Chrono, Calendar, MARCH),
             MODULE_CONST(Chrono, Calendar, SECOND_WEEKDAY),
             MODULE_CONST(Chrono, Calendar, SUNDAY),
             &secondSundayMarch2026) == STATUS_CONST(SUCCESS));
  assert(secondSundayMarch2026.year == 2026 &&
         secondSundayMarch2026.month == 3 && secondSundayMarch2026.day == 8);

  MODULE_TYPE(Chrono, Date) addedDate = {0};
  assert(MODULE_FUNC(Chrono, Calendar, Date, AddDays)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2024, 2, 28}, 2,
             &addedDate) == STATUS_CONST(SUCCESS));
  assert(addedDate.year == 2024 && addedDate.month == 3 && addedDate.day == 1);

  assert(MODULE_FUNC(Chrono, Calendar, Date, AddMonths)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2024, 1, 31}, 1,
             &addedDate) == STATUS_CONST(SUCCESS));
  assert(addedDate.year == 2024 && addedDate.month == 2 &&
         addedDate.day == 29);

  assert(MODULE_FUNC(Chrono, Calendar, Date, AddYears)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2024, 2, 29}, 1,
             &addedDate) == STATUS_CONST(SUCCESS));
  assert(addedDate.year == 2025 && addedDate.month == 2 &&
         addedDate.day == 28);

  MODULE_TYPE(Chrono, DateIsoWeek) isoWeek = {0};
  assert(MODULE_FUNC(Chrono, Calendar, Date, ToIsoWeek)(
             &gregorian, (MODULE_TYPE(Chrono, Date)){2021, 1, 1}, &isoWeek) ==
         STATUS_CONST(SUCCESS));
  assert(isoWeek.year == 2020 && isoWeek.week == 53 &&
         isoWeek.weekday == MODULE_CONST(Chrono, Calendar, FRIDAY));
  assert(MODULE_FUNC(Chrono, Calendar, Date, FromIsoWeek)(
             &gregorian, isoWeek, &addedDate) == STATUS_CONST(SUCCESS));
  assert(addedDate.year == 2021 && addedDate.month == 1 && addedDate.day == 1);
  assert(MODULE_FUNC(Chrono, Calendar, Date, FromIsoWeek)(
             &gregorian, (MODULE_TYPE(Chrono, DateIsoWeek)){2021, 53, 1},
             &addedDate) == STATUS_CONST(OUT_OF_RANGE));

  /* Civil */
  MODULE_TYPE(Chrono, CivilStamp) dt =
      MODULE_FUNC(Chrono, Civil, StartOfDay)(unixEpoch);
  assert(MODULE_FUNC(Chrono, Civil, IsValid)(&gregorian, dt));
  assert(MODULE_CALL(Chrono, Civil, Time, IsValid, (dt.time)));
  assert(MODULE_FUNC(Chrono, Civil, Midday)(unixEpoch).time.hour == 12);
  MODULE_TYPE(Chrono, CivilStamp) eod =
      MODULE_FUNC(Chrono, Civil, EndOfDay)(unixEpoch);
  assert(eod.time.hour == 23 && eod.time.minute == 59 &&
         eod.time.second == 59 && eod.time.millisecond == 999);
  assert(MODULE_FUNC(Chrono, Civil, TruncateToHour)(eod).time.minute == 0);
  assert(MODULE_FUNC(Chrono, Civil, TruncateToMinute)(eod).time.second == 0);
  assert(MODULE_FUNC(Chrono, Civil, TruncateToSecond)(eod).time.millisecond ==
         0);

  MODULE_TYPE(Chrono, TimeOfDay) laterTime = {
      .hour = 1, .minute = 0, .second = 0, .millisecond = 0};
  assert(MODULE_FUNC(Chrono, Civil, Time, Compare)(dt.time, laterTime) ==
         COMPARISON_CONST(LOWER));

  int64_t timeMs = 0;
  assert(MODULE_FUNC(Chrono, Civil, Time, ToMilliseconds)(laterTime, &timeMs) ==
         STATUS_CONST(SUCCESS));
  assert(timeMs == 3600000LL);
  MODULE_TYPE(Chrono, TimeOfDay) fromMsTime = {0};
  assert(MODULE_FUNC(Chrono, Civil, Time, FromMilliseconds)(
             timeMs, &fromMsTime) == STATUS_CONST(SUCCESS));
  assert(fromMsTime.hour == 1 && fromMsTime.minute == 0);
  int64_t dayCarry = 0;
  MODULE_TYPE(Chrono, TimeOfDay) wrappedTime = {0};
  MODULE_TYPE(Chrono, Span) minusTwoHours =
      MODULE_FUNC(Chrono, Math, FromNanoseconds)(
          -7200LL * MODULE_CONST(Chrono, Math, NANOSECONDS_PER_SECOND));
  assert(MODULE_FUNC(Chrono, Civil, Time, AddSpan)(
             laterTime, minusTwoHours, &wrappedTime, &dayCarry) ==
         STATUS_CONST(SUCCESS));
  assert(wrappedTime.hour == 23 && dayCarry == -1);

  MODULE_TYPE(Chrono, Span) minusOneHour =
      MODULE_FUNC(Chrono, Math, FromNanoseconds)(
          -3600LL * MODULE_CONST(Chrono, Math, NANOSECONDS_PER_SECOND));
  MODULE_TYPE(Chrono, CivilStamp) prevDay = {0};
  assert(MODULE_FUNC(Chrono, Civil, AddSpan)(
             &gregorian, dt, minusOneHour, &prevDay) == STATUS_CONST(SUCCESS));
  assert(prevDay.date.year == 1969 && prevDay.date.month == 12 &&
         prevDay.date.day == 31 && prevDay.time.hour == 23);

  MODULE_TYPE(Chrono, Span) diffDt = {0};
  assert(MODULE_FUNC(Chrono, Civil, Difference)(
             &gregorian, prevDay, dt, &diffDt) == STATUS_CONST(SUCCESS));
  assert(diffDt.nanoseconds ==
         3600LL * MODULE_CONST(Chrono, Math, NANOSECONDS_PER_SECOND));

  MODULE_TYPE(Chrono, Instant) inst = {0};
  assert(MODULE_FUNC(Chrono, Civil, ToInstantUTC)(dt, &inst) ==
         STATUS_CONST(SUCCESS));
  assert(inst.seconds == 0 && inst.nanoseconds == 0U);
  MODULE_TYPE(Chrono, CivilStamp) roundTrip = {0};
  assert(MODULE_FUNC(Chrono, Civil, FromInstantUTC)(inst, &roundTrip) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Civil, Compare)(dt, roundTrip) ==
         COMPARISON_CONST(EQUAL));

  /* Timezone */
  MODULE_TYPE(Chrono, UtcOffset) utcMinus3 = {0};
  assert(MODULE_FUNC(Chrono, Timezone, Offset, FromHoursMinutes)(
             -3, 0, &utcMinus3) == STATUS_CONST(SUCCESS));
  assert(utcMinus3.minutesEastOfUtc == -180);
  MODULE_TYPE(Chrono, UtcOffset) invalidOffset = {1500};
  assert(MODULE_CALL(Chrono, Timezone, Offset, IsValid, (utcMinus3)));
  assert(!MODULE_FUNC(Chrono, Timezone, Offset, IsValid)(invalidOffset));
  MODULE_TYPE(Chrono, CivilOffset) odt = {0};
  assert(MODULE_FUNC(Chrono, Timezone, FromUTC)(inst, utcMinus3, &odt) ==
         STATUS_CONST(SUCCESS));
  assert(odt.local.date.year == 1969 && odt.local.date.month == 12 &&
         odt.local.date.day == 31 && odt.local.time.hour == 21);

  MODULE_TYPE(Chrono, Instant) backToUtc = {0};
  assert(MODULE_FUNC(Chrono, Timezone, ToUTC)(odt, &backToUtc) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Clock, Instant, Compare)(inst, backToUtc) ==
         COMPARISON_CONST(EQUAL));

  MODULE_TYPE(Chrono, TimezoneRule) nyRule = {0};
  assert(MODULE_FUNC(Chrono, Timezone, Zone, FromName)(
             "America/New_York", &nyRule) == STATUS_CONST(SUCCESS));
  assert(nyRule.standardOffset.minutesEastOfUtc == -300);
  assert(nyRule.dstSavingsMinutes == 60);

  MODULE_TYPE(Chrono, CivilStamp) nySummerTs = {
      .date = {2026, 7, 15}, .time = {.hour = 12}};
  MODULE_TYPE(Chrono, CivilOffset) nySummerResolved = {0};
  assert(MODULE_FUNC(Chrono, Timezone, Zone, ResolveLocal)(
             &nyRule, nySummerTs, &nySummerResolved) == STATUS_CONST(SUCCESS));
  assert(nySummerResolved.daylightSavings == true);
  assert(nySummerResolved.offset.minutesEastOfUtc == -240);

  MODULE_TYPE(Chrono, Instant) nySummerUtc = {0};
  assert(MODULE_FUNC(Chrono, Timezone, ToUTC)(
             nySummerResolved, &nySummerUtc) == STATUS_CONST(SUCCESS));
  MODULE_TYPE(Chrono, CivilOffset) nyRoundTrip = {0};
  assert(MODULE_FUNC(Chrono, Timezone, FromUTCWithZone)(
             nySummerUtc, MODULE_CONST(Chrono, Timezone, AMERICA_NEW_YORK),
             &nyRoundTrip) == STATUS_CONST(SUCCESS));
  assert(nyRoundTrip.daylightSavings == true);
  assert(nyRoundTrip.offset.minutesEastOfUtc == -240);
  assert(MODULE_FUNC(Chrono, Civil, Compare)(nyRoundTrip.local, nySummerTs) ==
         COMPARISON_CONST(EQUAL));

  /* ConvertZone, ConvertRule, OffsetAt, NextTransition, PreviousTransition */
  MODULE_TYPE(Chrono, CivilOffset) spConverted = {0};
  assert(MODULE_FUNC(Chrono, Timezone, ConvertZone)(
             nySummerResolved,
             MODULE_CONST(Chrono, Timezone, AMERICA_SAO_PAULO), &spConverted) ==
         STATUS_CONST(SUCCESS));
  assert(spConverted.offset.minutesEastOfUtc == -180 &&
         spConverted.local.time.hour == 13);

  MODULE_TYPE(Chrono, UtcOffset) probeOffset = {0};
  bool probeIsDst = false;
  assert(MODULE_FUNC(Chrono, Timezone, Zone, OffsetAt)(
             &nyRule, nySummerUtc, &probeOffset, &probeIsDst) ==
         STATUS_CONST(SUCCESS));
  assert(probeOffset.minutesEastOfUtc == -240 && probeIsDst);

  MODULE_TYPE(Chrono, Instant) nextTrans = {0};
  MODULE_TYPE(Chrono, Instant) prevTrans = {0};
  MODULE_TYPE(Chrono, UtcOffset) transNewOffset = {0};
  assert(MODULE_FUNC(Chrono, Timezone, Zone, NextTransition)(
             &nyRule, nySummerUtc, &nextTrans, &transNewOffset) ==
         STATUS_CONST(SUCCESS));
  assert(transNewOffset.minutesEastOfUtc == -300);
  assert(MODULE_FUNC(Chrono, Timezone, Zone, PreviousTransition)(
             &nyRule, nySummerUtc, &prevTrans, &transNewOffset) ==
         STATUS_CONST(SUCCESS));
  assert(transNewOffset.minutesEastOfUtc == -240);

  /* Southern Hemisphere & Cairo & London & Aliases */
  MODULE_TYPE(Chrono, TimezoneRule) sydneyRule = {0};
  assert(MODULE_FUNC(Chrono, Timezone, Zone, FromId)(
             MODULE_CONST(Chrono, Timezone, AUSTRALIA_SYDNEY), &sydneyRule) ==
         STATUS_CONST(SUCCESS));
  bool sydneyDstJan = false;
  assert(MODULE_FUNC(Chrono, Timezone, Zone, IsDstAtLocal)(
             &sydneyRule,
             (MODULE_TYPE(Chrono, CivilStamp)){.date = {2026, 1, 15}},
             &sydneyDstJan) == STATUS_CONST(SUCCESS));
  assert(sydneyDstJan == true);

  MODULE_TYPE(Chrono, TimezoneRule) cairoRule = {0};
  assert(MODULE_FUNC(Chrono, Timezone, Zone, FromName)(
             "Africa/Cairo", &cairoRule) == STATUS_CONST(SUCCESS));
  bool cairoDstJul = false;
  assert(MODULE_FUNC(Chrono, Timezone, Zone, IsDstAtLocal)(
             &cairoRule,
             (MODULE_TYPE(Chrono, CivilStamp)){.date = {2026, 7, 15}},
             &cairoDstJul) == STATUS_CONST(SUCCESS));
  assert(cairoDstJul == true);

  MODULE_TYPE(Chrono, TimezoneRule) londonRule = {0};
  assert(MODULE_FUNC(Chrono, Timezone, Zone, FromName)(
             "Europe/London", &londonRule) == STATUS_CONST(SUCCESS));
  assert(londonRule.start.timeBase ==
         MODULE_CONST(Chrono, Timezone, TIME_BASE_UTC));

  assert(strcmp(MODULE_FUNC(Chrono, Timezone, Zone, DataVersion)(), "2026f") ==
         0);
  MODULE_TYPE(Chrono, TimezoneRule) aliasRule = {0};
  assert(MODULE_FUNC(Chrono, Timezone, Zone, FromName)(
             "Brazil/East", &aliasRule) == STATUS_CONST(SUCCESS));
  assert(strcmp(aliasRule.ianaName, "America/Sao_Paulo") == 0);
  assert(MODULE_FUNC(Chrono, Timezone, Zone, NextTransition)(
             &aliasRule, nySummerUtc, &nextTrans, &transNewOffset) ==
         STATUS_CONST(OUT_OF_RANGE));
  assert(MODULE_FUNC(Chrono, Timezone, Zone, FromName)("UTC", &aliasRule) ==
         STATUS_CONST(SUCCESS));
  assert(aliasRule.standardOffset.minutesEastOfUtc == 0);
  assert(MODULE_FUNC(Chrono, Timezone, Zone, FromName)(
             "Etc/GMT+3", &aliasRule) == STATUS_CONST(SUCCESS));
  assert(aliasRule.standardOffset.minutesEastOfUtc == -180);

  MODULE_TYPE(Chrono, CivilOffset) nyFromRuleConv = {0};
  assert(MODULE_FUNC(Chrono, Timezone, ConvertRule)(
             spConverted, &nyRule, &nyFromRuleConv) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Civil, Compare)(
             nyFromRuleConv.local, nySummerResolved.local) ==
         COMPARISON_CONST(EQUAL));

  /* DST Gap & Overlap */
  MODULE_ENUM(Chrono, Timezone, LocalTimeStatus) lstat = 0;
  MODULE_TYPE(Chrono, CivilOffset) earlier = {0};
  MODULE_TYPE(Chrono, CivilOffset) later = {0};
  MODULE_TYPE(Chrono, CivilStamp) nySpringGap = {
      .date = {2026, 3, 8}, .time = {.hour = 2, .minute = 30}};
  assert(MODULE_FUNC(Chrono, Timezone, Zone, ClassifyLocal)(
             &nyRule, nySpringGap, &lstat, &earlier, &later) ==
         STATUS_CONST(SUCCESS));
  assert(lstat == MODULE_CONST(Chrono, Timezone, LOCAL_TIME_NONEXISTENT));
  assert(earlier.local.time.hour == 1 && !earlier.daylightSavings);
  assert(later.local.time.hour == 3 && later.daylightSavings);

  MODULE_TYPE(Chrono, CivilOffset) resolvedPolicy = {0};
  assert(MODULE_FUNC(Chrono, Timezone, Zone, ResolveLocalWithPolicy)(
             &nyRule, nySpringGap,
             MODULE_CONST(Chrono, Timezone, DISAMBIGUATE_REJECT),
             &resolvedPolicy) == STATUS_CONST(OUT_OF_RANGE));
  assert(MODULE_FUNC(Chrono, Timezone, Zone, ResolveLocalWithPolicy)(
             &nyRule, nySpringGap,
             MODULE_CONST(Chrono, Timezone, DISAMBIGUATE_COMPATIBLE),
             &resolvedPolicy) == STATUS_CONST(SUCCESS));
  assert(resolvedPolicy.local.time.hour == 3 && resolvedPolicy.daylightSavings);

  MODULE_TYPE(Chrono, CivilStamp) nyFallOverlap = {
      .date = {2026, 11, 1}, .time = {.hour = 1, .minute = 30}};
  assert(MODULE_FUNC(Chrono, Timezone, Zone, ClassifyLocal)(
             &nyRule, nyFallOverlap, &lstat, &earlier, &later) ==
         STATUS_CONST(SUCCESS));
  assert(lstat == MODULE_CONST(Chrono, Timezone, LOCAL_TIME_AMBIGUOUS));
  assert(earlier.daylightSavings && earlier.offset.minutesEastOfUtc == -240);
  assert(!later.daylightSavings && later.offset.minutesEastOfUtc == -300);

  /* System Timezone & Country Catalog */
  MODULE_TYPE(Chrono, CivilOffset) sysNow = {0};
  assert(MODULE_FUNC(Chrono, Timezone, SystemNow)(&sysNow) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Timezone, Offset, IsValid)(sysNow.offset));

  const char *countryName = NULL;
  assert(MODULE_FUNC(Chrono, Timezone, Country, NameFromCode)(
             "BR", &countryName) == STATUS_CONST(SUCCESS));
  assert(strcmp(countryName, "Brazil") == 0);
  size_t brCount = 0U;
  assert(MODULE_FUNC(Chrono, Timezone, Country, ZoneCount)("BR", &brCount) ==
         STATUS_CONST(SUCCESS));
  assert(brCount > 1U);
  MODULE_TYPE(Chrono, TimezoneRule) brFirstRule = {0};
  assert(MODULE_FUNC(Chrono, Timezone, Country, ZoneRuleAt)(
             "BR", 0U, &brFirstRule) == STATUS_CONST(SUCCESS));
  assert(brFirstRule.ianaName != NULL);

  /* Format: ISO, RFC3339, RFC2822, Pattern, HumanSpan */
  char isoInstBuf[MODULE_CONST(Chrono, Format, ISO_INSTANT_BUFFER_SIZE)];
  assert(MODULE_FUNC(Chrono, Format, ToISOInstant)(
             (MODULE_TYPE(Chrono, Instant)){0, 500000000U}, isoInstBuf,
             sizeof(isoInstBuf)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoInstBuf, "1970-01-01T00:00:00.500Z") == 0);
  MODULE_TYPE(Chrono, Instant) parsedInst = {0};
  assert(MODULE_FUNC(Chrono, Format, FromISOInstant)(
             "1969-12-31T21:00:00.500-03:00", &parsedInst) ==
         STATUS_CONST(SUCCESS));
  assert(parsedInst.seconds == 0 && parsedInst.nanoseconds == 500000000U);

  char isoSpanBuf[MODULE_CONST(Chrono, Format, ISO_SPAN_BUFFER_SIZE)];
  MODULE_TYPE(Chrono, Span) spanSample =
      MODULE_FUNC(Chrono, Math, FromNanoseconds)(-5405250000000LL);
  assert(MODULE_FUNC(Chrono, Format, ToISOSpan)(
             spanSample, isoSpanBuf, sizeof(isoSpanBuf)) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(isoSpanBuf, "-PT1H30M5.25S") == 0);
  MODULE_TYPE(Chrono, Span) parsedSpan = {0};
  assert(MODULE_FUNC(Chrono, Format, FromISOSpan)(isoSpanBuf, &parsedSpan) ==
         STATUS_CONST(SUCCESS));
  assert(parsedSpan.nanoseconds == spanSample.nanoseconds);
  assert(MODULE_FUNC(Chrono, Format, FromISOSpan)("P1DT2H", &parsedSpan) ==
         STATUS_CONST(SUCCESS));
  assert(parsedSpan.nanoseconds ==
         93600LL * MODULE_CONST(Chrono, Math, NANOSECONDS_PER_SECOND));

  char isoDate[MODULE_CONST(Chrono, Format, ISO_DATE_BUFFER_SIZE)];
  assert(MODULE_CALL(Chrono, Format, ToISODate,
                     (unixEpoch, isoDate, sizeof(isoDate))) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(isoDate, "1970-01-01") == 0);

  MODULE_TYPE(Chrono, Date) negativeYear = {-44, 3, 15};
  assert(MODULE_FUNC(Chrono, Format, ToISODate)(negativeYear, isoDate,
                                                sizeof(isoDate)) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(isoDate, "-0044-03-15") == 0);

  MODULE_TYPE(Chrono, Date) parsedDate = {0};
  assert(MODULE_FUNC(Chrono, Format, FromISODate)("1970-01-01", &parsedDate) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Calendar, Date, Compare)(unixEpoch, parsedDate) ==
         COMPARISON_CONST(EQUAL));
  assert(MODULE_FUNC(Chrono, Format, FromISODate)("1970-1-1", &parsedDate) ==
         STATUS_CONST(OUT_OF_RANGE));
  assert(MODULE_FUNC(Chrono, Format, FromISODate)(" 1970-01-01", &parsedDate) ==
         STATUS_CONST(OUT_OF_RANGE));

  char isoTimeBuf[MODULE_CONST(Chrono, Format, ISO_TIME_BUFFER_SIZE)];
  assert(MODULE_FUNC(Chrono, Format, ToISOTimeOfDay)(
             (MODULE_TYPE(Chrono, TimeOfDay)){
                 .hour = 15, .minute = 9, .second = 47, .millisecond = 123},
             isoTimeBuf, sizeof(isoTimeBuf)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoTimeBuf, "15:09:47.123") == 0);
  MODULE_TYPE(Chrono, TimeOfDay) parsedTod = {0};
  assert(MODULE_FUNC(Chrono, Format, FromISOTimeOfDay)(
             isoTimeBuf, &parsedTod) == STATUS_CONST(SUCCESS));
  assert(parsedTod.hour == 15 && parsedTod.millisecond == 123);

  char isoWeekBuf[MODULE_CONST(Chrono, Format, ISO_WEEK_DATE_BUFFER_SIZE)];
  assert(MODULE_FUNC(Chrono, Format, ToISODateIsoWeek)(
             isoWeek, isoWeekBuf, sizeof(isoWeekBuf)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoWeekBuf, "2020-W53-5") == 0);
  MODULE_TYPE(Chrono, DateIsoWeek) parsedWeek = {0};
  assert(MODULE_FUNC(Chrono, Format, FromISODateIsoWeek)(
             isoWeekBuf, &parsedWeek) == STATUS_CONST(SUCCESS));
  assert(parsedWeek.year == 2020 && parsedWeek.week == 53 &&
         parsedWeek.weekday == 5);
  assert(MODULE_FUNC(Chrono, Format, FromISODateIsoWeek)(
             "2021-W53-1", &parsedWeek) == STATUS_CONST(OUT_OF_RANGE));

  char isoCivil[MODULE_CONST(Chrono, Format, ISO_CIVIL_BUFFER_SIZE)];
  assert(MODULE_FUNC(Chrono, Format, ToISOCivilStamp)(
             dt, isoCivil, sizeof(isoCivil)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoCivil, "1970-01-01T00:00:00.000") == 0);

  MODULE_TYPE(Chrono, CivilStamp) parsedDt = {0};
  assert(MODULE_FUNC(Chrono, Format, FromISOCivilStamp)(isoCivil, &parsedDt) ==
         STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Civil, Compare)(dt, parsedDt) ==
         COMPARISON_CONST(EQUAL));

  char isoOffset[MODULE_CONST(Chrono, Format, ISO_CIVIL_OFFSET_BUFFER_SIZE)];
  assert(MODULE_FUNC(Chrono, Format, ToISOCivilOffset)(
             odt, isoOffset, sizeof(isoOffset)) == STATUS_CONST(SUCCESS));
  assert(strcmp(isoOffset, "1969-12-31T21:00:00.000-03:00") == 0);

  MODULE_TYPE(Chrono, CivilOffset) parsedOdt = {0};
  assert(MODULE_FUNC(Chrono, Format, FromISOCivilOffset)(
             isoOffset, &parsedOdt) == STATUS_CONST(SUCCESS));
  assert(parsedOdt.offset.minutesEastOfUtc == -180);
  assert(MODULE_FUNC(Chrono, Civil, Compare)(odt.local, parsedOdt.local) ==
         COMPARISON_CONST(EQUAL));

  /* RFC 3339 & RFC 2822 */
  char rfc3339Buf[MODULE_CONST(Chrono, Format, RFC3339_BUFFER_SIZE)];
  assert(MODULE_FUNC(Chrono, Format, ToRFC3339)(
             odt, rfc3339Buf, sizeof(rfc3339Buf)) == STATUS_CONST(SUCCESS));
  assert(strcmp(rfc3339Buf, "1969-12-31T21:00:00-03:00") == 0);
  MODULE_TYPE(Chrono, CivilOffset) parsedRfc = {0};
  assert(MODULE_FUNC(Chrono, Format, FromRFC3339)(
             "2026-11-21 15:09:47.5z", &parsedRfc) == STATUS_CONST(SUCCESS));
  assert(parsedRfc.local.time.millisecond == 500 &&
         parsedRfc.offset.minutesEastOfUtc == 0);

  char rfc2822Buf[MODULE_CONST(Chrono, Format, RFC2822_BUFFER_SIZE)];
  MODULE_TYPE(Chrono, CivilOffset) sampleCo = {
      .local = {.date = {2026, 11, 21}, .time = {123, 47, 9, 15}},
      .offset = utcMinus3,
      .daylightSavings = false};
  assert(MODULE_FUNC(Chrono, Format, ToRFC2822)(
             sampleCo, rfc2822Buf, sizeof(rfc2822Buf)) ==
         STATUS_CONST(SUCCESS));
  assert(strcmp(rfc2822Buf, "Sat, 21 Nov 2026 15:09:47 -0300") == 0);

  /* Custom Pattern */
  char patBuf[128];
  size_t patWritten = 0U;
  assert(MODULE_FUNC(Chrono, Format, Pattern)(
             NULL, 0U, &patWritten,
             "%a %d %b %Y (%y) %H:%M:%S.%f %I%p j=%j u=%u %z %:z %Z %%",
             sampleCo) == STATUS_CONST(SUCCESS));
  assert(MODULE_FUNC(Chrono, Format, Pattern)(
             patBuf, sizeof(patBuf), &patWritten,
             "%a %d %b %Y (%y) %H:%M:%S.%f %I%p j=%j u=%u %z %:z %Z %%",
             sampleCo) == STATUS_CONST(SUCCESS));
  assert(strcmp(patBuf,
                "Sat 21 Nov 2026 (26) 15:09:47.123 03PM j=325 u=6 -0300 "
                "-03:00 -03:00 %") == 0);
  assert(strlen(patBuf) == patWritten);
  char smallPat[8];
  assert(MODULE_FUNC(Chrono, Format, Pattern)(
             smallPat, sizeof(smallPat), &patWritten, "%Y-%m-%d", sampleCo) ==
         STATUS_CONST(INSUFFICIENT_SPACE));
  assert(patWritten == 10U);

  /* Human Span */
  char humanBuf[MODULE_CONST(Chrono, Format, HUMAN_SPAN_BUFFER_SIZE)];
  assert(MODULE_FUNC(Chrono, Format, ToHumanSpan)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(0), humanBuf,
             sizeof(humanBuf)) == STATUS_CONST(SUCCESS));
  assert(strcmp(humanBuf, "0ns") == 0);
  assert(MODULE_FUNC(Chrono, Format, ToHumanSpan)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(-14200000LL), humanBuf,
             sizeof(humanBuf)) == STATUS_CONST(SUCCESS));
  assert(strcmp(humanBuf, "-14ms 200us") == 0);
  assert(MODULE_FUNC(Chrono, Format, ToHumanSpan)(
             MODULE_FUNC(Chrono, Math, FromNanoseconds)(90061250000000LL),
             humanBuf, sizeof(humanBuf)) == STATUS_CONST(SUCCESS));
  assert(strcmp(humanBuf, "1d 1h 1m 1s 250ms") == 0);
}

int main(void) {
  TestMath();
  TestClockTimerAndInstant();
  TestCalendarCivilTimezoneFormat();
  printf("Chrono tests passed!\n");
  return 0;
}
