/* Standalone .def verification for Core templates (Mode 1 & Mode 2)
 * Must compile completely independently without including any table .h headers
 * (Bitset.h, Block.h, ByteTag.h, Optional.h, Result.h, Pointer, Swap, Repeat, Bit).
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

/* --- Mode 2 Direct Includes --- */
#define SUFFIX U64
#define TYPE uint64_t
#include "../../Cosmeron/Core/Bit/Bit.def"

#define SUFFIX My64
#define BIT_COUNT 64U
#include "../../Cosmeron/Core/Bit/Bitset.def"

#define SUFFIX MyInt
#define TYPE int32_t
#include "../../Cosmeron/Core/Algorithm/Swap.def"

#define SUFFIX MyInt
#define TYPE int32_t
#include "../../Cosmeron/Core/Algorithm/Repeat.def"

#define SUFFIX SoloU32
#define TYPE uint32_t
#include "../../Cosmeron/Core/Memory/Pointer/Unique.def"

#define SUFFIX SoloOpt
#define TYPE int32_t
#include "../../Cosmeron/Core/Types/Optional.def"

#define SUFFIX SoloRes
#define TYPE int32_t
#include "../../Cosmeron/Core/Types/Result.def"

/* --- Mode 1 Macro Instantiations --- */
#include "../../Cosmeron/Core/Memory/Block.def"
TEMPLATE_INSTANTIATE(Memory, Block, 64, uint64_t);

#include "../../Cosmeron/Core/Types/ByteTag.def"
TEMPLATE_INSTANTIATE(Type, ByteTag, 32, uint32_t);

#include "../../Cosmeron/Core/Memory/Pointer/Weak.def"
#include "../../Cosmeron/Core/Memory/Pointer/Shared.def"
#include "../../Cosmeron/Core/Memory/Pointer/AtomicWeak.def"
#include "../../Cosmeron/Core/Memory/Pointer/AtomicShared.def"
#include "../../Cosmeron/Core/Memory/Pointer/Intrusive.def"

TEMPLATE_INSTANTIATE(Pointer, Weak, I32, int32_t);
TEMPLATE_INSTANTIATE(Pointer, Shared, I32, int32_t);
TEMPLATE_INSTANTIATE(Pointer, AtomicWeak, I32, int32_t);
TEMPLATE_INSTANTIATE(Pointer, AtomicShared, I32, int32_t);

typedef struct StandaloneIntrusiveItem {
  INTRUSIVE_REF_COUNT_FIELD
  int32_t value;
} StandaloneIntrusiveItem;

TEMPLATE_INSTANTIATE(Pointer, Intrusive, Item, StandaloneIntrusiveItem);

/* Also verify Allocator_Ops, Collector_Descriptor, Exception_Frame, Panic_Info */
#include "../../Cosmeron/Core/Memory/Collector.h"
#include "../../Cosmeron/Core/Condition/Exception.h"
#include "../../Cosmeron/Core/Condition/Panic.h"

int main(void) {
  /* Bit & Bitset (Mode 2) */
  uint64_t v = 0ULL;
  assert(Bit_U64_Set(&v, 3U) == STATUS_CONST(SUCCESS));
  assert(Bit_U64_Check(v, 3U));

  TBitsetMy64 bs = {0};
  assert(BitsetMy64_Init(&bs) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&bs, BitSet, 17U) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&bs, Count) == 1U);

  /* Block64 & ByteTag32 (Mode 1) */
  TMemory_Block64 blk = {0};
  assert(Memory_Block64_Init(&blk) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&blk, BitSet, 5U) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&blk, Popcount) == 1U);

  TType_ByteTag32 tag = {0};
  assert(Type_ByteTag32_Init(&tag) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&tag, FromString, "COSM") == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&tag, Text)[0] == 'C');

  /* Swap & Repeat (Mode 2) */
  int32_t a = 1, b = 2;
  Memory_Swap_MyInt_Values(&a, &b);
  assert(a == 2 && b == 1);
  int32_t arr[3] = {0};
  assert(Algorithm_Repeat_MyInt_Fill(arr, 7, 3U) == STATUS_CONST(SUCCESS));
  assert(arr[0] == 7 && arr[2] == 7);

  /* Optional & Result (Mode 2, root modules) */
  TOptional_SoloOpt opt = {0};
  assert(Optional_SoloOpt_Init(&opt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&opt, Some, 42) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&opt, IsSome));

  TResult_SoloRes res = {0};
  assert(Result_SoloRes_Init(&res) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&res, Ok, 99) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&res, IsOk));

  /* Pointers (Mode 1 & Mode 2) */
  TPointer_Unique_SoloU32 u = {0};
  assert(Pointer_Unique_SoloU32_Init(&u) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&u, Alloc, 123U) == STATUS_CONST(SUCCESS));
  assert(*OBJECT_CALL_0(&u, Get) == 123U);
  OBJECT_CALL_0(&u, Free);

  TPointer_Shared_I32 sh = {0};
  assert(Pointer_Shared_I32_Init(&sh) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&sh, Alloc, 77) == STATUS_CONST(SUCCESS));
  TPointer_Weak_I32 wk = {0};
  assert(OBJECT_CALL(&sh, Downgrade, &wk) == STATUS_CONST(SUCCESS));
  assert(!OBJECT_CALL_0(&wk, IsExpired));
  OBJECT_CALL_0(&wk, Free);
  OBJECT_CALL_0(&sh, Free);

  TPointer_AtomicShared_I32 ash = {0};
  assert(Pointer_AtomicShared_I32_Init(&ash) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&ash, Alloc, 88) == STATUS_CONST(SUCCESS));
  TPointer_AtomicWeak_I32 awk = {0};
  assert(OBJECT_CALL(&ash, Downgrade, &awk) == STATUS_CONST(SUCCESS));
  OBJECT_CALL_0(&awk, Free);
  OBJECT_CALL_0(&ash, Free);

  /* Core structs check */
  assert(sizeof(TMemory_Allocator_Ops) > 0U);
  assert(sizeof(TMemory_Collector_Descriptor) > 0U);
  assert(sizeof(TCondition_Exception_Frame) > 0U);
  assert(sizeof(TCondition_Panic_Info) > 0U);

  printf("Core StandaloneDef tests passed!\n");
  return 0;
}
