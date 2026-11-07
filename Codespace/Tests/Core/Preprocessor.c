#include "../../Cosmeron/Core/Module/Namespace.h"
#include "../../Cosmeron/Core/Preprocessor/Arguments.h"
#include "../../Cosmeron/Core/Preprocessor/Boolean.h"
#include "../../Cosmeron/Core/Preprocessor/Eval.h"
#include "../../Cosmeron/Core/Preprocessor/Foreach.h"
#include "../../Cosmeron/Core/Preprocessor/Map.h"
#include "../../Cosmeron/Core/Preprocessor/Operations.h"
#include "../../Cosmeron/Core/Preprocessor/Repeat.h"
#include "../../Cosmeron/Core/Preprocessor/Sequence.h"
#include "../../Cosmeron/Core/Preprocessor/Tokens.h"
#include "../../Cosmeron/Core/Preprocessor/Tree.h"
#include "../../Cosmeron/Core/Preprocessor/While.h"

#define TEST_ONE 1
#define TEST_TWO 2

#define PREPROCESSOR_TEST_COUNT_ITEM(item) +1
#define PREPROCESSOR_TEST_REPEAT_ITEM(index) +(index)
#define PREPROCESSOR_TEST_WHILE_PRED(state) PP_BOOL(state)
#define PREPROCESSOR_TEST_WHILE_OP(state) PP_DEC(state)

#if PP_ARG_COUNT(a) != 1 || PP_ARG_COUNT(a, b, c) != 3
#error "PP_ARG_COUNT failed"
#endif

#if PP_IS_EMPTY() != 1 || PP_IS_EMPTY(foo) != 0
#error "PP_IS_EMPTY failed"
#endif

#if PP_IS_EMPTY_PARAMS(()) != 1 || PP_IS_EMPTY_PARAMS((int a)) != 0
#error "PP_IS_EMPTY_PARAMS failed"
#endif

#if PP_IIF(1, 10, 20) != 10 || PP_IIF(0, 10, 20) != 20
#error "PP_IIF failed"
#endif

#if PP_IF(5, 10, 20) != 10 || PP_IF(0, 10, 20) != 20
#error "PP_IF failed"
#endif

#if PP_BOOL_NOT(0) != 1 || PP_BOOL_NOT(1) != 0 || PP_BOOL_NOT(7) != 0
#error "PP_BOOL_NOT failed"
#endif

#if PP_BOOL(0) != 0 || PP_BOOL(1) != 1 || PP_BOOL(7) != 1 || PP_BOOL(256) != 1
#error "PP_BOOL failed"
#endif

#if PP_DEC(0) != 0 || PP_DEC(1) != 0 || PP_DEC(200) != 199 || PP_DEC(256) != 255
#error "PP_DEC failed"
#endif

#if PP_INC(0) != 1 || PP_INC(99) != 100 || PP_INC(255) != 256 || PP_INC(256) != 256
#error "PP_INC failed"
#endif

#if PP_BOOL_AND(1, 1) != 1 || PP_BOOL_AND(7, 2) != 1
#error "PP_BOOL_AND failed"
#endif

#if PP_BOOL_OR(0, 1) != 1 || PP_BOOL_OR(0, 9) != 1
#error "PP_BOOL_OR failed"
#endif

#define PREPROCESSOR_TEST_CTX_ITEM(ctx, item) +(ctx + item)

enum {
  Preprocessor_Test_Cat = PP_OP_CAT(TEST_, ONE),
  Preprocessor_Test_CatVariadic = PP_OP_CAT(1, 2, 3, 4, 5),
  Preprocessor_Test_Cat4 = PP_OP_CAT4(1, 2, 3, 4),
  Preprocessor_Test_Seq = PP_SEQ_GET_2(1, 2, 3),
  Preprocessor_Test_Foreach =
      0 PP_FOREACH(PREPROCESSOR_TEST_COUNT_ITEM, a, b, c, d),
  Preprocessor_Test_ForeachCtx =
      0 PP_FOREACH_CTX(PREPROCESSOR_TEST_CTX_ITEM, 10, 1, 2, 3),
  Preprocessor_Test_Map =
      0 PP_MAP_NONE(PREPROCESSOR_TEST_COUNT_ITEM, a, b, c),
  Preprocessor_Test_Repeat =
      0 PP_REPEAT(3, PREPROCESSOR_TEST_REPEAT_ITEM),
  Preprocessor_Test_IfElseTrue = PP_BOOL_IF_ELSE(7)(11)(22),
  Preprocessor_Test_IfElseFalse = PP_BOOL_IF_ELSE(0)(11)(22),
  Preprocessor_Test_While =
      PP_WHILE(PREPROCESSOR_TEST_WHILE_PRED, PREPROCESSOR_TEST_WHILE_OP, 3),
  Preprocessor_Test_TreeCat = PP_TREE_CAT(1, 2, 3, 4),
  Preprocessor_Test_SeqHead = PP_SEQ_HEAD((10)(20)(30)),
  Preprocessor_Test_SeqSize = PP_SEQ_SIZE((a)(b)(c)(d)(e))
};

