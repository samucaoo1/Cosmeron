/* Compile with:
 * gcc -std=c11 -Wall -Wextra -Werror -pedantic -I Codespace Codespace/PendingRefactoring/Tests/Core/Closure.c -o /tmp/test_closure && /tmp/test_closure
 */
#include "Cosmeron/Core/Algorithm/Comparison.h"
#include "Cosmeron/Core/Outcome/Status.h"
#include "Cosmeron/Core/Module/Module.h"
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ========================================================================== */
/* 1. Root Declarations                                                       */
/* ========================================================================== */

/* Pure 8-byte stateless callbacks */
CALLBACK_FUNCTION(int, IntUnaryOp, (int value));
CALLBACK_PROCEDURE(IntProcedure, (int code));
CALLBACK_PREDICATE(IntPredicate, (int value));
CALLBACK_OPERATION(IntOperation, (int value));
CALLBACK_COMPARISON(IntComparison, (int left, int right));

/* 16-byte dual-mode closures */
CLOSURE_FUNCTION(int, IntClosureFunc, (int value));
CLOSURE_PROCEDURE(IntClosureProc, (int code));
CLOSURE_PREDICATE(IntClosurePred, (int value));
CLOSURE_OPERATION(IntClosureOp, (int value));
CLOSURE_COMPARISON(IntClosureCmp, (int left, int right));

/* Nullary callbacks & closures */
CALLBACK_FUNCTION(int, GetMagic, ());
CALLBACK_PROCEDURE(ResetGlobal, ());
CLOSURE_FUNCTION(int, GetContextMagic, ());
CLOSURE_PROCEDURE(ResetContext, ());

/* ========================================================================== */
/* 2. Scoped Declarations inside MODULE, PACKAGE, OBJECT                      */
/* ========================================================================== */

MODULE(DOMAIN(Core), ClosureTestModule,
  CALLBACK(
    FUNCTION(int, Transform, (int x)),
    PROCEDURE(ModuleLog, (int code))
  ),
  CLOSURE(
    FUNCTION(int, StatefulCompute, (int x)),
    PREDICATE(StatefulValidate, (int x))
  ),
  PACKAGE(TestClosurePkg,
    CALLBACK(
      FUNCTION(int, PkgTransform, (int x))
    ),
    CLOSURE(
      PROCEDURE(PkgNotify, (int code))
    )
  ),
  OBJECT(WorkerObject,
    FIELDS(
      int workerId;
      int accumulator;
    ),
    CLOSURE(
      OPERATION(OnTask, (int taskId))
    ),
    PROCEDURE_INLINE(Add, (int delta), {
      self->accumulator += delta;
    })
  )
);

/* Adapter for ClosureTestModule_WorkerObject_Add */
CLOSURE_ADAPT_OBJECT_METHOD(
    TCLOSURE(IntClosureProc),
    MODULE_TYPE(ClosureTestModule, WorkerObject),
    MODULE_FUNC(ClosureTestModule, WorkerObject, Add),
    void,
    (int delta),
    (delta)
)

/* ========================================================================== */
/* 3. Standalone Object with Events (Testing Closure Event Dispatch)          */
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

static void Test_StatelessCallbacks(void) {
  /* 1. Size must be exactly 1 pointer (8 bytes on 64-bit platforms) */
  assert(sizeof(TCALLBACK(IntUnaryOp)) == sizeof(void *));
  assert(sizeof(TCALLBACK(IntProcedure)) == sizeof(void *));
  assert(sizeof(TCALLBACK(IntPredicate)) == sizeof(void *));
  assert(sizeof(TCALLBACK(IntOperation)) == sizeof(void *));
  assert(sizeof(TCALLBACK(IntComparison)) == sizeof(void *));
  assert(sizeof(TCALLBACK(GetMagic)) == sizeof(void *));
  assert(sizeof(TCALLBACK(ResetGlobal)) == sizeof(void *));

  /* 2. Direct assignment and invocation */
  TCALLBACK(IntUnaryOp) op = Square;
  assert(CALLBACK_IS_VALID(op));
  assert(CALLBACK_CALL(op, 5) == 25);
  assert(CALLBACK_INVOKE(op, 6) == 36);
  assert(op(7) == 49);

  TCALLBACK(IntProcedure) proc = RecordCode;
  CALLBACK_CALL(proc, 100);
  assert(g_last_code == 100);

  TCALLBACK(IntPredicate) pred = IsEven;
  assert(CALLBACK_CALL(pred, 4) == true);
  assert(CALLBACK_CALL(pred, 5) == false);

  TCALLBACK(IntOperation) oper = CheckPositive;
  assert(CALLBACK_CALL(oper, 10) == STATUS_CONST(SUCCESS));
  assert(CALLBACK_CALL(oper, -1) == STATUS_CONST(INVALID_ARGUMENT));

  TCALLBACK(IntComparison) cmp = CompareAscending;
  assert(CALLBACK_CALL(cmp, 3, 5) == COMPARISON_CONST(LOWER));
  assert(CALLBACK_CALL(cmp, 5, 5) == COMPARISON_CONST(EQUAL));
  assert(CALLBACK_CALL(cmp, 8, 5) == COMPARISON_CONST(HIGHER));

  TCALLBACK(GetMagic) magic = Return42;
  assert(CALLBACK_CALL(magic) == 42);

  TCALLBACK(ResetGlobal) reset = ResetGlobalFlag;
  CALLBACK_CALL(reset);
  assert(g_last_code == -1);

  /* 3. Empty / Invalid */
  TCALLBACK(IntUnaryOp) emptyCb = CALLBACK_EMPTY;
  assert(!CALLBACK_IS_VALID(emptyCb));
}

