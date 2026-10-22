#pragma once
#include <string.h>

static inline OPSTATUS UI_INS(Put)(
    UI_TYPE(TContext) *context,uint32_t x,uint32_t y,uint32_t glyph,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style) {
  TERMINAL_TYPE(TCanvas) *canvas=UI_INS(Canvas)(context);
  if(!canvas || x>=canvas->size.col || y>=canvas->size.row)
    return STATUS_CONST(OUT_OF_RANGE);
  return TERMINAL_FUNC(Canvas_Set)(canvas,
      (TDUAL_TYPE(uint16)){.x=(uint16_t)x,.y=(uint16_t)y},glyph,style);
}
static inline OPSTATUS UI_INS(Text)(
    UI_TYPE(TContext) *context,uint32_t x,uint32_t y,uint32_t right,
    const char *text,TEXT_GRID_ATTRIBUTE_TYPE(Cell) style,
    uint32_t *outEnd) {
  size_t offset=0, length;
  if(!text) return STATUS_CONST(INVALID_ARGUMENT);
  length=strlen(text);
  while(offset<length && x<=right) {
    TEXT_TYPE(TChar32) glyph;
    size_t consumed=0;
    int width=0;
    OPSTATUS status=TEXT_UTF8_FUNC(Decode)(
       (const unsigned char *)text+offset,length-offset,&glyph,&consumed);
    if(status!=STATUS_CONST(SUCCESS)) return status;
    status=TEXT_WIDTH_FUNC(Codepoint)(glyph,&width);
    if(status!=STATUS_CONST(SUCCESS)) return status;
    if(width>0) {
      if(x+(uint32_t)width-1U>right) break;
      status=UI_INS(Put)(context,x,y,glyph,style);
      if(status!=STATUS_CONST(SUCCESS)) return status;
      x+=(uint32_t)width;
    } else return STATUS_CONST(NOT_SUPPORTED);
    offset+=consumed;
  }
  if(outEnd) *outEnd=x;
  return STATUS_CONST(SUCCESS);
}
static inline void UI_INS(BorderGlyphs)(
    UI_TYPE(Border) border,uint32_t *horizontal,uint32_t *vertical,
    uint32_t *tl,uint32_t *tr,uint32_t *bl,uint32_t *br) {
  *horizontal='-';*vertical='|';
  *tl='+';*tr='+';*bl='+';*br='+';
  switch(border) {
  case UI_CONST(BORDER_SINGLE):
    *horizontal=0x2500;*vertical=0x2502;
    *tl=0x250C;*tr=0x2510;*bl=0x2514;*br=0x2518;break;
  case UI_CONST(BORDER_DOUBLE):
    *horizontal=0x2550;*vertical=0x2551;
    *tl=0x2554;*tr=0x2557;*bl=0x255A;*br=0x255D;break;
  case UI_CONST(BORDER_ROUNDED):
    *horizontal=0x2500;*vertical=0x2502;
    *tl=0x256D;*tr=0x256E;*bl=0x2570;*br=0x256F;break;
  case UI_CONST(BORDER_HEAVY):
    *horizontal=0x2501;*vertical=0x2503;
    *tl=0x250F;*tr=0x2513;*bl=0x2517;*br=0x251B;break;
  default:break;
  }
}
static inline OPSTATUS UI_INS(Frame)(
    UI_TYPE(TContext) *context,TQUAD_TYPE(uint16) region,
    UI_TYPE(Border) border,TEXT_GRID_ATTRIBUTE_TYPE(Cell) style) {
  uint32_t x,y,h,v,tl,tr,bl,br;
  OPSTATUS status=UI_INS(Check)(context,region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  status=TERMINAL_FUNC(Canvas_FillRegion)(
      UI_INS(Canvas)(context),region,' ',style);
  if(status!=STATUS_CONST(SUCCESS) || border==UI_CONST(BORDER_NONE))return status;
  UI_INS(BorderGlyphs)(border,&h,&v,&tl,&tr,&bl,&br);
  for(x=region.left;x<=region.right;x++) {
    status=UI_INS(Put)(context,x,region.top,h,style);
    if(status!=STATUS_CONST(SUCCESS))return status;
    if(region.bottom>region.top) {
      status=UI_INS(Put)(context,x,region.bottom,h,style);
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
  }
  for(y=region.top;y<=region.bottom;y++) {
    status=UI_INS(Put)(context,region.left,y,v,style);
    if(status!=STATUS_CONST(SUCCESS))return status;
    if(region.right>region.left) {
      status=UI_INS(Put)(context,region.right,y,v,style);
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
  }
  status=UI_INS(Put)(context,region.left,region.top,tl,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  status=UI_INS(Put)(context,region.right,region.top,tr,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  status=UI_INS(Put)(context,region.left,region.bottom,bl,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  return UI_INS(Put)(context,region.right,region.bottom,br,style);
}
static inline TEXT_GRID_ATTRIBUTE_TYPE(Cell) UI_INS(ResolveStyle)(
    UI_TYPE(TContext) *context,const UI_TYPE(TVisual) *visual,
    UI_TYPE(WidgetId) id,TQUAD_TYPE(uint16) region,bool disabled) {
  if(disabled)return visual->disabled;
  if(id && context->held==id && context->pointerDown) return visual->active;
  if(id && context->focus==id)return visual->focused;
  if(UI_INS(Hovered)(context,region))return visual->hovered;
  return visual->normal;
}
static inline OPSTATUS UI_INS(Paint)(
    UI_TYPE(TContext) *context,TQUAD_TYPE(uint16) region,
    const UI_TYPE(TVisual) *visual,const char *label,
    UI_TYPE(WidgetId) id,bool disabled) {
  OPSTATUS status;
  uint32_t start,end,available,y;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  size_t units=0,bytes=0;
  if(!visual)return STATUS_CONST(INVALID_ARGUMENT);
  style=UI_INS(ResolveStyle)(context,visual,id,region,disabled);
  status=UI_INS(Frame)(context,region,visual->border,style);
  if(status!=STATUS_CONST(SUCCESS) || !label || !*label)return status;
  if(visual->border!=UI_CONST(BORDER_NONE) &&
     ((uint32_t)region.right-region.left<2U ||
      (uint32_t)region.bottom-region.top<2U))return STATUS_CONST(SUCCESS);
  start=(uint32_t)region.left+(visual->border!=UI_CONST(BORDER_NONE))+
        visual->padding.x;
  end=(uint32_t)region.right-(visual->border!=UI_CONST(BORDER_NONE));
  if(end<visual->padding.x || end-visual->padding.x<start)
    return STATUS_CONST(SUCCESS);
  end-=visual->padding.x;
  y=((uint32_t)region.top+region.bottom)/2U;
  if(TEXT_WIDTH_FUNC(UTF8)((const unsigned char *)label,strlen(label),
       &units,&bytes)!=STATUS_CONST(SUCCESS)) {
    return STATUS_CONST(INVALID_SEQUENCE);
  }
  available=end-start+1U;
  if(visual->align==UI_CONST(ALIGN_CENTER) && units<available)
    start+=(uint32_t)(available-units)/2U;
  else if(visual->align==UI_CONST(ALIGN_RIGHT) && units<available)
    start+=(uint32_t)(available-units);
  return UI_INS(Text)(context,start,y,end,label,style,NULL);
}
UI_BEGIN_PROTOTYPE {
  size_t i,write=0;
  if(!context || context->frame || !canvas ||
     !TEXT_GRID_FUNC(char32, IsValid)(canvas->backing))
    return STATUS_CONST(INVALID_ARGUMENT);
#ifndef PACKAGE_DISABLE_FUNCTION_TABLE
  if(!context->api) {
    context->api = UI_GetFunctionTable();
  }
#endif
  if(!context->themeSet) {
    TERMINAL_TYPE(TCapabilities) caps={0};
    context->theme=UI_FUNC(Theme_Default)();
    if(TERMINAL_FUNC(Capabilities_Get)(&caps)==STATUS_CONST(SUCCESS)){
      UI_TYPE(TVisual) *visuals[]={
        &context->theme.text,&context->theme.button,
        &context->theme.checkbox,&context->theme.radio,
        &context->theme.slider,&context->theme.input,
        &context->theme.panel,&context->theme.progress,
        &context->theme.selection,&context->theme.scrollbar
      };
      size_t visualIndex;
      if(!caps.unicode){
        context->theme.glyphs=UI_FUNC(Glyphs_ASCII)();
        for(visualIndex=0;visualIndex<sizeof(visuals)/sizeof(visuals[0]);
            ++visualIndex) {
          if(visuals[visualIndex]->border!=UI_CONST(BORDER_NONE))
            visuals[visualIndex]->border=UI_CONST(BORDER_ASCII);
        }
      }
      if(!caps.ansi) {
        TEXT_GRID_ATTRIBUTE_TYPE(Cell) fallback=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
        for(visualIndex=0;visualIndex<sizeof(visuals)/sizeof(visuals[0]);
            ++visualIndex) {
          visuals[visualIndex]->normal=fallback;
          visuals[visualIndex]->hovered=fallback;
          visuals[visualIndex]->focused=fallback;
          visuals[visualIndex]->active=fallback;
          visuals[visualIndex]->disabled=fallback;
        }
      }
    }
    context->themeSet=true;
  }
  context->canvas=canvas;context->lastCanvas=canvas;context->frame=true;
  context->panelDepth=0;context->layoutDepth=0;context->visualDepth=0;
  context->idDepth=0;context->idScope=0;
  /* Tab moves using the focusable elements registered in the previous frame. */
  for(i=0;i<context->keyCount;i++){
    if(context->keys[i]=='\t') (void)UI_FUNC(Focus_Next)(context);
    else context->keys[write++]=context->keys[i];
  }
  context->keyCount=write;
  context->focusCount=0;
  return STATUS_CONST(SUCCESS);
}
UI_END_PROTOTYPE {
  OPSTATUS status;
  if(!UI_INS(Ready)(context))return STATUS_CONST(INVALID_ARGUMENT);
  status=context->panelDepth || context->layoutDepth ||
         context->visualDepth || context->idDepth ?
         STATUS_CONST(INVALID_ARGUMENT):STATUS_CONST(SUCCESS);
  while(context->panelDepth)
    TERMINAL_FUNC(Canvas_Free)(&context->views[--context->panelDepth]);
  context->keyCount=0;context->scroll=0;
  context->pointerPressed=false;context->pointerReleased=false;
  if(!context->pointerDown)context->held=0;
  context->canvas=NULL;context->frame=false;
  context->visualDepth=0;context->layoutDepth=0;context->idDepth=0;
  return status;
}
UI_GET_SIZE_PROTOTYPE {
  if(!UI_INS(Ready)(context) || !outSize)return STATUS_CONST(INVALID_ARGUMENT);
  *outSize=context->canvas->size;return STATUS_CONST(SUCCESS);
}
UI_KEY_PROTOTYPE {
  if(!context || !key)return STATUS_CONST(INVALID_ARGUMENT);
  if(context->keyCount>=UI_MAX_KEYS)return STATUS_CONST(INSUFFICIENT_SPACE);
  context->keys[context->keyCount++]=key;
  return STATUS_CONST(SUCCESS);
}
UI_POINTER_MOVE_PROTOTYPE {
  if(!context)return STATUS_CONST(INVALID_ARGUMENT);
  context->pointer=position;context->pointerKnown=true;
  return STATUS_CONST(SUCCESS);
}
UI_POINTER_BUTTON_PROTOTYPE {
  if(!context)return STATUS_CONST(INVALID_ARGUMENT);
  if(pressed && !context->pointerDown)context->pointerPressed=true;
  if(!pressed && context->pointerDown)context->pointerReleased=true;
  context->pointerDown=pressed;
  return STATUS_CONST(SUCCESS);
}
UI_SCROLL_PROTOTYPE {
  if(!context)return STATUS_CONST(INVALID_ARGUMENT);
  if((delta>0 && context->scroll>INT32_MAX-delta) ||
     (delta<0 && context->scroll<INT32_MIN-delta))
    return STATUS_CONST(ARITHMETIC_OVERFLOW);
  context->scroll+=delta;
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS UI_INS(LayoutBegin)(
    UI_TYPE(TContext) *context,TQUAD_TYPE(uint16) region,
    uint16_t gap,bool row) {
  size_t index=context?context->layoutDepth:0;
  OPSTATUS status=UI_INS(Check)(context,region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(index>=UI_MAX_SCOPES)return STATUS_CONST(INSUFFICIENT_SPACE);
  context->layouts[index].region=region;
  context->layouts[index].gap=gap;
  context->layouts[index].row=row;
  context->layouts[index].cursor=row?region.left:region.top;
  ++context->layoutDepth;
  return STATUS_CONST(SUCCESS);
}
UI_ROW_BEGIN_PROTOTYPE {return UI_INS(LayoutBegin)(context,region,gap,true);}
UI_COLUMN_BEGIN_PROTOTYPE {return UI_INS(LayoutBegin)(context,region,gap,false);}
UI_LAYOUT_NEXT_PROTOTYPE {
  uint32_t from,to,max,other;
  if(!UI_INS(Ready)(context)||!context->layoutDepth || !outRegion || !extent)
    return STATUS_CONST(INVALID_ARGUMENT);
  {
    size_t i=context->layoutDepth-1;
    from=context->layouts[i].cursor;
    max=context->layouts[i].row?context->layouts[i].region.right:
           context->layouts[i].region.bottom;
    if(from>max || (uint32_t)extent>max-from+1U)
      return STATUS_CONST(OUT_OF_RANGE);
    to=from+extent-1U;
    other=context->layouts[i].gap;
    *outRegion=context->layouts[i].region;
    if(context->layouts[i].row){
      outRegion->left=(uint16_t)from;outRegion->right=(uint16_t)to;
    }else{
      outRegion->top=(uint16_t)from;outRegion->bottom=(uint16_t)to;
    }
    context->layouts[i].cursor=to+1U+other;
  }
  return STATUS_CONST(SUCCESS);
}
UI_LAYOUT_REMAINING_PROTOTYPE {
  uint32_t from,max;
  size_t i;
  if(!UI_INS(Ready)(context)||!context->layoutDepth||!outRegion)
    return STATUS_CONST(INVALID_ARGUMENT);
  i=context->layoutDepth-1;
  from=context->layouts[i].cursor;
  max=context->layouts[i].row?context->layouts[i].region.right:
      context->layouts[i].region.bottom;
  if(from>max)return STATUS_CONST(OUT_OF_RANGE);
  *outRegion=context->layouts[i].region;
  if(context->layouts[i].row)outRegion->left=(uint16_t)from;
  else outRegion->top=(uint16_t)from;
  context->layouts[i].cursor=max+1U;
  return STATUS_CONST(SUCCESS);
}
UI_LAYOUT_END_PROTOTYPE {
  if(!UI_INS(Ready)(context)||!context->layoutDepth)
    return STATUS_CONST(INVALID_ARGUMENT);
  --context->layoutDepth;return STATUS_CONST(SUCCESS);
}
UI_PANEL_BEGIN_EX_PROTOTYPE {
  const UI_TYPE(TVisual) *visual;
  TERMINAL_TYPE(TCanvas) *source;
  TQUAD_TYPE(uint16) inner;
  OPSTATUS status;
  size_t depth;
  if(!panel)return STATUS_CONST(INVALID_ARGUMENT);
  status=UI_INS(Check)(context,panel->region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  depth=context->panelDepth;
  if(depth>=UI_MAX_SCOPES)return STATUS_CONST(INSUFFICIENT_SPACE);
  if((uint32_t)panel->region.right-panel->region.left<2U ||
     (uint32_t)panel->region.bottom-panel->region.top<2U)
    return STATUS_CONST(OUT_OF_RANGE);
  visual=UI_INS(ChooseVisual)(context,panel->visual,&context->theme.panel);
  status=UI_INS(Paint)(context,panel->region,visual,panel->title,0,false);
  if(status!=STATUS_CONST(SUCCESS))return status;
  source=UI_INS(Canvas)(context);
  inner=panel->region;
  inner.left++;inner.right--;inner.top++;inner.bottom--;
  /* Panel views are borrowed and must be popped before root resize/free. */
  context->views[depth]=(TERMINAL_TYPE(TCanvas)){0};
  status=TERMINAL_FUNC(Canvas_View)(source,inner,&context->views[depth]);
  if(status==STATUS_CONST(SUCCESS))++context->panelDepth;
  return status;
}
UI_PANEL_BEGIN_PROTOTYPE {
  UI_TYPE(TPanel) panel={0};
  panel.region=region;panel.title=title;
  return UI_FUNC(Panel_BeginEx)(context,&panel);
}
UI_PANEL_END_PROTOTYPE {
  if(!UI_INS(Ready)(context)||!context->panelDepth)
    return STATUS_CONST(INVALID_ARGUMENT);
  TERMINAL_FUNC(Canvas_Free)(&context->views[--context->panelDepth]);
  return STATUS_CONST(SUCCESS);
}

UI_DISPATCH_PROTOTYPE {
  if(!context || !event || context->frame)
    return STATUS_CONST(INVALID_ARGUMENT);
  switch(event->kind){
    case TERMINAL_CONST(EVENT_KEY):
      if(event->key == '\t')
        return UI_FUNC(Focus_Next)(context);
      return UI_FUNC(Key)(context, event->key);
    case TERMINAL_CONST(EVENT_POINTER_MOVE):
      return UI_FUNC(PointerMove)(context, event->position);
    case TERMINAL_CONST(EVENT_POINTER_BUTTON): {
      OPSTATUS st = UI_FUNC(PointerMove)(context, event->position);
      if(st != STATUS_CONST(SUCCESS)) return st;
      return UI_FUNC(PointerButton)(context, event->pressed);
    }
    case TERMINAL_CONST(EVENT_SCROLL): {
      OPSTATUS st = UI_FUNC(PointerMove)(context, event->position);
      if(st != STATUS_CONST(SUCCESS)) return st;
      return UI_FUNC(Scroll)(context, event->scroll);
    }
    case TERMINAL_CONST(EVENT_RESIZE):
      context->newSize = event->size;
      context->resized = true;
      return STATUS_CONST(SUCCESS);
    default:
      return STATUS_CONST(NOT_SUPPORTED);
  }
}

UI_DISPATCH_INPUT_PROTOTYPE {
  if(!context || !event || context->frame)
    return STATUS_CONST(INVALID_ARGUMENT);
  switch(event->type){
    case INPUT_CONST(EVENT_KEY_PRESS):
    case INPUT_CONST(EVENT_KEY_REPEAT): {
      if(event->data.key == INPUT_CONST(KEY_TAB)) {
        if(event->modifiers & INPUT_CONST(MODIFIER_SHIFT))
          return UI_FUNC(Focus_Previous)(context);
        return UI_FUNC(Focus_Next)(context);
      }
      switch(event->data.key){
        case INPUT_CONST(KEY_UP): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_UP));
        case INPUT_CONST(KEY_DOWN): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_DOWN));
        case INPUT_CONST(KEY_LEFT): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_LEFT));
        case INPUT_CONST(KEY_RIGHT): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_RIGHT));
        case INPUT_CONST(KEY_ENTER): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_ENTER));
        case INPUT_CONST(KEY_ESCAPE): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_ESCAPE));
        case INPUT_CONST(KEY_BACKSPACE): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_BACKSPACE));
        case INPUT_CONST(KEY_DELETE): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_DELETE));
        case INPUT_CONST(KEY_HOME): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_HOME));
        case INPUT_CONST(KEY_END): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_END));
        case INPUT_CONST(KEY_PAGE_UP): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_PAGE_UP));
        case INPUT_CONST(KEY_PAGE_DOWN): return UI_FUNC(Key)(context, TERMINAL_CONST(KEY_PAGE_DOWN));
        case INPUT_CONST(KEY_SPACE): return UI_FUNC(Key)(context, ' ');
        default: break;
      }
      return STATUS_CONST(SUCCESS);
    }
    case INPUT_CONST(EVENT_TEXT_INPUT): {
      if(event->data.text.length > 0) {
        return UI_FUNC(Key)(context, (TERMINAL_TYPE(TKey))(unsigned char)event->data.text.utf8[0]);
      }
      return STATUS_CONST(SUCCESS);
    }
    case INPUT_CONST(EVENT_MOUSE_POSITION): {
      TDUAL_TYPE(uint16) pos;
      pos.x = (uint16_t)(event->data.motion.x >= 0 ? event->data.motion.x : 0);
      pos.y = (uint16_t)(event->data.motion.y >= 0 ? event->data.motion.y : 0);
      return UI_FUNC(PointerMove)(context, pos);
    }
    case INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS):
    case INPUT_CONST(EVENT_MOUSE_BUTTON_RELEASE): {
      bool pressed = (event->type == INPUT_CONST(EVENT_MOUSE_BUTTON_PRESS));
      if(event->data.mouseButton == INPUT_CONST(MOUSE_BUTTON_LEFT)) {
        return UI_FUNC(PointerButton)(context, pressed);
      }
      return STATUS_CONST(SUCCESS);
    }
    case INPUT_CONST(EVENT_MOUSE_SCROLL): {
      int32_t delta = (int32_t)event->data.motion.y;
      if(delta != 0) {
        return UI_FUNC(Scroll)(context, delta);
      }
      return STATUS_CONST(SUCCESS);
    }
    default:
      return STATUS_CONST(NOT_SUPPORTED);
  }
}

