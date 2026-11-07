#pragma once
#include "../../../Core/Module/Object.h"
#include "Compositor.space"

#if defined(COMPOSITOR_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

OBJECT(TCompositor,
  FIELDS(
    COMPOSITOR_TYPE(TLayer) *layers;
    size_t layerCount;
    size_t capacity;
    bool owns;
  ),
  METHOD(OPSTATUS, Create, (size_t capacity)),
  METHOD(void, Destroy, ()),
  METHOD(OPSTATUS, AddLayer, (const TERMINAL_TYPE(TCanvas) *canvas, int32_t zOrder, TDUAL_TYPE(int32) offset, float opacity, size_t *outIndex)),
  METHOD(OPSTATUS, SetLayerZ, (size_t index, int32_t zOrder)),
  METHOD(OPSTATUS, SetLayerOffset, (size_t index, TDUAL_TYPE(int32) offset)),
  METHOD(OPSTATUS, SetLayerOpacity, (size_t index, float opacity)),
  METHOD(OPSTATUS, SetLayerVisible, (size_t index, bool visible)),
  METHOD(OPSTATUS, SetLayerColorKey, (size_t index, bool useColorKey, uint32_t colorKey)),
  METHOD(OPSTATUS, Compose, (TERMINAL_TYPE(TCanvas) *destination)),
  METHOD(OPSTATUS, Clear, ()),
  METHOD(size_t, LayerCount, ())
);

#define COMPOSITOR_CREATE_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, Create, (size_t capacity))
#define COMPOSITOR_DESTROY_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, Destroy, ())
#define COMPOSITOR_ADD_LAYER_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, AddLayer, (const TERMINAL_TYPE(TCanvas) *canvas, int32_t zOrder, TDUAL_TYPE(int32) offset, float opacity, size_t *outIndex))
#define COMPOSITOR_SET_LAYER_Z_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, SetLayerZ, (size_t index, int32_t zOrder))
#define COMPOSITOR_SET_LAYER_OFFSET_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, SetLayerOffset, (size_t index, TDUAL_TYPE(int32) offset))
#define COMPOSITOR_SET_LAYER_OPACITY_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, SetLayerOpacity, (size_t index, float opacity))
#define COMPOSITOR_SET_LAYER_VISIBLE_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, SetLayerVisible, (size_t index, bool visible))
#define COMPOSITOR_SET_LAYER_COLOR_KEY_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, SetLayerColorKey, (size_t index, bool useColorKey, uint32_t colorKey))
#define COMPOSITOR_COMPOSE_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, Compose, (TERMINAL_TYPE(TCanvas) *destination))
#define COMPOSITOR_CLEAR_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, Clear, ())
#define COMPOSITOR_LAYER_COUNT_PROTOTYPE OBJECT_PROTOTYPE(TCompositor, LayerCount, ())

#include "Impl/Compositor.impl"
