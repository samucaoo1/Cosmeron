#include "../../Cosmeron/Modules/Graphics/Raycasting/Render.h"

int main(void) {
  const uint16_t cells[64]={
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,1,0,0,1,
    1,0,0,0,1,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1};
  TERMINAL_TYPE(TCanvas) canvas={0},texture={0},objectImage={0};
  const TERMINAL_TYPE(TCanvas) *textures[1]={&texture};
  RAYCAST_TYPE(TSprite) object={0};
  float depth[80];
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) base=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  RAYCAST_TYPE(TScene) scene={
    .map={.cells=cells,.size={.x=8,.y=8},.stride=8,.cellCount=64},
    .wallCodepoint=0x2588,.ceilingCodepoint=' ',.floorCodepoint='.'};
  RAYCAST_TYPE(TCamera) camera={
    .position={.x=2.5f,.y=2.5f},.direction={.x=1,.y=0},.plane={.x=0,.y=0.66f},
    .maxDistance=24,.wallHeight=1,.horizon=0.5f};
  OPSTATUS status;
  scene.wallStyle=scene.sideStyle=scene.ceilingStyle=scene.floorStyle=base;
  scene.wallStyle.flags &= (uint16_t)~TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND);
  scene.sideStyle.flags &= (uint16_t)~TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND);
  scene.wallStyle.foreground=0xD0AA70;scene.sideStyle.foreground=0x806040;
  status=TERMINAL_FUNC(Canvas_Create)(&canvas,(TDUAL_TYPE(uint16)){.x=80,.y=24});
  if (status!=STATUS_CONST(SUCCESS)) return 1;
  status=TERMINAL_FUNC(Canvas_Create)(&texture,(TDUAL_TYPE(uint16)){.x=4,.y=4});
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  status=TERMINAL_FUNC(Canvas_Create)(&objectImage,(TDUAL_TYPE(uint16)){.x=3,.y=3});
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  status=TERMINAL_FUNC(Canvas_Fill)(&texture,'#',scene.wallStyle);
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  status=RASTER_FUNC(Line)(&texture,(TDUAL_TYPE(int32)){.x=0,.y=2},
      (TDUAL_TYPE(int32)){.x=3,.y=2},'-',scene.sideStyle);
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  status=RASTER_FUNC(Line)(&objectImage,(TDUAL_TYPE(int32)){.x=1,.y=0},
      (TDUAL_TYPE(int32)){.x=1,.y=2},'@',base);
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  scene.textures=textures;scene.textureCount=1;
  object.image=SPRITE_FUNC(Layer)(&objectImage);
  object.image.useColorKey=true;object.image.colorKey=' ';
  object.position=(TDUAL_TYPE(float)){.x=4,.y=2.5f};
  object.size=(TDUAL_TYPE(float)){.x=0.5f,.y=0.8f};object.elevation=-0.5f;
  status=RAYCAST_FUNC(RenderColumnsDepth)(&canvas,&scene,&camera,depth,80);
  if (status==STATUS_CONST(SUCCESS))
    status=RAYCAST_FUNC(RenderSprites)(&canvas,&camera,&object,1,depth,80);
  if (status==STATUS_CONST(SUCCESS)) status=TERMINAL_FUNC(Canvas_Update)(&canvas);
cleanup:
  TERMINAL_FUNC(Canvas_Free)(&objectImage);
  TERMINAL_FUNC(Canvas_Free)(&texture);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return status==STATUS_CONST(SUCCESS) ? 0 : 1;
}
