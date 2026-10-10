#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/Font/Font.h"
#include "Fixtures/Retro.asf.h"
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
static void test_asf_embedded(void) {
  FONT_TYPE(TFont) font={0},invalid={0};
  TERMINAL_TYPE(TCanvas) c={0};
  TDUAL_TYPE(uint16) measure={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  assert(FONT_FUNC(ParseASF)((const char *)RetroTestFont_Data,
         sizeof(RetroTestFont_Data),&font)==OK);
  assert(font.height==3);
  assert(FONT_FUNC(Measure)(&font,"AB",&measure)==OK);
  assert(measure.col==10&&measure.row==3);
  assert(FONT_FUNC(Measure)(&font,"A\nB",&measure)==OK);
  assert(measure.col==5&&measure.row==6);
  assert(TERMINAL_FUNC(Canvas_Create)(&c,(TDUAL_TYPE(uint16)){
    .col=18,.row=8})==OK);
  assert(FONT_FUNC(Draw)(&c,(TDUAL_TYPE(uint16)){.x=1,.y=1},
         &font,"AB",attr)==OK);
  assert(at(&c,3,1,&attr)=='A');
  assert(at(&c,6,1,&attr)=='B');
  assert(attr.foreground==TEXT_GRID_COLOR_RGB(255,40,30));
  assert((attr.flags&TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND))==0);
  assert(at(&c,6,2,&attr)=='B');
  assert((attr.flags&TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND))!=0);
  assert(FONT_FUNC(Draw)(&c,(TDUAL_TYPE(uint16)){.x=15,.y=1},
         &font,"AB",attr)==STATUS_CONST(OUT_OF_RANGE));
  assert(FONT_FUNC(Measure)(&font,"C",&measure)==STATUS_CONST(NOT_FOUND));
  assert(FONT_FUNC(Measure)(&font,"\xC3\xA9",&measure)==
         STATUS_CONST(NOT_SUPPORTED));
  assert(FONT_FUNC(ParseASF)("ASF1 2\n@65\nAA\nB\n",
         strlen("ASF1 2\n@65\nAA\nB\n"),&invalid)==STATUS_CONST(INVALID_SEQUENCE));
  assert(FONT_FUNC(ParseASF)("ASF1 1\n@65\n\x1b[2J\n",
         strlen("ASF1 1\n@65\n\x1b[2J\n"),&invalid)!=OK);
  TERMINAL_FUNC(Canvas_Free)(&c);
}
static void test_figlet(void) {
  char flf[1024];
  size_t total=0;
  unsigned code;
  FONT_TYPE(TFont) font={0};
  TERMINAL_TYPE(TCanvas) c={0};
  TDUAL_TYPE(uint16) measure={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  int n=snprintf(flf,sizeof(flf),"flf2a$ 1 1 1 0 0\n");
  assert(n>0);
  total=(size_t)n;
  for(code=32;code<=126;code++){
    char marker=code=='@'?'#':'@';
    n=snprintf(flf+total,sizeof(flf)-total,"%c%c%c\n",
         (char)code,marker,marker);
    assert(n>0 && (size_t)n<sizeof(flf)-total);
    total+=(size_t)n;
  }
  assert(FONT_FUNC(ParseFIGlet)(flf,total,&font)==OK);
  assert(FONT_FUNC(Measure)(&font,"AB",&measure)==OK);
  assert(measure.col==2&&measure.row==1);
  assert(TERMINAL_FUNC(Canvas_Create)(&c,(TDUAL_TYPE(uint16)){
      .col=10,.row=3})==OK);
  assert(FONT_FUNC(Draw)(&c,(TDUAL_TYPE(uint16)){.x=2,.y=1},
         &font,"AB",attr)==OK);
  assert(at(&c,2,1,&attr)=='A');
  assert(at(&c,3,1,&attr)=='B');
  assert(FONT_FUNC(ParseFIGlet)("flf2a$ 2 2 2 0 0\nA@@\n",
      strlen("flf2a$ 2 2 2 0 0\nA@@\n"),&font)==STATUS_CONST(INVALID_SEQUENCE));
  TERMINAL_FUNC(Canvas_Free)(&c);
}

static void test_asf2_readable(void) {
  const char source[] =
    "ASF2 3\n"
    "# readable glyphs, no escapes inside the artwork\n"
    "@A\n"
    "  A\n"
    " A A\n"
    "AAAAA\n"
    "@B\n"
    "BBBBB\n"
    "B   B\n"
    "BBBBB\n"
    "!fg 0 #FF281E\n";
  FONT_TYPE(TFont) font={0};
  TERMINAL_TYPE(TCanvas) canvas={0};
  TDUAL_TYPE(uint16) size={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  assert(FONT_FUNC(ParseASF2)(source,sizeof(source)-1U,&font)==OK);
  assert(FONT_FUNC(Measure)(&font,"AB",&size)==OK);
  assert(size.col==10 && size.row==3);
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
      (TDUAL_TYPE(uint16)){.col=20,.row=5})==OK);
  assert(FONT_FUNC(Draw)(&canvas,(TDUAL_TYPE(uint16)){.x=1,.y=1},
      &font,"AB",style)==OK);
  assert(at(&canvas,6,1,&style)=='B');
  assert(style.foreground==TEXT_GRID_COLOR_RGB(255,40,30));
  assert(at(&canvas,6,2,&style)=='B');
  assert((style.flags&TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND))!=0);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}


static void test_static_const_asf(void) {
  TERMINAL_TYPE(TCanvas) canvas={0};
  TDUAL_TYPE(uint16) measured={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  FONT_TYPE(TFont) bad={0};
  static const FONT_TYPE(TFont) malformed={
      .format=FONT_NS(FORMAT_STATIC),
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
  (void)bad;
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

int main(void) { test_asf_embedded();test_figlet();test_asf2_readable();test_static_const_asf();test_vga8x8_public_domain();return 0; }
