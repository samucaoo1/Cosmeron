#include "../../Cosmeron/Modules/Chronometry/Calendar/Calendar.h"

int main(void) {
  Chronometry_Calendar_Policy g = {0};
  Chronometry_Calendar_Policy reform = {0};
  Chronometry_Calendar_Policy sentinel = {
      .kind = CHRONOMETRY_CALENDAR_HISTORICAL_REFORM,
      .system = CHRONOMETRY_CALENDAR_JULIAN,
  };
  Chronometry_Date d = {2024, 2, 29}, back;
  uint16_t doy = 0;
  int64_t ord = 0, diff = 0;

  if (Chronometry_Calendar_SystemPolicy(
          CHRONOMETRY_CALENDAR_GREGORIAN, &g) != STATUS_CONST(SUCCESS))
    return 1;
  if (Chronometry_Calendar_SystemPolicy(
          CHRONOMETRY_CALENDAR_GREGORIAN, NULL) !=
      STATUS_CONST(INVALID_ARGUMENT))
    return 5;
  reform = sentinel;
  if (Chronometry_Calendar_SystemPolicy(
          (Chronometry_Calendar_System)99, &reform) !=
          STATUS_CONST(OUT_OF_RANGE) ||
      reform.kind != sentinel.kind || reform.system != sentinel.system)
    return 6;

  if (!Chronometry_Calendar_IsLeapYear(CHRONOMETRY_CALENDAR_GREGORIAN, 2024))
    return 7;
  if (Chronometry_Calendar_IsLeapYear(CHRONOMETRY_CALENDAR_GREGORIAN, 2100))
    return 5;
  {
    uint8_t days = 0U;
    if (Chronometry_Calendar_DaysInMonth(
            CHRONOMETRY_CALENDAR_GREGORIAN, 2024, 2, &days) !=
            STATUS_CONST(SUCCESS) ||
        days != 29U)
      return 6;
    days = 77U;
    if (Chronometry_Calendar_DaysInMonth(
            CHRONOMETRY_CALENDAR_GREGORIAN, 2024, 0, &days) !=
            STATUS_CONST(OUT_OF_RANGE) ||
        days != 77U)
      return 7;
    if (Chronometry_Calendar_DaysInMonth(
            CHRONOMETRY_CALENDAR_GREGORIAN, 2024, 2, NULL) !=
        STATUS_CONST(INVALID_ARGUMENT))
      return 8;
  }
  if (!Chronometry_Calendar_DateIsValid(&g, d))
    return 7;
  if (Chronometry_Calendar_DateToDayOfYear(&g, d, &doy) != STATUS_CONST(SUCCESS) ||
      doy != 60U)
    return 10;
  if (Chronometry_Calendar_DateToOrdinal(&g, d, &ord) != STATUS_CONST(SUCCESS))
    return 11;
  if (Chronometry_Calendar_DateFromOrdinal(&g, ord, &back) != STATUS_CONST(SUCCESS))
    return 12;
  if (Chronometry_Calendar_DateCompare(d, back) != 0)
    return 13;
  if (Chronometry_Calendar_DateDifference(
          &g, d, (Chronometry_Date){2024, 3, 1}, &diff) != STATUS_CONST(SUCCESS) ||
      diff != 1)
    return 14;

  if (Chronometry_Calendar_ReformPolicy(
          CHRONOMETRY_CALENDAR_CATHOLIC_1582, &reform) != STATUS_CONST(SUCCESS))
    return 15;
  if (reform.kind != CHRONOMETRY_CALENDAR_HISTORICAL_REFORM ||
      reform.reform.suppressedDays != 10U)
    return 16;

  if (Chronometry_Calendar_DateFromOrdinal(&g, INT64_MAX, &back) !=
      STATUS_CONST(OUT_OF_RANGE))
    return 17;
  if (Chronometry_Calendar_DateFromOrdinal(&g, INT64_MIN, &back) !=
      STATUS_CONST(OUT_OF_RANGE))
    return 18;

  return 0;
}
