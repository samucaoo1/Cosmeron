#include <assert.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/UI/UI.h"
#define OK STATUS_CONST(SUCCESS)

static TQUAD_TYPE(uint16) box(uint16_t l,uint16_t t,uint16_t r,uint16_t b){
  TQUAD_TYPE(uint16) region={0};
  region.left=l;region.top=t;region.right=r;region.bottom=b;return region;
}
static void test_terminal_events(void){
  TERMINAL_TYPE(TCapabilities) caps={0};
  TERMINAL_TYPE(TEvent) event={0};
  bool hasEvent=false;
  assert(TERMINAL_FUNC(Capabilities_Get)(&caps)==OK);
  assert(TERMINAL_FUNC(Capabilities_Get)(NULL)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(Event_Poll)(NULL,&hasEvent)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(Event_Poll)(&event,NULL)==STATUS_CONST(INVALID_ARGUMENT));
  if(!TERMINAL_INS(IsTTY)(stdin)){
    assert(TERMINAL_FUNC(Mouse_Enable)(true)==STATUS_CONST(NOT_AVAILABLE));
    assert(TERMINAL_FUNC(Event_Poll)(&event,&hasEvent)==STATUS_CONST(NOT_AVAILABLE));
  }
#if !OS_WINDOWS
  {
    TERMINAL_TYPE(TEvent) mouse={0};
    assert(TERMINAL_INS(ParseMouseSGR)("0;14;8M",&mouse));
    assert(mouse.kind==TERMINAL_CONST(EVENT_POINTER_BUTTON));
    assert(mouse.pressed && mouse.position.x==13 && mouse.position.y==7);
    assert(TERMINAL_INS(ParseMouseSGR)("0;14;8m",&mouse));
    assert(!mouse.pressed);
    assert(TERMINAL_INS(ParseMouseSGR)("64;4;2M",&mouse));
    assert(mouse.kind==TERMINAL_CONST(EVENT_SCROLL) && mouse.scroll==1);
    assert(TERMINAL_INS(ParseMouseSGR)("65;4;2M",&mouse));
    assert(mouse.scroll==-1);
    assert(!TERMINAL_INS(ParseMouseSGR)("bad",&mouse));
    assert(!TERMINAL_INS(ParseMouseSGR)("1;0;0M",&mouse));
  }
#endif
}
static void test_editor_api(void){
  char buffer[40]="A\xC3\xA9" "B";
  char copy[40]={0};
  size_t copied=0;
  UI_TYPE(TInputText) input={0};
  input.buffer=buffer;input.capacity=sizeof(buffer);input.length=4;
  input.caret=4;input.selectionAnchor=4;
  assert(UI_FUNC(InputText_Select)(&input,1,3)==OK);
  assert(UI_FUNC(InputText_Copy)(&input,copy,sizeof(copy),&copied)==OK);
  assert(copied==2 && strcmp(copy,"\xC3\xA9")==0);
  assert(UI_FUNC(InputText_Cut)(&input,copy,sizeof(copy),&copied)==OK);
  assert(strcmp(buffer,"AB")==0 && input.length==2 && input.caret==1);
  assert(UI_FUNC(InputText_Paste)(&input,"\xE4\xB8\xAD",3)==OK);
  assert(input.length==5 && input.caret==4);
  assert(strcmp(buffer,"A\xE4\xB8\xAD" "B")==0);
  assert(UI_FUNC(InputText_Select)(&input,2,4)==STATUS_CONST(INVALID_ARGUMENT));
  assert(UI_FUNC(InputText_Select)(&input,4,4)==OK);
  assert(UI_FUNC(InputText_Copy)(&input,copy,1,&copied)==OK && copied==0);
  assert(UI_FUNC(InputText_Paste)(&input,"\xFF",1)==STATUS_CONST(INVALID_SEQUENCE));
  assert(UI_FUNC(InputText_Paste)(&input,"",0)==OK);
}
static void test_cursor_keys(void){
  TERMINAL_TYPE(TCanvas) canvas={0};
  UI_TYPE(TContext) ctx={0};
  UI_TYPE(TInputText) edit={0};
  char str[20]="abc";
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
       (TDUAL_TYPE(uint16)){.col=32,.row=12})==OK);
  edit.buffer=str;edit.capacity=sizeof(str);edit.length=3;
  edit.caret=3;edit.selectionAnchor=3;edit.region=box(0,0,20,2);
  assert(UI_FUNC(Focus_Set)(&ctx,UI_FUNC(GetId)(&ctx,"##input"))==OK);
  assert(UI_FUNC(Key)(&ctx,TERMINAL_CONST(KEY_LEFT))==OK);
  assert(UI_FUNC(Key)(&ctx,'X')==OK);
  assert(UI_FUNC(Begin)(&ctx,&canvas)==OK);
  assert(UI_FUNC(InputTextEx)(&ctx,&edit)==OK);
  assert(UI_FUNC(End)(&ctx)==OK);
  assert(strcmp(str,"abXc")==0 && edit.caret==3);
  assert(UI_FUNC(Key)(&ctx,TERMINAL_CONST(KEY_HOME))==OK);
  assert(UI_FUNC(Key)(&ctx,TERMINAL_CONST(KEY_DELETE))==OK);
  assert(UI_FUNC(Begin)(&ctx,&canvas)==OK);
  assert(UI_FUNC(InputTextEx)(&ctx,&edit)==OK);
  assert(UI_FUNC(End)(&ctx)==OK);
  assert(strcmp(str,"bXc")==0 && edit.caret==0);
  assert(UI_FUNC(Key)(&ctx,TERMINAL_CONST(KEY_END))==OK);
  assert(UI_FUNC(Key)(&ctx,TERMINAL_CONST(KEY_BACKSPACE))==OK);
  assert(UI_FUNC(Begin)(&ctx,&canvas)==OK);
  assert(UI_FUNC(InputTextEx)(&ctx,&edit)==OK);
  assert(UI_FUNC(End)(&ctx)==OK);
  assert(strcmp(str,"bX")==0);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
