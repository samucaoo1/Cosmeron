#pragma once
/* Event polling consumes the console input stream: applications must not
 * concurrently use stdio/GetChar/Key_Get on the same input descriptor. */
static inline bool *TERMINAL_INS(MouseEnabled)(void) {
  static bool value=false;return &value;
}
static inline bool *TERMINAL_INS(MouseOriginalSaved)(void) {
  static bool value=false;return &value;
}
#if OS_WINDOWS
static inline DWORD *TERMINAL_INS(MouseOriginalFlags)(void) {
  static DWORD value=0;return &value;
}
static inline WCHAR *TERMINAL_INS(HighSurrogate)(void) {
  static WCHAR value=0;return &value;
}
#else
static inline TERMINAL_TYPE(InputMode) *TERMINAL_INS(MouseOldMode)(void) {
  static TERMINAL_TYPE(InputMode) value=TERMINAL_CONST(INPUT_CANONICAL);
  return &value;
}
static inline int TERMINAL_INS(EventByte)(void) {
  int queued=TERMINAL_INS(KeyPop)();
  unsigned char byte=0;
  if(queued>=0)return queued;
  return read(fileno(stdin),&byte,1)==1?(int)byte:-1;
}
static inline int TERMINAL_INS(EventExtraByte)(void) {
  int queued=TERMINAL_INS(KeyPop)();
  return queued>=0?queued:TERMINAL_INS(KeyTimedRead)();
}
static inline bool TERMINAL_INS(EventNumber)(const char **p,uint32_t *out) {
  uint32_t n=0;
  const char *s=*p;
  if(*s<'0'||*s>'9')return false;
  while(*s>='0'&&*s<='9'){
    uint32_t digit=(uint32_t)(*s-'0');
    if(n>(UINT32_MAX-digit)/10U)return false;
    n=n*10U+digit;
    ++s;
  }
  *p=s;*out=n;return true;
}
static inline bool TERMINAL_INS(ParseMouseSGR)(
    const char *sequence,TERMINAL_TYPE(TEvent) *out) {
  uint32_t button,x,y;
  const char *p=sequence;
  if(!TERMINAL_INS(EventNumber)(&p,&button)||*p++!=';')return false;
  if(!TERMINAL_INS(EventNumber)(&p,&x)||*p++!=';')return false;
  if(!TERMINAL_INS(EventNumber)(&p,&y))return false;
  if((*p!='M'&&*p!='m')||p[1] || !x || !y ||
      x-1U>UINT16_MAX||y-1U>UINT16_MAX)return false;
  out->position=(TDUAL_TYPE(uint16)){.x=(uint16_t)(x-1U),
                                     .y=(uint16_t)(y-1U)};
  if(button&64U){
    out->kind=TERMINAL_CONST(EVENT_SCROLL);
    out->scroll=(button&1U)?-1:1;
  }else if((button&32U)!=0U || (((button&3U)==3U) && *p=='M')){
    out->kind=TERMINAL_CONST(EVENT_POINTER_MOVE);
  }else{
    out->kind=TERMINAL_CONST(EVENT_POINTER_BUTTON);
    out->pressed=(*p=='M');
  }
  return true;
}
static inline int TERMINAL_INS(EventEscape)(
    TERMINAL_TYPE(TEvent) *event) {
  int prefix=TERMINAL_INS(EventExtraByte)();
  int code;
  if(prefix!='[' && prefix!='O') {
    if(prefix>=0)TERMINAL_INS(KeyPush)((unsigned char)prefix);
    return 27;
  }
  code=TERMINAL_INS(EventExtraByte)();
  if(code=='<' && prefix=='[') {
    char sgr[48]={0};size_t n=0;
    int ch;
    while(n<sizeof(sgr)-1U) {
      ch=TERMINAL_INS(EventExtraByte)();
      if(ch<0)break;
      sgr[n++]=(char)ch;
      if(ch=='M'||ch=='m') {
        if(TERMINAL_INS(ParseMouseSGR)(sgr,event))return -2;
        break;
      }
    }
    return -1;
  }
  switch(code){
    case 'A':return (int)TERMINAL_CONST(KEY_UP);
    case 'B':return (int)TERMINAL_CONST(KEY_DOWN);
    case 'C':return (int)TERMINAL_CONST(KEY_RIGHT);
    case 'D':return (int)TERMINAL_CONST(KEY_LEFT);
    case 'H':return (int)TERMINAL_CONST(KEY_HOME);
    case 'F':return (int)TERMINAL_CONST(KEY_END);
    case 'Z':return '\t';
    default:break;
  }
  if(code=='1'||code=='3'||code=='4'||code=='7'||code=='8'){
    int terminator=TERMINAL_INS(EventExtraByte)();
    if(terminator=='~') {
      if(code=='3')return (int)TERMINAL_CONST(KEY_DELETE);
      if(code=='1'||code=='7')return (int)TERMINAL_CONST(KEY_HOME);
      return (int)TERMINAL_CONST(KEY_END);
    }
    if(terminator>=0)TERMINAL_INS(KeyPush)((unsigned char)terminator);
  }
  return -1;
}
static inline bool TERMINAL_INS(EventReady)(void) {
  int fd=fileno(stdin);
  fd_set set;
  struct timeval timeout={0,0};
  if(TERMINAL_INS(KeyQueue)()->count)return true;
  FD_ZERO(&set);FD_SET(fd,&set);
  return select(fd+1,&set,NULL,NULL,&timeout)>0;
}
static inline TDUAL_TYPE(uint16) *TERMINAL_INS(LastEventSize)(void) {
  static TDUAL_TYPE(uint16) size={0};return &size;
}
#endif
static void TERMINAL_INS(MouseAtExit)(void) {
  if(!*TERMINAL_INS(MouseEnabled)())return;
#if OS_WINDOWS
  if(*TERMINAL_INS(MouseOriginalSaved)())
    (void)SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE),
                         *TERMINAL_INS(MouseOriginalFlags)());
