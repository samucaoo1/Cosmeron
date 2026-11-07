#pragma once
GUI_FONT_ASF_PROTOTYPE {
  GUI_TYPE(TFont) result = {0};
  result.asf = font;
  result.height = font ? (float)font->height : 0;
  return result;
}
static inline GUI_TYPE(TTheme)
    GUI_INS(Theme)(uint32_t fg, uint32_t bg, uint32_t hover, uint32_t accent) {
  GUI_TYPE(TTheme) t = {0};
  GUI_TYPE(TVisual) v = {0};
  v.normal = bg;
  v.hovered = hover;
  v.focused = hover;
  v.active = accent;
  v.disabled = UINT32_C(0xFF343A46);
  v.foreground = fg;
  v.border = UINT32_C(0xFF65748B);
  v.accent = accent;
  v.borderWidth = 1;
  v.radius = 5;
  v.fontScale = 1;
  v.padding.x = 8;
  v.padding.y = 4;
  v.align = GUI_CONST(ALIGN_CENTER);
  t.button = v;
  t.checkbox = v;
  t.radio = v;
  t.slider = v;
  t.input = v;
  t.panel = v;
  t.progress = v;
  t.selection = v;
  v.borderWidth = 0;
  v.radius = 0;
  v.normal = 0;
  v.hovered = 0;
  v.focused = 0;
  v.active = 0;
  v.align = GUI_CONST(ALIGN_LEFT);
  t.text = v;
  return t;
}
GUI_THEME_DARK_PROTOTYPE {
  return GUI_INS(Theme)(UINT32_C(0xFFEAF0FC), UINT32_C(0xFF171D29), UINT32_C(0xFF304E78),
                        UINT32_C(0xFF4F91E8));
}
GUI_THEME_LIGHT_PROTOTYPE {
  return GUI_INS(Theme)(UINT32_C(0xFF182334), UINT32_C(0xFFF4F6FA), UINT32_C(0xFFBFDAFF),
                        UINT32_C(0xFF3779D0));
}
GUI_THEME_CLASSIC_PROTOTYPE {
  return GUI_INS(Theme)(UINT32_C(0xFFAAFFB0), UINT32_C(0xFF102018), UINT32_C(0xFF326044),
                        UINT32_C(0xFF40B85E));
}
GUI_THEME_MONOCHROME_PROTOTYPE {
  return GUI_INS(Theme)(UINT32_C(0xFFEEEEEE), UINT32_C(0xFF101010), UINT32_C(0xFF555555),
                        UINT32_C(0xFF999999));
}
GUI_THEME_DEFAULT_PROTOTYPE { return GUI_FUNC(Theme_Dark)(); }
GUI_THEME_SET_PROTOTYPE {
  const GUI_TYPE(TVisual) * v[9];
  size_t i;
  if (!context || !theme || context->frame)
    return STATUS_CONST(INVALID_ARGUMENT);
  v[0] = &theme->text;
  v[1] = &theme->button;
  v[2] = &theme->checkbox;
  v[3] = &theme->radio;
  v[4] = &theme->slider;
  v[5] = &theme->input;
  v[6] = &theme->panel;
  v[7] = &theme->progress;
  v[8] = &theme->selection;
  for (i = 0; i < 9; ++i)
    if (!GUI_INS(VisualValid)(v[i]))
      return STATUS_CONST(INVALID_ARGUMENT);
  context->theme = *theme;
  context->themeSet = true;
  return STATUS_CONST(SUCCESS);
}
GUI_THEME_GET_PROTOTYPE {
  if (!context || !outTheme)
    return STATUS_CONST(INVALID_ARGUMENT);
  *outTheme = context->themeSet ? context->theme : GUI_FUNC(Theme_Default)();
  return STATUS_CONST(SUCCESS);
}
GUI_SCALE_SET_PROTOTYPE {
  if (!context || context->frame || !(scale > 0 && scale <= 1024))
    return STATUS_CONST(INVALID_ARGUMENT);
  context->scale = scale;
  return STATUS_CONST(SUCCESS);
}
GUI_VISUAL_PUSH_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !GUI_INS(VisualValid)(visual))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (context->visualDepth >= GUI_MAX_SCOPES)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  context->visuals[context->visualDepth++] = *visual;
  return STATUS_CONST(SUCCESS);
}
GUI_VISUAL_POP_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !context->visualDepth)
    return STATUS_CONST(INVALID_ARGUMENT);
  --context->visualDepth;
  return STATUS_CONST(SUCCESS);
}
GUI_BEGIN_PROTOTYPE {
  if (!context || context->frame || !GRAPHICS_FUNC(Surface_IsValid)(surface) ||
      surface->size.x > INT32_MAX || surface->size.y > INT32_MAX)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (!context->scale)
    context->scale = 1;
  if (!(context->scale > 0 && context->scale <= 1024))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (!context->themeSet) {
    context->theme = GUI_FUNC(Theme_Default)();
    context->themeSet = true;
  }
  context->surface = surface;
  context->frame = true;
  context->widgetsStarted = false;
  context->commandCount = 0;
  context->textCount = 0;
  context->eventCount = 0;
  context->idCount = 0;
  context->idDepth = 0;
  context->idScope = 0;
  context->clipDepth = 0;
  context->visualDepth = 0;
  context->layoutDepth = 0;
  context->panelDepth = 0;
  context->pressed = false;
  context->released = false;
  context->scroll = 0;
  context->modal = 0;
  context->modalFrame = 0;
  memset(context->consumed, 0, sizeof(context->consumed));
  return STATUS_CONST(SUCCESS);
}
GUI_END_PROTOTYPE {
  OPSTATUS status = STATUS_CONST(SUCCESS);
  size_t i, j, write = 0;
  if (!GUI_INS(Ready)(context))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (context->clipDepth || context->visualDepth || context->layoutDepth || context->panelDepth ||
      context->idDepth || context->modal)
    status = STATUS_CONST(INVALID_ARGUMENT);
  if (status == STATUS_CONST(SUCCESS)) {
    /* Stable partition: modal commands are rendered after normal commands. */
    for (i = 0; i < context->commandCount; ++i)
      if (!context->commands[i].overlay) {
        GUI_TYPE(TCommand) cmd = context->commands[i];
        for (j = i; j > write; --j)
          context->commands[j] = context->commands[j - 1];
        context->commands[write++] = cmd;
      }
    if (context->renderer)
      status = context->renderer(context->rendererUser, context->surface, context->commands,
                                 context->commandCount, context->scale);
    else
      status = GUI_FUNC(RenderSoftware)(context->surface, context->commands, context->commandCount,
                                        context->scale);
  }
  memcpy(context->previousIds, context->ids, context->idCount * sizeof(context->ids[0]));
  context->previousCount = context->idCount;
  for (i = 0; i < context->idCount; ++i)
    if (context->ids[i] == context->focus)
      break;
  if (i == context->idCount)
    context->focus = context->idCount ? context->ids[0] : 0;
  if (!context->down)
    context->held = 0;
  if (context->previousModal && !context->modalFrame) {
    context->focus = context->savedFocus;
    context->savedFocus = 0;
  }
  context->previousModal = context->modalFrame;
  context->surface = NULL;
  context->frame = false;
  context->clipDepth = 0;
  context->visualDepth = 0;
  context->idDepth = 0;
  context->layoutDepth = 0;
  context->panelDepth = 0;
  context->modal = 0;
  return status;
}
GUI_GET_SIZE_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !outSize)
    return STATUS_CONST(INVALID_ARGUMENT);
  outSize->x = (float)context->surface->size.x / context->scale;
  outSize->y = (float)context->surface->size.y / context->scale;
  return STATUS_CONST(SUCCESS);
}
GUI_GET_ID_PROTOTYPE {
  if (!context || !label)
    return 0;
  return COSMERON_MACRO_INTERNAL_UI_SHARED(HashText)(
      context->idScope ? context->idScope : UINT64_C(14695981039346656037), label);
}
GUI_PUSH_ID_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !scope)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (context->idDepth >= GUI_MAX_SCOPES)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  context->idStack[context->idDepth++] = context->idScope;
  context->idScope = GUI_FUNC(GetId)(context, scope);
  return STATUS_CONST(SUCCESS);
}
GUI_POP_ID_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !context->idDepth)
    return STATUS_CONST(INVALID_ARGUMENT);
  context->idScope = context->idStack[--context->idDepth];
  return STATUS_CONST(SUCCESS);
}
GUI_FOCUS_SET_PROTOTYPE {
  if (!context || !widget)
    return STATUS_CONST(INVALID_ARGUMENT);
  context->focus = widget;
  return STATUS_CONST(SUCCESS);
}
GUI_IS_FOCUSED_PROTOTYPE { return context && widget && context->focus == widget; }
static inline OPSTATUS GUI_INS(FocusStep)(GUI_TYPE(TContext) * c, bool forward) {
  if (!c)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (!c->previousCount)
    return STATUS_CONST(NOT_FOUND);
  c->focus = COSMERON_MACRO_INTERNAL_UI_SHARED(FocusStep)(c->previousIds, c->previousCount,
                                                          c->focus, forward);
  return STATUS_CONST(SUCCESS);
}
GUI_FOCUS_NEXT_PROTOTYPE { return GUI_INS(FocusStep)(context, true); }
GUI_FOCUS_PREVIOUS_PROTOTYPE { return GUI_INS(FocusStep)(context, false); }
GUI_EVENT_PROTOTYPE {
  OPSTATUS status;
  if (!GUI_INS(Ready)(context) || !event || context->widgetsStarted)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (context->eventCount >= GUI_MAX_EVENTS)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  switch (event->type) {
  case INPUT_CONST(EVENT_MOUSE_POSITION):
  case INPUT_CONST(EVENT_MOUSE_MOVE):
  case INPUT_CONST(EVENT_MOUSE_SCROLL):
    if (!GUI_INS(Finite)(event->data.motion.x) || !GUI_INS(Finite)(event->data.motion.y))
      return STATUS_CONST(INVALID_ARGUMENT);
    if (event->type == INPUT_CONST(EVENT_MOUSE_POSITION)) {
      context->pointer.x = event->data.motion.x / context->scale;
      context->pointer.y = event->data.motion.y / context->scale;
      context->pointerKnown = true;
    } else if (event->type == INPUT_CONST(EVENT_MOUSE_MOVE)) {
      float x = context->pointer.x + event->data.motion.x / context->scale,
            y = context->pointer.y + event->data.motion.y / context->scale;
      if (!GUI_INS(Finite)(x) || !GUI_INS(Finite)(y))
        return STATUS_CONST(OUT_OF_RANGE);
      context->pointer.x = x;
      context->pointer.y = y;
      context->pointerKnown = true;
    } else {
      float scroll = context->scroll + event->data.motion.y;
      if (!GUI_INS(Finite)(scroll))
        return STATUS_CONST(OUT_OF_RANGE);
      context->scroll = scroll;
    }
    break;
  case INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS):
  case INPUT_CONST(EVENT_MOUSE_BUTTON_RELEASE):
    if (event->data.mouseButton < 0 || event->data.mouseButton >= INPUT_CONST(MOUSE_BUTTON_COUNT))
      return STATUS_CONST(OUT_OF_RANGE);
    if (event->data.mouseButton == INPUT_CONST(MOUSE_BUTTON_LEFT)) {
      bool down = event->type == INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS);
      if (down && !context->down) {
        context->pressed = true;
        context->pressAt = context->pointer;
      }
      if (!down && context->down) {
        context->released = true;
        context->releaseAt = context->pointer;
      }
      context->down = down;
    }
    break;
  case INPUT_CONST(EVENT_KEY_PRESS):
  case INPUT_CONST(EVENT_KEY_REPEAT):
  case INPUT_CONST(EVENT_KEY_RELEASE):
    if (event->data.key < 0 || event->data.key >= LIB_PREFIX_CONST(INPUT_KEY_COUNT))
      return STATUS_CONST(OUT_OF_RANGE);
    if (event->type != INPUT_CONST(EVENT_KEY_RELEASE) &&
        event->data.key == LIB_PREFIX_CONST(INPUT_KEY_TAB)) {
      (void)GUI_INS(FocusStep)(context, (event->modifiers & INPUT_CONST(MODIFIER_SHIFT)) == 0);
      return STATUS_CONST(SUCCESS);
    }
    break;
  case INPUT_CONST(EVENT_TEXT_INPUT):
  case INPUT_CONST(EVENT_TEXT_COMPOSITION):
    if (event->data.text.length >= sizeof(event->data.text.utf8) ||
        event->data.text.utf8[event->data.text.length] ||
        !GUI_INS(Boundary)(event->data.text.utf8, event->data.text.length,
                           event->data.text.caret) ||
        event->data.text.selection > event->data.text.length - event->data.text.caret ||
        !GUI_INS(Boundary)(event->data.text.utf8, event->data.text.length,
                           event->data.text.caret + event->data.text.selection))
      return STATUS_CONST(INVALID_ARGUMENT);
    status = GUI_INS(UTF8)(event->data.text.utf8, event->data.text.length);
    if (status != STATUS_CONST(SUCCESS))
      return status;
    break;
  case INPUT_CONST(EVENT_FOCUS_LOST):
  case INPUT_CONST(EVENT_SOURCE_REMOVED):
    context->down = false;
    context->pressed = false;
    context->released = false;
    context->held = 0;
    context->pointerKnown = false;
    context->eventCount = 0;
    context->compositionLength = 0;
    break;
  case INPUT_CONST(EVENT_FOCUS_GAINED):
    break;
  default:
    return STATUS_CONST(NOT_SUPPORTED);
  }
  context->events[context->eventCount++] = *event;
  return STATUS_CONST(SUCCESS);
}
GUI_CLIP_PUSH_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !GUI_INS(RegionValid)(region))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (context->clipDepth >= GUI_MAX_SCOPES)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  region = GUI_INS(Intersect)(GUI_INS(Clip)(context), region);
  context->clips[context->clipDepth++] = region;
  return STATUS_CONST(SUCCESS);
}
GUI_CLIP_POP_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !context->clipDepth)
    return STATUS_CONST(INVALID_ARGUMENT);
  --context->clipDepth;
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS GUI_INS(LayoutBegin)(GUI_TYPE(TContext) * c, TQUAD_TYPE(float) r, float gap,
                                            bool row) {
  size_t i;
  if (!GUI_INS(Ready)(c) || !GUI_INS(RegionValid)(r) || !(gap >= 0 && gap <= 1000000))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (c->layoutDepth >= GUI_MAX_SCOPES)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  i = c->layoutDepth++;
  c->layouts[i].region = r;
  c->layouts[i].cursor = 0;
  c->layouts[i].gap = gap;
  c->layouts[i].row = row;
  return STATUS_CONST(SUCCESS);
}
GUI_ROW_BEGIN_PROTOTYPE { return GUI_INS(LayoutBegin)(context, region, gap, true); }
GUI_COLUMN_BEGIN_PROTOTYPE { return GUI_INS(LayoutBegin)(context, region, gap, false); }
GUI_LAYOUT_NEXT_PROTOTYPE {
  size_t i;
  float limit;
  TQUAD_TYPE(float) r;
  if (!GUI_INS(Ready)(context) || !context->layoutDepth || !outRegion ||
      !(extent > 0 && extent <= 1000000))
    return STATUS_CONST(INVALID_ARGUMENT);
  i = context->layoutDepth - 1;
  r = context->layouts[i].region;
  limit = context->layouts[i].row ? r.value3 : r.value4;
  if (extent > limit - context->layouts[i].cursor)
    return STATUS_CONST(OUT_OF_RANGE);
  if (context->layouts[i].row) {
    r.value1 += context->layouts[i].cursor;
    r.value3 = extent;
  } else {
    r.value2 += context->layouts[i].cursor;
    r.value4 = extent;
  }
  *outRegion = r;
  context->layouts[i].cursor += extent + context->layouts[i].gap;
  return STATUS_CONST(SUCCESS);
}
GUI_LAYOUT_END_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !context->layoutDepth)
    return STATUS_CONST(INVALID_ARGUMENT);
  --context->layoutDepth;
  return STATUS_CONST(SUCCESS);
}
GUI_LAYOUT_MEASURE_TEXT_PROTOTYPE {
  TDUAL_TYPE(float) size;
  OPSTATUS s;
  if (!text || !outSize)
    return STATUS_CONST(INVALID_ARGUMENT);
  s = GUI_INS(Measure)(GUI_INS(Font)(font), text, strlen(text), scale, &size);
  if (s == STATUS_CONST(SUCCESS))
    *outSize = size;
  return s;
}
GUI_DRAW_RECTANGLE_PROTOTYPE {
  OPSTATUS s = GUI_INS(Budget)(context, 1, 0);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (!GUI_INS(RegionValid)(region) || !(radius >= 0 && radius <= 1000000))
    return STATUS_CONST(INVALID_ARGUMENT);
  GUI_INS(Rect)(context, region, color, radius);
  return STATUS_CONST(SUCCESS);
}
GUI_DRAW_TEXT_PROTOTYPE {
  GUI_TYPE(TFont) f = GUI_INS(Font)(font);
  TDUAL_TYPE(float) size;
  OPSTATUS s;
  if (!text || !GUI_INS(RegionValid)(region))
    return STATUS_CONST(INVALID_ARGUMENT);
  s = GUI_INS(Measure)(f, text, strlen(text), scale, &size);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  s = GUI_INS(Budget)(context, 1, strlen(text));
  if (s != STATUS_CONST(SUCCESS))
    return s;
  GUI_INS(Text)(context, region, text, strlen(text), color, f, scale);
  context->commands[context->commandCount - 1].clip =
      GUI_INS(Intersect)(GUI_INS(Clip)(context), region);
  return STATUS_CONST(SUCCESS);
}
GUI_IMAGE_PROTOTYPE {
  GUI_TYPE(TCommand) cmd = {0};
  OPSTATUS s = GUI_INS(Budget)(context, 1, 0);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (!GUI_INS(RegionValid)(region) || !GRAPHICS_FUNC(Surface_IsValid)(surface) ||
      GRAPHICS_FUNC(Surface_Overlaps)(surface, context->surface))
    return STATUS_CONST(INVALID_ARGUMENT);
  cmd.type = GUI_CONST(COMMAND_IMAGE);
  cmd.region = region;
  cmd.clip = GUI_INS(Clip)(context);
  cmd.image = surface;
  cmd.overlay = context->modal != 0;
  context->commands[context->commandCount++] = cmd;
  return STATUS_CONST(SUCCESS);
}
GUI_IMAGE_EX_PROTOTYPE {
  if (!image)
    return STATUS_CONST(INVALID_ARGUMENT);
  return GUI_FUNC(Image)(context, image->region, image->surface);
}
