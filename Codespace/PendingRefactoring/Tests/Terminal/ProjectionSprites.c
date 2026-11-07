#include <assert.h>
#include <math.h>
#include "../../Cosmeron/Modules/Graphics/Raycasting/Render.h"
#include "../../Cosmeron/Modules/Graphics/Pseudo3DRoad/Render.h"
static uint32_t at(TERMINAL_TYPE(TCanvas) *c,unsigned x,unsigned y) {return c->backing->chars.data[y*c->size.x+x];}
int main(void) {
  TERMINAL_TYPE(TCanvas) canvas={0},texture={0},nearImage={0},farImage={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  const uint16_t cells[25]={1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1};
  const TERMINAL_TYPE(TCanvas) *textures[1]={&texture};
  RAYCAST_TYPE(TScene) scene={.map={.cells=cells,.size={.x=5,.y=5},.stride=5,.cellCount=25},
    .wallCodepoint='#',.ceilingCodepoint='.',.floorCodepoint='_',.textures=textures,.textureCount=1};
  RAYCAST_TYPE(TCamera) camera={.position={.x=2.5f,.y=2.5f},.direction={.x=1,.y=0},
    .plane={.x=0,.y=0.66f},.wallHeight=1,.horizon=0.5f,.maxDistance=20};
  RAYCAST_TYPE(TSprite) sprites[2];
  ROAD_TYPE(TNode) nodes[3]={
    {.position={.x=0,.y=0,.z=0},.halfWidth=2},
    {.position={.x=0,.y=0,.z=3},.halfWidth=2},
    {.position={.x=0,.y=0,.z=10},.halfWidth=2}};
  ROAD_TYPE(TTrack) track={.nodes=nodes,.count=3};
  ROAD_TYPE(TCamera) roadCamera={.position={.x=0,.y=1,.z=0},.focalLength=1,.nearClip=0.1f,.drawDistance=20,.horizon=0.25f};
  ROAD_TYPE(TStyle) roadStyle=ROAD_FUNC(StyleDefault)();
  ROAD_TYPE(TObject) objects[2];
  ROAD_TYPE(TNode) sample;
  float depth[20],rows[20];unsigned i;
  scene.wallStyle=scene.sideStyle=scene.floorStyle=scene.ceilingStyle=style;
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,(TDUAL_TYPE(uint16)){.x=20,.y=20})==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Create)(&texture,(TDUAL_TYPE(uint16)){.x=2,.y=2})==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Create)(&nearImage,(TDUAL_TYPE(uint16)){.x=1,.y=1})==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Create)(&farImage,(TDUAL_TYPE(uint16)){.x=1,.y=1})==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Fill)(&nearImage,'N',style)==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Fill)(&farImage,'F',style)==STATUS_CONST(SUCCESS));
  for(i=0;i<4;i++) assert(TERMINAL_FUNC(Canvas_Set)(&texture,
      (TDUAL_TYPE(uint16)){.x=(uint16_t)(i%2),.y=(uint16_t)(i/2)},'A'+i,style)==STATUS_CONST(SUCCESS));
  depth[0]=42;
  assert(RAYCAST_FUNC(RenderColumnsDepth)(&canvas,&scene,&camera,depth,19)==STATUS_CONST(INVALID_ARGUMENT));assert(depth[0]==42);
  assert(RAYCAST_FUNC(RenderColumnsDepth)(&canvas,&scene,&camera,depth,20)==STATUS_CONST(SUCCESS));
  for(i=0;i<20;i++) assert(depth[i]==1.5f);
  assert(at(&canvas,10,5)=='B' && at(&canvas,10,14)=='D');
  sprites[0]=(RAYCAST_TYPE(TSprite)){.image=SPRITE_FUNC(Layer)(&nearImage),
      .position={.x=3,.y=2.5f},.size={.x=0.25f,.y=0.5f},.elevation=-0.25f};
  sprites[1]=(RAYCAST_TYPE(TSprite)){.image=SPRITE_FUNC(Layer)(&farImage),
      .position={.x=3.5f,.y=2.5f},.size={.x=0.5f,.y=1},.elevation=-0.5f};
  assert(RAYCAST_FUNC(RenderSprites)(&canvas,&camera,sprites,2,depth,20)==STATUS_CONST(SUCCESS));
  assert(at(&canvas,10,10)=='N'); /* Sorted even when caller passes near first. */
  sprites[0].position.x=4.5f;
  assert(RAYCAST_FUNC(RenderColumnsDepth)(&canvas,&scene,&camera,depth,20)==STATUS_CONST(SUCCESS));
  assert(RAYCAST_FUNC(RenderSprites)(&canvas,&camera,sprites,1,depth,20)==STATUS_CONST(SUCCESS));
  assert(at(&canvas,10,10)=='D'); /* Wall occludes the sprite. */
  sprites[0].position.x=3;sprites[0].position.y=2.665f;
  assert(RAYCAST_FUNC(RenderSprites)(&canvas,&camera,sprites,1,depth,20)==STATUS_CONST(SUCCESS));
  assert(at(&canvas,15,10)=='N' && at(&canvas,8,10)!='N'); /* Perspective lateral offset. */
  depth[0]=NAN;assert(RAYCAST_FUNC(RenderSprites)(&canvas,&camera,sprites,1,depth,20)==STATUS_CONST(INVALID_ARGUMENT));
  scene.textures=NULL;assert(RAYCAST_FUNC(RenderColumnsDepth)(&canvas,&scene,&camera,depth,20)==STATUS_CONST(INVALID_ARGUMENT));scene.textures=textures;
  assert(ROAD_FUNC(TrackSample)(&track,1.5f,&sample)==STATUS_CONST(SUCCESS));assert(sample.position.z==1.5f && sample.halfWidth==2);
  assert(ROAD_FUNC(TrackSample)(&track,-1,&sample)==STATUS_CONST(OUT_OF_RANGE));assert(sample.position.z==1.5f);
  assert(ROAD_FUNC(RenderDepth)(&canvas,&track,&roadCamera,&roadStyle,rows,20)==STATUS_CONST(SUCCESS));
  assert(rows[0]==FLT_MAX && rows[19]>1 && rows[19]<2);
  objects[0]=(ROAD_TYPE(TObject)){.image=SPRITE_FUNC(Layer)(&nearImage),
      .position={.x=0,.y=0,.z=2},.size={.x=1,.y=1}};
  objects[1]=(ROAD_TYPE(TObject)){.image=SPRITE_FUNC(Layer)(&farImage),
      .position={.x=0,.y=0,.z=4},.size={.x=2,.y=2}};
  assert(ROAD_FUNC(RenderObjects)(&canvas,&roadCamera,objects,2,rows,20)==STATUS_CONST(SUCCESS));assert(at(&canvas,10,10)=='N');
  /* Crest masks the bottom of a tall object, while the top stays visible. */
  nodes[1].position.y=1;
  objects[0].position.z=5;objects[0].size.y=2;
  assert(ROAD_FUNC(RenderDepth)(&canvas,&track,&roadCamera,&roadStyle,rows,20)==STATUS_CONST(SUCCESS));
  assert(ROAD_FUNC(RenderObjects)(&canvas,&roadCamera,objects,1,rows,20)==STATUS_CONST(SUCCESS));
  assert(at(&canvas,10,2)=='N' && at(&canvas,10,6)!='N');
  objects[0].position.z=-1;
  assert(ROAD_FUNC(RenderObjects)(&canvas,&roadCamera,objects,1,rows,20)==STATUS_CONST(SUCCESS));
  assert(ROAD_FUNC(RenderDepth)(&canvas,&track,&roadCamera,&roadStyle,rows,19)==STATUS_CONST(INVALID_ARGUMENT));
  rows[0]=NAN;assert(ROAD_FUNC(RenderObjects)(&canvas,&roadCamera,objects,1,rows,20)==STATUS_CONST(INVALID_ARGUMENT));
  TERMINAL_FUNC(Canvas_Free)(&farImage);TERMINAL_FUNC(Canvas_Free)(&nearImage);
  TERMINAL_FUNC(Canvas_Free)(&texture);TERMINAL_FUNC(Canvas_Free)(&canvas);
  return 0;
}
