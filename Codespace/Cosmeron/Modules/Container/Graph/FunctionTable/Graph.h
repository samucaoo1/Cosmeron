#pragma once

#if CONTAINER_ENABLE_PSEUDO_METHODS

#define GRAPH_FUNCTION_TABLE(VERTEX_TYPE, VERTEX_SUFFIX, WEIGHT_TYPE,        \
                             WEIGHT_SUFFIX)                                  \
  typedef struct GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, FunctionTable) {\
    OPSTATUS (*init)(TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *);           \
    void (*destroy)(TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *);            \
    void (*clear)(TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *);              \
    OPSTATUS (*addVertex)(TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *,       \
                          VERTEX_TYPE, size_t *);                             \
    OPSTATUS (*addEdge)(TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *,         \
                        size_t, size_t, WEIGHT_TYPE);                         \
    OPSTATUS (*removeEdge)(TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *,      \
                           size_t, size_t);                                   \
    OPSTATUS (*removeVertex)(TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *,    \
                             size_t);                                         \
    bool (*hasEdge)(const TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *,       \
                    size_t, size_t);                                         \
    WEIGHT_TYPE *(*findWeight)(                                              \
        TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *, size_t, size_t);        \
    WEIGHT_TYPE const *(*constFindWeight)(                                   \
        const TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *, size_t, size_t);  \
    size_t (*vertexCount)(                                                    \
        const TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *);                  \
    size_t (*edgeCount)(                                                      \
        const TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *);                  \
    bool (*isEmpty)(const TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *);      \
    OPSTATUS (*bfs)(TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *, size_t,     \
                    TGRAPH_VISITOR_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX),       \
                    void *);                                                 \
    OPSTATUS (*dfs)(TGRAPH_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX) *, size_t,     \
                    TGRAPH_VISITOR_TYPE(VERTEX_SUFFIX, WEIGHT_SUFFIX),       \
                    void *);                                                 \
  } GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, FunctionTable);

#define GRAPH_FUNCTION_TABLE_INSTANCE(VERTEX_TYPE, VERTEX_SUFFIX,            \
                                      WEIGHT_TYPE, WEIGHT_SUFFIX)            \
  static const GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, FunctionTable)  \
      GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, functions) = {           \
          .init = GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, Init),        \
          .destroy = GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, Destroy),  \
          .clear = GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, Clear),      \
          .addVertex =                                                        \
              GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, AddVertex),       \
          .addEdge = GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, AddEdge),  \
          .removeEdge =                                                       \
              GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, RemoveEdge),      \
          .removeVertex =                                                     \
              GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, RemoveVertex),    \
          .hasEdge = GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, HasEdge),  \
          .findWeight =                                                       \
              GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, FindWeight),      \
          .constFindWeight =                                                  \
              GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, ConstFindWeight), \
          .vertexCount =                                                      \
              GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, VertexCount),     \
          .edgeCount =                                                        \
              GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, EdgeCount),       \
          .isEmpty = GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, IsEmpty),  \
          .bfs = GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, BFS),          \
          .dfs = GRAPH_OPERATION(VERTEX_SUFFIX, WEIGHT_SUFFIX, DFS),          \
      };

#else

#define GRAPH_FUNCTION_TABLE(...)
#define GRAPH_FUNCTION_TABLE_INSTANCE(...)

#endif
