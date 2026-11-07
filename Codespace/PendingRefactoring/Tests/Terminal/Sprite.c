#include <assert.h>
#include "../../Cosmeron/Modules/Graphics/Sprite/Sprite.h"
static uint32_t get(TERMINAL_TYPE(TCanvas) *c,uint16_t x,uint16_t y) {
  uint32_t cp;TEXT_GRID_ATTRIBUTE_TYPE(Cell) a;
  assert(TERMINAL_FUNC(Canvas_Get)(c,(TDUAL_TYPE(uint16)){.x=x,.y=y},&cp,&a)==STATUS_CONST(SUCCESS));return cp;
}
int main(void) {
  TERMINAL_TYPE(TCanvas) canvas={0},source={0},view={0};
  TEXT_GRID_ATTRIBUTE_TYPE(Cell) attr=TEXT_GRID_ATTRIBUTE_FUNC(Default)();
  SPRITE_TYPE(TLayer) layer,layers[2];
  SPRITE_TYPE(TFrame) frames[2];SPRITE_TYPE(TAnimation) animation;
  size_t index=99;
  assert(TERMINAL_FUNC(Canvas_Create)(&canvas,(TDUAL_TYPE(uint16)){.x=8,.y=6})==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Create)(&source,(TDUAL_TYPE(uint16)){.x=2,.y=2})==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Set)(&source,(TDUAL_TYPE(uint16)){.x=0,.y=0},'A',attr)==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Set)(&source,(TDUAL_TYPE(uint16)){.x=1,.y=0},'.',attr)==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Set)(&source,(TDUAL_TYPE(uint16)){.x=0,.y=1},'B',attr)==STATUS_CONST(SUCCESS));
  assert(TERMINAL_FUNC(Canvas_Set)(&source,(TDUAL_TYPE(uint16)){.x=1,.y=1},'C',attr)==STATUS_CONST(SUCCESS));
  layer=SPRITE_FUNC(Layer)(&source);layer.position=(TDUAL_TYPE(int32)){.x=1,.y=1};
  layer.size=(TDUAL_TYPE(uint16)){.x=4,.y=4};layer.useColorKey=true;layer.colorKey='.';
  assert(TERMINAL_FUNC(Canvas_Fill)(&canvas,'_',attr)==STATUS_CONST(SUCCESS));
  assert(SPRITE_FUNC(DrawWith)(&canvas,&layer)==STATUS_CONST(SUCCESS));
  assert(get(&canvas,1,1)=='A' && get(&canvas,2,2)=='A' && get(&canvas,3,1)=='_' && get(&canvas,4,4)=='C');
  layer.flipX=true;layer.flipY=true;
  assert(SPRITE_FUNC(DrawWith)(&canvas,&layer)==STATUS_CONST(SUCCESS));
  assert(get(&canvas,1,1)=='C' && get(&canvas,4,4)=='A');
  layer=SPRITE_FUNC(Layer)(&source);layer.region=(TQUAD_TYPE(uint16)){.left=0,.top=1,.right=0,.bottom=1};
  layer.size=(TDUAL_TYPE(uint16)){.x=1,.y=1};
  assert(SPRITE_FUNC(DrawWith)(&canvas,&layer)==STATUS_CONST(SUCCESS));assert(get(&canvas,0,0)=='B');
  layers[0]=layers[1]=layer;layers[0].position=(TDUAL_TYPE(int32)){.x=100,.y=100};
  layers[1].position=layers[0].position;layers[1].region.left=layers[1].region.right=1;
  assert(SPRITE_FUNC(Compose)(&canvas,layers,2,(TDUAL_TYPE(int32)){.x=100,.y=100})==STATUS_CONST(SUCCESS));
  assert(get(&canvas,0,0)=='C');
  assert(SPRITE_FUNC(Compose)(&canvas,NULL,0,(TDUAL_TYPE(int32)){0})==STATUS_CONST(SUCCESS));
  /* Validation happens before any layer is drawn. */
  layers[1].region.right=100;
  assert(SPRITE_FUNC(Compose)(&canvas,layers,2,(TDUAL_TYPE(int32)){.x=100,.y=100})!=STATUS_CONST(SUCCESS));
  assert(get(&canvas,0,0)=='C');
  assert(SPRITE_FUNC(Draw)(&canvas,&canvas,(TDUAL_TYPE(int32)){0})==STATUS_CONST(INVALID_ARGUMENT));
  assert(SPRITE_FUNC(Draw)(&canvas,&source,(TDUAL_TYPE(int32)){.x=-1,.y=-1})==STATUS_CONST(SUCCESS));
  assert(get(&canvas,0,0)=='C');
  /* Transparent background preserves destination color/default flag. */
  attr.background=0x123456;attr.flags &= (uint16_t)~TEXT_GRID_ATTRIBUTE_CONST(DEFAULT_BACKGROUND);
  assert(TERMINAL_FUNC(Canvas_Fill)(&canvas,'_',attr)==STATUS_CONST(SUCCESS));
  attr.flags |= TEXT_GRID_ATTRIBUTE_CONST(TRANSPARENT_BACKGROUND);
  assert(TERMINAL_FUNC(Canvas_Set)(&source,(TDUAL_TYPE(uint16)){.x=0,.y=0},'A',attr)==STATUS_CONST(SUCCESS));
  assert(SPRITE_FUNC(Draw)(&canvas,&source,(TDUAL_TYPE(int32)){0})==STATUS_CONST(SUCCESS));
  assert(canvas.backing->attributes.data[0].background==0x123456);
  assert(!(canvas.backing->attributes.data[0].flags&TEXT_GRID_ATTRIBUTE_CONST(TRANSPARENT_BACKGROUND)));
  /* Nested drawing is bounded by a borrowed view. */
  assert(TERMINAL_FUNC(Canvas_View)(&canvas,(TQUAD_TYPE(uint16)){.left=4,.top=4,.right=5,.bottom=5},&view)==STATUS_CONST(SUCCESS));
  assert(SPRITE_FUNC(Draw)(&view,&source,(TDUAL_TYPE(int32)){.x=1,.y=1})==STATUS_CONST(SUCCESS));
  assert(get(&canvas,5,5)=='A' && get(&canvas,6,5)=='_');
  frames[0]=(SPRITE_TYPE(TFrame)){.source=&source,.region={.left=0,.top=0,.right=0,.bottom=0},.duration=10};
  frames[1]=(SPRITE_TYPE(TFrame)){.source=&source,.region={.left=1,.top=1,.right=1,.bottom=1},.duration=20};
  animation=(SPRITE_TYPE(TAnimation)){.frames=frames,.count=2,.loop=true};
  assert(SPRITE_FUNC(FrameAt)(&animation,9,&index)==STATUS_CONST(SUCCESS) && index==0);
  assert(SPRITE_FUNC(FrameAt)(&animation,10,&index)==STATUS_CONST(SUCCESS) && index==1);
  assert(SPRITE_FUNC(FrameAt)(&animation,30,&index)==STATUS_CONST(SUCCESS) && index==0);
  animation.loop=false;assert(SPRITE_FUNC(FrameAt)(&animation,UINT64_MAX,&index)==STATUS_CONST(SUCCESS) && index==1);
  layer.size=(TDUAL_TYPE(uint16)){.x=1,.y=1};layer.position=(TDUAL_TYPE(int32)){.x=7,.y=0};
  assert(SPRITE_FUNC(DrawFrame)(&canvas,&animation,10,&layer)==STATUS_CONST(SUCCESS));assert(get(&canvas,7,0)=='C');
  frames[0].duration=UINT64_MAX;index=42;
  assert(SPRITE_FUNC(FrameAt)(&animation,0,&index)==STATUS_CONST(OUT_OF_RANGE) && index==42);
  frames[0].duration=0;assert(SPRITE_FUNC(FrameAt)(&animation,0,&index)==STATUS_CONST(INVALID_ARGUMENT));
  assert(TERMINAL_FUNC(Canvas_Set)(&source,(TDUAL_TYPE(uint16)){0},0x4e00,attr)==STATUS_CONST(SUCCESS));
  assert(SPRITE_FUNC(Draw)(&canvas,&source,(TDUAL_TYPE(int32)){0})==STATUS_CONST(NOT_SUPPORTED));
  TERMINAL_FUNC(Canvas_Free)(&view);TERMINAL_FUNC(Canvas_Free)(&source);TERMINAL_FUNC(Canvas_Free)(&canvas);
  return 0;
}
