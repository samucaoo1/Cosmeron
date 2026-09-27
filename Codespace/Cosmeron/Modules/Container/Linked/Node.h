#pragma once

#include "Linked.space"

/* ============================================================
 * Linked Node Core Contracts
 *
 * - CONTAINER is the API seed (e.g. Linked_Stack, Linked_List).
 * - SUFFIX is the concrete data/type suffix (e.g. int, float).
 * - Generated node structs are container-scoped by design.
 * ============================================================ */

#define LINKED_NODE_TYPE(CONTAINER, SUFFIX) GNS3(CONTAINER, TNode, SUFFIX)

#define LINKED_NODE_VALUE_FIELD(TYPE) TYPE value

/* ============================================================
 * Single linked node family
 * ============================================================ */

#define SINGLE_LINKED_NODE_TYPE(SUFFIX, CONTAINER)                             \
  LINKED_NODE_TYPE(CONTAINER, SUFFIX)

#define SINGLE_LINKED_NODE_STRUCT(TYPE, SUFFIX, CONTAINER)                     \
  typedef struct SINGLE_LINKED_NODE_TYPE(SUFFIX, CONTAINER) {                  \
    LINKED_NODE_VALUE_FIELD(TYPE);                                             \
    struct SINGLE_LINKED_NODE_TYPE(SUFFIX, CONTAINER) * next;                  \
  } SINGLE_LINKED_NODE_TYPE(SUFFIX, CONTAINER);

/* ============================================================
 * Double linked node family
 * ============================================================ */

#define DOUBLE_LINKED_NODE_TYPE(SUFFIX, CONTAINER)                             \
  LINKED_NODE_TYPE(CONTAINER, SUFFIX)

#define DOUBLE_LINKED_NODE_STRUCT(TYPE, SUFFIX, CONTAINER)                     \
  typedef struct DOUBLE_LINKED_NODE_TYPE(SUFFIX, CONTAINER) {                  \
    LINKED_NODE_VALUE_FIELD(TYPE);                                             \
    struct DOUBLE_LINKED_NODE_TYPE(SUFFIX, CONTAINER) * prev;                  \
    struct DOUBLE_LINKED_NODE_TYPE(SUFFIX, CONTAINER) * next;                  \
  } DOUBLE_LINKED_NODE_TYPE(SUFFIX, CONTAINER);
