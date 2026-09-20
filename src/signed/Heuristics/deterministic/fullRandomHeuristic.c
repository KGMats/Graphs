#include <stdlib.h>
#include "signed/Heuristics/deterministic/fullRandomHeuristic.h"


bool fullRandomHeuristic(const SignedGraph *graph, uint64_t *result, uint64_t initialActiveNodes)
{
    for (uint64_t i = 0; i < initialActiveNodes; i++) {
        result[i] = rand() % graph->n_nodes;
    }

    PropagationResult propagationResult = partialPropagate(graph, initialActiveNodes, result);
    free(propagationResult.activated_nodes);
    return (graph->n_nodes == countActiveNodes(graph));
}
