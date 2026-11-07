#pragma once

/* All four widgets are rendered using UI's existing Canvas and TVisual.
 * Time and animation remain caller-owned; there are no implicit timers. */
static inline OPSTATUS UI_INS(OverlayParagraph)(
    UI_TYPE(TContext) *context,const char *message,
    uint32_t left,uint32_t top,uint32_t right,uint32_t bottom,
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) style) {
  size_t offset=0,bytes;
  uint32_t x=left,y=top;
  if(!message)return STATUS_CONST(INVALID_ARGUMENT);
  bytes=strlen(message);
  while(offset<bytes && y<=bottom) {
    TEXT_TYPE(TChar32) cp=0;
    size_t consumed=0;
    int width=0;
    OPSTATUS status=TEXT_UTF8_FUNC(Decode)(
        (const unsigned char *)message+offset,bytes-offset,&cp,&consumed);
    if(status!=STATUS_CONST(SUCCESS))return status;
    offset+=consumed;
    if(cp=='\n') { x=left; ++y; continue; }
    status=TEXT_WIDTH_FUNC(Codepoint)(cp,&width);
    if(status!=STATUS_CONST(SUCCESS))return status;
    if(width<1)return STATUS_CONST(NOT_SUPPORTED);
    if(x+(uint32_t)width-1U>right) { x=left; ++y; }
    if(y>bottom)break;
    status=UI_INS(Put)(context,x,y,cp,style);
    if(status!=STATUS_CONST(SUCCESS))return status;
    x+=(uint32_t)width;
  }
  return STATUS_CONST(SUCCESS);
}
static inline OPSTATUS UI_INS(OverlayInner)(
    const TQUAD_TYPE(uint16) *region,const UI_TYPE(TVisual) *visual,
    uint32_t *left,uint32_t *top,uint32_t *right,uint32_t *bottom) {
  uint32_t edge;
  if(!region||!visual||!left||!top||!right||!bottom)
    return STATUS_CONST(INVALID_ARGUMENT);
  edge=visual->border!=UI_CONST(BORDER_NONE)?1U:0U;
  if((uint32_t)region->right<edge+visual->padding.x ||
     (uint32_t)region->bottom<edge+visual->padding.y)
    return STATUS_CONST(OUT_OF_RANGE);
  *left=(uint32_t)region->left+edge+visual->padding.x;
  *top=(uint32_t)region->top+edge+visual->padding.y;
  *right=(uint32_t)region->right-edge-visual->padding.x;
  *bottom=(uint32_t)region->bottom-edge-visual->padding.y;
  return *left>*right||*top>*bottom?
      STATUS_CONST(OUT_OF_RANGE):STATUS_CONST(SUCCESS);
}
UI_DIALOG_EX_PROTOTYPE {
  static const char *const defaultActions[]={"OK"};
  const UI_TYPE(TVisual) *visual;
  const char *const *actions;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  UI_TYPE(WidgetId) id;
  uint32_t left,top,right,bottom,columns,actionY;
  size_t count,i;
  OPSTATUS status;
  if(!dialog || !dialog->message)return STATUS_CONST(INVALID_ARGUMENT);
  dialog->activated=false;dialog->dismissed=false;
  count=dialog->actionCount?dialog->actionCount:1U;
  actions=dialog->actionCount?dialog->actions:defaultActions;
  if(!actions || dialog->selected>=count)
    return STATUS_CONST(INVALID_ARGUMENT);
  for(i=0;i<count;i++) if(!actions[i])return STATUS_CONST(INVALID_ARGUMENT);
  status=UI_INS(Check)(context,dialog->region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,dialog->visual,&context->theme.panel);
  status=UI_INS(OverlayInner)(&dialog->region,visual,
                              &left,&top,&right,&bottom);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(bottom<=top)return STATUS_CONST(OUT_OF_RANGE);
  columns=right-left+1U;
  if(count>(size_t)columns)return STATUS_CONST(OUT_OF_RANGE);
  id=dialog->id?dialog->id:UI_FUNC(GetId)(context,"##dialog");
  status=UI_INS(Register)(context,id,true);
  if(status!=STATUS_CONST(SUCCESS))return status;
  /* The caller should render inactive background widgets in a separate
   * application branch, then render this dialog last as an overlay. */
  context->focus=id;
  actionY=bottom;
  for(i=0;i<context->keyCount;i++){
    TERMINAL_TYPE(TKey) key=context->keys[i];
    bool consumed=true;
    if(key==TERMINAL_CONST(KEY_LEFT) || key==TERMINAL_CONST(KEY_UP)){
      dialog->selected=(dialog->selected+count-1U)%count;
    }else if(key==TERMINAL_CONST(KEY_RIGHT) ||
             key==TERMINAL_CONST(KEY_DOWN)){
      dialog->selected=(dialog->selected+1U)%count;
    }else if(key==TERMINAL_CONST(KEY_HOME)){
      dialog->selected=0;
    }else if(key==TERMINAL_CONST(KEY_END)){
      dialog->selected=count-1U;
    }else if(key==TERMINAL_CONST(KEY_ENTER)||key==' '){
      dialog->activated=true;
    }else if(key==TERMINAL_CONST(KEY_ESCAPE)){
      dialog->dismissed=true;
    }else consumed=false;
    if(consumed)context->keys[i]=0;
  }
  if(context->pointerKnown &&
     UI_INS(Hovered)(context,dialog->region)){
    TERMINAL_TYPE(TCanvas) *cv=UI_INS(Canvas)(context);
    if(context->pointer.x>=cv->origin.x &&
       context->pointer.y>=cv->origin.y) {
      uint32_t px=(uint32_t)context->pointer.x-cv->origin.x;
      uint32_t py=(uint32_t)context->pointer.y-cv->origin.y;
      if(py==actionY && px>=left && px<=right){
        size_t at=(size_t)(((uint64_t)(px-left)*count)/columns);
        if(at>=count)at=count-1U;
        if(context->pointerPressed){
          dialog->selected=at;
          context->held=id;
          context->dialogHeldChoice=at;
        }
        if(context->pointerReleased && context->held==id &&
           context->dialogHeldChoice==at) {
          dialog->selected=at;
          dialog->activated=true;
        }
      }
    }
  }
  style=visual->normal;
  status=UI_INS(Frame)(context,dialog->region,visual->border,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(dialog->title && *dialog->title){
    uint32_t at=left;
    status=UI_INS(MenuAlignedX)(visual,dialog->title,left,right,&at);
    if(status!=STATUS_CONST(SUCCESS))return status;
    status=UI_INS(Text)(context,at,top,right,dialog->title,style,NULL);
    if(status!=STATUS_CONST(SUCCESS))return status;
    ++top;
  }
  if(top<actionY) {
    status=UI_INS(OverlayParagraph)(context,dialog->message,left,top,right,
                                    actionY-1U,style);
    if(status!=STATUS_CONST(SUCCESS))return status;
  }
  for(i=0;i<count;i++){
    uint32_t slotLeft=left+(uint32_t)(((uint64_t)i*columns)/count);
    uint32_t slotRight=left+(uint32_t)(((uint64_t)(i+1U)*columns)/count)-1U;
    uint32_t at=slotLeft,x;
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) cell=
        i==dialog->selected?visual->focused:visual->normal;
    for(x=slotLeft;x<=slotRight;x++){
      status=UI_INS(Put)(context,x,actionY,' ',cell);
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
    status=UI_INS(MenuAlignedX)(visual,actions[i],slotLeft,slotRight,&at);
    if(status!=STATUS_CONST(SUCCESS))return status;
    status=UI_INS(Text)(context,at,actionY,slotRight,actions[i],cell,NULL);
    if(status!=STATUS_CONST(SUCCESS))return status;
  }
  return STATUS_CONST(SUCCESS);
}
UI_DIALOG_PROTOTYPE {
  UI_TYPE(TDialog) dialog={0};
  OPSTATUS status;
  if(!outDismissed)return STATUS_CONST(INVALID_ARGUMENT);
  dialog.region=region;dialog.title=title;dialog.message=message;
  status=UI_FUNC(DialogEx)(context,&dialog);
  if(status==STATUS_CONST(SUCCESS))
    *outDismissed=dialog.activated||dialog.dismissed;
  return status;
}
UI_CONFIRM_EX_PROTOTYPE {
  static const char *const choices[]={"Yes","No"};
  UI_TYPE(TDialog) effective;
  OPSTATUS status;
  if(!dialog)return STATUS_CONST(INVALID_ARGUMENT);
  effective=*dialog;
  if(!effective.actions && !effective.actionCount){
    effective.actions=choices;effective.actionCount=2U;
  }else if(!effective.actions||effective.actionCount!=2U)
    return STATUS_CONST(INVALID_ARGUMENT);
  status=UI_FUNC(DialogEx)(context,&effective);
  if(status==STATUS_CONST(SUCCESS)){
    dialog->selected=effective.selected;
    dialog->activated=effective.activated;
    dialog->dismissed=effective.dismissed;
  }
  return status;
}
UI_CONFIRM_PROTOTYPE {
  UI_TYPE(TDialog) dialog={0};
  UI_TYPE(WidgetId) id;
  OPSTATUS status;
  if(!context || !outAccepted || !outRejected)
    return STATUS_CONST(INVALID_ARGUMENT);
  id=UI_FUNC(GetId)(context,"##confirm");
  dialog.id=id;
  dialog.region=region;
  dialog.message=message;
  dialog.selected=context->dialogChoiceId==id?context->dialogChoice:0;
  status=UI_FUNC(ConfirmEx)(context,&dialog);
  if(status==STATUS_CONST(SUCCESS)){
    context->dialogChoiceId=id;
    context->dialogChoice=dialog.selected;
    *outAccepted=dialog.activated && dialog.selected==0;
    *outRejected=dialog.dismissed ||
                 (dialog.activated && dialog.selected==1);
  }
  return status;
}
UI_STATUS_BAR_EX_PROTOTYPE {
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  uint32_t left,top,right,bottom,rightStart;
  size_t cols=0,units=0;
  OPSTATUS status;
  if(!bar)return STATUS_CONST(INVALID_ARGUMENT);
  status=UI_INS(Check)(context,bar->region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,bar->visual,&context->theme.text);
  status=UI_INS(OverlayInner)(&bar->region,visual,
                              &left,&top,&right,&bottom);
  if(status!=STATUS_CONST(SUCCESS))return status;
  style=visual->normal;
  status=UI_INS(Frame)(context,bar->region,visual->border,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  rightStart=right+1U;
  if(bar->right && *bar->right) {
    status=TEXT_WIDTH_FUNC(UTF8)((const unsigned char *)bar->right,
        strlen(bar->right),&cols,&units);
    if(status!=STATUS_CONST(SUCCESS))return status;
    rightStart=cols>(size_t)(right-left+1U)?left:
               right+1U-(uint32_t)cols;
    status=UI_INS(Text)(context,rightStart,top,right,bar->right,style,NULL);
    if(status!=STATUS_CONST(SUCCESS))return status;
  }
  if(bar->left && *bar->left && rightStart>left+1U)
    return UI_INS(Text)(context,left,top,rightStart-2U,bar->left,style,NULL);
  if(bar->left && *bar->left && (!bar->right||!*bar->right))
    return UI_INS(Text)(context,left,top,right,bar->left,style,NULL);
  return STATUS_CONST(SUCCESS);
}
UI_STATUS_BAR_PROTOTYPE {
  UI_TYPE(TStatusBar) bar={0};
  bar.region=region;bar.left=left;bar.right=right;
  return UI_FUNC(StatusBarEx)(context,&bar);
}
UI_TOAST_EX_PROTOTYPE {
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  uint32_t left,top,right,bottom;
  OPSTATUS status;
  if(!toast)return STATUS_CONST(INVALID_ARGUMENT);
  if(!toast->visible ||
     (toast->expiresAtTick && toast->nowTick>=toast->expiresAtTick))
    return STATUS_CONST(SUCCESS);
  if(!toast->message)return STATUS_CONST(INVALID_ARGUMENT);
  status=UI_INS(Check)(context,toast->region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,toast->visual,&context->theme.panel);
  status=UI_INS(OverlayInner)(&toast->region,visual,
                              &left,&top,&right,&bottom);
  if(status!=STATUS_CONST(SUCCESS))return status;
  style=visual->normal;
  status=UI_INS(Frame)(context,toast->region,visual->border,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(toast->title && *toast->title) {
    status=UI_INS(Text)(context,left,top,right,toast->title,
                        visual->focused,NULL);
    if(status!=STATUS_CONST(SUCCESS))return status;
    ++top;
  }
  if(top<=bottom)
    return UI_INS(OverlayParagraph)(context,toast->message,
                                    left,top,right,bottom,style);
  return STATUS_CONST(SUCCESS);
}
UI_TOAST_PROTOTYPE {
  UI_TYPE(TToast) toast={0};
  toast.region=region;toast.message=message;toast.visible=visible;
  return UI_FUNC(ToastEx)(context,&toast);
}
UI_SPINNER_EX_PROTOTYPE {
  static const char *const asciiFrames[]={"|","/","-","\\"};
  static const char *const unicodeFrames[]={"◐","◓","◑","◒"};
  const char *const *frames;
  const UI_TYPE(TVisual) *visual;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  uint32_t left,top,right,bottom,at;
  size_t count;
  OPSTATUS status;
  if(!spinner)return STATUS_CONST(INVALID_ARGUMENT);
  status=UI_INS(Check)(context,spinner->region);
  if(status!=STATUS_CONST(SUCCESS))return status;
  visual=UI_INS(ChooseVisual)(context,spinner->visual,&context->theme.text);
  status=UI_INS(OverlayInner)(&spinner->region,visual,
                              &left,&top,&right,&bottom);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(spinner->frames || spinner->frameCount){
    if(!spinner->frames || !spinner->frameCount)
      return STATUS_CONST(INVALID_ARGUMENT);
    frames=spinner->frames;count=spinner->frameCount;
  }else if(context->theme.glyphs.selection &&
           strcmp(context->theme.glyphs.selection,">")==0){
    frames=asciiFrames;count=sizeof(asciiFrames)/sizeof(asciiFrames[0]);
  }else {
    frames=unicodeFrames;
    count=sizeof(unicodeFrames)/sizeof(unicodeFrames[0]);
  }
  if(!frames[spinner->phase%count])return STATUS_CONST(INVALID_ARGUMENT);
  style=visual->normal;
  status=UI_INS(Frame)(context,spinner->region,visual->border,style);
  if(status!=STATUS_CONST(SUCCESS))return status;
  top+=(bottom-top)/2U;
  status=UI_INS(Text)(context,left,top,right,
                      frames[spinner->phase%count],style,&at);
  if(status!=STATUS_CONST(SUCCESS))return status;
  if(spinner->label && *spinner->label && at<right)
    return UI_INS(Text)(context,at+1U,top,right,spinner->label,style,NULL);
  return STATUS_CONST(SUCCESS);
}
UI_SPINNER_PROTOTYPE {
  UI_TYPE(TSpinner) spinner={0};
  spinner.region=region;spinner.label=label;spinner.phase=phase;
  return UI_FUNC(SpinnerEx)(context,&spinner);
}
