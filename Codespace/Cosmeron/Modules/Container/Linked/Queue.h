#pragma once

#include "Linked.space"
#include "Node.h"

/* ============================================================
 * Linked_Queue — Fila usando encadeamento simples
 *
 * FIFO: push no tail, pop no head. O(1) em todas as operações.
 * ============================================================ */

#define LINKED_QUEUE_TYPE(SUFFIX) GNS2(LINKED_NS(TQueue), SUFFIX)

#define LINKED_QUEUE_FUNC(SUFFIX, FUNC) GNS2(GNS2(LINKED_NS(Queue), SUFFIX), FUNC)

#define LINKED_QUEUE_NODE_TYPE(SUFFIX)                                         \
  SINGLE_LINKED_NODE_TYPE(SUFFIX, LINKED_NS(Queue))

#define LINKED_QUEUE_STRUCT(TYPE, SUFFIX)                                      \
  CONTAINER_API_FORWARD(LINKED_QUEUE_FUNC(SUFFIX, FunctionTable))                          \
  typedef struct LINKED_QUEUE_FUNC(SUFFIX, str) {                              \
    CONTAINER_API_FIELD(LINKED_QUEUE_FUNC(SUFFIX, FunctionTable))            \
    LINKED_QUEUE_NODE_TYPE(SUFFIX) * head;                                     \
    LINKED_QUEUE_NODE_TYPE(SUFFIX) * tail;                                     \
    size_t size;                                                               \
  } LINKED_QUEUE_TYPE(SUFFIX);

/* ============================================================
 * Names and Prototypes — Lifecycle
 * ============================================================ */

#define LINKED_QUEUE_INIT_PROTOTYPE(SUFFIX)                           \
  inline static OPSTATUS LINKED_QUEUE_FUNC(SUFFIX, Init)(                      \
      LINKED_QUEUE_TYPE(SUFFIX) * queue)

#define LINKED_QUEUE_DESTROY_PROTOTYPE(SUFFIX)                        \
  inline static void LINKED_QUEUE_FUNC(SUFFIX, Destroy)(                       \
      LINKED_QUEUE_TYPE(SUFFIX) * queue)

/* ============================================================
 * Names and Prototypes — Modifiers
 * ============================================================ */

#define LINKED_QUEUE_PUSH_PROTOTYPE(TYPE, SUFFIX)                     \
  inline static OPSTATUS LINKED_QUEUE_FUNC(SUFFIX, Push)(                      \
      LINKED_QUEUE_TYPE(SUFFIX) * queue, TYPE value)

#define LINKED_QUEUE_POP_PROTOTYPE(TYPE, SUFFIX)                      \
  inline static OPSTATUS LINKED_QUEUE_FUNC(SUFFIX, Pop)(                       \
      LINKED_QUEUE_TYPE(SUFFIX) * queue, TYPE * outValue)

#define LINKED_QUEUE_CLEAR_PROTOTYPE(SUFFIX)                          \
  inline static void LINKED_QUEUE_FUNC(SUFFIX, Clear)(                         \
      LINKED_QUEUE_TYPE(SUFFIX) * queue)

/* ============================================================
 * Names and Prototypes — Element Access
 * ============================================================ */

#define LINKED_QUEUE_FRONT_PROTOTYPE(TYPE, SUFFIX)                    \
  inline static TYPE *LINKED_QUEUE_FUNC(SUFFIX, Front)(                        \
      LINKED_QUEUE_TYPE(SUFFIX) * queue)

#define LINKED_QUEUE_BACK_PROTOTYPE(TYPE, SUFFIX)                     \
  inline static TYPE *LINKED_QUEUE_FUNC(SUFFIX, Back)(                         \
      LINKED_QUEUE_TYPE(SUFFIX) * queue)

/* ============================================================
 * Names and Prototypes — Observers
 * ============================================================ */

#define LINKED_QUEUE_EMPTY_PROTOTYPE(SUFFIX)                          \
  inline static bool LINKED_QUEUE_FUNC(SUFFIX, Empty)(                         \
      const LINKED_QUEUE_TYPE(SUFFIX) * queue)

#define LINKED_QUEUE_SIZE_PROTOTYPE(SUFFIX)                           \
  inline static size_t LINKED_QUEUE_FUNC(SUFFIX, Size)(                        \
      const LINKED_QUEUE_TYPE(SUFFIX) * queue)

#include "VTable/Queue.h"

/* ============================================================
 * Complete Declaration
 * ============================================================ */

#define LINKED_QUEUE_DECLARE(TYPE, SUFFIX)                                     \
  SINGLE_LINKED_NODE_STRUCT(TYPE, SUFFIX, Linked_Queue)                        \
  LINKED_QUEUE_STRUCT(TYPE, SUFFIX)                                            \
  LINKED_QUEUE_INIT_PROTOTYPE(SUFFIX);                                \
  LINKED_QUEUE_DESTROY_PROTOTYPE(SUFFIX);                             \
  LINKED_QUEUE_PUSH_PROTOTYPE(TYPE, SUFFIX);                          \
  LINKED_QUEUE_POP_PROTOTYPE(TYPE, SUFFIX);                           \
  LINKED_QUEUE_CLEAR_PROTOTYPE(SUFFIX);                               \
  LINKED_QUEUE_FRONT_PROTOTYPE(TYPE, SUFFIX);                         \
  LINKED_QUEUE_BACK_PROTOTYPE(TYPE, SUFFIX);                          \
  LINKED_QUEUE_EMPTY_PROTOTYPE(SUFFIX);                               \
  LINKED_QUEUE_SIZE_PROTOTYPE(SUFFIX);                                \
  LINKED_QUEUE_IMPLEMENT(TYPE, SUFFIX)                                         \
  LINKED_QUEUE_FUNCTION_TABLE(TYPE, SUFFIX)                                  \
  LINKED_QUEUE_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

/* ============================================================
 * Implementation
 * ============================================================ */
#include "Single/Queue.impl"

#define LINKED_QUEUE_INSTANCE_DECLARE(TYPE, NAME)                                               \
  LINKED_QUEUE_TYPE(TYPE) NAME;                                                \
  CONTAINER_API_BIND(NAME, LINKED_QUEUE_FUNC(TYPE, functions));                              \
  LINKED_QUEUE_FUNC(TYPE, Init)(&NAME);
/* Compatibility alias: prefer LINKED_QUEUE_INSTANCE_DECLARE. */
#define TLinked_Queue(TYPE, NAME) LINKED_QUEUE_INSTANCE_DECLARE(TYPE, NAME)

/* ============================================================
 * Instanciações
 * ============================================================ */
LINKED_QUEUE_DECLARE(int, int)
LINKED_QUEUE_DECLARE(float, float)
LINKED_QUEUE_DECLARE(double, double)
LINKED_QUEUE_DECLARE(char, char)

