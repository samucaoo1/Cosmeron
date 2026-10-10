#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/Font/Font.h"

#define OK STATUS_CONST(SUCCESS)

int main(void) {
  static char source[16384];
  const char *path="../../Cosmeron/Modules/Graphics/Font/Fonts/NibbleBits.flf";
  FILE *file=fopen(path,"rb");
  size_t size;
  FONT_TYPE(TFont) font={0};
  TERMINAL_TYPE(TCanvas) canvas={0};
  TDUAL_TYPE(uint16) measured={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  uint32_t codepoint=0;
  unsigned index, inkCount=0;
  assert(file);
  size=fread(source,1,sizeof(source),file);
  assert(ferror(file)==0);
  assert(feof(file)!=0);
  assert(fclose(file)==0);
  assert(size>1000 && size<sizeof(source));
  assert(FONT_FUNC(ParseFIGlet)(source,size,&font)==OK);
  assert(font.height==7 && font.format==FONT_NS(FORMAT_FIGLET));
  for(index=0;index<95;index++) {
    assert(font.glyphs[index].present);
    assert(font.glyphs[index].columns==8);
  }
  assert(FONT_FUNC(Measure)(&font,"COSMERON",&measured)==OK);
  assert(measured.col==64 && measured.row==7);
  assert(FONT_FUNC(Measure)(&font,"Cosmeron\n2026",&measured)==OK);
  assert(measured.col==64 && measured.row==14);
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
      (TDUAL_TYPE(uint16)){.col=80,.row=20})==OK);
  assert(FONT_FUNC(Draw)(&canvas,
      (TDUAL_TYPE(uint16)){.x=0,.y=0},
      &font,"COSMERON",attr)==OK);
  for(index=0;index<64;index++) {
    unsigned row;
    for(row=0;row<7;row++) {
      assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
          (TDUAL_TYPE(uint16)){.x=(uint16_t)index,.y=(uint16_t)row},
          &codepoint,&attr)==OK);
      assert(codepoint=='0'||codepoint=='1'||codepoint==' ');
      if(codepoint=='0'||codepoint=='1')++inkCount;
    }
  }
  assert(inkCount>100);
  assert(FONT_FUNC(Draw)(&canvas,
      (TDUAL_TYPE(uint16)){.x=20,.y=16},
      &font,"COSMERON",attr)==STATUS_CONST(OUT_OF_RANGE));
  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return 0;
}
