#pragma once

#include "../Epoch/Epoch.h"

typedef struct CHRONOMETRY_TYPE(UtcOffset) {
  int16_t minutesEastOfUtc;
} CHRONOMETRY_TYPE(UtcOffset);

typedef struct CHRONOMETRY_TYPE(OffsetDateTime) {
  CHRONOMETRY_TYPE(DateTime) local;
  CHRONOMETRY_TYPE(UtcOffset) offset;
  bool daylightSavings : 1;
} CHRONOMETRY_TYPE(OffsetDateTime);

#define CHRONOMETRY_TIMEZONE_OFFSET_IS_VALID_PROTOTYPE               \
  inline static bool TIMEZONE_FUNC(OffsetIsValid)(                              \
      CHRONOMETRY_TYPE(UtcOffset) offset)
#define CHRONOMETRY_TIMEZONE_TO_UTC_PROTOTYPE                        \
  inline static OPSTATUS TIMEZONE_FUNC(ToUTC)(                                  \
      CHRONOMETRY_TYPE(OffsetDateTime) value,                                 \
      CHRONOMETRY_TYPE(Instant) *instant)
#define CHRONOMETRY_TIMEZONE_FROM_UTC_PROTOTYPE                      \
  inline static OPSTATUS TIMEZONE_FUNC(FromUTC)(                                \
      CHRONOMETRY_TYPE(Instant) instant, CHRONOMETRY_TYPE(UtcOffset) offset,  \
      CHRONOMETRY_TYPE(OffsetDateTime) *value)

#include "Impl/Timezone.impl"
