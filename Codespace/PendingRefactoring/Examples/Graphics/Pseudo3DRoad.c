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
  TERMINAL_TYPE(TCanvas) canvas={0},image={0};
  ROAD_TYPE(TObject) object={0};ROAD_TYPE(TNode) ground;
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) treeStyle=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  float rowDepth[24];
  OPSTATUS status=TERMINAL_FUNC(Canvas_Create)(&canvas,(TDUAL_TYPE(uint16)){.x=80,.y=24});
  if (status!=STATUS_CONST(SUCCESS)) return 1;
  status=TERMINAL_FUNC(Canvas_Create)(&image,(TDUAL_TYPE(uint16)){.x=3,.y=4});
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  treeStyle.flags &= (uint16_t)~TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_FOREGROUND);
  treeStyle.flags |= TEXT_GRID_ATTRIBUTE_CONST(TRANSPARENT_BACKGROUND);
  treeStyle.foreground=0x00EE66;
  status=RASTER_FUNC(FillTriangle)(&image,(TDUAL_TYPE(int32)){.x=1,.y=0},
      (TDUAL_TYPE(int32)){.x=0,.y=2},(TDUAL_TYPE(int32)){.x=2,.y=2},'#',treeStyle);
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  status=RASTER_FUNC(Point)(&image,(TDUAL_TYPE(int32)){.x=1,.y=3},'|',treeStyle);
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  status=ROAD_FUNC(TrackSample)(&track,8,&ground);
  if (status!=STATUS_CONST(SUCCESS)) goto cleanup;
  object.image=SPRITE_FUNC(Layer)(&image);object.image.useColorKey=true;object.image.colorKey=' ';
  object.position=ground.position;object.position.x-=ground.halfWidth+1;
  object.size=(TDUAL_TYPE(float)){.x=1.5f,.y=3};
  status=ROAD_FUNC(RenderDepth)(&canvas,&track,&camera,&style,rowDepth,24);
  if (status==STATUS_CONST(SUCCESS))
    status=ROAD_FUNC(RenderObjects)(&canvas,&camera,&object,1,rowDepth,24);
  if (status==STATUS_CONST(SUCCESS)) status=TERMINAL_FUNC(Canvas_Update)(&canvas);
cleanup:
  TERMINAL_FUNC(Canvas_Free)(&image);
  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return status==STATUS_CONST(SUCCESS) ? 0 : 1;
}
