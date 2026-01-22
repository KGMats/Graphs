//
// Created by ra173048 on 10/27/25.
//

#include "../../../include/Heuristics/deterministic/fullRandomHeuristic.h"

#include <stdlib.h>

#include "Algorithms.h"

bool fullRandomHeuristic(const Graph *graph, uint64_t *result, uint64_t initialActiveNodes) {
    for (uint64_t i = 0; i < initialActiveNodes; i++) {
        result[i] = rand() % graph->n_nodes;
    }

    PropagationResult propagationResult = partialPropagate(graph, initialActiveNodes, result);
    free(propagationResult.activated_nodes);
    return (graph->n_nodes == countActiveNodes(graph));
}