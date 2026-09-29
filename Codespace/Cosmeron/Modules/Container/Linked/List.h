#pragma once

#include "Linked.space"
#include "Node.h"

/* ============================================================
 * LINKED_NS(List) - Lista duplamente encadeada
 * Baseado em std::list da STL C++
 *
 * Usa DOUBLE_LINKED_* como base genérica.
 * ============================================================ */

/* ============================================================
 * Helpers
 * ============================================================ */

// #define LINKED_LIST_TYPE(SUFFIX) GNS2(LINKED_NS(TList), SUFFIX)
#define LINKED_LIST_TYPE(SUFFIX) GNS2(LINKED_NS(TList), SUFFIX)

#define LINKED_LIST_NS(SUFFIX) GNS2(LINKED_NS(List), SUFFIX)
#define LINKED_LIST_FUNC(SUFFIX, FUNC) GNS2(LINKED_LIST_NS(SUFFIX), FUNC)
#define LINKED_LIST_STRUCT_TAG(SUFFIX) GNS2(LINKED_LIST_NS(SUFFIX), str)
#define LINKED_LIST_FUNCTION_TABLE_TYPE(SUFFIX) \
  GNS2(LINKED_LIST_NS(SUFFIX), FunctionTable)

#define LINKED_LIST_NODE_TYPE(SUFFIX)                                          \
  DOUBLE_LINKED_NODE_TYPE(SUFFIX, LINKED_NS(List))

/* ============================================================
 * Struct
 * ============================================================ */

#define LINKED_LIST_STRUCT(TYPE, SUFFIX)                                       \
  CONTAINER_API_FORWARD(LINKED_LIST_FUNCTION_TABLE_TYPE(SUFFIX))                           \
  typedef struct LINKED_LIST_STRUCT_TAG(SUFFIX) {                               \
    CONTAINER_API_FIELD(LINKED_LIST_FUNCTION_TABLE_TYPE(SUFFIX))             \
    LINKED_LIST_NODE_TYPE(SUFFIX) * head;                                      \
    LINKED_LIST_NODE_TYPE(SUFFIX) * tail;                                      \
    size_t size;                                                               \
  } LINKED_LIST_TYPE(SUFFIX);

/* ============================================================
 * Names and Prototypes — Lifecycle
 * ============================================================ */

#define LINKED_LIST_INIT_PROTOTYPE(SUFFIX)                            \
  static inline OPSTATUS LINKED_LIST_FUNC(SUFFIX, Init)(                       \
      LINKED_LIST_TYPE(SUFFIX) * list)

#define LINKED_LIST_DESTROY_PROTOTYPE(SUFFIX)                         \
  static inline void LINKED_LIST_FUNC(SUFFIX, Destroy)(                        \
      LINKED_LIST_TYPE(SUFFIX) * list)

/* ============================================================
 * Names and Prototypes — Element Access
 * ============================================================ */

#define LINKED_LIST_FRONT_PROTOTYPE(TYPE, SUFFIX)                     \
  static inline OPSTATUS LINKED_LIST_FUNC(SUFFIX, Front)(                      \
      LINKED_LIST_TYPE(SUFFIX) * list, TYPE **out)

#define LINKED_LIST_BACK_PROTOTYPE(TYPE, SUFFIX)                      \
  static inline OPSTATUS LINKED_LIST_FUNC(SUFFIX, Back)(                       \
      LINKED_LIST_TYPE(SUFFIX) * list, TYPE **out)

#define LINKED_LIST_BEGIN_PROTOTYPE(SUFFIX)                           \
  static inline LINKED_LIST_NODE_TYPE(SUFFIX) *                                \
      LINKED_LIST_FUNC(SUFFIX, Begin)(LINKED_LIST_TYPE(SUFFIX) * list)

#define LINKED_LIST_END_PROTOTYPE(SUFFIX)                             \
  static inline LINKED_LIST_NODE_TYPE(SUFFIX) *                                \
      LINKED_LIST_FUNC(SUFFIX, End)(LINKED_LIST_TYPE(SUFFIX) * list)