UI_POLL_EVENTS_PROTOTYPE {
  size_t handled=0;
  if(!context || !outCount || context->frame)
    return STATUS_CONST(INVALID_ARGUMENT);
  while(handled<UI_MAX_KEYS) {
    TERMINAL_TYPE(TEvent) event={0};
    bool available=false;
    OPSTATUS status=TERMINAL_FUNC(Event_Poll)(&event,&available);
    if(status!=STATUS_CONST(SUCCESS))return status;
    if(!available)break;
    ++handled;
    status=UI_FUNC(Dispatch)(context,&event);
    if(status!=STATUS_CONST(SUCCESS))return status;
  }
  *outCount=handled;
  return STATUS_CONST(SUCCESS);
}
UI_RESIZE_TAKE_PROTOTYPE {
  if(!context || !outSize || !context->resized)return false;
  *outSize=context->newSize;context->resized=false;return true;
}
UI_LAYOUT_SHARE_PROTOTYPE {
  size_t index;
  uint32_t remaining,maximum,share;
  if(!UI_INS(Ready)(context)||!outRegion||!numerator||!denominator||
     numerator>denominator||!context->layoutDepth)
    return STATUS_CONST(INVALID_ARGUMENT);
  index=context->layoutDepth-1;
  maximum=context->layouts[index].row?context->layouts[index].region.right:
          context->layouts[index].region.bottom;
  if(context->layouts[index].cursor>maximum)return STATUS_CONST(OUT_OF_RANGE);
  remaining=maximum-context->layouts[index].cursor+1U;
  share=(remaining*numerator)/denominator;
  if(!share)share=1;
  return UI_FUNC(Layout_Next)(context,(uint16_t)share,outRegion);
}
UI_LAYOUT_MEASURE_TEXT_PROTOTYPE {
  size_t columns=0,codepoints=0;
  OPSTATUS status;
  if(!text||!outSize)return STATUS_CONST(INVALID_ARGUMENT);
  status=TEXT_WIDTH_FUNC(UTF8)((const unsigned char *)text,
         strlen(text),&columns,&codepoints);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(columns+2U*padding.x>UINT16_MAX||1U+2U*padding.y>UINT16_MAX)
    return STATUS_CONST(OUT_OF_RANGE);
  *outSize=(TDUAL_TYPE(uint16)){
    .col=(uint16_t)(columns+2U*padding.x),
    .row=(uint16_t)(1U+2U*padding.y)};
  return STATUS_CONST(SUCCESS);
}

