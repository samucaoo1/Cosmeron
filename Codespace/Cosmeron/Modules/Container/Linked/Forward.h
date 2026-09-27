#pragma once

#include "Linked.space"
#include "Node.h"

#define LINKED_FORWARD_LIST_TYPE(SUFFIX) GNS2(LINKED_NS(TForwardList), SUFFIX)

#define LINKED_FORWARD_LIST_FUNC(SUFFIX, FUNC)                                 \
  GNS2(GNS2(LINKED_NS(ForwardList), SUFFIX), FUNC)

#define LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX)                                  \
  SINGLE_LINKED_NODE_TYPE(SUFFIX, LINKED_NS(ForwardList))

/* ============================================================
 * Struct
 * ============================================================ */

#define LINKED_FORWARD_LIST_STRUCT(TYPE, SUFFIX)                               \
  CONTAINER_API_FORWARD(LINKED_FORWARD_LIST_FUNC(SUFFIX, FunctionTable))                   \
  typedef struct LINKED_FORWARD_LIST_FUNC(SUFFIX, str) {                       \
    CONTAINER_API_FIELD(LINKED_FORWARD_LIST_FUNC(SUFFIX, FunctionTable))     \
    LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) * head;                              \
    LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) * tail;                              \
    size_t size;                                                               \
  } LINKED_FORWARD_LIST_TYPE(SUFFIX);

/* ============================================================
 * Names and Prototypes — Lifecycle
 * ============================================================ */

#define LINKED_FORWARD_LIST_INIT_PROTOTYPE(SUFFIX)                    \
  inline static OPSTATUS LINKED_FORWARD_LIST_FUNC(SUFFIX, Init)(               \
      LINKED_FORWARD_LIST_TYPE(SUFFIX) * container)

#define LINKED_FORWARD_LIST_DESTROY_PROTOTYPE(SUFFIX)                 \
  inline static void LINKED_FORWARD_LIST_FUNC(SUFFIX, Destroy)(                \
      LINKED_FORWARD_LIST_TYPE(SUFFIX) * container)

/* ============================================================
 * Names and Prototypes — Modifiers
 * ============================================================ */

#define LINKED_FORWARD_LIST_PUSH_FRONT_PROTOTYPE(TYPE, SUFFIX)        \
  inline static OPSTATUS LINKED_FORWARD_LIST_FUNC(SUFFIX, PushFront)(          \
      LINKED_FORWARD_LIST_TYPE(SUFFIX) * container, TYPE value)

#define LINKED_FORWARD_LIST_POP_FRONT_PROTOTYPE(TYPE, SUFFIX)         \
  inline static OPSTATUS LINKED_FORWARD_LIST_FUNC(SUFFIX, PopFront)(           \
      LINKED_FORWARD_LIST_TYPE(SUFFIX) * container, TYPE * outValue)

#define LINKED_FORWARD_LIST_INSERT_AFTER_PROTOTYPE(TYPE, SUFFIX)      \
  inline static OPSTATUS LINKED_FORWARD_LIST_FUNC(SUFFIX, InsertAfter)(        \
      LINKED_FORWARD_LIST_TYPE(SUFFIX) * container,                            \
      LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) * pos, TYPE value,                 \
      LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) * *outNode)

#define LINKED_FORWARD_LIST_ERASE_AFTER_PROTOTYPE(SUFFIX)             \
  inline static OPSTATUS LINKED_FORWARD_LIST_FUNC(SUFFIX, EraseAfter)(         \
      LINKED_FORWARD_LIST_TYPE(SUFFIX) * container,                            \
      LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) * pos)

#define LINKED_FORWARD_LIST_CLEAR_PROTOTYPE(SUFFIX)                   \
  inline static void LINKED_FORWARD_LIST_FUNC(SUFFIX, Clear)(                  \
      LINKED_FORWARD_LIST_TYPE(SUFFIX) * container)

/* ============================================================
 * Names and Prototypes — Element Access
 * ============================================================ */

#define LINKED_FORWARD_LIST_FRONT_PROTOTYPE(TYPE, SUFFIX)             \
  inline static TYPE *LINKED_FORWARD_LIST_FUNC(SUFFIX, Front)(                 \
      LINKED_FORWARD_LIST_TYPE(SUFFIX) * container)

