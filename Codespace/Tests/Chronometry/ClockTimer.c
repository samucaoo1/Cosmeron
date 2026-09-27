#include "../../Cosmeron/Modules/Chronometry/Clock/Clock.h"
#include "../../Cosmeron/Modules/Chronometry/Timer/Timer.h"

int main(void) {
  Chronometry_ClockTimePoint a, b;
  Chronometry_Duration d;

  if (Chronometry_Clock_Now(&a) != STATUS_CONST(SUCCESS))
    return 1;
  if (Chronometry_Clock_Now(&b) != STATUS_CONST(SUCCESS))
    return 2;
  if (Chronometry_Clock_DurationBetween(a, b, &d) != STATUS_CONST(SUCCESS) ||
      d.nanoseconds < 0)
    return 3;

  Chronometry_Timer timer;
  if (Chronometry_Timer_Start(&timer) != STATUS_CONST(SUCCESS))
    return 4;
  if (Chronometry_Timer_Elapsed(&timer, &d) != STATUS_CONST(SUCCESS) ||
      d.nanoseconds < 0)
    return 5;
  if (Chronometry_Timer_Restart(&timer, &d) != STATUS_CONST(SUCCESS) ||
      d.nanoseconds < 0)
    return 6;

  Chronometry_Delta delta;
  if (Chronometry_Timer_DeltaStart(&delta) != STATUS_CONST(SUCCESS))
    return 7;
  if (Chronometry_Timer_DeltaUpdate(&delta, &d) != STATUS_CONST(SUCCESS) ||
      d.nanoseconds < 0)
    return 8;

  Chronometry_FrameLimiter limiter;
  Chronometry_Duration target = Chronometry_Duration_FromNanoseconds(
      16 * CHRONOMETRY_NANOSECONDS_PER_MILLISECOND);
  if (Chronometry_Timer_FrameLimiterCreate(target, &limiter) !=
          STATUS_CONST(SUCCESS) ||
      limiter.target.nanoseconds != target.nanoseconds)
    return 9;
  if (Chronometry_Timer_FrameLimiterFromFPS(60, &limiter) != STATUS_CONST(SUCCESS))
    return 10;
  if (limiter.target.nanoseconds <= 0)
    return 11;
  if (Chronometry_Timer_FrameLimiterBegin(&limiter) != STATUS_CONST(SUCCESS))
    return 12;
  if (Chronometry_Timer_FrameLimiterRemaining(&limiter, &d) !=
      STATUS_CONST(SUCCESS))
    return 13;
  return 0;
}
