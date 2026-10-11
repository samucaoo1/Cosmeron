#pragma once
static inline OPSTATUS GUI_INS(EditorValid)(const GUI_TYPE(TInputText) * w) {
  OPSTATUS s;
  if (!w || !w->buffer || !w->capacity || w->length >= w->capacity || w->buffer[w->length] ||
      !GUI_INS(Boundary)(w->buffer, w->length, w->caret) ||
      !GUI_INS(Boundary)(w->buffer, w->length, w->selectionAnchor))
    return STATUS_CONST(INVALID_ARGUMENT);
  s = GUI_INS(UTF8)(w->buffer, w->length);
  return s;
}
static inline void GUI_INS(SelectionDelete)(GUI_TYPE(TInputText) * w) {
  size_t start = w->caret < w->selectionAnchor ? w->caret : w->selectionAnchor;
  size_t end = w->caret > w->selectionAnchor ? w->caret : w->selectionAnchor;
  COSMERON_MACRO_INTERNAL_UI_SHARED(DeleteRange)(w->buffer, &w->length, start, end);
  w->caret = start;
  w->selectionAnchor = start;
}
GUI_INPUT_TEXT_SELECT_PROTOTYPE {
  OPSTATUS s = GUI_INS(EditorValid)(input);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (!GUI_INS(Boundary)(input->buffer, input->length, anchor) ||
      !GUI_INS(Boundary)(input->buffer, input->length, caret))
    return STATUS_CONST(INVALID_ARGUMENT);
  input->selectionAnchor = anchor;
  input->caret = caret;
  return STATUS_CONST(SUCCESS);
}
GUI_INPUT_TEXT_COPY_PROTOTYPE {
  size_t start, end, length;
  OPSTATUS s = GUI_INS(EditorValid)(input);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (!buffer || !outLength)
    return STATUS_CONST(INVALID_ARGUMENT);
  start = input->caret < input->selectionAnchor ? input->caret : input->selectionAnchor;
  end = input->caret > input->selectionAnchor ? input->caret : input->selectionAnchor;
  length = end - start;
  if (length >= capacity)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  memmove(buffer, input->buffer + start, length);
  buffer[length] = 0;
  *outLength = length;
  return STATUS_CONST(SUCCESS);
}
static inline bool GUI_INS(Overlaps)(const char *a, size_t an, const char *b, size_t bn) {
  uintptr_t av = (uintptr_t)a, bv = (uintptr_t)b;
  if (av <= bv)
    return bv - av < an;
  return av - bv < bn;
}
GUI_INPUT_TEXT_CUT_PROTOTYPE {
  OPSTATUS s = GUI_INS(EditorValid)(input);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (!buffer || GUI_INS(Overlaps)(input->buffer, input->capacity, buffer, capacity))
    return STATUS_CONST(INVALID_ARGUMENT);
  s = GUI_FUNC(InputText_Copy)(input, buffer, capacity, outLength);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  GUI_INS(SelectionDelete)(input);
  input->changed = true;
  return STATUS_CONST(SUCCESS);
}
GUI_INPUT_TEXT_PASTE_PROTOTYPE {
  size_t selected;
  OPSTATUS s = GUI_INS(EditorValid)(input);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (!utf8Text || GUI_INS(Overlaps)(input->buffer, input->capacity, utf8Text, byteLength))
    return STATUS_CONST(INVALID_ARGUMENT);
  s = GUI_INS(UTF8)(utf8Text, byteLength);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  selected = input->caret > input->selectionAnchor ? input->caret - input->selectionAnchor
                                                   : input->selectionAnchor - input->caret;
  if (byteLength > input->capacity - 1 - (input->length - selected))
    return STATUS_CONST(INSUFFICIENT_SPACE);
  GUI_INS(SelectionDelete)(input);
  memmove(input->buffer + input->caret + byteLength, input->buffer + input->caret,
          input->length - input->caret + 1);
  memcpy(input->buffer + input->caret, utf8Text, byteLength);
  input->length += byteLength;
  input->caret += byteLength;
  input->selectionAnchor = input->caret;
  input->changed = true;
  return STATUS_CONST(SUCCESS);
}
static inline size_t GUI_INS(HitText)(GUI_TYPE(TInputText) * w, GUI_TYPE(TFont) font, float scale,
                                      TDUAL_TYPE(float) p, bool multiline) {
  size_t offset = 0;
  float x = 0, y = 0, targetX = p.x - w->region.value1 - 5 + w->scrollX,
        targetY = p.y - w->region.value2 - 5 + w->scrollY;
  while (offset < w->length) {
    TEXT_TYPE(TChar32) cp;
    size_t n;
    float advance;
    (void)TEXT_UTF8_FUNC(Decode)((const unsigned char *)w->buffer + offset, w->length - offset, &cp,
                                 &n);
    if (cp == '\n' && multiline) {
      if (targetY < y + (font.height + 1) * scale)
        return offset;
      y += (font.height + 1) * scale;
      x = 0;
      offset += n;
      continue;
    }
    advance = GUI_INS(Advance)(font, cp) * scale;
    if (targetY < y + (font.height + 1) * scale && targetX < x + advance / 2)
      return offset;
    x += advance;
    offset += n;
  }
  return w->length;
}
static inline OPSTATUS GUI_INS(Editor)(GUI_TYPE(TContext) * c, GUI_TYPE(TInputText) * input,
                                       bool multiline) {
  GUI_TYPE(TInputText) w;
  const GUI_TYPE(TVisual) * v;
  GUI_TYPE(TFont) font;
  OPSTATUS s;
  size_t i, offset, selectionStart, selectionEnd, renderStart;
  uint64_t id;
  char temporary[GUI_TEXT_CAPACITY], composition[64];
  bool consumed[GUI_MAX_EVENTS];
  size_t compositionLength = c ? c->compositionLength : 0,
         compositionCaret = c ? c->compositionCaret : 0,
         compositionSelection = c ? c->compositionSelection : 0, selectionCommands;
  float oldScrollX, oldScrollY;
  float x = 0, y = 0, caretX = 0, caretY = 0;
  bool active;
  s = GUI_INS(EditorValid)(input);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (!GUI_INS(Ready)(c))
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(c, input->visual, &c->theme.input);
  s = GUI_INS(Check)(c, input->region, v, input->buffer, 0);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (input->length >= sizeof(temporary))
    return STATUS_CONST(INSUFFICIENT_SPACE);
  w = *input;
  memcpy(temporary, w.buffer, w.length + 1);
  w.buffer = temporary;
  memcpy(composition, c->composition, sizeof(composition));
  if (w.capacity > sizeof(temporary))
    w.capacity = sizeof(temporary);
  w.changed = false;
  memcpy(consumed, c->consumed, sizeof(consumed));
  font = GUI_INS(Font)(v->font);
  id = w.id ? w.id : GUI_INS(RegionId)(c, multiline ? "textarea" : "input", w.region);
  s = GUI_INS(Register)(c, id, w.disabled);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  active = !w.disabled && (!c->previousModal || c->modal);
  if (active && c->pressed && GUI_INS(Hover)(c, w.region, c->pressAt) &&
      (!c->held || c->held == id)) {
    c->held = id;
    c->focus = id;
    w.caret = GUI_INS(HitText)(&w, font, v->fontScale, c->pressAt, multiline);
    w.selectionAnchor = w.caret;
    compositionLength = 0;
  }
  if (active && c->held == id && (c->down || c->released))
    w.caret = GUI_INS(HitText)(&w, font, v->fontScale, c->released ? c->releaseAt : c->pointer,
                               multiline);
  if (active && c->focus == id)
    for (i = 0; i < c->eventCount; ++i)
      if (!consumed[i]) {
        INPUT_TYPE(TEvent) *e = &c->events[i];
        if (e->type == INPUT_CONST(EVENT_TEXT_INPUT)) {
          if (!multiline && memchr(e->data.text.utf8, '\n', e->data.text.length)) {
            consumed[i] = true;
            continue;
          }
          s = GUI_FUNC(InputText_Paste)(&w, e->data.text.utf8, e->data.text.length);
          if (s != STATUS_CONST(SUCCESS))
            return s;
          compositionLength = 0;
          consumed[i] = true;
        } else if (e->type == INPUT_CONST(EVENT_TEXT_COMPOSITION)) {
          memcpy(composition, e->data.text.utf8, e->data.text.length + 1);
          compositionLength = e->data.text.length;
          compositionCaret = e->data.text.caret;
          compositionSelection = e->data.text.selection;
          consumed[i] = true;
        } else if (e->type == INPUT_CONST(EVENT_KEY_PRESS) ||
                   e->type == INPUT_CONST(EVENT_KEY_REPEAT)) {
          INPUT_TYPE(Key) key = e->data.key;
          bool shift = (e->modifiers & INPUT_CONST(MODIFIER_SHIFT)) != 0;
          if ((e->modifiers & INPUT_CONST(MODIFIER_CONTROL)) &&
              key == LIB_PREFIX_CONST(INPUT_KEY_A)) {
            w.selectionAnchor = 0;
            w.caret = w.length;
          } else if (key == LIB_PREFIX_CONST(INPUT_KEY_LEFT))
            w.caret = GUI_INS(Previous)(w.buffer, w.caret);
          else if (key == LIB_PREFIX_CONST(INPUT_KEY_RIGHT))
            w.caret = GUI_INS(Next)(w.buffer, w.length, w.caret);
          else if (multiline && key == LIB_PREFIX_CONST(INPUT_KEY_UP))
            w.caret =
                COSMERON_MACRO_INTERNAL_UI_SHARED(Vertical)(w.buffer, w.length, w.caret, false);
          else if (multiline && key == LIB_PREFIX_CONST(INPUT_KEY_DOWN))
            w.caret =
                COSMERON_MACRO_INTERNAL_UI_SHARED(Vertical)(w.buffer, w.length, w.caret, true);
          else if (key == LIB_PREFIX_CONST(INPUT_KEY_HOME))
            w.caret =
                multiline ? COSMERON_MACRO_INTERNAL_UI_SHARED(LineStart)(w.buffer, w.caret) : 0;
          else if (key == LIB_PREFIX_CONST(INPUT_KEY_END))
            w.caret = multiline
                          ? COSMERON_MACRO_INTERNAL_UI_SHARED(LineEnd)(w.buffer, w.length, w.caret)
                          : w.length;
          else if (key == LIB_PREFIX_CONST(INPUT_KEY_BACKSPACE) ||
                   key == LIB_PREFIX_CONST(INPUT_KEY_DELETE)) {
            if (w.caret == w.selectionAnchor) {
              if (key == LIB_PREFIX_CONST(INPUT_KEY_BACKSPACE))
                w.selectionAnchor = GUI_INS(Previous)(w.buffer, w.caret);
              else
                w.selectionAnchor = GUI_INS(Next)(w.buffer, w.length, w.caret);
            }
            if (w.caret != w.selectionAnchor) {
              GUI_INS(SelectionDelete)(&w);
              w.changed = true;
            }
          } else if (key == LIB_PREFIX_CONST(INPUT_KEY_ENTER) && multiline) {
            /* Hosts normally emit the committed newline as a text event. */
            continue;
          } else
            continue;
          if (!shift && key != LIB_PREFIX_CONST(INPUT_KEY_A))
            w.selectionAnchor = w.caret;
          consumed[i] = true;
        }
      }
  s = GUI_INS(Check)(c, w.region, v, w.buffer, w.length + 3);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  s = GUI_INS(Budget)(c, w.length + 6, w.length + compositionLength);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  oldScrollX = w.scrollX;
  oldScrollY = w.scrollY;
  /* Selection/caret commands precede text so the glyphs remain visible. */
  renderStart = c->commandCount;
  GUI_INS(Paint)(c, w.region, v, NULL, id, w.disabled);
  selectionCommands = c->commandCount;
  selectionStart = w.caret < w.selectionAnchor ? w.caret : w.selectionAnchor;
  selectionEnd = w.caret > w.selectionAnchor ? w.caret : w.selectionAnchor;
  for (offset = 0; offset <= w.length;) {
    TEXT_TYPE(TChar32) cp;
    size_t n;
    float advance;
    if (offset == w.caret) {
      caretX = x;
      caretY = y;
    }
    if (offset == w.length)
      break;
    (void)TEXT_UTF8_FUNC(Decode)((const unsigned char *)w.buffer + offset, w.length - offset, &cp,
                                 &n);
    advance = GUI_INS(Advance)(font, cp) * v->fontScale;
    if (offset >= selectionStart && offset < selectionEnd)
      GUI_INS(Rect)
      (c,
       GUI_INS(Region)(w.region.value1 + 5 + x - w.scrollX, w.region.value2 + 5 + y - w.scrollY,
                       advance, font.height * v->fontScale),
       v->accent, 0);
    if (cp == '\n' && multiline) {
      x = 0;
      y += (font.height + 1) * v->fontScale;
    } else
      x += advance;
    offset += n;
  }
  if (caretX - w.scrollX > w.region.value3 - 12)
    w.scrollX = caretX - (w.region.value3 - 12);
  if (caretX - w.scrollX < 0)
    w.scrollX = caretX;
  if (multiline && caretY - w.scrollY > w.region.value4 - font.height * v->fontScale - 8)
    w.scrollY = caretY - (w.region.value4 - font.height * v->fontScale - 8);
  if (caretY - w.scrollY < 0)
    w.scrollY = caretY;
  if (w.scrollX < 0)
    w.scrollX = 0;
  if (w.scrollY < 0)
    w.scrollY = 0;
  for (i = selectionCommands; i < c->commandCount; ++i) {
    c->commands[i].region.value1 += oldScrollX - w.scrollX;
    c->commands[i].region.value2 += oldScrollY - w.scrollY;
  }
  GUI_INS(Text)
  (c,
   GUI_INS(Region)(w.region.value1 + 5 - w.scrollX, w.region.value2 + 5 - w.scrollY,
                   w.region.value3, w.region.value4),
   w.buffer, w.length, v->foreground, font, v->fontScale);
  if (c->focus == id && active)
    GUI_INS(Rect)
    (c,
     GUI_INS(Region)(w.region.value1 + 5 + caretX - w.scrollX,
                     w.region.value2 + 5 + caretY - w.scrollY, 1, font.height * v->fontScale),
     v->foreground, 0);
  for (i = renderStart; i < c->commandCount; ++i)
    c->commands[i].clip = GUI_INS(Intersect)(c->commands[i].clip, w.region);
  if (active && c->focus == id && compositionLength) {
    GUI_INS(Text)
    (c,
     GUI_INS(Region)(w.region.value1 + 5 + caretX - w.scrollX,
                     w.region.value2 + 5 + caretY - w.scrollY, w.region.value3,
                     font.height * v->fontScale),
     composition, compositionLength, v->accent, font, v->fontScale);
    c->commands[c->commandCount - 1].clip = GUI_INS(Intersect)(GUI_INS(Clip)(c), w.region);
  }
  memcpy(c->composition, composition, sizeof(composition));
  c->compositionLength = compositionLength;
  c->compositionCaret = compositionCaret;
  c->compositionSelection = compositionSelection;
  memcpy(input->buffer, w.buffer, w.length + 1);
  w.buffer = input->buffer;
  w.capacity = input->capacity;
  *input = w;
  memcpy(c->consumed, consumed, sizeof(consumed));
  return STATUS_CONST(SUCCESS);
}
GUI_INPUT_TEXT_EX_PROTOTYPE { return GUI_INS(Editor)(context, input, false); }
GUI_TEXT_AREA_EX_PROTOTYPE { return GUI_INS(Editor)(context, input, true); }
static inline OPSTATUS GUI_INS(EditorDirect)(GUI_TYPE(TContext) * c, TQUAD_TYPE(float) r,
                                             char *buffer, size_t capacity, size_t *length,
                                             bool multiline) {
  GUI_TYPE(TInputText) w = {0};
  size_t index;
  uint64_t id;
  OPSTATUS s;
  if (!GUI_INS(Ready)(c) || !length)
    return STATUS_CONST(INVALID_ARGUMENT);
  id = GUI_INS(RegionId)(c, multiline ? "textarea" : "input", r);
  for (index = 0; index < c->editorCount; ++index)
    if (c->editors[index].id == id)
      break;
  if (index == c->editorCount && index >= GUI_MAX_WIDGETS)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  w.id = id;
  w.region = r;
  w.buffer = buffer;
  w.capacity = capacity;
  w.length = *length;
  if (index < c->editorCount) {
    w.caret = c->editors[index].caret;
    w.selectionAnchor = c->editors[index].anchor;
    w.scrollX = c->editors[index].scrollX;
    w.scrollY = c->editors[index].scrollY;
  } else {
    w.caret = w.length;
    w.selectionAnchor = w.length;
  }
  if (w.caret > w.length)
    w.caret = w.length;
  if (w.selectionAnchor > w.length)
    w.selectionAnchor = w.length;
  s = GUI_INS(Editor)(c, &w, multiline);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (index == c->editorCount)
    ++c->editorCount;
  c->editors[index].id = id;
  c->editors[index].caret = w.caret;
  c->editors[index].anchor = w.selectionAnchor;
  c->editors[index].scrollX = w.scrollX;
  c->editors[index].scrollY = w.scrollY;
  *length = w.length;
  return STATUS_CONST(SUCCESS);
}
GUI_INPUT_TEXT_PROTOTYPE {
  return GUI_INS(EditorDirect)(context, region, buffer, capacity, inoutLength, false);
}
GUI_TEXT_AREA_PROTOTYPE {
  return GUI_INS(EditorDirect)(context, region, buffer, capacity, inoutLength, true);
}
