#include "Cosmeron/Modules/Chrono/Clock/Clock.h"
OPSTATUS CosmeronOHOS_Clock(void) {
  MODULE_TYPE(Chrono, ClockPoint) point = {0};
  return MODULE_FUNC(Chrono, Clock, Now)(&point);
}
