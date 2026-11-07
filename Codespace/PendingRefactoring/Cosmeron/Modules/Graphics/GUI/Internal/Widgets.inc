#pragma once
static inline uint64_t GUI_INS(RegionId)(GUI_TYPE(TContext) * c, const char *kind,
                                         TQUAD_TYPE(float) r) {
  uint64_t id = GUI_FUNC(GetId)(c, kind);
  size_t i;
  const unsigned char *p = (const unsigned char *)r.values;
  if (c->idScope)
    return id;
  for (i = 0; i < sizeof(r.values); ++i) {
    id ^= p[i];
    id *= UINT64_C(1099511628211);
  }
  return id ? id : 1;
}
GUI_LABEL_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  if (!label || !GUI_INS(Ready)(context) || !label->text)
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, label->visual, &context->theme.text);
  s = GUI_INS(Check)(context, label->region, v, label->text, 0);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  GUI_INS(Paint)(context, label->region, v, label->text, 0, false);
  return STATUS_CONST(SUCCESS);
}
GUI_LABEL_PROTOTYPE {
  GUI_TYPE(TLabel) w = {0};
  w.region = region;
  w.text = text;
  return GUI_FUNC(LabelEx)(context, &w);
}
GUI_SEPARATOR_PROTOTYPE {
  if (!GUI_INS(Ready)(context))
    return STATUS_CONST(INVALID_ARGUMENT);
  return GUI_FUNC(DrawRectangle)(context, region, context->theme.panel.border, 0);
}
GUI_BUTTON_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  uint64_t id;
  bool pressed;
  if (!button || !GUI_INS(Ready)(context) || !button->label)
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, button->visual, &context->theme.button);
  s = GUI_INS(Check)(context, button->region, v, button->label, 0);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  id = button->id ? button->id : GUI_FUNC(GetId)(context, button->label);
  s = GUI_INS(Register)(context, id, button->disabled);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  pressed = GUI_INS(Activate)(context, id, button->region, button->disabled);
  GUI_INS(Paint)(context, button->region, v, button->label, id, button->disabled);
  button->pressed = pressed;
  return STATUS_CONST(SUCCESS);
}
GUI_BUTTON_PROTOTYPE {
  GUI_TYPE(TButton) w = {0};
  OPSTATUS s;
  if (!outPressed)
    return STATUS_CONST(INVALID_ARGUMENT);
  w.region = region;
  w.label = label;
  s = GUI_FUNC(ButtonEx)(context, &w);
  if (s == STATUS_CONST(SUCCESS))
    *outPressed = w.pressed;
  return s;
}
GUI_CHECKBOX_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  uint64_t id;
  bool checked;
  TQUAD_TYPE(float) mark;
  if (!checkbox || !GUI_INS(Ready)(context) || !checkbox->label)
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, checkbox->visual, &context->theme.checkbox);
  s = GUI_INS(Check)(context, checkbox->region, v, checkbox->label, 2);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  id = checkbox->id ? checkbox->id : GUI_FUNC(GetId)(context, checkbox->label);
  s = GUI_INS(Register)(context, id, checkbox->disabled);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  checked = checkbox->checked;
  if (GUI_INS(Activate)(context, id, checkbox->region, checkbox->disabled))
    checked = !checked;
  GUI_INS(Paint)(context, checkbox->region, v, checkbox->label, id, checkbox->disabled);
  mark = GUI_INS(Region)(checkbox->region.value1 + 3, checkbox->region.value2 + 3, 10, 10);
  GUI_INS(Rect)(context, mark, v->border, 2);
  if (checked)
    GUI_INS(Rect)(context, GUI_INS(Region)(mark.value1 + 2, mark.value2 + 2, 6, 6), v->accent, 1);
  checkbox->checked = checked;
  return STATUS_CONST(SUCCESS);
}
GUI_CHECKBOX_PROTOTYPE {
  GUI_TYPE(TCheckbox) w = {0};
  OPSTATUS s;
  if (!checked)
    return STATUS_CONST(INVALID_ARGUMENT);
  w.region = region;
  w.label = label;
  w.checked = *checked;
  s = GUI_FUNC(CheckboxEx)(context, &w);
  if (s == STATUS_CONST(SUCCESS))
    *checked = w.checked;
  return s;
}
GUI_RADIO_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  uint64_t id;
  uint32_t selected;
  TQUAD_TYPE(float) mark;
  if (!radio || !GUI_INS(Ready)(context) || !radio->label || !radio->selected)
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, radio->visual, &context->theme.radio);
  s = GUI_INS(Check)(context, radio->region, v, radio->label, 2);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  id = radio->id ? radio->id : GUI_FUNC(GetId)(context, radio->label);
  s = GUI_INS(Register)(context, id, radio->disabled);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  selected = *radio->selected;
  if (GUI_INS(Activate)(context, id, radio->region, radio->disabled))
    selected = radio->option;
  GUI_INS(Paint)(context, radio->region, v, radio->label, id, radio->disabled);
  mark = GUI_INS(Region)(radio->region.value1 + 3, radio->region.value2 + 3, 10, 10);
  GUI_INS(Rect)(context, mark, v->border, 5);
  if (selected == radio->option)
    GUI_INS(Rect)(context, GUI_INS(Region)(mark.value1 + 2, mark.value2 + 2, 6, 6), v->accent, 3);
  *radio->selected = selected;
  return STATUS_CONST(SUCCESS);
}
GUI_RADIO_PROTOTYPE {
  GUI_TYPE(TRadio) w = {0};
  w.region = region;
  w.label = label;
  w.option = option;
  w.selected = selected;
  return GUI_FUNC(RadioEx)(context, &w);
}
GUI_SLIDER_EX_PROTOTYPE {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  uint64_t id;
  float value, fraction, step;
  if (!slider || !GUI_INS(Ready)(context) || !GUI_INS(Finite)(slider->minimum) ||
      !GUI_INS(Finite)(slider->maximum) || !GUI_INS(Finite)(slider->value) ||
      !(slider->maximum > slider->minimum) || !GUI_INS(Finite)(slider->maximum - slider->minimum))
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(context, slider->visual, &context->theme.slider);
  s = GUI_INS(Check)(context, slider->region, v, NULL, 2);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  id = slider->id ? slider->id : GUI_INS(RegionId)(context, "slider", slider->region);
  s = GUI_INS(Register)(context, id, slider->disabled);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  (void)GUI_INS(Activate)(context, id, slider->region, slider->disabled);
  value = slider->value;
  step = (slider->maximum - slider->minimum) / 100;
  if (!slider->disabled && (!context->previousModal || context->modal)) {
    if (context->held == id && (context->down || context->released)) {
      float x = context->released ? context->releaseAt.x : context->pointer.x;
      fraction = (x - slider->region.value1) / slider->region.value3;
      if (fraction < 0)
        fraction = 0;
      if (fraction > 1)
        fraction = 1;
      value = slider->minimum + fraction * (slider->maximum - slider->minimum);
    }
    if (context->focus == id) {
      if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_LEFT)))
        value -= step;
      if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_RIGHT)))
        value += step;
      if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_HOME)))
        value = slider->minimum;
      if (GUI_INS(KeyTake)(context, LIB_PREFIX_CONST(INPUT_KEY_END)))
        value = slider->maximum;
    }
  }
  if (value < slider->minimum)
    value = slider->minimum;
  if (value > slider->maximum)
    value = slider->maximum;
  fraction = (value - slider->minimum) / (slider->maximum - slider->minimum);
  GUI_INS(Paint)(context, slider->region, v, NULL, id, slider->disabled);
  if (fraction > 0)
    GUI_INS(Rect)
    (context,
     GUI_INS(Region)(slider->region.value1, slider->region.value2, slider->region.value3 * fraction,
                     slider->region.value4),
     v->accent, v->radius);
  GUI_INS(Rect)
  (context,
   GUI_INS(Region)(slider->region.value1 + (slider->region.value3 - 6) * fraction,
                   slider->region.value2, 6, slider->region.value4),
   v->foreground, 3);
  slider->value = value;
  return STATUS_CONST(SUCCESS);
}
GUI_SLIDER_PROTOTYPE {
  GUI_TYPE(TSlider) w = {0};
  OPSTATUS s;
  if (!value)
    return STATUS_CONST(INVALID_ARGUMENT);
  w.region = region;
  w.minimum = minimum;
  w.maximum = maximum;
  w.value = *value;
  s = GUI_FUNC(SliderEx)(context, &w);
  if (s == STATUS_CONST(SUCCESS))
    *value = w.value;
  return s;
}
static inline OPSTATUS GUI_INS(Progress)(GUI_TYPE(TContext) * c, TQUAD_TYPE(float) r,
                                         float fraction, const GUI_TYPE(TVisual) * specific) {
  const GUI_TYPE(TVisual) * v;
  OPSTATUS s;
  if (!GUI_INS(Ready)(c) || !(fraction >= 0 && fraction <= 1))
    return STATUS_CONST(INVALID_ARGUMENT);
  v = GUI_INS(Visual)(c, specific, &c->theme.progress);
  s = GUI_INS(Check)(c, r, v, NULL, 1);
  if (s != STATUS_CONST(SUCCESS))
    return s;
  GUI_INS(Paint)(c, r, v, NULL, 0, false);
  if (fraction > 0)
    GUI_INS(Rect)
    (c, GUI_INS(Region)(r.value1, r.value2, r.value3 * fraction, r.value4), v->accent, v->radius);
  return STATUS_CONST(SUCCESS);
}
GUI_PROGRESS_PROTOTYPE { return GUI_INS(Progress)(context, region, fraction, NULL); }
GUI_PROGRESS_BAR_EX_PROTOTYPE {
  if (!progress || !progress->maximum || progress->value > progress->maximum)
    return STATUS_CONST(INVALID_ARGUMENT);
  return GUI_INS(Progress)(context, progress->region,
                           (float)((double)progress->value / (double)progress->maximum),
                           progress->visual);
}
GUI_PROGRESS_BAR_PROTOTYPE {
  GUI_TYPE(TProgressBar) p = {0};
  p.region = region;
  p.value = value;
  p.maximum = maximum;
  return GUI_FUNC(ProgressBarEx)(context, &p);
}
