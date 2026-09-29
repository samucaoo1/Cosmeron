#include "../../Cosmeron/Modules/Chronometry/Chronometry.h"

int main(void) {
  Chronometry_Calendar_Policy policy =
      Chronometry_Calendar_SystemPolicy(CHRONOMETRY_CALENDAR_GREGORIAN);
  Chronometry_Date date = {2024, CHRONOMETRY_CALENDAR_FEBRUARY, 29};
  uint16_t dayOfYear = 0U;
  Chronometry_Duration second;

  if (!Chronometry_Calendar_DateIsValid(&policy, date))
    return 1;
  if (Chronometry_Calendar_DateToDayOfYear(&policy, date, &dayOfYear) !=
      STATUS_CONST(SUCCESS))
    return 2;
  if (dayOfYear != 60U)
    return 3;
  if (Chronometry_Duration_FromSeconds(1, &second) != STATUS_CONST(SUCCESS))
    return 4;
  if (second.nanoseconds != DURATION_CONST(NANOSECONDS_PER_SECOND))
    return 5;
  return 0;
}