#define PREPROCESSOR_TEST_NESTED_INNER(x) +(x)
#define PREPROCESSOR_TEST_NESTED_OUTER(row)                                    \
  +(0 PP_MAP_NONE(PREPROCESSOR_TEST_NESTED_INNER, PP_UNWRAP row))

/* 5-level nested Foreach tests (Dimension 1 -> Dimension 2 -> Dimension 3 -> Dimension 4 -> Dimension 5) */
#define PREPROCESSOR_TEST_D5_ITEM(ctx, x) +(ctx + x)
#define PREPROCESSOR_TEST_D4_ITEM(ctx, x)                                      \
  +(PP_FOREACH_D5_CTX(PREPROCESSOR_TEST_D5_ITEM, ctx + x, 1))
#define PREPROCESSOR_TEST_D3_ITEM(ctx, x)                                      \
  +(PP_FOREACH_D4_CTX(PREPROCESSOR_TEST_D4_ITEM, ctx + x, 1))
#define PREPROCESSOR_TEST_D2_ITEM(ctx, x)                                      \
  +(PP_FOREACH_D3_CTX(PREPROCESSOR_TEST_D3_ITEM, ctx + x, 1))
#define PREPROCESSOR_TEST_D1_ITEM(ctx, x)                                      \
  +(PP_FOREACH_D2_CTX(PREPROCESSOR_TEST_D2_ITEM, ctx + x, 10, 20))

#define PREPROCESSOR_TEST_CHUNK_ITEM(ctx, x) +(ctx + x)

