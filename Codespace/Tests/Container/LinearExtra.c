#include "Test.h"
#include "../../Cosmeron/Modules/Container/Array/Vector.h"
#include "../../Cosmeron/Modules/Container/Array/Queue.h"
#include "../../Cosmeron/Modules/Container/Array/Stack.h"
#include "../../Cosmeron/Modules/Container/Linked/Queue.h"
#include "../../Cosmeron/Modules/Container/Linked/Stack.h"

int main(void){
  TVector(int,v)
  TEST_ASSERT(Container_Flat_Vector_int_Reserve(&v,32U)==STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Vector_int_Capacity(&v)>=32U);
  TEST_ASSERT(Container_Flat_Vector_int_PushFront(&v,2)==STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Flat_Vector_int_PushFront(&v,1)==STATUS_CONST(SUCCESS));
  int x=0;
  TEST_ASSERT(Container_Flat_Vector_int_PopFront(&v,&x)==STATUS_CONST(SUCCESS) && x==1);
  TEST_ASSERT(Container_Flat_Vector_int_ShrinkToFit(&v)==STATUS_CONST(SUCCESS));
  Container_Flat_Vector_int_Destroy(&v);

  TLinked_Queue(int,q)
  TEST_ASSERT(Container_Linked_Queue_int_Push(&q,4)==STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Linked_Queue_int_Push(&q,5)==STATUS_CONST(SUCCESS));
  int *front = NULL;
  int *back = NULL;
  TEST_ASSERT(Container_Linked_Queue_int_Front(&q,&front)==STATUS_CONST(SUCCESS) && *front==4);
  TEST_ASSERT(Container_Linked_Queue_int_Back(&q,&back)==STATUS_CONST(SUCCESS) && *back==5);
  TEST_ASSERT(Container_Linked_Queue_int_Pop(&q,&x)==STATUS_CONST(SUCCESS) && x==4);
  Container_Linked_Queue_int_Clear(&q);
  TEST_ASSERT(Container_Linked_Queue_int_Empty(&q));
  Container_Linked_Queue_int_Destroy(&q);

  TLinked_Stack(int,s)
  TEST_ASSERT(Container_Linked_Stack_int_Push(&s,7)==STATUS_CONST(SUCCESS));
  TEST_ASSERT(Container_Linked_Stack_int_Push(&s,8)==STATUS_CONST(SUCCESS));
  int *top = NULL;
  TEST_ASSERT(Container_Linked_Stack_int_Top(&s,&top)==STATUS_CONST(SUCCESS) && *top==8);
  TEST_ASSERT(Container_Linked_Stack_int_Pop(&s,&x)==STATUS_CONST(SUCCESS) && x==8);
  Container_Linked_Stack_int_Clear(&s);
  TEST_ASSERT(Container_Linked_Stack_int_Empty(&s));
  Container_Linked_Stack_int_Destroy(&s);
  return 0;
}
