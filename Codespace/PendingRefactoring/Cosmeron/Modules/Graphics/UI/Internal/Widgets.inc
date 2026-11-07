#pragma once

static inline UI_TYPE(WidgetId) UI_INS(WidgetIdFor)(
    UI_TYPE(TContext) *context, UI_TYPE(WidgetId) explicitId, const char *label) {
  return explicitId?explicitId:UI_FUNC(GetId)(context,label);
}
static inline OPSTATUS UI_INS(PreWidget)(
    UI_TYPE(TContext) *context,TQUAD_TYPE(uint16) region,
    UI_TYPE(WidgetId) id,bool enabled) {
  OPSTATUS status=UI_INS(Check)(context,region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  return UI_INS(Register)(context,id,enabled);
}
static inline OPSTATUS UI_INS(LabeledGlyph)(
    UI_TYPE(TContext) *context,TQUAD_TYPE(uint16) region,
    const UI_TYPE(TVisual) *visual,UI_TYPE(WidgetId) id,bool disabled,
    const char *glyph,const char *label) {
  uint32_t start,after=0,right;
  OPSTATUS status=UI_INS(Paint)(context,region,visual,NULL,id,disabled);
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=
      UI_INS(ResolveStyle)(context,visual,id,region,disabled);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(visual->border!=UI_CONST(BORDER_NONE) &&
     (uint32_t)region.right-region.left<2U)return STATUS_CONST(SUCCESS);
  start=(uint32_t)region.left+(visual->border!=UI_CONST(BORDER_NONE))+
        visual->padding.x;
  right=(uint32_t)region.right-(visual->border!=UI_CONST(BORDER_NONE));
  if(right<visual->padding.x || start>right-visual->padding.x)
    return STATUS_CONST(SUCCESS);
  right-=visual->padding.x;
  if(glyph) {
    status=UI_INS(Text)(context,start,((uint32_t)region.top+region.bottom)/2,
        right,glyph,style,&after);
    if(status!=STATUS_CONST(SUCCESS))return status;
    start=after+1U;
  }
  if(label && start<=right)
    return UI_INS(Text)(context,start,((uint32_t)region.top+region.bottom)/2,
        right,label,style,NULL);
  return STATUS_CONST(SUCCESS);
}
UI_LABEL_EX_PROTOTYPE {
  const UI_TYPE(TVisual) *visual;
  if(!label || !label->text)return STATUS_CONST(INVALID_ARGUMENT);
  if(UI_INS(Check)(context,label->region)!=STATUS_CONST(SUCCESS))
    return UI_INS(Check)(context,label->region);
  visual=UI_INS(ChooseVisual)(context,label->visual,&context->theme.text);
  return UI_INS(Paint)(context,label->region,visual,label->text,0,false);
}
UI_LABEL_PROTOTYPE {
  UI_TYPE(TLabel) widget={0};
  widget.region=region;widget.text=text;
  return UI_FUNC(LabelEx)(context,&widget);
}
UI_SEPARATOR_EX_PROTOTYPE {
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  uint32_t x,y;
  OPSTATUS status;
  if(!separator)return STATUS_CONST(INVALID_ARGUMENT);
  status=UI_INS(Check)(context,separator->region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,separator->visual,&context->theme.text);
  style=visual->normal;
  y=((uint32_t)separator->region.top+separator->region.bottom)/2U;
  for(x=separator->region.left;x<=separator->region.right;x++){
    status=UI_INS(Put)(context,x,y,0x2500,style);
    if(status!=STATUS_CONST(SUCCESS))return status;
  }
  return STATUS_CONST(SUCCESS);
}
UI_SEPARATOR_PROTOTYPE {
  UI_TYPE(TSeparator) separator={0};separator.region=region;
  return UI_FUNC(SeparatorEx)(context,&separator);
}
UI_BUTTON_EX_PROTOTYPE {
  UI_TYPE(WidgetId) id;
  const UI_TYPE(TVisual) *visual;
  OPSTATUS status;
  if(!button || !button->label)return STATUS_CONST(INVALID_ARGUMENT);
  button->pressed=false;
  id=UI_INS(WidgetIdFor)(context,button->id,button->label);
  status=UI_INS(PreWidget)(context,button->region,id,!button->disabled);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,button->visual,&context->theme.button);
  button->pressed=UI_INS(Activate)(context,id,button->region,button->disabled);
  return UI_INS(Paint)(context,button->region,visual,button->label,id,button->disabled);
}
UI_BUTTON_PROTOTYPE {
  UI_TYPE(TButton) button={0};
  OPSTATUS status;
  if(!outPressed)return STATUS_CONST(INVALID_ARGUMENT);
  button.region=region;button.label=label;
  status=UI_FUNC(ButtonEx)(context,&button);
  if(status==STATUS_CONST(SUCCESS))*outPressed=button.pressed;
  return status;
}
UI_CHECKBOX_EX_PROTOTYPE {
  UI_TYPE(WidgetId) id;
  const UI_TYPE(TVisual) *visual;
  OPSTATUS status;
  if(!checkbox || !checkbox->label)return STATUS_CONST(INVALID_ARGUMENT);
  id=UI_INS(WidgetIdFor)(context,checkbox->id,checkbox->label);
  status=UI_INS(PreWidget)(context,checkbox->region,id,!checkbox->disabled);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(UI_INS(Activate)(context,id,checkbox->region,checkbox->disabled))
    checkbox->checked=!checkbox->checked;
  visual=UI_INS(ChooseVisual)(context,checkbox->visual,&context->theme.checkbox);
  return UI_INS(LabeledGlyph)(context,checkbox->region,visual,id,checkbox->disabled,
      checkbox->checked?context->theme.glyphs.checkboxOn:
                        context->theme.glyphs.checkboxOff,checkbox->label);
}
UI_CHECKBOX_PROTOTYPE {
  UI_TYPE(TCheckbox) checkbox={0};OPSTATUS status;
  if(!checked)return STATUS_CONST(INVALID_ARGUMENT);
  checkbox.region=region;checkbox.label=label;checkbox.checked=*checked;
  status=UI_FUNC(CheckboxEx)(context,&checkbox);
  if(status==STATUS_CONST(SUCCESS))*checked=checkbox.checked;
  return status;
}
UI_RADIO_EX_PROTOTYPE {
  UI_TYPE(WidgetId) id;
  const UI_TYPE(TVisual) *visual;
  OPSTATUS status;
  if(!radio || !radio->label || !radio->selected)
    return STATUS_CONST(INVALID_ARGUMENT);
  id=UI_INS(WidgetIdFor)(context,radio->id,radio->label);
  status=UI_INS(PreWidget)(context,radio->region,id,!radio->disabled);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(UI_INS(Activate)(context,id,radio->region,radio->disabled))
    *radio->selected=radio->option;
  visual=UI_INS(ChooseVisual)(context,radio->visual,&context->theme.radio);
  return UI_INS(LabeledGlyph)(context,radio->region,visual,id,radio->disabled,
      *radio->selected==radio->option?context->theme.glyphs.radioOn:
                                     context->theme.glyphs.radioOff,radio->label);
}
UI_RADIO_PROTOTYPE {
  UI_TYPE(TRadio) radio={0};
  radio.region=region;radio.label=label;radio.option=option;radio.selected=selected;
  return UI_FUNC(RadioEx)(context,&radio);
}
static inline uint32_t UI_INS(FirstGlyph)(const char *text,uint32_t fallback){
  TEXT_TYPE(TChar32) point=0;
  size_t consumed=0;
  if(!text || !*text)return fallback;
  if(TEXT_UTF8_FUNC(Decode)((const unsigned char *)text,strlen(text),
      &point,&consumed)!=STATUS_CONST(SUCCESS))return fallback;
  return (uint32_t)point;
}
UI_SLIDER_EX_PROTOTYPE {
  UI_TYPE(WidgetId) id;
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  OPSTATUS status;
  uint32_t left,right,x,thumb,track,highlight;
  int64_t range,value;
  size_t i;
  if(!slider || slider->minimum>=slider->maximum ||
     slider->value<slider->minimum || slider->value>slider->maximum)
    return STATUS_CONST(INVALID_ARGUMENT);
  id=slider->id?slider->id:UI_FUNC(GetId)(context,"##slider");
  status=UI_INS(PreWidget)(context,slider->region,id,!slider->disabled);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,slider->visual,&context->theme.slider);
  left=(uint32_t)slider->region.left+(visual->border!=UI_CONST(BORDER_NONE));
  right=(uint32_t)slider->region.right-(visual->border!=UI_CONST(BORDER_NONE));
  if(visual->border!=UI_CONST(BORDER_NONE) &&
     (uint32_t)slider->region.right-slider->region.left<2U)
    return STATUS_CONST(OUT_OF_RANGE);
  if(right<left)return STATUS_CONST(OUT_OF_RANGE);
  if(!slider->disabled) {
    if(context->pointerPressed && UI_INS(Hovered)(context,slider->region)) {
      context->focus=id;context->held=id;
    }
    if(context->held==id && context->pointerDown) {
      TERMINAL_TYPE(TCanvas) *c=UI_INS(Canvas)(context);
      uint32_t pointerX=context->pointer.x>=c->origin.x?
          context->pointer.x-c->origin.x:0;
      if(pointerX<left)pointerX=left;
      if(pointerX>right)pointerX=right;
      range=(int64_t)slider->maximum-slider->minimum;
      slider->value=(int32_t)((int64_t)slider->minimum+
             (int64_t)(pointerX-left)*range/(right-left?right-left:1U));
    }
    if(context->focus==id)for(i=0;i<context->keyCount;i++){
      if(context->keys[i]==TERMINAL_CONST(KEY_LEFT) && slider->value>slider->minimum){
        slider->value--;context->keys[i]=0;
      }else if(context->keys[i]==TERMINAL_CONST(KEY_RIGHT) &&
               slider->value<slider->maximum){
        slider->value++;context->keys[i]=0;
      }
    }
  }
  style=UI_INS(ResolveStyle)(context,visual,id,slider->region,slider->disabled);
  status=UI_INS(Frame)(context,slider->region,visual->border,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  range=(int64_t)slider->maximum-slider->minimum;
  value=(int64_t)slider->value-slider->minimum;
  highlight=(uint32_t)((uint64_t)value*(right-left)/(uint64_t)range)+left;
  track=UI_INS(FirstGlyph)(context->theme.glyphs.sliderTrack,'-');
  thumb=UI_INS(FirstGlyph)(context->theme.glyphs.sliderThumb,'#');
  for(x=left;x<=right;x++){
    status=UI_INS(Put)(context,x,
        ((uint32_t)slider->region.top+slider->region.bottom)/2U,
        x==highlight?thumb:track,style);
    if(status!=STATUS_CONST(SUCCESS))return status;
  }
  return STATUS_CONST(SUCCESS);
}
UI_SLIDER_PROTOTYPE {
  UI_TYPE(TSlider) slider={0};OPSTATUS status;
  if(!value)return STATUS_CONST(INVALID_ARGUMENT);
  slider.region=region;slider.minimum=minimum;slider.maximum=maximum;
  slider.value=*value;
  status=UI_FUNC(SliderEx)(context,&slider);
  if(status==STATUS_CONST(SUCCESS))*value=slider.value;
  return status;
}
UI_PROGRESS_EX_PROTOTYPE {
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  OPSTATUS status;
  uint32_t x,left,right,filled,full,empty;
  if(!progress || !(progress->fraction>=0.0f && progress->fraction<=1.0f))
    return STATUS_CONST(INVALID_ARGUMENT);
  status=UI_INS(Check)(context,progress->region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,progress->visual,&context->theme.progress);
  style=visual->normal;
  status=UI_INS(Frame)(context,progress->region,visual->border,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  left=(uint32_t)progress->region.left+(visual->border!=UI_CONST(BORDER_NONE));
  right=(uint32_t)progress->region.right-(visual->border!=UI_CONST(BORDER_NONE));
  if(visual->border!=UI_CONST(BORDER_NONE) &&
     (uint32_t)progress->region.right-progress->region.left<2U)
    return STATUS_CONST(OUT_OF_RANGE);
  if(right<left)return STATUS_CONST(OUT_OF_RANGE);
  filled=(uint32_t)((float)(right-left+1U)*progress->fraction);
  full=UI_INS(FirstGlyph)(context->theme.glyphs.progressFull,'#');
  empty=UI_INS(FirstGlyph)(context->theme.glyphs.progressEmpty,'-');
  for(x=left;x<=right;x++){
    status=UI_INS(Put)(context,x,
        ((uint32_t)progress->region.top+progress->region.bottom)/2U,
        x-left<filled?full:empty,style);
    if(status!=STATUS_CONST(SUCCESS))return status;
  }
  return STATUS_CONST(SUCCESS);
}
UI_PROGRESS_PROTOTYPE {
  UI_TYPE(TProgress) progress={0};
  progress.region=region;progress.fraction=fraction;
  return UI_FUNC(ProgressEx)(context,&progress);
}

UI_SCROLLBAR_EX_PROTOTYPE {
  UI_TYPE(WidgetId) id;
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  OPSTATUS status;
  uint32_t left, right, top, bottom;
  int64_t totalRange, valRange;
  size_t i;

  if (!scrollbar || scrollbar->minimum >= scrollbar->maximum ||
      scrollbar->value < scrollbar->minimum || scrollbar->value > scrollbar->maximum ||
      scrollbar->pageSize <= 0)
    return STATUS_CONST(INVALID_ARGUMENT);

  if (scrollbar->id) {
    id = scrollbar->id;
  } else {
    id = UI_FUNC(GetId)(context, "##scrollbar") ^
         (((uint64_t)scrollbar->region.left << 16) | (uint64_t)scrollbar->region.top);
    if (!id) id = 1;
  }
  status = UI_INS(PreWidget)(context, scrollbar->region, id, !scrollbar->disabled);
  if (status != STATUS_CONST(SUCCESS))
    return status;

  visual = UI_INS(ChooseVisual)(context, scrollbar->visual, &context->theme.scrollbar);
  bool hasBorder = (visual->border != UI_CONST(BORDER_NONE)) &&
                   ((uint32_t)scrollbar->region.right - scrollbar->region.left >= 2U) &&
                   ((uint32_t)scrollbar->region.bottom - scrollbar->region.top >= 2U);
  left = (uint32_t)scrollbar->region.left + hasBorder;
  right = (uint32_t)scrollbar->region.right - hasBorder;
  top = (uint32_t)scrollbar->region.top + hasBorder;
  bottom = (uint32_t)scrollbar->region.bottom - hasBorder;

  if (right < left || bottom < top)
    return STATUS_CONST(OUT_OF_RANGE);

  totalRange = scrollbar->maximum - scrollbar->minimum + scrollbar->pageSize;
  valRange = scrollbar->maximum - scrollbar->minimum;

  if (!scrollbar->disabled) {
    if (context->pointerPressed && UI_INS(Hovered)(context, scrollbar->region)) {
      context->focus = id;
      context->held = id;
    }

    if (scrollbar->vertical) {
      uint32_t h = bottom - top + 1U;
      uint32_t thumbSize = (uint32_t)((int64_t)h * scrollbar->pageSize / (totalRange ? totalRange : 1));
      if (thumbSize < 1U) thumbSize = 1U;
      if (thumbSize > h) thumbSize = h;
      uint32_t travel = h - thumbSize;

      if (context->held == id && context->pointerDown) {
        TERMINAL_TYPE(TCanvas) *c = UI_INS(Canvas)(context);
        uint32_t pointerY = context->pointer.y >= c->origin.y ? context->pointer.y - c->origin.y : 0;
        if (pointerY <= top) {
          scrollbar->value = scrollbar->minimum;
        } else if (pointerY >= bottom) {
          scrollbar->value = scrollbar->maximum;
        } else if (travel > 0U) {
          uint32_t relY = pointerY > top ? pointerY - top : 0;
          if (relY > travel) relY = travel;
          scrollbar->value = scrollbar->minimum + (int64_t)relY * valRange / travel;
        }
      }

      if (UI_INS(Hovered)(context, scrollbar->region) && context->scroll != 0) {
        scrollbar->value += context->scroll;
        if (scrollbar->value < scrollbar->minimum) scrollbar->value = scrollbar->minimum;
        if (scrollbar->value > scrollbar->maximum) scrollbar->value = scrollbar->maximum;
      }

      if (context->focus == id) {
        for (i = 0; i < context->keyCount; i++) {
          if (context->keys[i] == TERMINAL_CONST(KEY_UP) && scrollbar->value > scrollbar->minimum) {
            scrollbar->value--;
            context->keys[i] = 0;
          } else if (context->keys[i] == TERMINAL_CONST(KEY_DOWN) && scrollbar->value < scrollbar->maximum) {
            scrollbar->value++;
            context->keys[i] = 0;
          } else if (context->keys[i] == TERMINAL_CONST(KEY_PAGE_UP)) {
            scrollbar->value -= scrollbar->pageSize;
            if (scrollbar->value < scrollbar->minimum) scrollbar->value = scrollbar->minimum;
            context->keys[i] = 0;
          } else if (context->keys[i] == TERMINAL_CONST(KEY_PAGE_DOWN)) {
            scrollbar->value += scrollbar->pageSize;
            if (scrollbar->value > scrollbar->maximum) scrollbar->value = scrollbar->maximum;
            context->keys[i] = 0;
          }
        }
      }
    } else {
      /* Horizontal */
      uint32_t w = right - left + 1U;
      uint32_t thumbSize = (uint32_t)((int64_t)w * scrollbar->pageSize / (totalRange ? totalRange : 1));
      if (thumbSize < 1U) thumbSize = 1U;
      if (thumbSize > w) thumbSize = w;
      uint32_t travel = w - thumbSize;

      if (context->held == id && context->pointerDown) {
        TERMINAL_TYPE(TCanvas) *c = UI_INS(Canvas)(context);
        uint32_t pointerX = context->pointer.x >= c->origin.x ? context->pointer.x - c->origin.x : 0;
        if (pointerX <= left) {
          scrollbar->value = scrollbar->minimum;
        } else if (pointerX >= right) {
          scrollbar->value = scrollbar->maximum;
        } else if (travel > 0U) {
          uint32_t relX = pointerX > left ? pointerX - left : 0;
          if (relX > travel) relX = travel;
          scrollbar->value = scrollbar->minimum + (int64_t)relX * valRange / travel;
        }
      }

      if (UI_INS(Hovered)(context, scrollbar->region) && context->scroll != 0) {
        scrollbar->value += context->scroll;
        if (scrollbar->value < scrollbar->minimum) scrollbar->value = scrollbar->minimum;
        if (scrollbar->value > scrollbar->maximum) scrollbar->value = scrollbar->maximum;
      }

      if (context->focus == id) {
        for (i = 0; i < context->keyCount; i++) {
          if (context->keys[i] == TERMINAL_CONST(KEY_LEFT) && scrollbar->value > scrollbar->minimum) {
            scrollbar->value--;
            context->keys[i] = 0;
          } else if (context->keys[i] == TERMINAL_CONST(KEY_RIGHT) && scrollbar->value < scrollbar->maximum) {
            scrollbar->value++;
            context->keys[i] = 0;
          } else if (context->keys[i] == TERMINAL_CONST(KEY_PAGE_UP)) {
            scrollbar->value -= scrollbar->pageSize;
            if (scrollbar->value < scrollbar->minimum) scrollbar->value = scrollbar->minimum;
            context->keys[i] = 0;
          } else if (context->keys[i] == TERMINAL_CONST(KEY_PAGE_DOWN)) {
            scrollbar->value += scrollbar->pageSize;
            if (scrollbar->value > scrollbar->maximum) scrollbar->value = scrollbar->maximum;
            context->keys[i] = 0;
          }
        }
      }
    }
  }

  style = UI_INS(ResolveStyle)(context, visual, id, scrollbar->region, scrollbar->disabled);
  status = UI_INS(Frame)(context, scrollbar->region, hasBorder ? visual->border : UI_CONST(BORDER_NONE), style);
  if (status != STATUS_CONST(SUCCESS))
    return status;

  if (scrollbar->vertical) {
    uint32_t h = bottom - top + 1U;
    uint32_t thumbSize = (uint32_t)((int64_t)h * scrollbar->pageSize / (totalRange ? totalRange : 1));
    if (thumbSize < 1U) thumbSize = 1U;
    if (thumbSize > h) thumbSize = h;
    uint32_t travel = h - thumbSize;
    uint32_t thumbStart = top + (travel > 0U && valRange > 0 ? (uint32_t)((scrollbar->value - scrollbar->minimum) * travel / valRange) : 0U);
    uint32_t midX = (scrollbar->region.left + scrollbar->region.right) / 2U;
    uint32_t trackGlyph = UI_INS(FirstGlyph)(context->theme.glyphs.sliderTrack, '|');
    uint32_t thumbGlyph = UI_INS(FirstGlyph)(context->theme.glyphs.sliderThumb, '#');

    for (uint32_t y = top; y <= bottom; ++y) {
      bool isThumb = (y >= thumbStart && y < thumbStart + thumbSize);
      status = UI_INS(Put)(context, midX, y, isThumb ? thumbGlyph : trackGlyph, style);
      if (status != STATUS_CONST(SUCCESS))
        return status;
    }
  } else {
    uint32_t w = right - left + 1U;
    uint32_t thumbSize = (uint32_t)((int64_t)w * scrollbar->pageSize / (totalRange ? totalRange : 1));
    if (thumbSize < 1U) thumbSize = 1U;
    if (thumbSize > w) thumbSize = w;
    uint32_t travel = w - thumbSize;
    uint32_t thumbStart = left + (travel > 0U && valRange > 0 ? (uint32_t)((scrollbar->value - scrollbar->minimum) * travel / valRange) : 0U);
    uint32_t midY = (scrollbar->region.top + scrollbar->region.bottom) / 2U;
    uint32_t trackGlyph = UI_INS(FirstGlyph)(context->theme.glyphs.sliderTrack, '-');
    uint32_t thumbGlyph = UI_INS(FirstGlyph)(context->theme.glyphs.sliderThumb, '#');

    for (uint32_t x = left; x <= right; ++x) {
      bool isThumb = (x >= thumbStart && x < thumbStart + thumbSize);
      status = UI_INS(Put)(context, x, midY, isThumb ? thumbGlyph : trackGlyph, style);
      if (status != STATUS_CONST(SUCCESS))
        return status;
    }
  }

  return STATUS_CONST(SUCCESS);
}

UI_SCROLLBAR_PROTOTYPE {
  UI_TYPE(TScrollbar) bar = {0};
  OPSTATUS status;
  if (!value)
    return STATUS_CONST(INVALID_ARGUMENT);
  bar.region = region;
  bar.minimum = minimum;
  bar.maximum = maximum;
  bar.pageSize = pageSize;
  bar.vertical = vertical;
  bar.value = *value;
  status = UI_FUNC(ScrollbarEx)(context, &bar);
  if (status == STATUS_CONST(SUCCESS))
    *value = bar.value;
  return status;
}
