#pragma once

#include "Linked.space"
#include "Node.h"

/* ============================================================
 * Linked_List - Lista duplamente encadeada
 * Baseado em std::list da STL C++
 *
 * Usa DOUBLE_LINKED_* como base genérica.
 * ============================================================ */

/* ============================================================
 * Helpers
 * ============================================================ */

// #define LINKED_LIST_TYPE(SUFFIX) GNS2(LINKED_NS(TList), SUFFIX)
#define LINKED_LIST_TYPE(SUFFIX) GNS2(LINKED_NS(TList), SUFFIX)

#define LINKED_LIST_FUNC(SUFFIX, FUNC) GNS2(GNS2(LINKED_NS(List), SUFFIX), FUNC)

#define LINKED_LIST_NODE_TYPE(SUFFIX)                                          \
  DOUBLE_LINKED_NODE_TYPE(SUFFIX, LINKED_NS(List))

/* ============================================================
 * Struct
 * ============================================================ */

#define LINKED_LIST_STRUCT(TYPE, SUFFIX)                                       \
  CONTAINER_API_FORWARD(LINKED_LIST_FUNC(SUFFIX, FunctionTable))                           \
  typedef struct LINKED_LIST_FUNC(SUFFIX, str) {                               \
    CONTAINER_API_FIELD(LINKED_LIST_FUNC(SUFFIX, FunctionTable))             \
    LINKED_LIST_NODE_TYPE(SUFFIX) * head;                                      \
    LINKED_LIST_NODE_TYPE(SUFFIX) * tail;                                      \
    size_t size;                                                               \
  } LINKED_LIST_TYPE(SUFFIX);

/* ============================================================
 * Names and Prototypes — Lifecycle
 * ============================================================ */

#define LINKED_LIST_INIT_PROTOTYPE(SUFFIX)                            \
  inline static OPSTATUS LINKED_LIST_FUNC(SUFFIX, Init)(                       \
      LINKED_LIST_TYPE(SUFFIX) * list)

#define LINKED_LIST_DESTROY_PROTOTYPE(SUFFIX)                         \
  inline static void LINKED_LIST_FUNC(SUFFIX, Destroy)(                        \
      LINKED_LIST_TYPE(SUFFIX) * list)

/* ============================================================
 * Names and Prototypes — Element Access
 * ============================================================ */

#define LINKED_LIST_FRONT_PROTOTYPE(TYPE, SUFFIX)                     \
  inline static TYPE *LINKED_LIST_FUNC(SUFFIX,                                 \
                                       Front)(LINKED_LIST_TYPE(SUFFIX) * list)

#define LINKED_LIST_BACK_PROTOTYPE(TYPE, SUFFIX)                      \
  inline static TYPE *LINKED_LIST_FUNC(SUFFIX,                                 \
                                       Back)(LINKED_LIST_TYPE(SUFFIX) * list)

#define LINKED_LIST_BEGIN_PROTOTYPE(SUFFIX)                           \
  inline static LINKED_LIST_NODE_TYPE(SUFFIX) *                                \
      LINKED_LIST_FUNC(SUFFIX, Begin)(LINKED_LIST_TYPE(SUFFIX) * list)

#define LINKED_LIST_END_PROTOTYPE(SUFFIX)                             \
  inline static LINKED_LIST_NODE_TYPE(SUFFIX) *                                \
      LINKED_LIST_FUNC(SUFFIX, End)(LINKED_LIST_TYPE(SUFFIX) * list)

/* ============================================================
 * Names and Prototypes — Modifiers
 * ============================================================ */

#define LINKED_LIST_PUSH_FRONT_PROTOTYPE(TYPE, SUFFIX)                \
  inline static OPSTATUS LINKED_LIST_FUNC(SUFFIX, PushFront)(                  \
      LINKED_LIST_TYPE(SUFFIX) * list, TYPE value)

#define LINKED_LIST_PUSH_BACK_PROTOTYPE(TYPE, SUFFIX)                 \
  inline static OPSTATUS LINKED_LIST_FUNC(SUFFIX, PushBack)(                   \
      LINKED_LIST_TYPE(SUFFIX) * list, TYPE value)

#define LINKED_LIST_POP_FRONT_PROTOTYPE(TYPE, SUFFIX)                 \
  inline static OPSTATUS LINKED_LIST_FUNC(SUFFIX, PopFront)(                   \
      LINKED_LIST_TYPE(SUFFIX) * list, TYPE * outValue)

#define LINKED_LIST_POP_BACK_PROTOTYPE(TYPE, SUFFIX)                  \
  inline static OPSTATUS LINKED_LIST_FUNC(SUFFIX, PopBack)(                    \
      LINKED_LIST_TYPE(SUFFIX) * list, TYPE * outValue)

