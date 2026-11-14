#ifndef COSMERON_CONTAINER_GRAPH_TGRAPH_H
#define COSMERON_CONTAINER_GRAPH_TGRAPH_H

#include "Common.inc"
#include "Graph.def"

/* Pre-instantiated common Graph types */
TEMPLATE_INSTANTIATE(Graph, Graph, Int32_Int32, int32_t, int32_t);
TEMPLATE_INSTANTIATE(Graph, Graph, Int32_Float64, int32_t, double);
TEMPLATE_INSTANTIATE(Graph, Graph, Size_Float64, size_t, double);

#endif /* COSMERON_CONTAINER_GRAPH_TGRAPH_H */

