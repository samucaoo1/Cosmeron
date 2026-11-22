/* Compile with:
 * gcc -std=c11 -Wall -Wextra -Werror -pedantic -I Codespace Codespace/Tests/Core/Delegate.c -o /tmp/test_delegate && /tmp/test_delegate
 */
#include "Cosmeron/Core/Algorithm/Comparison.h"
#include "Cosmeron/Core/Condition/Status.h"
#include "Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ========================================================================== */
/* 1. Root Declarations                                                       */
/* ========================================================================== */

/* Pure 8-byte stateless callables */
CALLABLE_FUNCTION(int, IntUnaryOp, (int value));
CALLABLE_PROCEDURE(IntProcedure, (int code));
CALLABLE_PREDICATE(IntPredicate, (int value));
CALLABLE_OPERATION(IntOperation, (int value));
CALLABLE_COMPARISON(IntComparison, (int left, int right));

/* 16-byte dual-mode delegates */
DELEGATE_FUNCTION(int, IntDelegateFunc, (int value));
DELEGATE_PROCEDURE(IntDelegateProc, (int code));
DELEGATE_PREDICATE(IntDelegatePred, (int value));
DELEGATE_OPERATION(IntDelegateOp, (int value));
DELEGATE_COMPARISON(IntDelegateCmp, (int left, int right));

/* Nullary callables & delegates */
CALLABLE_FUNCTION(int, GetMagic, ());
CALLABLE_PROCEDURE(ResetGlobal, ());
DELEGATE_FUNCTION(int, GetContextMagic, ());
DELEGATE_PROCEDURE(ResetContext, ());

/* ========================================================================== */
/* 2. Scoped Declarations inside MODULE, PACKAGE, OBJECT                      */
/* ========================================================================== */

MODULE(DOMAIN(Core), DelegateTestModule,
  CALLABLE(
    FUNCTION(int, Transform, (int x)),
    PROCEDURE(ModuleLog, (int code))
  ),
  DELEGATE(
    FUNCTION(int, StatefulCompute, (int x)),
    PREDICATE(StatefulValidate, (int x))
  ),
  PACKAGE(TestDelegatePkg,
    CALLABLE(
      FUNCTION(int, PkgTransform, (int x))
    ),
    DELEGATE(
      PROCEDURE(PkgNotify, (int code))
    )
  ),
  OBJECT(WorkerObject,
    FIELDS(
      int workerId;
      int accumulator;
    ),
    DELEGATE(
      OPERATION(OnTask, (int taskId))
    ),
    PROCEDURE_INLINE(Add, (int delta), {
      self->accumulator += delta;
    })
  )
);

/* Adapter for DelegateTestModule_WorkerObject_Add */
DELEGATE_ADAPT_OBJECT_METHOD(
    TDELEGATE(IntDelegateProc),
    MODULE_TYPE(DelegateTestModule, WorkerObject),
    MODULE_FUNC(DelegateTestModule, WorkerObject, Add),
    void,
    (int delta),
    (delta)
)

/* ========================================================================== */
/* 3. Standalone Object with Events (Testing Delegate Event Dispatch)         */
/* ========================================================================== */

OBJECT_DEF(EventWorker,
  FIELDS(
    int workerId;
  ),
  EVENT_PROCEDURE(TaskCompleted, ((int, taskId)))
);

OBJECT_IMPLEMENT(EventWorker);

/* ========================================================================== */
/* Implementation Functions for Testing                                       */
/* ========================================================================== */

static int Square(int value) {
  return value * value;
}

static int g_last_code = 0;
static void RecordCode(int code) {
  g_last_code = code;
}

static bool IsEven(int value) {
  return (value % 2) == 0;
}

static OPSTATUS CheckPositive(int value) {
  return value >= 0 ? STATUS_CONST(SUCCESS) : STATUS_CONST(INVALID_ARGUMENT);
}

static CMPOUT CompareAscending(int left, int right) {
  if (left < right) return COMPARISON_CONST(LOWER);
  if (left > right) return COMPARISON_CONST(HIGHER);
  return COMPARISON_CONST(EQUAL);
}

static int Return42(void) {
  return 42;
}

static void ResetGlobalFlag(void) {
  g_last_code = -1;
}

/* Stateful function targets (taking context as trailing parameter) */
static int MultiplyByContext(int value, void *context) {
  int factor = *(int *)context;
  return value * factor;
}

static int NullContextMultiplier(int value, void *context) {
  assert(context == NULL);
  return value * 10;
}

static void AccumulateCode(int code, void *context) {
  int *sum = (int *)context;
  *sum += code;
}

