#include "../../Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* ========================================================================== */
/* 1. Declarative Object with Events                                          */
/* ========================================================================== */

OBJECT(Button,
    FIELDS(
        int id;
    ),
    /* Default capacity: 4 slots, zero parameter duplication */
    EVENT_PROCEDURE(Click, ((int, mouseButton), (int, x), (int, y))),
    /* Explicit capacity 6 -> decays to 8 slots, zero parameter duplication */
    EVENT_PROCEDURE(Hover, ((int, x), (int, y)), 6),
    /* Nullary event (no arguments), default capacity 4 */
    EVENT_PROCEDURE(Reset, ()),
    /* Fallible event returning OPSTATUS, zero parameter duplication */
    EVENT_OPERATION(Validate, ((int, code)), 4)
);

OBJECT_IMPLEMENT(Button);

/* ========================================================================== */
/* 2. Declarative Object with Reactive Mutation (OBJECT_MUTATE)               */
/* ========================================================================== */

OBJECT(Slider,
    FIELDS(
        int value;
    ),
    EVENT_PROCEDURE(OnValueChanged, ((int, oldValue), (int, newValue)), 4),
    OPERATION_INLINE(SetValue, (int newValue), {
        if (!self) return STATUS_CONST(INVALID_ARGUMENT);
        OBJECT_MUTATE(self, Slider, value, newValue, OnValueChanged);
        return STATUS_CONST(SUCCESS);
    })
);

OBJECT_IMPLEMENT(Slider);

/* ========================================================================== */
/* 3. Declarative Package with Events                                         */
/* ========================================================================== */

PACKAGE(Bus,
    EVENT_PROCEDURE(OnMessage, ((int, topic), (int, payload)), 4),
    EVENT_OPERATION(OnFilter, ((int, value)), 4)
);

PACKAGE_IMPLEMENT(Bus);

/* ========================================================================== */
/* Handlers & State for Tests                                                */
/* ========================================================================== */

static int g_click_sum = 0;
static void OnButtonClick(int button, int x, int y, void *context) {
  (void)context;
  g_click_sum += button + x + y;
}

static int g_hover_count = 0;
static void OnButtonHover(int x, int y, void *context) {
  (void)x; (void)y; (void)context;
  g_hover_count++;
}

static int g_reset_count = 0;
static void OnButtonReset(void *context) {
  (void)context;
  g_reset_count++;
}

static int g_validate_calls = 0;
static OPSTATUS OnValidatePass(int code, void *context) {
  (void)code; (void)context;
  g_validate_calls++;
  return STATUS_CONST(SUCCESS);
}

static OPSTATUS OnValidateFail(int code, void *context) {
  (void)code; (void)context;
  g_validate_calls++;
  return STATUS_CONST(INVALID_ARGUMENT);
}

static OPSTATUS OnValidateUnreachable(int code, void *context) {
  (void)code; (void)context;
  g_validate_calls++;
  return STATUS_CONST(SUCCESS);
}

static int g_bus_topic = 0;
static int g_bus_payload = 0;
static void OnBusMessage(int topic, int payload, void *context) {
  (void)context;
  g_bus_topic = topic;
  g_bus_payload = payload;
}

/* Handler for re-entrancy test */
static Button *g_reentrant_btn = NULL;
static TCALLBACK(Button_Click) g_reentrant_cb;
static int g_reentrant_calls = 0;

static void OnReentrantClick(int button, int x, int y, void *context) {
  (void)button; (void)x; (void)y; (void)context;
  g_reentrant_calls++;
  /* Unsubscribe self during emit */
  if (g_reentrant_btn) {
    OBJECT_CALL(g_reentrant_btn, Click_Unsubscribe, g_reentrant_cb);
  }
}

/* ========================================================================== */
/* Tests Execution                                                            */
/* ========================================================================== */

