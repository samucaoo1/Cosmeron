#pragma once
#include <stdio.h>
static inline uint64_t GUI_INS(ChildId)(uint64_t parent, size_t index) {
  uint64_t id = (parent ^ (uint64_t)index) * UINT64_C(1099511628211);
  return id ? id : 1;
}
GUI_IMAGE_BUTTON_EX_PROTOTYPE {
  GUI_TYPE(TButton) w = {0};
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  if (!button || !GUI_INS(Ready)(context) || !GRAPHICS_FUNC(Surface_IsValid)(button->surface) ||
      GRAPHICS_FUNC(Surface_Overlaps)(button->surface, context->surface))
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, button->visual, &context->theme.button);
  s = GUI_INS(Check)(context, button->region, v, NULL, 1);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  w.id = button->id ? button->id : GUI_INS(RegionId)(context, "imagebutton", button->region);
  w.region = button->region;
  w.label = "";
  w.visual = v;
  w.disabled = button->disabled;
  s = GUI_FUNC(ButtonEx)(context, &w);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  s = GUI_FUNC(Image)(context, button->region, button->surface);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  button->pressed = w.pressed;
  return STATUS_CONST(SUCCESS);
}
GUI_IMAGE_BUTTON_PROTOTYPE {
  GUI_TYPE(TImageButton) w = {0};
  OPSTATUS s;
  if (!outPressed)
    return STATUS_CONST(INVALID_ARGUMENT);
  w.region = region;
  w.surface = surface;
  s = GUI_FUNC(ImageButtonEx)(context, &w);
  if (s == STATUS_CONST(SUCCESS))
    *outPressed = w.pressed;
  return s;
}
static inline OPSTATUS GUI_INS(ItemsCheck)(GUI_TYPE(TContext) * c, TQUAD_TYPE(float) r,
                                           const GUI_TYPE(TVisual) * v, const char *const *items,
                                           size_t count, size_t extra) {
  size_t i, total = 0;
  OPSTATUS s = GUI_INS(Check)(c, r, v, NULL, extra);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (!count || !items || count > GUI_MAX_COMMANDS)
    return STATUS_CONST(INVALID_ARGUMENT);
  for (i = 0; i < count; ++i) {
    TDUAL_TYPE(float) measured;
    size_t n;
    if (!items[i])
      return STATUS_CONST(INVALID_ARGUMENT);
    n = strlen(items[i]);
    if (n > GUI_TEXT_CAPACITY - total)
      return STATUS_CONST(INSUFFICIENT_SPACE);
    s = GUI_INS(Measure)(GUI_INS(Font)(v->font), items[i], n, v->fontScale, &measured);
    if (s != STATUS_CONST(SUCCESS))
      return s;
    total += n;
  }
  return GUI_INS(Budget)(c, 3 + extra, total);
}
GUI_TABS_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  uint64_t id;
  size_t selected, i;
  OPSTATUS s;
  if (!tabs || !GUI_INS(Ready)(context) || !tabs->selected)
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, tabs->visual, &context->theme.button);
  s = GUI_INS(ItemsCheck)(context, tabs->region, v, tabs->items, tabs->count, tabs->count * 3);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (*tabs->selected >= tabs->count)
    return STATUS_CONST(OUT_OF_RANGE);
  id = tabs->id ? tabs->id : GUI_INS(RegionId)(context, "tabs", tabs->region);
  s = GUI_INS(Register)(context, id, tabs->disabled);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  selected = *tabs->selected;
  if (GUI_INS(Activate)(context, id, tabs->region, tabs->disabled) && context->released &&
      context->held == id && GUI_INS(Hover)(context, tabs->region, context->releaseAt)) {
    selected = (size_t)((context->releaseAt.x - tabs->region.value1) / tabs->region.value3 *
                        (float)tabs->count);
    if (selected >= tabs->count)
      selected = tabs->count - 1;
  }
  if (!tabs->disabled && (!context->previousModal || context->modal) && context->focus == id) {
    if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_LEFT)) && selected)
      --selected;
    if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_RIGHT)) && selected + 1 < tabs->count)
      ++selected;
  }
  for (i = 0; i < tabs->count; ++i) {
    GUI_TYPE(TVisual) visual = *v;
    TQUAD_TYPE(float) r = tabs->region;
    r.value3 /= (float)tabs->count;
    r.value1 += (float)i * r.value3;
    if (i == selected) {
      visual.normal = v->accent;
      visual.focused = v->accent;
      visual.hovered = v->accent;
    }
    GUI_INS(Paint)(context, r, &visual, tabs->items[i], i == selected ? id : 0, tabs->disabled);
  }
  *tabs->selected = selected;
  return STATUS_CONST(SUCCESS);
}
GUI_TABS_PROTOTYPE {
  GUI_TYPE(TTabs) w = {0};
  w.region = region;
  w.items = items;
  w.count = count;
  w.selected = selected;
  return GUI_FUNC(TabsEx)(context, &w);
}
GUI_COMBO_BOX_EX_PROTOTYPE {
  GUI_TYPE(TButton) button = {0};
  GUI_TYPE(TList) list = {0};
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  bool open;
  uint64_t id;
  if (!combo || !GUI_INS(Ready)(context) || !combo->selected)
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, combo->visual, &context->theme.button);
  s = GUI_INS(ItemsCheck)(context, combo->region, v, combo->items, combo->count,
                          combo->count * 3 + 3);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (*combo->selected >= combo->count)
    return STATUS_CONST(OUT_OF_RANGE);
  if (GUI_MAX_WIDGETS - context->idCount < 2)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  id = combo->id ? combo->id : GUI_INS(RegionId)(context, "combo", combo->region);
  button.id = id;
  button.region = combo->region;
  button.label = combo->items[*combo->selected];
  button.visual = v;
  button.disabled = combo->disabled;
  s = GUI_FUNC(ButtonEx)(context, &button);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  open = combo->open;
  if (button.pressed)
    open = !open;
  if (combo->disabled)
    open = false;
  if (open) {
    size_t start = context->commandCount, i;
    list.id = GUI_INS(ChildId)(id, 1);
    list.items = combo->items;
    list.count = combo->count;
    list.selected = combo->selected;
    list.visual = v;
    list.region = GUI_INS(Region)(
        combo->region.value1, combo->region.value2 + combo->region.value4, combo->region.value3,
        (GUI_INS(Font)(v->font).height * v->fontScale + 2 * v->padding.y) *
            (float)(combo->count < 6 ? combo->count : 6));
    list.firstVisible = combo->firstVisible;
    if (button.pressed && *list.selected > 5)
      list.firstVisible = *list.selected - 5;
    if (button.pressed)
      context->focus = list.id;
    s = GUI_FUNC(ListEx)(context, &list);
    if (s != STATUS_CONST(SUCCESS))
      return s;
    combo->firstVisible = list.firstVisible;
    for (i = start; i < context->commandCount; ++i)
      context->commands[i].overlay = true;
    if (list.activated || list.cancelled ||
        (context->pressed && !GUI_INS(Hover)(context, list.region, context->pressAt) &&
         !GUI_INS(Hover)(context, button.region, context->pressAt)))
      open = false;
  }
  combo->open = open;
  return STATUS_CONST(SUCCESS);
}
GUI_COMBO_BOX_PROTOTYPE {
  GUI_TYPE(TComboBox) w = {0};
  OPSTATUS s;
  size_t index;
  uint64_t id;
  if (!GUI_INS(Ready)(context) || !open)
    return STATUS_CONST(INVALID_ARGUMENT);
  id = GUI_INS(RegionId)(context, "combo", region);
  for (index = 0; index < context->editorCount; ++index)
    if (context->editors[index].id == id)
      break;
  if (index == context->editorCount && index >= GUI_MAX_WIDGETS)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  w.id = id;
  w.region = region;
  w.items = items;
  w.count = count;
  w.selected = selected;
  w.open = *open;
  if (index < context->editorCount)
    w.firstVisible = context->editors[index].first;
  else if (selected)
    w.firstVisible = *selected;
  s = GUI_FUNC(ComboBoxEx)(context, &w);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (index == context->editorCount)
    ++context->editorCount;
  context->editors[index].id = id;
  context->editors[index].first = w.firstVisible;
  *open = w.open;
  return STATUS_CONST(SUCCESS);
}
GUI_TREE_NODE_EX_PROTOTYPE {
  GUI_TYPE(TButton) b = {0};
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  TQUAD_TYPE(float) indicator;
  if (!node || !GUI_INS(Ready)(context) || !node->label)
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, node->visual, &context->theme.button);
  s = GUI_INS(Check)(context, node->region, v, node->label, 1);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  b.id = node->id ? node->id : GUI_FUNC(GetId)(context, node->label);
  b.region = node->region;
  b.label = node->label;
  b.visual = v;
  b.disabled = node->disabled;
  s = GUI_FUNC(ButtonEx)(context, &b);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (b.pressed) {
    node->expanded = !node->expanded;
    node->selected = true;
  }
  indicator = GUI_INS(Region)(node->region.value1 + 4,
                              node->region.value2 + node->region.value4 / 2 - 2, 8, 4);
  if (!node->expanded)
    indicator = GUI_INS(Region)(indicator.value1 + 2, indicator.value2 - 2, 4, 8);
  GUI_INS(Rect)(context, indicator, v->accent, 1);
  return STATUS_CONST(SUCCESS);
}
GUI_TREE_NODE_PROTOTYPE {
  GUI_TYPE(TTreeNode) w = {0};
  OPSTATUS s;
  if (!expanded || !selected)
    return STATUS_CONST(INVALID_ARGUMENT);
  w.region = region;
  w.label = label;
  w.expanded = *expanded;
  w.selected = *selected;
  s = GUI_FUNC(TreeNodeEx)(context, &w);
  if (s == STATUS_CONST(SUCCESS)) {
    *expanded = w.expanded;
    *selected = w.selected;
  }
  return s;
}
GUI_TABLE_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  uint64_t id;
  size_t i, j, visible, first, selected, total = 0, count;
  float rowHeight;
  if (!table || !GUI_INS(Ready)(context) || !table->selectedRow || !table->rows ||
      !table->columns || !table->cells || table->rows > SIZE_MAX / table->columns)
    return STATUS_CONST(INVALID_ARGUMENT);
  if (*table->selectedRow >= table->rows)
    return STATUS_CONST(OUT_OF_RANGE);
  v = GUI_INS(Visual)(context, table->visual, &context->theme.selection);
  s = GUI_INS(Check)(context, table->region, v, NULL, 0);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  rowHeight = GUI_INS(Font)(v->font).height * v->fontScale + 2 * v->padding.y;
  if (rowHeight < 1)
    rowHeight = 1;
  visible = (size_t)(table->region.value4 / rowHeight);
  if (!visible)
    visible = 1;
  if (visible > table->rows)
    visible = table->rows;
  if (table->columns > GUI_MAX_COMMANDS / 3 / visible)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  count = table->rows * table->columns;
  for (i = 0; i < count; ++i) {
    size_t length;
    TDUAL_TYPE(float) measured;
    if (!table->cells[i])
      return STATUS_CONST(INVALID_ARGUMENT);
    length = strlen(table->cells[i]);
    s = GUI_INS(Measure)(GUI_INS(Font)(v->font), table->cells[i], length, v->fontScale, &measured);
    if (s != STATUS_CONST(SUCCESS))
      return s;
    if (length > total)
      total = length;
  }
  if (total > GUI_TEXT_CAPACITY / (visible * table->columns))
    return STATUS_CONST(INSUFFICIENT_SPACE);
  s = GUI_INS(Budget)(context, 3 + visible * table->columns * 3, total * visible * table->columns);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  id = table->id ? table->id : GUI_INS(RegionId)(context, "table", table->region);
  s = GUI_INS(Register)(context, id, table->disabled);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  selected = *table->selectedRow;
  first = table->firstVisible;
  if (first > table->rows - visible)
    first = table->rows - visible;
  if (GUI_INS(Activate)(context, id, table->region, table->disabled) && context->released &&
      context->held == id && GUI_INS(Hover)(context, table->region, context->releaseAt)) {
    size_t hit = (size_t)((context->releaseAt.y - table->region.value2) / rowHeight);
    if (hit < visible)
      selected = first + hit;
  }
  if (!table->disabled && (!context->previousModal || context->modal) && context->focus == id) {
    if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_UP)) && selected)
      --selected;
    if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_DOWN)) && selected + 1 < table->rows)
      ++selected;
    if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_HOME)))
      selected = 0;
    if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_END)))
      selected = table->rows - 1;
  }
  if (selected != *table->selectedRow) {
    if (selected < first)
      first = selected;
    if (selected >= first + visible)
      first = selected - visible + 1;
  }
  if (!table->disabled && (!context->previousModal || context->modal) &&
      GUI_INS(Hover)(context, table->region, context->pointer) && context->scroll) {
    float delta = context->scroll;
    if (delta > 0) {
      size_t amount = delta >= (float)first ? first : (size_t)delta;
      first -= amount;
    } else {
      size_t room = table->rows - visible - first;
      float positive = -delta;
      size_t amount = positive >= (float)room ? room : (size_t)positive;
      first += amount;
    }
    context->scroll = 0;
  }
  for (i = 0; i < visible; ++i)
    for (j = 0; j < table->columns; ++j) {
      GUI_TYPE(TVisual) cell = *v;
      size_t start = context->commandCount;
      TQUAD_TYPE(float)
      r = GUI_INS(Region)(table->region.value1 +
                              (float)j * table->region.value3 / (float)table->columns,
                          table->region.value2 + (float)i * rowHeight,
                          table->region.value3 / (float)table->columns, rowHeight);
      cell.radius = 0;
      cell.align = GUI_CONST(ALIGN_LEFT);
      if (first + i == selected)
        cell.normal = v->accent;
      GUI_INS(Paint)
      (context, r, &cell, table->cells[(first + i) * table->columns + j], 0, table->disabled);
      while (start < context->commandCount) {
        context->commands[start].clip =
            GUI_INS(Intersect)(context->commands[start].clip, table->region);
        ++start;
      }
    }
  *table->selectedRow = selected;
  table->firstVisible = first;
  return STATUS_CONST(SUCCESS);
}
GUI_TABLE_PROTOTYPE {
  GUI_TYPE(TTable) w = {0};
  OPSTATUS s;
  size_t index;
  uint64_t id;
  if (!GUI_INS(Ready)(context))
    return STATUS_CONST(INVALID_ARGUMENT);
  id = GUI_INS(RegionId)(context, "table", region);
  for (index = 0; index < context->editorCount; ++index)
    if (context->editors[index].id == id)
      break;
  if (index == context->editorCount && index >= GUI_MAX_WIDGETS)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  w.id = id;
  w.region = region;
  w.cells = cells;
  w.rows = rows;
  w.columns = columns;
  w.selectedRow = selectedRow;
  if (index < context->editorCount)
    w.firstVisible = context->editors[index].first;
  else if (selectedRow)
    w.firstVisible = *selectedRow;
  s = GUI_FUNC(TableEx)(context, &w);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (index == context->editorCount)
    ++context->editorCount;
  context->editors[index].id = id;
  context->editors[index].first = w.firstVisible;
  return STATUS_CONST(SUCCESS);
}
GUI_COLOR_PICKER_EX_PROTOTYPE {
  GUI_TYPE(TSlider) slider = {0};
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  size_t i, j;
  uint64_t id;
  uint32_t color = 0;
  if (!picker || !GUI_INS(Ready)(context))
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, picker->visual, &context->theme.slider);
  s = GUI_INS(Check)(context, picker->region, v, NULL, 20);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  if (GUI_MAX_WIDGETS - context->idCount < 4)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  id = picker->id ? picker->id : GUI_INS(RegionId)(context, "colorpicker", picker->region);
  for (i = 0; i < 4; ++i)
    for (j = 0; j < context->idCount; ++j)
      if (context->ids[j] == GUI_INS(ChildId)(id, i + 1))
        return STATUS_CONST(ALREADY_EXISTS);
  for (i = 0; i < 4; ++i) {
    unsigned shift = (unsigned)(3 - i) * 8;
    slider.id = GUI_INS(ChildId)(id, i + 1);
    slider.region = picker->region;
    slider.region.value4 /= 4;
    slider.region.value2 += (float)i * slider.region.value4;
    slider.minimum = 0;
    slider.maximum = 255;
    slider.value = (float)((picker->color >> shift) & 255U);
    slider.visual = v;
    slider.disabled = picker->disabled;
    s = GUI_FUNC(SliderEx)(context, &slider);
    if (s != STATUS_CONST(SUCCESS))
      return s;
    color |= (uint32_t)(slider.value + 0.5f) << shift;
  }
  picker->color = color;
  return STATUS_CONST(SUCCESS);
}
GUI_COLOR_PICKER_PROTOTYPE {
  GUI_TYPE(TColorPicker) w = {0};
  OPSTATUS s;
  if (!color)
    return STATUS_CONST(INVALID_ARGUMENT);
  w.region = region;
  w.color = *color;
  s = GUI_FUNC(ColorPickerEx)(context, &w);
  if (s == STATUS_CONST(SUCCESS))
    *color = w.color;
  return s;
}
GUI_NUMBER_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  uint64_t id;
  double value;
  char label[64];
  if (!number || !GUI_INS(Ready)(context) || !GUI_INS(Finite)(number->minimum) ||
      !GUI_INS(Finite)(number->maximum) || !GUI_INS(Finite)(number->value) ||
      !(number->maximum >= number->minimum) || !(number->step > 0 && number->step <= FLT_MAX))
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, number->visual, &context->theme.input);
  s = GUI_INS(Check)(context, number->region, v, NULL, 0);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  s = GUI_INS(Budget)(context, 3, sizeof(label));
  if (s != STATUS_CONST(SUCCESS))
    return s;
  id = number->id ? number->id : GUI_INS(RegionId)(context, "number", number->region);
  s = GUI_INS(Register)(context, id, number->disabled);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  (void)GUI_INS(Activate)(context, id, number->region, number->disabled);
  value = number->value;
  if (!number->disabled && (!context->previousModal || context->modal) && context->focus == id) {
    if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_RIGHT)) ||
        GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_UP)))
      value += number->step;
    if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_LEFT)) ||
        GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_DOWN)))
      value -= number->step;
    if (GUI_INS(Hover)(context, number->region, context->pointer) && context->scroll) {
      value += (double)context->scroll * (double)number->step;
      context->scroll = 0;
    }
  }
  if (value < number->minimum)
    value = number->minimum;
  if (value > number->maximum)
    value = number->maximum;
  (void)snprintf(label, sizeof(label), "%.7g", (double)value);
  GUI_INS(Paint)(context, number->region, v, label, id, number->disabled);
  number->value = (float)value;
  return STATUS_CONST(SUCCESS);
}
GUI_NUMBER_PROTOTYPE {
  GUI_TYPE(TNumber) w = {0};
  OPSTATUS s;
  if (!value)
    return STATUS_CONST(INVALID_ARGUMENT);
  w.region = region;
  w.minimum = minimum;
  w.maximum = maximum;
  w.step = step;
  w.value = *value;
  s = GUI_FUNC(NumberEx)(context, &w);
  if (s == STATUS_CONST(SUCCESS))
    *value = w.value;
  return s;
}
GUI_SCROLLBAR_EX_PROTOTYPE {
  GUI_TYPE(TSlider) slider = {0};
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  uint64_t id;
  float value, range, thumb;
  if (!scrollbar || !GUI_INS(Ready)(context) ||
      !(scrollbar->maximum > 0 && scrollbar->maximum <= FLT_MAX) ||
      !(scrollbar->visible >= 0 && scrollbar->visible <= scrollbar->maximum) ||
      !GUI_INS(Finite)(scrollbar->offset))
    return STATUS_CONST(INVALID_ARGUMENT);
  range = scrollbar->maximum - scrollbar->visible;
  if (!scrollbar->vertical && range > 0) {
    slider.id =
        scrollbar->id ? scrollbar->id : GUI_INS(RegionId)(context, "scrollbar", scrollbar->region);
    slider.region = scrollbar->region;
    slider.minimum = 0;
    slider.maximum = range;
    slider.value = scrollbar->offset;
    slider.disabled = scrollbar->disabled;
    slider.visual = scrollbar->visual;
    s = GUI_FUNC(SliderEx)(context, &slider);
    if (s == STATUS_CONST(SUCCESS))
      scrollbar->offset = slider.value;
    return s;
  }
  v = GUI_INS(Visual)(context, scrollbar->visual, &context->theme.slider);
  s = GUI_INS(Check)(context, scrollbar->region, v, NULL, 1);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  id = scrollbar->id ? scrollbar->id : GUI_INS(RegionId)(context, "scrollbar", scrollbar->region);
  s = GUI_INS(Register)(context, id, scrollbar->disabled);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  (void)GUI_INS(Activate)(context, id, scrollbar->region, scrollbar->disabled);
  value = scrollbar->offset;
  if (!scrollbar->disabled && (!context->previousModal || context->modal)) {
    if (context->held == id && (context->down || context->released)) {
      float y = context->released ? context->releaseAt.y : context->pointer.y;
      value = (y - scrollbar->region.value2) / scrollbar->region.value4 * range;
    }
    if (context->focus == id) {
      if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_UP)))
        value -= 1;
      if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_DOWN)))
        value += 1;
    }
  }
  if (value < 0)
    value = 0;
  if (value > range)
    value = range;
  GUI_INS(Paint)(context, scrollbar->region, v, NULL, id, scrollbar->disabled);
  thumb = scrollbar->region.value4 * (scrollbar->visible / scrollbar->maximum);
  if (thumb < 2)
    thumb = 2;
  if (thumb > scrollbar->region.value4)
    thumb = scrollbar->region.value4;
  GUI_INS(Rect)
  (context,
   GUI_INS(Region)(scrollbar->region.value1,
                   scrollbar->region.value2 +
                       (range > 0 ? value / range * (scrollbar->region.value4 - thumb) : 0),
                   scrollbar->region.value3, thumb),
   v->accent, v->radius);
  scrollbar->offset = value;
  return STATUS_CONST(SUCCESS);
}
GUI_SCROLLBAR_PROTOTYPE {
  GUI_TYPE(TScrollbar) w = {0};
  OPSTATUS s;
  if (!offset)
    return STATUS_CONST(INVALID_ARGUMENT);
  w.region = region;
  w.maximum = maximum;
  w.visible = visible;
  w.offset = *offset;
  w.vertical = vertical;
  s = GUI_FUNC(ScrollbarEx)(context, &w);
  if (s == STATUS_CONST(SUCCESS))
    *offset = w.offset;
  return s;
}
