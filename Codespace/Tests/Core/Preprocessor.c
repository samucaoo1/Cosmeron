#include "../../Cosmeron/Core/Namespace.h"
#include "../../Cosmeron/Core/Preprocessor/Arguments.h"
#include "../../Cosmeron/Core/Preprocessor/Boolean.h"
#include "../../Cosmeron/Core/Preprocessor/Eval.h"
#include "../../Cosmeron/Core/Preprocessor/Foreach.h"
#include "../../Cosmeron/Core/Preprocessor/Map.h"
#include "../../Cosmeron/Core/Preprocessor/Operations.h"
#include "../../Cosmeron/Core/Preprocessor/Repeat.h"
#include "../../Cosmeron/Core/Preprocessor/Sequence.h"
#include "../../Cosmeron/Core/Preprocessor/Tokens.h"
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

#if PP_BOOL(0) != 0 || PP_BOOL(1) != 1 || PP_BOOL(7) != 1
#error "PP_BOOL failed"
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
      PP_WHILE(PREPROCESSOR_TEST_WHILE_PRED, PREPROCESSOR_TEST_WHILE_OP, 3)
};

#define PREPROCESSOR_TEST_NESTED_INNER(x) +(x)
#define PREPROCESSOR_TEST_NESTED_OUTER(row)                                    \
  +(0 PP_MAP_NONE(PREPROCESSOR_TEST_NESTED_INNER, PP_UNWRAP row))

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

  return Preprocessor_Test_Cat == 1 && Preprocessor_Test_Cat4 == 1234 &&
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
                 mapCtxArray[0] == 101 && mapCtxArray[1] == 102 && mapCtxArray[2] == 103 &&
                 nestedSum == 10
             ? 0
             : 1;
}

