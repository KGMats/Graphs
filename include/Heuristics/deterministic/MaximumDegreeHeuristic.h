//
// Created by Elon Pereira Neto on 24/10/25.
//

#ifndef GRAFOS_MAXIMUMDEGREEHEURISTIC_H
#define GRAFOS_MAXIMUMDEGREEHEURISTIC_H

#include <stdbool.h>
#include <stdint.h>

#include "../../include/Graph.h"

bool maximumDegreeHeuristic(const Graph *graph, uint64_t *result, uint64_t initialActiveNodes);

#endif //GRAFOS_MAXIMUMDEGREEHEURISTIC_H