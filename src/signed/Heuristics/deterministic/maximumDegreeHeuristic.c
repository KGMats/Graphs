#include "Utils.h"
#include "signed/algorithms.h"
#include "signed/heuristics/deterministic/maximumDegreeHeuristic.h"

// Heuristica do Maior Saldo Positivo de Arestas
uint64_t maximumDegreeHeuristicSigned(const SignedGraph *graph, uint64_t *result, const uint64_t initialActiveNodes, const uint64_t t)
{
    tuple *list = malloc(sizeof(tuple) * graph->n_nodes);
    for (int i = 0; i < graph->n_nodes; i++)
    {
        SignedNode *node = graph->nodes[i];
        list[i].ID = i;
        list[i].priority = signedNodeBalance(node);
    }

    mergeSort(list, 0, graph->n_nodes - 1);

    for (uint64_t i = 0; i < initialActiveNodes; i++)
    {
        result[i] = list[graph->n_nodes - 1 - i].ID;
    }

    free(list);
    return initialActiveNodes;
}
