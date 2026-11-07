#include "../../Cosmeron/Core/Module/Object.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct Canvas {
  int id;
} Canvas;

/* ============================================================
 * Class 1: TWidget (Alternativa B: Declaração limpa em um único bloco)
 * ============================================================ */

OBJECT(TWidget,
    FIELDS(
        int x;
        int y;
        bool visible;
        int lastCanvas;
    ),
    METHOD(void, Draw, (Canvas *canvas)),
    METHOD(void, Move, (int dx, int dy)),
    METHOD(bool, IsVisible, ())
);

/* Method implementations using OBJECT_PROTOTYPE (inferred parameter types via __typeof__) */
OBJECT_PROTOTYPE(TWidget, Draw, canvas) {
  self->lastCanvas = canvas ? canvas->id : 0;
}

OBJECT_PROTOTYPE(TWidget, Move, dx, dy) {
  self->x += dx;
  self->y += dy;
}

OBJECT_PROTOTYPE(TWidget, IsVisible) {
  return self->visible;
}

OBJECT_IMPLEMENT(TWidget);

/* ============================================================
 * Class 2: TButton (Herança estendendo TWidget)
 * ============================================================ */

OBJECT_EXTENDS(TButton, TWidget,
    FIELDS(
        const char *label;
        int clickCount;
    ),
    METHOD(void, Click, ()),
    METHOD(void, SetLabel, (const char *newLabel))
);

/* Method implementations using OBJECT_PROTOTYPEEX (pure standard C11 without __typeof__) */
OBJECT_PROTOTYPEEX(TButton, Click) {
  self->clickCount++;
}

OBJECT_PROTOTYPEEX(TButton, SetLabel, (const char *newLabel)) {
  self->label = newLabel;
}

OBJECT_IMPLEMENT(TButton);

/* ============================================================
 * Class 3: TCounter (Implementação Inline Dentro do Bloco)
 * ============================================================ */

OBJECT(TCounter,
    FIELDS(
        int count;
    ),
    METHOD_INLINE(void, Increment, (), {
        self->count++;
    }),
    CONST_METHOD_INLINE(int, Value, (), {
        return self->count;
    })
);

OBJECT_IMPLEMENT(TCounter);

/* ============================================================
 * Class 4: TManyArgs (Teste com 20 parâmetros superando limite de 16)
 * ============================================================ */

OBJECT(TManyArgs,
    FIELDS(
        int sum;
    ),
    METHOD(int, Sum20, (
        int a1, int a2, int a3, int a4, int a5,
        int a6, int a7, int a8, int a9, int a10,
        int a11, int a12, int a13, int a14, int a15,
        int a16, int a17, int a18, int a19, int a20
    ))
);

OBJECT_PROTOTYPE(TManyArgs, Sum20,
    a1, a2, a3, a4, a5,
    a6, a7, a8, a9, a10,
    a11, a12, a13, a14, a15,
    a16, a17, a18, a19, a20
) {
  self->sum = a1 + a2 + a3 + a4 + a5 +
              a6 + a7 + a8 + a9 + a10 +
              a11 + a12 + a13 + a14 + a15 +
              a16 + a17 + a18 + a19 + a20;
  return self->sum;
}

OBJECT_IMPLEMENT(TManyArgs);

/* ============================================================
 * Main Test Runner
 * ============================================================ */

int main(void) {
  /* 1. Test Base TObject */
  TObject baseObj;
  assert(Object_Init(&baseObj) == STATUS_CONST(SUCCESS));
  assert(baseObj.api != NULL);
  assert(Object_Equals(&baseObj, &baseObj) == true);
  assert(Object_Hash(&baseObj) != 0ULL);
  assert(Object_Destroy(&baseObj) == STATUS_CONST(SUCCESS));

  /* 2. Test TWidget */
  TWidget widget = {0};
  assert(TWidget_Init(&widget) == STATUS_CONST(SUCCESS));
  assert(widget.api != NULL);
  widget.visible = true;

  /* Direct calls */
  TWidget_Move(&widget, 10, 20);
  assert(widget.x == 10);
  assert(widget.y == 20);
  assert(TWidget_IsVisible(&widget) == true);

  /* Polymorphic calls via FunctionTable */
  Canvas canvas = {.id = 999};
  widget.api->Draw(&widget, &canvas);
  assert(widget.lastCanvas == 999);

  /* Helper macro OBJECT_CALL */
  OBJECT_CALL(&widget, Move, 5, 5);
  assert(widget.x == 15);
  assert(widget.y == 25);

  /* 3. Test TButton (Inheritance & Upcasting) */
  TButton button = {0};
  assert(TButton_Init(&button) == STATUS_CONST(SUCCESS));
  assert(button.api != NULL);
  button.label = "OK";
  button.clickCount = 0;

  TButton_Click(&button);
  assert(button.clickCount == 1);
  TButton_SetLabel(&button, "Cancel");
  assert(strcmp(button.label, "Cancel") == 0);
  button.api->SetLabel(&button, "Submit");
  assert(strcmp(button.label, "Submit") == 0);

  /* Safe upcast to base TWidget */
  TWidget *baseWidget = OBJECT_AS(&button, TWidget);
  assert(TWidget_Init(baseWidget) == STATUS_CONST(SUCCESS));
  TWidget_Move(baseWidget, 100, 200);
  assert(button.base.x == 100);
  assert(button.base.y == 200);

  /* 4. Test TCounter (Inline Implementation) */
  TCounter counter = {0};
  assert(TCounter_Init(&counter) == STATUS_CONST(SUCCESS));
  assert(counter.api != NULL);
  assert(TCounter_Value(&counter) == 0);
  TCounter_Increment(&counter);
  TCounter_Increment(&counter);
  assert(TCounter_Value(&counter) == 2);
  assert(OBJECT_CALL_0(&counter, Value) == 2);

  /* 5. Test TManyArgs (20 arguments exceeding previous 16-parameter limit) */
  TManyArgs many = {0};
  assert(TManyArgs_Init(&many) == STATUS_CONST(SUCCESS));
  assert(many.api != NULL);
  int total = many.api->Sum20(&many,
                              1, 1, 1, 1, 1,
                              1, 1, 1, 1, 1,
                              1, 1, 1, 1, 1,
                              1, 1, 1, 1, 1);
  assert(total == 20);
  assert(many.sum == 20);

  return 0;
}
