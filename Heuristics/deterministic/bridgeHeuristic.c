//
// Created by Elon Pereira Neto on 24/10/25.
//

#include <stdlib.h>

#include "Utils.h"
#include "Algorithms.h"
#include "Tarjan.h"
#include "bridgeHeuristic.h"

// Heuristica das pontes.
bool bridgeHeuristic(const Graph *graph, uint64_t *result, const uint64_t initialActiveNodes) {
    uint64_t counter = 0;

    uint64_t *changedNodes = malloc(sizeof(uint64_t) * graph->n_nodes);
    uint64_t n_changed = 0;

    // Encontrando os candidatos a nos "Ponto de articulação"
    uint64_t *tarjanSolutions = tarjan(graph);
    tuple *list = orderedSetList(graph, tarjanSolutions);
    uint64_t n_ids = 0;
    for (uint64_t i = 0; tarjanSolutions[i] != UINT64_MAX; i++) n_ids++;

    for (uint64_t i = n_ids - 1; i > 0 && counter < initialActiveNodes; i--) {
        const uint64_t nodeID = list[i].ID;

        if (getNodeState(graph->active_nodes, nodeID)) continue;

        result[counter++] = nodeID;
        changedNodes[n_changed++] = nodeID;
    }

    // Completa o restante dos nos caso o initialActive nodes > n de nos selecionados.
    uint64_t i = graph->n_nodes;
    while (i > n_ids && counter < initialActiveNodes) {
        i--;
        const uint64_t nodeID = list[i].ID;

        if (getNodeState(graph->active_nodes, nodeID)) continue;

        result[counter++] = nodeID;
        changedNodes[n_changed++] = nodeID;

    }

    free(changedNodes);
    free(list);
    free(tarjanSolutions);

    return runTest(graph, result, initialActiveNodes);
}