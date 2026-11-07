#include "../../Cosmeron/Modules/Struct/TDual.h"
#include "../../Cosmeron/Modules/Struct/TPair.h"
#include "../../Cosmeron/Modules/Struct/TTriple.h"
#include "../../Cosmeron/Modules/Struct/TQuad.h"
#include "../../Cosmeron/Modules/Struct/TPenta.h"

int main(void) {
  int32_t a=0,b=0,c=0,d=0,e=0;
  Struct_TPair_int32 p = Cast_int32_To_TPair(1,2);
  Cast_TPair_To_int32(p,&a,&b);
  if (a!=1 || b!=2 || p.x!=1 || p.y!=2) return 1;

  Struct_TDual_int32 q = Cast_int32_To_TDual(3,4);
  Cast_TDual_To_int32(q,&a,&b);
  if (a!=3 || b!=4 || q.first!=3 || q.last!=4) return 2;

  Struct_TTriple_int32 t = Cast_int32_To_TTriple(5,6,7);
  Cast_TTriple_To_int32(t,&a,&b,&c);
  if (a!=5 || b!=6 || c!=7 || t.length!=5 || t.height!=7) return 3;

  Struct_TQuad_int32 u = Cast_int32_To_TQuad(8,9,10,11);
  Cast_TQuad_To_int32(u,&a,&b,&c,&d);
  if (a!=8 || b!=9 || c!=10 || d!=11 || u.left!=8 || u.bottom!=11) return 4;

  Struct_TPenta_int32 v = Cast_int32_To_TPenta(12,13,14,15,16);
  Cast_TPenta_To_int32(v,&a,&b,&c,&d,&e);
  if (a!=12 || b!=13 || c!=14 || d!=15 || e!=16 || v.values[4]!=16) return 5;

  if (sizeof(Struct_TPair_uint64) != sizeof(uint64_t)*2U) return 6;
  if (sizeof(Struct_TTriple_uint16) != sizeof(uint16_t)*3U) return 7;
  if (sizeof(Struct_TQuad_uint8) != sizeof(uint8_t)*4U) return 8;
  if (sizeof(Struct_TPenta_uint32) != sizeof(uint32_t)*5U) return 9;
  return 0;
}
