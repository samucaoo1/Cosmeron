#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "../../Cosmeron/Modules/Graphics/FIGlet/FIGlet.h"

#define OK STATUS_CONST(SUCCESS)

static void Verify(const char *filename,bool legacy){
  static char buffer[32768];
  FIGLET_TYPE(TFont) font={0};
  TERMINAL_TYPE(TCanvas) canvas={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  TDUAL_TYPE(uint16) dims={0};
  uint32_t codepoint=0;
  size_t size;
  unsigned glyph;
  FILE *file=fopen(filename,"rb");
  assert(file);
  size=fread(buffer,1,sizeof(buffer),file);
  assert(!ferror(file) && feof(file) && size>1000 && size<sizeof(buffer));
  assert(fclose(file)==0);
  assert(FIGLET_FUNC(Parse)(buffer,size,&font)==OK);
  assert(font.height==9);
  for(glyph=0;glyph<95;glyph++){
    assert(font.glyphs[glyph].columns>0);
    assert(font.glyphs[glyph].columns==12);
  }
  assert(FIGLET_FUNC(Measure)(&font,"COSMER0N",&dims)==OK);
  assert(dims.col==96 && dims.row==9);
  assert(FIGLET_FUNC(Measure)(&font,"COSMER0N\nBOLD",&dims)==OK);
  assert(dims.col==96 && dims.row==18);
  assert(TERMINAL_FUNC(Canvas_Create)(
      &canvas,(TDUAL_TYPE(uint16)){.col=100,.row=20})==OK);
  assert(FIGLET_FUNC(Draw)(
      &canvas,(TDUAL_TYPE(uint16)){.x=0,.y=0},
      &font,"COSMERON",style)==OK);
  assert(TERMINAL_FUNC(Canvas_Get)(
      &canvas,(TDUAL_TYPE(uint16)){.x=18,.y=4},
      &codepoint,&style)==OK);
  assert(codepoint==(legacy?'@':UINT32_C(0x25CF)));
  assert(TERMINAL_FUNC(Canvas_Get)(
      &canvas,(TDUAL_TYPE(uint16)){.x=4,.y=1},
      &codepoint,&style)==OK);
  assert(codepoint==(legacy?'=':UINT32_C(0x2501)));
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(FIGLET_FUNC(Draw)(
      &canvas,(TDUAL_TYPE(uint16)){.x=0,.y=0},
      &font,"COSMER0N",style)==OK);
  assert(TERMINAL_FUNC(Canvas_Get)(
      &canvas,(TDUAL_TYPE(uint16)){.x=78,.y=4},
      &codepoint,&style)==OK);
  assert(codepoint==(legacy?'#':UINT32_C(0x2503)));
  assert(FIGLET_FUNC(Draw)(
      &canvas,(TDUAL_TYPE(uint16)){.x=25,.y=13},
      &font,"COSMER0N",style)==STATUS_CONST(OUT_OF_RANGE));
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
int main(void){
  Verify("../../Asset/FIGlet/Cosmeron-Bold.flf",false);
  Verify("../../Asset/FIGlet/Cosmeron-Bold-ASCII.flf",true);
  return 0;
}
