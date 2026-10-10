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
  if(!context->themeSet) {
    context->theme=UI_FUNC(Theme_Default)();
    context->themeSet=true;
  }
  context->canvas=canvas;context->frame=true;
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
  context->pointer=position;return STATUS_CONST(SUCCESS);
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
