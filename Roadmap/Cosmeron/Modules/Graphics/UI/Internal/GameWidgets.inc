#pragma once
#include <stdio.h>

/* A menu is one focusable widget, not a tree of Button allocations. */
static inline bool UI_INS(MenuDisabled)(const UI_TYPE(TMenu) *menu,size_t at) {
  return menu->disabled || (menu->itemDisabled && menu->itemDisabled[at]);
}
static inline bool UI_INS(MenuStep)(UI_TYPE(TMenu) *menu,int direction) {
  size_t cursor,tries;
  if(!menu->count)return false;
  cursor=*menu->selected;
  for(tries=0;tries<menu->count;tries++){
    if(direction>0){
      if(cursor==menu->count-1U){
        if(!menu->wrap)return false;
        cursor=0;
      }else ++cursor;
    }else{
      if(!cursor){
        if(!menu->wrap)return false;
        cursor=menu->count-1U;
      }else --cursor;
    }
    if(!UI_INS(MenuDisabled)(menu,cursor)){
      *menu->selected=cursor;
      return true;
    }
  }
  return false;
}
static inline OPSTATUS UI_INS(MenuGeometry)(
    const UI_TYPE(TMenu) *menu,const UI_TYPE(TVisual) *visual,
    uint32_t *outLeft,uint32_t *outRight,
    uint32_t *outTop,uint32_t *outBottom,uint32_t *outTitle) {
  uint32_t border=visual->border!=UI_CONST(BORDER_NONE)?1U:0U;
  uint32_t left=menu->region.left+border+visual->padding.x;
  uint32_t top=menu->region.top+border+visual->padding.y;
  uint32_t right=menu->region.right, bottom=menu->region.bottom;
  if(right<border+visual->padding.x ||
     bottom<border+visual->padding.y)
    return STATUS_CONST(OUT_OF_RANGE);
  right-=border+visual->padding.x;
  bottom-=border+visual->padding.y;
  if(left>right || top>bottom)return STATUS_CONST(OUT_OF_RANGE);
  *outTitle=top;
  if(menu->title && *menu->title) ++top;
  *outLeft=left;*outRight=right;*outTop=top;*outBottom=bottom;
  if(top>bottom && menu->count)return STATUS_CONST(OUT_OF_RANGE);
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS UI_INS(MenuAlignedX)(
    const UI_TYPE(TVisual) *visual,const char *label,
    uint32_t left,uint32_t right,uint32_t *outX) {
  size_t columns=0,units=0;
  uint32_t width;
  OPSTATUS status;
  if(!visual || !label || !outX || right<left)
    return STATUS_CONST(INVALID_ARGUMENT);
  status=TEXT_WIDTH_FUNC(UTF8)((const unsigned char *)label,
      strlen(label),&columns,&units);
  if(status!=STATUS_CONST(SUCCESS))return status;
  width=right-left+1U;
  *outX=left;
  if(columns<width){
    uint32_t available=(uint32_t)(width-columns);
    if(visual->align==UI_CONST(ALIGN_CENTER))*outX+=available/2U;
    else if(visual->align==UI_CONST(ALIGN_RIGHT))*outX+=available;
  }
  return STATUS_CONST(SUCCESS);
}
UI_MENU_EX_PROTOTYPE {
  UI_TYPE(WidgetId) id;
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  uint32_t left,right,top,bottom,titleRow,y;
  size_t rows,i;
  OPSTATUS status;
  if(!menu || !menu->selected || (menu->count && !menu->items))
    return STATUS_CONST(INVALID_ARGUMENT);
  menu->activated=false;menu->cancelled=false;
  if(menu->count && *menu->selected>=menu->count)
    return STATUS_CONST(OUT_OF_RANGE);
  status=UI_INS(Check)(context,menu->region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  id=menu->id?menu->id:UI_FUNC(GetId)(context,"##menu");
  status=UI_INS(Register)(context,id,!menu->disabled && menu->count!=0);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,menu->visual,&context->theme.selection);
  status=UI_INS(MenuGeometry)(menu,visual,&left,&right,&top,&bottom,&titleRow);
  if(status!=STATUS_CONST(SUCCESS))return status;
  rows=top<=bottom?(size_t)(bottom-top)+1U:0U;
  /* Mouse coordinates are terminal-cell absolute, panel coordinates local. */
  if(!menu->disabled && menu->count && rows &&
     UI_INS(Hovered)(context,menu->region)) {
    TERMINAL_TYPE(TCanvas) *canvas=UI_INS(Canvas)(context);
    if(context->pointer.y>=canvas->origin.y){
      uint32_t row=(uint32_t)context->pointer.y-canvas->origin.y;
      if(row>=top && row<=bottom && (context->pointerPressed ||
                                       context->pointerReleased)) {
        size_t at=menu->firstVisible+(size_t)(row-top);
        if(at<menu->count && !UI_INS(MenuDisabled)(menu,at)){
          *menu->selected=at;
          if(context->pointerPressed){
            context->focus=id;
            context->held=id;
          }else if(context->held==id)menu->activated=true;
        }
      }
    }
    if(context->scroll){
      int dir=context->scroll<0?1:-1;
      unsigned int steps=context->scroll<0?
          (unsigned int)(-(int64_t)context->scroll):
          (unsigned int)context->scroll;
      if(steps>menu->count)steps=(unsigned int)menu->count;
      while(steps-- && UI_INS(MenuStep)(menu,dir)){}
      context->scroll=0;
      context->focus=id;
    }
  }
  if(context->focus==id && !menu->disabled && menu->count){
    for(i=0;i<context->keyCount;i++){
      TERMINAL_TYPE(TKey) key=context->keys[i];
      bool handled=true;
      if(key==TERMINAL_CONST(KEY_UP))
        (void)UI_INS(MenuStep)(menu,-1);
      else if(key==TERMINAL_CONST(KEY_DOWN))
        (void)UI_INS(MenuStep)(menu,1);
      else if(key==TERMINAL_CONST(KEY_HOME)){
        size_t candidate;
        for(candidate=0;candidate<menu->count;candidate++)
          if(!UI_INS(MenuDisabled)(menu,candidate)){
            *menu->selected=candidate;break;
          }
      }else if(key==TERMINAL_CONST(KEY_END)){
        size_t candidate=menu->count;
        while(candidate){
          --candidate;
          if(!UI_INS(MenuDisabled)(menu,candidate)){
            *menu->selected=candidate;break;
          }
        }
      }else if(key==TERMINAL_CONST(KEY_ENTER) || key==' '){
        if(!UI_INS(MenuDisabled)(menu,*menu->selected))
          menu->activated=true;
      }else if(key==TERMINAL_CONST(KEY_ESCAPE))
        menu->cancelled=true;
      else handled=false;
      if(handled)context->keys[i]=0;
    }
  }
  if(rows && menu->count){
    if(*menu->selected<menu->firstVisible)
      menu->firstVisible=*menu->selected;
    else if(*menu->selected-menu->firstVisible>=rows)
      menu->firstVisible=*menu->selected-rows+1U;
  }
  if(menu->firstVisible>menu->count)menu->firstVisible=menu->count;
  style=visual->normal;
  status=UI_INS(Frame)(context,menu->region,visual->border,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(menu->title && *menu->title){
    uint32_t titleX=left;
    status=UI_INS(MenuAlignedX)(visual,menu->title,left,right,&titleX);
    if(status!=STATUS_CONST(SUCCESS))return status;
    status=UI_INS(Text)(context,titleX,titleRow,right,menu->title,style,NULL);
    if(status!=STATUS_CONST(SUCCESS))return status;
  }
  for(y=top;y<=bottom && rows;y++){
    size_t item=menu->firstVisible+(size_t)(y-top);
    uint32_t x;
    const char *cursorGlyph;
    if(item>=menu->count)break;
    if(!menu->items[item])return STATUS_CONST(INVALID_ARGUMENT);
    if(UI_INS(MenuDisabled)(menu,item)) style=visual->disabled;
    else if(item==*menu->selected)style=visual->focused;
    else if(context->focus==id)style=visual->normal;
    else style=visual->normal;
    for(x=left;x<=right;x++){
      status=UI_INS(Put)(context,x,y,' ',style);
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
    x=left;
    if(item==*menu->selected){
      cursorGlyph=context->theme.glyphs.selection;
      if(cursorGlyph){
        status=UI_INS(Text)(context,x,y,right,cursorGlyph,style,&x);
        if(status!=STATUS_CONST(SUCCESS))return status;
      }
    }
    if(x<right)x++;
    if(x<=right){
      uint32_t aligned=x;
      status=UI_INS(MenuAlignedX)(visual,menu->items[item],x,right,&aligned);
      if(status!=STATUS_CONST(SUCCESS))return status;
      status=UI_INS(Text)(context,aligned,y,right,menu->items[item],style,NULL);
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
  }
  return STATUS_CONST(SUCCESS);
}
UI_MENU_PROTOTYPE {
  UI_TYPE(TMenu) menu={0};
  OPSTATUS status;
  if(!outActivated)return STATUS_CONST(INVALID_ARGUMENT);
  menu.region=region;menu.items=items;menu.count=count;menu.selected=selected;
  status=UI_FUNC(MenuEx)(context,&menu);
  if(status==STATUS_CONST(SUCCESS))*outActivated=menu.activated;
  return status;
}
/* Value/range + optional percentage, label, vertical or indeterminate pulse. */
UI_PROGRESS_BAR_EX_PROTOTYPE {
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  uint32_t left,right,top,bottom,border,columns,rows,span,filled,track,solid;
  uint32_t x,y;
  OPSTATUS status;
  if(!progress || (!progress->indeterminate &&
       (!progress->maximum || progress->value>progress->maximum)))
    return STATUS_CONST(INVALID_ARGUMENT);
  status=UI_INS(Check)(context,progress->region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,progress->visual,&context->theme.progress);
  border=visual->border!=UI_CONST(BORDER_NONE)?1U:0U;
  left=progress->region.left+border+visual->padding.x;
  top=progress->region.top+border+visual->padding.y;
  if(progress->region.right<border+visual->padding.x ||
     progress->region.bottom<border+visual->padding.y)
    return STATUS_CONST(OUT_OF_RANGE);
  right=progress->region.right-border-visual->padding.x;
  bottom=progress->region.bottom-border-visual->padding.y;
  if(left>right || top>bottom)return STATUS_CONST(OUT_OF_RANGE);
  style=visual->normal;
  status=UI_INS(Frame)(context,progress->region,visual->border,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(progress->label && *progress->label && top<bottom){
    status=UI_INS(Text)(context,left,top,right,progress->label,style,NULL);
    if(status!=STATUS_CONST(SUCCESS))return status;
    ++top;
  }
  columns=right-left+1U;rows=bottom-top+1U;
  span=progress->vertical?rows:columns;
  if(progress->indeterminate){
    filled=span>=4U?span/4U:1U;
  }else{
    filled=(uint32_t)(((long double)progress->value*
                      (long double)span)/(long double)progress->maximum);
    if(progress->value==progress->maximum)filled=span;
  }
  track=UI_INS(FirstGlyph)(context->theme.glyphs.progressEmpty,'-');
  solid=UI_INS(FirstGlyph)(context->theme.glyphs.progressFull,'#');
  {
    int w=0;
    if(TEXT_WIDTH_FUNC(Codepoint)((TEXT_TYPE(TChar32))track,&w)!=
       STATUS_CONST(SUCCESS) || w!=1)track='-';
    if(TEXT_WIDTH_FUNC(Codepoint)((TEXT_TYPE(TChar32))solid,&w)!=
       STATUS_CONST(SUCCESS) || w!=1)solid='#';
  }
  for(y=top;y<=bottom;y++){
    for(x=left;x<=right;x++){
      uint32_t index=progress->vertical?bottom-y:x-left;
      bool active;
      if(progress->indeterminate){
        uint32_t start=(uint32_t)(progress->phase%(uint64_t)span);
        active=index>=start && index-start<filled;
      }else active=index<filled;
      status=UI_INS(Put)(context,x,y,active?solid:track,
          active?visual->active:style);
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
  }
  if(progress->showPercent && !progress->indeterminate){
    char percent[8];
    uint32_t value=(uint32_t)(
       ((long double)progress->value*100.0L)/
       (long double)progress->maximum);
    int len=snprintf(percent,sizeof(percent),"%u%%",(unsigned)value);
    if(len<0)return STATUS_CONST(GENERIC_ERROR);
    if((uint32_t)len<=columns){
      uint32_t start=left+(columns-(uint32_t)len)/2U;
      TEXT_GRID_ATTRIBUTE_TYPE(Cell) number=visual->focused;
      status=UI_INS(Text)(context,start,top+rows/2U,right,
                          percent,number,NULL);
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
  }
  return STATUS_CONST(SUCCESS);
}
UI_PROGRESS_BAR_PROTOTYPE {
  UI_TYPE(TProgressBar) bar={0};
  bar.region=region;bar.value=value;bar.maximum=maximum;
  bar.showPercent=true;
  return UI_FUNC(ProgressBarEx)(context,&bar);
}
