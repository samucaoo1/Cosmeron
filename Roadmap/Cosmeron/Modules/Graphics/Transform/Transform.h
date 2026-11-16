#pragma once
#include "../../../Core/Module/Object.h"
#include "Transform.space"

#if defined(TRANSFORM_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

OBJECT(TTransform2D,
  FIELDS(
    float a, b;
    float c, d;
    float tx, ty;
  ),
  METHOD(OPSTATUS, Identity, ()),
  METHOD(OPSTATUS, Translate, (float tx, float ty)),
  METHOD(OPSTATUS, Scale, (float sx, float sy)),
  METHOD(OPSTATUS, Rotate, (float radians)),
  METHOD(OPSTATUS, Shear, (float shx, float shy)),
  METHOD(OPSTATUS, Multiply, (const struct TTransform2D *other)),
  METHOD(OPSTATUS, Invert, (struct TTransform2D *outInverse)),
  METHOD(OPSTATUS, ApplyPoint, (GEOMETRY_TYPE(TPoint2D) point, GEOMETRY_TYPE(TPoint2D) *outPoint)),
  METHOD(OPSTATUS, ApplyVector, (GEOMETRY_TYPE(TPoint2D) vector, GEOMETRY_TYPE(TPoint2D) *outVector)),
  METHOD(OPSTATUS, ApplyRect, (const GEOMETRY_TYPE(TRect2D) *rect, GEOMETRY_TYPE(TRect2D) *outBounds)),
  METHOD(float, Determinant, ())
);

#define TRANSFORM_IDENTITY_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, Identity, ())
#define TRANSFORM_TRANSLATE_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, Translate, (float tx, float ty))
#define TRANSFORM_SCALE_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, Scale, (float sx, float sy))
#define TRANSFORM_ROTATE_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, Rotate, (float radians))
#define TRANSFORM_SHEAR_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, Shear, (float shx, float shy))
#define TRANSFORM_MULTIPLY_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, Multiply, (const struct TTransform2D *other))
#define TRANSFORM_INVERT_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, Invert, (struct TTransform2D *outInverse))
#define TRANSFORM_APPLY_POINT_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, ApplyPoint, (GEOMETRY_TYPE(TPoint2D) point, GEOMETRY_TYPE(TPoint2D) *outPoint))
#define TRANSFORM_APPLY_VECTOR_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, ApplyVector, (GEOMETRY_TYPE(TPoint2D) vector, GEOMETRY_TYPE(TPoint2D) *outVector))
#define TRANSFORM_APPLY_RECT_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, ApplyRect, (const GEOMETRY_TYPE(TRect2D) *rect, GEOMETRY_TYPE(TRect2D) *outBounds))
#define TRANSFORM_DETERMINANT_PROTOTYPE OBJECT_PROTOTYPE(TTransform2D, Determinant, ())

#include "Impl/Transform.impl"
