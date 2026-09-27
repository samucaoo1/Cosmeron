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

enum {
  Preprocessor_Test_Cat = PP_OP_CAT(TEST_, ONE),
  Preprocessor_Test_Seq = PP_SEQ_GET_2(1, 2, 3),
  Preprocessor_Test_Foreach =
      0 PP_FOREACH(PREPROCESSOR_TEST_COUNT_ITEM, a, b, c, d),
  Preprocessor_Test_Map =
      0 PP_MAP(PREPROCESSOR_TEST_COUNT_ITEM, a, b, c),
  Preprocessor_Test_Repeat =
      0 PP_REPEAT(3, PREPROCESSOR_TEST_REPEAT_ITEM),
  Preprocessor_Test_IfElseTrue = PP_BOOL_IF_ELSE(7)(11)(22),
  Preprocessor_Test_IfElseFalse = PP_BOOL_IF_ELSE(0)(11)(22),
  Preprocessor_Test_While =
      PP_WHILE(PREPROCESSOR_TEST_WHILE_PRED, PREPROCESSOR_TEST_WHILE_OP, 3)
};

int main(void) {
  return Preprocessor_Test_Cat == 1 && Preprocessor_Test_Seq == 2 &&
                 Preprocessor_Test_Foreach == 4 &&
                 Preprocessor_Test_Map == 3 &&
                 Preprocessor_Test_Repeat == 6 &&
                 Preprocessor_Test_IfElseTrue == 11 &&
                 Preprocessor_Test_IfElseFalse == 22 &&
                 Preprocessor_Test_While == 0
             ? 0
             : 1;
}
