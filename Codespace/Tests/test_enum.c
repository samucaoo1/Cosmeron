#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "Codespace/Cosmeron/Core/Outcome/Status.h"
#include "Codespace/Cosmeron/Core/Module/Module.h"

/* Define sample X-Macro tables */
#define TEST_SEEK_TABLE(X) \
  X((SET, 0)) \
  X((CUR, 1)) \
  X((END, 2))

#define TEST_COLOR_TABLE(X) \
  X((RED)) \
  X((GREEN)) \
  X((BLUE))

#define TEST_STATE_TABLE(X) \
  X((IDLE, 10, "Idle state")) \
  X((RUNNING, 20, "Running state")) \
  X((STOPPED, 30, "Stopped state"))

/* Custom dispatch macro: left-shifts the value */
#define SHIFT_DISPATCH(TARGET, ENUM_NAME, ROW) \
  CNS(TARGET, ENUM_NAME, PP_SEQ_GET_1 ROW) = (1U << PP_SEQ_GET_2 ROW),

#define TEST_FLAGS_TABLE(X) \
  X((READ, 0)) \
  X((WRITE, 1)) \
  X((EXEC, 2))

/* Declare a module demonstrating ENUM and ENUM_EX across MODULE, PACKAGE, OBJECT */
MODULE(DOMAIN(Core), Demo,
  /* 1. Top-level MODULE enums */
  ENUM(Seek, TEST_SEEK_TABLE),
  ENUM_EX(Color, TEST_COLOR_TABLE, QUALIFIED),
  ENUM_EX(Flags, TEST_FLAGS_TABLE, SHIFT_DISPATCH),

  /* 2. ENUM inside PACKAGE */
  PACKAGE(Stream,
    ENUM(Seek, TEST_SEEK_TABLE),
    ENUM_EX(QualifiedSeek, TEST_SEEK_TABLE, QUALIFIED),

    FUNCTION_INLINE(int, GetCur, (void), {
      return Demo_Stream_CUR;
    }),

    FUNCTION_INLINE(int, GetQualifiedEnd, (void), {
      return Demo_Stream_QualifiedSeek_END;
    })
  ),

  /* 3. ENUM inside OBJECT */
  OBJECT(Machine,
    FIELDS(
      Demo_Machine_State state;
    ),
    ENUM(State, TEST_STATE_TABLE),
    ENUM_EX(QualifiedState, TEST_STATE_TABLE, QUALIFIED),

    OPERATION_INLINE(InitMachine, (), {
      self->state = Demo_Machine_IDLE;
      return STATUS_CONST(SUCCESS);
    }),

    PREDICATE_INLINE(IsRunning, (), {
      return self->state == Demo_Machine_RUNNING;
    }),

    PROCEDURE_INLINE(SetRunning, (), {
      self->state = Demo_Machine_RUNNING;
    })
  )
);

TEMPLATE_OBJECT(Demo, Container, int,
  FIELDS(
    int value;
    NS(Demo_Container, int, State) state;
  ),
  ENUM(State, TEST_STATE_TABLE),

  OPERATION_INLINE(SetVal, (int v), {
    self->value = v;
    self->state = NS(Demo_Container, int, RUNNING);
    return STATUS_CONST(SUCCESS);
  })
);

int main(void) {
  OPSTATUS status;

  /* 1. Top-level MODULE enums */
  assert(Demo_SET == 0);
  assert(Demo_CUR == 1);
  assert(Demo_END == 2);

  assert(Demo_Color_RED == 0);
  assert(Demo_Color_GREEN == 1);
  assert(Demo_Color_BLUE == 2);

  assert(Demo_Flags_READ == (1U << 0));
  assert(Demo_Flags_WRITE == (1U << 1));
  assert(Demo_Flags_EXEC == (1U << 2));

  /* 2. PACKAGE enums */
  assert(Demo_Stream_SET == 0);
  assert(Demo_Stream_CUR == 1);
  assert(Demo_Stream_END == 2);
  assert(Demo_Stream_GetCur() == 1);

  assert(Demo_Stream_QualifiedSeek_SET == 0);
  assert(Demo_Stream_QualifiedSeek_CUR == 1);
  assert(Demo_Stream_QualifiedSeek_END == 2);
  assert(Demo_Stream_GetQualifiedEnd() == 2);

  /* 3. OBJECT enums and field usage */
  assert(Demo_Machine_IDLE == 10);
  assert(Demo_Machine_RUNNING == 20);
  assert(Demo_Machine_STOPPED == 30);

  assert(Demo_Machine_QualifiedState_IDLE == 10);
  assert(Demo_Machine_QualifiedState_RUNNING == 20);

  /* 4. TEMPLATE_OBJECT enum */
  assert(Demo_Container_int_IDLE == 10);
  assert(Demo_Container_int_RUNNING == 20);

  MODULE_TYPED(Demo, Container, int) container;
  status = Demo_Container_int_Init(&container);
  assert(status == STATUS_CONST(SUCCESS));
  container.state = Demo_Container_int_IDLE;
  assert(container.state == 10);

  MODULE_TYPE(Demo, Machine) machine;
  status = Demo_Machine_Init(&machine);
  assert(status == STATUS_CONST(SUCCESS));

#if defined(MODULE_DISABLE_FUNCTION_TABLE) || defined(OBJECT_DISABLE_FUNCTION_TABLE)
  status = Demo_Machine_InitMachine(&machine);
  assert(status == STATUS_CONST(SUCCESS));
  assert(machine.state == Demo_Machine_IDLE);
  assert(!Demo_Machine_IsRunning(&machine));

  Demo_Machine_SetRunning(&machine);
  assert(machine.state == Demo_Machine_RUNNING);
  assert(Demo_Machine_IsRunning(&machine));
#else
  status = OBJECT_CALL_0(&machine, InitMachine);
  assert(status == STATUS_CONST(SUCCESS));
  assert(machine.state == Demo_Machine_IDLE);
  assert(!OBJECT_CALL_0(&machine, IsRunning));

  OBJECT_CALL_0(&machine, SetRunning);
  assert(machine.state == Demo_Machine_RUNNING);
  assert(OBJECT_CALL_0(&machine, IsRunning));
#endif

  printf("All ENUM and ENUM_EX tests passed successfully!\n");
  return 0;
}