static void Test_ObjectEventProcedure(void) {
  Button btn;
  assert(Button_Init(&btn) == STATUS_CONST(SUCCESS));

  assert(OBJECT_CALL(&btn, Click_Capacity) == 4);
  assert(OBJECT_CALL(&btn, Click_Count) == 0);

  /* Hover requested 6 -> decayed to 8 */
  assert(OBJECT_CALL(&btn, Hover_Capacity) == 8);
  assert(OBJECT_CALL(&btn, Hover_Count) == 0);

  TCALLBACK(Button_Hover) hcb = CALLBACK_FUNC(Button_Hover_Bind)(OnButtonHover, NULL);
  assert(OBJECT_CALL(&btn, Hover_Subscribe, hcb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Hover_Count) == 1);
  g_hover_count = 0;
  OBJECT_CALL(&btn, Hover_Emit, 10, 20);
  assert(g_hover_count == 1);
  assert(OBJECT_CALL(&btn, Hover_Unsubscribe, hcb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Hover_Count) == 0);

  TCALLBACK(Button_Click) cb1 = CALLBACK_FUNC(Button_Click_Bind)(OnButtonClick, (void *)(uintptr_t)1);
  TCALLBACK(Button_Click) cb2 = CALLBACK_FUNC(Button_Click_Bind)(OnButtonClick, (void *)(uintptr_t)2);
  TCALLBACK(Button_Click) cb3 = CALLBACK_FUNC(Button_Click_Bind)(OnButtonClick, (void *)(uintptr_t)3);
  TCALLBACK(Button_Click) cb4 = CALLBACK_FUNC(Button_Click_Bind)(OnButtonClick, (void *)(uintptr_t)4);
  TCALLBACK(Button_Click) cb5 = CALLBACK_FUNC(Button_Click_Bind)(OnButtonClick, (void *)(uintptr_t)5);

  /* Subscribe 4 listeners */
  assert(OBJECT_CALL(&btn, Click_Subscribe, cb1) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Click_Subscribe, cb2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Click_Subscribe, cb3) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Click_Subscribe, cb4) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Click_Count) == 4);

  /* 5th listener exceeds capacity -> OUT_OF_RANGE */
  assert(OBJECT_CALL(&btn, Click_Subscribe, cb5) == STATUS_CONST(OUT_OF_RANGE));
  assert(OBJECT_CALL(&btn, Click_Count) == 4);

  /* Emit event: 4 listeners called with (1, 10, 20) -> sum = 4 * 31 = 124 */
  g_click_sum = 0;
  OBJECT_CALL(&btn, Click_Emit, 1, 10, 20);
  assert(g_click_sum == 124);

  /* Unsubscribe cb2 */
  assert(OBJECT_CALL(&btn, Click_Unsubscribe, cb2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Click_Count) == 3);

  /* Unsubscribing non-existent listener -> NOT_FOUND */
  assert(OBJECT_CALL(&btn, Click_Unsubscribe, cb2) == STATUS_CONST(NOT_FOUND));

  /* Emit again: 3 listeners called -> sum = 124 + (3 * 31) = 217 */
  OBJECT_CALL(&btn, Click_Emit, 1, 10, 20);
  assert(g_click_sum == 217);

  /* Clear */
  assert(OBJECT_CALL(&btn, Click_Clear) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Click_Count) == 0);
  OBJECT_CALL(&btn, Click_Emit, 1, 10, 20);
  assert(g_click_sum == 217); /* No change */
}

static void Test_ObjectNullaryEvent(void) {
  Button btn;
  assert(Button_Init(&btn) == STATUS_CONST(SUCCESS));

  TCALLBACK(Button_Reset) cb = CALLBACK_FUNC(Button_Reset_Bind)(OnButtonReset, NULL);
  assert(OBJECT_CALL(&btn, Reset_Subscribe, cb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Reset_Count) == 1);

  g_reset_count = 0;
  OBJECT_CALL(&btn, Reset_Emit);
  assert(g_reset_count == 1);

  assert(OBJECT_CALL(&btn, Reset_Unsubscribe, cb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Reset_Count) == 0);
}

static void Test_ObjectReentrancy(void) {
  Button btn;
  assert(Button_Init(&btn) == STATUS_CONST(SUCCESS));

  g_reentrant_btn = &btn;
  g_reentrant_cb = CALLBACK_FUNC(Button_Click_Bind)(OnReentrantClick, NULL);
  assert(OBJECT_CALL(&btn, Click_Subscribe, g_reentrant_cb) == STATUS_CONST(SUCCESS));

  TCALLBACK(Button_Click) dummyCb = CALLBACK_FUNC(Button_Click_Bind)(OnButtonClick, (void *)(uintptr_t)99);
  assert(OBJECT_CALL(&btn, Click_Subscribe, dummyCb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Click_Count) == 2);

  g_reentrant_calls = 0;
  g_click_sum = 0;

  /* Emit: g_reentrant_cb is executed, unregisters itself inside callback.
   * But snapshot on stack ensures dummyCb is still called safely without crash or skip! */
  OBJECT_CALL(&btn, Click_Emit, 1, 0, 0);

  assert(g_reentrant_calls == 1);
  assert(g_click_sum == 1);
  /* After emit finishes, count is 1 because reentrant unregistered itself */
  assert(OBJECT_CALL(&btn, Click_Count) == 1);
}

