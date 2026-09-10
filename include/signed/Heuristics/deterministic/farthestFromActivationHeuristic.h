#ifndef SIGNED_FARTHEST_FROM_ACTIVATION_HEURISTIC_H
#define SIGNED_FARTHEST_FROM_ACTIVATION_HEURISTIC_H
#include "signed/SignedGraph.h"

bool farthestFromActivationHeuristic(const SignedGraph *graph, uint64_t *result, uint64_t initialActiveNodes);

#endif