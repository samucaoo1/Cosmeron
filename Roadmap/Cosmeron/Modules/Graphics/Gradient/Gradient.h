#pragma once
#include "../../../Core/Module/Object.h"
#include "Gradient.space"

#if defined(GRADIENT_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

OBJECT(TGradient,
  FIELDS(
    GRADIENT_TYPE(TColorStop) *stops;
    size_t count;
    size_t capacity;
    bool owns;
  ),
  METHOD(OPSTATUS, Create, (size_t capacity)),
  METHOD(void, Destroy, ()),
  METHOD(OPSTATUS, AddStop, (float position, uint32_t color)),
  METHOD(OPSTATUS, Clear, ()),
  METHOD(uint32_t, Sample, (float t)),
  METHOD(OPSTATUS, SampleRange, (uint32_t *outColors, size_t count)),
  METHOD(size_t, Count, ())
);

#define GRADIENT_CREATE_PROTOTYPE OBJECT_PROTOTYPE(TGradient, Create, (size_t capacity))
#define GRADIENT_DESTROY_PROTOTYPE OBJECT_PROTOTYPE(TGradient, Destroy, ())
#define GRADIENT_ADD_STOP_PROTOTYPE OBJECT_PROTOTYPE(TGradient, AddStop, (float position, uint32_t color))
#define GRADIENT_CLEAR_PROTOTYPE OBJECT_PROTOTYPE(TGradient, Clear, ())
#define GRADIENT_SAMPLE_PROTOTYPE OBJECT_PROTOTYPE(TGradient, Sample, (float t))
#define GRADIENT_SAMPLE_RANGE_PROTOTYPE OBJECT_PROTOTYPE(TGradient, SampleRange, (uint32_t *outColors, size_t count))
#define GRADIENT_COUNT_PROTOTYPE OBJECT_PROTOTYPE(TGradient, Count, ())

#define GRADIENT_LERP_COLOR_PROTOTYPE \
  static inline uint32_t GRADIENT_FUNC(LerpColor)(uint32_t c1, uint32_t c2, float t)
GRADIENT_LERP_COLOR_PROTOTYPE;

#include "Impl/Gradient.impl"
