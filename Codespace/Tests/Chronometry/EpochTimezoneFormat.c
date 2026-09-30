#include "../../Cosmeron/Modules/Chronometry/Epoch/Epoch.h"
#include "../../Cosmeron/Modules/Chronometry/Timezone/Timezone.h"
#include "../../Cosmeron/Modules/Chronometry/Format/Format.h"
#include <string.h>

int main(void) {
  Chronometry_Calendar_Policy g;
  if (Chronometry_Calendar_SystemPolicy(
          CHRONOMETRY_CALENDAR_GREGORIAN, &g) != STATUS_CONST(SUCCESS))
    return 2;
  Chronometry_Date epoch = {1970, 1, 1}, back;
  int64_t jdn = 0, rd = 0;

  if (Chronometry_Epoch_DateToJulianDayNumber(&g, epoch, &jdn) !=
      STATUS_CONST(SUCCESS))
    return 2;
  if (Chronometry_Epoch_DateFromJulianDayNumber(&g, jdn, &back) !=
          STATUS_CONST(SUCCESS) ||
      Chronometry_Calendar_DateCompare(epoch, back) != 0)
    return 3;
  if (Chronometry_Epoch_DateToRataDie(epoch, &rd) != STATUS_CONST(SUCCESS) ||
      rd <= 0)
    return 4;

  Chronometry_DateTime dt = {{1970, 1, 1}, {0, 0, 0, 0}}, parsed;
  Chronometry_Instant instant;
  if (Chronometry_Epoch_DateTimeToUnix(dt, &instant) != STATUS_CONST(SUCCESS) ||
      instant.seconds != 0 || instant.nanoseconds != 0)
    return 5;
  if (Chronometry_Epoch_DateTimeFromUnix(instant, &parsed) !=
          STATUS_CONST(SUCCESS) ||
      Chronometry_DateTime_Compare(dt, parsed) != 0)
    return 6;

  if (!Chronometry_Timezone_OffsetIsValid((Chronometry_UtcOffset){60}))
    return 7;
  if (Chronometry_Timezone_OffsetIsValid((Chronometry_UtcOffset){1440}))
    return 8;

  Chronometry_OffsetDateTime local = {
      {{1970, 1, 1}, {0, 0, 0, 1}}, {60}, false};
  if (Chronometry_Timezone_ToUTC(local, &instant) != STATUS_CONST(SUCCESS) ||
      instant.seconds != 0)
    return 9;
  Chronometry_OffsetDateTime restored;
  if (Chronometry_Timezone_FromUTC(
          instant, (Chronometry_UtcOffset){60}, &restored) != STATUS_CONST(SUCCESS))
    return 10;
  if (restored.local.time.hour != 1 || restored.offset.minutesEastOfUtc != 60)
    return 11;

  char buf[64];
  char unchanged[64];

  memset(buf, 0x5A, sizeof(buf));
  memcpy(unchanged, buf, sizeof(buf));
  if (Chronometry_Format_DateISO(epoch, buf, 4U) != STATUS_CONST(OUT_OF_RANGE))
    return 12;
  if (memcmp(buf, unchanged, sizeof(buf)) != 0)
    return 13;

  if (Chronometry_Format_DateISO(epoch, buf, sizeof(buf)) != STATUS_CONST(SUCCESS))
    return 14;
  if (strcmp(buf, "+1970-01-01") != 0)
    return 15;
  if (Chronometry_Format_ParseDateISO(buf, &back) != STATUS_CONST(SUCCESS) ||
      Chronometry_Calendar_DateCompare(epoch, back) != 0)
    return 16;

  memset(buf, 0x5A, sizeof(buf));
  memcpy(unchanged, buf, sizeof(buf));
  if (Chronometry_Format_DateTimeISO(dt, buf, 8U) != STATUS_CONST(OUT_OF_RANGE))
    return 17;
  if (memcmp(buf, unchanged, sizeof(buf)) != 0)
    return 18;

  if (Chronometry_Format_DateTimeISO(dt, buf, sizeof(buf)) != STATUS_CONST(SUCCESS))
    return 19;
  if (Chronometry_Format_ParseDateTimeISO(buf, &parsed) != STATUS_CONST(SUCCESS) ||
      Chronometry_DateTime_Compare(dt, parsed) != 0)
    return 20;
  return 0;
}
