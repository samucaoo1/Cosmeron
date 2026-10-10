#include <assert.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/UI/UI.h"
#define OK STATUS_CONST(SUCCESS)
static TQUAD_TYPE(uint16) r(uint16_t l,uint16_t t,uint16_t right,uint16_t b) {
  TQUAD_TYPE(uint16) q={0};q.left=l;q.top=t;q.right=right;q.bottom=b;return q;
}
static void test_theme(void) {
  UI_TYPE(TTheme) theme=UI_FUNC(Theme_Dark)();
  UI_TYPE(TContext) ctx={0};
  UI_TYPE(TTheme) result;
  UI_TYPE(TVisual) visual=UI_FUNC(Visual_Default)();
  assert(theme.glyphs.checkboxOn && theme.button.border==UI_CONST(BORDER_SINGLE));
  assert(UI_FUNC(Theme_Set)(&ctx,&theme)==OK);
  assert(UI_FUNC(Theme_Get)(&ctx,&result)==OK);
  assert(result.button.border==UI_CONST(BORDER_SINGLE));
  assert(UI_FUNC(Theme_Get)(NULL,&result)==STATUS_CONST(INVALID_ARGUMENT));
  assert(visual.border==UI_CONST(BORDER_NONE));
  assert(UI_FUNC(Glyphs_ASCII)().checkboxOff!=NULL);
  assert(UI_FUNC(Theme_Light)().glyphs.radioOn!=NULL);
  assert(UI_FUNC(Theme_Classic)().glyphs.selection!=NULL);
  assert(UI_FUNC(Theme_Monochrome)().glyphs.progressFull!=NULL);
}
static void test_ui(void) {
  TERMINAL_TYPE(TCanvas) canvas={0};
  UI_TYPE(TContext) ui={0};
  UI_TYPE(TVisual) accent=UI_FUNC(Visual_Default)();
  UI_TYPE(TButton) button={0};
  UI_TYPE(TCheckbox) checkbox={0};
  UI_TYPE(TSlider) slider={0};
  UI_TYPE(TInputText) input={0};
  UI_TYPE(TList) list={0};
  TQUAD_TYPE(uint16) piece;
  const char *items[]={"One","Two","Three"};
  char buffer[32]={0};
  size_t selected=0, length=0;
  bool pressed=false,checked=false;
  uint32_t chosen=1, point=0;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr;
  assert(TERMINAL_FUNC(Canvas_Create)(
      &canvas,(TDUAL_TYPE(uint16)){.col=50,.row=20})==OK);
  button.region=r(0,0,14,2);button.label="Run";
  checkbox.region=r(0,3,19,5);checkbox.label="Audio";checkbox.checked=true;
  slider.region=r(0,9,20,11);slider.minimum=0;slider.maximum=100;slider.value=50;
  input.region=r(22,0,45,2);input.buffer=buffer;input.capacity=sizeof(buffer);
  list.region=r(22,3,45,7);list.items=items;list.count=3;list.selected=&selected;
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(Label)(&ui,r(0,18,19,19),"Cosmeron")==OK);
  assert(UI_FUNC(ButtonEx)(&ui,&button)==OK);
  assert(UI_FUNC(CheckboxEx)(&ui,&checkbox)==OK);
  assert(UI_FUNC(Radio)(&ui,r(0,6,19,8),"Automatic",1,&chosen)==OK);
  assert(UI_FUNC(SliderEx)(&ui,&slider)==OK);
  assert(UI_FUNC(InputTextEx)(&ui,&input)==OK);
  assert(UI_FUNC(ListEx)(&ui,&list)==OK);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(ui.focus==UI_FUNC(GetId)(&ui,"Run"));
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
      (TDUAL_TYPE(uint16)){.x=0,.y=0},&point,&attr)==OK && point!=0);

  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_ENTER))==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(ButtonEx)(&ui,&button)==OK && button.pressed);
  assert(UI_FUNC(CheckboxEx)(&ui,&checkbox)==OK);
  assert(UI_FUNC(End)(&ui)==OK);

  assert(UI_FUNC(Focus_Set)(&ui,UI_FUNC(GetId)(&ui,"Audio"))==OK);
  assert(UI_FUNC(Key)(&ui,' ')==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(CheckboxEx)(&ui,&checkbox)==OK && !checkbox.checked);
  assert(UI_FUNC(End)(&ui)==OK);

  assert(UI_FUNC(Focus_Set)(&ui,UI_FUNC(GetId)(&ui,"##slider"))==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_RIGHT))==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(SliderEx)(&ui,&slider)==OK && slider.value==51);
  assert(UI_FUNC(End)(&ui)==OK);

  assert(UI_FUNC(Focus_Set)(&ui,UI_FUNC(GetId)(&ui,"##input"))==OK);
  assert(UI_FUNC(Key)(&ui,'A')==OK);
  assert(UI_FUNC(Key)(&ui,0xE9)==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(InputTextEx)(&ui,&input)==OK);
  assert(input.length==3 && buffer[0]=='A');
  assert(UI_FUNC(End)(&ui)==OK);

  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_BACKSPACE))==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(InputTextEx)(&ui,&input)==OK);
  assert(input.length==1 && strcmp(buffer,"A")==0);
  assert(UI_FUNC(End)(&ui)==OK);

  assert(UI_FUNC(Focus_Set)(&ui,UI_FUNC(GetId)(&ui,"##list"))==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_DOWN))==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(ListEx)(&ui,&list)==OK && selected==1);
  assert(UI_FUNC(End)(&ui)==OK);

  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  accent.border=UI_CONST(BORDER_DOUBLE);
  assert(UI_FUNC(Visual_Push)(&ui,&accent)==OK);
  assert(UI_FUNC(Button)(&ui,r(0,0,14,2),"Temporary",&pressed)==OK);
  assert(UI_FUNC(Visual_Pop)(&ui)==OK);
  assert(UI_FUNC(Progress)(&ui,r(0,6,20,8),0.7f)==OK);
  assert(UI_FUNC(Separator)(&ui,r(0,12,20,12))==OK);
  assert(UI_FUNC(Row_Begin)(&ui,r(0,13,20,14),1)==OK);
  assert(UI_FUNC(Layout_Next)(&ui,5,&piece)==OK &&
         piece.left==0 && piece.right==4);
  assert(UI_FUNC(Layout_Remaining)(&ui,&piece)==OK &&
         piece.left==6);
  assert(UI_FUNC(Layout_End)(&ui)==OK);
  assert(UI_FUNC(Panel_Begin)(&ui,r(22,9,46,17),"Info")==OK);
  assert(UI_FUNC(Label)(&ui,r(0,0,14,0),"Panel text")==OK);
  assert(UI_FUNC(Panel_End)(&ui)==OK);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(Theme_Set)(&ui,&ui.theme)==OK);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
