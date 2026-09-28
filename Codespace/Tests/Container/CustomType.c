#include "Test.h"
#include "../../Cosmeron/Modules/Container/Tree/AVL.h"

typedef struct TContainerTestRecord {
  int key;
  int payload;
} TContainerTestRecord;

static CMPOUT ContainerTest_Record_Compare(TContainerTestRecord left,
                                           TContainerTestRecord right) {
  if (left.key < right.key)
    return COMPARISON_CONST(LOWER);
  if (left.key > right.key)
    return COMPARISON_CONST(HIGHER);
  return COMPARISON_CONST(EQUAL);
}

TREE_AVL_SET_IMPLEMENT_ALL_CMP(record, TContainerTestRecord,
                                  ContainerTest_Record_Compare)

int main(void) {
  TTree_AVL_Set(record, records);
  for (int key = 63; key >= 0; --key) {
    TContainerTestRecord record = {.key = key, .payload = key * 2};
    TEST_ASSERT(Container_Tree_AVL_Set_record_Insert(&records, record) ==
                STATUS_CONST(SUCCESS));
  }
  TContainerTestRecord query = {.key = 17, .payload = 0};
  TContainerTestRecord *found = Container_Tree_AVL_Set_record_Find(&records, query);
  TEST_ASSERT(found != NULL && found->payload == 34);
  Container_Tree_AVL_Set_record_Destroy(&records);
  return 0;
}

