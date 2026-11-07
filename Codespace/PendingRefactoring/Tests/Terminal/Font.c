#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/Font/Font.h"
#include "../../Cosmeron/Modules/Graphics/Font/Fonts/Readable.asf"
#include "../../Cosmeron/Modules/Graphics/Font/Fonts/VGA8x8.asf"
#define OK STATUS_CONST(SUCCESS)

static uint32_t at(const TERMINAL_TYPE(TCanvas) *c,unsigned x,unsigned y,
                   TEXT_GRID_ATTRIBUTE_TYPE(Cell) *attribute) {
  uint32_t character=0;
  assert(TERMINAL_FUNC(Canvas_Get)(c,(TDUAL_TYPE(uint16)){
      .x=(uint16_t)x,.y=(uint16_t)y},&character,attribute)==OK);
  return character;
}
static void test_static_const_asf(void) {
  TERMINAL_TYPE(TCanvas) canvas={0};
  TDUAL_TYPE(uint16) measured={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  static const FONT_TYPE(TFont) malformed={
      .height=3,
      .glyphs={FONT_GLYPH('A')={.art="A\nAA"}}
  };
  assert(FONT_FUNC(Measure)(&Readable_Font,"AB",&measured)==OK);
  assert(measured.col==10 && measured.row==3);
  assert(FONT_FUNC(Measure)(&Readable_Font,"A B",&measured)==OK);
  assert(measured.col==13 && measured.row==3);
  assert(FONT_FUNC(Measure)(&Readable_Font,"A\nB",&measured)==OK);
  assert(measured.col==5 && measured.row==6);
  assert(FONT_FUNC(Measure)(&malformed,"A",&measured)==STATUS_CONST(INVALID_SEQUENCE));
  assert(FONT_FUNC(Measure)(&Readable_Font,"C",&measured)==STATUS_CONST(NOT_FOUND));
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
      (TDUAL_TYPE(uint16)){.col=20,.row=8})==OK);
  assert(FONT_FUNC(Draw)(&canvas,
      (TDUAL_TYPE(uint16)){.col=1,.row=1},
      &Readable_Font,"AB",style)==OK);
  assert(at(&canvas,3,1,&style)=='A');
  assert(at(&canvas,6,1,&style)=='B');
  assert(style.foreground==TEXT_GRID_COLOR_RGB(255,40,30));
  assert((style.flags&TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND))==0);
  assert(at(&canvas,6,2,&style)=='B');
  assert((style.flags&TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND))!=0);
  assert(FONT_FUNC(Draw)(&canvas,
      (TDUAL_TYPE(uint16)){.col=17,.row=1},
      &Readable_Font,"AB",style)==STATUS_CONST(OUT_OF_RANGE));
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}


static void test_vga8x8_public_domain(void) {
  TERMINAL_TYPE(TCanvas) cv={0};
  TDUAL_TYPE(uint16) measured={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  char printable[96];
  unsigned i;
  for(i=0;i<95;i++) printable[i]=(char)(32U+i);
  printable[95]=0;
  assert(strcmp(VGA8x8_Font.name,"VGA 8x8 Basic")==0);
  assert(strcmp(VGA8x8_Font.license,"Public Domain")==0);
  assert(FONT_FUNC(Measure)(&VGA8x8_Font,"AB",&measured)==OK);
  assert(measured.col==17 && measured.row==8);
  assert(FONT_FUNC(Measure)(&VGA8x8_Font,"A\nB",&measured)==OK);
  assert(measured.col==8 && measured.row==17);
  assert(FONT_FUNC(Measure)(&VGA8x8_Font,printable,&measured)==OK);
  assert(measured.col==854 && measured.row==8);
  assert(TERMINAL_FUNC(Canvas_Create)(&cv,
      (TDUAL_TYPE(uint16)){.col=40,.row=20})==OK);
  assert(FONT_FUNC(Draw)(&cv,(TDUAL_TYPE(uint16)){.col=1,.row=1},
      &VGA8x8_Font,"AB",attr)==OK);
  assert(at(&cv,3,1,&attr)=='#'); /* A first row is 0x0C */
  assert(at(&cv,10,1,&attr)=='#'); /* B starts after 8 columns + 1 gap */
  assert(at(&cv,9,1,&attr)==' ');  /* inter-glyph gap */
  TERMINAL_FUNC(Canvas_Free)(&cv);
}

int main(void){test_static_const_asf();test_vga8x8_public_domain();return 0;}
