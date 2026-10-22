#pragma once
#include "../../../Core/Module/Object.h"
#include "Animation.space"

#if defined(ANIMATION_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

OBJECT(TAnimationController,
  FIELDS(
    float startValue;
    float endValue;
    float duration;
    float elapsed;
    ANIMATION_TYPE(EasingKind) easing;
    bool loop;
    bool reverse;
    bool running;
    bool forward;
  ),
  METHOD(OPSTATUS, Configure, (float startValue, float endValue, float duration, ANIMATION_TYPE(EasingKind) easing)),
  METHOD(OPSTATUS, Update, (float deltaSeconds)),
  METHOD(float, Value, ()),
  METHOD(float, Progress, ()),
  METHOD(bool, IsFinished, ()),
  METHOD(OPSTATUS, Reset, ()),
  METHOD(OPSTATUS, SetLoop, (bool loop)),
  METHOD(OPSTATUS, SetReverse, (bool reverse)),
  METHOD(OPSTATUS, Play, ()),
  METHOD(OPSTATUS, Pause, ())
);

#define ANIMATION_CONFIGURE_PROTOTYPE OBJECT_PROTOTYPE(TAnimationController, Configure, (float startValue, float endValue, float duration, ANIMATION_TYPE(EasingKind) easing))
#define ANIMATION_UPDATE_PROTOTYPE OBJECT_PROTOTYPE(TAnimationController, Update, (float deltaSeconds))
#define ANIMATION_VALUE_PROTOTYPE OBJECT_PROTOTYPE(TAnimationController, Value, ())
#define ANIMATION_PROGRESS_PROTOTYPE OBJECT_PROTOTYPE(TAnimationController, Progress, ())
#define ANIMATION_IS_FINISHED_PROTOTYPE OBJECT_PROTOTYPE(TAnimationController, IsFinished, ())
#define ANIMATION_RESET_PROTOTYPE OBJECT_PROTOTYPE(TAnimationController, Reset, ())
#define ANIMATION_SET_LOOP_PROTOTYPE OBJECT_PROTOTYPE(TAnimationController, SetLoop, (bool loop))
#define ANIMATION_SET_REVERSE_PROTOTYPE OBJECT_PROTOTYPE(TAnimationController, SetReverse, (bool reverse))
#define ANIMATION_PLAY_PROTOTYPE OBJECT_PROTOTYPE(TAnimationController, Play, ())
#define ANIMATION_PAUSE_PROTOTYPE OBJECT_PROTOTYPE(TAnimationController, Pause, ())

#define ANIMATION_EASE_PROTOTYPE \
  static inline float ANIMATION_FUNC(Ease)(float t, ANIMATION_TYPE(EasingKind) easing)
ANIMATION_EASE_PROTOTYPE;

#include "Impl/Animation.impl"
