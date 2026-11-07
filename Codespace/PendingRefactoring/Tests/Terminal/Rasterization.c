#include <assert.h>
#include <limits.h>
#include "../../Cosmeron/Modules/Graphics/Rasterization/Rasterization.h"

static TDUAL_TYPE(int32) point(int32_t x,int32_t y) {
  return (TDUAL_TYPE(int32)){.x=x,.y=y};
}
static uint32_t cell(TERMINAL_TYPE(TCanvas) *canvas,uint16_t x,uint16_t y) {
  uint32_t cp;TEXT_GRID_ATTRIBUTE_TYPE(Cell) style;
  assert(TERMINAL_FUNC(Canvas_Get)(canvas,
      (TDUAL_TYPE(uint16)){.x=x,.y=y},&cp,&style)==STATUS_CONST(SUCCESS));
  return cp;
}
int main(void) {
  TERMINAL_TYPE(TCanvas) canvas={0},reverse={0},view={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  TDUAL_TYPE(int32) polygon[6]={{.x=1,.y=1},{.x=7,.y=1},
      {.x=7,.y=3},{.x=3,.y=3},{.x=3,.y=7},{.x=1,.y=7}};
  int x,y,dx,dy;
  style.foreground=0x123456;
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,(TDUAL_TYPE(uint16)){.x=9,.y=9})==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Create)(&reverse,(TDUAL_TYPE(uint16)){.x=9,.y=9})==STATUS_CONST(SUCCESS));
  assert(RASTER_FUNC(Point)(NULL,point(0,0),'#',style)==STATUS_CONST(INVALID_ARGUMENT));
  assert(RASTER_FUNC(Point)(&canvas,point(-1,-1),'#',style)==STATUS_CONST(SUCCESS));
  assert(RASTER_FUNC(Point)(&canvas,point(0,0),0x4e00,style)==STATUS_CONST(NOT_SUPPORTED));
  assert(RASTER_FUNC(Point)(&canvas,point(0,0),0x0301,style)==STATUS_CONST(NOT_SUPPORTED));
  assert(RASTER_FUNC(Point)(&canvas,point(0,0),0x110000,style)!=STATUS_CONST(SUCCESS));
  /* All octants, reversal invariance, endpoints and one cell per major step. */
  for (dx=-4;dx<=4;dx++) for (dy=-4;dy<=4;dy++) {
    int total=0,major=abs(dx)>abs(dy) ? abs(dx) : abs(dy);
    TERMINAL_FUNC(Canvas_Clear)(&canvas);TERMINAL_FUNC(Canvas_Clear)(&reverse);
    assert(RASTER_FUNC(Line)(&canvas,point(4,4),point(4+dx,4+dy),'#',style)==STATUS_CONST(SUCCESS));
    assert(RASTER_FUNC(Line)(&reverse,point(4+dx,4+dy),point(4,4),'#',style)==STATUS_CONST(SUCCESS));
    assert(cell(&canvas,4,4)=='#' && cell(&canvas,(uint16_t)(4+dx),(uint16_t)(4+dy))=='#');
    for (y=0;y<9;y++) for (x=0;x<9;x++) {
      assert(cell(&canvas,(uint16_t)x,(uint16_t)y)==cell(&reverse,(uint16_t)x,(uint16_t)y));
      total+=cell(&canvas,(uint16_t)x,(uint16_t)y)=='#';
    }
    assert(total==major+1);
  }
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(RASTER_FUNC(Line)(&canvas,point(INT32_MIN,INT32_MIN),point(INT32_MAX,INT32_MAX),'#',style)==STATUS_CONST(SUCCESS));
  for(y=0;y<9;y++) for(x=0;x<9;x++) assert((cell(&canvas,(uint16_t)x,(uint16_t)y)=='#')==(x==y));
  assert(RASTER_FUNC(Line)(&canvas,point(INT32_MIN,INT32_MAX),point(INT32_MAX,INT32_MIN),'#',style)==STATUS_CONST(SUCCESS));
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(RASTER_FUNC(Rectangle)(&canvas,(TQUAD_TYPE(uint16)){.left=2,.top=2,.right=12,.bottom=12},'#',style)==STATUS_CONST(SUCCESS));
  assert(cell(&canvas,8,8)==' ' && cell(&canvas,2,8)=='#');
  assert(RASTER_FUNC(FillRectangle)(&canvas,(TQUAD_TYPE(uint16)){.left=2,.top=2,.right=12,.bottom=12},'#',style)==STATUS_CONST(SUCCESS));
  assert(cell(&canvas,8,8)=='#');
  assert(RASTER_FUNC(Rectangle)(&canvas,(TQUAD_TYPE(uint16)){.left=2,.right=1},'#',style)==STATUS_CONST(INVALID_ARGUMENT));
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(RASTER_FUNC(FillCircle)(&canvas,point(4,4),3,'#',style)==STATUS_CONST(SUCCESS));
  for(y=0;y<9;y++) for(x=0;x<9;x++) assert((cell(&canvas,(uint16_t)x,(uint16_t)y)=='#')==((x-4)*(x-4)+(y-4)*(y-4)<=9));
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(RASTER_FUNC(Circle)(&canvas,point(4,4),3,'#',style)==STATUS_CONST(SUCCESS));
  assert(cell(&canvas,4,4)==' ' && cell(&canvas,4,1)=='#' && cell(&canvas,7,4)=='#');
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(RASTER_FUNC(FillEllipse)(&canvas,point(4,4),(TDUAL_TYPE(uint32)){.x=4,.y=2},'#',style)==STATUS_CONST(SUCCESS));
  for(y=0;y<9;y++) for(x=0;x<9;x++) assert((cell(&canvas,(uint16_t)x,(uint16_t)y)=='#')==(4*(x-4)*(x-4)+16*(y-4)*(y-4)<=64));
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(RASTER_FUNC(Ellipse)(&canvas,point(4,4),(TDUAL_TYPE(uint32)){.x=0,.y=3},'#',style)==STATUS_CONST(SUCCESS));
  assert(cell(&canvas,4,1)=='#' && cell(&canvas,4,7)=='#' && cell(&canvas,3,4)==' ');
  assert(RASTER_FUNC(Ellipse)(&canvas,point(4,4),(TDUAL_TYPE(uint32)){.x=3,.y=0},'#',style)==STATUS_CONST(SUCCESS));
  assert(cell(&canvas,1,4)=='#' && cell(&canvas,7,4)=='#');
  assert(RASTER_FUNC(Circle)(&canvas,point(0,0),0,'#',style)==STATUS_CONST(SUCCESS));
  assert(cell(&canvas,0,0)=='#');
  assert(RASTER_FUNC(Circle)(&canvas,point(INT32_MIN,INT32_MIN),UINT32_MAX,'#',style)==STATUS_CONST(SUCCESS));
  assert(RASTER_FUNC(FillCircle)(&canvas,point(INT32_MAX,INT32_MAX),UINT32_MAX,'#',style)==STATUS_CONST(SUCCESS));
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(RASTER_FUNC(FillPolygon)(&canvas,polygon,6,'#',style)==STATUS_CONST(SUCCESS));
  assert(cell(&canvas,2,6)=='#' && cell(&canvas,6,2)=='#' && cell(&canvas,6,6)==' ');
  assert(RASTER_FUNC(Polyline)(&canvas,polygon,1,'#',style)==STATUS_CONST(SUCCESS));
  assert(RASTER_FUNC(Polygon)(&canvas,NULL,3,'#',style)==STATUS_CONST(INVALID_ARGUMENT));
  assert(RASTER_FUNC(FillPolygon)(&canvas,polygon,0,'#',style)==STATUS_CONST(INVALID_ARGUMENT));
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(RASTER_FUNC(FillTriangle)(&canvas,point(1,1),point(7,1),point(1,7),'#',style)==STATUS_CONST(SUCCESS));
  assert(cell(&canvas,2,2)=='#' && cell(&canvas,6,6)==' ');
  assert(RASTER_FUNC(Triangle)(&canvas,point(8,8),point(8,8),point(8,8),'#',style)==STATUS_CONST(SUCCESS));
  assert(cell(&canvas,8,8)=='#');
  /* A view never modifies outside cells, even when a wide glyph straddles it. */
  TERMINAL_FUNC(Canvas_Clear)(&canvas);
  assert(TERMINAL_FUNC(Canvas_Set)(&canvas,(TDUAL_TYPE(uint16)){.x=1,.y=3},0x4e00,style)==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Set)(&canvas,(TDUAL_TYPE(uint16)){.x=5,.y=3},0x4e00,style)==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_View)(&canvas,(TQUAD_TYPE(uint16)){.left=2,.top=2,.right=5,.bottom=5},&view)==STATUS_CONST(SUCCESS));
  assert(RASTER_FUNC(FillRectangle)(&view,(TQUAD_TYPE(uint16)){.left=0,.top=0,.right=8,.bottom=8},'#',style)==STATUS_CONST(SUCCESS));
  assert(cell(&canvas,1,3)==0x4e00 && cell(&canvas,2,3)==0 && cell(&canvas,5,3)==0x4e00 && cell(&canvas,6,3)==0);
  assert(cell(&canvas,3,3)=='#' && cell(&canvas,6,4)==' ');
  assert(canvas.backing->attributes.data[3*9+3].foreground==style.foreground);
  assert(canvas.cursor.x==0 && canvas.cursor.y==0);
  TERMINAL_FUNC(Canvas_Free)(&view);
  TERMINAL_FUNC(Canvas_Free)(&reverse);TERMINAL_FUNC(Canvas_Free)(&canvas);
  return 0;
}
