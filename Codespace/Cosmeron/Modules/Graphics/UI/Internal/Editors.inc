#pragma once

static inline OPSTATUS UI_INS(TextEdit)(
    UI_TYPE(TContext) *context,UI_TYPE(TInputText) *input,
    UI_TYPE(WidgetId) id,bool multiline) {
  size_t i;
  if(!input->buffer || !input->capacity || input->length>=input->capacity ||
     input->buffer[input->length]!=0)
    return STATUS_CONST(INVALID_ARGUMENT);
  if(!TEXT_UTF8_FUNC(Validate)((const unsigned char *)input->buffer,input->length))
    return STATUS_CONST(INVALID_SEQUENCE);
  if(input->disabled || context->focus!=id)return STATUS_CONST(SUCCESS);
  for(i=0;i<context->keyCount;i++){
    TERMINAL_TYPE(TKey) key=context->keys[i];
    if(!key)continue;
    if(key==TERMINAL_CONST(KEY_BACKSPACE) || key=='\b'){
      if(input->length){
        do { --input->length; }
        while(input->length &&
          (((unsigned char)input->buffer[input->length]&0xC0U)==0x80U));
        input->buffer[input->length]=0;
      }
      context->keys[i]=0;
    }else if(key==TERMINAL_CONST(KEY_ENTER) || key=='\n'){
      if(multiline) {
        if(input->length+2>input->capacity)return STATUS_CONST(INSUFFICIENT_SPACE);
        input->buffer[input->length++]='\n';
        input->buffer[input->length]=0;
      }
      context->keys[i]=0;
    }else if(key>=32U && key<=UINT32_C(0x10FFFF)){
      unsigned char bytes[4];size_t amount=0;
      OPSTATUS status=TEXT_UTF8_FUNC(Encode)((TEXT_TYPE(TChar32))key,
           bytes,sizeof(bytes),&amount);
      if(status!=STATUS_CONST(SUCCESS))return status;
      if(input->length+amount>=input->capacity)
        return STATUS_CONST(INSUFFICIENT_SPACE);
      memcpy(input->buffer+input->length,bytes,amount);
      input->length+=amount;
      input->buffer[input->length]=0;
      context->keys[i]=0;
    }
  }
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS UI_INS(DrawEdit)(
    UI_TYPE(TContext) *context,TQUAD_TYPE(uint16) region,
    const UI_TYPE(TVisual) *visual,UI_TYPE(WidgetId) id,
    const UI_TYPE(TInputText) *input,bool multiline) {
  uint32_t x,y,left,right,top,bottom;
  size_t i=0;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  OPSTATUS status;
  style=UI_INS(ResolveStyle)(context,visual,id,region,input->disabled);
  status=UI_INS(Frame)(context,region,visual->border,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(visual->border!=UI_CONST(BORDER_NONE) &&
     ((uint32_t)region.right-region.left<2U ||
      (uint32_t)region.bottom-region.top<2U))return STATUS_CONST(SUCCESS);
  left=(uint32_t)region.left+(visual->border!=UI_CONST(BORDER_NONE))+
       visual->padding.x;
  right=(uint32_t)region.right-(visual->border!=UI_CONST(BORDER_NONE));
  top=(uint32_t)region.top+(visual->border!=UI_CONST(BORDER_NONE))+
      visual->padding.y;
  bottom=(uint32_t)region.bottom-(visual->border!=UI_CONST(BORDER_NONE));
  if(right<visual->padding.x || bottom<visual->padding.y)
    return STATUS_CONST(SUCCESS);
  right-=visual->padding.x;bottom-=visual->padding.y;
  if(left>right || top>bottom)return STATUS_CONST(SUCCESS);
  x=left;y=top;
  while(i<input->length && y<=bottom) {
    TEXT_TYPE(TChar32) point;size_t consumed=0;int width=0;
    status=TEXT_UTF8_FUNC(Decode)((const unsigned char *)input->buffer+i,
        input->length-i,&point,&consumed);
    if(status!=STATUS_CONST(SUCCESS))return status;
    i+=consumed;
    if(point=='\n') {
      if(multiline){x=left;++y;}
      continue;
    }
    status=TEXT_WIDTH_FUNC(Codepoint)(point,&width);
    if(status!=STATUS_CONST(SUCCESS))return status;
    if(width==0)return STATUS_CONST(NOT_SUPPORTED);
    if(x+(uint32_t)width>right+1U) {
      if(!multiline)break;
      x=left;++y;
      if(y>bottom)break;
    }
    status=UI_INS(Put)(context,x,y,point,style);
    if(status!=STATUS_CONST(SUCCESS))return status;
    x+=(uint32_t)width;
  }
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS UI_INS(InputCommon)(
    UI_TYPE(TContext) *context,UI_TYPE(TInputText) *input,bool multiline) {
  const UI_TYPE(TVisual) *visual;
  UI_TYPE(WidgetId) id;
  OPSTATUS status;
  if(!input || !input->buffer)return STATUS_CONST(INVALID_ARGUMENT);
  id=input->id?input->id:UI_FUNC(GetId)(context,
      multiline?"##textarea":"##input");
  status=UI_INS(PreWidget)(context,input->region,id,!input->disabled);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(context->pointerPressed && UI_INS(Hovered)(context,input->region) &&
     !input->disabled) context->focus=id;
  status=UI_INS(TextEdit)(context,input,id,multiline);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,input->visual,&context->theme.input);
  return UI_INS(DrawEdit)(context,input->region,visual,id,input,multiline);
}
UI_INPUT_TEXT_EX_PROTOTYPE {
  return UI_INS(InputCommon)(context,input,false);
}
UI_INPUT_TEXT_PROTOTYPE {
  UI_TYPE(TInputText) input={0};OPSTATUS status;
  if(!inoutLength)return STATUS_CONST(INVALID_ARGUMENT);
  input.region=region;input.buffer=buffer;input.capacity=capacity;
  input.length=*inoutLength;
  status=UI_FUNC(InputTextEx)(context,&input);
  if(status==STATUS_CONST(SUCCESS))*inoutLength=input.length;
  return status;
}
UI_TEXT_AREA_EX_PROTOTYPE {
  return UI_INS(InputCommon)(context,input,true);
}
UI_TEXT_AREA_PROTOTYPE {
  UI_TYPE(TTextArea) input={0};OPSTATUS status;
  if(!inoutLength)return STATUS_CONST(INVALID_ARGUMENT);
  input.region=region;input.buffer=buffer;input.capacity=capacity;
  input.length=*inoutLength;
  status=UI_FUNC(TextAreaEx)(context,&input);
  if(status==STATUS_CONST(SUCCESS))*inoutLength=input.length;
  return status;
}
UI_LIST_EX_PROTOTYPE {
  UI_TYPE(WidgetId) id;
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) normal,highlight;
  uint32_t top,bottom,y;
  size_t i;
  OPSTATUS status;
  if(!list || !list->selected || (list->count && !list->items))
    return STATUS_CONST(INVALID_ARGUMENT);
  if(list->count && *list->selected>=list->count)
    return STATUS_CONST(OUT_OF_RANGE);
  id=list->id?list->id:UI_FUNC(GetId)(context,"##list");
  status=UI_INS(PreWidget)(context,list->region,id,!list->disabled);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,list->visual,&context->theme.selection);
  if(!list->disabled) {
    if(context->pointerPressed && UI_INS(Hovered)(context,list->region))
      context->focus=id;
    if(context->focus==id) {
      for(i=0;i<context->keyCount;i++) {
        if(context->keys[i]==TERMINAL_CONST(KEY_UP) && *list->selected>0){
          --*list->selected;context->keys[i]=0;
        }else if(context->keys[i]==TERMINAL_CONST(KEY_DOWN) &&
          *list->selected+1<list->count){
          ++*list->selected;context->keys[i]=0;
        }
      }
      if(context->scroll>0 && *list->selected>0)--*list->selected;
      if(context->scroll<0 && *list->selected+1<list->count)++*list->selected;
    }
    if(context->pointerReleased && context->held==id &&
       UI_INS(Hovered)(context,list->region)) {
      TERMINAL_TYPE(TCanvas) *canvas=UI_INS(Canvas)(context);
      uint32_t localY=(uint32_t)context->pointer.y-canvas->origin.y;
      uint32_t top=(uint32_t)list->region.top+
           (visual->border!=UI_CONST(BORDER_NONE));
      size_t candidate=localY>=top?(size_t)(localY-top):list->count;
      if(candidate<list->count)*list->selected=candidate;
    }
    if(context->pointerPressed && UI_INS(Hovered)(context,list->region))
      context->held=id;
  }
  normal=visual->normal;
  highlight=visual->focused;
  status=UI_INS(Frame)(context,list->region,visual->border,normal);
  if(status!=STATUS_CONST(SUCCESS))return status;
  top=(uint32_t)list->region.top+(visual->border!=UI_CONST(BORDER_NONE));
  bottom=(uint32_t)list->region.bottom-(visual->border!=UI_CONST(BORDER_NONE));
  for(y=top;y<=bottom && (size_t)(y-top)<list->count;y++) {
    size_t index=(size_t)(y-top);
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr=index==*list->selected?highlight:normal;
    uint32_t x,left=(uint32_t)list->region.left+
        (visual->border!=UI_CONST(BORDER_NONE));
    uint32_t right=(uint32_t)list->region.right-
        (visual->border!=UI_CONST(BORDER_NONE));
    for(x=left;x<=right;x++){
      status=UI_INS(Put)(context,x,y,' ',attr);
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
    if(list->items[index]) {
      status=UI_INS(Text)(context,left,y,right,list->items[index],attr,NULL);
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
  }
  return STATUS_CONST(SUCCESS);
}
UI_LIST_PROTOTYPE {
  UI_TYPE(TList) list={0};
  list.region=region;list.items=items;list.count=count;list.selected=selected;
  return UI_FUNC(ListEx)(context,&list);
}
