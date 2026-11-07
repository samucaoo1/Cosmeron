#pragma once
/* Geometry is x,y,width,height using value1..value4 of Struct/TQuad. */
static inline bool GUI_INS(Finite)(float v) { return v >= -FLT_MAX && v <= FLT_MAX; }
static inline bool GUI_INS(RegionValid)(TQUAD_TYPE(float) r) {
  return GUI_INS(Finite)(r.value1) && GUI_INS(Finite)(r.value2) && r.value3 > 0 && r.value4 > 0 &&
         r.value3 <= 1000000.0f && r.value4 <= 1000000.0f && r.value1 >= -1000000.0f &&
         r.value1 <= 1000000.0f && r.value2 >= -1000000.0f && r.value2 <= 1000000.0f;
}
static inline TQUAD_TYPE(float) GUI_INS(Region)(float x, float y, float width, float height) {
  TQUAD_TYPE(float) r;
  r.value1 = x;
  r.value2 = y;
  r.value3 = width;
  r.value4 = height;
  return r;
}
static inline TQUAD_TYPE(float) GUI_INS(Intersect)(TQUAD_TYPE(float) a, TQUAD_TYPE(float) b) {
  float x = a.value1 > b.value1 ? a.value1 : b.value1,
        y = a.value2 > b.value2 ? a.value2 : b.value2;
  float right =
      a.value1 + a.value3 < b.value1 + b.value3 ? a.value1 + a.value3 : b.value1 + b.value3;
  float bottom =
      a.value2 + a.value4 < b.value2 + b.value4 ? a.value2 + a.value4 : b.value2 + b.value4;
  return GUI_INS(Region)(x, y, right > x ? right - x : 0, bottom > y ? bottom - y : 0);
}
static inline bool GUI_INS(Contains)(TQUAD_TYPE(float) r, TDUAL_TYPE(float) p) {
  return p.x >= r.value1 && p.y >= r.value2 && p.x < r.value1 + r.value3 &&
         p.y < r.value2 + r.value4;
}
static inline bool GUI_INS(Ready)(const GUI_TYPE(TContext) * c) {
  return c && c->frame && c->surface;
}
static inline TQUAD_TYPE(float) GUI_INS(Clip)(const GUI_TYPE(TContext) * c) {
  if (c->clipDepth)
    return c->clips[c->clipDepth - 1];
  return GUI_INS(Region)(0, 0, (float)c->surface->size.x / c->scale,
                         (float)c->surface->size.y / c->scale);
}
static inline OPSTATUS GUI_INS(Budget)(GUI_TYPE(TContext) * c, size_t commands, size_t text) {
  if (!GUI_INS(Ready)(c))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (commands > GUI_MAX_COMMANDS - c->commandCount || text > GUI_TEXT_CAPACITY - c->textCount)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS GUI_INS(UTF8)(const char *text, size_t length) {
  size_t offset = 0;
  if (!text)
    return STATUS_CONST(INVALID_ARGUMENT);
  while (offset < length) {
    TEXT_TYPE(TChar32) cp = 0;
    size_t consumed = 0;
    OPSTATUS s = TEXT_UTF8_FUNC(Decode)((const unsigned char *)text + offset, length - offset, &cp,
                                        &consumed);
    if (s != STATUS_CONST(SUCCESS))
      return s;
    if (!cp)
      return STATUS_CONST(INVALID_ARGUMENT);
    offset += consumed;
  }
  return STATUS_CONST(SUCCESS);
}
static inline bool GUI_INS(Boundary)(const char *text, size_t length, size_t offset) {
  return COSMERON_MACRO_INTERNAL_UI_SHARED(Boundary)(text, length, offset);
}
static inline size_t GUI_INS(Previous)(const char *text, size_t offset) {
  return COSMERON_MACRO_INTERNAL_UI_SHARED(Previous)(text, offset);
}
static inline size_t GUI_INS(Next)(const char *text, size_t length, size_t offset) {
  return COSMERON_MACRO_INTERNAL_UI_SHARED(Next)(text, length, offset);
}
static inline GUI_TYPE(TFont) GUI_INS(Font)(const GUI_TYPE(TFont) * font) {
  return font ? *font : GUI_FUNC(Font_ASF)(&VGA8x8_Font);
}
static inline bool GUI_INS(FontValid)(GUI_TYPE(TFont) font) {
  return font.height > 0 && font.height <= 1024 && (font.asf || (font.advance && font.coverage));
}
static inline float GUI_INS(Advance)(GUI_TYPE(TFont) font, uint32_t cp) {
  float value;
  if (font.asf) {
    if (cp < 32 || cp > 126)
      cp = '?';
    value = (float)(font.asf->glyphs[cp - 32].columns + font.asf->letterSpacing);
    return value > 0 ? value : font.height;
  }
  value = font.advance(font.user, cp);
  return value > 0 && value <= 1024 ? value : font.height;
}
static inline OPSTATUS GUI_INS(Measure)(GUI_TYPE(TFont) font, const char *text, size_t length,
                                        float scale, TDUAL_TYPE(float) * size) {
  size_t offset = 0;
  float x = 0, max = 0, y = font.height;
  if (!GUI_INS(FontValid)(font) || !(scale > 0 && scale <= 1024))
    return STATUS_CONST(INVALID_ARGUMENT);
  while (offset < length) {
    TEXT_TYPE(TChar32) cp = 0;
    size_t n = 0;
    OPSTATUS s =
        TEXT_UTF8_FUNC(Decode)((const unsigned char *)text + offset, length - offset, &cp, &n);
    if (s != STATUS_CONST(SUCCESS))
      return s;
    if (!cp)
      return STATUS_CONST(INVALID_ARGUMENT);
    offset += n;
    if (cp == '\n') {
      if (x > max)
        max = x;
      x = 0;
      y += font.height + 1;
    } else
      x += GUI_INS(Advance)(font, cp);
    if (x > 1000000 || y > 1000000)
      return STATUS_CONST(OUT_OF_RANGE);
  }
  if (x > max)
    max = x;
  size->x = max * scale;
  size->y = y * scale;
  return STATUS_CONST(SUCCESS);
}
static inline const GUI_TYPE(TVisual) * GUI_INS(Visual)(const GUI_TYPE(TContext) * c,
                                                        const GUI_TYPE(TVisual) * specific,
                                                        const GUI_TYPE(TVisual) * base) {
  if (specific)
    return specific;
  return c->visualDepth ? &c->visuals[c->visualDepth - 1] : base;
}
static inline bool GUI_INS(VisualValid)(const GUI_TYPE(TVisual) * v) {
  return v && v->borderWidth >= 0 && v->borderWidth <= 1024 && v->radius >= 0 &&
         v->radius <= 1000000 && v->padding.x >= 0 && v->padding.x <= 1024 && v->padding.y >= 0 &&
         v->padding.y <= 1024 && v->fontScale > 0 && v->fontScale <= 1024 &&
         GUI_INS(FontValid)(GUI_INS(Font)(v->font)) && v->align >= GUI_CONST(ALIGN_LEFT) &&
         v->align <= GUI_CONST(ALIGN_RIGHT);
}
static inline OPSTATUS GUI_INS(Check)(GUI_TYPE(TContext) * c, TQUAD_TYPE(float) r,
                                      const GUI_TYPE(TVisual) * v, const char *text, size_t extra) {
  OPSTATUS s;
  TDUAL_TYPE(float) size;
  size_t length = text ? strlen(text) : 0;
  if (!GUI_INS(Ready)(c) || !GUI_INS(RegionValid)(r) || !GUI_INS(VisualValid)(v))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (text) {
    s = GUI_INS(Measure)(GUI_INS(Font)(v->font), text, length, v->fontScale, &size);
    if (s != STATUS_CONST(SUCCESS))
      return s;
  }
  return GUI_INS(Budget)(c, 3 + extra, length);
}
static inline OPSTATUS GUI_INS(Register)(GUI_TYPE(TContext) * c, uint64_t id, bool disabled) {
  size_t i;
  if (!id)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (disabled || (c->previousModal && !c->modal))
    return STATUS_CONST(SUCCESS);
  for (i = 0; i < c->idCount; ++i)
    if (c->ids[i] == id)
      return STATUS_CONST(ALREADY_EXISTS);
  if (c->idCount >= GUI_MAX_WIDGETS)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  c->ids[c->idCount++] = id;
  c->widgetsStarted = true;
  if (!c->focus)
    c->focus = id;
  return STATUS_CONST(SUCCESS);
}
static inline bool GUI_INS(Hover)(GUI_TYPE(TContext) * c, TQUAD_TYPE(float) r,
                                  TDUAL_TYPE(float) p) {
  return c->pointerKnown && GUI_INS(Contains)(GUI_INS(Intersect)(r, GUI_INS(Clip)(c)), p);
}
static inline bool GUI_INS(KeyTake)(GUI_TYPE(TContext) * c, INPUT_TYPE(Key) key) {
  size_t i;
  for (i = 0; i < c->eventCount; ++i)
    if (!c->consumed[i] &&
        (c->events[i].type == INPUT_CONST(EVENT_KEY_PRESS) ||
         c->events[i].type == INPUT_CONST(EVENT_KEY_REPEAT)) &&
        c->events[i].data.key == key) {
      c->consumed[i] = true;
      return true;
    }
  return false;
}
static inline bool GUI_INS(Activate)(GUI_TYPE(TContext) * c, uint64_t id, TQUAD_TYPE(float) r,
                                     bool disabled) {
  bool fired;
  if (disabled || (c->previousModal && !c->modal))
    return false;
  fired = COSMERON_MACRO_INTERNAL_UI_SHARED(PointerActivate)(
      &c->focus, &c->held, id, c->pressed, c->released, GUI_INS(Hover)(c, r, c->pressAt),
      GUI_INS(Hover)(c, r, c->releaseAt), disabled);
  if (c->focus == id && (GUI_INS(KeyTake)(c, LIB_PREFIX_CONST(INPUT_KEY_ENTER)) ||
                         GUI_INS(KeyTake)(c, LIB_PREFIX_CONST(INPUT_KEY_SPACE))))
    fired = true;
  return fired;
}
static inline uint32_t GUI_INS(Color)(GUI_TYPE(TContext) * c, const GUI_TYPE(TVisual) * v,
                                      uint64_t id, TQUAD_TYPE(float) r, bool disabled) {
  if (disabled)
    return v->disabled;
  if (id && c->held == id && c->down)
    return v->active;
  if (id && c->focus == id)
    return v->focused;
  return GUI_INS(Hover)(c, r, c->pointer) ? v->hovered : v->normal;
}
static inline void GUI_INS(Rect)(GUI_TYPE(TContext) * c, TQUAD_TYPE(float) r, uint32_t color,
                                 float radius) {
  GUI_TYPE(TCommand) cmd = {0};
  cmd.type = GUI_CONST(COMMAND_RECTANGLE);
  cmd.region = r;
  cmd.clip = GUI_INS(Clip)(c);
  cmd.overlay = c->modal != 0;
  cmd.color = color;
  cmd.radius = radius;
  c->commands[c->commandCount++] = cmd;
}
static inline void GUI_INS(Text)(GUI_TYPE(TContext) * c, TQUAD_TYPE(float) r, const char *text,
                                 size_t length, uint32_t color, GUI_TYPE(TFont) font, float scale) {
  GUI_TYPE(TCommand) cmd = {0};
  cmd.type = GUI_CONST(COMMAND_TEXT);
  cmd.region = r;
  cmd.clip = GUI_INS(Clip)(c);
  cmd.overlay = c->modal != 0;
  cmd.color = color;
  cmd.font = font;
  cmd.scale = scale;
  cmd.length = length;
  cmd.text = c->text + c->textCount;
  memcpy(c->text + c->textCount, text, length);
  c->textCount += length;
  c->commands[c->commandCount++] = cmd;
}
static inline void GUI_INS(Paint)(GUI_TYPE(TContext) * c, TQUAD_TYPE(float) r,
                                  const GUI_TYPE(TVisual) * v, const char *label, uint64_t id,
                                  bool disabled) {
  TQUAD_TYPE(float) inner = r;
  TDUAL_TYPE(float) size;
  float b = v->borderWidth;
  uint32_t color = GUI_INS(Color)(c, v, id, r, disabled);
  if (b > 0) {
    GUI_INS(Rect)(c, r, v->border, v->radius);
    inner = GUI_INS(Region)(r.value1 + b, r.value2 + b, r.value3 - 2 * b, r.value4 - 2 * b);
  }
  if (inner.value3 > 0 && inner.value4 > 0)
    GUI_INS(Rect)(c, inner, color, v->radius > b ? v->radius - b : 0);
  if (label && *label) {
    float x = r.value1 + v->padding.x + b, y;
    GUI_TYPE(TFont) font = GUI_INS(Font)(v->font);
    (void)GUI_INS(Measure)(font, label, strlen(label), v->fontScale, &size);
    if (v->align == GUI_CONST(ALIGN_CENTER))
      x = r.value1 + (r.value3 - size.x) / 2;
    else if (v->align == GUI_CONST(ALIGN_RIGHT))
      x = r.value1 + r.value3 - v->padding.x - b - size.x;
    y = r.value2 + (r.value4 - size.y) / 2;
    {
      size_t start = c->commandCount;
      GUI_INS(Text)
      (c, GUI_INS(Region)(x, y, size.x > 0 ? size.x : 1, size.y), label, strlen(label),
       v->foreground, font, v->fontScale);
      c->commands[start].clip = GUI_INS(Intersect)(GUI_INS(Clip)(c), r);
    }
  }
}
