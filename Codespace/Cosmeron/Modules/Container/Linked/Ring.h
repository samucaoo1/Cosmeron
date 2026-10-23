#pragma once

#include "Linked.space"
#include "Node.h"

/* ============================================================
 * LINKED_NS(Ring) — Bounded Circular Linked Ring Buffer
 *
 * FIFO: O(1) push, pop, overwrite, front, back.
 * Fixed node cycle: zero dynamic allocation during operation.
 * ============================================================ */

#define LINKED_RING_TYPE(SUFFIX) GNS2(LINKED_NS(TRing), SUFFIX)
#define LINKED_RING_NS(SUFFIX) GNS2(LINKED_NS(Ring), SUFFIX)
#define LINKED_RING_FUNC(SUFFIX, FUNC) GNS2(LINKED_RING_NS(SUFFIX), FUNC)
#define LINKED_RING_STRUCT_TAG(SUFFIX) GNS2(LINKED_RING_NS(SUFFIX), str)
#define LINKED_RING_FUNCTION_TABLE_TYPE(SUFFIX)                                \
  GNS2(LINKED_RING_NS(SUFFIX), FunctionTable)

#define LINKED_RING_NODE_TYPE(SUFFIX)                                          \
  SINGLE_LINKED_NODE_TYPE(SUFFIX, LINKED_NS(Ring))

#define LINKED_RING_STRUCT(TYPE, SUFFIX)                                       \
  CONTAINER_API_FORWARD(LINKED_RING_FUNCTION_TABLE_TYPE(SUFFIX))               \
  typedef struct LINKED_RING_STRUCT_TAG(SUFFIX) {                              \
    CONTAINER_API_FIELD(LINKED_RING_FUNCTION_TABLE_TYPE(SUFFIX))               \
    LINKED_RING_NODE_TYPE(SUFFIX) * head;                                      \
    LINKED_RING_NODE_TYPE(SUFFIX) * tail;                                      \
    LINKED_RING_NODE_TYPE(SUFFIX) * back;                                      \
    size_t size;                                                               \
    size_t capacity;                                                           \
    bool isOwned;                                                              \
  } LINKED_RING_TYPE(SUFFIX);

/* ============================================================
 * Names and Prototypes — Lifecycle
 * ============================================================ */

#define LINKED_RING_INIT_PROTOTYPE(SUFFIX)                                     \
  static inline OPSTATUS LINKED_RING_FUNC(SUFFIX, Init)(                       \
      LINKED_RING_TYPE(SUFFIX) * ring, size_t capacity)

#define LINKED_RING_INIT_STATIC_PROTOTYPE(SUFFIX)                              \
  static inline OPSTATUS LINKED_RING_FUNC(SUFFIX, InitStatic)(                 \
      LINKED_RING_TYPE(SUFFIX) * ring,                                         \
      LINKED_RING_NODE_TYPE(SUFFIX) * nodes, size_t capacity)

#define LINKED_RING_DESTROY_PROTOTYPE(SUFFIX)                                  \
  static inline void LINKED_RING_FUNC(SUFFIX, Destroy)(                        \
      LINKED_RING_TYPE(SUFFIX) * ring)

#define LINKED_RING_CLEAR_PROTOTYPE(SUFFIX)                                    \
  static inline void LINKED_RING_FUNC(SUFFIX, Clear)(                          \
      LINKED_RING_TYPE(SUFFIX) * ring)

/* ============================================================
 * Names and Prototypes — Modifiers
 * ============================================================ */

#define LINKED_RING_PUSH_PROTOTYPE(TYPE, SUFFIX)                               \
  static inline OPSTATUS LINKED_RING_FUNC(SUFFIX, Push)(                       \
      LINKED_RING_TYPE(SUFFIX) * ring, TYPE value)

#define LINKED_RING_POP_PROTOTYPE(TYPE, SUFFIX)                                \
  static inline OPSTATUS LINKED_RING_FUNC(SUFFIX, Pop)(                        \
      LINKED_RING_TYPE(SUFFIX) * ring, TYPE * outValue)

#define LINKED_RING_PEEK_PROTOTYPE(TYPE, SUFFIX)                               \
  static inline OPSTATUS LINKED_RING_FUNC(SUFFIX, Peek)(                       \
      const LINKED_RING_TYPE(SUFFIX) * ring, TYPE * outValue)

#define LINKED_RING_OVERWRITE_PROTOTYPE(TYPE, SUFFIX)                          \
  static inline OPSTATUS LINKED_RING_FUNC(SUFFIX, Overwrite)(                   \
      LINKED_RING_TYPE(SUFFIX) * ring, TYPE value, TYPE * outDropped)

/* ============================================================
 * Names and Prototypes — Element Access
 * ============================================================ */

#define LINKED_RING_FRONT_PROTOTYPE(TYPE, SUFFIX)                              \
  static inline OPSTATUS LINKED_RING_FUNC(SUFFIX, Front)(                       \
      LINKED_RING_TYPE(SUFFIX) * ring, TYPE **out)

#define LINKED_RING_BACK_PROTOTYPE(TYPE, SUFFIX)                               \
  static inline OPSTATUS LINKED_RING_FUNC(SUFFIX, Back)(                        \
      LINKED_RING_TYPE(SUFFIX) * ring, TYPE **out)

