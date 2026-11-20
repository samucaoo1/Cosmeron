#include "../../Cosmeron/Core/Algorithm/Comparison.h"
#include "../../Cosmeron/Core/Condition/Status.h"
#include "../../Cosmeron/Core/Module/Module.h"

#include <stdio.h>

MODULE(DOMAIN(Core), Greeter,
    PACKAGE(Message,
        PROCEDURE_INLINE(PrintHello, (const char *name), {
            printf("Hello from %s!\n", name ? name : "Cosmeron");
        })
    )
);

OBJECT_DEF(Counter,
    FIELDS(
        int32_t value;
    ),
    EVENT_PROCEDURE(Changed, ((int32_t, oldValue), (int32_t, newValue))),
    OPERATION_INLINE(SetValue, (int32_t next), {
        if (!self) {
            return STATUS_CONST(INVALID_ARGUMENT);
        }
        OBJECT_MUTATE(self, Counter, value, next, Changed);
        return STATUS_CONST(SUCCESS);
    }),
    CONST_FUNCTION_INLINE(int32_t, Value, (), {
        return self ? self->value : 0;
    })
);

OBJECT_IMPLEMENT(Counter);

static void ExampleApp_OnChanged(int32_t oldValue, int32_t newValue,
                                 void *context) {
  int *notifications = (int *)context;
  if (notifications) {
    (*notifications)++;
  }
  printf("Counter changed: %d -> %d\n", (int)oldValue, (int)newValue);
}

int main(void) {
  MODULE_CALL(Greeter, Message, PrintHello, ("Cosmeron Module DSL"));

  Counter counter = {0};
  (void)Counter_Init(&counter);

  int notifications = 0;
  TCLOSURE(Counter_Changed) onChanged = NS_JOIN(
      CLOSURE_FUNC(Counter_Changed), Bind)(ExampleApp_OnChanged, &notifications);
  (void)OBJECT_CALL(&counter, Changed_Subscribe, onChanged);

  (void)OBJECT_CALL(&counter, SetValue, 10);
  (void)OBJECT_CALL(&counter, SetValue, 25);

  printf("Final value=%d (notifications=%d)\n",
         (int)OBJECT_CALL_0(&counter, Value), notifications);

  return 0;
}
