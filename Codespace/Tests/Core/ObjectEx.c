#include "../../Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <string.h>

/* ============================================================
 * Generic Stack Template using the new Core/Module architecture
 * ============================================================ */

#define TEMPLATE_TSTACK(SUFFIX, TYPE)                                          \
  OBJECT(                                                                      \
      GNS2(TStack, SUFFIX),                                                    \
      FIELDS(                                                                  \
          TYPE items[16];                                                      \
          size_t count;                                                        \
      ),                                                                       \
      OPERATION(Create, ()),                                                   \
      OPERATION(Push, (TYPE value)),                                           \
      OPERATION(Pop, (TYPE *outValue)),                                        \
      CONST_FUNCTION_INLINE(size_t, Size, (), {                                \
        return self ? self->count : 0U;                                        \
      }),                                                                      \
      CONST_PREDICATE_INLINE(IsEmpty, (), {                                    \
        return self ? self->count == 0U : true;                                \
      })                                                                       \
  )

#define TStack_int_IS_OBJECT 1

#define TEMPLATE_TSTACK_IMPLEMENT(SUFFIX, TYPE)                                \
  OPERATION_IMPLEMENT_EX(GNS2(TStack, SUFFIX), Create, ()) {                   \
    if (!self)                                                                 \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    self->count = 0U;                                                          \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  OPERATION_IMPLEMENT_EX(GNS2(TStack, SUFFIX), Push, (TYPE value)) {           \
    if (!self || self->count >= 16U)                                           \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    self->items[self->count++] = value;                                        \
    return STATUS_CONST(SUCCESS);                                              \
  }                                                                            \
  OPERATION_IMPLEMENT_EX(GNS2(TStack, SUFFIX), Pop, (TYPE *outValue)) {        \
    if (!self || !outValue || self->count == 0U)                               \
      return STATUS_CONST(INVALID_ARGUMENT);                                   \
    *outValue = self->items[--self->count];                                    \
    return STATUS_CONST(SUCCESS);                                              \
  }

/* Instantiate TStack_int via unified TEMPLATE_IMPLEMENT */
TEMPLATE_IMPLEMENT(TSTACK, int, int);

int main(void) {
  TStack_int stack;
  assert(TStack_int_Init(&stack) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&stack, Create) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&stack, IsEmpty) == true);
  assert(OBJECT_CALL_0(&stack, Size) == 0U);

  assert(OBJECT_CALL(&stack, Push, 42) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&stack, Push, 100) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&stack, Size) == 2U);
  assert(OBJECT_CALL_0(&stack, IsEmpty) == false);

  int val = 0;
  assert(OBJECT_CALL(&stack, Pop, &val) == STATUS_CONST(SUCCESS));
  assert(val == 100);
  assert(OBJECT_CALL(&stack, Pop, &val) == STATUS_CONST(SUCCESS));
  assert(val == 42);
  assert(OBJECT_CALL_0(&stack, IsEmpty) == true);

  return 0;
}
