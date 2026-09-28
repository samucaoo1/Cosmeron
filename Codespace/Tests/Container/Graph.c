#include "Test.h"

#include "../../Cosmeron/Modules/Container/Graph/Graph.h"

typedef struct TVisitLog {
  size_t values[16];
  size_t count;
} TVisitLog;

static void VisitIntGraph(TGRAPH_TYPE(int, int) *graph, size_t vertexIndex,
                          void *context) {
  TVisitLog *log = (TVisitLog *)context;
  (void)graph;
  if (log->count < 16u)
    log->values[log->count++] = vertexIndex;
}

int main(void) {
  TGRAPH_TYPE(int, int) graph = {0};
  size_t a, b, c, d;
  int *weight;
  const int *constWeight;
  TVisitLog bfs = {{0}, 0};
  TVisitLog dfs = {{0}, 0};

  CONTAINER_API_BIND(graph, GRAPH_OPERATION(int, int, functions));

  TEST_ASSERT(GRAPH_OPERATION(int, int, Init)(&graph) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(GRAPH_OPERATION(int, int, IsEmpty)(&graph));

  TEST_ASSERT(GRAPH_OPERATION(int, int, AddVertex)(&graph, 10, &a) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(GRAPH_OPERATION(int, int, AddVertex)(&graph, 20, &b) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(GRAPH_OPERATION(int, int, AddVertex)(&graph, 30, &c) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(GRAPH_OPERATION(int, int, AddVertex)(&graph, 40, &d) ==
              STATUS_CONST(SUCCESS));

  TEST_ASSERT(a == 0u && b == 1u && c == 2u && d == 3u);
  TEST_ASSERT(GRAPH_OPERATION(int, int, VertexCount)(&graph) == 4u);

  TEST_ASSERT(GRAPH_OPERATION(int, int, AddEdge)(&graph, a, b, 5) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(GRAPH_OPERATION(int, int, AddEdge)(&graph, a, c, 7) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(GRAPH_OPERATION(int, int, AddEdge)(&graph, b, d, 9) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(GRAPH_OPERATION(int, int, AddEdge)(&graph, c, d, 11) ==
              STATUS_CONST(SUCCESS));

  TEST_ASSERT(GRAPH_OPERATION(int, int, EdgeCount)(&graph) == 4u);
  TEST_ASSERT(GRAPH_OPERATION(int, int, HasEdge)(&graph, a, c));

  weight = NULL;
  TEST_ASSERT(GRAPH_OPERATION(int, int, FindWeight)(
                  &graph, a, c, &weight) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(weight && *weight == 7);
  *weight = 8;
  constWeight = NULL;
  TEST_ASSERT(GRAPH_OPERATION(int, int, ConstFindWeight)(
                  &graph, a, c, &constWeight) == STATUS_CONST(SUCCESS));
  TEST_ASSERT(constWeight && *constWeight == 8);

  TEST_ASSERT(GRAPH_OPERATION(int, int, BFS)(&graph, a, VisitIntGraph, &bfs) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(bfs.count == 4u);
  TEST_ASSERT(bfs.values[0] == a);
  TEST_ASSERT(bfs.values[1] == b);
  TEST_ASSERT(bfs.values[2] == c);
  TEST_ASSERT(bfs.values[3] == d);

  TEST_ASSERT(GRAPH_OPERATION(int, int, DFS)(&graph, a, VisitIntGraph, &dfs) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(dfs.count == 4u);
  TEST_ASSERT(dfs.values[0] == a);

  TEST_ASSERT(GRAPH_OPERATION(int, int, RemoveEdge)(&graph, a, c) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(!GRAPH_OPERATION(int, int, HasEdge)(&graph, a, c));
  weight = NULL;
  TEST_ASSERT(GRAPH_OPERATION(int, int, FindWeight)(
                  &graph, a, c, &weight) == STATUS_CONST(NOT_FOUND));
  TEST_ASSERT(weight == NULL);
  TEST_ASSERT(GRAPH_OPERATION(int, int, FindWeight)(
                  &graph, a, c, NULL) == STATUS_CONST(INVALID_ARGUMENT));
  TEST_ASSERT(GRAPH_OPERATION(int, int, RemoveEdge)(&graph, a, c) ==
              STATUS_CONST(NOT_FOUND));

  TEST_ASSERT(GRAPH_OPERATION(int, int, RemoveVertex)(&graph, b) ==
              STATUS_CONST(SUCCESS));
  TEST_ASSERT(GRAPH_OPERATION(int, int, VertexCount)(&graph) == 3u);
  TEST_ASSERT(GRAPH_OPERATION(int, int, EdgeCount)(&graph) == 1u);
  TEST_ASSERT(GRAPH_OPERATION(int, int, HasEdge)(&graph, 1u, 2u));

#if CONTAINER_FUNCTION_TABLE_ENABLED
  TEST_ASSERT(graph.api != NULL);
  TEST_ASSERT(graph.api->vertexCount(&graph) == 3u);
#endif

  GRAPH_OPERATION(int, int, Clear)(&graph);
  TEST_ASSERT(GRAPH_OPERATION(int, int, IsEmpty)(&graph));
  TEST_ASSERT(graph.vertexCapacity > 0u);

  GRAPH_OPERATION(int, int, Destroy)(&graph);
  TEST_ASSERT(graph.vertices == NULL);
  TEST_ASSERT(graph.vertexCapacity == 0u);

  return 0;
}
