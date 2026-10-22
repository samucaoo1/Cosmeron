#pragma once
#include "../UI.space"
#include "../../../../Core/Module/Object.h"

#if defined(UI_DISABLE_FUNCTION_TABLE) ||                                       \
    defined(UI_COMPONENT_DISABLE_FUNCTION_TABLE)
#ifndef OBJECT_DISABLE_FUNCTION_TABLE
#define OBJECT_DISABLE_FUNCTION_TABLE
#endif
#endif

#define UI_COMPONENT_NS(NAME) GNS2(UI_NS(Component), NAME)
#define UI_COMPONENT_CNS(NAME) CNS3(LIB_PREFIX_CONST(UI_CMOD), COMPONENT, NAME)
#define UI_COMPONENT_TYPE(NAME) UI_COMPONENT_NS(NAME)
#define UI_COMPONENT_FUNC(NAME) GNS2(UI_COMPONENT_TYPE(TComponent), NAME)
#define UI_COMPONENT_CONST(NAME) UI_COMPONENT_CNS(NAME)

/* Retained/Object-oriented TUI Component */
OBJECT(UI_COMPONENT_TYPE(TComponent),
       FIELDS(
         UI_TYPE(WidgetId) id;
         TQUAD_TYPE(uint16) region;
         UI_TYPE(TVisual) visual;
         bool disabled;
         bool visible;
         void *userData;
         OPSTATUS (*drawCallback)(struct UI_COMPONENT_TYPE(TComponent) *self, UI_TYPE(TContext) *context);
         OPSTATUS (*eventCallback)(struct UI_COMPONENT_TYPE(TComponent) *self, UI_TYPE(TContext) *context, const TERMINAL_TYPE(TEvent) *event, bool *outHandled);
       ),
       METHOD(OPSTATUS, Create, (TQUAD_TYPE(uint16) region, const char *label)),
       METHOD(OPSTATUS, Destroy, ()),
       METHOD(OPSTATUS, Draw, (UI_TYPE(TContext) *context)),
       METHOD(OPSTATUS, HandleEvent,
              (UI_TYPE(TContext) *context, const TERMINAL_TYPE(TEvent) *event, bool *outHandled)),
       METHOD(OPSTATUS, SetRegion, (TQUAD_TYPE(uint16) region)),
       METHOD(OPSTATUS, SetDisabled, (bool disabled)),
       METHOD(OPSTATUS, SetVisible, (bool visible)),
       METHOD(OPSTATUS, SetCallbacks,
              (OPSTATUS (*drawCallback)(struct UI_COMPONENT_TYPE(TComponent) *self, UI_TYPE(TContext) *context),
               OPSTATUS (*eventCallback)(struct UI_COMPONENT_TYPE(TComponent) *self, UI_TYPE(TContext) *context, const TERMINAL_TYPE(TEvent) *event, bool *outHandled))),
       CONST_METHOD(TQUAD_TYPE(uint16), Region, ()),
       CONST_METHOD(bool, IsDisabled, ()),
       CONST_METHOD(bool, IsVisible, ()));

typedef UI_COMPONENT_TYPE(TComponent) TComponent;

#include "Impl/Component.impl"

/* EOF */
