/* Standalone .def verification for Container, Tuple, Numeric, Text, Math
 * Must compile completely independently without including any pre-instantiated
 * table .h headers.
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

/* --- Tuple (Mode 1 & Mode 2) --- */
#define SUFFIX MyI32
#define TYPE int32_t
#include "../../Cosmeron/Modules/Tuple/Pair.def"

#include "../../Cosmeron/Modules/Tuple/Dual.def"
#include "../../Cosmeron/Modules/Tuple/Triple.def"
TEMPLATE_INSTANTIATE(Tuple, Dual, MyI32, int32_t);
TEMPLATE_INSTANTIATE(Tuple, Triple, MyI32, int32_t);

/* --- Iterator + Array + Linked (Mode 1 & Mode 2) --- */
#include "../../Cosmeron/Modules/Container/Iterator/Stream.def"
#include "../../Cosmeron/Modules/Container/Iterator/Contiguous.def"
#include "../../Cosmeron/Modules/Container/Array/Vector.def"
TEMPLATE_INSTANTIATE(Iterator, Stream, MyI32, int32_t);
TEMPLATE_INSTANTIATE(Iterator, Contiguous, MyI32, int32_t);
TEMPLATE_INSTANTIATE(Array, Vector, MyI32, int32_t);

#define SUFFIX MyI32
#define TYPE int32_t
#include "../../Cosmeron/Modules/Container/Array/Stack.def"

#define SUFFIX MyI32
#define TYPE int32_t
#include "../../Cosmeron/Modules/Container/Linked/SingleNode.def"

#define SUFFIX MyI32
#define TYPE int32_t
#include "../../Cosmeron/Modules/Container/Iterator/Forward.def"

#define SUFFIX MyI32
#define TYPE int32_t
#include "../../Cosmeron/Modules/Container/Linked/Stack.def"

/* --- Hash (Mode 1 & Mode 2) --- */
#define SUFFIX MyI32
#define KEY_TYPE int32_t
#include "../../Cosmeron/Modules/Container/Hash/Set.def"

#include "../../Cosmeron/Modules/Container/Hash/Map.def"
TEMPLATE_INSTANTIATE(Hash, Map, MyKV, int32_t, int32_t);

/* --- Tree (Mode 1 & Mode 2) --- */
#define SUFFIX MyI32
#define KEY_TYPE int32_t
#include "../../Cosmeron/Modules/Container/Tree/BST.def"

#include "../../Cosmeron/Modules/Container/Tree/AVL.def"
#include "../../Cosmeron/Modules/Container/Tree/RedBlack.def"
#include "../../Cosmeron/Modules/Container/Tree/BTree.def"
TEMPLATE_INSTANTIATE(Tree, AVL_Set, MyI32, int32_t);
TEMPLATE_INSTANTIATE(Tree, RedBlack_Set, MyI32, int32_t);
TEMPLATE_INSTANTIATE(Tree, B_Set, MyI32, int32_t);

/* --- Graph (Mode 1) --- */
#include "../../Cosmeron/Modules/Container/Graph/Graph.def"
TEMPLATE_INSTANTIATE(Graph, MyI32, int32_t, int32_t);

/* --- Numeric (Mode 1 & Mode 2) --- */
#include "../../Cosmeron/Modules/Numeric/Bigint.def"
#include "../../Cosmeron/Modules/Numeric/Decimal.def"
TEMPLATE_INSTANTIATE(Numeric, Bigint, 192, uint64_t);
TEMPLATE_INSTANTIATE(Numeric, Decimal, 192, uint64_t);

#define SUFFIX MyF64
#define TYPE double
#include "../../Cosmeron/Modules/Numeric/Complex.def"

/* --- Text (Mode 1 & Mode 2) --- */
#define SUFFIX 8
#define TYPE char
#include "../../Cosmeron/Modules/Text/View/View.def"

#define SUFFIX 8
#define TYPE char
#include "../../Cosmeron/Modules/Text/String/String.def"

/* --- Math (Mode 1 & Mode 2) --- */
#define SUFFIX MyF64
#define TYPE double
#include "../../Cosmeron/Modules/Math/Value/Clamp.def"

#include "../../Cosmeron/Modules/Math/Equation/Linear.def"
#include "../../Cosmeron/Modules/Math/Equation/Quadratic.def"
TEMPLATE_INSTANTIATE(Math, LinearEquation, MyF64, double);
TEMPLATE_INSTANTIATE(Math, QuadraticEquation, MyF64, double);

int main(void) {
  TTuple_Pair_MyI32 p = {.first = 10, .second = 20};
  assert(p.first == 10 && p.second == 20);

  TArray_Vector_MyI32 vec = {0};
  assert(Array_Vector_MyI32_Create(&vec, 2U) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&vec, PushBack, 42) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&vec, Size) == 1U);
  assert(OBJECT_CALL_0(&vec, Dispose) == STATUS_CONST(SUCCESS));

  TLinked_Stack_MyI32 lstack = {0};
  assert(Linked_Stack_MyI32_Create(&lstack) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&lstack, Push, 99) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&lstack, Size) == 1U);
  assert(OBJECT_CALL_0(&lstack, Dispose) == STATUS_CONST(SUCCESS));

  THash_Set_MyI32 hset = {0};
  assert(Hash_Set_MyI32_Create(&hset) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&hset, Insert, 5) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&hset, Contains, 5));
  assert(OBJECT_CALL_0(&hset, Dispose) == STATUS_CONST(SUCCESS));

  TTree_BST_Set_MyI32 bst = {0};
  assert(Tree_BST_Set_MyI32_Create(&bst) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&bst, Insert, 7) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&bst, Contains, 7));
  assert(OBJECT_CALL_0(&bst, Dispose) == STATUS_CONST(SUCCESS));

  TTree_B_Set_MyI32 bt = {0};
  assert(Tree_B_Set_MyI32_Create(&bt) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&bt, Insert, 11) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&bt, Contains, 11));
  assert(OBJECT_CALL_0(&bt, Dispose) == STATUS_CONST(SUCCESS));

  TGraph_MyI32 g = {0};
  assert(Graph_MyI32_Create(&g) == STATUS_CONST(SUCCESS));
  size_t v0 = 0U;
  assert(OBJECT_CALL(&g, AddVertex, 1, &v0) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&g, Dispose) == STATUS_CONST(SUCCESS));

  TNumeric_Bigint192 bi = {0};
  assert(Numeric_Bigint192_Init(&bi) == STATUS_CONST(SUCCESS));
  bi.limb[0] = 42ULL;
  assert(!OBJECT_CALL_0(&bi, IsZero));

  TText_View8 vw = Text_View8_FromCStr("hello");
  assert(Text_View8_Length(vw) == 5U);
  TText_String8 str = {0};
  assert(Text_String8_FromCStr(&str, "ok") == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&str, Length) == 2U);
  assert(OBJECT_CALL_0(&str, Destroy) == STATUS_CONST(SUCCESS));

  assert(Math_Clamp_MyF64_Apply(15.0, 0.0, 10.0) == 10.0);

  printf("Container StandaloneDef tests passed!\n");
  return 0;
}
