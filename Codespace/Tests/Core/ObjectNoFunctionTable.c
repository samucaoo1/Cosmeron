#define OBJECT_DISABLE_FUNCTION_TABLE
#include "../../Cosmeron/Core/Object/Object.h"
#include <assert.h>

/* 1. Alternativa B without function table */
OBJECT(TMiniObject,
    FIELDS(
        int val;
    ),
    METHOD(void, Execute, (int bonus))
);

void TMiniObject_Execute(TMiniObject *self, int bonus) {
  self->val = 100 + bonus;
}

/* 2. Legacy form without function table */
#define TMini_Run_PROTOTYPE void TMini_Run(TMini *self)

#define TMini_METHODS(M, ...)                                                  \
  M(Run, __VA_ARGS__)

OBJECT_STRUCT(TMini,
  int val;
);

OBJECT_PROTOTYPE(TMini, Run) {
  self->val = 42;
}

OBJECT_IMPLEMENT(TMini);

int main(void) {
  /* Test Alternativa B */
  TMiniObject obj = {0};
  assert(TMiniObject_Init(&obj) == STATUS_CONST(SUCCESS));
  TMiniObject_Execute(&obj, 5);
  assert(obj.val == 105);

  /* Test Legacy */
  TMini mini = {0};
  assert(TMini_Init(&mini) == STATUS_CONST(SUCCESS));
  TMini_Run(&mini);
  assert(mini.val == 42);

  return 0;
}