#define LINKED_RING_AT_PROTOTYPE(TYPE, SUFFIX)                                 \
  static inline OPSTATUS LINKED_RING_FUNC(SUFFIX, At)(                         \
      LINKED_RING_TYPE(SUFFIX) * ring, size_t index, TYPE **out)

#define LINKED_RING_EMPTY_PROTOTYPE(SUFFIX)                                    \
  static inline bool LINKED_RING_FUNC(SUFFIX, Empty)(                          \
      const LINKED_RING_TYPE(SUFFIX) * ring)

#define LINKED_RING_FULL_PROTOTYPE(SUFFIX)                                     \
  static inline bool LINKED_RING_FUNC(SUFFIX, Full)(                           \
      const LINKED_RING_TYPE(SUFFIX) * ring)

#define LINKED_RING_SIZE_PROTOTYPE(SUFFIX)                                     \
  static inline size_t LINKED_RING_FUNC(SUFFIX, Size)(                         \
      const LINKED_RING_TYPE(SUFFIX) * ring)

#define LINKED_RING_CAPACITY_PROTOTYPE(SUFFIX)                                 \
  static inline size_t LINKED_RING_FUNC(SUFFIX, Capacity)(                     \
      const LINKED_RING_TYPE(SUFFIX) * ring)

#define LINKED_RING_AVAILABLE_PROTOTYPE(SUFFIX)                                \
  static inline size_t LINKED_RING_FUNC(SUFFIX, Available)(                    \
      const LINKED_RING_TYPE(SUFFIX) * ring)

#include "FunctionTable/Ring.h"

/* ============================================================
 * Complete Declaration
 * ============================================================ */

#define LINKED_RING_IMPLEMENT_ALL(TYPE, SUFFIX)                                \
  SINGLE_LINKED_NODE_STRUCT(TYPE, SUFFIX, LINKED_NS(Ring))                     \
  LINKED_RING_STRUCT(TYPE, SUFFIX)                                             \
  LINKED_RING_INIT_PROTOTYPE(SUFFIX);                                          \
  LINKED_RING_INIT_STATIC_PROTOTYPE(SUFFIX);                                   \
  LINKED_RING_DESTROY_PROTOTYPE(SUFFIX);                                       \
  LINKED_RING_CLEAR_PROTOTYPE(SUFFIX);                                         \
  LINKED_RING_PUSH_PROTOTYPE(TYPE, SUFFIX);                                    \
  LINKED_RING_POP_PROTOTYPE(TYPE, SUFFIX);                                     \
  LINKED_RING_PEEK_PROTOTYPE(TYPE, SUFFIX);                                    \
  LINKED_RING_OVERWRITE_PROTOTYPE(TYPE, SUFFIX);                               \
  LINKED_RING_FRONT_PROTOTYPE(TYPE, SUFFIX);                                   \
  LINKED_RING_BACK_PROTOTYPE(TYPE, SUFFIX);                                    \
  LINKED_RING_AT_PROTOTYPE(TYPE, SUFFIX);                                      \
  LINKED_RING_EMPTY_PROTOTYPE(SUFFIX);                                         \
  LINKED_RING_FULL_PROTOTYPE(SUFFIX);                                          \
  LINKED_RING_SIZE_PROTOTYPE(SUFFIX);                                          \
  LINKED_RING_CAPACITY_PROTOTYPE(SUFFIX);                                      \
  LINKED_RING_AVAILABLE_PROTOTYPE(SUFFIX);                                     \
  COSMERON_MACRO_INTERNAL_LINKED_RING_IMPLEMENT(TYPE, SUFFIX)                  \
  LINKED_RING_FUNCTION_TABLE_STRUCT(TYPE, SUFFIX)                              \
  LINKED_RING_FUNCTION_TABLE_INSTANCE(TYPE, SUFFIX)

/* ============================================================
 * Implementation
 * ============================================================ */
#include "Single/Ring.impl"

#define LINKED_RING_INSTANCE_DECLARE(TYPE, NAME, CAPACITY)                     \
  LINKED_RING_TYPE(TYPE) NAME;                                                 \
  CONTAINER_API_BIND(NAME, LINKED_RING_FUNC(TYPE, functions));                \
  LINKED_RING_FUNC(TYPE, Init)(&NAME, (CAPACITY));

#define LINKED_RING_STATIC_DECLARE(TYPE, NAME, NODES, CAPACITY)                \
  LINKED_RING_TYPE(TYPE) NAME;                                                 \
  CONTAINER_API_BIND(NAME, LINKED_RING_FUNC(TYPE, functions));                \
  LINKED_RING_FUNC(TYPE, InitStatic)(&NAME, (NODES), (CAPACITY));

/* Compatibility alias */
#define TLinked_Ring(TYPE, NAME, CAPACITY)                                     \
  LINKED_RING_INSTANCE_DECLARE(TYPE, NAME, CAPACITY)

/* ============================================================
 * Instantiations
 * ============================================================ */
LINKED_RING_IMPLEMENT_ALL(int, int)
LINKED_RING_IMPLEMENT_ALL(float, float)
LINKED_RING_IMPLEMENT_ALL(double, double)
LINKED_RING_IMPLEMENT_ALL(char, char)
LINKED_RING_IMPLEMENT_ALL(uint8_t, byte)
LINKED_RING_IMPLEMENT_ALL(uint8_t, uint8)
