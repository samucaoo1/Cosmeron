#pragma once

#include "../Clock/Clock.h"

typedef struct CHRONOMETRY_TYPE(Timer) {
  CHRONOMETRY_TYPE(ClockTimePoint) start;
} CHRONOMETRY_TYPE(Timer);

typedef struct CHRONOMETRY_TYPE(Delta) {
  CHRONOMETRY_TYPE(ClockTimePoint) previous;
} CHRONOMETRY_TYPE(Delta);

typedef struct CHRONOMETRY_TYPE(FrameLimiter) {
  CHRONOMETRY_TYPE(Duration) target;
  CHRONOMETRY_TYPE(ClockTimePoint) frameStart;
} CHRONOMETRY_TYPE(FrameLimiter);

#define CHRONOMETRY_TIMER_START_PROTOTYPE                            \
  inline static OPSTATUS TIMER_FUNC(Start)(CHRONOMETRY_TYPE(Timer) *timer)
#define CHRONOMETRY_TIMER_ELAPSED_PROTOTYPE                          \
  inline static OPSTATUS TIMER_FUNC(Elapsed)(                                   \
      const CHRONOMETRY_TYPE(Timer) *timer,                                   \
      CHRONOMETRY_TYPE(Duration) *elapsed)
#define CHRONOMETRY_TIMER_RESTART_PROTOTYPE                          \
  inline static OPSTATUS TIMER_FUNC(Restart)(                                   \
      CHRONOMETRY_TYPE(Timer) *timer, CHRONOMETRY_TYPE(Duration) *elapsed)
#define CHRONOMETRY_DELTA_START_PROTOTYPE                            \
  inline static OPSTATUS TIMER_FUNC(DeltaStart)(CHRONOMETRY_TYPE(Delta) *delta)
#define CHRONOMETRY_DELTA_UPDATE_PROTOTYPE                           \
  inline static OPSTATUS TIMER_FUNC(DeltaUpdate)(                               \
      CHRONOMETRY_TYPE(Delta) *delta, CHRONOMETRY_TYPE(Duration) *elapsed)
#define CHRONOMETRY_FRAME_LIMITER_CREATE_PROTOTYPE                   \
  inline static OPSTATUS TIMER_FUNC(FrameLimiterCreate)(                        \
      CHRONOMETRY_TYPE(Duration) target,                                      \
      CHRONOMETRY_TYPE(FrameLimiter) *limiter)
#define CHRONOMETRY_FRAME_LIMITER_FROM_FPS_PROTOTYPE                 \
  inline static OPSTATUS TIMER_FUNC(FrameLimiterFromFPS)(                       \
      uint32_t fps, CHRONOMETRY_TYPE(FrameLimiter) *limiter)
#define CHRONOMETRY_FRAME_LIMITER_BEGIN_PROTOTYPE                    \
  inline static OPSTATUS TIMER_FUNC(FrameLimiterBegin)(                         \
      CHRONOMETRY_TYPE(FrameLimiter) *limiter)
#define CHRONOMETRY_FRAME_LIMITER_REMAINING_PROTOTYPE                \
  inline static OPSTATUS TIMER_FUNC(FrameLimiterRemaining)(                     \
      const CHRONOMETRY_TYPE(FrameLimiter) *limiter,                          \
      CHRONOMETRY_TYPE(Duration) *remaining)

#include "Impl/Timer.impl"
