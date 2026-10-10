#pragma once
/* Selection and cursor offsets are always UTF-8 byte offsets. */
static inline bool UI_INS(Boundary)(const UI_TYPE(TInputText) *input,size_t at) {
  return at<=input->length && (at==input->length ||
    ((unsigned char)input->buffer[at]&0xC0U)!=0x80U);
}
static inline bool UI_INS(EditValid)(const UI_TYPE(TInputText) *input) {
  return input && input->buffer && input->capacity && input->length<input->capacity &&
     input->buffer[input->length]==0 &&
     UI_INS(Boundary)(input,input->caret) &&
     UI_INS(Boundary)(input,input->selectionAnchor);
}
static inline size_t UI_INS(Previous)(const UI_TYPE(TInputText) *input,size_t at) {
  if(!at)return 0;
  do { --at; }
  while(at && ((unsigned char)input->buffer[at]&0xC0U)==0x80U);
  return at;
}
static inline size_t UI_INS(Next)(const UI_TYPE(TInputText) *input,size_t at) {
  if(at>=input->length)return input->length;
  ++at;
  while(at<input->length &&
        ((unsigned char)input->buffer[at]&0xC0U)==0x80U)++at;
  return at;
}
static inline size_t UI_INS(SelectionStart)(const UI_TYPE(TInputText) *input) {
  return input->caret<input->selectionAnchor?input->caret:input->selectionAnchor;
}
static inline size_t UI_INS(SelectionEnd)(const UI_TYPE(TInputText) *input) {
  return input->caret>input->selectionAnchor?input->caret:input->selectionAnchor;
}
static inline void UI_INS(DeleteRange)(
    UI_TYPE(TInputText) *input,size_t start,size_t finish) {
  memmove(input->buffer+start,input->buffer+finish,
          input->length-finish+1U);
  input->length-=finish-start;
  input->caret=input->selectionAnchor=start;
  if(input->firstVisible>input->length)input->firstVisible=0;
}
static inline void UI_INS(DeleteSelection)(UI_TYPE(TInputText) *input) {
  UI_INS(DeleteRange)(input,UI_INS(SelectionStart)(input),
                             UI_INS(SelectionEnd)(input));
}
UI_INPUT_TEXT_SELECT_PROTOTYPE {
  if(!UI_INS(EditValid)(input) ||
     !UI_INS(Boundary)(input,anchor) ||
     !UI_INS(Boundary)(input,caret))
    return STATUS_CONST(INVALID_ARGUMENT);
  input->selectionAnchor=anchor;input->caret=caret;
  return STATUS_CONST(SUCCESS);
}
UI_INPUT_TEXT_COPY_PROTOTYPE {
  size_t start,finish,n;
  if(!UI_INS(EditValid)(input) || !buffer || !outLength)
    return STATUS_CONST(INVALID_ARGUMENT);
  start=UI_INS(SelectionStart)(input);
  finish=UI_INS(SelectionEnd)(input);n=finish-start;
  if(n>=capacity)return STATUS_CONST(INSUFFICIENT_SPACE);
  memcpy(buffer,input->buffer+start,n);
  buffer[n]=0;*outLength=n;return STATUS_CONST(SUCCESS);
}
UI_INPUT_TEXT_CUT_PROTOTYPE {
  OPSTATUS status=UI_FUNC(InputText_Copy)(input,buffer,capacity,outLength);
  if(status!=STATUS_CONST(SUCCESS))return status;
  UI_INS(DeleteSelection)(input);
  return STATUS_CONST(SUCCESS);
}
UI_INPUT_TEXT_PASTE_PROTOTYPE {
  size_t start,finish,removed;
  if(!UI_INS(EditValid)(input) || (!utf8Text && byteLength))
    return STATUS_CONST(INVALID_ARGUMENT);
  if(byteLength && (memchr(utf8Text,0,byteLength) ||
     !TEXT_UTF8_FUNC(Validate)((const unsigned char *)utf8Text,byteLength)))
    return STATUS_CONST(INVALID_SEQUENCE);
  start=UI_INS(SelectionStart)(input);
  finish=UI_INS(SelectionEnd)(input);
  removed=finish-start;
  if(byteLength>SIZE_MAX-(input->length-removed)-1U ||
     input->length-removed+byteLength>=input->capacity)
    return STATUS_CONST(INSUFFICIENT_SPACE);
  /* Caller must not use the edited buffer as the paste source. */
  /* Paste sources must not alias the destination buffer. */
  memmove(input->buffer+start+byteLength,input->buffer+finish,
          input->length-finish+1U);
  if(byteLength)memcpy(input->buffer+start,utf8Text,byteLength);
  input->length=input->length-removed+byteLength;
  input->caret=input->selectionAnchor=start+byteLength;
  return STATUS_CONST(SUCCESS);
}
static inline size_t UI_INS(LineStart)(const UI_TYPE(TInputText) *input,size_t at) {
  while(at && input->buffer[at-1]!='\n')--at;
  return at;
}
static inline size_t UI_INS(LineEnd)(const UI_TYPE(TInputText) *input,size_t at) {
  while(at<input->length && input->buffer[at]!='\n')++at;
  return at;
}
/* Text-aware cursor movement, based on Unicode scalars rather than bytes. */
static inline size_t UI_INS(Column)(const UI_TYPE(TInputText) *input,size_t at) {
  size_t cursor=UI_INS(LineStart)(input,at),n=0;
  while(cursor<at) {cursor=UI_INS(Next)(input,cursor);++n;}
  return n;
}
static inline size_t UI_INS(Advance)(
    const UI_TYPE(TInputText) *input,size_t at,size_t scalars) {
  while(scalars && at<input->length && input->buffer[at]!='\n'){
    at=UI_INS(Next)(input,at);--scalars;
  }
  return at;
}
static inline OPSTATUS UI_INS(TextEdit)(
    UI_TYPE(TContext) *context,UI_TYPE(TInputText) *input,
    UI_TYPE(WidgetId) id,bool multiline) {
  size_t i;
  if(!UI_INS(EditValid)(input))
    return STATUS_CONST(INVALID_ARGUMENT);
  if(!TEXT_UTF8_FUNC(Validate)((const unsigned char *)input->buffer,input->length))
    return STATUS_CONST(INVALID_SEQUENCE);
  if(input->disabled || context->focus!=id)return STATUS_CONST(SUCCESS);
  for(i=0;i<context->keyCount;i++) {
    TERMINAL_TYPE(TKey) key=context->keys[i];
    size_t at=input->caret;
    if(!key)continue;
    if(key==TERMINAL_CONST(KEY_LEFT)) {
      if(input->caret!=input->selectionAnchor)
        input->caret=UI_INS(SelectionStart)(input);
      else input->caret=UI_INS(Previous)(input,at);
    }else if(key==TERMINAL_CONST(KEY_RIGHT)){
      if(input->caret!=input->selectionAnchor)
        input->caret=UI_INS(SelectionEnd)(input);
      else input->caret=UI_INS(Next)(input,at);
    }else if(key==TERMINAL_CONST(KEY_HOME)){
      input->caret=multiline?UI_INS(LineStart)(input,at):0;
    }else if(key==TERMINAL_CONST(KEY_END)){
      input->caret=multiline?UI_INS(LineEnd)(input,at):input->length;
    }else if(multiline && (key==TERMINAL_CONST(KEY_UP) ||
                          key==TERMINAL_CONST(KEY_DOWN))){
      size_t col=UI_INS(Column)(input,at),start=UI_INS(LineStart)(input,at);
      if(key==TERMINAL_CONST(KEY_UP)) {
        if(start>0)input->caret=UI_INS(Advance)(input,
            UI_INS(LineStart)(input,start-1U),col);
      }else {
        size_t finish=UI_INS(LineEnd)(input,at);
        if(finish<input->length)
          input->caret=UI_INS(Advance)(input,finish+1U,col);
      }
    }else if(key==TERMINAL_CONST(KEY_BACKSPACE) || key==8){
      if(input->caret!=input->selectionAnchor)UI_INS(DeleteSelection)(input);
      else if(at)UI_INS(DeleteRange)(input,UI_INS(Previous)(input,at),at);
    }else if(key==TERMINAL_CONST(KEY_DELETE)){
      if(input->caret!=input->selectionAnchor)UI_INS(DeleteSelection)(input);
      else if(at<input->length)UI_INS(DeleteRange)(input,at,UI_INS(Next)(input,at));
    }else if(key==TERMINAL_CONST(KEY_ENTER) || key=='\n'){
      if(multiline) {
        OPSTATUS status=UI_FUNC(InputText_Paste)(input,"\n",1);
        if(status!=STATUS_CONST(SUCCESS))return status;
      }
    }else if(key>=32U && key<=UINT32_C(0x10FFFF)) {
      unsigned char bytes[4];size_t count=0;
      OPSTATUS status=TEXT_UTF8_FUNC(Encode)((TEXT_TYPE(TChar32))key,
                 bytes,sizeof(bytes),&count);
      if(status!=STATUS_CONST(SUCCESS))return status;
      {
        char text[4];
        memcpy(text,bytes,count);
        status=UI_FUNC(InputText_Paste)(input,text,count);
      }
      if(status!=STATUS_CONST(SUCCESS))return status;
    }else continue;
    input->selectionAnchor=input->caret;
    context->keys[i]=0;
  }
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS UI_INS(DrawEdit)(
    UI_TYPE(TContext) *context,TQUAD_TYPE(uint16) region,
    const UI_TYPE(TVisual) *visual,UI_TYPE(WidgetId) id,
    UI_TYPE(TInputText) *input,bool multiline) {
  uint32_t x,y,left,right,top,bottom;
  size_t i;
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
  /* Horizontal scrolling for one-line edits. Vertical scrolling can be set
   * with firstVisible to a line boundary for multi-line text areas. */
  if(!multiline && input->caret<input->firstVisible)
    input->firstVisible=input->caret;
  if(!multiline) {
    size_t visibleColumns=0,j=input->firstVisible;
    while(j<input->caret) {
      TEXT_TYPE(TChar32) point;size_t count=0;int width=0;
      status=TEXT_UTF8_FUNC(Decode)(
          (const unsigned char *)input->buffer+j,input->length-j,&point,&count);
      if(status!=STATUS_CONST(SUCCESS))return status;
      status=TEXT_WIDTH_FUNC(Codepoint)(point,&width);
      if(status!=STATUS_CONST(SUCCESS))return status;
      visibleColumns+=(size_t)(width>0?width:0);j+=count;
    }
    while(visibleColumns>(size_t)(right-left) && input->firstVisible<input->caret){
      size_t next=UI_INS(Next)(input,input->firstVisible);
      TEXT_TYPE(TChar32) point;size_t count=0;int width=0;
      status=TEXT_UTF8_FUNC(Decode)((const unsigned char *)input->buffer+
           input->firstVisible,input->length-input->firstVisible,&point,&count);
      if(status!=STATUS_CONST(SUCCESS))return status;
      status=TEXT_WIDTH_FUNC(Codepoint)(point,&width);
      if(status!=STATUS_CONST(SUCCESS))return status;
      visibleColumns-=(size_t)(width>0?width:0);input->firstVisible=next;
    }
  }
  x=left;y=top;
  i=input->firstVisible;
  while(i<input->length && y<=bottom) {
    TEXT_TYPE(TChar32) point;size_t consumed=0;int width=0;
    size_t at=i;
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
    {
      TEXT_GRID_ATTRIBUTE_TYPE(Cell) glyphStyle=style;
      if(at>=UI_INS(SelectionStart)(input) &&
         at<UI_INS(SelectionEnd)(input))
        glyphStyle.flags|=TEXT_GRID_ATTRIBUTE_CONST(INVERSE);
      if(at==input->caret && context->focus==id && !input->disabled)
        glyphStyle.flags|=TEXT_GRID_ATTRIBUTE_CONST(INVERSE);
      status=UI_INS(Put)(context,x,y,point,glyphStyle);
    }
    if(status!=STATUS_CONST(SUCCESS))return status;
    x+=(uint32_t)width;
  }
  if(i==input->caret && context->focus==id && x<=right && y<=bottom) {
    style.flags|=TEXT_GRID_ATTRIBUTE_CONST(INVERSE);
    return UI_INS(Put)(context,x,y,' ',style);
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
     !input->disabled) {
    context->focus=id;
    /* Cursor moves to clicked scalar column on the first line. */
    {
      TERMINAL_TYPE(TCanvas) *cv=UI_INS(Canvas)(context);
      uint32_t x=(uint32_t)context->pointer.x-cv->origin.x;
      uint32_t start=input->region.left+1U;
      size_t at=multiline?UI_INS(LineStart)(input,input->firstVisible):0;
      if(x>start)at=UI_INS(Advance)(input,at,(size_t)(x-start));
      input->caret=input->selectionAnchor=at;
    }
  }
  status=UI_INS(TextEdit)(context,input,id,multiline);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,input->visual,&context->theme.input);
  return UI_INS(DrawEdit)(context,input->region,visual,id,input,multiline);
}
UI_INPUT_TEXT_EX_PROTOTYPE {return UI_INS(InputCommon)(context,input,false);}
UI_INPUT_TEXT_PROTOTYPE {
  UI_TYPE(TInputText) input={0};OPSTATUS status;
  if(!inoutLength)return STATUS_CONST(INVALID_ARGUMENT);
  input.region=region;input.buffer=buffer;input.capacity=capacity;
  input.length=*inoutLength;input.caret=input.selectionAnchor=input.length;
  status=UI_FUNC(InputTextEx)(context,&input);
  if(status==STATUS_CONST(SUCCESS))*inoutLength=input.length;
  return status;
}
UI_TEXT_AREA_EX_PROTOTYPE {return UI_INS(InputCommon)(context,input,true);}
UI_TEXT_AREA_PROTOTYPE {
  UI_TYPE(TTextArea) input={0};OPSTATUS status;
  if(!inoutLength)return STATUS_CONST(INVALID_ARGUMENT);
  input.region=region;input.buffer=buffer;input.capacity=capacity;
  input.length=*inoutLength;input.caret=input.selectionAnchor=input.length;
  status=UI_FUNC(TextAreaEx)(context,&input);
  if(status==STATUS_CONST(SUCCESS))*inoutLength=input.length;
  return status;
}