#define LINKED_LIST_INSERT_PROTOTYPE(TYPE, SUFFIX)                    \
  inline static OPSTATUS LINKED_LIST_FUNC(SUFFIX, Insert)(                     \
      LINKED_LIST_TYPE(SUFFIX) * list, LINKED_LIST_NODE_TYPE(SUFFIX) * pos,    \
      TYPE value, LINKED_LIST_NODE_TYPE(SUFFIX) * *outNode)

#define LINKED_LIST_ERASE_PROTOTYPE(SUFFIX)                           \
  inline static OPSTATUS LINKED_LIST_FUNC(SUFFIX, Erase)(                      \
      LINKED_LIST_TYPE(SUFFIX) * list, LINKED_LIST_NODE_TYPE(SUFFIX) * pos,    \
      LINKED_LIST_NODE_TYPE(SUFFIX) * *outNode)

#define LINKED_LIST_CLEAR_PROTOTYPE(SUFFIX)                           \
  inline static void LINKED_LIST_FUNC(SUFFIX,                                  \
                                      Clear)(LINKED_LIST_TYPE(SUFFIX) * list)

/* ============================================================
 * Names and Prototypes — Observers
 * ============================================================ */

#define LINKED_LIST_EMPTY_PROTOTYPE(SUFFIX)                           \
  inline static bool LINKED_LIST_FUNC(SUFFIX,                                  \
                                      Empty)(const LINKED_LIST_TYPE(SUFFIX) * list)

#define LINKED_LIST_SIZE_PROTOTYPE(SUFFIX)                            \
  inline static size_t LINKED_LIST_FUNC(SUFFIX,                                \
                                        Size)(const LINKED_LIST_TYPE(SUFFIX) * list)

/* ============================================================
 * Complete Declaration
 * ============================================================ */

#include "VTable/List.h"

#define LINKED_LIST_DECLARE(TYPE, SUFFIX)                                      \
  DOUBLE_LINKED_NODE_STRUCT(TYPE, SUFFIX, Linked_List)                         \
  LINKED_LIST_STRUCT(TYPE, SUFFIX)                                             \
  LINKED_LIST_INIT_PROTOTYPE(SUFFIX);                                 \
  LINKED_LIST_DESTROY_PROTOTYPE(SUFFIX);                              \
  LINKED_LIST_FRONT_PROTOTYPE(TYPE, SUFFIX);                          \
  LINKED_LIST_BACK_PROTOTYPE(TYPE, SUFFIX);                           \
  LINKED_LIST_BEGIN_PROTOTYPE(SUFFIX);                                \
  LINKED_LIST_END_PROTOTYPE(SUFFIX);                                  \
  LINKED_LIST_PUSH_FRONT_PROTOTYPE(TYPE, SUFFIX);                     \
  LINKED_LIST_PUSH_BACK_PROTOTYPE(TYPE, SUFFIX);                      \
  LINKED_LIST_POP_FRONT_PROTOTYPE(TYPE, SUFFIX);                      \
  LINKED_LIST_POP_BACK_PROTOTYPE(TYPE, SUFFIX);                       \
  LINKED_LIST_INSERT_PROTOTYPE(TYPE, SUFFIX);                         \
  LINKED_LIST_ERASE_PROTOTYPE(SUFFIX);                                \
  LINKED_LIST_CLEAR_PROTOTYPE(SUFFIX);                                \
  LINKED_LIST_EMPTY_PROTOTYPE(SUFFIX);                                \
  LINKED_LIST_SIZE_PROTOTYPE(SUFFIX);                                 \
  LINKED_LIST_IMPLEMENT(TYPE, SUFFIX)                                          \
  LINKED_LIST_FUNCTION_TABLE(TYPE, SUFFIX)                                   \
  LINKED_LIST_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

#define LINKED_LIST_INSTANCE_DECLARE(TYPE, NAME)                                                \
  LINKED_LIST_TYPE(TYPE) NAME;                                                 \
  CONTAINER_API_BIND(NAME, LINKED_LIST_FUNC(TYPE, functions));                               \
  LINKED_LIST_FUNC(TYPE, Init)(&NAME);
/* Compatibility alias: prefer LINKED_LIST_INSTANCE_DECLARE. */
#define TLinked_List(TYPE, NAME) LINKED_LIST_INSTANCE_DECLARE(TYPE, NAME)

/* ============================================================
 * Implementation
 * ============================================================ */
#include "Double/List.impl"

/* ============================================================
 * Instantiations
 * ============================================================ */
LINKED_LIST_DECLARE(int, int)
LINKED_LIST_DECLARE(float, float)
LINKED_LIST_DECLARE(double, double)
LINKED_LIST_DECLARE(char, char)
typedef void *void_ptr;
LINKED_LIST_DECLARE(void_ptr, void_ptr)
