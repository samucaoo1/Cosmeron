#pragma once

#include "Graph.space"

#define GRAPH_EDGE_STRUCT(VERTEX_SUFFIX, WEIGHT_TYPE, WEIGHT_SUFFIX)         \
  typedef struct TGRAPH_EDGE_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) {            \
    size_t destination;                                                       \
    WEIGHT_TYPE weight;                                                       \
  } TGRAPH_EDGE_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);

#define GRAPH_VERTEX_STRUCT(VERTEX_TYPE, VERTEX_SUFFIX, WEIGHT_SUFFIX)       \
  typedef struct TGRAPH_VERTEX_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) {          \
    VERTEX_TYPE data;                                                         \
    TGRAPH_EDGE_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * edges;                  \
    size_t edgeCount;                                                         \
    size_t edgeCapacity;                                                      \
  } TGRAPH_VERTEX_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);

#define GRAPH_STRUCT(VERTEX_SUFFIX, WEIGHT_SUFFIX)                           \
  CONTAINER_API_FORWARD(                                                      \
      GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, FunctionTable))          \
  typedef struct TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) {                 \
    CONTAINER_API_FIELD(                                                      \
        GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, FunctionTable))        \
    TGRAPH_VERTEX_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * vertices;             \
    size_t vertexCount;                                                       \
    size_t vertexCapacity;                                                    \
    size_t edgeCount;                                                         \
  } TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);

#define GRAPH_VISITOR_TYPEDEF(VERTEX_SUFFIX, WEIGHT_SUFFIX)                  \
  typedef void (*TGRAPH_VISITOR_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX))(         \
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph,                     \
      size_t vertexIndex, void *context);

/* ============================================================
 * Prototypes — Lifecycle
 * ============================================================ */

#define GRAPH_INIT_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)                   \
  static inline OPSTATUS GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, Init)(\
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph)

#define GRAPH_DESTROY_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)                \
  static inline void GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, Destroy)( \
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph)

#define GRAPH_CLEAR_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)                  \
  static inline void GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, Clear)(   \
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph)

/* ============================================================
 * Prototypes — Modifiers
 * ============================================================ */

#define GRAPH_ADD_VERTEX_PROTOTYPE(VERTEX_TYPE, VERTEX_SUFFIX, WEIGHT_SUFFIX)\
  static inline OPSTATUS                                                      \
  GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, AddVertex)(                  \
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph, VERTEX_TYPE data,   \
      size_t *outIndex)

#define GRAPH_ADD_EDGE_PROTOTYPE(WEIGHT_TYPE, VERTEX_SUFFIX, WEIGHT_SUFFIX)  \
  static inline OPSTATUS                                                      \
  GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, AddEdge)(                    \
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph, size_t source,      \
      size_t destination, WEIGHT_TYPE weight)

#define GRAPH_REMOVE_EDGE_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)            \
  static inline OPSTATUS                                                      \
  GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, RemoveEdge)(                 \
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph, size_t source,      \
      size_t destination)

#define GRAPH_REMOVE_VERTEX_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)          \
  static inline OPSTATUS                                                      \
  GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, RemoveVertex)(               \
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph, size_t index)

/* ============================================================
 * Prototypes — Observers
 * ============================================================ */

#define GRAPH_HAS_EDGE_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)               \
  static inline bool GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, HasEdge)( \
      const TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph,               \
      size_t source, size_t destination)

#define GRAPH_FIND_WEIGHT_PROTOTYPE(WEIGHT_TYPE, VERTEX_SUFFIX, WEIGHT_SUFFIX)\
  static inline WEIGHT_TYPE *                                                 \
  GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, FindWeight)(                 \
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph, size_t source,      \
      size_t destination)

#define GRAPH_CONST_FIND_WEIGHT_PROTOTYPE(WEIGHT_TYPE, VERTEX_SUFFIX,        \
                                          WEIGHT_SUFFIX)                     \
  static inline WEIGHT_TYPE const *                                           \
  GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, ConstFindWeight)(            \
      const TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph,               \
      size_t source, size_t destination)

#define GRAPH_VERTEX_COUNT_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)           \
  static inline size_t                                                       \
  GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, VertexCount)(                \
      const TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph)

