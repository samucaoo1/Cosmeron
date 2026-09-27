#include "../../Cosmeron/Modules/Random/Random.h"

static int sorted_sum(const int *v, size_t n) {
  int s=0;
  for(size_t i=0;i<n;++i) s+=v[i];
  return s;
}

int main(void) {
  RANDOM_SOURCE_TYPE(Value) source;
  RANDOM_ENGINE_TYPE(Xoshiro) storage;
  if (Random_Source_InitWithState(&source,&Random_Engine_Xoshiro_FunctionTable,&storage,sizeof(storage),42U,Random_Mixer_Splitmix64)!=STATUS_CONST(SUCCESS)) return 1;
  for (int i=0;i<64;++i) {
    uint64_t u=Random_Distribution_U64(&source,10U,20U);
    int64_t s=Random_Distribution_I64(&source,-10,10);
    double d=Random_Distribution_F64(&source);
    if (u<10U || u>20U || s<-10 || s>10 || d<0.0 || d>=1.0) return 2;
    (void)Random_Distribution_Bool(&source);
  }
  if (Random_Source_Reseed(&source,42U)!=STATUS_CONST(SUCCESS)) return 3;
  uint64_t first=Random_Source_NextU64(&source);
  if (Random_Source_Reseed(&source,42U)!=STATUS_CONST(SUCCESS)) return 4;
  if (first!=Random_Source_NextU64(&source)) return 5;

  int v[]={1,2,3,4,5,6};
  int before=sorted_sum(v,6);
  if (Random_Shuffle_VectorFromSource(v,6,sizeof(v[0]),&source)!=STATUS_CONST(SUCCESS)) return 6;
  if (sorted_sum(v,6)!=before) return 7;
  Random_Source_Destroy(&source);
  if (source.state!=NULL || source.engine!=NULL) return 8;
  return 0;
}
