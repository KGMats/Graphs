//
// Created by Elon Pereira Neto on 24/10/25.
//

#ifndef GRAFOS_TARJAN_H
#define GRAFOS_TARJAN_H

#include <stdint.h>
#include <stdbool.h>

#include "../include/Graph.h"

uint64_t* tarjan(const Graph* graph);
void dfs_AP(Node *startNode, char *visited, uint64_t disc[], uint64_t low[], uint64_t parent[], char *isAP, uint64_t *time);


#endif //GRAFOS_TARJAN_H