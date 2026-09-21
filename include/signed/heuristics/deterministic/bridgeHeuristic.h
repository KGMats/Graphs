//
// Created by Elon Pereira Neto on 24/10/25.
//

#ifndef SIGNED_BRIDGEHEURISTIC_H
#define SIGNED_BRIDGEHEURISTIC_H

#include <stdbool.h>
#include <stdint.h>

#include "signed/signedGraph.h"

uint64_t bridgeHeuristicSigned(const SignedGraph *graph, uint64_t *result, uint64_t initialActiveNodes, uint64_t steps);

#endif