#include "../../../Cosmeron/Modules/Container/Graph/Graph.h"
#include <assert.h>
#include <stdio.h>

typedef struct NS(GraphTest, VisitLog) {
  int32_t order[8];
  size_t count;
} NS(GraphTest, VisitLog);

INTERNAL_DEF(
    GraphTest,
    PROCEDURE(RecordVisit, (size_t vertexIndex, int32_t *data, void *context), {
      (void)vertexIndex;
      NS(GraphTest, VisitLog) *log = (NS(GraphTest, VisitLog) *)context;
      if (log && data && log->count < 8U) {
        log->order[log->count++] = *data;
      }
    }))

static void NS(GraphTest, BasicAndTraversals)(void) {
  MODULE_TYPED(Graph, Graph, Int32_Int32) g;
  assert(MODULE_FUNC(Graph, Graph, Int32_Int32, Create)(&g) ==
         STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&g, IsEmpty));

  size_t v0 = 0U, v1 = 0U, v2 = 0U, v3 = 0U;
  assert(OBJECT_CALL(&g, AddVertex, 100, &v0) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&g, AddVertex, 200, &v1) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&g, AddVertex, 300, &v2) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&g, AddVertex, 400, &v3) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&g, VertexCount) == 4U);

  assert(OBJECT_CALL(&g, AddEdge, v0, v1, 10) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&g, AddEdge, v0, v2, 20) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&g, AddEdge, v1, v3, 30) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&g, AddEdge, v2, v3, 40) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&g, EdgeCount) == 4U);
  assert(OBJECT_CALL(&g, HasEdge, v0, v2));
  assert(OBJECT_CALL(&g, OutDegree, v0) == 2U);

  int32_t *w = NULL;
  assert(OBJECT_CALL(&g, FindWeight, v0, v2, &w) == STATUS_CONST(SUCCESS));
  assert(*w == 20);

  /* Optional Vertex & Edge Iterators */
  MODULE_TYPED(Graph, GraphVertexIter, Int32_Int32) vit;
  MODULE_TYPED(Graph, GraphVertexIter, Int32_Int32) vend;
  assert(OBJECT_CALL(&g, BeginVertices, &vit) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&g, EndVertices, &vend) == STATUS_CONST(SUCCESS));
  int32_t vsum = 0;
  while (!OBJECT_CALL(&vit, Equals, &vend)) {
    int32_t *vdata = NULL;
    assert(OBJECT_CALL(&vit, Next, &vdata) == STATUS_CONST(SUCCESS));
    vsum += *vdata;
  }
  assert(vsum == 1000);

  MODULE_TYPED(Graph, GraphEdgeIter, Int32_Int32) eit;
  MODULE_TYPED(Graph, GraphEdgeIter, Int32_Int32) eend;
  assert(OBJECT_CALL(&g, BeginEdges, v0, &eit) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL(&g, EndEdges, v0, &eend) == STATUS_CONST(SUCCESS));
  int32_t esum = 0;
  while (!OBJECT_CALL(&eit, Equals, &eend)) {
    MODULE_TYPED(Graph, GraphEdge, Int32_Int32) *e = NULL;
    assert(OBJECT_CALL(&eit, Next, &e) == STATUS_CONST(SUCCESS));
    esum += e->weight;
  }
  assert(esum == 30);

  /* BFS & DFS */
  NS(GraphTest, VisitLog) bfsLog = {{0}, 0U};
  MODULE_CLOSURE(Graph, Graph, Int32_Int32, Visitor) bfsVisitor =
      NS_JOIN(MODULE_CLOSURE_FUNC(Graph, Graph, Int32_Int32, Visitor), Bind)(
          MODULE_IFUNC(GraphTest, RecordVisit), &bfsLog);
  assert(OBJECT_CALL(&g, BFS, v0, bfsVisitor) == STATUS_CONST(SUCCESS));
  assert(bfsLog.count == 4U);
  assert(bfsLog.order[0] == 100 && bfsLog.order[1] == 200 &&
         bfsLog.order[2] == 300 && bfsLog.order[3] == 400);

  NS(GraphTest, VisitLog) dfsLog = {{0}, 0U};
  MODULE_CLOSURE(Graph, Graph, Int32_Int32, Visitor) dfsVisitor =
      NS_JOIN(MODULE_CLOSURE_FUNC(Graph, Graph, Int32_Int32, Visitor), Bind)(
          MODULE_IFUNC(GraphTest, RecordVisit), &dfsLog);
  assert(OBJECT_CALL(&g, DFS, v0, dfsVisitor) == STATUS_CONST(SUCCESS));
  assert(dfsLog.count == 4U);
  assert(dfsLog.order[0] == 100 && dfsLog.order[1] == 200 &&
         dfsLog.order[2] == 400 && dfsLog.order[3] == 300);

  /* RemoveEdge & RemoveVertex */
  assert(OBJECT_CALL(&g, RemoveEdge, v0, v1) == STATUS_CONST(SUCCESS));
  assert(!OBJECT_CALL(&g, HasEdge, v0, v1));
  assert(OBJECT_CALL_0(&g, EdgeCount) == 3U);

  assert(OBJECT_CALL(&g, RemoveVertex, v1) == STATUS_CONST(SUCCESS));
  assert(OBJECT_CALL_0(&g, VertexCount) == 3U);
  assert(OBJECT_CALL_0(&g, EdgeCount) == 2U);

  OBJECT_CALL_0(&g, Destroy);
}

int main(void) {
  NS(GraphTest, BasicAndTraversals)();
  printf("Graph tests passed!\n");
  return 0;
}

