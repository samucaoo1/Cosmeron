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
  static inline OPSTATUS TIMER_FUNC(Start)(CHRONOMETRY_TYPE(Timer) *timer)
#define CHRONOMETRY_TIMER_ELAPSED_PROTOTYPE                          \
  static inline OPSTATUS TIMER_FUNC(Elapsed)(                                   \
      const CHRONOMETRY_TYPE(Timer) *timer,                                   \
      CHRONOMETRY_TYPE(Duration) *outElapsed)
#define CHRONOMETRY_TIMER_RESTART_PROTOTYPE                          \
  static inline OPSTATUS TIMER_FUNC(Restart)(                                   \
      CHRONOMETRY_TYPE(Timer) *timer, CHRONOMETRY_TYPE(Duration) *outElapsed)
#define CHRONOMETRY_DELTA_START_PROTOTYPE                            \
  static inline OPSTATUS TIMER_FUNC(DeltaStart)(CHRONOMETRY_TYPE(Delta) *delta)
#define CHRONOMETRY_DELTA_UPDATE_PROTOTYPE                           \
  static inline OPSTATUS TIMER_FUNC(DeltaUpdate)(                               \
      CHRONOMETRY_TYPE(Delta) *delta, CHRONOMETRY_TYPE(Duration) *outElapsed)
#define CHRONOMETRY_FRAME_LIMITER_CREATE_PROTOTYPE                   \
  static inline OPSTATUS TIMER_FUNC(FrameLimiterCreate)(                        \
      CHRONOMETRY_TYPE(Duration) target,                                      \
      CHRONOMETRY_TYPE(FrameLimiter) *outLimiter)
#define CHRONOMETRY_FRAME_LIMITER_FROM_FPS_PROTOTYPE                 \
  static inline OPSTATUS TIMER_FUNC(FrameLimiterFromFPS)(                       \
      uint32_t fps, CHRONOMETRY_TYPE(FrameLimiter) *outLimiter)
#define CHRONOMETRY_FRAME_LIMITER_BEGIN_PROTOTYPE                    \
  static inline OPSTATUS TIMER_FUNC(FrameLimiterBegin)(                         \
      CHRONOMETRY_TYPE(FrameLimiter) *limiter)
#define CHRONOMETRY_FRAME_LIMITER_REMAINING_PROTOTYPE                \
  static inline OPSTATUS TIMER_FUNC(FrameLimiterRemaining)(                     \
      const CHRONOMETRY_TYPE(FrameLimiter) *limiter,                          \
      CHRONOMETRY_TYPE(Duration) *outRemaining)

#include "Impl/Timer.impl"
