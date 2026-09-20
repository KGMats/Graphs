//
// Created by Elon Pereira Neto on 24/10/25.
//

#include <stdlib.h>

#include "Utils.h"
#include "signed/algorithms.h"
#include "signed/heuristics/deterministic/tarjan.h"
#include "signed/heuristics/deterministic/bridgeHeuristic.h"

// Heuristica das pontes.
bool bridgeHeuristicSigned(const SignedGraph *graph, uint64_t *result, const uint64_t initialActiveNodes, uint64_t steps) {
    uint64_t counter = 0;

    uint64_t *changedNodes = malloc(sizeof(uint64_t) * graph->n_nodes);
    uint64_t n_changed = 0;

    // Encontrando os candidatos a nos "Ponto de articulação"
    uint64_t *tarjanSolutions = tarjanSigned(graph);


    // Ordenando os pontos de articulacao pelo saldo de vertices postivos - negativos
    // E completando o restante do conjunto solucao com os vertices de maior saldo
    const uint64_t n_nodes = graph->n_nodes;
    uint64_t n_ids = 0;
    tuple *list = calloc(n_nodes,sizeof(tuple));
    for (uint64_t i = 0; tarjanSolutions[i] != UINT64_MAX; i++) {
        list[i].ID = tarjanSolutions[i];

        SignedNode *node = graph->nodes[list[i].ID];

        list[i].priority = (node->positive_edges - (node->degree - node->positive_edges));
        n_ids++;
    }

    uint64_t id_search = 0;
    for (uint64_t i = 0; i < n_nodes; i++) {
        bool isInList = false;
        for (uint64_t j = 0; j < n_ids; j++) if (i == tarjanSolutions[j]) isInList = true;

        if (!isInList) {
            list[id_search + n_ids].ID = i;
            list[id_search + n_ids].priority = graph->nodes[i]->degree;
            id_search++;
        }
    }

    mergeSort(list, 0, n_ids - 1);
    mergeSort(list, n_ids, n_nodes - 1);


    n_ids = 0;
    for (uint64_t i = 0; tarjanSolutions[i] != UINT64_MAX; i++) n_ids++;

    for (uint64_t i = n_ids - 1; i > 0 && counter < initialActiveNodes; i--) {
        const uint64_t nodeID = list[i].ID;

        if (getSignedNodeState(graph->active_nodes, nodeID)) continue;

        result[counter++] = nodeID;
        changedNodes[n_changed++] = nodeID;
    }

    // Completa o restante dos nos caso o initialActive nodes > n de nos selecionados.
    uint64_t i = graph->n_nodes;
    while (i > n_ids && counter < initialActiveNodes) {
        i--;
        const uint64_t nodeID = list[i].ID;

        if (getSignedNodeState(graph->active_nodes, nodeID)) continue;

        result[counter++] = nodeID;
        changedNodes[n_changed++] = nodeID;

    }

    free(changedNodes);
    free(list);
    free(tarjanSolutions);

    return signedRunTest(graph, result, initialActiveNodes, steps);
}
