#pragma once

#include "../Input.space"
#include "../Internal/Core.inc"

/* Bounded event history. Events are values with no borrowed payload memory. */

/* Copy and remove the oldest event. False for an empty queue or NULL output.
 * False leaves output untouched. Polling does not alter device states. */
#define INPUT_EVENT_POLL_PROTOTYPE \
  static inline bool INPUT_EVENT_FUNC(Poll)( \
      INPUT_TYPE(TEvent) *event)
INPUT_EVENT_POLL_PROTOTYPE;

/* Discard queued events, preserving device states and the dropped counter. */
#define INPUT_EVENT_CLEAR_PROTOTYPE \
  static inline void INPUT_EVENT_FUNC(Clear)( \
      void)
INPUT_EVENT_CLEAR_PROTOTYPE;

/* Return cumulative queue drops plus detected native-loss incidents. */
#define INPUT_EVENT_GET_DROPPED_COUNT_PROTOTYPE \
  static inline uint64_t INPUT_EVENT_FUNC(GetDroppedCount)( \
      void)
INPUT_EVENT_GET_DROPPED_COUNT_PROTOTYPE;

/* Queue a normalized host-window event for the next Update; never injects OS input. */
#define INPUT_EVENT_SUBMIT_PROTOTYPE \
  static inline OPSTATUS INPUT_EVENT_FUNC(Submit)( \
      const INPUT_TYPE(TEvent) *event)
INPUT_EVENT_SUBMIT_PROTOTYPE;

#include "Impl/Event.impl"
