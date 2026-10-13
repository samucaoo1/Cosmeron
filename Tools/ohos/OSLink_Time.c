#include "Cosmeron/Modules/Chronometry/Clock/Clock.h"
OPSTATUS CosmeronOHOS_Clock(void) {
  CHRONOMETRY_TYPE(ClockTimePoint) point = {0};
  return CLOCK_FUNC(Now)(&point);
}
