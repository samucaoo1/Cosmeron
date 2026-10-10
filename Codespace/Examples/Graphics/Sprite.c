#include "../../Cosmeron/Modules/Graphics/Sprite/Sprite.h"
int main(void) {
  TERMINAL_TYPE(TCanvas) canvas={0},atlas={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  SPRITE_TYPE(TLayer) layer;
  SPRITE_TYPE(TFrame) frames[2];
  SPRITE_TYPE(TAnimation) animation;
  /* Milliseconds supplied by the application (e.g. using Chronometry). */
  const uint64_t elapsed=350;
  OPSTATUS status=TERMINAL_FUNC(Canvas_Create)(&canvas,(TDUAL_TYPE(uint16)){.x=40,.y=12});
  if (status!=STATUS_CONST(SUCCESS)) return 1;
  status=TERMINAL_FUNC(Canvas_Create)(&atlas,(TDUAL_TYPE(uint16)){.x=6,.y=3});
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  status=RASTER_FUNC(Circle)(&atlas,(TDUAL_TYPE(int32)){.x=1,.y=1},1,'O',style);
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  status=RASTER_FUNC(FillCircle)(&atlas,(TDUAL_TYPE(int32)){.x=4,.y=1},1,'@',style);
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  frames[0]=(SPRITE_TYPE(TFrame)){.source=&atlas,.region={.left=0,.top=0,.right=2,.bottom=2},.duration=200};
  frames[1]=(SPRITE_TYPE(TFrame)){.source=&atlas,.region={.left=3,.top=0,.right=5,.bottom=2},.duration=200};
  animation=(SPRITE_TYPE(TAnimation)){.frames=frames,.count=2,.loop=true};
  layer=SPRITE_FUNC(Layer)(&atlas);
  layer.position=(TDUAL_TYPE(int32)){.x=12,.y=3};layer.size=(TDUAL_TYPE(uint16)){.x=12,.y=6};
  layer.useColorKey=true;layer.colorKey=' ';
  status=SPRITE_FUNC(DrawFrame)(&canvas,&animation,elapsed,&layer);
  if (status==STATUS_CONST(SUCCESS)) status=TERMINAL_FUNC(Canvas_Update)(&canvas);
cleanup:
  TERMINAL_FUNC(Canvas_Free)(&atlas);TERMINAL_FUNC(Canvas_Free)(&canvas);
  return status==STATUS_CONST(SUCCESS) ? 0 : 1;
}
