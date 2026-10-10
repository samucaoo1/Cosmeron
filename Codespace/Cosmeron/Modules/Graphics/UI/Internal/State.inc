#pragma once
#include <string.h>
#include <stdlib.h>

/* TVisual reuses Text/Grid attributes; no alternate color or cell structures. */
UI_VISUAL_DEFAULT_PROTOTYPE {
  UI_TYPE(TVisual) visual = {0};
  visual.normal = TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  visual.hovered = visual.normal;
  visual.focused = visual.normal;
  visual.active = visual.normal;
  visual.disabled = visual.normal;
  visual.align = UI_CONST(ALIGN_LEFT);
  visual.border = UI_CONST(BORDER_NONE);
  return visual;
}
UI_VISUAL_RGB_PROTOTYPE {
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  cell.foreground=foreground & UINT32_C(0xFFFFFF);
  cell.background=background & UINT32_C(0xFFFFFF);
  cell.flags &= (uint16_t)~(TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND) |
                            TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_BACKGROUND));
  return cell;
}
UI_GLYPHS_ASCII_PROTOTYPE {
  UI_TYPE(TGlyphs) glyphs={
    "[X]","[ ]","(*)","( )","-","#","=","-",
    ">"
  };
  return glyphs;
}
UI_GLYPHS_UNICODE_PROTOTYPE {
  UI_TYPE(TGlyphs) glyphs={
    "☑","☐","◉","○","─","◆","█","░","▶"
  };
  return glyphs;
}
static inline UI_TYPE(TTheme) UI_INS(ThemeBase)(
    uint32_t foreground,uint32_t background,uint32_t highlight,
    uint32_t muted, UI_TYPE(TGlyphs) glyphs) {
  UI_TYPE(TTheme) theme={0};
  UI_TYPE(TVisual) v=UI_FUNC(Visual_Default)();
  v.normal=UI_FUNC(Visual_RGB)(foreground,background);
  v.hovered=UI_FUNC(Visual_RGB)(foreground,highlight);
  v.focused=UI_FUNC(Visual_RGB)(foreground,highlight);
  v.active=UI_FUNC(Visual_RGB)(background,foreground);
  v.disabled=UI_FUNC(Visual_RGB)(muted,background);
  v.border=UI_CONST(BORDER_SINGLE);
  v.align=UI_CONST(ALIGN_CENTER);
  theme.button=v;
  theme.checkbox=v; theme.radio=v; theme.slider=v;
  theme.input=v; theme.panel=v;
  theme.selection=v; theme.progress=v;
  v.border=UI_CONST(BORDER_NONE);
  v.align=UI_CONST(ALIGN_LEFT);
  theme.text=v;
  theme.glyphs=glyphs;
  return theme;
}
UI_THEME_DARK_PROTOTYPE {
  return UI_INS(ThemeBase)(UINT32_C(0xEAF0FC),UINT32_C(0x171D29),
      UINT32_C(0x304E78),UINT32_C(0x8492A5),UI_FUNC(Glyphs_Unicode)());
}
UI_THEME_LIGHT_PROTOTYPE {
  return UI_INS(ThemeBase)(UINT32_C(0x182334),UINT32_C(0xF4F6FA),
      UINT32_C(0xBFDAFF),UINT32_C(0x8290A2),UI_FUNC(Glyphs_Unicode)());
}
UI_THEME_CLASSIC_PROTOTYPE {
  return UI_INS(ThemeBase)(UINT32_C(0xAAFFB0),UINT32_C(0x102018),
      UINT32_C(0x326044),UINT32_C(0x5D9572),UI_FUNC(Glyphs_ASCII)());
}
UI_THEME_MONOCHROME_PROTOTYPE {
  return UI_INS(ThemeBase)(UINT32_C(0xEEEEEE),UINT32_C(0x101010),
      UINT32_C(0x555555),UINT32_C(0x777777),UI_FUNC(Glyphs_ASCII)());
}
UI_THEME_DEFAULT_PROTOTYPE { return UI_FUNC(Theme_Dark)(); }
UI_THEME_SET_PROTOTYPE {
  if(!context || !theme || context->frame) return STATUS_CONST(INVALID_ARGUMENT);
  context->theme=*theme;
  context->themeSet=true;
  return STATUS_CONST(SUCCESS);
}
UI_THEME_GET_PROTOTYPE {
  if(!context || !outTheme) return STATUS_CONST(INVALID_ARGUMENT);
  *outTheme=context->themeSet?context->theme:UI_FUNC(Theme_Default)();
  return STATUS_CONST(SUCCESS);
}
UI_VISUAL_PUSH_PROTOTYPE {
  if(!context || !context->frame || !visual)
    return STATUS_CONST(INVALID_ARGUMENT);
  if(context->visualDepth>=UI_MAX_SCOPES) return STATUS_CONST(INSUFFICIENT_SPACE);
  context->visualStack[context->visualDepth++]=*visual;
  return STATUS_CONST(SUCCESS);
}
UI_VISUAL_POP_PROTOTYPE {
  if(!context || !context->frame || !context->visualDepth)
    return STATUS_CONST(INVALID_ARGUMENT);
  --context->visualDepth;
  return STATUS_CONST(SUCCESS);
}
static inline const UI_TYPE(TVisual) *UI_INS(ChooseVisual)(
    const UI_TYPE(TContext) *context,const UI_TYPE(TVisual) *specific,
    const UI_TYPE(TVisual) *base) {
  if(specific) return specific;
  if(context->visualDepth) return &context->visualStack[context->visualDepth-1];
  return base;
}
static inline TERMINAL_TYPE(TCanvas) *UI_INS(Canvas)(
    UI_TYPE(TContext) *context) {
  if(!context || !context->canvas) return NULL;
  if(context->panelDepth) return &context->views[context->panelDepth-1];
  return context->canvas;
}
static inline bool UI_INS(Ready)(const UI_TYPE(TContext) *context) {
  return context && context->frame && context->canvas!=NULL;
}
static inline bool UI_INS(RectValid)(
    const TERMINAL_TYPE(TCanvas) *canvas, TQUAD_TYPE(uint16) region) {
  return canvas && region.left<=region.right && region.top<=region.bottom &&
         region.right<canvas->size.col && region.bottom<canvas->size.row;
}
static inline OPSTATUS UI_INS(Check)(
    UI_TYPE(TContext) *context,TQUAD_TYPE(uint16) region) {
  if(!UI_INS(Ready)(context)) return STATUS_CONST(INVALID_ARGUMENT);
  return UI_INS(RectValid)(UI_INS(Canvas)(context),region)?
         STATUS_CONST(SUCCESS):STATUS_CONST(OUT_OF_RANGE);
}
static inline UI_TYPE(WidgetId) UI_INS(HashText)(
    UI_TYPE(WidgetId) hash,const char *text) {
  const unsigned char *p=(const unsigned char *)text;
  if(!text) return hash;
  while(*p) {
    hash^=(UI_TYPE(WidgetId))*p++;
    hash*=UINT64_C(1099511628211);
  }
  return hash?hash:UINT64_C(1);
}
UI_GET_ID_PROTOTYPE {
  UI_TYPE(WidgetId) hash;
  if(!context || !label) return 0;
  hash=context->idScope?context->idScope:UINT64_C(14695981039346656037);
  return UI_INS(HashText)(hash,label);
}
UI_PUSH_ID_PROTOTYPE {
  if(!UI_INS(Ready)(context) || !scope)
    return STATUS_CONST(INVALID_ARGUMENT);
  if(context->idDepth>=UI_MAX_SCOPES) return STATUS_CONST(INSUFFICIENT_SPACE);
  context->idStack[context->idDepth++]=context->idScope;
  context->idScope=UI_FUNC(GetId)(context,scope);
  return STATUS_CONST(SUCCESS);
}
UI_POP_ID_PROTOTYPE {
  if(!UI_INS(Ready)(context) || !context->idDepth)
    return STATUS_CONST(INVALID_ARGUMENT);
  context->idScope=context->idStack[--context->idDepth];
  return STATUS_CONST(SUCCESS);
}
UI_FOCUS_SET_PROTOTYPE {
  if(!context || widget==0) return STATUS_CONST(INVALID_ARGUMENT);
  context->focus=widget;return STATUS_CONST(SUCCESS);
}
UI_IS_FOCUSED_PROTOTYPE { return context && widget && context->focus==widget; }
static inline OPSTATUS UI_INS(FocusStep)(UI_TYPE(TContext) *context, bool forward) {
  size_t i, index=0;
  if(!context) return STATUS_CONST(INVALID_ARGUMENT);
  if(!context->focusCount) return STATUS_CONST(NOT_FOUND);
  for(i=0;i<context->focusCount;i++) {
    if(context->focusIds[i]==context->focus) {index=i;break;}
  }
  if(i==context->focusCount) index=forward?context->focusCount-1:0;
  index=forward?(index+1)%context->focusCount:
                (index+context->focusCount-1)%context->focusCount;
  context->focus=context->focusIds[index];
  return STATUS_CONST(SUCCESS);
}
UI_FOCUS_NEXT_PROTOTYPE {return UI_INS(FocusStep)(context,true);}
UI_FOCUS_PREVIOUS_PROTOTYPE {return UI_INS(FocusStep)(context,false);}
static inline OPSTATUS UI_INS(Register)(
    UI_TYPE(TContext) *context,UI_TYPE(WidgetId) id,bool enabled) {
  size_t i;
  if(!id || !UI_INS(Ready)(context)) return STATUS_CONST(INVALID_ARGUMENT);
  for(i=0;i<context->focusCount;i++)
    if(context->focusIds[i]==id) return STATUS_CONST(ALREADY_EXISTS);
  if(!enabled) return STATUS_CONST(SUCCESS);
  if(context->focusCount>=UI_MAX_WIDGETS) return STATUS_CONST(INSUFFICIENT_SPACE);
  context->focusIds[context->focusCount++]=id;
  if(!context->focus) context->focus=id;
  return STATUS_CONST(SUCCESS);
}
static inline bool UI_INS(Hovered)(
    UI_TYPE(TContext) *context,TQUAD_TYPE(uint16) region) {
  TERMINAL_TYPE(TCanvas) *canvas=UI_INS(Canvas)(context);
  uint32_t localX,localY;
  if(!canvas || !context->pointerKnown) return false;
  if(context->pointer.x<canvas->origin.x ||
     context->pointer.y<canvas->origin.y) return false;
  localX=(uint32_t)context->pointer.x-canvas->origin.x;
  localY=(uint32_t)context->pointer.y-canvas->origin.y;
  return localX>=region.left && localX<=region.right &&
         localY>=region.top && localY<=region.bottom;
}
static inline bool UI_INS(Activate)(
    UI_TYPE(TContext) *context,UI_TYPE(WidgetId) id,
    TQUAD_TYPE(uint16) region,bool disabled) {
  size_t i;
  bool hovered=UI_INS(Hovered)(context,region);
  bool fire=false;
  if(disabled) return false;
  if(context->pointerPressed && hovered) {
    context->held=id;
    context->focus=id;
  }
  if(context->pointerReleased && hovered && context->held==id) fire=true;
  if(context->focus==id) for(i=0;i<context->keyCount;i++) {
    TERMINAL_TYPE(TKey) key=context->keys[i];
    if(key==TERMINAL_CONST(KEY_ENTER) || key==' ') {
      fire=true;
      context->keys[i]=0;
    }
  }
  return fire;
}
