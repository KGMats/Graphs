//
// Created by Elon Pereira Neto on 24/10/25.
//

#ifndef GRAFOS_HILLCLIMBLOCALSEARCH_H
#define GRAFOS_HILLCLIMBLOCALSEARCH_H

#include <stdint.h>

#include "../../Graph.h"

uint64_t hillClimbSimple(const Graph *graph, uint64_t *bestSolution, uint64_t nActiveNodes);
#endif //GRAFOS_HILLCLIMBLOCALSEARCH_H