#include <assert.h>
#include <math.h>
#include <string.h>
#include "../../Cosmeron/Modules/Graphics/Raycasting/Render.h"
static RAYCAST_TYPE(TRay2D) ray(float x,float y,float dx,float dy,float limit) {
  return (RAYCAST_TYPE(TRay2D)){.origin={.x=x,.y=y},.direction={.x=dx,.y=dy},.maxDistance=limit};
}
int main(void) {
  uint16_t cells[25]={1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1};
  RAYCAST_TYPE(TGridMap) map={.cells=cells,.size={.x=5,.y=5},.stride=5,.cellCount=25};
  RAYCAST_TYPE(TRay2D) r=ray(2.5f,2.5f,1,0,10);
  RAYCAST_TYPE(THit) hit={0},saved;
  RAYCAST_TYPE(TCamera) camera={.position={.x=2.5f,.y=2.5f},.direction={.x=1,.y=0},
      .plane={.x=0,.y=0.66f},.maxDistance=10,.wallHeight=1,.horizon=0.5f};
  TERMINAL_TYPE(TCanvas) canvas={0},view={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) style=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  RAYCAST_TYPE(TScene) scene={.map=map,.wallCodepoint='#',.ceilingCodepoint='.',.floorCodepoint='_'};
  unsigned x,y;
  scene.wallStyle=scene.sideStyle=scene.ceilingStyle=scene.floorStyle=style;
  assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
  assert(hit.cell.x==4 && hit.cell.y==2 && hit.distance==1.5f && hit.normal.x==-1);
  assert(hit.wallCoordinate==0.5f && hit.side==RAYCAST_CONST(SIDE_X));
  r=ray(2.5f,2.5f,-1,0,10);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
  assert(hit.cell.x==0 && hit.distance==1.5f && hit.normal.x==1);
  r=ray(2.5f,2.5f,0,1,10);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
  assert(hit.cell.y==4 && hit.normal.y==-1);
  r=ray(2.5f,2.5f,0,-1,10);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
  assert(hit.cell.y==0 && hit.normal.y==1);
  saved=hit;r=ray(2.5f,2.5f,1,0,1);
  assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(NOT_FOUND));assert(memcmp(&hit,&saved,sizeof hit)==0);
  r=ray(2.5f,2.5f,0,0,10);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(INVALID_ARGUMENT));
  r=ray(NAN,1,1,0,10);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(INVALID_ARGUMENT));
  r=ray(1,1,INFINITY,0,10);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(INVALID_ARGUMENT));
  r=ray(-2,2.5f,1,0,10);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
  assert(hit.distance==2 && hit.cell.x==0 && hit.normal.x==-1);
  r=ray(7,2.5f,-1,0,10);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
  assert(hit.distance==2 && hit.cell.x==4);
  r=ray(-2,2,-1,0,10);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(NOT_FOUND));
  r=ray(0.5f,0.5f,1,0,0);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
  assert(hit.distance==0 && hit.side==RAYCAST_CONST(SIDE_NONE));
  r=ray(1,2.5f,-1,0,10);assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
  assert(hit.distance==0 && hit.cell.x==0);
  map.cellCount=24;assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(INVALID_ARGUMENT));map.cellCount=25;
  /* Exact diagonal tie visits X neighbor first. */
  cells[2*5+3]=2;cells[3*5+2]=3;r=ray(2.5f,2.5f,1,1,10);
  assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));assert(hit.material==2 && hit.side==RAYCAST_CONST(SIDE_X));
  cells[2*5+3]=cells[3*5+2]=0;
  for(x=0;x<9;x++) {
    assert(RAYCAST_FUNC(CameraRay)(&camera,(uint16_t)x,9,&r)==STATUS_CONST(SUCCESS));
    assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
    assert(hit.distance==1.5f); /* Flat wall stays flat across the field of view. */
  }
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,(TDUAL_TYPE(uint16)){.x=13,.y=13})==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_View)(&canvas,(TQUAD_TYPE(uint16)){.left=2,.top=2,.right=10,.bottom=10},&view)==STATUS_CONST(SUCCESS));
  assert(RAYCAST_FUNC(RenderColumns)(&view,&scene,&camera)==STATUS_CONST(SUCCESS));
  for(y=0;y<13;y++) for(x=0;x<13;x++) {
    uint32_t cp;TEXT_GRID_ATTRIBUTE_TYPE(Cell) actual;
    assert(TERMINAL_FUNC(Canvas_Get)(&canvas,(TDUAL_TYPE(uint16)){.x=(uint16_t)x,.y=(uint16_t)y},&cp,&actual)==STATUS_CONST(SUCCESS));
    if(x<2 || x>10 || y<2 || y>10) assert(cp==' ');
    else if(y==2) assert(cp=='.');
    else if(y>=4 && y<=8) assert(cp=='#');
    else if(y==10) assert(cp=='_');
  }
  /* Compare DDA with independent ray/box intersections for non-corner rays. */
  for(x=0;x<400;x++) {
    double best=100,dx,dy;
    bool found=false;
    unsigned index;
    r=ray(-1.37f+(float)(x%31)*0.23f,-0.83f+(float)(x%23)*0.29f,
        -1.11f+(float)(x%17)*0.137f,0.07f+(float)(x%13)*0.113f,20);
    dx=r.direction.x;dy=r.direction.y;
    for(index=0;index<25;index++) if(cells[index]) {
      double ox=r.origin.x,oy=r.origin.y,lo=0,hi=20;
      double ax=((double)(index%5)-ox)/dx,bx=((double)(index%5)+1-ox)/dx;
      double ay=((double)(index/5)-oy)/dy,by=((double)(index/5)+1-oy)/dy,temp;
      if(ax>bx){temp=ax;ax=bx;bx=temp;}if(ay>by){temp=ay;ay=by;by=temp;}
      if(ax>lo)lo=ax;
      if(ay>lo)lo=ay;
      if(bx<hi)hi=bx;
      if(by<hi)hi=by;
      if(lo<=hi && lo<best){best=lo;found=true;}
    }
    if(found) {
      double error;
      assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
      error=hit.distance-best;assert(error>-0.00001 && error<0.00001);
    } else assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(NOT_FOUND));
  }
  r=ray(-FLT_MAX,2,FLT_MAX,0,2);
  assert(RAYCAST_FUNC(GridCast)(&map,&r,&hit)==STATUS_CONST(SUCCESS));
  scene.wallCodepoint=0x4e00;assert(RAYCAST_FUNC(RenderColumns)(&view,&scene,&camera)==STATUS_CONST(NOT_SUPPORTED));
  camera.direction.x=2;assert(RAYCAST_FUNC(CameraRay)(&camera,0,9,&r)==STATUS_CONST(INVALID_ARGUMENT));
  TERMINAL_FUNC(Canvas_Free)(&view);TERMINAL_FUNC(Canvas_Free)(&canvas);
  return 0;
}
