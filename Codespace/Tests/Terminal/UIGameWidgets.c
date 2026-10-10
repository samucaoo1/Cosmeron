#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../Cosmeron/Modules/Graphics/UI/UI.h"

#define OK STATUS_CONST(SUCCESS)
static TQUAD_TYPE(uint16) rect(uint16_t x,uint16_t y,uint16_t r,uint16_t b){
  TQUAD_TYPE(uint16) q={0};
  q.left=x;q.top=y;q.right=r;q.bottom=b;return q;
}
static void test_classic_menu(void){
  TERMINAL_TYPE(TCanvas) canvas={0};
  UI_TYPE(TContext) ui={0};
  UI_TYPE(TMenu) menu={0};
  const char *items[]={"NEW GAME","LOAD GAME","OPTIONS","QUIT"};
  const bool disabled[]={false,true,false,false};
  size_t selected=0;
  uint32_t cell;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr;
  assert(TERMINAL_FUNC(Canvas_Create)(
    &canvas,(TDUAL_TYPE(uint16)){.col=42,.row=15})==OK);
  menu.region=rect(2,1,24,9);
  menu.title="MAIN MENU";menu.items=items;menu.count=4;
  menu.selected=&selected;menu.itemDisabled=disabled;menu.wrap=true;
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK);
  assert(!menu.activated && !menu.cancelled);
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
    (TDUAL_TYPE(uint16)){.x=3,.y=2},&cell,&attr)==OK);
  assert(cell=='M');
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_DOWN))==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK && selected==2);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_END))==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_ENTER))==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK);
  assert(selected==3 && menu.activated);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_DOWN))==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK && selected==0);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_ESCAPE))==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK && menu.cancelled && !menu.activated);
  assert(UI_FUNC(End)(&ui)==OK);
  /* Mouse click on the second disabled option does nothing. */
  assert(UI_FUNC(PointerMove)(&ui,(TDUAL_TYPE(uint16)){.x=5,.y=4})==OK);
  assert(UI_FUNC(PointerButton)(&ui,true)==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK && selected==0);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(PointerButton)(&ui,false)==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK && !menu.activated);
  assert(UI_FUNC(End)(&ui)==OK);
  /* Third item is at interior y=5: border/title/two preceding rows. */
  assert(UI_FUNC(PointerMove)(&ui,(TDUAL_TYPE(uint16)){.x=5,.y=5})==OK);
  assert(UI_FUNC(PointerButton)(&ui,true)==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK && selected==2);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(PointerButton)(&ui,false)==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK && menu.activated && selected==2);
  assert(UI_FUNC(End)(&ui)==OK);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
static void test_scrolling_menu(void){
  TERMINAL_TYPE(TCanvas) canvas={0};
  UI_TYPE(TContext) ui={0};
  UI_TYPE(TMenu) menu={0};
  const char *items[]={"Start","Level 1","Level 2","Level 3","Level 4",
                        "Level 5","Level 6","Quit"};
  size_t selected=7;
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
    (TDUAL_TYPE(uint16)){.col=32,.row=10})==OK);
  menu.region=rect(0,0,19,4);menu.items=items;menu.count=8;
  menu.selected=&selected;
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK && menu.firstVisible>0);
  assert(UI_FUNC(End)(&ui)==OK);
  assert(UI_FUNC(Key)(&ui,TERMINAL_CONST(KEY_HOME))==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(MenuEx)(&ui,&menu)==OK && selected==0 && menu.firstVisible==0);
  assert(UI_FUNC(End)(&ui)==OK);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
static void test_progress_bar(void){
  TERMINAL_TYPE(TCanvas) canvas={0};
  UI_TYPE(TContext) ui={0};
  UI_TYPE(TProgressBar) bar={0};
  uint32_t point;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr;
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
    (TDUAL_TYPE(uint16)){.col=40,.row=12})==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(ProgressBar)(&ui,rect(0,0,29,2),25,100)==OK);
  bar.region=rect(0,4,16,8);
  bar.label="LOADING";
  bar.value=50;bar.maximum=100;
  bar.showPercent=true;
  assert(UI_FUNC(ProgressBarEx)(&ui,&bar)==OK);
  bar.region=rect(20,4,25,10);
  bar.label=NULL;bar.vertical=true;bar.indeterminate=true;bar.phase=3;
  assert(UI_FUNC(ProgressBarEx)(&ui,&bar)==OK);
  bar.maximum=0;bar.value=0;bar.indeterminate=false;
  assert(UI_FUNC(ProgressBarEx)(&ui,&bar)==STATUS_CONST(INVALID_ARGUMENT));
  assert(UI_FUNC(ProgressBar)(&ui,rect(0,0,29,2),101,100)==
         STATUS_CONST(INVALID_ARGUMENT));
  assert(UI_FUNC(End)(&ui)==OK);
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
    (TDUAL_TYPE(uint16)){.x=1,.y=1},&point,&attr)==OK);
  assert(point!=0);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
static void test_direct_menu(void){
  TERMINAL_TYPE(TCanvas) canvas={0};
  UI_TYPE(TContext) ui={0};
  const char *choices[]={"PLAY","QUIT"};
  size_t selected=0;
  bool activated=true;
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
    (TDUAL_TYPE(uint16)){.col=22,.row=8})==OK);
  assert(UI_FUNC(Begin)(&ui,&canvas)==OK);
  assert(UI_FUNC(Menu)(&ui,rect(0,0,18,5),choices,2,&selected,&activated)==OK);
  assert(!activated);
  assert(UI_FUNC(End)(&ui)==OK);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
int main(void){
  test_classic_menu();test_scrolling_menu();
  test_progress_bar();test_direct_menu();
  return 0;
}
