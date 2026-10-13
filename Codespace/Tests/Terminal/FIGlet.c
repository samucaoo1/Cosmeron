#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/FIGlet/FIGlet.h"
#define OK STATUS_CONST(SUCCESS)
static void test_parser(void) {
  char buffer[1024];
  size_t length=0;
  unsigned code;
  FIGLET_TYPE(TFont) font={0};
  TERMINAL_TYPE(TCanvas) canvas={0};
  TDUAL_TYPE(uint16) measured={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell)attr=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  uint32_t character=0;
  int n=snprintf(buffer,sizeof(buffer),"flf2a$ 1 1 1 0 0\n");
  assert(n>0);
  length=(size_t)n;
  for(code=32;code<=126;code++){
    char end=code=='@'?'#':'@';
    n=snprintf(buffer+length,sizeof(buffer)-length,
        "%c%c%c\n",(char)code,end,end);
    assert(n>0&&(size_t)n<sizeof(buffer)-length);
    length+=(size_t)n;
  }
  assert(FIGLET_FUNC(Parse)(buffer,length,&font)==OK);
  assert(FIGLET_FUNC(Measure)(&font,"AB",&measured)==OK);
  assert(measured.col==2 && measured.row==1);
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,
      (TDUAL_TYPE(uint16)){.col=10,.row=3})==OK);
  assert(FIGLET_FUNC(Draw)(&canvas,(TDUAL_TYPE(uint16)){.x=2,.y=1},
      &font,"AB",attr)==OK);
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
      (TDUAL_TYPE(uint16)){.x=2,.y=1},&character,&attr)==OK&&character=='A');
  assert(TERMINAL_FUNC(Canvas_Get)(&canvas,
      (TDUAL_TYPE(uint16)){.x=3,.y=1},&character,&attr)==OK&&character=='B');
  assert(FIGLET_FUNC(Parse)("flf2a$ 2 2 2 0 0\nA@@\n",
      strlen("flf2a$ 2 2 2 0 0\nA@@\n"),&font)==STATUS_CONST(INVALID_SEQUENCE));
  TERMINAL_FUNC(Canvas_Free)(&canvas);
}
int main(void){test_parser();return 0;}