static void test_layout_and_list(void){
  TERMINAL_TYPE(TCanvas) canvas={0};
  UI_TYPE(TContext) context={0};
  UI_TYPE(TList) list={0};
  TDUAL_TYPE(uint16) measure={0};
  TQUAD_TYPE(uint16) region={0};
  const char *items[]={"0","1","2","3","4","5","6","7","8","9","10","11"};
  size_t selected=10;
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
      (TDUAL_TYPE(uint16)){.col=40,.row=12})==OK);
  assert(UI_FUNC(Layout_MeasureText)("ABC",(TDUAL_TYPE(uint16)){.x=2,.y=1},&measure)==OK);
  assert(measure.col==7 && measure.row==3);
  list.region=box(0,0,20,4);list.items=items;list.count=12;
  list.selected=&selected;
  assert(UI_FUNC(Begin)(&context,&canvas)==OK);
  assert(UI_FUNC(Row_Begin)(&context,box(22,0,39,4),1)==OK);
  assert(UI_FUNC(Layout_Share)(&context,1,2,&region)==OK);
  assert(region.left==22 && region.right==30);
  assert(UI_FUNC(Layout_End)(&context)==OK);
  assert(UI_FUNC(ListEx)(&context,&list)==OK);
  assert(list.firstVisible>0 && list.firstVisible<=selected);
  assert(UI_FUNC(End)(&context)==OK);
  assert(UI_FUNC(Resize_Take)(&context,&measure)==false);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
static void test_diff(void){
  TERMINAL_TYPE(TCanvas) current={0},previous={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  assert(TERMINAL_FUNC(Canvas_Create)(&current,
       (TDUAL_TYPE(uint16)){.col=8,.row=3})==OK);
  assert(TERMINAL_FUNC(Canvas_Copy)(&current,&previous)==OK);
  assert(TERMINAL_FUNC(Canvas_Set)(&current,
       (TDUAL_TYPE(uint16)){.x=1,.y=1},'X',style)==OK);
  if(!TERMINAL_INS(Interactive)()){
    assert(TERMINAL_FUNC(Canvas_UpdateDiff)(&current,&previous)==
           STATUS_CONST(NOT_AVAILABLE));
  }
  TERMINAL_FUNC(Canvas_Free)(&previous);
  TERMINAL_FUNC(Canvas_Free)(&current);
}
int main(void){
  test_terminal_events();test_editor_api();test_cursor_keys();
  test_layout_and_list();test_diff();return 0;
}
