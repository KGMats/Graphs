//
// Created by Elon Pereira Neto on 24/10/25.
//

#include "Utils.h"
#include "Algorithms.h"
#include "MaximumDegreeHeuristic.h"

// Heuristica do número de Arestas
bool maximumDegreeHeuristic(const Graph *graph, uint64_t *result, const uint64_t initialActiveNodes) {
    tuple *list = orderedList(graph);

    for (int i = 0; i < initialActiveNodes; i++) {
        result[i] = list[graph->n_nodes - i - 1].ID;
        // result[i] = list[i].ID;
    }

    free(list);
    return runTest(graph, result, initialActiveNodes);

}
