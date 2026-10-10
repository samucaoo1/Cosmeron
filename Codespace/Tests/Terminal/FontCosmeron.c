#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/FIGlet/FIGlet.h"

#define OK STATUS_CONST(SUCCESS)
static size_t ReadFixture(const char *name, char *buffer, size_t capacity) {
  FILE *file=fopen(name,"rb");
  size_t bytes;
  assert(file);
  bytes=fread(buffer,1,capacity,file);
  assert(!ferror(file));
  assert(feof(file) && bytes>0 && bytes<capacity);
  assert(fclose(file)==0);
  return bytes;
}
static void CheckCosmeron(void) {
  static char source[32768];
  size_t bytes=ReadFixture(
      "../../Asset/FIGlet/Cosmeron.flf",
      source,sizeof(source));
  FIGLET_TYPE(TFont) font={0};
  TERMINAL_TYPE(TCanvas) canvas={0};
  TDUAL_TYPE(uint16) measured={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attribute=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  uint32_t cp=0;
  unsigned index,nonblank=0;
  assert(FIGLET_FUNC(Parse)(source,bytes,&font)==OK);
  assert(font.height==9 && font.data==source);
  for(index=0;index<95;index++){
    assert(font.glyphs[index].columns>0);
    assert(font.glyphs[index].columns==12);
  }
  assert(FIGLET_FUNC(Measure)(&font,"COSMERON",&measured)==OK);
  assert(measured.col==96 && measured.row==9);
  assert(FIGLET_FUNC(Measure)(&font,"COSMER0N\n2026",&measured)==OK);
  assert(measured.col==96 && measured.row==18);
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
      (TDUAL_TYPE(uint16)){.col=112,.row=26})==OK);
  assert(FIGLET_FUNC(Draw)(&canvas,
      (TDUAL_TYPE(uint16)){.x=0,.y=0},
      &font,"COSMERON",attribute)==OK);
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
      (TDUAL_TYPE(uint16)){.x=18,.y=4},
      &cp,&attribute)==OK && cp==0x2022U); /* first O: core dot */
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
      (TDUAL_TYPE(uint16)){.x=78,.y=4},
      &cp,&attribute)==OK && cp==0x2022U); /* second O */
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(FIGLET_FUNC(Draw)(&canvas,
      (TDUAL_TYPE(uint16)){.x=0,.y=0},
      &font,"COSMER0N",attribute)==OK);
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
      (TDUAL_TYPE(uint16)){.x=78,.y=4},
      &cp,&attribute)==OK && cp==0x2502U); /* digit 0: power bar */
  for(index=0;index<96;index++){
    unsigned y;
    for(y=0;y<9;y++){
      assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
          (TDUAL_TYPE(uint16)){.x=(uint16_t)index,.y=(uint16_t)y},
          &cp,&attribute)==OK);
      if(cp!=' ')++nonblank;
    }
  }
  assert(nonblank>85);
  assert(FIGLET_FUNC(Draw)(&canvas,
      (TDUAL_TYPE(uint16)){.x=20,.y=20},
      &font,"COSMERON",attribute)==STATUS_CONST(OUT_OF_RANGE));
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
static void CheckAsciiFallback(void) {
  static char source[32768];
  size_t bytes=ReadFixture(
      "../../Asset/FIGlet/Cosmeron-ASCII.flf",
      source,sizeof(source));
  FIGLET_TYPE(TFont) font={0};
  TERMINAL_TYPE(TCanvas) canvas={0};
  TDUAL_TYPE(uint16) size={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  uint32_t cp=0;
  assert(FIGLET_FUNC(Parse)(source,bytes,&font)==OK);
  assert(font.height==9);
  assert(FIGLET_FUNC(Measure)(&font,"COSMER0N",&size)==OK);
  assert(size.col==96 && size.row==9);
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
      (TDUAL_TYPE(uint16)){.col=96,.row=9})==OK);
  assert(FIGLET_FUNC(Draw)(&canvas,
      (TDUAL_TYPE(uint16)){0},&font,"COSMER0N",style)==OK);
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
      (TDUAL_TYPE(uint16)){.x=78,.y=4},&cp,&style)==OK);
  assert(cp=='|');
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
int main(void) {CheckCosmeron();CheckAsciiFallback();return 0;}