static void test_direct(void) {
  TERMINAL_TYPE(TCanvas) canvas={0};
  UI_TYPE(TContext) ctx={0};
  bool pressed=false, checked=true;
  int32_t value=5;
  size_t length=0, selected=0;
  char buffer[12]={0};
  const char *items[]={"Alpha","Beta"};
  uint32_t choice=0;
  assert(TERMINAL_FUNC(Canvas_Create)(
      &canvas,(TDUAL_TYPE(uint16)){.col=40,.row=18})==OK);
  assert(UI_FUNC(Begin)(&ctx,&canvas)==OK);
  assert(UI_FUNC(Button)(&ctx,r(0,0,14,2),"Go",&pressed)==OK);
  assert(UI_FUNC(Checkbox)(&ctx,r(0,3,18,5),"Ready",&checked)==OK);
  assert(UI_FUNC(Radio)(&ctx,r(0,6,18,8),"A",0,&choice)==OK);
  assert(UI_FUNC(Slider)(&ctx,r(0,9,18,11),0,10,&value)==OK);
  assert(UI_FUNC(Progress)(&ctx,r(20,9,38,11),0.2f)==OK);
  assert(UI_FUNC(InputText)(&ctx,r(20,0,38,2),buffer,sizeof(buffer),&length)==OK);
  assert(UI_FUNC(List)(&ctx,r(20,3,38,7),items,2,&selected)==OK);
  assert(UI_FUNC(End)(&ctx)==OK);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
int main(void) {test_theme();test_ui();test_direct();return 0;}
