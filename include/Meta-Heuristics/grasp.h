//
// Created by ra173048 on 10/24/25.
//

#ifndef GRAFOS_GRASP_H
#define GRAFOS_GRASP_H
#include "Graph.h"

uint64_t GRASP(Graph *graph, const unsigned int max_iterations,
               const int seed, const bool randomGreedyFunction(const Graph *, uint64_t *result, const uint64_t),
               const uint64_t localSearchFunction(const Graph *graph, uint64_t *bestSolution, uint64_t nActiveNodes));

#endif //GRAFOS_GRASP_H