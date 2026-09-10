#ifndef SEMNOME_ALGORITHMS_H
#define SEMNOME_ALGORITHMS_H

#include <stdlib.h>

#include "Graph.h"

typedef struct {
    uint64_t count;
    uint64_t *activated_nodes;
} PropagationResult;

PropagationResult propagate(const Graph *graph);

PropagationResult partialPropagate(const Graph *graph, uint64_t n_changed, const uint64_t *changed_nodes);

PropagationResult partialReversePropagate(const Graph *graph, uint64_t n_changed, const uint64_t *changed_nodes);

PropagationResult partialReversePropagateBlackListed(const Graph *graph, uint64_t n_changed,
                                                     const uint64_t *changed_nodes, const uint64_t *black_list,
                                                     uint64_t black_list_size);

bool runTest(const Graph *graph, const uint64_t *activeNodeIDs, uint64_t n_ids);

void testHeuristics(const Graph *graph, bool heuristicFunction(const Graph *, uint64_t *, uint64_t));

void testLocalSearch(const Graph *graph, bool heuristicFunction(const Graph *, uint64_t *, uint64_t),
                     uint64_t localSearchFunction(const Graph *, uint64_t *, uint64_t));


#endif //SEMNOME_ALGORITHMS_H