static void Test_StatefulClosures(void) {
  /* 1. Size must be exactly 2 pointers (16 bytes on 64-bit platforms) */
  assert(sizeof(TCLOSURE(IntClosureFunc)) == 2 * sizeof(void *));
  assert(sizeof(TCLOSURE(IntClosureProc)) == 2 * sizeof(void *));
  assert(sizeof(TCLOSURE(IntClosurePred)) == 2 * sizeof(void *));
  assert(sizeof(TCLOSURE(IntClosureOp)) == 2 * sizeof(void *));
  assert(sizeof(TCLOSURE(IntClosureCmp)) == 2 * sizeof(void *));
  assert(sizeof(TCLOSURE(GetContextMagic)) == 2 * sizeof(void *));
  assert(sizeof(TCLOSURE(ResetContext)) == 2 * sizeof(void *));

  /* 2. Bind with real context */
  int factor = 4;
  TCLOSURE(IntClosureFunc) clMul = CLOSURE_FUNC(IntClosureFunc_Bind)(MultiplyByContext, &factor);
  assert(CLOSURE_IS_VALID(clMul));
  assert(CLOSURE_CALL(clMul, 6) == 24);
  assert(CLOSURE_INVOKE(clMul, 10) == 40);

  int sum = 0;
  TCLOSURE(IntClosureProc) clSum = CLOSURE_FUNC(IntClosureProc_Bind)(AccumulateCode, &sum);
  CLOSURE_CALL(clSum, 15);
  CLOSURE_CALL(clSum, 25);
  assert(sum == 40);

  int threshold = 18;
  TCLOSURE(IntClosurePred) clPred = CLOSURE_FUNC(IntClosurePred_Bind)(GreaterThanContext, &threshold);
  assert(CLOSURE_CALL(clPred, 20) == true);
  assert(CLOSURE_CALL(clPred, 15) == false);

  int limit = 100;
  TCLOSURE(IntClosureOp) clOp = CLOSURE_FUNC(IntClosureOp_Bind)(ContextBoundOperation, &limit);
  assert(CLOSURE_CALL(clOp, 50) == STATUS_CONST(SUCCESS));
  assert(CLOSURE_CALL(clOp, 150) == STATUS_CONST(OUT_OF_RANGE));

  bool reverse = true;
  TCLOSURE(IntClosureCmp) clCmp = CLOSURE_FUNC(IntClosureCmp_Bind)(ContextBoundComparison, &reverse);
  assert(CLOSURE_CALL(clCmp, 3, 5) == COMPARISON_CONST(HIGHER));
  assert(CLOSURE_CALL(clCmp, 5, 5) == COMPARISON_CONST(EQUAL));
  assert(CLOSURE_CALL(clCmp, 8, 5) == COMPARISON_CONST(LOWER));

  int magicVal = 777;
  TCLOSURE(GetContextMagic) clMagic = CLOSURE_FUNC(GetContextMagic_Bind)(ContextGetMagic, &magicVal);
  assert(CLOSURE_CALL(clMagic) == 777);

  int resetVal = 999;
  TCLOSURE(ResetContext) clReset = CLOSURE_FUNC(ResetContext_Bind)(ContextReset, &resetVal);
  CLOSURE_CALL(clReset);
  assert(resetVal == 0);

  /* 3. Empty / Invalid */
  TCLOSURE(IntClosureFunc) emptyCl = CLOSURE_EMPTY;
  assert(!CLOSURE_IS_VALID(emptyCl));
}