static bool GreaterThanContext(int value, void *context) {
  int threshold = *(int *)context;
  return value > threshold;
}

static OPSTATUS ContextBoundOperation(int value, void *context) {
  int limit = *(int *)context;
  return value <= limit ? STATUS_CONST(SUCCESS) : STATUS_CONST(OUT_OF_RANGE);
}

static CMPOUT ContextBoundComparison(int left, int right, void *context) {
  bool reverse = *(bool *)context;
  if (left < right) return reverse ? COMPARISON_CONST(HIGHER) : COMPARISON_CONST(LOWER);
  if (left > right) return reverse ? COMPARISON_CONST(LOWER) : COMPARISON_CONST(HIGHER);
  return COMPARISON_CONST(EQUAL);
}

static int ContextGetMagic(void *context) {
  return *(int *)context;
}

static void ContextReset(void *context) {
  *(int *)context = 0;
}

static OPSTATUS WorkerTaskOp(int taskId, void *context) {
  int *lastTask = (int *)context;
  *lastTask = taskId;
  return STATUS_CONST(SUCCESS);
}

/* ========================================================================== */
/* Test Cases                                                                 */
/* ========================================================================== */

static void Test_StatelessCallables(void) {
  /* 1. Size must be exactly 1 pointer (8 bytes on 64-bit platforms) */
  assert(sizeof(TCALLABLE(IntUnaryOp)) == sizeof(void *));
  assert(sizeof(TCALLABLE(IntProcedure)) == sizeof(void *));
  assert(sizeof(TCALLABLE(IntPredicate)) == sizeof(void *));
  assert(sizeof(TCALLABLE(IntOperation)) == sizeof(void *));
  assert(sizeof(TCALLABLE(IntComparison)) == sizeof(void *));
  assert(sizeof(TCALLABLE(GetMagic)) == sizeof(void *));
  assert(sizeof(TCALLABLE(ResetGlobal)) == sizeof(void *));

  /* 2. Direct assignment and invocation */
  TCALLABLE(IntUnaryOp) op = Square;
  assert(CALLABLE_IS_VALID(op));
  assert(CALLABLE_CALL(op, 5) == 25);
  assert(CALLABLE_INVOKE(op, 6) == 36);
  assert(op(7) == 49);

  TCALLABLE(IntProcedure) proc = RecordCode;
  CALLABLE_CALL(proc, 100);
  assert(g_last_code == 100);

  TCALLABLE(IntPredicate) pred = IsEven;
  assert(CALLABLE_CALL(pred, 4) == true);
  assert(CALLABLE_CALL(pred, 5) == false);

  TCALLABLE(IntOperation) oper = CheckPositive;
  assert(CALLABLE_CALL(oper, 10) == STATUS_CONST(SUCCESS));
  assert(CALLABLE_CALL(oper, -1) == STATUS_CONST(INVALID_ARGUMENT));

  TCALLABLE(IntComparison) cmp = CompareAscending;
  assert(CALLABLE_CALL(cmp, 3, 5) == COMPARISON_CONST(LOWER));
  assert(CALLABLE_CALL(cmp, 5, 5) == COMPARISON_CONST(EQUAL));
  assert(CALLABLE_CALL(cmp, 8, 5) == COMPARISON_CONST(HIGHER));

  TCALLABLE(GetMagic) magic = Return42;
  assert(CALLABLE_CALL(magic) == 42);

  TCALLABLE(ResetGlobal) reset = ResetGlobalFlag;
  CALLABLE_CALL(reset);
  assert(g_last_code == -1);

  /* 3. Empty / Invalid */
  TCALLABLE(IntUnaryOp) emptyCb = CALLABLE_EMPTY;
  assert(!CALLABLE_IS_VALID(emptyCb));
}

