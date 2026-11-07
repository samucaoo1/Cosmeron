#pragma once
GUI_PANEL_BEGIN_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  TQUAD_TYPE(float) inner;
  if (!panel || !GUI_INS(Ready)(context) || !panel->title)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (context->panelDepth >= GUI_MAX_SCOPES || context->clipDepth >= GUI_MAX_SCOPES)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  v = GUI_INS(Visual)(context, panel->visual, &context->theme.panel);
  s = GUI_INS(Check)(context, panel->region, v, NULL, 1);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (panel->region.value3 <= 2 * v->padding.x || panel->region.value4 <= 2 * v->padding.y + 16)
    return STATUS_CONST(OUT_OF_RANGE);
  s = GUI_INS(UTF8)(panel->title, strlen(panel->title));
  if (s != STATUS_CONST(SUCCESS))
    return s;
  s = GUI_INS(Budget)(context, 4, strlen(panel->title));
  if (s != STATUS_CONST(SUCCESS))
    return s;
  GUI_INS(Paint)(context, panel->region, v, NULL, 0, false);
  GUI_INS(Text)
  (context,
   GUI_INS(Region)(panel->region.value1 + v->padding.x, panel->region.value2 + v->padding.y,
                   panel->region.value3 - 2 * v->padding.x, 16),
   panel->title, strlen(panel->title), v->foreground, GUI_INS(Font)(v->font), v->fontScale);
  context->commands[context->commandCount - 1].clip =
      GUI_INS(Intersect)(GUI_INS(Clip)(context), panel->region);
  inner = GUI_INS(Region)(
      panel->region.value1 + v->padding.x, panel->region.value2 + v->padding.y + 16,
      panel->region.value3 - 2 * v->padding.x, panel->region.value4 - 2 * v->padding.y - 16);
  s = GUI_FUNC(Clip_Push)(context, inner);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  context->panelClips[context->panelDepth++] = context->clipDepth;
  return STATUS_CONST(SUCCESS);
}
GUI_PANEL_BEGIN_PROTOTYPE {
  GUI_TYPE(TPanel) p = {0};
  p.region = region;
  p.title = title;
  return GUI_FUNC(Panel_BeginEx)(context, &p);
}
GUI_PANEL_END_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !context->panelDepth ||
      context->clipDepth != context->panelClips[context->panelDepth - 1])
    return STATUS_CONST(INVALID_ARGUMENT);
  --context->panelDepth;
  return GUI_FUNC(Clip_Pop)(context);
}
GUI_LIST_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  uint64_t id;
  size_t first, selected, visible, i, total = 0;
  float rowHeight;
  bool activated = false, cancelled = false;
  if (!list || !GUI_INS(Ready)(context) || !list->selected || (!list->items && list->count))
    return STATUS_CONST(INVALID_ARGUMENT);
  if (list->count && *list->selected >= list->count)
    return STATUS_CONST(OUT_OF_RANGE);
  v = GUI_INS(Visual)(context, list->visual, &context->theme.selection);
  s = GUI_INS(Check)(context, list->region, v, NULL, 0);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  rowHeight = GUI_INS(Font)(v->font).height * v->fontScale + 2 * v->padding.y;
  if (rowHeight < 1)
    rowHeight = 1;
  if (list->region.value4 / rowHeight > GUI_MAX_COMMANDS)
    return STATUS_CONST(OUT_OF_RANGE);
  visible = (size_t)(list->region.value4 / rowHeight);
  if (!visible)
    visible = 1;
  if (visible > list->count)
    visible = list->count;
  first = list->firstVisible;
  if (first > list->count - visible)
    first = list->count - visible;
  /* Validate every item: keyboard/scroll can change which slice becomes visible. */
  for (i = 0; i < list->count; ++i) {
    size_t length;
    TDUAL_TYPE(float) measured;
    if (!list->items[i])
      return STATUS_CONST(INVALID_ARGUMENT);
    length = strlen(list->items[i]);
    s = GUI_INS(Measure)(GUI_INS(Font)(v->font), list->items[i], length, v->fontScale, &measured);
    if (s != STATUS_CONST(SUCCESS))
      return s;
    if (length > total)
      total = length;
  }
  if (visible && total > GUI_TEXT_CAPACITY / visible)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  s = GUI_INS(Budget)(context, 3 + visible * 3, total * visible);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  id = list->id ? list->id : GUI_INS(RegionId)(context, "list", list->region);
  s = GUI_INS(Register)(context, id, list->disabled);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  selected = *list->selected;
  if (!list->disabled && (!context->previousModal || context->modal)) {
    if (context->pressed && GUI_INS(Hover)(context, list->region, context->pressAt) &&
        !context->held) {
      context->focus = id;
      context->held = id;
    }
    if (context->released && context->held == id &&
        GUI_INS(Hover)(context, list->region, context->releaseAt) && list->count) {
      size_t hit = (size_t)((context->releaseAt.y - list->region.value2) / rowHeight);
      if (hit < visible) {
        selected = first + hit;
        activated = true;
      }
    }
    if (context->focus == id && list->count) {
      if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_UP)) && selected)
        --selected;
      if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_DOWN)) && selected + 1 < list->count)
        ++selected;
      if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_HOME)))
        selected = 0;
      if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_END)))
        selected = list->count - 1;
      activated = GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_ENTER)) || activated;
      cancelled = GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_ESCAPE));
      if (selected != *list->selected) {
        if (selected < first)
          first = selected;
        if (selected >= first + visible)
          first = selected - visible + 1;
      }
    }
    if (GUI_INS(Hover)(context, list->region, context->pointer) && context->scroll != 0) {
      float delta = context->scroll;
      if (delta > 0) {
        size_t amount = delta >= (float)first ? first : (size_t)delta;
        first -= amount;
      } else {
        size_t room = list->count - visible - first;
        float positive = -delta;
        size_t amount = positive >= (float)room ? room : (size_t)positive;
        first += amount;
      }
      context->scroll = 0;
    }
  }
  GUI_INS(Paint)(context, list->region, v, NULL, id, list->disabled);
  for (i = 0; i < visible; ++i) {
    TQUAD_TYPE(float)
    r = GUI_INS(Region)(list->region.value1, list->region.value2 + (float)i * rowHeight,
                        list->region.value3, rowHeight);
    GUI_TYPE(TVisual) row = *v;
    size_t start = context->commandCount;
    row.borderWidth = 0;
    row.radius = 0;
    row.align = GUI_CONST(ALIGN_LEFT);
    if (first + i == selected)
      row.normal = v->accent;
    GUI_INS(Paint)(context, r, &row, list->items[first + i], 0, list->disabled);
    while (start < context->commandCount) {
      context->commands[start].clip =
          GUI_INS(Intersect)(context->commands[start].clip, list->region);
      ++start;
    }
  }
  list->firstVisible = first;
  list->activated = activated;
  list->cancelled = cancelled;
  *list->selected = selected;
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS GUI_INS(ListDirect)(GUI_TYPE(TContext) * c, TQUAD_TYPE(float) r,
                                           const char *const *items, size_t count, size_t *selected,
                                           bool *activated) {
  GUI_TYPE(TList) list = {0};
  size_t index;
  uint64_t id;
  OPSTATUS s;
  if (!GUI_INS(Ready)(c))
    return STATUS_CONST(INVALID_ARGUMENT);
  id = GUI_INS(RegionId)(c, "list", r);
  for (index = 0; index < c->editorCount; ++index)
    if (c->editors[index].id == id)
      break;
  if (index == c->editorCount && index >= GUI_MAX_WIDGETS)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  list.id = id;
  list.region = r;
  list.items = items;
  list.count = count;
  list.selected = selected;
  if (index < c->editorCount)
    list.firstVisible = c->editors[index].first;
  else if (selected)
    list.firstVisible = *selected;
  s = GUI_FUNC(ListEx)(c, &list);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (index == c->editorCount)
    ++c->editorCount;
  c->editors[index].id = id;
  c->editors[index].first = list.firstVisible;
  if (activated)
    *activated = list.activated;
  return STATUS_CONST(SUCCESS);
}
GUI_LIST_PROTOTYPE { return GUI_INS(ListDirect)(context, region, items, count, selected, NULL); }
GUI_MENU_EX_PROTOTYPE { return GUI_FUNC(ListEx)(context, menu); }
GUI_MENU_PROTOTYPE {
  if (!outActivated)
    return STATUS_CONST(INVALID_ARGUMENT);
  return GUI_INS(ListDirect)(context, region, items, count, selected, outActivated);
}
GUI_MODAL_BEGIN_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !id || context->modal || context->widgetsStarted)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (!context->previousModal) {
    context->savedFocus = context->focus;
    context->focus = 0;
    context->held = 0;
  }
  context->modal = id;
  context->modalFrame = id;
  return STATUS_CONST(SUCCESS);
}
GUI_MODAL_END_PROTOTYPE {
  if (!GUI_INS(Ready)(context) || !context->modal)
    return STATUS_CONST(INVALID_ARGUMENT);
  context->previousModal = context->modal;
  context->modal = 0;
  return STATUS_CONST(SUCCESS);
}
GUI_CONFIRM_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  bool accepted = false, rejected = false;
  uint64_t id;
  GUI_TYPE(TButton) yes = {0}, no = {0};
  TQUAD_TYPE(float) r;
  if (!dialog || !GUI_INS(Ready)(context) || !dialog->message || !dialog->title)
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, dialog->visual, &context->theme.panel);
  r = dialog->region;
  s = GUI_INS(Check)(context, r, v, dialog->message, 10);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (r.value3 < 80 || r.value4 < 72)
    return STATUS_CONST(OUT_OF_RANGE);
  {
    TDUAL_TYPE(float) measured;
    s = GUI_INS(Measure)(GUI_INS(Font)(v->font), dialog->title, strlen(dialog->title), v->fontScale,
                         &measured);
    if (s != STATUS_CONST(SUCCESS))
      return s;
  }
  s = GUI_INS(Budget)(context, 14, strlen(dialog->message) + strlen(dialog->title) + 8);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (GUI_MAX_WIDGETS - context->idCount < 2 || context->idDepth >= GUI_MAX_SCOPES)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  id = dialog->id ? dialog->id : GUI_FUNC(GetId)(context, dialog->title);
  s = GUI_FUNC(Modal_Begin)(context, id);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  GUI_INS(Rect)(context, GUI_INS(Clip)(context), UINT32_C(0x99000000), 0);
  GUI_INS(Paint)(context, r, v, NULL, 0, false);
  GUI_INS(Text)
  (context, GUI_INS(Region)(r.value1 + 8, r.value2 + 8, r.value3 - 16, 16), dialog->title,
   strlen(dialog->title), v->foreground, GUI_INS(Font)(v->font), v->fontScale);
  GUI_INS(Text)
  (context, GUI_INS(Region)(r.value1 + 8, r.value2 + 28, r.value3 - 16, r.value4 - 60),
   dialog->message, strlen(dialog->message), v->foreground, GUI_INS(Font)(v->font), v->fontScale);
  (void)GUI_FUNC(PushId)(context, dialog->title);
  yes.id = GUI_FUNC(GetId)(context, "accept");
  yes.region = GUI_INS(Region)(r.value1 + 8, r.value2 + r.value4 - 28, (r.value3 - 24) / 2, 20);
  yes.label = "OK";
  no.id = GUI_FUNC(GetId)(context, "reject");
  no.region = yes.region;
  no.region.value1 += yes.region.value3 + 8;
  no.label = "Cancel";
  s = GUI_FUNC(ButtonEx)(context, &yes);
  if (s == STATUS_CONST(SUCCESS))
    s = GUI_FUNC(ButtonEx)(context, &no);
  accepted = yes.pressed;
  rejected = no.pressed || GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_ESCAPE));
  (void)GUI_FUNC(PopId)(context);
  (void)GUI_FUNC(Modal_End)(context);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  dialog->accepted = accepted;
  dialog->rejected = rejected;
  return STATUS_CONST(SUCCESS);
}
GUI_CONFIRM_PROTOTYPE {
  GUI_TYPE(TDialog) w = {0};
  OPSTATUS s;
  if (!outAccepted || !outRejected)
    return STATUS_CONST(INVALID_ARGUMENT);
  w.region = region;
  w.title = "Confirm";
  w.message = message;
  s = GUI_FUNC(ConfirmEx)(context, &w);
  if (s == STATUS_CONST(SUCCESS)) {
    *outAccepted = w.accepted;
    *outRejected = w.rejected;
  }
  return s;
}
GUI_DIALOG_EX_PROTOTYPE { return GUI_FUNC(ConfirmEx)(context, dialog); }
GUI_DIALOG_PROTOTYPE {
  GUI_TYPE(TDialog) w = {0};
  OPSTATUS s;
  if (!outDismissed)
    return STATUS_CONST(INVALID_ARGUMENT);
  w.region = region;
  w.title = title;
  w.message = message;
  s = GUI_FUNC(DialogEx)(context, &w);
  if (s == STATUS_CONST(SUCCESS))
    *outDismissed = w.accepted || w.rejected;
  return s;
}
GUI_STATUS_BAR_PROTOTYPE {
  GUI_TYPE(TVisual) v;
  TQUAD_TYPE(float) a, b;
  OPSTATUS s;
  if (!GUI_INS(Ready)(context) || !left || !right)
    return STATUS_CONST(INVALID_ARGUMENT);
  v = context->theme.panel;
  s = GUI_INS(Check)(context, region, &v, left, 3);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  s = GUI_INS(Budget)(context, 6, strlen(left) + strlen(right));
  if (s != STATUS_CONST(SUCCESS))
    return s;
  s = GUI_INS(UTF8)(right, strlen(right));
  if (s != STATUS_CONST(SUCCESS))
    return s;
  a = region;
  b = region;
  a.value3 /= 2;
  b.value1 += a.value3;
  b.value3 -= a.value3;
  v.align = GUI_CONST(ALIGN_LEFT);
  GUI_INS(Paint)(context, a, &v, left, 0, false);
  v.align = GUI_CONST(ALIGN_RIGHT);
  GUI_INS(Paint)(context, b, &v, right, 0, false);
  return STATUS_CONST(SUCCESS);
}
GUI_TOAST_PROTOTYPE {
  GUI_TYPE(TLabel) w = {0};
  if (!GUI_INS(Ready)(context) || !message)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (!visible)
    return STATUS_CONST(SUCCESS);
  w.region = region;
  w.text = message;
  w.visual = &context->theme.panel;
  return GUI_FUNC(LabelEx)(context, &w);
}
GUI_SPINNER_PROTOTYPE {
  GUI_TYPE(TLabel) w = {0};
  OPSTATUS s;
  TQUAD_TYPE(float) r;
  if (!GUI_INS(Ready)(context) || !label)
    return STATUS_CONST(INVALID_ARGUMENT);
  s = GUI_INS(Check)(context, region, &context->theme.text, label, 1);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  w.region = region;
  w.text = label;
  s = GUI_FUNC(LabelEx)(context, &w);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  r = GUI_INS(Region)(region.value1 + region.value3 - 8, region.value2 + (region.value4 - 6) / 2, 6,
                      6);
  if ((phase & 1U) == 0)
    GUI_INS(Rect)(context, r, context->theme.progress.accent, 3);
  return STATUS_CONST(SUCCESS);
}
