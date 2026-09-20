#ifndef SIGNED_FARTHEST_FROM_ACTIVATION_HEURISTIC_H
#define SIGNED_FARTHEST_FROM_ACTIVATION_HEURISTIC_H
#include <stdint.h>
#include <stdbool.h>
#include "signed/signedGraph.h"

bool farthestFromActivationHeuristicSigned(const SignedGraph *graph, uint64_t *result, uint64_t initialActiveNodes, uint64_t steps);

#endif