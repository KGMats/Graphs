//
// Created by Elon Pereira Neto on 24/10/25.
//

#include "Utils.h"
#include "unsigned/algorithms.h"
#include "unsigned/heuristics/deterministic/maximumDegreeHeuristic.h"

// Heuristica do número de Arestas
bool maximumDegreeHeuristic(const Graph *graph, uint64_t *result, const uint64_t initialActiveNodes) {
    tuple *list = malloc(sizeof(tuple) * graph->n_nodes);
    for (int i = 0; i < graph->n_nodes; i++) {
        list[i].ID = i;
        list[i].priority = graph->nodes[i]->n_neighbors;
    }

    mergeSort(list, 0, graph->n_nodes - 1);

    for (int i = graph->n_nodes; i != 0; i--) {
        result[i] = list[i].ID;
    }

    free(list);
    return runTest(graph, result, initialActiveNodes);

}