/* ============================================================
 * Names and Prototypes — Modifiers
 * ============================================================ */

#define LINKED_LIST_PUSH_FRONT_PROTOTYPE(TYPE, SUFFIX)                \
  static inline OPSTATUS LINKED_LIST_FUNC(SUFFIX, PushFront)(                  \
      LINKED_LIST_TYPE(SUFFIX) * list, TYPE value)

#define LINKED_LIST_PUSH_BACK_PROTOTYPE(TYPE, SUFFIX)                 \
  static inline OPSTATUS LINKED_LIST_FUNC(SUFFIX, PushBack)(                   \
      LINKED_LIST_TYPE(SUFFIX) * list, TYPE value)

#define LINKED_LIST_POP_FRONT_PROTOTYPE(TYPE, SUFFIX)                 \
  static inline OPSTATUS LINKED_LIST_FUNC(SUFFIX, PopFront)(                   \
      LINKED_LIST_TYPE(SUFFIX) * list, TYPE * outValue)

#define LINKED_LIST_POP_BACK_PROTOTYPE(TYPE, SUFFIX)                  \
  static inline OPSTATUS LINKED_LIST_FUNC(SUFFIX, PopBack)(                    \
      LINKED_LIST_TYPE(SUFFIX) * list, TYPE * outValue)

#define LINKED_LIST_INSERT_PROTOTYPE(TYPE, SUFFIX)                    \
  static inline OPSTATUS LINKED_LIST_FUNC(SUFFIX, Insert)(                     \
      LINKED_LIST_TYPE(SUFFIX) * list, LINKED_LIST_NODE_TYPE(SUFFIX) * pos,    \
      TYPE value, LINKED_LIST_NODE_TYPE(SUFFIX) * *outNode)

#define LINKED_LIST_ERASE_PROTOTYPE(SUFFIX)                           \
  static inline OPSTATUS LINKED_LIST_FUNC(SUFFIX, Erase)(                      \
      LINKED_LIST_TYPE(SUFFIX) * list, LINKED_LIST_NODE_TYPE(SUFFIX) * pos,    \
      LINKED_LIST_NODE_TYPE(SUFFIX) * *outNode)

#define LINKED_LIST_CLEAR_PROTOTYPE(SUFFIX)                           \
  static inline void LINKED_LIST_FUNC(SUFFIX,                                  \
                                      Clear)(LINKED_LIST_TYPE(SUFFIX) * list)

/* ============================================================
 * Names and Prototypes — Observers
 * ============================================================ */

#define LINKED_LIST_EMPTY_PROTOTYPE(SUFFIX)                           \
  static inline bool LINKED_LIST_FUNC(SUFFIX,                                  \
                                      Empty)(const LINKED_LIST_TYPE(SUFFIX) * list)

#define LINKED_LIST_SIZE_PROTOTYPE(SUFFIX)                            \
  static inline size_t LINKED_LIST_FUNC(SUFFIX,                                \
                                        Size)(const LINKED_LIST_TYPE(SUFFIX) * list)

/* ============================================================
 * Complete Declaration
 * ============================================================ */

#include "FunctionTable/List.h"

#define LINKED_LIST_IMPLEMENT_ALL(TYPE, SUFFIX)                                      \
  DOUBLE_LINKED_NODE_STRUCT(TYPE, SUFFIX, LINKED_NS(List))                         \
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
  COSMERON_MACRO_INTERNAL_LINKED_LIST_IMPLEMENT(TYPE, SUFFIX)                                          \
  LINKED_LIST_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                                   \
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
LINKED_LIST_IMPLEMENT_ALL(int, int)
LINKED_LIST_IMPLEMENT_ALL(float, float)
LINKED_LIST_IMPLEMENT_ALL(double, double)
LINKED_LIST_IMPLEMENT_ALL(char, char)
typedef void *CONTAINER_INS(VoidPointer_InternalType_);
LINKED_LIST_IMPLEMENT_ALL(CONTAINER_INS(VoidPointer_InternalType_), void_ptr)