int main(void) {
  int repeatCount = 0;
  REPEAT(5) {
    repeatCount++;
  }

  int sumIndex = 0;
  REPEAT_INDEX(idx, 4) {
    sumIndex += (int)idx;
  }

  /* Test functional comma-separated PP_MAP */
  int mapArray[] = {PP_MAP(PREPROCESSOR_TEST_REPEAT_ITEM, 10, 20, 30)};
  int mapCtxArray[] = {
      PP_MAP_CTX(PREPROCESSOR_TEST_CTX_ITEM, 100, 1, 2, 3)};

  /* Test Motor 2 (PP_MAP) nested inside Motor 1 (PP_FOREACH) */
  int nestedSum = 0 PP_FOREACH(PREPROCESSOR_TEST_NESTED_OUTER, (1, 2), (3, 4));

  /* Test Dimensional Foreach Engine (D1, D2, D3, D4, D5) */
  int d2Array[] = {PP_FOREACH_D2_COMMA(PREPROCESSOR_TEST_REPEAT_ITEM, 40, 50)};
  int d3Array[] = {
      PP_FOREACH_COMMA_D(3, PREPROCESSOR_TEST_REPEAT_ITEM, 60, 70)};
  int d4Array[] = {
      PP_FOREACH_D4_COMMA(PREPROCESSOR_TEST_REPEAT_ITEM, 80, 90)};
  int d5Array[] = {
      PP_FOREACH_COMMA_D(5, PREPROCESSOR_TEST_REPEAT_ITEM, 100, 110)};

  int fiveLevelSum =
      0 + PP_FOREACH_CTX(PREPROCESSOR_TEST_D1_ITEM, 0, 100, 200);

  /* Test Chunk Engine */
  int chunkSum = 0 + PP_FOREACH_CHUNKS_CTX(PREPROCESSOR_TEST_CHUNK_ITEM, 10,
      (1, 2),
      (3, 4),
      (5, 6)
  );

  int chunkArray[] = {
      PP_FOREACH_CHUNKS_COMMA(PREPROCESSOR_TEST_REPEAT_ITEM,
          (100, 200),
          (300, 400)
      )
  };

  /* Test 3D and 4D Foreach Chunks */
  int foreachChunk3DSum = 0 PP_FOREACH_CHUNKS_3D(PREPROCESSOR_TEST_COUNT_ITEM,
      ((a, b), (c, d)),
      ((e, f), (g, h))
  );

  int foreachChunk4DSum = 0 PP_FOREACH_CHUNKS_4D(PREPROCESSOR_TEST_COUNT_ITEM,
      ( ((a, b), (c, d)), ((e, f), (g, h)) ),
      ( ((i, j), (k, l)), ((m, n), (o, p)) )
  );

#define PREPROCESSOR_TEST_OP_ADD(a, b) (a + b)
#define PREPROCESSOR_TEST_OP_SUB(a, b) (a - b)

  int PP_TREE_JOIN(_, tree, var, one) = 111;
  int PP_TREE_JOIN_UNDERSCORE(tree, var, two) = 222;
  int treeReduceSum =
      PP_TREE_REDUCE(PREPROCESSOR_TEST_OP_ADD, 1, 2, 3, 4, 5, 6, 7, 8);

  /* Test 2D, 3D, and 4D Tree Chunks */
  int treeChunks2DSum =
      PP_TREE_REDUCE_CHUNKS_2D(PREPROCESSOR_TEST_OP_ADD, (1, 2), (3, 4));
  int treeChunks3DSum = PP_TREE_REDUCE_CHUNKS_3D(PREPROCESSOR_TEST_OP_ADD,
      ((1, 2), (3, 4)),
      ((5, 6), (7, 8))
  );
  int treeChunks4DSum = PP_TREE_REDUCE_CHUNKS_4D(PREPROCESSOR_TEST_OP_ADD,
      ( ((1, 2), (3, 4)), ((5, 6), (7, 8)) ),
      ( ((9, 10), (11, 12)), ((13, 14), (15, 16)) )
  );

  int PP_OP_CAT32(
      a, b, c, d, e, f, g, h,
      i, j, k, l, m, n, o, p,
      q, r, s, t, u, v, w, x,
      y, z, 0, 1, 2, 3, 4, 5
  ) = 32;

  int seqFoldLeft = PP_SEQ_FOLD_LEFT(PREPROCESSOR_TEST_OP_SUB, 100, (1)(2)(3));
  int seqFoldRight =
      PP_SEQ_FOLD_RIGHT(PREPROCESSOR_TEST_OP_SUB, 100, (1)(2)(3));
  int seqTupleArray[] = {PP_SEQ_ENUM((10)(20)(30))};
  int seqForeachSum =
      0 PP_SEQ_FOREACH(PREPROCESSOR_TEST_COUNT_ITEM, (a)(b)(c)(d));
  int seqForeachCtxSum =
      0 PP_SEQ_FOREACH_CTX(PREPROCESSOR_TEST_CTX_ITEM, 10, (1)(2)(3));

  return Preprocessor_Test_Cat == 1 &&
         Preprocessor_Test_CatVariadic == 12345 &&
         Preprocessor_Test_Cat4 == 1234 &&
         abcdefghijklmnopqrstuvwxyz012345 == 32 &&
                 Preprocessor_Test_TreeCat == 1234 &&
                 Preprocessor_Test_SeqHead == 10 &&
                 Preprocessor_Test_SeqSize == 5 &&
                 tree_var_one == 111 && tree_var_two == 222 &&
                 treeReduceSum == 36 &&
                 foreachChunk3DSum == 8 && foreachChunk4DSum == 16 &&
                 treeChunks2DSum == 10 && treeChunks3DSum == 36 &&
                 treeChunks4DSum == 136 &&
                 seqFoldLeft == 94 && seqFoldRight == -98 &&
                 seqTupleArray[0] == 10 && seqTupleArray[1] == 20 &&
                 seqTupleArray[2] == 30 &&
                 seqForeachSum == 4 && seqForeachCtxSum == 36 &&
                 Preprocessor_Test_Seq == 2 &&
                 Preprocessor_Test_Foreach == 4 &&
                 Preprocessor_Test_ForeachCtx == 36 &&
                 Preprocessor_Test_Map == 3 &&
                 Preprocessor_Test_Repeat == 6 &&
                 Preprocessor_Test_IfElseTrue == 11 &&
                 Preprocessor_Test_IfElseFalse == 22 &&
                 Preprocessor_Test_While == 0 &&
                 repeatCount == 5 &&
                 sumIndex == 6 &&
                 mapArray[0] == 10 && mapArray[1] == 20 && mapArray[2] == 30 &&
                 mapCtxArray[0] == 101 && mapCtxArray[1] == 102 &&
                 mapCtxArray[2] == 103 &&
                 nestedSum == 10 &&
                 d2Array[0] == 40 && d2Array[1] == 50 &&
                 d3Array[0] == 60 && d3Array[1] == 70 &&
                 d4Array[0] == 80 && d4Array[1] == 90 &&
                 d5Array[0] == 100 && d5Array[1] == 110 &&
                 fiveLevelSum == 672 &&
                 chunkSum == 81 &&
                 chunkArray[0] == 100 && chunkArray[1] == 200 &&
                 chunkArray[2] == 300 && chunkArray[3] == 400
             ? 0
             : 1;
}