static void Test_StatefulDelegates(void) {
  /* 1. Size must be exactly 2 pointers (16 bytes on 64-bit platforms) */
  assert(sizeof(TDELEGATE(IntDelegateFunc)) == 2 * sizeof(void *));
  assert(sizeof(TDELEGATE(IntDelegateProc)) == 2 * sizeof(void *));
  assert(sizeof(TDELEGATE(IntDelegatePred)) == 2 * sizeof(void *));
  assert(sizeof(TDELEGATE(IntDelegateOp)) == 2 * sizeof(void *));
  assert(sizeof(TDELEGATE(IntDelegateCmp)) == 2 * sizeof(void *));
  assert(sizeof(TDELEGATE(GetContextMagic)) == 2 * sizeof(void *));
  assert(sizeof(TDELEGATE(ResetContext)) == 2 * sizeof(void *));

  /* 2. Bind with real context */
  int factor = 4;
  TDELEGATE(IntDelegateFunc) dlMul = DELEGATE_FUNC(IntDelegateFunc_Bind)(MultiplyByContext, &factor);
  assert(DELEGATE_IS_VALID(dlMul));
  assert(DELEGATE_CALL(dlMul, 6) == 24);
  assert(DELEGATE_INVOKE(dlMul, 10) == 40);

  int sum = 0;
  TDELEGATE(IntDelegateProc) dlSum = DELEGATE_FUNC(IntDelegateProc_Bind)(AccumulateCode, &sum);
  DELEGATE_CALL(dlSum, 15);
  DELEGATE_CALL(dlSum, 25);
  assert(sum == 40);

  int threshold = 18;
  TDELEGATE(IntDelegatePred) dlPred = DELEGATE_FUNC(IntDelegatePred_Bind)(GreaterThanContext, &threshold);
  assert(DELEGATE_CALL(dlPred, 20) == true);
  assert(DELEGATE_CALL(dlPred, 15) == false);

  int limit = 100;
  TDELEGATE(IntDelegateOp) dlOp = DELEGATE_FUNC(IntDelegateOp_Bind)(ContextBoundOperation, &limit);
  assert(DELEGATE_CALL(dlOp, 50) == STATUS_CONST(SUCCESS));
  assert(DELEGATE_CALL(dlOp, 150) == STATUS_CONST(OUT_OF_RANGE));

  bool reverse = true;
  TDELEGATE(IntDelegateCmp) dlCmp = DELEGATE_FUNC(IntDelegateCmp_Bind)(ContextBoundComparison, &reverse);
  assert(DELEGATE_CALL(dlCmp, 3, 5) == COMPARISON_CONST(HIGHER));
  assert(DELEGATE_CALL(dlCmp, 5, 5) == COMPARISON_CONST(EQUAL));
  assert(DELEGATE_CALL(dlCmp, 8, 5) == COMPARISON_CONST(LOWER));

  int magicVal = 777;
  TDELEGATE(GetContextMagic) dlMagic = DELEGATE_FUNC(GetContextMagic_Bind)(ContextGetMagic, &magicVal);
  assert(DELEGATE_CALL(dlMagic) == 777);

  int resetVal = 999;
  TDELEGATE(ResetContext) dlReset = DELEGATE_FUNC(ResetContext_Bind)(ContextReset, &resetVal);
  DELEGATE_CALL(dlReset);
  assert(resetVal == 0);

  /* 3. Empty / Invalid */
  TDELEGATE(IntDelegateFunc) emptyDl = DELEGATE_EMPTY;
  assert(!DELEGATE_IS_VALID(emptyDl));
}

static void Test_SentinelDualDispatch(void) {
  /* Sentinel test 1: Bind with NULL context
   * Sentinel maps context to internal sentinel pointer.
   * At runtime, DELEGATE_CALL sees dl.context != NULL, unwraps
   * and invokes the function with NULL! */
  TDELEGATE(IntDelegateFunc) dlNullCtx =
      DELEGATE_FUNC(IntDelegateFunc_Bind)(NullContextMultiplier, NULL);
  assert(DELEGATE_IS_VALID(dlNullCtx));
  assert(DELEGATE_CALL(dlNullCtx, 7) == 70);

  /* Sentinel test 2: Promotion from stateless callable via FromCallable
   * Sets dl.context = NULL.
   * At runtime, DELEGATE_CALL sees dl.context == NULL and invokes callable(args...) directly! */
  TCALLABLE(IntUnaryOp) cb = Square;
  TDELEGATE(IntDelegateFunc) promotedDl =
      DELEGATE_FUNC(IntDelegateFunc_FromCallable)(cb);
  assert(DELEGATE_IS_VALID(promotedDl));
  assert(promotedDl.context == NULL);
  assert(DELEGATE_CALL(promotedDl, 9) == 81);
}

static void Test_ObjectMethodAdapter(void) {
  MODULE_TYPE(DelegateTestModule, WorkerObject) worker;
  assert(DelegateTestModule_WorkerObject_Init(&worker) == STATUS_CONST(SUCCESS));
  worker.accumulator = 100;

  /* Use the generated adapter to bind worker object instance to a delegate */
  TDELEGATE(IntDelegateProc) addDelegate =
      DELEGATE_AS_DELEGATE(MODULE_FUNC(DelegateTestModule, WorkerObject, Add), &worker);

  assert(DELEGATE_IS_VALID(addDelegate));
  DELEGATE_CALL(addDelegate, 50);
  assert(worker.accumulator == 150);

  DELEGATE_CALL(addDelegate, 25);
  assert(worker.accumulator == 175);
}

