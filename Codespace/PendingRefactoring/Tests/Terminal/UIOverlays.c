#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/UI/UI.h"

#define OK STATUS_CONST(SUCCESS)
static TQUAD_TYPE(uint16) region(uint16_t l,uint16_t t,uint16_t r,uint16_t b){
  TQUAD_TYPE(uint16) q={0};q.left=l;q.top=t;q.right=r;q.bottom=b;return q;
}
static uint32_t cell(const TERMINAL_TYPE(TCanvas) *cv,uint16_t x,uint16_t y){
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) a;
  uint32_t cp=0;
  assert(TERMINAL_FUNC(Canvas_Get)(cv,
    (TDUAL_TYPE(uint16)){.x=x,.y=y},&cp,&a)==OK);
  return cp;
}
static void test_dialog_keyboard(void){
  TERMINAL_TYPE(TCanvas) cv={0};
  UI_TYPE(TContext) ui={0};
  UI_TYPE(TDialog) dialog={0};
  const char *actions[]={"Accept","Decline","More"};
  assert(TERMINAL_FUNC(Canvas_Create)(
      &cv,(TDUAL_TYPE(uint16)){.col=58,.row=19})==OK);
  dialog.region=region(2,2,42,12);dialog.title="QUEST";
  dialog.message="Continue\nwith the mission?";
  dialog.actions=actions;dialog.actionCount=3;
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(DialogEx)(&ui,&dialog)==OK);
  assert(!dialog.activated && !dialog.dismissed);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(cell(&cv,3,4)=='C');
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_RIGHT))==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_ENTER))==OK);
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(DialogEx)(&ui,&dialog)==OK);
  assert(dialog.selected==1 && dialog.activated && !dialog.dismissed);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_ESCAPE))==OK);
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(DialogEx)(&ui,&dialog)==OK);
  assert(!dialog.activated && dialog.dismissed);
  assert(UI_FUNC(End)(&ui)==OK);
  dialog.selected=4;
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(DialogEx)(&ui,&dialog)==STATUS_CONST(INVALID_ARGUMENT));
  assert(UI_FUNC(End)(&ui)==OK);
  TERMINAL_FUNC(Canvas_Free)(&cv);
}
static void test_confirm_click(void){
  TERMINAL_TYPE(TCanvas) cv={0};
  UI_TYPE(TContext) ui={0};
  UI_TYPE(TDialog) dialog={0};
  bool accepted=false,rejected=false;
  assert(TERMINAL_FUNC(Canvas_Create)(
      &cv,(TDUAL_TYPE(uint16)){.col=44,.row=12})==OK);
  dialog.region=region(2,1,33,8);
  dialog.message="Delete save?";
  assert(UI_FUNC(PointerMove)(&ui,
      (TDUAL_TYPE(uint16)){.x=25,.y=7})==OK);
  assert(UI_FUNC(PointerButton)(&ui,true)==OK);
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(ConfirmEx)(&ui,&dialog)==OK);
  assert(dialog.selected==1 && !dialog.activated);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(PointerButton)(&ui,false)==OK);
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(ConfirmEx)(&ui,&dialog)==OK);
  assert(dialog.selected==1 && dialog.activated);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(Confirm)(&ui,region(2,1,33,8),
      "Overwrite?",&accepted,&rejected)==OK);
  assert(!accepted && !rejected);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_RIGHT))==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_ENTER))==OK);
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(Confirm)(&ui,region(2,1,33,8),
      "Overwrite?",&accepted,&rejected)==OK);
  assert(!accepted && rejected);
  assert(UI_FUNC(End)(&ui)==OK);
  TERMINAL_FUNC(Canvas_Free)(&cv);
}
static void test_status_toast_spinner(void){
  TERMINAL_TYPE(TCanvas) cv={0};
  UI_TYPE(TContext) ui={0};
  UI_TYPE(TToast) toast={0};
  UI_TYPE(TSpinner) spin={0};
  UI_TYPE(TTheme) theme=UI_FUNC(Theme_Classic)();
  static const char *const custom[]={"a","b"};
  bool dismissed=false;
  assert(TERMINAL_FUNC(Canvas_Create)(
      &cv,(TDUAL_TYPE(uint16)){.col=50,.row=16})==OK);
  assert(UI_FUNC(Theme_Set)(&ui,&theme)==OK);
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(StatusBar)(&ui,region(0,0,39,0),"HP 100","F1 Help")==OK);
  assert(cell(&cv,0,0)=='H' && cell(&cv,33,0)=='F');
  assert(UI_FUNC(Spinner)(&ui,region(0,1,22,1),"Loading",1)==OK);
  assert(cell(&cv,0,1)=='/');
  spin.region=region(0,2,15,2);
  spin.frames=custom;spin.frameCount=2;spin.phase=5;
  assert(UI_FUNC(SpinnerEx)(&ui,&spin)==OK);
  assert(cell(&cv,0,2)=='b');
  toast.region=region(2,4,40,10);
  toast.title="NOTICE";toast.message="Saved!";
  toast.visible=true;toast.nowTick=10;toast.expiresAtTick=20;
  assert(UI_FUNC(ToastEx)(&ui,&toast)==OK);
  assert(cell(&cv,3,6)=='S');
  assert(UI_FUNC(Dialog)(&ui,region(2,11,40,15),"Warning",
      "Press Enter to close",&dismissed)==OK);
  assert(!dismissed);
  assert(UI_FUNC(End)(&ui)==OK);
  TERMINAL_FUNC(Canvas_Clear)(&cv);
  toast.nowTick=20;
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(ToastEx)(&ui,&toast)==OK);
  assert(cell(&cv,3,6)==' ');
  assert(UI_FUNC(Toast)(&ui,region(0,4,22,8),"Transient",false)==OK);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_ENTER))==OK);
  assert(UI_FUNC(Begin)(&ui,&cv)==OK);
  assert(UI_FUNC(Dialog)(&ui,region(2,11,40,15),"Warning",
       "Press Enter to close",&dismissed)==OK);
  assert(dismissed);
  assert(UI_FUNC(End)(&ui)==OK);
  TERMINAL_FUNC(Canvas_Free)(&cv);
}
int main(void){
  test_dialog_keyboard();
  test_confirm_click();
  test_status_toast_spinner();
  return 0;
}