UI_RENDER_PROTOTYPE {
  TERMINAL_TYPE(TCanvas) *sourceCanvas = context ? (context->canvas ? context->canvas : context->lastCanvas) : NULL;
  if(!context || !canvas || !sourceCanvas)
    return STATUS_CONST(INVALID_ARGUMENT);
  TDUAL_TYPE(uint16) destPos = {.col = 0U, .row = 0U};
  return TERMINAL_FUNC(Canvas_Blit)(canvas, destPos, sourceCanvas);
}

UI_PRESENT_PROTOTYPE {
  TERMINAL_TYPE(TCanvas) *sourceCanvas = context ? (context->canvas ? context->canvas : context->lastCanvas) : NULL;
  if(!context || !screen || !sourceCanvas)
    return STATUS_CONST(INVALID_ARGUMENT);
  TDUAL_TYPE(uint16) size = screen->api ? screen->api->Size(screen) : screen->size;
  uint16_t maxCol = sourceCanvas->size.col < size.col ? sourceCanvas->size.col : size.col;
  uint16_t maxRow = sourceCanvas->size.row < size.row ? sourceCanvas->size.row : size.row;
  for(uint16_t r = 0U; r < maxRow; ++r) {
    for(uint16_t c = 0U; c < maxCol; ++c) {
      TDUAL_TYPE(uint16) pos;
      pos.col = c;
      pos.row = r;
      uint32_t cp = 0;
      TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr;
      if(TERMINAL_FUNC(Canvas_Get)(sourceCanvas, pos, &cp, &attr) == STATUS_CONST(SUCCESS)) {
        if(screen->api && screen->api->SetCell) {
          screen->api->SetCell(screen, c, r, cp, attr);
        } else {
          TERMINAL_SCREEN_FUNC(SetCell)(screen, c, r, cp, attr);
        }
      }
    }
  }
  return screen->api && screen->api->Present ? screen->api->Present(screen) : TERMINAL_SCREEN_FUNC(Present)(screen);
}