static void Test_ObjectEventOperation(void) {
  Button btn;
  assert(Button_Init(&btn) == STATUS_CONST(SUCCESS));

  TCALLBACK(Button_Validate) passCb = CALLBACK_FUNC(Button_Validate_Bind)(OnValidatePass, NULL);
  TCALLBACK(Button_Validate) failCb = CALLBACK_FUNC(Button_Validate_Bind)(OnValidateFail, NULL);
  TCALLBACK(Button_Validate) unreachCb = CALLBACK_FUNC(Button_Validate_Bind)(OnValidateUnreachable, NULL);

  assert(OBJECT_CALL(&btn, Validate_Subscribe, passCb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Validate_Subscribe, failCb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Validate_Subscribe, unreachCb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&btn, Validate_Count) == 3);

  g_validate_calls = 0;

  /* Emit should call passCb, then failCb, which returns INVALID_ARGUMENT.
   * Short-circuit must stop execution, so unreachCb is NOT called! */
  OPSTATUS status = OBJECT_CALL(&btn, Validate_Emit, 42);
  assert(status == STATUS_CONST(INVALID_ARGUMENT));
  assert(g_validate_calls == 2); /* passCb (1) + failCb (1) = 2, unreachCb was skipped */
}

static void Test_PackageEvents(void) {
  assert(PACKAGE_CALL(Bus, OnMessage_Capacity) == 4);
  assert(PACKAGE_CALL(Bus, OnMessage_Count) == 0);

  TCALLBACK(Bus_OnMessage) cb = CALLBACK_FUNC(Bus_OnMessage_Bind)(OnBusMessage, NULL);
  assert(PACKAGE_CALL(Bus, OnMessage_Subscribe, cb) == STATUS_CONST(SUCCESS));
  assert(PACKAGE_CALL(Bus, OnMessage_Count) == 1);

  g_bus_topic = 0;
  g_bus_payload = 0;
  PACKAGE_CALL(Bus, OnMessage_Emit, 10, 200);
  assert(g_bus_topic == 10);
  assert(g_bus_payload == 200);

  assert(PACKAGE_CALL(Bus, OnMessage_Unsubscribe, cb) == STATUS_CONST(SUCCESS));
  assert(PACKAGE_CALL(Bus, OnMessage_Count) == 0);
}

static void Test_DirectObjectEventCalls(void) {
  Button btn;
  assert(Button_Init(&btn) == STATUS_CONST(SUCCESS));

  assert(Button_Click_Capacity(&btn) == 4);
  assert(Button_Click_Count(&btn) == 0);

  TCALLBACK(Button_Click) cb = CALLBACK_FUNC(Button_Click_Bind)(OnButtonClick, (void *)(uintptr_t)10);
  assert(Button_Click_Subscribe(&btn, cb) == STATUS_CONST(SUCCESS));
  assert(Button_Click_Count(&btn) == 1);

  g_click_sum = 0;
  Button_Click_Emit(&btn, 5, 2, 3);
  assert(g_click_sum == 10);

  assert(Button_Click_Unsubscribe(&btn, cb) == STATUS_CONST(SUCCESS));
  assert(Button_Click_Count(&btn) == 0);
}

static int g_old_val = 0;
static int g_new_val = 0;
static int g_mutate_calls = 0;

static void OnSliderChanged(int oldValue, int newValue, void *context) {
  (void)context;
  g_old_val = oldValue;
  g_new_val = newValue;
  g_mutate_calls++;
}

static void Test_ReactiveMutation(void) {
  Slider slider;
  assert(Slider_Init(&slider) == STATUS_CONST(SUCCESS));
  slider.value = 10;

  TCALLBACK(Slider_OnValueChanged) cb =
      CALLBACK_FUNC(Slider_OnValueChanged_Bind)(OnSliderChanged, NULL);

  assert(OBJECT_CALL(&slider, OnValueChanged_Subscribe, cb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&slider, OnValueChanged_Count) == 1);

  g_mutate_calls = 0;

  /* 1. Value changes from 10 to 25 -> automatically emits */
  assert(OBJECT_CALL(&slider, SetValue, 25) == STATUS_CONST(SUCCESS));
  assert(slider.value == 25);
  assert(g_mutate_calls == 1);
  assert(g_old_val == 10);
  assert(g_new_val == 25);

  /* 2. Setting same value (25) -> NO mutation, NO emit */
  assert(OBJECT_CALL(&slider, SetValue, 25) == STATUS_CONST(SUCCESS));
  assert(g_mutate_calls == 1); /* Still 1 */

  /* 3. Value changes from 25 to 50 -> automatically emits */
  assert(OBJECT_CALL(&slider, SetValue, 50) == STATUS_CONST(SUCCESS));
  assert(slider.value == 50);
  assert(g_mutate_calls == 2);
  assert(g_old_val == 25);
  assert(g_new_val == 50);

  assert(OBJECT_CALL(&slider, OnValueChanged_Unsubscribe, cb) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&slider, OnValueChanged_Count) == 0);
}

int main(void) {
  Test_ObjectEventProcedure();
  Test_ObjectNullaryEvent();
  Test_ObjectReentrancy();
  Test_ObjectEventOperation();
  Test_PackageEvents();
  Test_DirectObjectEventCalls();
  Test_ReactiveMutation();

  printf("All Event tests passed successfully!\n");
  return 0;
}
