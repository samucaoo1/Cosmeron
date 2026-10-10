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
  TERMINAL_TYPE(TCanvas) canvas={0};
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
  status=RAYCAST_FUNC(RenderColumns)(&canvas,&scene,&camera);
  if (status==STATUS_CONST(SUCCESS)) status=TERMINAL_FUNC(Canvas_Update)(&canvas);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return status==STATUS_CONST(SUCCESS) ? 0 : 1;
}