UI_LAYOUT_PROTOTYPE {
  if(!context || context->frame)
    return STATUS_CONST(INVALID_ARGUMENT);
  if(viewportSize.col == 0U || viewportSize.row == 0U)
    return STATUS_CONST(INVALID_ARGUMENT);
  context->layoutDepth = 0U;
  context->layouts[0].region.left = 0U;
  context->layouts[0].region.top = 0U;
  context->layouts[0].region.right = (uint16_t)(viewportSize.col - 1U);
  context->layouts[0].region.bottom = (uint16_t)(viewportSize.row - 1U);
  context->layouts[0].cursor = 0U;
  context->layouts[0].gap = 0U;
  context->layouts[0].row = false;
  return STATUS_CONST(SUCCESS);
}

UI_LAYOUT_SPLIT_HORIZONTAL_PROTOTYPE {
  if(!outTop || !outBottom || topRatio + bottomRatio == 0U)
    return STATUS_CONST(INVALID_ARGUMENT);
  if(parent.left > parent.right || parent.top > parent.bottom)
    return STATUS_CONST(INVALID_ARGUMENT);
  uint16_t height = (uint16_t)(parent.bottom - parent.top + 1U);
  if(height <= gap) return STATUS_CONST(OUT_OF_RANGE);
  uint16_t available = (uint16_t)(height - gap);
  uint16_t topH = (uint16_t)((uint32_t)available * topRatio / (topRatio + bottomRatio));
  outTop->left = parent.left;
  outTop->right = parent.right;
  outTop->top = parent.top;
  outTop->bottom = (uint16_t)(parent.top + (topH > 0U ? topH - 1U : 0U));
  outBottom->left = parent.left;
  outBottom->right = parent.right;
  outBottom->top = (uint16_t)(outTop->bottom + 1U + gap);
  outBottom->bottom = parent.bottom;
  return STATUS_CONST(SUCCESS);
}

