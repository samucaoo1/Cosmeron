#include "../../Cosmeron/Modules/Graphics/Pseudo3DRoad/Render.h"

int main(void) {
  const ROAD_TYPE(TNode) nodes[]={
    {.position={.x=0,.y=0,.z=0},.halfWidth=2},
    {.position={.x=0,.y=0,.z=4},.halfWidth=2},
    {.position={.x=0,.y=0,.z=8},.halfWidth=2},
    {.position={.x=1,.y=0,.z=12},.halfWidth=2},
    {.position={.x=3,.y=0.2f,.z=16},.halfWidth=2},
    {.position={.x=5,.y=0.5f,.z=20},.halfWidth=2},
    {.position={.x=6,.y=1,.z=28},.halfWidth=2},
    {.position={.x=5,.y=1.5f,.z=40},.halfWidth=2},
    {.position={.x=0,.y=0,.z=64},.halfWidth=2}};
  const ROAD_TYPE(TTrack) track={.nodes=nodes,.count=sizeof nodes/sizeof nodes[0]};
  const ROAD_TYPE(TCamera) camera={.position={.x=0,.y=1.5f,.z=0},
      .focalLength=1,.nearClip=0.1f,.drawDistance=64,.horizon=0.3f};
  ROAD_TYPE(TStyle) style=ROAD_FUNC(StyleDefault)();
  TERMINAL_TYPE(TCanvas) canvas={0};
  OPSTATUS status=TERMINAL_FUNC(Canvas_Create)(&canvas,(TDUAL_TYPE(uint16)){.x=80,.y=24});
  if (status!=STATUS_CONST(SUCCESS)) return 1;
  status=ROAD_FUNC(Render)(&canvas,&track,&camera,&style);
  if (status==STATUS_CONST(SUCCESS)) status=TERMINAL_FUNC(Canvas_Update)(&canvas);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return status==STATUS_CONST(SUCCESS) ? 0 : 1;
}
