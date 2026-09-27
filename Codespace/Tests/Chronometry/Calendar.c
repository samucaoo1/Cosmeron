#include "../../Cosmeron/Modules/Chronometry/Calendar/Calendar.h"

int main(void) {
  Chronometry_Calendar_Policy g =
      Chronometry_Calendar_SystemPolicy(CHRONOMETRY_CALENDAR_GREGORIAN);
  Chronometry_Calendar_Policy reform;
  Chronometry_Date d = {2024, 2, 29}, back;
  uint16_t doy = 0;
  int64_t ord = 0, diff = 0;

  if (!Chronometry_Calendar_IsLeapYear(CHRONOMETRY_CALENDAR_GREGORIAN, 2024))
    return 1;
  if (Chronometry_Calendar_IsLeapYear(CHRONOMETRY_CALENDAR_GREGORIAN, 2100))
    return 2;
  if (Chronometry_Calendar_DaysInMonth(CHRONOMETRY_CALENDAR_GREGORIAN, 2024, 2) != 29)
    return 3;
  if (!Chronometry_Calendar_DateIsValid(&g, d))
    return 4;
  if (Chronometry_Calendar_DateToDayOfYear(&g, d, &doy) != STATUS_CONST(SUCCESS) ||
      doy != 60U)
    return 5;
  if (Chronometry_Calendar_DateToOrdinal(&g, d, &ord) != STATUS_CONST(SUCCESS))
    return 6;
  if (Chronometry_Calendar_DateFromOrdinal(&g, ord, &back) != STATUS_CONST(SUCCESS))
    return 7;
  if (Chronometry_Calendar_DateCompare(d, back) != 0)
    return 8;
  if (Chronometry_Calendar_DateDifference(
          &g, d, (Chronometry_Date){2024, 3, 1}, &diff) != STATUS_CONST(SUCCESS) ||
      diff != 1)
    return 9;

  if (Chronometry_Calendar_ReformPolicy(
          CHRONOMETRY_CALENDAR_CATHOLIC_1582, &reform) != STATUS_CONST(SUCCESS))
    return 10;
  if (reform.kind != CHRONOMETRY_CALENDAR_HISTORICAL_REFORM ||
      reform.reform.suppressedDays != 10U)
    return 11;

  if (Chronometry_Calendar_DateFromOrdinal(&g, INT64_MAX, &back) !=
      STATUS_CONST(OUT_OF_RANGE))
    return 12;
  if (Chronometry_Calendar_DateFromOrdinal(&g, INT64_MIN, &back) !=
      STATUS_CONST(OUT_OF_RANGE))
    return 13;

  return 0;
}