UI_LAYOUT_SPLIT_VERTICAL_PROTOTYPE {
  if(!outLeft || !outRight || leftRatio + rightRatio == 0U)
    return STATUS_CONST(INVALID_ARGUMENT);
  if(parent.left > parent.right || parent.top > parent.bottom)
    return STATUS_CONST(INVALID_ARGUMENT);
  uint16_t width = (uint16_t)(parent.right - parent.left + 1U);
  if(width <= gap) return STATUS_CONST(OUT_OF_RANGE);
  uint16_t available = (uint16_t)(width - gap);
  uint16_t leftW = (uint16_t)((uint32_t)available * leftRatio / (leftRatio + rightRatio));
  outLeft->top = parent.top;
  outLeft->bottom = parent.bottom;
  outLeft->left = parent.left;
  outLeft->right = (uint16_t)(parent.left + (leftW > 0U ? leftW - 1U : 0U));
  outRight->top = parent.top;
  outRight->bottom = parent.bottom;
  outRight->left = (uint16_t)(outLeft->right + 1U + gap);
  outRight->right = parent.right;
  return STATUS_CONST(SUCCESS);
}

UI_LAYOUT_GRID_PROTOTYPE {
  if(!outRegion || cols == 0U || rows == 0U || col >= cols || row >= rows)
    return STATUS_CONST(INVALID_ARGUMENT);
  if(parent.left > parent.right || parent.top > parent.bottom)
    return STATUS_CONST(INVALID_ARGUMENT);
  uint16_t totalW = (uint16_t)(parent.right - parent.left + 1U);
  uint16_t totalH = (uint16_t)(parent.bottom - parent.top + 1U);
  uint16_t totalGapX = (uint16_t)((cols - 1U) * gapX);
  uint16_t totalGapY = (uint16_t)((rows - 1U) * gapY);
  if(totalW <= totalGapX || totalH <= totalGapY)
    return STATUS_CONST(OUT_OF_RANGE);
  uint16_t cellW = (uint16_t)((totalW - totalGapX) / cols);
  uint16_t cellH = (uint16_t)((totalH - totalGapY) / rows);
  outRegion->left = (uint16_t)(parent.left + col * (cellW + gapX));
  outRegion->right = (uint16_t)(col == cols - 1U ? parent.right : outRegion->left + cellW - 1U);
  outRegion->top = (uint16_t)(parent.top + row * (cellH + gapY));
  outRegion->bottom = (uint16_t)(row == rows - 1U ? parent.bottom : outRegion->top + cellH - 1U);
  return STATUS_CONST(SUCCESS);
}

UI_LAYOUT_CENTER_PROTOTYPE {
  if(!outRegion || size.col == 0U || size.row == 0U)
    return STATUS_CONST(INVALID_ARGUMENT);
  if(parent.left > parent.right || parent.top > parent.bottom)
    return STATUS_CONST(INVALID_ARGUMENT);
  uint16_t parentW = (uint16_t)(parent.right - parent.left + 1U);
  uint16_t parentH = (uint16_t)(parent.bottom - parent.top + 1U);
  if(size.col > parentW || size.row > parentH)
    return STATUS_CONST(OUT_OF_RANGE);
  outRegion->left = (uint16_t)(parent.left + (parentW - size.col) / 2U);
  outRegion->right = (uint16_t)(outRegion->left + size.col - 1U);
  outRegion->top = (uint16_t)(parent.top + (parentH - size.row) / 2U);
  outRegion->bottom = (uint16_t)(outRegion->top + size.row - 1U);
  return STATUS_CONST(SUCCESS);
}
