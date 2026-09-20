//
// Created by Elon Pereira Neto on 24/10/25.
//

#ifndef SIGNED_BRIDGEHEURISTIC_H
#define SIGNED_BRIDGEHEURISTIC_H

#include <stdbool.h>
#include <stdint.h>

#include "signed/SignedGraph.h"

bool bridgeHeuristic(const SignedGraph *graph, uint64_t *result, uint64_t initialActiveNodes);

#endif