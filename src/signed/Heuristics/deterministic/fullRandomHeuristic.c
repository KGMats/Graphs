#include <stdlib.h>
#include "signed/algorithms.h"
#include "signed/heuristics/deterministic/fullRandomHeuristic.h"


bool fullRandomHeuristicSigned(const SignedGraph *graph, uint64_t *result, uint64_t initialActiveNodes, uint64_t steps)
{
    for (uint64_t i = 0; i < initialActiveNodes; i++) {
        result[i] = rand() % graph->n_nodes;
    }

    const PropagationResult propagationResult = signedPartialPropagate(graph, initialActiveNodes, result, steps);
    free(propagationResult.activated_nodes);
    // TODO: Pensar nos valores de retorno nas funcoes de propagacao. Usar número de vértices positivamente influenciados?
    return 1;
}