static void Test_SentinelDualDispatch(void) {
  /* Sentinel test 1: Bind with NULL context
   * Sentinel maps context to internal sentinel pointer.
   * At runtime, CLOSURE_CALL sees cl.context != NULL, unwraps
   * and invokes the function with NULL! */
  TCLOSURE(IntClosureFunc) clNullCtx =
      CLOSURE_FUNC(IntClosureFunc_Bind)(NullContextMultiplier, NULL);
  assert(CLOSURE_IS_VALID(clNullCtx));
  assert(CLOSURE_CALL(clNullCtx, 7) == 70);

  /* Sentinel test 2: Promotion from stateless callback via FromCallback
   * Sets cl.context = NULL.
   * At runtime, CLOSURE_CALL sees cl.context == NULL and invokes callback(args...) directly! */
  TCALLBACK(IntUnaryOp) cb = Square;
  TCLOSURE(IntClosureFunc) promotedCl =
      CLOSURE_FUNC(IntClosureFunc_FromCallback)(cb);
  assert(CLOSURE_IS_VALID(promotedCl));
  assert(promotedCl.context == NULL);
  assert(CLOSURE_CALL(promotedCl, 9) == 81);
}

static void Test_ObjectMethodAdapter(void) {
  MODULE_TYPE(ClosureTestModule, WorkerObject) worker;
  assert(ClosureTestModule_WorkerObject_Init(&worker) == STATUS_CONST(SUCCESS));
  worker.accumulator = 100;

  /* Use the generated adapter to bind worker object instance to a closure */
  TCLOSURE(IntClosureProc) addClosure =
      CLOSURE_AS_CLOSURE(MODULE_FUNC(ClosureTestModule, WorkerObject, Add), &worker);

  assert(CLOSURE_IS_VALID(addClosure));
  CLOSURE_CALL(addClosure, 50);
  assert(worker.accumulator == 150);

  CLOSURE_CALL(addClosure, 25);
  assert(worker.accumulator == 175);
}

static void Test_ScopedDeclarations(void) {
  /* 1. Module root scoped types */
  MODULE_CALLBACK(ClosureTestModule, Transform) modCb = Square;
  assert(modCb(8) == 64);

  int factor = 10;
  MODULE_CLOSURE(ClosureTestModule, StatefulCompute) modCl =
      NS_JOIN(MODULE_CLOSURE_FUNC(ClosureTestModule, StatefulCompute), Bind)(MultiplyByContext, &factor);
  assert(CLOSURE_CALL(modCl, 5) == 50);

  /* 2. Package scoped types */
  MODULE_CALLBACK(ClosureTestModule, TestClosurePkg, PkgTransform) pkgCb = Square;
  assert(pkgCb(3) == 9);

  int sum = 0;
  MODULE_CLOSURE(ClosureTestModule, TestClosurePkg, PkgNotify) pkgCl =
      NS_JOIN(MODULE_CLOSURE_FUNC(ClosureTestModule, TestClosurePkg, PkgNotify), Bind)(AccumulateCode, &sum);
  CLOSURE_CALL(pkgCl, 77);
  assert(sum == 77);

  /* 3. Object member scoped closure */
  int lastTaskId = 0;
  MODULE_CLOSURE(ClosureTestModule, WorkerObject, OnTask) taskCl =
      NS_JOIN(MODULE_CLOSURE_FUNC(ClosureTestModule, WorkerObject, OnTask), Bind)(WorkerTaskOp, &lastTaskId);
  assert(CLOSURE_IS_VALID(taskCl));
  assert(CLOSURE_CALL(taskCl, 42) == STATUS_CONST(SUCCESS));
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

  /* 1. Stateful closure listener */
  int localSum = 0;
  TCLOSURE(EventWorker_TaskCompleted) statefulCl =
      CLOSURE_FUNC(EventWorker_TaskCompleted_Bind)(StatefulEventListener, &localSum);

  /* 2. Stateless callback listener promoted via FromCallback */
  TCLOSURE(EventWorker_TaskCompleted) statelessCl =
      CLOSURE_FUNC(EventWorker_TaskCompleted_FromCallback)(StatelessEventListener);

  assert(OBJECT_CALL(&worker, TaskCompleted_Subscribe, statefulCl) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&worker, TaskCompleted_Subscribe, statelessCl) == STATUS_CONST(SUCCESS));
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
  assert(OBJECT_CALL(&worker, TaskCompleted_Unsubscribe, statefulCl) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&worker, TaskCompleted_Count) == 1);

  /* Emit again: only stateless should receive */
  OBJECT_CALL(&worker, TaskCompleted_Emit, 5);
  assert(g_event_stateful_received == 2); /* unchanged */
  assert(localSum == 35);                 /* unchanged */
  assert(g_event_stateless_received == 40);

  /* Unsubscribe stateless */
  assert(OBJECT_CALL(&worker, TaskCompleted_Unsubscribe, statelessCl) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&worker, TaskCompleted_Count) == 0);
}

int main(void) {
  Test_StatelessCallbacks();
  Test_StatefulClosures();
  Test_SentinelDualDispatch();
  Test_ObjectMethodAdapter();
  Test_ScopedDeclarations();
  Test_EventsDualDispatch();
  return 0;
}