UI_LIST_EX_PROTOTYPE {
  UI_TYPE(WidgetId) id;
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) normal,highlight;
  uint32_t top,bottom,y;
  size_t i,visible;
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
    }
    if(context->scroll && UI_INS(Hovered)(context,list->region)) {
      if(context->scroll>0 && list->firstVisible)
        --list->firstVisible;
      else if(context->scroll<0 && list->firstVisible+1<list->count)
        ++list->firstVisible;
      context->scroll=0;
    }
    if(context->pointerReleased && context->held==id &&
       UI_INS(Hovered)(context,list->region)) {
      TERMINAL_TYPE(TCanvas) *canvas=UI_INS(Canvas)(context);
      uint32_t localY=(uint32_t)context->pointer.y-canvas->origin.y;
      uint32_t firstRow=(uint32_t)list->region.top+
           (visual->border!=UI_CONST(BORDER_NONE));
      size_t candidate=localY>=firstRow?
          list->firstVisible+(size_t)(localY-firstRow):list->count;
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
  if(top>bottom)return STATUS_CONST(OUT_OF_RANGE);
  visible=(size_t)(bottom-top+1U);
  if(!visible)return STATUS_CONST(SUCCESS);
  if(context->focus==id && list->count) {
    if(*list->selected<list->firstVisible)
      list->firstVisible=*list->selected;
    else if(*list->selected-list->firstVisible>=visible)
      list->firstVisible=*list->selected-visible+1U;
  }
  if(list->firstVisible>list->count)
    list->firstVisible=list->count;
  for(y=top;y<=bottom && list->firstVisible+(size_t)(y-top)<list->count;y++) {
    size_t index=list->firstVisible+(size_t)(y-top);
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