#define GRAPH_EDGE_COUNT_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)             \
  static inline size_t                                                       \
  GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, EdgeCount)(                  \
      const TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph)

#define GRAPH_IS_EMPTY_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)               \
  static inline bool GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, IsEmpty)( \
      const TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph)

/* ============================================================
 * Prototypes — Traversal
 * ============================================================ */

#define GRAPH_BFS_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)                    \
  static inline OPSTATUS GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, BFS)( \
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph, size_t start,       \
      TGRAPH_VISITOR_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) visitor,             \
      void *context)

#define GRAPH_DFS_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX)                    \
  static inline OPSTATUS GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, DFS)( \
      TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) * graph, size_t start,       \
      TGRAPH_VISITOR_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) visitor,             \
      void *context)

#include "Impl/Graph.impl"
#include "VTable/Graph.h"

/* ============================================================
 * Complete declaration
 * ============================================================ */

#define GRAPH_DEFINE(VERTEX_TYPE, VERTEX_SUFFIX, WEIGHT_TYPE, WEIGHT_SUFFIX) \
  GRAPH_EDGE_STRUCT(VERTEX_SUFFIX, WEIGHT_TYPE, WEIGHT_SUFFIX)               \
  GRAPH_VERTEX_STRUCT(VERTEX_TYPE, VERTEX_SUFFIX, WEIGHT_SUFFIX)             \
  GRAPH_STRUCT(VERTEX_SUFFIX, WEIGHT_SUFFIX)                                 \
  GRAPH_VISITOR_TYPEDEF(VERTEX_SUFFIX, WEIGHT_SUFFIX)                        \
  GRAPH_INIT_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);                        \
  GRAPH_DESTROY_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);                     \
  GRAPH_CLEAR_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);                       \
  GRAPH_ADD_VERTEX_PROTOTYPE(VERTEX_TYPE, VERTEX_SUFFIX, WEIGHT_SUFFIX);     \
  GRAPH_ADD_EDGE_PROTOTYPE(WEIGHT_TYPE, VERTEX_SUFFIX, WEIGHT_SUFFIX);       \
  GRAPH_REMOVE_EDGE_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);                 \
  GRAPH_REMOVE_VERTEX_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);               \
  GRAPH_HAS_EDGE_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);                    \
  GRAPH_FIND_WEIGHT_PROTOTYPE(WEIGHT_TYPE, VERTEX_SUFFIX, WEIGHT_SUFFIX);    \
  GRAPH_CONST_FIND_WEIGHT_PROTOTYPE(WEIGHT_TYPE, VERTEX_SUFFIX,              \
                                    WEIGHT_SUFFIX);                           \
  GRAPH_VERTEX_COUNT_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);                \
  GRAPH_EDGE_COUNT_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);                  \
  GRAPH_IS_EMPTY_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);                    \
  GRAPH_BFS_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);                         \
  GRAPH_DFS_PROTOTYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX);                         \
  GRAPH_IMPLEMENT(VERTEX_TYPE, VERTEX_SUFFIX, WEIGHT_TYPE, WEIGHT_SUFFIX)    \
  GRAPH_FUNCTION_TABLE(VERTEX_TYPE, VERTEX_SUFFIX, WEIGHT_TYPE,              \
                       WEIGHT_SUFFIX)                                        \
  GRAPH_FUNCTION_TABLE_INSTANCE(VERTEX_TYPE, VERTEX_SUFFIX, WEIGHT_TYPE,     \
                                WEIGHT_SUFFIX)

#define GRAPH_DECLARE(VERTEX_SUFFIX, WEIGHT_SUFFIX, NAME)                    \
  TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) NAME = {                         \
      CONTAINER_API_INITIALIZER(                                              \
          GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, functions))          \
      .vertices = NULL,                                                       \
      .vertexCount = 0,                                                       \
      .vertexCapacity = 0,                                                    \
      .edgeCount = 0,                                                         \
  };                                                                          \
  (void)GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, Init)(&(NAME))

/* ============================================================
 * Common instantiations
 * ============================================================ */

GRAPH_DEFINE(int, int, int, int)
GRAPH_DEFINE(int, int, float, float)
