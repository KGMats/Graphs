#ifndef SIGNED_ALGORITHMS_H
#define SIGNED_ALGORITHMS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "signed/SignedGraph.h"

#ifndef PROPAGATION_RESULT_DEFINED
#define PROPAGATION_RESULT_DEFINED
typedef struct {
    uint64_t count;
    uint64_t *activated_nodes;
} PropagationResult;
#endif

bool activationFunction(SignedNode *node);

PropagationResult propagate(const SignedGraph *graph, uint64_t t);

PropagationResult partialPropagate(const SignedGraph *graph, uint64_t n_changed,
                                   const uint64_t *changed_nodes, uint64_t t);

PropagationResult partialReversePropagate(const SignedGraph *graph, uint64_t n_changed,
                                          const uint64_t *changed_nodes, uint64_t t);

bool runTest(const SignedGraph *graph, const uint64_t *activeNodeIDs, uint64_t n_ids, uint64_t t);

void testHeuristics(const SignedGraph *graph,
                    bool heuristicFunction(const SignedGraph *, uint64_t *, uint64_t, uint64_t),
                    uint64_t t);

#endif //SIGNED_ALGORITHMS_H
