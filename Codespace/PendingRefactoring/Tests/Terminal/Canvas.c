#include <assert.h>
#include <stdint.h>
#include "../../Cosmeron/Modules/Graphics/Canvas/Canvas.h"

#define OK STATUS_CONST(SUCCESS)
static TDUAL_TYPE(uint16) at(uint16_t x,uint16_t y) {
  TDUAL_TYPE(uint16) p={0};p.x=x;p.y=y;return p;
}
static TQUAD_TYPE(uint16) rect(uint16_t l,uint16_t t,uint16_t r,uint16_t b) {
  TQUAD_TYPE(uint16) q={0};q.left=l;q.right=r;q.top=t;q.bottom=b;return q;
}
static uint32_t character(const TERMINAL_TYPE(TCanvas) *canvas,uint16_t x,uint16_t y) {
  uint32_t codepoint=0;TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr;
  assert(TERMINAL_FUNC(Canvas_Get)(canvas,at(x,y),&codepoint,&attr)==OK);
  return codepoint;
}
static void test_views(void) {
  TERMINAL_TYPE(TCanvas) root={0}, view={0}, child={0}, copy={0},crop={0};
  TDUAL_TYPE(uint16) size={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  assert(TERMINAL_FUNC(Canvas_Create)(&root,at(8,4))==OK);
  assert(TERMINAL_FUNC(Canvas_GetSize)(&root,&size)==OK && size.x==8 && size.y==4);
  assert(TERMINAL_FUNC(Canvas_View)(&root,rect(1,1,5,3),&view)==OK);
  assert(TERMINAL_FUNC(Canvas_View)(&view,rect(1,0,2,1),&child)==OK);
  assert(TERMINAL_FUNC(Canvas_Set)(&child,at(0,0),'Q',style)==OK);
  assert(character(&root,2,1)=='Q');
  assert(TERMINAL_FUNC(Canvas_Get)(&child,at(4,0),&(uint32_t){0},&style)==STATUS_CONST(OUT_OF_RANGE));
  assert(TERMINAL_FUNC(Canvas_Copy)(&child,&copy)==OK);
  assert(TERMINAL_FUNC(Canvas_Crop)(&root,rect(1,1,3,2),&crop)==OK);
  assert(character(&copy,0,0)=='Q');
  assert(character(&crop,1,0)=='Q');
  assert(TERMINAL_FUNC(Canvas_Set)(&child,at(0,0),'R',style)==OK);
  assert(character(&root,2,1)=='R' && character(&copy,0,0)=='Q');
  assert(TERMINAL_FUNC(Canvas_FillRegion)(&view,rect(0,0,1,0),'F',style)==OK);
  assert(character(&root,1,1)=='F' && character(&root,2,1)=='F');
  TERMINAL_FUNC(Canvas_Free)(&child);
  TERMINAL_FUNC(Canvas_Free)(&child);
  TERMINAL_FUNC(Canvas_Free)(&view);
  TERMINAL_FUNC(Canvas_Free)(&crop);
  TERMINAL_FUNC(Canvas_Free)(&copy);
  assert(TERMINAL_FUNC(Canvas_Resize)(&root,at(10,5))==OK);
  assert(TERMINAL_FUNC(Canvas_GetSize)(&root,&size)==OK && size.x==10);
  TERMINAL_FUNC(Canvas_Free)(&root);
}
static void test_text(void) {
  TERMINAL_TYPE(TCanvas) canvas={0};
  TDUAL_TYPE(uint16) saved;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr;
  uint32_t point=0;
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,at(10,4))==OK);
  assert(TERMINAL_FUNC(Canvas_Print)(&canvas,"Hello %u",42u)==OK);
  assert(character(&canvas,0,0)=='H' && character(&canvas,6,0)=='4');
  assert(TERMINAL_FUNC(Canvas_Cursor_GetPosition)(&canvas,&saved)==OK);
  assert(saved.x==8 && saved.y==0);
  style.foreground=TEXT_GRID_COLOR_RGB(255,16,8);
  style.flags &= (uint16_t)~TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND);
  assert(TERMINAL_FUNC(Canvas_PrintStyledAt)(&canvas,at(1,1),style,"%s","YES")==OK);
  assert(character(&canvas,1,1)=='Y');
  assert(TERMINAL_FUNC(Canvas_Cursor_GetPosition)(&canvas,&saved)==OK && saved.x==8 && saved.y==0);
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,at(1,1),&point,&attr)==OK);
  assert(attr.foreground==style.foreground);
  assert(TERMINAL_FUNC(Canvas_PrintAt)(&canvas,at(9,3),"wide") == STATUS_CONST(OUT_OF_RANGE));
  assert(TERMINAL_FUNC(Canvas_Cursor_GetPosition)(&canvas,&saved)==OK && saved.x==8);
  assert(TERMINAL_FUNC(Canvas_Set)(&canvas,at(2,2),0x4E2D,style)==OK);
  assert(character(&canvas,2,2)==0x4E2D);
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,at(3,2),&point,&attr)==OK);
  assert(attr.flags & TEXT_GRID_ATTRIBUTE_CONST(CODE_UNIT_CONTINUATION));
  assert(TERMINAL_FUNC(Canvas_Set)(&canvas,at(3,2),'!',style)==OK);
  assert(character(&canvas,2,2)==' ' && character(&canvas,3,2)=='!');
  assert(TERMINAL_FUNC(Canvas_Set)(&canvas,at(9,2),0x4E2D,style)==STATUS_CONST(OUT_OF_RANGE));
  assert(TERMINAL_FUNC(Canvas_Write)(&canvas,"\xFF",1)==STATUS_CONST(INVALID_SEQUENCE));
  assert(TERMINAL_FUNC(Canvas_PutChar)(&canvas,0x0301)==STATUS_CONST(NOT_SUPPORTED));
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(character(&canvas,1,1)==' ');
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
static void test_blit(void) {
  TERMINAL_TYPE(TCanvas) dst={0},source={0},view={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) src=attr;
  assert(TERMINAL_FUNC(Canvas_Create)(&dst,at(5,2))==OK);
  assert(TERMINAL_FUNC(Canvas_Create)(&source,at(3,1))==OK);
  assert(TERMINAL_FUNC(Canvas_Print)(&source,"ABC")==OK);
  assert(TERMINAL_FUNC(Canvas_Blit)(&dst,at(1,0),&source)==OK);
  assert(character(&dst,1,0)=='A' && character(&dst,3,0)=='C');
  assert(TERMINAL_FUNC(Canvas_View)(&dst,rect(1,0,3,0),&view)==OK);
  assert(TERMINAL_FUNC(Canvas_Blit)(&dst,at(2,0),&view)==OK);
  assert(character(&dst,0,0)==' ' && character(&dst,2,0)=='A' &&
         character(&dst,3,0)=='B' && character(&dst,4,0)=='C');
  src.background=TEXT_GRID_COLOR_RGB(30,90,50);
  src.flags=(uint16_t)(TEXT_GRID_ATTRIBUTE_CONST(TRANSPARENT_BACKGROUND));
  attr.background=TEXT_GRID_COLOR_RGB(10,20,30);
  attr.flags &= (uint16_t)~TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_BACKGROUND);
  assert(TERMINAL_FUNC(Canvas_Set)(&dst,at(0,1),'X',attr)==OK);
  assert(TERMINAL_FUNC(Canvas_Set)(&source,at(0,0),'Y',src)==OK);
  assert(TERMINAL_FUNC(Canvas_BlitRegion)(&dst,at(0,1),&source,rect(0,0,0,0))==OK);
  assert(TERMINAL_FUNC(Canvas_Get)(&dst,at(0,1),&(uint32_t){0},&attr)==OK);
  assert(attr.background==TEXT_GRID_COLOR_RGB(10,20,30));
  TERMINAL_FUNC(Canvas_Free)(&view);
  TERMINAL_FUNC(Canvas_Free)(&dst);
  TERMINAL_FUNC(Canvas_Free)(&source);
}
static void test_resize_wide(void) {
  TERMINAL_TYPE(TCanvas) canvas={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  uint32_t glyph=0;
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,at(3,1))==OK);
  assert(TERMINAL_FUNC(Canvas_Set)(&canvas,at(1,0),0x4E2D,attr)==OK);
  assert(TERMINAL_FUNC(Canvas_Resize)(&canvas,at(2,1))==OK);
  assert(character(&canvas,1,0)==' ');
  assert(TERMINAL_FUNC(Canvas_Resize)(&canvas,at(4,2))==OK);
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,at(3,1),&glyph,&attr)==OK);
  assert(glyph==' ' && (attr.flags&TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND)));
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}

static void test_update_capability(void) {
  TERMINAL_TYPE(TCanvas) canvas={0};
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,at(2,1))==OK);
  if(!TERMINAL_INS(Interactive)())
    assert(TERMINAL_FUNC(Canvas_Update)(&canvas)==STATUS_CONST(NOT_AVAILABLE));
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
int main(void) {test_views();test_text();test_blit();test_resize_wide();test_update_capability();return 0;}