#else
  (void)fputs("\x1b[?1006l\x1b[?1003l\x1b[?1002l\x1b[?1000l",stdout);
  (void)fflush(stdout);
  (void)TERMINAL_FUNC(InputMode_Set)(*TERMINAL_INS(MouseOldMode)());
#endif
  *TERMINAL_INS(MouseEnabled)()=false;
}
TERMINAL_MOUSE_ENABLE_PROTOTYPE {
  static bool registered=false;
  if(!TERMINAL_INS(IsTTY)(stdin)||!TERMINAL_INS(IsTTY)(stdout))
    return STATUS_CONST(NOT_AVAILABLE);
  if(enabled==*TERMINAL_INS(MouseEnabled)())return STATUS_CONST(SUCCESS);
  if(enabled) {
    if(!registered) {
      if(atexit(TERMINAL_INS(MouseAtExit))!=0)
        return STATUS_CONST(GENERIC_ERROR);
      registered=true;
    }
#if OS_WINDOWS
    {
      HANDLE input=GetStdHandle(STD_INPUT_HANDLE);
      DWORD flags;
      if(!GetConsoleMode(input,&flags))return STATUS_CONST(GENERIC_ERROR);
      *TERMINAL_INS(MouseOriginalFlags)()=flags;
      *TERMINAL_INS(MouseOriginalSaved)()=true;
      flags|=ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT|ENABLE_EXTENDED_FLAGS;
      flags&=~ENABLE_QUICK_EDIT_MODE;
      if(!SetConsoleMode(input,flags))return STATUS_CONST(GENERIC_ERROR);
    }
#else
    {
      OPSTATUS status=TERMINAL_FUNC(InputMode_Get)(TERMINAL_INS(MouseOldMode)());
      if(status!=STATUS_CONST(SUCCESS))return status;
      status=TERMINAL_FUNC(InputMode_Set)(TERMINAL_CONST(INPUT_CBREAK));
      if(status!=STATUS_CONST(SUCCESS))return status;
      if(fputs("\x1b[?1000h\x1b[?1002h\x1b[?1003h\x1b[?1006h",stdout)==EOF ||
         fflush(stdout)!=0) {
        (void)TERMINAL_FUNC(InputMode_Set)(*TERMINAL_INS(MouseOldMode)());
        return STATUS_CONST(GENERIC_ERROR);
      }
    }
#endif
    *TERMINAL_INS(MouseEnabled)()=true;
    return STATUS_CONST(SUCCESS);
  }
  TERMINAL_INS(MouseAtExit)();
  return STATUS_CONST(SUCCESS);
}
TERMINAL_EVENT_POLL_PROTOTYPE {
  TERMINAL_TYPE(TEvent) event={0};
  if(!outEvent||!outAvailable)return STATUS_CONST(INVALID_ARGUMENT);
  *outAvailable=false;
  if(!TERMINAL_INS(IsTTY)(stdin))return STATUS_CONST(NOT_AVAILABLE);
#if OS_WINDOWS
  {
    HANDLE input=GetStdHandle(STD_INPUT_HANDLE);
    DWORD available=0,readCount=0;
    INPUT_RECORD record;
    if(!PeekConsoleInputW(input,&record,1,&available))
      return STATUS_CONST(GENERIC_ERROR);
    if(!available)return STATUS_CONST(SUCCESS);
    if(!ReadConsoleInputW(input,&record,1,&readCount) || !readCount)
      return STATUS_CONST(GENERIC_ERROR);
    if(record.EventType==WINDOW_BUFFER_SIZE_EVENT) {
      COORD size=record.Event.WindowBufferSizeEvent.dwSize;
      event.kind=TERMINAL_CONST(EVENT_RESIZE);
      event.size=(TDUAL_TYPE(uint16)){.col=(uint16_t)size.X,.row=(uint16_t)size.Y};
    }else if(record.EventType==MOUSE_EVENT) {
      MOUSE_EVENT_RECORD mouse=record.Event.MouseEvent;
      static bool down=false;
      bool now=(mouse.dwButtonState&FROM_LEFT_1ST_BUTTON_PRESSED)!=0;
      event.position=(TDUAL_TYPE(uint16)){
        .x=(uint16_t)mouse.dwMousePosition.X,
        .y=(uint16_t)mouse.dwMousePosition.Y};
      if(mouse.dwEventFlags&MOUSE_WHEELED) {
        event.kind=TERMINAL_CONST(EVENT_SCROLL);
        event.scroll=(SHORT)HIWORD(mouse.dwButtonState)>0?1:-1;
      }else if(down!=now) {
        down=now;
        event.kind=TERMINAL_CONST(EVENT_POINTER_BUTTON);
        event.pressed=now;
      }else if(mouse.dwEventFlags==MOUSE_MOVED) {
        event.kind=TERMINAL_CONST(EVENT_POINTER_MOVE);
      }else return STATUS_CONST(SUCCESS);
    }else if(record.EventType==KEY_EVENT && record.Event.KeyEvent.bKeyDown){
      KEY_EVENT_RECORD key=record.Event.KeyEvent;
      WCHAR code=key.uChar.UnicodeChar;
      event.kind=TERMINAL_CONST(EVENT_KEY);
      switch(key.wVirtualKeyCode){
        case VK_UP:event.key=TERMINAL_CONST(KEY_UP);break;
        case VK_DOWN:event.key=TERMINAL_CONST(KEY_DOWN);break;
        case VK_LEFT:event.key=TERMINAL_CONST(KEY_LEFT);break;
        case VK_RIGHT:event.key=TERMINAL_CONST(KEY_RIGHT);break;
        case VK_HOME:event.key=TERMINAL_CONST(KEY_HOME);break;
        case VK_END:event.key=TERMINAL_CONST(KEY_END);break;
        case VK_DELETE:event.key=TERMINAL_CONST(KEY_DELETE);break;
        case VK_RETURN:event.key=TERMINAL_CONST(KEY_ENTER);break;
        case VK_BACK:event.key=TERMINAL_CONST(KEY_BACKSPACE);break;
        case VK_TAB:event.key='\t';break;
        default:
          if(code>=0xD800 && code<=0xDBFF) {
            *TERMINAL_INS(HighSurrogate)()=code;
            return STATUS_CONST(SUCCESS);
          }
          if(code>=0xDC00 && code<=0xDFFF &&
             *TERMINAL_INS(HighSurrogate)()) {
            event.key=(TERMINAL_TYPE(TKey))(0x10000U+
              (((uint32_t)*TERMINAL_INS(HighSurrogate)()-0xD800U)<<10)+
              (uint32_t)code-0xDC00U);
            *TERMINAL_INS(HighSurrogate)()=0;
          }else {
            *TERMINAL_INS(HighSurrogate)()=0;
            if(!code)return STATUS_CONST(SUCCESS);
            event.key=(TERMINAL_TYPE(TKey))code;
          }
          break;
      }
    }else return STATUS_CONST(SUCCESS);
  }
#else
  {
    struct winsize size;
    static bool sizeKnown=false;
    TDUAL_TYPE(uint16) *previous=TERMINAL_INS(LastEventSize)();
    TERMINAL_TYPE(InputMode) prior=*TERMINAL_INS(ModeState)();
    bool changed=prior==TERMINAL_CONST(INPUT_CANONICAL);
    int first;
    if(ioctl(fileno(stdin),TIOCGWINSZ,&size)==0){
      if(sizeKnown && (size.ws_col!=previous->col||size.ws_row!=previous->row)){
        event.kind=TERMINAL_CONST(EVENT_RESIZE);
        event.size=(TDUAL_TYPE(uint16)){.col=size.ws_col,.row=size.ws_row};
        *previous=event.size;*outEvent=event;*outAvailable=true;
        return STATUS_CONST(SUCCESS);
      }
      *previous=(TDUAL_TYPE(uint16)){.col=size.ws_col,.row=size.ws_row};
      sizeKnown=true;
    }
    if(changed) {
      OPSTATUS status=TERMINAL_FUNC(InputMode_Set)(TERMINAL_CONST(INPUT_CBREAK));
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
    if(!TERMINAL_INS(EventReady)()) {
      if(changed)(void)TERMINAL_FUNC(InputMode_Set)(prior);
      return STATUS_CONST(SUCCESS);
    }
    first=TERMINAL_INS(EventByte)();
    if(first<0) {
      if(changed)(void)TERMINAL_FUNC(InputMode_Set)(prior);
      return STATUS_CONST(SUCCESS);
    }
    event.kind=TERMINAL_CONST(EVENT_KEY);
    if(first==27) {
      int esc=TERMINAL_INS(EventEscape)(&event);
      if(esc==-1) {
        if(changed)(void)TERMINAL_FUNC(InputMode_Set)(prior);
        return STATUS_CONST(SUCCESS);
      }
      if(esc!=-2)event.key=(TERMINAL_TYPE(TKey))esc;
    }else if(first>=0xC2 && first<=0xF4){
      unsigned char bytes[4]={0};size_t needed,used=0,j;
      TEXT_TYPE(TChar32) point=0;
      bytes[0]=(unsigned char)first;
      needed=first<0xE0?2U:(first<0xF0?3U:4U);
      for(j=1;j<needed;j++) {
        int more=TERMINAL_INS(EventExtraByte)();
        if(more<0)break;
        bytes[j]=(unsigned char)more;
      }
      if(j==needed && TEXT_UTF8_FUNC(Decode)(bytes,needed,&point,&used)==
         STATUS_CONST(SUCCESS))event.key=(TERMINAL_TYPE(TKey))point;
      else {
        if(changed)(void)TERMINAL_FUNC(InputMode_Set)(prior);
        return STATUS_CONST(INVALID_SEQUENCE);
      }
    }else {
      event.key=(TERMINAL_TYPE(TKey))(unsigned char)first;
    }
    if(event.kind==TERMINAL_CONST(EVENT_KEY)) {
      if(event.key=='\n'||event.key=='\r')event.key=TERMINAL_CONST(KEY_ENTER);
      if(event.key==8 || event.key==127)event.key=TERMINAL_CONST(KEY_BACKSPACE);
    }
    if(changed) {
      OPSTATUS status=TERMINAL_FUNC(InputMode_Set)(prior);
      if(status!=STATUS_CONST(SUCCESS))return status;
    }
  }
#endif
  *outEvent=event;
  *outAvailable=true;
  return STATUS_CONST(SUCCESS);
}
