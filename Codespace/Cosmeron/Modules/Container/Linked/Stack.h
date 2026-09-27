#pragma once

#include "Linked.space"
#include "Node.h"

/* ============================================================
 * Linked_Stack — Pilha usando encadeamento simples
 *
 * LIFO: push/pop no head. O(1) em todas as operações.
 * ============================================================ */

#define LINKED_STACK_TYPE(SUFFIX) GNS2(LINKED_NS(TStack), SUFFIX)
#define LINKED_STACK_FUNC(SUFFIX, FUNC) GNS2(GNS2(LINKED_NS(Stack), SUFFIX), FUNC)

#define LINKED_STACK_NODE_TYPE(SUFFIX)                                         \
  SINGLE_LINKED_NODE_TYPE(SUFFIX, LINKED_NS(Stack))

#define LINKED_STACK_STRUCT(TYPE, SUFFIX)                                      \
  CONTAINER_API_FORWARD(LINKED_STACK_FUNC(SUFFIX, FunctionTable))                          \
  typedef struct LINKED_STACK_FUNC(SUFFIX, str) {                              \
    CONTAINER_API_FIELD(LINKED_STACK_FUNC(SUFFIX, FunctionTable))            \
    LINKED_STACK_NODE_TYPE(SUFFIX) * head;                                     \
    LINKED_STACK_NODE_TYPE(SUFFIX) * tail;                                     \
    size_t size;                                                               \
  } LINKED_STACK_TYPE(SUFFIX);

/* ============================================================
 * Names and Prototypes — Lifecycle
 * ============================================================ */

#define LINKED_STACK_INIT_PROTOTYPE(SUFFIX)                           \
  inline static OPSTATUS LINKED_STACK_FUNC(SUFFIX, Init)(                      \
      LINKED_STACK_TYPE(SUFFIX) * stack)

#define LINKED_STACK_DESTROY_PROTOTYPE(SUFFIX)                        \
  inline static void LINKED_STACK_FUNC(SUFFIX, Destroy)(                       \
      LINKED_STACK_TYPE(SUFFIX) * stack)

/* ============================================================
 * Names and Prototypes — Modifiers
 * ============================================================ */

#define LINKED_STACK_PUSH_PROTOTYPE(TYPE, SUFFIX)                     \
  inline static OPSTATUS LINKED_STACK_FUNC(SUFFIX, Push)(                      \
      LINKED_STACK_TYPE(SUFFIX) * stack, TYPE value)

#define LINKED_STACK_POP_PROTOTYPE(TYPE, SUFFIX)                      \
  inline static OPSTATUS LINKED_STACK_FUNC(SUFFIX, Pop)(                       \
      LINKED_STACK_TYPE(SUFFIX) * stack, TYPE * outValue)

#define LINKED_STACK_CLEAR_PROTOTYPE(SUFFIX)                          \
  inline static void LINKED_STACK_FUNC(SUFFIX, Clear)(                         \
      LINKED_STACK_TYPE(SUFFIX) * stack)

/* ============================================================
 * Names and Prototypes — Element Access
 * ============================================================ */

#define LINKED_STACK_TOP_PROTOTYPE(TYPE, SUFFIX)                      \
  inline static TYPE *LINKED_STACK_FUNC(SUFFIX, Top)(                          \
      LINKED_STACK_TYPE(SUFFIX) * stack)

/* ============================================================
 * Names and Prototypes — Observers
 * ============================================================ */

#define LINKED_STACK_EMPTY_PROTOTYPE(SUFFIX)                          \
  inline static bool LINKED_STACK_FUNC(SUFFIX, Empty)(                         \
      const LINKED_STACK_TYPE(SUFFIX) * stack)

#define LINKED_STACK_SIZE_PROTOTYPE(SUFFIX)                           \
  inline static size_t LINKED_STACK_FUNC(SUFFIX, Size)(                        \
      const LINKED_STACK_TYPE(SUFFIX) * stack)

#include "VTable/Stack.h"

/* ============================================================
 * Complete Declaration
 * ============================================================ */

#define LINKED_STACK_DECLARE(TYPE, SUFFIX)                                     \
  SINGLE_LINKED_NODE_STRUCT(TYPE, SUFFIX, Linked_Stack)                        \
  LINKED_STACK_STRUCT(TYPE, SUFFIX)                                            \
  LINKED_STACK_INIT_PROTOTYPE(SUFFIX);                                \
  LINKED_STACK_DESTROY_PROTOTYPE(SUFFIX);                             \
  LINKED_STACK_PUSH_PROTOTYPE(TYPE, SUFFIX);                          \
  LINKED_STACK_POP_PROTOTYPE(TYPE, SUFFIX);                           \
  LINKED_STACK_CLEAR_PROTOTYPE(SUFFIX);                               \
  LINKED_STACK_TOP_PROTOTYPE(TYPE, SUFFIX);                           \
  LINKED_STACK_EMPTY_PROTOTYPE(SUFFIX);                               \
  LINKED_STACK_SIZE_PROTOTYPE(SUFFIX);                                \
  LINKED_STACK_IMPLEMENT(TYPE, SUFFIX)                                         \
  LINKED_STACK_FUNCTION_TABLE(TYPE, SUFFIX)                                  \
  LINKED_STACK_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

/* ============================================================
 * Implementation
 * ============================================================ */
#include "Single/Stack.impl"

#define LINKED_STACK_INSTANCE_DECLARE(TYPE, NAME)                                               \
  LINKED_STACK_TYPE(TYPE) NAME;                                                \
  CONTAINER_API_BIND(NAME, LINKED_STACK_FUNC(TYPE, functions));                              \
  LINKED_STACK_FUNC(TYPE, Init)(&NAME);
/* Compatibility alias: prefer LINKED_STACK_INSTANCE_DECLARE. */
#define TLinked_Stack(TYPE, NAME) LINKED_STACK_INSTANCE_DECLARE(TYPE, NAME)

/* ============================================================
 * Instanciações
 * ============================================================ */
LINKED_STACK_DECLARE(int, int)
LINKED_STACK_DECLARE(float, float)
LINKED_STACK_DECLARE(double, double)
LINKED_STACK_DECLARE(char, char)

