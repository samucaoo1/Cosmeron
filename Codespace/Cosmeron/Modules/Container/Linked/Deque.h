#pragma once

#include "Linked.space"
#include "Node.h"

/* ============================================================
 * Linked_Deque — Double-ended queue usando encadeamento duplo
 *
 * O(1) em push/pop de ambas as extremidades.
 * ============================================================ */

#define LINKED_DEQUE_TYPE(SUFFIX) GNS2(LINKED_NS(TDeque), SUFFIX)

#define LINKED_DEQUE_FUNC(SUFFIX, FUNC) GNS2(GNS2(LINKED_NS(Deque), SUFFIX), FUNC)

#define LINKED_DEQUE_NODE_TYPE(SUFFIX)                                         \
  DOUBLE_LINKED_NODE_TYPE(SUFFIX, LINKED_NS(Deque))

/* ============================================================
 * Struct
 * ============================================================ */

#define LINKED_DEQUE_STRUCT(TYPE, SUFFIX)                                      \
  CONTAINER_API_FORWARD(LINKED_DEQUE_FUNC(SUFFIX, FunctionTable))                          \
  typedef struct LINKED_DEQUE_TYPE(SUFFIX) {                                   \
    CONTAINER_API_FIELD(LINKED_DEQUE_FUNC(SUFFIX, FunctionTable))            \
    LINKED_DEQUE_NODE_TYPE(SUFFIX) * head;                                     \
    LINKED_DEQUE_NODE_TYPE(SUFFIX) * tail;                                     \
    size_t size;                                                               \
  } LINKED_DEQUE_TYPE(SUFFIX);

/* ============================================================
 * Names and Prototypes — Lifecycle
 * ============================================================ */

#define LINKED_DEQUE_INIT_PROTOTYPE(SUFFIX)                           \
  inline static OPSTATUS LINKED_DEQUE_FUNC(SUFFIX, Init)(                      \
      LINKED_DEQUE_TYPE(SUFFIX) * deque)

#define LINKED_DEQUE_DESTROY_PROTOTYPE(SUFFIX)                        \
  inline static void LINKED_DEQUE_FUNC(SUFFIX, Destroy)(                       \
      LINKED_DEQUE_TYPE(SUFFIX) * deque)

/* ============================================================
 * Names and Prototypes — Modifiers
 * ============================================================ */

#define LINKED_DEQUE_PUSH_FRONT_PROTOTYPE(TYPE, SUFFIX)               \
  inline static OPSTATUS LINKED_DEQUE_FUNC(SUFFIX, PushFront)(                 \
      LINKED_DEQUE_TYPE(SUFFIX) * deque, TYPE value)

#define LINKED_DEQUE_PUSH_BACK_PROTOTYPE(TYPE, SUFFIX)                \
  inline static OPSTATUS LINKED_DEQUE_FUNC(SUFFIX, PushBack)(                  \
      LINKED_DEQUE_TYPE(SUFFIX) * deque, TYPE value)

#define LINKED_DEQUE_POP_FRONT_PROTOTYPE(TYPE, SUFFIX)                \
  inline static OPSTATUS LINKED_DEQUE_FUNC(SUFFIX, PopFront)(                  \
      LINKED_DEQUE_TYPE(SUFFIX) * deque, TYPE * outValue)

#define LINKED_DEQUE_POP_BACK_PROTOTYPE(TYPE, SUFFIX)                 \
  inline static OPSTATUS LINKED_DEQUE_FUNC(SUFFIX, PopBack)(                   \
      LINKED_DEQUE_TYPE(SUFFIX) * deque, TYPE * outValue)

#define LINKED_DEQUE_CLEAR_PROTOTYPE(SUFFIX)                          \
  inline static void LINKED_DEQUE_FUNC(SUFFIX, Clear)(                         \
      LINKED_DEQUE_TYPE(SUFFIX) * deque)

/* ============================================================
 * Names and Prototypes — Element Access
 * ============================================================ */

