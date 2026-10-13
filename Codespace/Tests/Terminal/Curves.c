#include <assert.h>
#include "../../Cosmeron/Modules/Graphics/Rasterization/Rasterization.h"
static TDUAL_TYPE(int32) p(int32_t x,int32_t y) {return (TDUAL_TYPE(int32)){.x=x,.y=y};}
static uint32_t at(TERMINAL_TYPE(TCanvas) *c,unsigned x,unsigned y) {return c->backing->chars.data[y*c->size.x+x];}
int main(void) {
  TERMINAL_TYPE(TCanvas) c={0};TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  unsigned x,y;
  assert(TERMINAL_FUNC(Canvas_Create)(&c,(TDUAL_TYPE(uint16)){.x=17,.y=17})==STATUS_CONST(SUCCESS));
  assert(RASTER_FUNC(Arc)(&c,p(8,8),(TDUAL_TYPE(uint32)){.x=6,.y=6},p(1,0),p(0,1),true,'#',style)==STATUS_CONST(SUCCESS));
  assert(at(&c,14,8)=='#' && at(&c,8,14)=='#' && at(&c,2,8)==' ' && at(&c,8,2)==' ');
  TERMINAL_FUNC(Canvas_Clear)(&c);
  assert(RASTER_FUNC(Arc)(&c,p(8,8),(TDUAL_TYPE(uint32)){.x=6,.y=6},p(1,0),p(0,1),false,'#',style)==STATUS_CONST(SUCCESS));
  assert(at(&c,2,8)=='#' && at(&c,8,2)=='#' && at(&c,12,12)==' ');
  assert(RASTER_FUNC(Arc)(&c,p(8,8),(TDUAL_TYPE(uint32)){.x=6,.y=6},p(0,0),p(0,1),true,'#',style)==STATUS_CONST(INVALID_ARGUMENT));
  TERMINAL_FUNC(Canvas_Clear)(&c);
  assert(RASTER_FUNC(BezierQuadratic)(&c,p(0,0),p(8,16),p(16,0),'#',style)==STATUS_CONST(SUCCESS));
  assert(at(&c,0,0)=='#' && at(&c,8,8)=='#' && at(&c,16,0)=='#');
  for(y=9;y<17;y++) for(x=0;x<17;x++) assert(at(&c,x,y)==' ');
  TERMINAL_FUNC(Canvas_Clear)(&c);
  assert(RASTER_FUNC(BezierCubic)(&c,p(0,0),p(0,16),p(16,16),p(16,0),'#',style)==STATUS_CONST(SUCCESS));
  assert(at(&c,0,0)=='#' && at(&c,8,12)=='#' && at(&c,16,0)=='#');
  TERMINAL_FUNC(Canvas_Clear)(&c);
  assert(RASTER_FUNC(BezierCubic)(&c,p(8,8),p(8,8),p(8,8),p(8,8),'#',style)==STATUS_CONST(SUCCESS));
  assert(at(&c,8,8)=='#');
  assert(RASTER_FUNC(BezierQuadratic)(&c,p(INT32_MIN,0),p(0,16),p(INT32_MAX,0),'#',style)==STATUS_CONST(SUCCESS));
  assert(RASTER_FUNC(BezierCubic)(&c,p(INT32_MIN,INT32_MIN),p(INT32_MAX,INT32_MIN),p(INT32_MIN,INT32_MAX),p(INT32_MAX,INT32_MAX),'#',style)==STATUS_CONST(SUCCESS));
  TERMINAL_FUNC(Canvas_Free)(&c);return 0;
}
