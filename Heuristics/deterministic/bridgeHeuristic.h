//
// Created by Elon Pereira Neto on 24/10/25.
//

#ifndef GRAFOS_BRIDGEHEURISTIC_H
#define GRAFOS_BRIDGEHEURISTIC_H

#include <stdbool.h>
#include <stdint.h>

#include "../../Graph.h"

bool bridgeHeuristic(const Graph *graph, uint64_t *result, uint64_t initialActiveNodes);

#endif //GRAFOS_BRIDGEHEURISTIC_H