#define LINKED_DEQUE_FRONT_PROTOTYPE(TYPE, SUFFIX)                    \
  inline static TYPE *LINKED_DEQUE_FUNC(SUFFIX, Front)(                        \
      LINKED_DEQUE_TYPE(SUFFIX) * deque)

#define LINKED_DEQUE_BACK_PROTOTYPE(TYPE, SUFFIX)                     \
  inline static TYPE *LINKED_DEQUE_FUNC(SUFFIX, Back)(                         \
      LINKED_DEQUE_TYPE(SUFFIX) * deque)

/* ============================================================
 * Names and Prototypes — Observers
 * ============================================================ */

#define LINKED_DEQUE_EMPTY_PROTOTYPE(SUFFIX)                          \
  inline static bool LINKED_DEQUE_FUNC(SUFFIX, Empty)(                         \
      const LINKED_DEQUE_TYPE(SUFFIX) * deque)

#define LINKED_DEQUE_SIZE_PROTOTYPE(SUFFIX)                           \
  inline static size_t LINKED_DEQUE_FUNC(SUFFIX, Size)(                        \
      const LINKED_DEQUE_TYPE(SUFFIX) * deque)

#include "VTable/Deque.h"

/* ============================================================
 * Complete Declaration
 * ============================================================ */

#define LINKED_DEQUE_DECLARE(TYPE, SUFFIX)                                     \
  DOUBLE_LINKED_NODE_STRUCT(TYPE, SUFFIX, Linked_Deque)                        \
  LINKED_DEQUE_STRUCT(TYPE, SUFFIX)                                            \
  LINKED_DEQUE_INIT_PROTOTYPE(SUFFIX);                                \
  LINKED_DEQUE_DESTROY_PROTOTYPE(SUFFIX);                             \
  LINKED_DEQUE_PUSH_FRONT_PROTOTYPE(TYPE, SUFFIX);                    \
  LINKED_DEQUE_PUSH_BACK_PROTOTYPE(TYPE, SUFFIX);                     \
  LINKED_DEQUE_POP_FRONT_PROTOTYPE(TYPE, SUFFIX);                     \
  LINKED_DEQUE_POP_BACK_PROTOTYPE(TYPE, SUFFIX);                      \
  LINKED_DEQUE_CLEAR_PROTOTYPE(SUFFIX);                               \
  LINKED_DEQUE_FRONT_PROTOTYPE(TYPE, SUFFIX);                         \
  LINKED_DEQUE_BACK_PROTOTYPE(TYPE, SUFFIX);                          \
  LINKED_DEQUE_EMPTY_PROTOTYPE(SUFFIX);                               \
  LINKED_DEQUE_SIZE_PROTOTYPE(SUFFIX);                                \
  LINKED_DEQUE_IMPLEMENT(TYPE, SUFFIX)                                         \
  LINKED_DEQUE_FUNCTION_TABLE(TYPE, SUFFIX)                                  \
  LINKED_DEQUE_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

/* ============================================================
 * Implementation
 * ============================================================ */
#include "Double/Deque.impl"

#define LINKED_DEQUE_INSTANCE_DECLARE(TYPE, NAME)                                               \
  LINKED_DEQUE_TYPE(TYPE) NAME;                                                \
  CONTAINER_API_BIND(NAME, LINKED_DEQUE_FUNC(TYPE, functions));                              \
  LINKED_DEQUE_FUNC(TYPE, Init)(&NAME);
/* Compatibility alias: prefer LINKED_DEQUE_INSTANCE_DECLARE. */
#define TLinked_Deque(TYPE, NAME) LINKED_DEQUE_INSTANCE_DECLARE(TYPE, NAME)

/* ============================================================
 * Instanciações
 * ============================================================ */
LINKED_DEQUE_DECLARE(int, int)
LINKED_DEQUE_DECLARE(float, float)
LINKED_DEQUE_DECLARE(double, double)
LINKED_DEQUE_DECLARE(char, char)

