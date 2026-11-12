#include "../../../Cosmeron/Core/Condition/Defer.h"
#include "../../../Cosmeron/Core/Condition/Exception.h"
#include "../../../Cosmeron/Core/Outcome/Status.h"
#include "../../../Cosmeron/Core/Primitive/Traits.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  int steps[8];
  size_t count;
} DeferOrderLog;

typedef struct {
  DeferOrderLog *log;
  int id;
} LogItem;

static void record_step(void *context) {
  LogItem *item = (LogItem *)context;
  assert(item->log->count < 8);
  item->log->steps[item->log->count++] = item->id;
}

typedef struct {
  int value;
  bool disposed;
} CustomResource;

static void dispose_custom_resource(void *instance) {
  CustomResource *res = (CustomResource *)instance;
  res->disposed = true;
}

static const MODULE_TYPE(Primitive, Traits) kCustomResourceTraits = {
    .name = "CustomResource",
    .size = sizeof(CustomResource),
    .alignment = _Alignof(CustomResource),
    .category = MODULE_CONST(Primitive, COMPOSITE),
    .flags = MODULE_CONST(Primitive, TraitFlag, NEEDS_DISPOSE),
    .dispose = dispose_custom_resource,
};

static int test_linear_defer_on_early_return(DeferOrderLog *log, bool failEarly) {
  LogItem first = {.log = log, .id = 1};
  LogItem second = {.log = log, .id = 2};
  LogItem third = {.log = log, .id = 3};

  DEFER(record_step, &first);
  DEFER(record_step, &second);

  if (failEarly) {
    return 42;
  }

  DEFER(record_step, &third);
  return 0;
}

static void test_linear_defer_cancel(DeferOrderLog *log, bool commit) {
  LogItem rollback = {.log = log, .id = 99};
  LogItem always = {.log = log, .id = 10};

  DEFER(record_step, &always);
  DEFER_NAMED(tx, record_step, &rollback);

  if (commit) {
    DEFER_CANCEL(tx);
  }
}

static void test_traits_defer_return(CustomResource *res) {
  DEFER_TRAITS(&kCustomResourceTraits, res);
  res->value = 123;
  return;
}

int main(void) {
  /* 1. DEFER_SCOPE: normal exit, break, continue, and nesting */
  {
    int closed = 0;
    DEFER_SCOPE(closed += 1) {
      closed += 10;
    }
    assert(closed == 11);

    int trace = 0;
    DEFER_SCOPE(trace = trace * 10 + 1) {
      DEFER_SCOPE(trace = trace * 10 + 2) {
        trace = 9;
        break;
        trace = 999; /* unreachable */
      }
      /* Outer scope continues after inner break, then exits cleanly */
      trace = trace * 10 + 3;
    }
    assert(trace == 9231);

    int contFlag = 0;
    DEFER_SCOPE(contFlag += 5) {
      contFlag = 1;
      continue;
      contFlag = 999; /* unreachable */
    }
    assert(contFlag == 6);
  }

  /* 2. DEFER_TRY_SCOPE: normal exit, break, and THROW unwinding */
  {
    int cleaned = 0;
    bool caught = false;

    DEFER_TRY_SCOPE(cleaned += 1) {
      cleaned += 10;
    }
    assert(cleaned == 11);

    cleaned = 0;
    DEFER_TRY_SCOPE(cleaned += 2) {
      cleaned += 20;
      break;
    }
    assert(cleaned == 22);

    cleaned = 0;
    TRY {
      DEFER_TRY_SCOPE(cleaned += 100) {
        THROW_STATUS(STATUS_CONST(INVALID_ARGUMENT), "Boom inside DEFER_TRY_SCOPE");
      }
    } CATCH {
      caught = true;
    }
    assert(caught == true);
    assert(cleaned == 100);
  }

  /* 3. Linear DEFER: LIFO order & early return */
  {
    DeferOrderLog log = {0};
    assert(test_linear_defer_on_early_return(&log, true) == 42);
    assert(log.count == 2);
    assert(log.steps[0] == 2);
    assert(log.steps[1] == 1);

    DeferOrderLog fullLog = {0};
    assert(test_linear_defer_on_early_return(&fullLog, false) == 0);
    assert(fullLog.count == 3);
    assert(fullLog.steps[0] == 3);
    assert(fullLog.steps[1] == 2);
    assert(fullLog.steps[2] == 1);
  }

  /* 4. DEFER_NAMED + DEFER_CANCEL */
  {
    DeferOrderLog commitLog = {0};
    test_linear_defer_cancel(&commitLog, true);
    assert(commitLog.count == 1);
    assert(commitLog.steps[0] == 10);

    DeferOrderLog rollbackLog = {0};
    test_linear_defer_cancel(&rollbackLog, false);
    assert(rollbackLog.count == 2);
    assert(rollbackLog.steps[0] == 99);
    assert(rollbackLog.steps[1] == 10);
  }

  /* 5. Linear DEFER + Exception THROW unwinding (watermark isolation) */
  {
    DeferOrderLog log = {0};
    LogItem outerItem = {.log = &log, .id = 1};
    LogItem innerFirst = {.log = &log, .id = 2};
    LogItem innerSecond = {.log = &log, .id = 3};
    bool caught = false;

    {
      DEFER(record_step, &outerItem);
      TRY {
        DEFER(record_step, &innerFirst);
        DEFER(record_step, &innerSecond);
        THROW_STATUS(STATUS_CONST(GENERIC_ERROR), "Unwind inner defers");
      } CATCH {
        caught = true;
      }
      /* Outer defer must not have run yet */
      assert(caught == true);
      assert(log.count == 2);
      assert(log.steps[0] == 3);
      assert(log.steps[1] == 2);
    }
    /* Now outer scope exited, outerItem must run */
    assert(log.count == 3);
    assert(log.steps[2] == 1);
  }

  /* 6. DEFER_TRAITS integration */
  {
    CustomResource res = {.value = 0, .disposed = false};
    test_traits_defer_return(&res);
    assert(res.value == 123);
    assert(res.disposed == true);

    CustomResource thrownRes = {.value = 77, .disposed = false};
    bool caught = false;
    TRY {
      DEFER_TRAITS(&kCustomResourceTraits, &thrownRes);
      THROW_STATUS(STATUS_CONST(GENERIC_ERROR), "Dispose via traits on throw");
    } CATCH {
      caught = true;
    }
    assert(caught == true);
    assert(thrownRes.disposed == true);
  }

  return 0;
}