#define LINKED_FORWARD_LIST_BACK_PROTOTYPE(TYPE, SUFFIX)              \
  inline static TYPE *LINKED_FORWARD_LIST_FUNC(SUFFIX, Back)(                  \
      LINKED_FORWARD_LIST_TYPE(SUFFIX) * container)

#define LINKED_FORWARD_LIST_BEGIN_PROTOTYPE(SUFFIX)                   \
  inline static LINKED_FORWARD_LIST_NODE_TYPE(SUFFIX) *                        \
      LINKED_FORWARD_LIST_FUNC(SUFFIX, Begin)(                                 \
          LINKED_FORWARD_LIST_TYPE(SUFFIX) * container)

/* ============================================================
 * Names and Prototypes — Observers
 * ============================================================ */

#define LINKED_FORWARD_LIST_EMPTY_PROTOTYPE(SUFFIX)                   \
  inline static bool LINKED_FORWARD_LIST_FUNC(SUFFIX, Empty)(                  \
      const LINKED_FORWARD_LIST_TYPE(SUFFIX) * container)

#define LINKED_FORWARD_LIST_SIZE_PROTOTYPE(SUFFIX)                    \
  inline static size_t LINKED_FORWARD_LIST_FUNC(SUFFIX, Size)(                 \
      const LINKED_FORWARD_LIST_TYPE(SUFFIX) * container)

#include "VTable/Forward.h"

/* ============================================================
 * Linked_ForwardList — Lista simplesmente encadeada
 *
 * Inserção O(1) no head. O(n) no tail.
 * ============================================================ */

/* ============================================================
 * Complete Declaration
 * ============================================================ */

#define LINKED_FORWARD_LIST_DECLARE(TYPE, SUFFIX)                              \
  SINGLE_LINKED_NODE_STRUCT(TYPE, SUFFIX, Linked_ForwardList)                  \
  LINKED_FORWARD_LIST_STRUCT(TYPE, SUFFIX)                                     \
  LINKED_FORWARD_LIST_INIT_PROTOTYPE(SUFFIX);                         \
  LINKED_FORWARD_LIST_DESTROY_PROTOTYPE(SUFFIX);                      \
  LINKED_FORWARD_LIST_PUSH_FRONT_PROTOTYPE(TYPE, SUFFIX);             \
  LINKED_FORWARD_LIST_POP_FRONT_PROTOTYPE(TYPE, SUFFIX);              \
  LINKED_FORWARD_LIST_INSERT_AFTER_PROTOTYPE(TYPE, SUFFIX);           \
  LINKED_FORWARD_LIST_ERASE_AFTER_PROTOTYPE(SUFFIX);                  \
  LINKED_FORWARD_LIST_CLEAR_PROTOTYPE(SUFFIX);                        \
  LINKED_FORWARD_LIST_FRONT_PROTOTYPE(TYPE, SUFFIX);                  \
  LINKED_FORWARD_LIST_BEGIN_PROTOTYPE(SUFFIX);                        \
  LINKED_FORWARD_LIST_EMPTY_PROTOTYPE(SUFFIX);                        \
  LINKED_FORWARD_LIST_SIZE_PROTOTYPE(SUFFIX);                         \
  LINKED_FORWARD_LIST_IMPLEMENT(TYPE, SUFFIX)                                  \
  LINKED_FORWARD_LIST_FUNCTION_TABLE(TYPE, SUFFIX)                           \
  LINKED_FORWARD_LIST_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

/* ============================================================
 * Implementation
 * ============================================================ */
#include "Single/Forward.impl"

#define LINKED_FORWARD_LIST_INSTANCE_DECLARE(TYPE, NAME)                                         \
  LINKED_FORWARD_LIST_TYPE(TYPE) NAME;                                         \
  CONTAINER_API_BIND(NAME, LINKED_FORWARD_LIST_FUNC(TYPE, functions));                       \
  LINKED_FORWARD_LIST_FUNC(TYPE, Init)(&NAME);
/* Compatibility alias: prefer LINKED_FORWARD_LIST_INSTANCE_DECLARE. */
#define TLinked_ForwardList(TYPE, NAME) LINKED_FORWARD_LIST_INSTANCE_DECLARE(TYPE, NAME)

/* ============================================================
 * Instanciações
 * ============================================================ */
LINKED_FORWARD_LIST_DECLARE(int, int)
LINKED_FORWARD_LIST_DECLARE(float, float)
LINKED_FORWARD_LIST_DECLARE(double, double)
LINKED_FORWARD_LIST_DECLARE(char, char)