static void Test_ScopedDeclarations(void) {
  /* 1. Module root scoped types */
  MODULE_CALLABLE(DelegateTestModule, Transform) modCb = Square;
  assert(modCb(8) == 64);

  int factor = 10;
  MODULE_DELEGATE(DelegateTestModule, StatefulCompute) modDl =
      NS_JOIN(MODULE_DELEGATE_FUNC(DelegateTestModule, StatefulCompute), Bind)(MultiplyByContext, &factor);
  assert(DELEGATE_CALL(modDl, 5) == 50);

  /* 2. Package scoped types */
  MODULE_CALLABLE(DelegateTestModule, TestDelegatePkg, PkgTransform) pkgCb = Square;
  assert(pkgCb(3) == 9);

  int sum = 0;
  MODULE_DELEGATE(DelegateTestModule, TestDelegatePkg, PkgNotify) pkgDl =
      NS_JOIN(MODULE_DELEGATE_FUNC(DelegateTestModule, TestDelegatePkg, PkgNotify), Bind)(AccumulateCode, &sum);
  DELEGATE_CALL(pkgDl, 77);
  assert(sum == 77);

  /* 3. Object member scoped delegate */
  int lastTaskId = 0;
  MODULE_DELEGATE(DelegateTestModule, WorkerObject, OnTask) taskDl =
      NS_JOIN(MODULE_DELEGATE_FUNC(DelegateTestModule, WorkerObject, OnTask), Bind)(WorkerTaskOp, &lastTaskId);
  assert(DELEGATE_IS_VALID(taskDl));
  assert(DELEGATE_CALL(taskDl, 42) == STATUS_CONST(SUCCESS));
  assert(lastTaskId == 42);
}

static int g_event_stateful_received = 0;
static void StatefulEventListener(int taskId, void *context) {
  int *ctxSum = (int *)context;
  *ctxSum += taskId;
  g_event_stateful_received++;
}

static int g_event_stateless_received = 0;
static void StatelessEventListener(int taskId) {
  g_event_stateless_received += taskId;
}

static void Test_EventsDualDispatch(void) {
  EventWorker worker;
  assert(EventWorker_Init(&worker) == STATUS_CONST(SUCCESS));

  /* 1. Stateful delegate listener */
  int localSum = 0;
  TDELEGATE(EventWorker_TaskCompleted) statefulDl =
      DELEGATE_FUNC(EventWorker_TaskCompleted_Bind)(StatefulEventListener, &localSum);

  /* 2. Stateless callable listener promoted via FromCallable */
  TDELEGATE(EventWorker_TaskCompleted) statelessDl =
      DELEGATE_FUNC(EventWorker_TaskCompleted_FromCallable)(StatelessEventListener);

  assert(OBJECT_CALL(&worker, TaskCompleted_Subscribe, statefulDl) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&worker, TaskCompleted_Subscribe, statelessDl) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&worker, TaskCompleted_Count) == 2);

  /* Emit event: taskId = 10 */
  g_event_stateful_received = 0;
  g_event_stateless_received = 0;
  OBJECT_CALL(&worker, TaskCompleted_Emit, 10);

  assert(g_event_stateful_received == 1);
  assert(localSum == 10);
  assert(g_event_stateless_received == 10);

  /* Emit event again: taskId = 25 */
  OBJECT_CALL(&worker, TaskCompleted_Emit, 25);
  assert(g_event_stateful_received == 2);
  assert(localSum == 35);
  assert(g_event_stateless_received == 35);

  /* Unsubscribe stateful */
  assert(OBJECT_CALL(&worker, TaskCompleted_Unsubscribe, statefulDl) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&worker, TaskCompleted_Count) == 1);

  /* Emit again: only stateless should receive */
  OBJECT_CALL(&worker, TaskCompleted_Emit, 5);
  assert(g_event_stateful_received == 2); /* unchanged */
  assert(localSum == 35);                 /* unchanged */
  assert(g_event_stateless_received == 40);

  /* Unsubscribe stateless */
  assert(OBJECT_CALL(&worker, TaskCompleted_Unsubscribe, statelessDl) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&worker, TaskCompleted_Count) == 0);
}

int main(void) {
  Test_StatelessCallables();
  Test_StatefulDelegates();
  Test_SentinelDualDispatch();
  Test_ObjectMethodAdapter();
  Test_ScopedDeclarations();
  Test_EventsDualDispatch();
  return 0;
}
