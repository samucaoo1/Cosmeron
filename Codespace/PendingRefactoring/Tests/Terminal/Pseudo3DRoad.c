#include <assert.h>
#include <math.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/Pseudo3DRoad/Render.h"
int main(void) {
  ROAD_TYPE(TNode) nodes[5]={
    {.position={.x=0,.y=0,.z=0},.halfWidth=1},
    {.position={.x=0,.y=0,.z=2},.halfWidth=1},
    {.position={.x=0,.y=0,.z=4},.halfWidth=1},
    {.position={.x=0,.y=0,.z=8},.halfWidth=1},
    {.position={.x=0,.y=0,.z=32},.halfWidth=1}};
  ROAD_TYPE(TTrack) track={.nodes=nodes,.count=5};
  ROAD_TYPE(TCamera) camera={.position={.x=0,.y=1,.z=0},.focalLength=1,
      .nearClip=0.1f,.drawDistance=32,.horizon=0.25f};
  ROAD_TYPE(TProjection) projection={0},saved;
  ROAD_TYPE(TStyle) style=ROAD_FUNC(StyleDefault)();
  TERMINAL_TYPE(TCanvas) canvas={0},view={0},nested={0};
  unsigned x,y;
  assert(ROAD_FUNC(TrackValidate)(&track)==STATUS_CONST(SUCCESS));
  assert(ROAD_FUNC(Project)(&camera,(TDUAL_TYPE(uint16)){.x=20,.y=16},&nodes[1],&projection)==STATUS_CONST(SUCCESS));
  assert(projection.center.x==10 && projection.center.y==12 && projection.halfWidth==8 && projection.depth==2);
  saved=projection;
  assert(ROAD_FUNC(Project)(&camera,(TDUAL_TYPE(uint16)){.x=20,.y=16},&nodes[0],&projection)==STATUS_CONST(OUT_OF_RANGE));
  assert(memcmp(&projection,&saved,sizeof projection)==0);
  nodes[1].position.z=0;assert(ROAD_FUNC(TrackValidate)(&track)==STATUS_CONST(INVALID_ARGUMENT));nodes[1].position.z=2;
  nodes[1].position.x=NAN;assert(ROAD_FUNC(TrackValidate)(&track)==STATUS_CONST(INVALID_ARGUMENT));nodes[1].position.x=0;
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,(TDUAL_TYPE(uint16)){.x=24,.y=20})==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_View)(&canvas,(TQUAD_TYPE(uint16)){.left=1,.top=1,.right=22,.bottom=18},&view)==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_View)(&view,(TQUAD_TYPE(uint16)){.left=1,.top=1,.right=20,.bottom=16},&nested)==STATUS_CONST(SUCCESS));
  style.lanes=1;
  assert(ROAD_FUNC(Render)(&nested,&track,&camera,&style)==STATUS_CONST(SUCCESS));
  for(y=0;y<20;y++) for(x=0;x<24;x++) {
    uint32_t cp;TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr;
    assert(TERMINAL_FUNC(Canvas_Get)(&canvas,(TDUAL_TYPE(uint16)){.x=(uint16_t)x,.y=(uint16_t)y},&cp,&attr)==STATUS_CONST(SUCCESS));
    if(x<2 || x>=22 || y<2 || y>=18) assert(attr.flags&TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_BACKGROUND));
    else if(y<6) assert(attr.background==style.sky.background);
    else if(x==12 && y>=7) assert(attr.background==style.road[0].background || attr.background==style.road[1].background);
  }
  /* Curves, hills and camera motion, followed by viewport resizing. */
  nodes[2].position.x=2;nodes[2].position.y=1;
  nodes[3].position.x=-3;nodes[3].position.y=2;
  camera.position.x=0.5f;camera.position.z=1;
  assert(ROAD_FUNC(Render)(&nested,&track,&camera,&style)==STATUS_CONST(SUCCESS));
  /* A crest hides changes to the descending far segment. */
  {
    TEXT_GRID_ATTRIBUTE_TYPE(Cell) snapshot[320];
    camera.position=(TTRIPLE_TYPE(float)){.x=0,.y=1,.z=0};
    nodes[2].position=(TTRIPLE_TYPE(float)){.x=0,.y=2,.z=4};
    nodes[3].position=(TTRIPLE_TYPE(float)){.x=0,.y=0,.z=8};
    assert(ROAD_FUNC(Render)(&nested,&track,&camera,&style)==STATUS_CONST(SUCCESS));
    for(y=0;y<16;y++) for(x=0;x<20;x++)
      snapshot[y*20+x]=canvas.backing->attributes.data[(y+2)*24+x+2];
    nodes[3].position.x=1000;nodes[4].position.x=-1000;
    assert(ROAD_FUNC(Render)(&nested,&track,&camera,&style)==STATUS_CONST(SUCCESS));
    for(y=0;y<16;y++) for(x=0;x<20;x++)
      assert(snapshot[y*20+x].background==canvas.backing->attributes.data[(y+2)*24+x+2].background);
  }
  camera.nearClip=0;assert(ROAD_FUNC(Render)(&nested,&track,&camera,&style)==STATUS_CONST(INVALID_ARGUMENT));camera.nearClip=0.1f;
  style.codepoint=0x4e00;assert(ROAD_FUNC(Render)(&nested,&track,&camera,&style)==STATUS_CONST(NOT_SUPPORTED));style.codepoint=' ';
  style.stripeSegments=0;assert(ROAD_FUNC(Render)(&nested,&track,&camera,&style)==STATUS_CONST(INVALID_ARGUMENT));style.stripeSegments=2;
  TERMINAL_FUNC(Canvas_Free)(&nested);TERMINAL_FUNC(Canvas_Free)(&view);
  assert(TERMINAL_FUNC(Canvas_Resize)(&canvas,(TDUAL_TYPE(uint16)){.x=1,.y=1})==STATUS_CONST(SUCCESS));
  assert(ROAD_FUNC(Render)(&canvas,&track,&camera,&style)==STATUS_CONST(SUCCESS));
  TERMINAL_FUNC(Canvas_Free)(&canvas);
  return 0;
}
